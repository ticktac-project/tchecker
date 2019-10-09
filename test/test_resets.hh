//
// Created by philipp on 09.10.19.
//

#ifndef TCHECKER_TEST_RESETS_HH
#define TCHECKER_TEST_RESETS_HH

#define DBM(i,j)       dbm[(i)*dim+(j)]
#define DBM1(i,j)      dbm1[(i)*dim+(j)]
#define DBM2(i,j)      dbm2[(i)*dim+(j)]

#include "tchecker/zone/dbm/semantics.hh"

TEST_CASE("Resets for elapsed semantics", "[elapsed_res]"){
  tchecker::clock_id_t const x_0 = 0;
  
  SECTION("Reset of singleton for trivial reset"){
    tchecker::clock_id_t const dim = 4;
    tchecker::dbm::db_t *dbm_before = new tchecker::dbm::db_t[dim*dim];
    tchecker::dbm::universal(dbm_before,  dim);
    //Make it contain {x=1, y=2, z=3}
    tchecker::clock_constraint_container_t constraints_before;
    constraints_before.reserve(6);
    for (signed char i=1; i<4;++i){
      constraints_before.emplace_back((tchecker::clock_id_t) i, x_0,tchecker::clock_constraint_t::comparator_t::LE, (tchecker::dbm::db_t) i);
      constraints_before.emplace_back(x_0,(tchecker::clock_id_t) i,tchecker::clock_constraint_t::comparator_t::LE,(tchecker::dbm::db_t) -i);
    }
    //Singleton
    REQUIRE(tchecker::dbm::details::constrain(dbm_before, dim, constraints_before));
    REQUIRE(tchecker::dbm::is_tight(dbm_before, dim));
    
    // Make two copies in order to apply the resets afterwards
    tchecker::dbm::db_t *dbm_1, *dbm_2;
    dbm_1 = new tchecker::dbm::db_t[dim*dim];
    dbm_2 = new tchecker::dbm::db_t[dim*dim];
    std::memcpy(dbm_1, dbm_before, dim*dim*sizeof(tchecker::dbm::db_t));
    std::memcpy(dbm_2, dbm_before, dim*dim*sizeof(tchecker::dbm::db_t));
    
    //Get the reset
    // We will set
    // y'=x and z'=y+1, so we except the outcome {x=1, y=1, z=3}
    tchecker::clock_reset_container_t reset_container1;
    reset_container1.reserve(2);
    reset_container1.emplace_back(2,1,0);
    reset_container1.emplace_back(3,2,1);
    // Check also order invariance
    tchecker::clock_reset_container_t reset_container2;
    reset_container2.reserve(2);
    reset_container2.emplace_back(3,2,1);
    reset_container2.emplace_back(2,1,0);
    
    //Apply them
    tchecker::dbm::details::reset(dbm_1, dim, reset_container1);
    tchecker::dbm::details::reset(dbm_2, dim, reset_container2);
    
    //Check if equivalent
    REQUIRE(tchecker::dbm::is_equal(dbm_1, dbm_2, dim));
    
    //Check if {x=1, y=1, z=3}
    tchecker::dbm::db_t *dbm_after = new tchecker::dbm::db_t[dim*dim];
    tchecker::dbm::universal(dbm_after,  dim);
    tchecker::clock_constraint_container_t constraints_after;
    constraints_after.reserve(6);
    constraints_after.emplace_back(1,0,tchecker::clock_constraint_t::comparator_t::LE, 1);
    constraints_after.emplace_back(0,1,tchecker::clock_constraint_t::comparator_t::LE, -1);
    constraints_after.emplace_back(2,0,tchecker::clock_constraint_t::comparator_t::LE, 1);
    constraints_after.emplace_back(0,2,tchecker::clock_constraint_t::comparator_t::LE, -1);
    constraints_after.emplace_back(3,0,tchecker::clock_constraint_t::comparator_t::LE, 3);
    constraints_after.emplace_back(0,3,tchecker::clock_constraint_t::comparator_t::LE, -3);

    //After becomes the singleton
    REQUIRE(tchecker::dbm::details::constrain(dbm_after, dim, constraints_after));
    
    //Final check
    REQUIRE(tchecker::dbm::is_equal(dbm_after, dbm_1, dim));
    
    delete []dbm_before;
    delete []dbm_1;
    delete []dbm_2;
    delete []dbm_after;
  }
  
  SECTION("Non-trivial reset for zone"){
    tchecker::clock_id_t const dim = 3;
    tchecker::dbm::db_t *dbm_before = new tchecker::dbm::db_t[dim*dim];
    tchecker::dbm::universal(dbm_before,  dim);
    //Make it contain {x=1, y=2, z=3}
    tchecker::clock_constraint_container_t constraints_before;
    constraints_before.reserve(5);
    constraints_before.emplace_back(1, x_0,tchecker::clock_constraint_t::comparator_t::LE, 4);
    constraints_before.emplace_back(x_0, 1,tchecker::clock_constraint_t::comparator_t::LE, -2);
    constraints_before.emplace_back(2, x_0,tchecker::clock_constraint_t::comparator_t::LE, 3);
    constraints_before.emplace_back(x_0, 2,tchecker::clock_constraint_t::comparator_t::LE, -1);
    constraints_before.emplace_back(1, 2,tchecker::clock_constraint_t::comparator_t::LE, 2);
  
    //Zone
    REQUIRE(tchecker::dbm::details::constrain(dbm_before, dim, constraints_before));
    REQUIRE(tchecker::dbm::is_tight(dbm_before, dim));
  
    // Make two copies in order to apply the resets afterwards
    tchecker::dbm::db_t *dbm_1, *dbm_2;
    dbm_1 = new tchecker::dbm::db_t[dim*dim];
    dbm_2 = new tchecker::dbm::db_t[dim*dim];
    std::memcpy(dbm_1, dbm_before, dim*dim*sizeof(tchecker::dbm::db_t));
    std::memcpy(dbm_2, dbm_before, dim*dim*sizeof(tchecker::dbm::db_t));
  
    //Get the reset
    // We will set
    // y'=x and z'=y+1, so we except the outcome {x=1, y=1, z=3}
    tchecker::clock_reset_container_t reset_container1;
    reset_container1.reserve(2);
    reset_container1.emplace_back(2,1,1);
    reset_container1.emplace_back(1,2,2);
    // Check also order invariance
    tchecker::clock_reset_container_t reset_container2;
    reset_container2.reserve(2);
    reset_container2.emplace_back(1,2,2);
    reset_container2.emplace_back(2,1,1);
  
    //Apply them
    tchecker::dbm::details::reset(dbm_1, dim, reset_container1);
    tchecker::dbm::details::reset(dbm_2, dim, reset_container2);
  
    //Check if equivalent
    REQUIRE(tchecker::dbm::is_equal(dbm_1, dbm_2, dim));
  
    //Check if {x=1, y=1, z=3}
    tchecker::dbm::db_t *dbm_after = new tchecker::dbm::db_t[dim*dim];
    tchecker::dbm::universal(dbm_after,  dim);
    tchecker::clock_constraint_container_t constraints_after;
    constraints_after.reserve(5);
    constraints_after.emplace_back(1, x_0,tchecker::clock_constraint_t::comparator_t::LE, 5);
    constraints_after.emplace_back(x_0, 1,tchecker::clock_constraint_t::comparator_t::LE, -3);
    constraints_after.emplace_back(2, x_0,tchecker::clock_constraint_t::comparator_t::LE, 5);
    constraints_after.emplace_back(x_0, 2,tchecker::clock_constraint_t::comparator_t::LE, -3);
    constraints_after.emplace_back(2, 1,tchecker::clock_constraint_t::comparator_t::LE, 1);
  
    //After becomes the singleton
    REQUIRE(tchecker::dbm::details::constrain(dbm_after, dim, constraints_after));
  
    //Final check
    REQUIRE(tchecker::dbm::is_equal(dbm_after, dbm_1, dim));
  
    delete []dbm_before;
    delete []dbm_1;
    delete []dbm_2;
    delete []dbm_after;
  }
  
  
}

#endif //TCHECKER_TEST_RESETS_HH
