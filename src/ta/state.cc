/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/ta/state.hh"

namespace tchecker {

namespace ta {

state_t::state_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval)
    : tchecker::syncprod::state_t(vloc), _intval(intval)
{
  assert(_intval.ptr() != nullptr);
}

state_t::state_t(tchecker::ta::state_t const & s, tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval)
    : tchecker::syncprod::state_t(s, vloc), _intval(intval)
{
  assert(_intval.ptr() != nullptr);
}

bool operator==(tchecker::ta::state_t const & s1, tchecker::ta::state_t const & s2)
{
  return tchecker::syncprod::operator==(s1, s2) && (s1.intval() == s2.intval());
}

bool operator!=(tchecker::ta::state_t const & s1, tchecker::ta::state_t const & s2) { return !(s1 == s2); }

std::size_t hash_value(tchecker::ta::state_t const & s)
{
  std::size_t h = tchecker::syncprod::hash_value(s);
  boost::hash_combine(h, s.intval());
  return h;
}

int lexical_cmp(tchecker::ta::state_t const & s1, tchecker::ta::state_t const & s2)
{
  int syncprod_cmp = tchecker::syncprod::lexical_cmp(s1, s2);
  if (syncprod_cmp != 0)
    return syncprod_cmp;
  return tchecker::lexical_cmp(s1.intval(), s2.intval());
}

} // end of namespace ta

} // end of namespace tchecker
