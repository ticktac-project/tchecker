/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_STATIC_ANALYSIS_HH
#define TCHECKER_TA_STATIC_ANALYSIS_HH

#include "tchecker/ta/system.hh"

/*!
 \file static_analysis.hh
 \brief Static analysis on system of finite state machines with bounded integer variables (details)
 */
namespace tchecker {

namespace ta {

/*!
 \brief Checks if a system has non-trivial guard on a weakly synchronized event
 \param system : a system of timed processes
 \return true if system has an edge with a non-trivial guard that is labeled with an event that is weakly synchronized
 */
bool has_guarded_weakly_synchronized_event(tchecker::ta::system_t const & system);

/*!
 \brief Checks if a system has diagonal clock constraints
 \param system : a system of timed processes
 \return true if system has a diagonal constraint x - y # c in guard or a location invariant,
 false otherwise
 */
bool has_diagonal_constraint(tchecker::ta::system_t const & system);

/*!
 \brief Checks if a system has a non-constant clock reset
 \param system : a system of timed processes
 \return true if system has a non-constant reset x := y or x := y + c in the statement of
 a transition, false otherwise
 */
bool has_non_constant_reset(tchecker::ta::system_t const & system);

/*!
 \brief Checks if a system has a shared clock
 \param system : a system of timed processes
 \return true if there exists a clock x that is used by more than one process,
 false otherwise
 \note clock arrays may lead to false positive answer. For instance, if one process uses
 x[i] and another process uses x[j], where i or j cannot be determined, then the analysis
 will conclude that x is shared by the two processes, although it may not be the case as
 i != j (which cannot be determined statically)
 */
bool has_shared_clocks(tchecker::ta::system_t const & system);

} // end of namespace ta

} // end of namespace tchecker

#endif // TCHECKER_TA_STATIC_ANALYSIS_HH
