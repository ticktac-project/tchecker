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
  \return STATE_OK if the resulting DBM is not empty, other values if the
  resulting DBM is empty (see details in implementations)
   */
  virtual tchecker::state_status_t initial(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                           boost::dynamic_bitset<> const & delay_allowed,
                                           tchecker::clock_constraint_container_t const & invariant) = 0;

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
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is tight and consistent.
  the size of src_delay_allowed is the number of reference clocks in r.
  the size of sync_ref_clocks is the number of reference clocks in r.
  the size of tgt_delay_allowed is the number of reference clocks in r
  \post rdbm has been updated to its strongest postcondition w.r.t. src_delay_allowed,
  src_invariant, sync_ref_clocks, guard, clkreset, tgt_delay_allowed and tgt_invariant
  \return STATE_OK if the resulting DBM is not empty, other values if the resulting
  DBM is empty (see details in implementations)
   */
  virtual tchecker::state_status_t
  next(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
       boost::dynamic_bitset<> const & src_delay_allowed, tchecker::clock_constraint_container_t const & src_invariant,
       boost::dynamic_bitset<> const & sync_ref_clocks, tchecker::clock_constraint_container_t const & guard,
       tchecker::clock_reset_container_t const & clkreset, boost::dynamic_bitset<> const & tgt_delay_allowed,
       tchecker::clock_constraint_container_t const & tgt_invariant) = 0;
};

/*!
\class standard_semantics_t
\brief Standard semantics: each transition in the zone graph consists of a delay
(if allowed) followed by a transition from the timed automaton
*/
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
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is tight and consistent.
  the size of delay_allowed is the number of reference clocks in r (checked
  by assertion)
  \post rdbm is the zone that only containts the zero valuation
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the zero valuation does not
  satisfy invariant.
  */
  virtual tchecker::state_status_t initial(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                           boost::dynamic_bitset<> const & delay_allowed,
                                           tchecker::clock_constraint_container_t const & invariant);

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
  src_delay_allowed), then intersected with src_invariant, then all reference
  clocks in sync_ref_clocks have been synchronized in rdbm, then rdbm has been
  intersected with guard, then dbm has been reset w.r.t clkreset, then
  intersected with tgt_invariant
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if intersection with src_invariant
  result in an empty zone, tchecker::STATE_CLOCKS_SYNC_EMPTY if synchronization
  of reference clocks in sync_ref_clocks yield an empty zone,
  tchecker::STATE_CLOCKS_GUARD_VIOLATED if intersection with guard result in an
  empty zone, tchecker::STATE_EMPTY_SYNC if synchronization of reference clocks
  result in an empty zone, tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if
  intersection with tgt_invariant result in an empty zone
  */
  virtual tchecker::state_status_t
  next(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
       boost::dynamic_bitset<> const & src_delay_allowed, tchecker::clock_constraint_container_t const & src_invariant,
       boost::dynamic_bitset<> const & sync_ref_clocks, tchecker::clock_constraint_container_t const & guard,
       tchecker::clock_reset_container_t const & clkreset, boost::dynamic_bitset<> const & tgt_delay_allowed,
       tchecker::clock_constraint_container_t const & tgt_invariant);
};

/*!
\class elapsed_semantics_t
\brief elapsed semantics: each transition in the zone graph consists of a
transition from the automaton, followed by a delay (if allowed)
*/
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
  \pre rdbm is not nullptr (checked by assertion).
  rdbm is a r.size()*r.size() array of difference bounds.
  rdbm is not tight and consistent.
  the size of delay_allowed is the number of reference clocks in r (checked
  by assertion))
  \post rdbm is the zone that contains all the time successors of the zero
  valuation (for reference clocks that are allowed to delay in delay_allowed),
  and that satisfy invariant
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the (time successors of the)
  zero valuation does not satisfy invariant.
  */
  virtual tchecker::state_status_t initial(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                           boost::dynamic_bitset<> const & delay_allowed,
                                           tchecker::clock_constraint_container_t const & invariant);

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
  clocks that are allowed in tgt_delay_allowed) and intersected wth
  tgt_invariant again
  \return tchecker::STATE_OK if the resulting DBM is not empty. Otherwise,
  tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if intersection with src_invariant
  result in an empty zone, tchecker::STATE_CLOCKS_EMPTY_SYNC if synchronization
  of reference clocks in sync_ref_clocks yields an empty zone,
  tchecker::STATE_CLOCKS_GUARD_VIOLATED if intersection with guard result in an
  empty zone, tchecker::STATE_EMPTY_SYNC if synchronization of reference clocks
  result in an empty zone, tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if
  intersection with tgt_invariant result in an empty zone
  */
  virtual tchecker::state_status_t
  next(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
       boost::dynamic_bitset<> const & src_delay_allowed, tchecker::clock_constraint_container_t const & src_invariant,
       boost::dynamic_bitset<> const & sync_ref_clocks, tchecker::clock_constraint_container_t const & guard,
       tchecker::clock_reset_container_t const & clkreset, boost::dynamic_bitset<> const & tgt_delay_allowed,
       tchecker::clock_constraint_container_t const & tgt_invariant);
};

/*!
 \brief type of semantics
*/
enum semantics_type_t {
  STANDARD_SEMANTICS, /*!< see tchecker::refzg::standard_semantics_t */
  ELAPSED_SEMANTICS,  /*!< see tchecker::refzg::elapsed_semantics_t */
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
