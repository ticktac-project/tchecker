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

} // end of namespace ta

} // end of namespace tchecker

#endif // TCHECKER_TA_STATIC_ANALYSIS_HH
