/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/zone/dbm/semantics.hh"

namespace tchecker {
  
  namespace dbm {
    
    namespace details {
      
      bool constrain(tchecker::dbm::db_t * dbm,
                     tchecker::clock_id_t dim,
                     tchecker::clock_constraint_container_t const & constraints)
      {
        for (tchecker::clock_constraint_t const & c : constraints) {
          auto cmp = (c.comparator() == tchecker::clock_constraint_t::LT
                      ? tchecker::dbm::LT
                      : tchecker::dbm::LE);
          if (tchecker::dbm::constrain(dbm, dim, c.id1(), c.id2(), cmp, c.value()) == tchecker::dbm::EMPTY)
            return false;
        }
        return true;
      }
      
      
      void reset(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim,
                 tchecker::clock_reset_container_t const & resets)
      {
        for (tchecker::clock_reset_t const & r : resets)
          tchecker::dbm::reset(dbm, dim, r.left_id(), r.right_id(), r.value());
      }
      
    } // end of namespace details
    
  } // end of namespace dbm
  
} // end of namespace tchecker
