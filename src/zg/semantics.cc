/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/zg/semantics.hh"
#include "tchecker/dbm/dbm.hh"

namespace tchecker {

namespace zg {

/* standard_semantics_t */

enum tchecker::state_status_t standard_semantics_t::initial(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim,
                                                            bool delay_allowed,
                                                            tchecker::clock_constraint_container_t const & invariant)
{
  tchecker::dbm::zero(dbm, dim);

  if (tchecker::dbm::constrain(dbm, dim, invariant) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

  return tchecker::STATE_OK;
}

enum tchecker::state_status_t standard_semantics_t::next(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim,
                                                         bool src_delay_allowed,
                                                         tchecker::clock_constraint_container_t const & src_invariant,
                                                         tchecker::clock_constraint_container_t const & guard,
                                                         tchecker::clock_reset_container_t const & clkreset,
                                                         bool tgt_delay_allowed,
                                                         tchecker::clock_constraint_container_t const & tgt_invariant)
{
  if (src_delay_allowed) {
    tchecker::dbm::open_up(dbm, dim);

    if (tchecker::dbm::constrain(dbm, dim, src_invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED; // should never occur
  }

  if (tchecker::dbm::constrain(dbm, dim, guard) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_GUARD_VIOLATED;

  tchecker::dbm::reset(dbm, dim, clkreset);

  if (tchecker::dbm::constrain(dbm, dim, tgt_invariant) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;

  return tchecker::STATE_OK;
}

/* elapsed_semantics_t */

enum tchecker::state_status_t elapsed_semantics_t::initial(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim,
                                                           bool delay_allowed,
                                                           tchecker::clock_constraint_container_t const & invariant)
{
  tchecker::dbm::zero(dbm, dim);

  if (tchecker::dbm::constrain(dbm, dim, invariant) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

  if (delay_allowed) {
    tchecker::dbm::open_up(dbm, dim);

    if (tchecker::dbm::constrain(dbm, dim, invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;
  }

  return tchecker::STATE_OK;
}

enum tchecker::state_status_t elapsed_semantics_t::next(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim,
                                                        bool src_delay_allowed,
                                                        tchecker::clock_constraint_container_t const & src_invariant,
                                                        tchecker::clock_constraint_container_t const & guard,
                                                        tchecker::clock_reset_container_t const & clkreset,
                                                        bool tgt_delay_allowed,
                                                        tchecker::clock_constraint_container_t const & tgt_invariant)
{
  if (tchecker::dbm::constrain(dbm, dim, src_invariant) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

  if (tchecker::dbm::constrain(dbm, dim, guard) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_GUARD_VIOLATED;

  tchecker::dbm::reset(dbm, dim, clkreset);

  if (tchecker::dbm::constrain(dbm, dim, tgt_invariant) == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;

  if (tgt_delay_allowed) {
    tchecker::dbm::open_up(dbm, dim);

    if (tchecker::dbm::constrain(dbm, dim, tgt_invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;
  }

  return tchecker::STATE_OK;
}

/* factory */

tchecker::zg::semantics_t * semantics_factory(enum semantics_type_t semantics)
{
  switch (semantics) {
  case tchecker::zg::STANDARD_SEMANTICS:
    return new tchecker::zg::standard_semantics_t{};
  case tchecker::zg::ELAPSED_SEMANTICS:
    return new tchecker::zg::elapsed_semantics_t{};
  default:
    throw std::invalid_argument("Unknown zone semantics");
  }
}

} // end of namespace zg

} // end of namespace tchecker