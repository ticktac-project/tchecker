/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_LOG_HH
#define TCHECKER_LOG_HH

#include <iostream>

/*!
 \file log.hh
 \brief Log
 */

namespace tchecker {

/*!
 \brief Type of error signal
 */
struct log_error_t {
};

/*!
 \brief Error signal
 */
extern struct log_error_t const log_error;

/*!
 \brief Type of warning signal
 */
struct log_warning_t {
};

/*!
 \brief Warning signal
 */
extern struct log_warning_t const log_warning;

/*!
 \brief Accessor
 \return The number of errors that have been output
*/
unsigned int log_error_count();

/*!
 \brief Accessor
 \return The number of warnings that have been output
*/
unsigned int log_warning_count();

/*!
 \brief Output error and warning counters
 \param os : output stream
 \post The number of of errors and warnings have been output to os
*/
void log_output_count(std::ostream & os);

/*!
 \brief Output an error header
 \param os : output stream
 \param error : error header
 \post An error header has been output to os, and the error counter has been
 incremented
 \return os after output
 \note Should be used as:
 std::cerr << tchecker::log_error << "my error message" << std::endl;
 to both print an error header and count this message as an error message
 */
std::ostream & operator<<(std::ostream & os, struct tchecker::log_error_t const & error);

/*!
 \brief Output a warning header
 \param os : output stream
 \param warning : warning header
 \post A warning header has been output to os, and the warning counter has been
 incremented
 \return os after output
 \note Should be used as:
 std::cerr << tchecker::log_warning << "my warning" << std::endl;
 to both print a warning header and count this message as a warning
 */
std::ostream & operator<<(std::ostream & os, struct tchecker::log_warning_t const & warning);

} // end of namespace tchecker

#endif // TCHECKER_LOG_HH
