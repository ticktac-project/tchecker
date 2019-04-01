/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/utils/log.hh"

namespace tchecker {
  
  /* log_t */
  
  log_t::log_t()
  : _null_os(boost::iostreams::null_sink()), _os(_null_os), _error_count(0), _warning_count(0)
  {}
  
  log_t::log_t(std::ostream & os)
  : _null_os(boost::iostreams::null_sink()), _os(os), _error_count(0), _warning_count(0)
  {}
  
  
  void log_t::display_counts() const
  {
    if ( (_error_count > 0) || (_warning_count > 0) )
      _os << std::endl;
    if (_error_count > 0)
      _os << _error_count << " error(s)";
    if ( (_error_count > 0) && (_warning_count > 0) )
      _os << " and ";
    if (_warning_count > 0)
      _os << _warning_count << " warning(s)";
    if ( (_error_count > 0) || (_warning_count > 0) )
      _os << std::endl;
  }
  
  
  void log_t::reset()
  {
    _os.flush();
    _error_count = 0;
    _warning_count = 0;
  }
  
} // end of namespace tchecker
