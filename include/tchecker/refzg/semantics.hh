/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_REFZG_SEMANTICS_HH
#define TCHECKER_REFZG_SEMANTICS_HH

#include <boost/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/dbm/refdbm.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file semantics.hh
 \brief Operational semantics on DBMs with reference clocks
 */

namespace tchecker {

namespace refzg {

/*!
 \class semantics_t
 \brief Semantics for zone graphs implemented by DBMs with reference clocks
 */
class semantics_t {
public:
  /*!
  \brief Destructor
   */
  virtual ~semantics_t() = default;

  /*!
  \brief Compute initial zone with reference clocks
  \param rdbm : a DBM
  \param r : reference clocks for rdbm
  \param delay_allowed : set of reference clocks allowed to delay
  \param invariant : invariant
  \param spread : reference clocks spread
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is tight and consistent.
  the size of delay_allowed is the number of reference clocks in r
  \post dbm is the initial zone w.r.t. delay_allowed, invariant and spread
  \return see details in implementations
  \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
   */
  virtual tchecker::state_status_t initial(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                           boost::dynamic_bitset<> const & delay_allowed,
                                           tchecker::clock_constraint_container_t const & invariant,
                                           tchecker::integer_t spread) = 0;

  /*!
  \brief Compute final zone with reference clocks
  \param rdbm : a DBM
  \param r : reference clocks for rdbm
  \param delay_allowed : set of reference clocks allowed to delay
  \param invariant : invariant
  \param spread : reference clocks spread
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is tight and consistent.
  the size of delay_allowed is the number of reference clocks in r
  \post rdbm is the final (target) zone w.r.t. delay_allowed, invariant and spread
  \return STATE_OK if the resulting dbm is not empty, other values if the
  resulting dbm is empty (see details in implementations)
  \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
  */
  virtual tchecker::state_status_t final(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                         boost::dynamic_bitset<> const & delay_allowed,
                                         tchecker::clock_constraint_container_t const & invariant,
                                         tchecker::integer_t spread) = 0;

  /*!
  \brief Compute next zone with reference clocks
  \param rdbm : a DBM
  \param r : reference clocks for rdbm
  \param src_delay_allowed : set of reference clocks allowed to delay in source
  state
  \param src_invariant : invariant in source state
  \param sync_ref_clocks : set of reference clocks to synchronize
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : set of reference clocks allowed to delay in target
  state
  \param tgt_invariant : invariant in target state
  \param spread : reference clocks spread
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is tight and consistent.
  the size of src_delay_allowed is the number of reference clocks in r.
  the size of sync_ref_clocks is the number of reference clocks in r.
  the size of tgt_delay_allowed is the number of reference clocks in r
  \post rdbm has been updated to its strongest postcondition w.r.t.
  src_delay_allowed, src_invariant, sync_ref_clocks, guard, clkreset,
  tgt_delay_allowed, tgt_invariant and spread
  \return see details in implementations
  \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
   */
  virtual tchecker::state_status_t
  next(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
       boost::dynamic_bitset<> const & src_delay_allowed, tchecker::clock_constraint_container_t const & src_invariant,
       boost::dynamic_bitset<> const & sync_ref_clocks, tchecker::clock_constraint_container_t const & guard,
       tchecker::clock_reset_container_t const & clkreset, boost::dynamic_bitset<> const & tgt_delay_allowed,
       tchecker::clock_constraint_container_t const & tgt_invariant, tchecker::integer_t spread) = 0;

  /*!
  \brief Compute previous zone with reference clocks
  \param rdbm : a DBM
  \param r : reference clocks for rdbm
  \param src_delay_allowed : set of reference clocks allowed to delay in source
  state
  \param src_invariant : invariant in source state
  \param sync_ref_clocks : set of reference clocks to synchronize
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : set of reference clocks allowed to delay in target
  state
  \param tgt_invariant : invariant in target state
  \param spread : reference clocks spread
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is tight and consistent.
  the size of src_delay_allowed is the number of reference clocks in r.
  the size of sync_ref_clocks is the number of reference clocks in r.
  the size of tgt_delay_allowed is the number of reference clocks in r
  \post dbm has been updated to its weakest precondition w.r.t. src_delay_allowed,
  src_invariant, sync_ref_clocks, guard, clkreset, tgt_delay_allowed, tgt_invariant and spread
  \return STATE_OK if the resulting dbm is not empty, other values if the resulting
  dbm is empty (see details in implementations)
   */
  virtual tchecker::state_status_t
  prev(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
       boost::dynamic_bitset<> const & src_delay_allowed, tchecker::clock_constraint_container_t const & src_invariant,
       boost::dynamic_bitset<> const & sync_ref_clocks, tchecker::clock_constraint_container_t const & guard,
       tchecker::clock_reset_container_t const & clkreset, boost::dynamic_bitset<> const & tgt_delay_allowed,
       tchecker::clock_constraint_container_t const & tgt_invariant, tchecker::integer_t spread) = 0;
};

/*!
 \brief Semantics selection with respect to non synchronizable zones
*/
enum empty_sync_zone_policy_t {
  VALID = 0, /*!< Zones which are not synchronizable are valid */
  INVALID,   /*!< Zones which are not synchronizable are considered invalid (empty) */
};

/*!
 \class standard_semantics_t
 \tparam EMPTY_ZONE_POLICY : select how non synchronizable zones are handled
 \brief Standard semantics: each transition in the zone graph consists of a delay
(if allowed) followed by a transition from the timed automaton
*/
template <enum empty_sync_zone_policy_t EMPTY_ZONE_POLICY>
class standard_semantics_t final : public tchecker::refzg::semantics_t {
public:
  /*!
  \brief Destructor
  */
  virtual ~standard_semantics_t() = default;

  /*!
  \brief Compute initial zone with reference clocks
  \param rdbm : a DBM
  \param r : reference clocks for rdbm
  \param delay_allowed : set of reference clocks allowed to delay
  \param invariant : invariant
  \param spread : reference clocks spread
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is tight and consistent.
  the size of delay_allowed is the number of reference clocks in r (checked
  by assertion)
  \post rdbm is the zone that only contains the zero valuation
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_EMPTY_SPREAD if the zero valuation does not satisfies
  the given spread,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the zero valuation does not
  satisfy invariant.
  tchecker::STATE_ZONE_EMPTY_SYNC if the resulting zone does not contain any
  synchronized valuation and EMPTY_ZONE_POLICY is INVALID
  \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
  */
  virtual tchecker::state_status_t initial(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                           [[maybe_unused]] boost::dynamic_bitset<> const & delay_allowed,
                                           tchecker::clock_constraint_container_t const & invariant, tchecker::integer_t spread)
  {
    tchecker::refdbm::zero(rdbm, r);

    if (tchecker::refdbm::bound_spread(rdbm, r, spread) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_EMPTY_SPREAD;

    if (tchecker::refdbm::constrain(rdbm, r, invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

    if (EMPTY_ZONE_POLICY == tchecker::refzg::empty_sync_zone_policy_t::INVALID)
      if (!tchecker::refdbm::is_synchronizable(rdbm, r))
        return tchecker::STATE_ZONE_EMPTY_SYNC;

    return tchecker::STATE_OK;
  }

  /*!
  \brief Compute final zone with reference clocks
  \param rdbm : a DBM
  \param r : reference clocks for rdbm
  \param delay_allowed : set of reference clocks allowed to delay
  \param invariant : invariant
  \param spread : reference clocks spread
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is tight and consistent.
  the size of delay_allowed is the number of reference clocks in r
  \post rdbm is the final (target) zone w.r.t. delay_allowed, invariant and spread
  \return tchecker::STATE_OK if the resulting dbm is not empty
  tchecker::STATE_CLOCKS_EMPTY_SPREAD if spread bounding results in the empty
  zone,
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED; if intersection with invariant
  results in the empty zone
  tchecker::STATE_ZONE_EMPTY_SYNC if the resulting zone does not contain any
  synchronized valuation and EMPTY_ZONE_POLICY is INVALID
  \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
  */
  virtual tchecker::state_status_t final(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                         [[maybe_unused]] boost::dynamic_bitset<> const & delay_allowed,
                                         tchecker::clock_constraint_container_t const & invariant, tchecker::integer_t spread)
  {
    tchecker::refdbm::universal_positive(rdbm, r);

    if (tchecker::refdbm::bound_spread(rdbm, r, spread) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_EMPTY_SPREAD;

    if (tchecker::refdbm::constrain(rdbm, r, invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;

    if (EMPTY_ZONE_POLICY == tchecker::refzg::empty_sync_zone_policy_t::INVALID)
      if (!tchecker::refdbm::is_synchronizable(rdbm, r))
        return tchecker::STATE_ZONE_EMPTY_SYNC;

    return tchecker::STATE_OK;
  }

  /*!
  \brief Compute next zone with reference clocks
  \param rdbm : a DBM
  \param r : reference clocks for dbm
  \param src_delay_allowed : set of reference clocks allowed to delay in source
  state
  \param src_invariant : invariant in source state
  \param sync_ref_clocks : set of reference clocks to synchronize
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : set of reference clocks allowed to delay in target
  state
  \param tgt_invariant : invariant in target state
  \param spread : reference clocks spread
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is not tight and consistent.
  the size of src_delay_allowed is the number of reference clocks in r
  (checked by assertion).
  The size of sync_ref_clocks is the number of reference clocks in r (checked by
  assertion).)
  The size of tgt_delay_allowed is the number of reference clocks in r (checked
  by assertion).
  \post rdbm has been delayed (only reference clocks which are allowed in
  src_delay_allowed), then the spread in rdbm has been bounded by spread, then
  intersected with src_invariant, then all reference clocks in sync_ref_clocks
  have been synchronized in rdbm, then rdbm has been intersected with guard,
  then dbm has been reset w.r.t clkreset, then intersected with tgt_invariant
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if intersection with src_invariant
  result in an empty zone,
  tchecker::STATE_CLOCKS_EMPTY_SPREAD if spread bounding results in the empty
  zone,
  tchecker::STATE_CLOCKS_SYNC_EMPTY if synchronization of reference clocks in
  sync_ref_clocks yield an empty zone,
  tchecker::STATE_CLOCKS_GUARD_VIOLATED if intersection with guard result in an
  empty zone,
  tchecker::STATE_EMPTY_SYNC if synchronization of reference clocks
  result in an empty zone,
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if intersection with
  tgt_invariant result in an empty zone
  tchecker::STATE_ZONE_EMPTY_SYNC if the resulting zone does not contain any
  synchronized valuation and EMPTY_ZONE_POLICY is INVALID
  \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
  */
  virtual tchecker::state_status_t
  next(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
       boost::dynamic_bitset<> const & src_delay_allowed, tchecker::clock_constraint_container_t const & src_invariant,
       boost::dynamic_bitset<> const & sync_ref_clocks, tchecker::clock_constraint_container_t const & guard,
       tchecker::clock_reset_container_t const & clkreset, [[maybe_unused]] boost::dynamic_bitset<> const & tgt_delay_allowed,
       tchecker::clock_constraint_container_t const & tgt_invariant, tchecker::integer_t spread)
  {
    if (tchecker::refdbm::constrain(rdbm, r, src_invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

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

    if (EMPTY_ZONE_POLICY == tchecker::refzg::empty_sync_zone_policy_t::INVALID)
      if (!tchecker::refdbm::is_synchronizable(rdbm, r))
        return tchecker::STATE_ZONE_EMPTY_SYNC;

    return tchecker::STATE_OK;
  }

  /*!
  \brief Compute previous zone with reference clocks
  \param rdbm : a DBM
  \param r : reference clocks for dbm
  \param src_delay_allowed : set of reference clocks allowed to delay in source
  state
  \param src_invariant : invariant in source state
  \param sync_ref_clocks : set of reference clocks to synchronize
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : set of reference clocks allowed to delay in target
  state
  \param tgt_invariant : invariant in target state
  \param spread : reference clocks spread
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is not tight and consistent.
  the size of src_delay_allowed is the number of reference clocks in r
  (checked by assertion).
  The size of sync_ref_clocks is the number of reference clocks in r (checked by
  assertion).)
  The size of tgt_delay_allowed is the number of reference clocks in r (checked
  by assertion).
  \post rdbm has been intersected with target invariant and with resets, then reset
  clocks have been freed, then the resulting zone has been intersected with guard,
  synchronization contraint on reference clocks and source invariant. Then it has been
  open down (ony allowed reference clocks), intersected with source invariant again,
  and applied spread
  \return tchecker::STATE_OK if the resulting DBM is not empty.
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if intersection with
  tgt_invariant result in an empty zone,
  tchecker::STATE_CLOCKS_RESET_FAILED if intersection with clkreset (as constraints)
  resulted in empty zone,
  tchecker::STATE_CLOCKS_GUARD_VIOLATED if intersection with guard result in an
  empty zone,
  tchecker::STATE_EMPTY_SYNC if synchronization of reference clocks
  result in an empty zone,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if intersection with src_invariant
  result in an empty zone,
  tchecker::STATE_CLOCKS_EMPTY_SPREAD if spread bounding results in the empty
  zone
  tchecker::STATE_ZONE_EMPTY_SYNC if the resulting zone does not contain any
  synchronized valuation and EMPTY_ZONE_POLICY is INVALID
  \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
  */
  virtual tchecker::state_status_t
  prev(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
       boost::dynamic_bitset<> const & src_delay_allowed, tchecker::clock_constraint_container_t const & src_invariant,
       boost::dynamic_bitset<> const & sync_ref_clocks, tchecker::clock_constraint_container_t const & guard,
       tchecker::clock_reset_container_t const & clkreset, [[maybe_unused]] boost::dynamic_bitset<> const & tgt_delay_allowed,
       tchecker::clock_constraint_container_t const & tgt_invariant, tchecker::integer_t spread)
  {
    if (tchecker::refdbm::constrain(rdbm, r, tgt_invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;

    tchecker::clock_constraint_container_t resets_as_constraints;
    tchecker::clock_resets_to_constraints(clkreset, resets_as_constraints);
    if (tchecker::refdbm::constrain(rdbm, r, resets_as_constraints) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_RESET_FAILED;

    tchecker::refdbm::free_clock(rdbm, r, clkreset);

    if (tchecker::refdbm::constrain(rdbm, r, guard) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_GUARD_VIOLATED;

    if (tchecker::refdbm::synchronize(rdbm, r, sync_ref_clocks) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_EMPTY_SYNC;

    if (tchecker::refdbm::constrain(rdbm, r, src_invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

    tchecker::refdbm::asynchronous_open_down(rdbm, r, src_delay_allowed);

    if (tchecker::refdbm::bound_spread(rdbm, r, spread) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_EMPTY_SPREAD;

    if (tchecker::refdbm::constrain(rdbm, r, src_invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

    if (EMPTY_ZONE_POLICY == tchecker::refzg::empty_sync_zone_policy_t::INVALID)
      if (!tchecker::refdbm::is_synchronizable(rdbm, r))
        return tchecker::STATE_ZONE_EMPTY_SYNC;

    return tchecker::STATE_OK;
  }
};

/*!
 \class elapsed_semantics_t
 \tparam EMPTY_ZONE_POLICY : select how non synchronizable zones are handled
 \brief elapsed semantics: each transition in the zone graph consists of a transition
 from the automaton, followed by a delay (if allowed)
*/
template <enum empty_sync_zone_policy_t EMPTY_ZONE_POLICY>
class elapsed_semantics_t final : public tchecker::refzg::semantics_t {
public:
  /*!
  \brief Destructor
  */
  virtual ~elapsed_semantics_t() = default;

  /*!
  \brief Compute initial zone with reference clocks
  \param rdbm : a DBM
  \param r : reference clocks for rdbm
  \param delay_allowed : set of reference clocks allowed to delay
  \param invariant : invariant
  \param spread : reference clocks spread
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is not tight and consistent.
  the size of delay_allowed is the number of reference clocks in r (checked
  by assertion))
  \post rdbm is the zone that contains all the time successors of the zero
  valuation (for reference clocks that are allowed to delay in delay_allowed),
  that satisfy invariant, and with spread bounded to spread
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the (time successors of the)
  zero valuation does not satisfy invariant,
  tchecker::STATE_CLOCKS_EMPTY_SPREAD if spread bounding results in the empty
  zone
  tchecker::STATE_ZONE_EMPTY_SYNC if the resulting zone does not contain any
  synchronized valuation and EMPTY_ZONE_POLICY is INVALID
  \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
  */
  virtual tchecker::state_status_t initial(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
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

    if (EMPTY_ZONE_POLICY == tchecker::refzg::empty_sync_zone_policy_t::INVALID)
      if (!tchecker::refdbm::is_synchronizable(rdbm, r))
        return tchecker::STATE_ZONE_EMPTY_SYNC;

    return tchecker::STATE_OK;
  }

  /*!
  \brief Compute final zone with reference clocks
  \param rdbm : a DBM
  \param r : reference clocks for rdbm
  \param delay_allowed : set of reference clocks allowed to delay
  \param invariant : invariant
  \param spread : reference clocks spread
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is tight and consistent.
  the size of delay_allowed is the number of reference clocks in r
  \post rdbm is the final (target) zone w.r.t. delay_allowed, invariant and spread
  \return tchecker::STATE_OK if the resulting dbm is not empty
  tchecker::STATE_CLOCKS_EMPTY_SPREAD if spread bounding results in the empty
  zone,
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED; if intersection with invariant
  results in the empty zone
  tchecker::STATE_ZONE_EMPTY_SYNC if the resulting zone does not contain any
  synchronized valuation and EMPTY_ZONE_POLICY is INVALID
  \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
  */
  virtual tchecker::state_status_t final(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                         [[maybe_unused]] boost::dynamic_bitset<> const & delay_allowed,
                                         tchecker::clock_constraint_container_t const & invariant, tchecker::integer_t spread)
  {
    tchecker::refdbm::universal_positive(rdbm, r);

    if (tchecker::refdbm::bound_spread(rdbm, r, spread) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_EMPTY_SPREAD;

    if (tchecker::refdbm::constrain(rdbm, r, invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;

    if (EMPTY_ZONE_POLICY == tchecker::refzg::empty_sync_zone_policy_t::INVALID)
      if (!tchecker::refdbm::is_synchronizable(rdbm, r))
        return tchecker::STATE_ZONE_EMPTY_SYNC;

    return tchecker::STATE_OK;
  }

  /*!
  \brief Compute next zone with reference clocks
  \param rdbm : a DBM
  \param r : reference clocks for rdbm
  \param src_delay_allowed : set of reference clocks allowed to delay in source
  state
  \param src_invariant : invariant in source state
  \param sync_ref_clocks : set of reference clocks to synchronize
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : set of reference clocks allowed to delay in target
  state
  \param tgt_invariant : invariant in target state
  \param spread : reference clocks spread
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is not tight and consistent.
  the size of src_delay_allowed is the number of reference clocks in r
  (checked by assertion).
  The size of sync_ref_clocks is the number of reference clocks in r (checked by
  assertion).)
  The size of tgt_delay_allowed is the number of reference clocks in r (checked
  by assertion).
  \post rdbm has been intersected with src_invariant, then all reference clocks
  in sync_ref_clocks have been synchronized in rdbm, then rdbm has been
  intersected with guard, then rdbm has been reset w.r.t clkreset, then
  intersected with tgt_invariant, then rdbm has been delayed (only reference
  clocks that are allowed in tgt_delay_allowed), intersected wth tgt_invariant
  again, and finally spread-bounded w.r.t spread
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if intersection with src_invariant
  result in an empty zone,
  tchecker::STATE_CLOCKS_EMPTY_SYNC if synchronization of reference clocks in
  sync_ref_clocks yields an empty zone,
  tchecker::STATE_CLOCKS_GUARD_VIOLATED if intersection with guard result in an
  empty zone,
  tchecker::STATE_EMPTY_SYNC if synchronization of reference clocks result in an
  empty zone,
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if intersection with
  tgt_invariant result in an empty zone,
  tchecker::STATE_CLOCKS_EMPTY_SPREAD if bounding the spread results in the
  empty zone.
  tchecker::STATE_ZONE_EMPTY_SYNC if the resulting zone does not contain any
  synchronized valuation and EMPTY_ZONE_POLICY is INVALID
  \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
  */
  virtual tchecker::state_status_t
  next(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
       [[maybe_unused]] boost::dynamic_bitset<> const & src_delay_allowed,
       tchecker::clock_constraint_container_t const & src_invariant, boost::dynamic_bitset<> const & sync_ref_clocks,
       tchecker::clock_constraint_container_t const & guard, tchecker::clock_reset_container_t const & clkreset,
       boost::dynamic_bitset<> const & tgt_delay_allowed, tchecker::clock_constraint_container_t const & tgt_invariant,
       tchecker::integer_t spread)
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

    if (EMPTY_ZONE_POLICY == tchecker::refzg::empty_sync_zone_policy_t::INVALID)
      if (!tchecker::refdbm::is_synchronizable(rdbm, r))
        return tchecker::STATE_ZONE_EMPTY_SYNC;

    return tchecker::STATE_OK;
  }

  /*!
  \brief Compute previous zone with reference clocks
  \param rdbm : a DBM
  \param r : reference clocks for rdbm
  \param src_delay_allowed : set of reference clocks allowed to delay in source
  state
  \param src_invariant : invariant in source state
  \param sync_ref_clocks : set of reference clocks to synchronize
  \param guard : transition guard
  \param clkreset : transition reset
  \param tgt_delay_allowed : set of reference clocks allowed to delay in target
  state
  \param tgt_invariant : invariant in target state
  \param spread : reference clocks spread
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is not tight and consistent.
  the size of src_delay_allowed is the number of reference clocks in r
  (checked by assertion).
  The size of sync_ref_clocks is the number of reference clocks in r (checked by
  assertion).)
  The size of tgt_delay_allowed is the number of reference clocks in r (checked
  by assertion).
  \post rdbm has first been intersected with target invariant. Then, all reference clocks
  in tgt_delay_allowed have been opened down. Then, the resulting zone has been intersected
  with tgt_invariant and resets. Then reset clocks have been freed. Then the resulting zone
  has been intersected with guard, synchronization constraints on reference clocks and
  source invariant. Finally, spread has been applied
  \return tchecker::STATE_OK if the resulting DBM is not empty.
  tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if intersection with
  tgt_invariant result in an empty zone,
  tchecker::STATE_CLOCKS_RESET_FAILED if intersection with clkreset (as constraints)
  resulted in empty zone,
  tchecker::STATE_CLOCKS_GUARD_VIOLATED if intersection with guard result in an
  empty zone,
  tchecker::STATE_EMPTY_SYNC if synchronization of reference clocks result in an
  empty zone,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if intersection with src_invariant
  result in an empty zone,
  tchecker::STATE_CLOCKS_EMPTY_SPREAD if bounding the spread results in the
  empty zone
  tchecker::STATE_ZONE_EMPTY_SYNC if the resulting zone does not contain any
  synchronized valuation and EMPTY_ZONE_POLICY is INVALID
  \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
  */
  virtual tchecker::state_status_t
  prev(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
       [[maybe_unused]] boost::dynamic_bitset<> const & src_delay_allowed,
       tchecker::clock_constraint_container_t const & src_invariant, boost::dynamic_bitset<> const & sync_ref_clocks,
       tchecker::clock_constraint_container_t const & guard, tchecker::clock_reset_container_t const & clkreset,
       boost::dynamic_bitset<> const & tgt_delay_allowed, tchecker::clock_constraint_container_t const & tgt_invariant,
       tchecker::integer_t spread)
  {
    if (tchecker::refdbm::constrain(rdbm, r, tgt_invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;

    if (tgt_delay_allowed.any()) {
      tchecker::refdbm::asynchronous_open_down(rdbm, r, tgt_delay_allowed);

      if (tchecker::refdbm::constrain(rdbm, r, tgt_invariant) == tchecker::dbm::EMPTY)
        return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;
    }

    tchecker::clock_constraint_container_t resets_as_constraints;
    tchecker::clock_resets_to_constraints(clkreset, resets_as_constraints);
    if (tchecker::refdbm::constrain(rdbm, r, resets_as_constraints) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_RESET_FAILED;

    tchecker::refdbm::free_clock(rdbm, r, clkreset);

    if (tchecker::refdbm::constrain(rdbm, r, guard) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_GUARD_VIOLATED;

    if (tchecker::refdbm::synchronize(rdbm, r, sync_ref_clocks) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_EMPTY_SYNC;

    if (tchecker::refdbm::constrain(rdbm, r, src_invariant) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

    if (tchecker::refdbm::bound_spread(rdbm, r, spread) == tchecker::dbm::EMPTY)
      return tchecker::STATE_CLOCKS_EMPTY_SPREAD;

    if (EMPTY_ZONE_POLICY == tchecker::refzg::empty_sync_zone_policy_t::INVALID)
      if (!tchecker::refdbm::is_synchronizable(rdbm, r))
        return tchecker::STATE_ZONE_EMPTY_SYNC;

    return tchecker::STATE_OK;
  }
};

/*!
 \brief type of semantics
*/
enum semantics_type_t {
  STANDARD_SEMANTICS,      /*!< see tchecker::refzg::standard_semantics_t with VALID semantics */
  ELAPSED_SEMANTICS,       /*!< see tchecker::refzg::elapsed_semantics_t with VALID semantics */
  SYNC_STANDARD_SEMANTICS, /*!< see tchecker::refzg::standard_semantics_t with INVALID semantics */
  SYNC_ELAPSED_SEMANTICS,  /*!< see tchecker::refzg::elapsed_semantics_t with INVALID semantics */
};

/*!
 \brief Zone semantics factory
 \param semantics_type : type zone graph semantics
 \return A zone semantics over DBMs with reference clocks of type semantics
 \note the returned semantics must be deallocated by the caller
 \throw std::invalid_argument : if semantics is unknown
*/
tchecker::refzg::semantics_t * semantics_factory(enum tchecker::refzg::semantics_type_t semantics_type);

} // end of namespace refzg

} // end of namespace tchecker

#endif // TCHECKER_REFZG_SEMANTICS_HH
