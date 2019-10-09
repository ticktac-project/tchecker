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
          auto cmp = (c.comparator() == tchecker::clock_constraint_t::LT ? tchecker::dbm::LT : tchecker::dbm::LE);
          if (tchecker::dbm::constrain(dbm, dim, c.id1(), c.id2(), cmp, c.value()) == tchecker::dbm::EMPTY)
            return false;
        }
        return true;
      }
      
      
      void reset(tchecker::dbm::db_t * dbm_old, tchecker::clock_id_t dim, tchecker::clock_reset_container_t const & resets)
      {
        // If the resets are safe all operations can be performed on the same dbm
        // otherwise we have to make a copy in order to ensure that the consecutive actions are order invariant
        if (resets.is_safe()){
          // Call the old version
          for (tchecker::clock_reset_t const & r : resets) {
            tchecker::dbm::reset(dbm_old, dim, r.left_id(), r.right_id(), r.value());
          }
        }else{
          tchecker::dbm::db_t *dbm_new = new tchecker::dbm::db_t[dim*dim];
          std::memcpy(dbm_new, dbm_old, dim*dim* sizeof(tchecker::dbm::db_t)); // [1]
          // Call the new version
          // Get the vector storing the informations necessary
          std::vector<reset_struct_t> reset_struct_vec;
          reset_struct_vec.reserve(dim*dim);
          tchecker::dbm::details::fill_reset_vector(reset_struct_vec, dim);
          //Get the info
          for (reset_struct_t & a_res_struct : reset_struct_vec){
            a_res_struct.visit_reset(resets);
          }
          //Apply
          //If apply_all is set to true, the above memcpy [1] is unnecessary
          tchecker::dbm::details::apply_reset_vector(dbm_new, dbm_old, dim, reset_struct_vec, false);
          // New version does not ensure tightness todo
          //Done
          std::memcpy(dbm_old, dbm_new, dim*dim* sizeof(tchecker::dbm::db_t));
          delete []dbm_new;
        }
      }
      
    } // end of namespace details
    
  } // end of namespace dbm
  
} // end of namespace tchecker
