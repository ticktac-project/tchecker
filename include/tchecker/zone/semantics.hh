/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZONE_SEMANTICS_HH
#define TCHECKER_ZONE_SEMANTICS_HH

#include "tchecker/basictypes.hh"

/*!
 \file semantics.hh
 \brief Operational semantics on zones
 */

namespace tchecker {
  
#ifdef HIDDEN_TO_COMPILER
  
  /*!
   \class zone_semantics_t
   \brief Zone semantics for timed automata
   */
  class zone_semantics_t {
  public:
    /*!
     \brief Compute initial zone
     \param zone : a zone
     \param delay_allowed : true if delay is allowed in initial state
     \param invariant : invariant
     \param vloc : tuple of locations
     \post zone is the initial zone w.r.t. delay_allowed, invariant and
     extrapolation w.r.t. clock bounds in vloc
     \return STATE_OK if the resulting zone is not empty, other values if the
     zone is empty (see details in implementations)
     */
    template <class VLOC>
    enum tchecker::state_status_t initialize(/* actual zone type */ & zone,
                                             bool delay_allowed,
                                             tchecker::clock_constraint_container_t const & invariant,
                                             VLOC const & vloc);
    
    /*!
     \brief Compute next zone
     \param zone : a zone
     \param src_delay_allowed : true if delay allowed in source state
     \param src_invariant : invariant in source state
     \param guard : transition guard
     \param clkreset : transition reset
     \param tgt_delay_allowed : true if delay allowed in target state
     \param tgt_invariant : invariant in target state
     \param tgt_vloc : tuple of locations in target state
     \post zone has been updated to its strongest postcondition w.r.t. src_delay_allowed,
     src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant and extrapolation
     w.r.t. clock bounds in tgt_vloc
     \return STATE_OK if the reulting zone is not empty, other values if the resulting
     zone is empty (see details in implementations)
     */
    template <class VLOC>
    enum tchecker::state_status_t next(/* actual zone type */ & zone,
                                       bool src_delay_allowed,
                                       tchecker::clock_constraint_container_t const & src_invariant,
                                       tchecker::clock_constraint_container_t const & guard,
                                       tchecker::clock_reset_container_t const & clkreset,
                                       bool tgt_delay_allowed,
                                       tchecker::clock_constraint_container_t const & tgt_invariant,
                                       VLOC const & tgt_vloc);
  };
  
#endif // HIDDEN_TO_COMPILER
  
} // end of namespace tchecker

#endif // TCHECKER_ZONE_SEMANTICS_HH

