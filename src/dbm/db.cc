/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <limits>
#include <stdexcept>

#include "tchecker/dbm/db.hh"

namespace tchecker {
  
  namespace dbm {
    
    std::ostream & output(std::ostream & os, tchecker::dbm::db_t db)
    {
      if (db == tchecker::dbm::LT_INFINITY)
        os << "<inf";
      else
        os << tchecker::dbm::comparator_str(db) << tchecker::dbm::value(db);
      return os;
    }
    
    
    std::string comparator_str(tchecker::dbm::db_t db)
    {
      return (tchecker::dbm::comparator(db) == tchecker::dbm::LE ? "<=" : "<");
    }
    
  } // end of namesape dbm
  
} // end of namespace tchecker
