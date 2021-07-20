/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/syncprod/state.hh"

namespace tchecker {

namespace syncprod {

/* state_t */

state_t::state_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc) : _vloc(vloc)
{
  assert(_vloc.ptr() != nullptr);
}

state_t::state_t(tchecker::syncprod::state_t const & s, tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc)
    : tchecker::ts::state_t(s), _vloc(vloc)
{
  assert(_vloc.ptr() != nullptr);
}

bool operator==(tchecker::syncprod::state_t const & s1, tchecker::syncprod::state_t const & s2)
{
  return (static_cast<tchecker::ts::state_t const &>(s1) == static_cast<tchecker::ts::state_t const &>(s2) &&
          s1.vloc() == s2.vloc());
}

bool operator!=(tchecker::syncprod::state_t const & s1, tchecker::syncprod::state_t const & s2) { return (!(s1 == s2)); }

std::size_t hash_value(tchecker::syncprod::state_t const & s)
{
  std::size_t h = hash_value(static_cast<tchecker::ts::state_t const &>(s));
  boost::hash_combine(h, s.vloc());
  return h;
}

int lexical_cmp(tchecker::syncprod::state_t const & s1, tchecker::syncprod::state_t const & s2)
{
  int ts_cmp = tchecker::ts::lexical_cmp(s1, s2);
  if (ts_cmp != 0)
    return ts_cmp;
  return tchecker::lexical_cmp(s1.vloc(), s2.vloc());
}

} // end of namespace syncprod

} // end of namespace tchecker
