/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VARIABLES_STATIC_ANALYSIS_HH
#define TCHECKER_VARIABLES_STATIC_ANALYSIS_HH

#include "tchecker/ta/system.hh"
#include "tchecker/variables/access.hh"

/*!
 \file static_analysis.hh
 \brief Static analysis for variables
 */

namespace tchecker {

/*!
 \brief Compute variable access map from a system
 \param system : a system
 \return the map of variable accesses from system
 */
tchecker::variable_access_map_t variable_access(tchecker::ta::system_t const & system);

} // end of namespace tchecker

#endif // TCHECKER_VARIABLES_STATIC_ANALYSIS_HH
