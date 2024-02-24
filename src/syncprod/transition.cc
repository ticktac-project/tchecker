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

#include "tchecker/syncprod/transition.hh"

namespace tchecker {

namespace syncprod {

/* transition_t */

transition_t::transition_t(tchecker::sync_id_t sync_id,
                           tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge)
    : _sync_id(sync_id), _vedge(vedge)
{
  assert((_sync_id != tchecker::NO_SYNC) || tchecker::is_asynchronous(*_vedge));
  assert(_vedge.ptr() != nullptr);
}

transition_t::transition_t(tchecker::syncprod::transition_t const & t, tchecker::sync_id_t sync_id,
                           tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge)
    : tchecker::ts::transition_t(t), _sync_id(sync_id), _vedge(vedge)
{
  assert((_sync_id != tchecker::NO_SYNC) || tchecker::is_asynchronous(*_vedge));
  assert(_vedge.ptr() != nullptr);
}

bool operator==(tchecker::syncprod::transition_t const & t1, tchecker::syncprod::transition_t const & t2)
{
  return (static_cast<tchecker::ts::transition_t const &>(t1) == static_cast<tchecker::ts::transition_t const &>(t2) &&
          (t1.sync_id() == t2.sync_id()) && (t1.vedge() == t2.vedge()));
}

bool operator!=(tchecker::syncprod::transition_t const & t1, tchecker::syncprod::transition_t const & t2)
{
  return !(t1 == t2);
}

bool shared_equal_to(tchecker::syncprod::transition_t const & t1, tchecker::syncprod::transition_t const & t2)
{
  return (tchecker::ts::shared_equal_to(t1, t2) && (t1.sync_id() == t2.sync_id()) && (t1.vedge_ptr() == t2.vedge_ptr()));
}

std::size_t hash_value(tchecker::syncprod::transition_t const & t)
{
  std::size_t h = hash_value(static_cast<tchecker::ts::transition_t const &>(t));
  boost::hash_combine(h, t.sync_id());
  boost::hash_combine(h, t.vedge());
  return h;
}

std::size_t shared_hash_value(tchecker::syncprod::transition_t const & t)
{
  std::size_t h = tchecker::ts::shared_hash_value(t);
  boost::hash_combine(h, t.sync_id());
  boost::hash_combine(h, t.vedge_ptr());
  return h;
}

int lexical_cmp(tchecker::syncprod::transition_t const & t1, tchecker::syncprod::transition_t const & t2)
{
  int ts_cmp = tchecker::ts::lexical_cmp(t1, t2);
  if (ts_cmp != 0)
    return ts_cmp;
  if (t1.sync_id() < t2.sync_id())
    return -1;
  else if (t1.sync_id() > t2.sync_id())
    return 1;
  return tchecker::lexical_cmp(t1.vedge(), t2.vedge());
}

} // end of namespace syncprod

} // end of namespace tchecker
