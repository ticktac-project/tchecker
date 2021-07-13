/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/utils/log.hh"

namespace tchecker {

/* Headers */

struct log_error_t const log_error;
struct log_warning_t const log_warning;

/* Counters */

static unsigned int _log_error_count;   /*!< Error counter */
static unsigned int _log_warning_count; /*!< Warning counter */

unsigned int log_error_count() { return tchecker::_log_error_count; }

unsigned int log_warning_count() { return tchecker::_log_warning_count; }

/* Output operators */

void log_output_count(std::ostream & os)
{
  if (tchecker::_log_error_count > 0)
    os << tchecker::_log_error_count << " error(s)";
  if (tchecker::_log_error_count > 0 && tchecker::_log_warning_count > 0)
    os << " and ";
  if (tchecker::_log_warning_count > 0)
    os << tchecker::_log_warning_count << " warning(s)";
  os << std::endl;
}

std::ostream & operator<<(std::ostream & os, struct tchecker::log_error_t const & error)
{
  os << "ERROR: ";
  ++tchecker::_log_error_count;
  return os;
}

std::ostream & operator<<(std::ostream & os, struct tchecker::log_warning_t const & warning)
{
  os << "WARNING: ";
  ++tchecker::_log_warning_count;
  return os;
}

} // end of namespace tchecker
