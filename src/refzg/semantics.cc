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

/* Semantics functions */

/*!
  \brief Compute initial zone with reference clocks w.r.t. standard semantics
  \note see tchecker::refzg::standard_semantics_t::initial
*/
tchecker::state_status_t standard_initial(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                          boost::dynamic_bitset<> const & delay_allowed,
                                          tchecker::clock_constraint_container_t const & invariant, tchecker::integer_t spread)
{
  tchecker::refdbm::zero(rdbm, r);

  if (tchecker::refdbm::bound_spread(rdbm, r, spread) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_EMPTY_SPREAD;

  if (tchecker::refdbm::constrain(rdbm, r, invariant) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

  return tchecker::STATE_OK;
}

/*!
  \brief Compute next zone with reference clocks w.r.t. standard semantics
  \note see tchecker::refzg::standard_semantics_t::next
*/
tchecker::state_status_t
standard_next(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
              boost::dynamic_bitset<> const & src_delay_allowed, tchecker::clock_constraint_container_t const & src_invariant,
              boost::dynamic_bitset<> const & sync_ref_clocks, tchecker::clock_constraint_container_t const & guard,
              tchecker::clock_reset_container_t const & clkreset, boost::dynamic_bitset<> const & tgt_delay_allowed,
              tchecker::clock_constraint_container_t const & tgt_invariant, tchecker::integer_t spread)
{
  if (src_delay_allowed.any()) {
    tchecker::refdbm::asynchronous_open_up(rdbm, r, src_delay_allowed);

    if (tchecker::refdbm::constrain(rdbm, r, src_invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED; // should never occur
  }

  if (tchecker::refdbm::bound_spread(rdbm, r, spread) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_EMPTY_SPREAD;

  if (tchecker::refdbm::synchronize(rdbm, r, sync_ref_clocks) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_EMPTY_SYNC;

  if (tchecker::refdbm::constrain(rdbm, r, guard) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_GUARD_VIOLATED;

  tchecker::refdbm::reset(rdbm, r, clkreset);

  if (tchecker::refdbm::constrain(rdbm, r, tgt_invariant) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;

  return tchecker::STATE_OK;
}

/*!
  \brief Compute initial zone with reference clocks w.r.t. elapsed semantics
  \note see tchecker::refzg::elapsed_semantics_t::initial
*/
tchecker::state_status_t elapsed_initial(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                         boost::dynamic_bitset<> const & delay_allowed,
                                         tchecker::clock_constraint_container_t const & invariant, tchecker::integer_t spread)
{
  tchecker::refdbm::zero(rdbm, r);

  if (tchecker::refdbm::constrain(rdbm, r, invariant) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

  if (delay_allowed.any()) {
    tchecker::refdbm::asynchronous_open_up(rdbm, r, delay_allowed);

    if (tchecker::refdbm::constrain(rdbm, r, invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;
  }

  if (tchecker::refdbm::bound_spread(rdbm, r, spread) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_EMPTY_SPREAD;

  return tchecker::STATE_OK;
}

/*!
  \brief Compute next zone with reference clocks w.r.t. elapsed semantics
  \note see tchecker::refzg::elapsed_semantics_t::next
*/
tchecker::state_status_t
elapsed_next(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
             boost::dynamic_bitset<> const & src_delay_allowed, tchecker::clock_constraint_container_t const & src_invariant,
             boost::dynamic_bitset<> const & sync_ref_clocks, tchecker::clock_constraint_container_t const & guard,
             tchecker::clock_reset_container_t const & clkreset, boost::dynamic_bitset<> const & tgt_delay_allowed,
             tchecker::clock_constraint_container_t const & tgt_invariant, tchecker::integer_t spread)
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

  if (tchecker::refdbm::bound_spread(rdbm, r, spread) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_EMPTY_SPREAD;

  return tchecker::STATE_OK;
}

/* standard_semantics_t */

tchecker::state_status_t standard_semantics_t::initial(tchecker::dbm::db_t * rdbm,
                                                       tchecker::reference_clock_variables_t const & r,
                                                       boost::dynamic_bitset<> const & delay_allowed,
                                                       tchecker::clock_constraint_container_t const & invariant,
                                                       tchecker::integer_t spread)
{
  return tchecker::refzg::standard_initial(rdbm, r, delay_allowed, invariant, spread);
}

tchecker::state_status_t standard_semantics_t::next(
    tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
    boost::dynamic_bitset<> const & src_delay_allowed, tchecker::clock_constraint_container_t const & src_invariant,
    boost::dynamic_bitset<> const & sync_ref_clocks, tchecker::clock_constraint_container_t const & guard,
    tchecker::clock_reset_container_t const & clkreset, boost::dynamic_bitset<> const & tgt_delay_allowed,
    tchecker::clock_constraint_container_t const & tgt_invariant, tchecker::integer_t spread)
{
  return tchecker::refzg::standard_next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard, clkreset,
                                        tgt_delay_allowed, tgt_invariant, spread);
}

/* elapsed_semantics_t */

tchecker::state_status_t elapsed_semantics_t::initial(tchecker::dbm::db_t * rdbm,
                                                      tchecker::reference_clock_variables_t const & r,
                                                      boost::dynamic_bitset<> const & delay_allowed,
                                                      tchecker::clock_constraint_container_t const & invariant,
                                                      tchecker::integer_t spread)
{
  return tchecker::refzg::elapsed_initial(rdbm, r, delay_allowed, invariant, spread);
}

tchecker::state_status_t
elapsed_semantics_t::next(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                          boost::dynamic_bitset<> const & src_delay_allowed,
                          tchecker::clock_constraint_container_t const & src_invariant,
                          boost::dynamic_bitset<> const & sync_ref_clocks, tchecker::clock_constraint_container_t const & guard,
                          tchecker::clock_reset_container_t const & clkreset, boost::dynamic_bitset<> const & tgt_delay_allowed,
                          tchecker::clock_constraint_container_t const & tgt_invariant, tchecker::integer_t spread)
{
  return tchecker::refzg::elapsed_next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard, clkreset,
                                       tgt_delay_allowed, tgt_invariant, spread);
}

/* synchronizable_standard_semantics_t */

tchecker::state_status_t synchronizable_standard_semantics_t::initial(tchecker::dbm::db_t * rdbm,
                                                                      tchecker::reference_clock_variables_t const & r,
                                                                      boost::dynamic_bitset<> const & delay_allowed,
                                                                      tchecker::clock_constraint_container_t const & invariant,
                                                                      tchecker::integer_t spread)
{
  tchecker::state_status_t status = tchecker::refzg::standard_initial(rdbm, r, delay_allowed, invariant, spread);
  if (status != tchecker::STATE_OK)
    return status;

  if (!tchecker::refdbm::is_synchronizable(rdbm, r))
    return tchecker::STATE_ZONE_EMPTY_SYNC;

  return tchecker::STATE_OK;
}

tchecker::state_status_t synchronizable_standard_semantics_t::next(
    tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
    boost::dynamic_bitset<> const & src_delay_allowed, tchecker::clock_constraint_container_t const & src_invariant,
    boost::dynamic_bitset<> const & sync_ref_clocks, tchecker::clock_constraint_container_t const & guard,
    tchecker::clock_reset_container_t const & clkreset, boost::dynamic_bitset<> const & tgt_delay_allowed,
    tchecker::clock_constraint_container_t const & tgt_invariant, tchecker::integer_t spread)
{
  tchecker::state_status_t status = tchecker::refzg::standard_next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks,
                                                                   guard, clkreset, tgt_delay_allowed, tgt_invariant, spread);
  if (status != tchecker::STATE_OK)
    return status;

  if (!tchecker::refdbm::is_synchronizable(rdbm, r))
    return tchecker::STATE_ZONE_EMPTY_SYNC;

  return tchecker::STATE_OK;
}

/* synchronizable_elapsed_semantics_t */

tchecker::state_status_t synchronizable_elapsed_semantics_t::initial(tchecker::dbm::db_t * rdbm,
                                                                     tchecker::reference_clock_variables_t const & r,
                                                                     boost::dynamic_bitset<> const & delay_allowed,
                                                                     tchecker::clock_constraint_container_t const & invariant,
                                                                     tchecker::integer_t spread)
{
  tchecker::state_status_t status = tchecker::refzg::elapsed_initial(rdbm, r, delay_allowed, invariant, spread);
  if (status != tchecker::STATE_OK)
    return status;

  if (!tchecker::refdbm::is_synchronizable(rdbm, r))
    return tchecker::STATE_ZONE_EMPTY_SYNC;

  return tchecker::STATE_OK;
}

tchecker::state_status_t synchronizable_elapsed_semantics_t::next(
    tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
    boost::dynamic_bitset<> const & src_delay_allowed, tchecker::clock_constraint_container_t const & src_invariant,
    boost::dynamic_bitset<> const & sync_ref_clocks, tchecker::clock_constraint_container_t const & guard,
    tchecker::clock_reset_container_t const & clkreset, boost::dynamic_bitset<> const & tgt_delay_allowed,
    tchecker::clock_constraint_container_t const & tgt_invariant, tchecker::integer_t spread)
{
  tchecker::state_status_t status = tchecker::refzg::elapsed_next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks,
                                                                  guard, clkreset, tgt_delay_allowed, tgt_invariant, spread);
  if (status != tchecker::STATE_OK)
    return status;

  if (!tchecker::refdbm::is_synchronizable(rdbm, r))
    return tchecker::STATE_ZONE_EMPTY_SYNC;

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
  case tchecker::refzg::SYNC_STANDARD_SEMANTICS:
    return new tchecker::refzg::synchronizable_standard_semantics_t{};
  case tchecker::refzg::SYNC_ELAPSED_SEMANTICS:
    return new tchecker::refzg::synchronizable_elapsed_semantics_t{};
  default:
    throw std::invalid_argument("Unknown semantics over zones with reference clocks");
  }
}

} // end of namespace refzg

} // end of namespace tchecker