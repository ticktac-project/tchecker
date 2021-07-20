/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_OUTPUT_HH
#define TCHECKER_SYSTEM_OUTPUT_HH

#include <iostream>
#include <string>

#include "tchecker/basictypes.hh"
#include "tchecker/system/attribute.hh"
#include "tchecker/system/clock.hh"
#include "tchecker/system/edge.hh"
#include "tchecker/system/system.hh"

/*!
 \file output.hh
 \brief System output functions
 */

namespace tchecker {

namespace system {

/*!
 \brief Output attributes following tchecker syntax
 \param os : output stream
 \param attr : attributes
 \post attr has been output to os
 \return os after output
 */
std::ostream & operator<<(std::ostream & os, tchecker::system::attributes_t const & attr);

/*!
 \brief Output system following tchecker syntax
 \param os : output stream
 \param s : system
 \post s name has been output to os
 \return os after output
 */
std::ostream & operator<<(std::ostream & os, tchecker::system::system_t const & s);

} // end of namespace system

} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_OUTPUT_HH
