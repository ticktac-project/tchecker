/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <iostream>

#include "tchecker/basictypes.hh"

namespace tchecker {
  
  /* sync_strength_t */
  
  std::ostream & operator<< (std::ostream & os, enum tchecker::sync_strength_t s)
  {
    if (s == tchecker::SYNC_WEAK)
      os << "?";
    return os;
  }
  
} // end of namespace tchecker
