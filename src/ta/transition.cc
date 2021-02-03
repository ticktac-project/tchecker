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

#include "tchecker/ta/transition.hh"

namespace tchecker {

namespace ta {

/* transition_t */

tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> transition_t::src_invariant() const
{
  return tchecker::make_range(_src_invariant.begin(), _src_invariant.end());
}

tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> transition_t::guard() const
{
  return tchecker::make_range(_guard.begin(), _guard.end());
}

tchecker::range_t<tchecker::clock_reset_container_const_iterator_t> transition_t::reset() const
{
  return tchecker::make_range(_reset.begin(), _reset.end());
}

tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> transition_t::tgt_invariant() const
{
  return tchecker::make_range(_tgt_invariant.begin(), _tgt_invariant.end());
}

bool operator==(tchecker::ta::transition_t const & t1, tchecker::ta::transition_t const & t2)
{
  return (tchecker::syncprod::operator==(t1, t2) && t1.src_invariant_container() == t2.src_invariant_container() &&
          t1.guard_container() == t2.guard_container() && t1.reset_container() == t2.reset_container() &&
          t1.tgt_invariant_container() == t2.tgt_invariant_container());
}

std::size_t hash_value(tchecker::ta::transition_t const & t)
{
  std::size_t h = tchecker::syncprod::hash_value(t);
  boost::hash_combine(h, t.src_invariant_container());
  boost::hash_combine(h, t.guard_container());
  boost::hash_combine(h, t.reset_container());
  boost::hash_combine(h, t.tgt_invariant_container());
  return h;
}

int lexical_cmp(tchecker::ta::transition_t const & t1, tchecker::ta::transition_t const & t2)
{
  int syncprod_cmp = tchecker::syncprod::lexical_cmp(t1, t2);
  if (syncprod_cmp != 0)
    return syncprod_cmp;
  int src_invariant_cmp = tchecker::lexical_cmp(t1.src_invariant_container(), t2.src_invariant_container());
  if (src_invariant_cmp != 0)
    return src_invariant_cmp;
  int guard_cmp = tchecker::lexical_cmp(t1.guard_container(), t2.guard_container());
  if (guard_cmp != 0)
    return guard_cmp;
  int reset_cmp = tchecker::lexical_cmp(t1.reset_container(), t2.reset_container());
  if (reset_cmp != 0)
    return reset_cmp;
  return tchecker::lexical_cmp(t1.tgt_invariant_container(), t2.tgt_invariant_container());
}

} // end of namespace ta

} // end of namespace tchecker
