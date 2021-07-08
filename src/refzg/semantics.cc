/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <boost/dynamic_bitset.hpp>

#include "tchecker/dbm/refdbm.hh"
#include "tchecker/refzg/semantics.hh"

namespace tchecker {

namespace refzg {

/* standard_semantics_t */

tchecker::state_status_t standard_semantics_t::initial(tchecker::dbm::db_t * rdbm,
                                                       tchecker::reference_clock_variables_t const & r,
                                                       boost::dynamic_bitset<> const & delay_allowed,
                                                       tchecker::clock_constraint_container_t const & invariant)
{
  tchecker::refdbm::zero(rdbm, r);

  if (tchecker::refdbm::constrain(rdbm, r, invariant) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

  return tchecker::STATE_OK;
}

tchecker::state_status_t standard_semantics_t::next(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                                    boost::dynamic_bitset<> const & src_delay_allowed,
                                                    tchecker::clock_constraint_container_t const & src_invariant,
                                                    boost::dynamic_bitset<> const & sync_ref_clocks,
                                                    tchecker::clock_constraint_container_t const & guard,
                                                    tchecker::clock_reset_container_t const & clkreset,
                                                    boost::dynamic_bitset<> const & tgt_delay_allowed,
                                                    tchecker::clock_constraint_container_t const & tgt_invariant)
{
  if (src_delay_allowed.any()) {
    tchecker::refdbm::asynchronous_open_up(rdbm, r, src_delay_allowed);

    if (tchecker::refdbm::constrain(rdbm, r, src_invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED; // should never occur
  }

  if (tchecker::refdbm::synchronize(rdbm, r, sync_ref_clocks) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_EMPTY_SYNC;

  if (tchecker::refdbm::constrain(rdbm, r, guard) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_GUARD_VIOLATED;

  tchecker::refdbm::reset(rdbm, r, clkreset);

  if (tchecker::refdbm::constrain(rdbm, r, tgt_invariant) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;

  return tchecker::STATE_OK;
}

/* elapsed_semantics_t */

tchecker::state_status_t elapsed_semantics_t::initial(tchecker::dbm::db_t * rdbm,
                                                      tchecker::reference_clock_variables_t const & r,
                                                      boost::dynamic_bitset<> const & delay_allowed,
                                                      tchecker::clock_constraint_container_t const & invariant)
{
  tchecker::refdbm::zero(rdbm, r);

  if (tchecker::refdbm::constrain(rdbm, r, invariant) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

  if (delay_allowed.any()) {
    tchecker::refdbm::asynchronous_open_up(rdbm, r, delay_allowed);

    if (tchecker::refdbm::constrain(rdbm, r, invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;
  }

  return tchecker::STATE_OK;
}

tchecker::state_status_t elapsed_semantics_t::next(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                                   boost::dynamic_bitset<> const & src_delay_allowed,
                                                   tchecker::clock_constraint_container_t const & src_invariant,
                                                   boost::dynamic_bitset<> const & sync_ref_clocks,
                                                   tchecker::clock_constraint_container_t const & guard,
                                                   tchecker::clock_reset_container_t const & clkreset,
                                                   boost::dynamic_bitset<> const & tgt_delay_allowed,
                                                   tchecker::clock_constraint_container_t const & tgt_invariant)
{
  if (tchecker::refdbm::constrain(rdbm, r, src_invariant) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

  if (tchecker::refdbm::synchronize(rdbm, r, sync_ref_clocks) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_EMPTY_SYNC;

  if (tchecker::refdbm::constrain(rdbm, r, guard) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_GUARD_VIOLATED;

  tchecker::refdbm::reset(rdbm, r, clkreset);

  if (tchecker::refdbm::constrain(rdbm, r, tgt_invariant) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;

  if (tgt_delay_allowed.any()) {
    tchecker::refdbm::asynchronous_open_up(rdbm, r, tgt_delay_allowed);

    if (tchecker::refdbm::constrain(rdbm, r, tgt_invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;
  }

  return tchecker::STATE_OK;
}

/* factory */

tchecker::refzg::semantics_t * semantics_factory(enum tchecker::refzg::semantics_type_t semantics)
{
  switch (semantics) {
  case tchecker::refzg::STANDARD_SEMANTICS:
    return new tchecker::refzg::standard_semantics_t{};
  case tchecker::refzg::ELAPSED_SEMANTICS:
    return new tchecker::refzg::elapsed_semantics_t{};
  default:
    throw std::invalid_argument("Unknown semantics over zones with reference clocks");
  }
}

} // end of namespace refzg

} // end of namespace tchecker