/*
* This file is a part of the TChecker project.
*
* See files AUTHORS and LICENSE for copyright details.
*
*/

#include <iterator>

#include "tchecker/variables/access.hh"

TEST_CASE( "Empty access map", "[access map]" ) {
  
  tchecker::variable_access_map_t m;
  
  SECTION( "has no shared variable" ) {
    REQUIRE( ! m.has_shared_variable() );
  }
  
  SECTION( "empty range of processes for given variable" ) {
    auto range = m.accessing_processes(0, tchecker::CLOCK, tchecker::ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }
  
  SECTION( "empty range of variables for given process" ) {
    auto range = m.accessed_variables(2, tchecker::INTVAR, tchecker::READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }
  
}


TEST_CASE( "Non empty access map", "[access map]" ) {

  tchecker::variable_access_map_t m;
  m.add(0, tchecker::CLOCK, tchecker::READ, 1);
  m.add(0, tchecker::INTVAR, tchecker::WRITE, 2);
  m.add(0, tchecker::CLOCK, tchecker::WRITE, 3);
  m.add(1, tchecker::INTVAR, tchecker::READ, 2);
  m.add(2, tchecker::CLOCK, tchecker::READ, 2);
  m.add(1, tchecker::CLOCK, tchecker::WRITE, 3);
  
  SECTION( "has shared variable" ) {
    REQUIRE( m.has_shared_variable() );
  }
  
  SECTION( "clock 0 is only read by process 1" ) {
    auto range = m.accessing_processes(0, tchecker::CLOCK, tchecker::READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(* range.begin() == 1);
    
    tchecker::process_id_t pid;
    REQUIRE_NOTHROW(pid = m.accessing_process(0, tchecker::CLOCK, tchecker::READ));
    REQUIRE(pid == 1);
  }
  
  SECTION( "clock 0 is accessed by two processes" ) {
    auto range = m.accessing_processes(0, tchecker::CLOCK, tchecker::ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 2);
    
    bool accessed_by_1 = false, accessed_by_3 = false;
    for (tchecker::process_id_t pid : range) {
      accessed_by_1 |= (pid == 1);
      accessed_by_3 |= (pid == 3);
    }
    REQUIRE(accessed_by_1);
    REQUIRE(accessed_by_3);
  }
  
  SECTION( "clock 2 is only accessed by process 2" ) {
    tchecker::process_id_t pid;
    REQUIRE_NOTHROW(pid = m.accessing_process(2, tchecker::CLOCK, tchecker::ANY));
    REQUIRE(pid == 2);
  }
  
  SECTION( "process 3 writes two clocks" ) {
    auto range = m.accessed_variables(3, tchecker::CLOCK, tchecker::WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 2);
    
    bool writes_0 = false, writes_1 = false;
    for (tchecker::variable_id_t vid : range) {
      writes_0 |= (vid == 0);
      writes_1 |= (vid == 1);
    }
    REQUIRE(writes_0);
    REQUIRE(writes_1);
  }
  
  SECTION( "process 2 accesses two bounded integer variables" ) {
    auto range = m.accessed_variables(2, tchecker::INTVAR, tchecker::ANY);
    REQUIRE(std::distance(range.begin(), range.end()));
    
    bool access_0 = false, access_1 = false;
    for (tchecker::variable_id_t vid : range) {
      access_0 |= (vid == 0);
      access_1 |= (vid == 1);
    }
    REQUIRE(access_0);
    REQUIRE(access_1);
  }
  
}
