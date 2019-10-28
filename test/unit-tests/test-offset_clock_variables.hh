/*
* This file is a part of the TChecker project.
*
* See files AUTHORS and LICENSE for copyright details.
*
*/

#include <stdexcept>

#include "tchecker/basictypes.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/ta/ta.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/variables/access.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/offset_clocks.hh"
#include "tchecker/variables/static_analysis.hh"

TEST_CASE( "Offset clock variables from empty access map", "[offset clock variables]" ) {
  
  tchecker::variable_access_map_t m;

  SECTION( "No clock" ) {
    tchecker::process_id_t proc_count = 3;
    tchecker::flat_clock_variables_t flat_clocks;
    
    tchecker::offset_clock_variables_t offset_clocks = build_from_variable_access(m, proc_count, flat_clocks);
    
    REQUIRE( offset_clocks.refcount() == proc_count );
    REQUIRE( offset_clocks.size() == offset_clocks.refcount() );
  }
  
  SECTION( "Unaccessed clocks" ) {
    tchecker::process_id_t proc_count = 2;
    
    tchecker::flat_clock_variables_t flat_clocks;
    flat_clocks.declare(tchecker::zero_clock_name, tchecker::clock_info_t{1});
    flat_clocks.declare("x", tchecker::clock_info_t{1});
    
    REQUIRE_THROWS_AS( build_from_variable_access(m, proc_count, flat_clocks), std::invalid_argument );
  }
  
}

TEST_CASE( "Offset clock variables from system - no array", "[offset clock variables]" ) {
  std::string declarations =
  "system:access_map_no_clock_array \n\
  event:a \n\
  \n\
  int:1:1:1:1:i \n\
  clock:1:x \n\
  clock:1:y \n\
  \n\
  process:P1 \n\
  location:P1:l0{initial:} \n\
  location:P1:l1{invariant: x<=1} \n\
  edge:P1:l0:l1:a{provided: y>0 : do: x=0} \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial:} \n\
  edge:P2:l0:l0:a{provided: i<=3} \n\
  ";
  
  tchecker::log_t log(&std::cerr);
  tchecker::parsing::system_declaration_t const * sysdecl = tchecker::test::parse(declarations, log);
  
  REQUIRE(sysdecl != nullptr);
  
  tchecker::ta::model_t model(*sysdecl, log);
  tchecker::variable_access_map_t vaccess_map = tchecker::variable_access(model);
  
  tchecker::process_id_t P1 = model.system().processes().key("P1");
  tchecker::process_id_t P2 = model.system().processes().key("P2");
  tchecker::clock_id_t x = model.system_clock_variables().id("x");
  tchecker::clock_id_t y = model.system_clock_variables().id("y");
  
  tchecker::offset_clock_variables_t offset_clocks = build_from_variable_access(vaccess_map,
                                                                                model.system().processes_count(),
                                                                                model.flattened_clock_variables());
  
  SECTION( "Check reference clocks" ) {
    REQUIRE( offset_clocks.refcount() == model.system().processes_count() );
    REQUIRE( offset_clocks.refmap()[P1] == P1 );
    REQUIRE( offset_clocks.refmap()[P2] == P2 );
  }
  
  SECTION( "Check offset clocks" ) {
    REQUIRE( offset_clocks.size() == offset_clocks.refcount() + 2 ); // x, y
    
    tchecker::clock_id_t offset_x = offset_clocks.refcount() + x - 1;
    tchecker::clock_id_t offset_y = offset_clocks.refcount() + y - 1;
    
    REQUIRE( offset_clocks.id("$x") == offset_x );
    REQUIRE( offset_clocks.id("$y") == offset_y );
    
    REQUIRE( offset_clocks.refmap()[offset_x] == P1 );
    REQUIRE( offset_clocks.refmap()[offset_y] == P1 );
  }
  
}


TEST_CASE( "Offset clock variables from system - array", "[offset clock variables]" ) {
  std::string declarations =
  "system:access_map_clock_array \n\
  event:a \n\
  \n\
  int:1:1:1:1:i \n\
  int:3:0:1:0:t \n\
  clock:1:x \n\
  clock:2:y \n\
  clock:1:z \n\
  \n\
  process:P1 \n\
  location:P1:l0{initial:} \n\
  location:P1:l1{invariant: x<=1} \n\
  edge:P1:l0:l1:a{provided: y[0]>0 : do: x=0} \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial:} \n\
  edge:P2:l0:l0:a{provided: i<=3 : do: t[i]=1} \n\
  \n\
  process:P3 \n\
  location:P3:l0{initial:} \n\
  location:P3:l1{invariant: y[1]<=2} \n\
  location:P3:l2 \n\
  edge:P3:l0:l1:a{provided: t[0]==1} \n\
  edge:P3:l1:l2:a{do: z=1+z} \n\
  ";
  
  tchecker::log_t log(&std::cerr);
  tchecker::parsing::system_declaration_t const * sysdecl = tchecker::test::parse(declarations, log);
  
  REQUIRE(sysdecl != nullptr);
  
  tchecker::ta::model_t model(*sysdecl, log);
  tchecker::variable_access_map_t vaccess_map = tchecker::variable_access(model);
  
  tchecker::process_id_t P1 = model.system().processes().key("P1");
  tchecker::process_id_t P2 = model.system().processes().key("P2");
  tchecker::process_id_t P3 = model.system().processes().key("P3");
  tchecker::clock_id_t x = model.flattened_clock_variables().id("x");
  tchecker::clock_id_t y0 = model.flattened_clock_variables().id("y[0]");
  tchecker::clock_id_t y1 = model.flattened_clock_variables().id("y[1]");
  tchecker::clock_id_t z = model.flattened_clock_variables().id("z");
  
  tchecker::offset_clock_variables_t offset_clocks = build_from_variable_access(vaccess_map,
                                                                                model.system().processes_count(),
                                                                                model.flattened_clock_variables());
  
  SECTION( "Check reference clocks" ) {
    REQUIRE( offset_clocks.refcount() == model.system().processes_count() );
    REQUIRE( offset_clocks.refmap()[P1] == P1 );
    REQUIRE( offset_clocks.refmap()[P2] == P2 );
    REQUIRE( offset_clocks.refmap()[P3] == P3 );
  }
  
  SECTION( "Check offset clocks" ) {
    REQUIRE( offset_clocks.size() == offset_clocks.refcount() + 4 ); // x, y[], z
    
    tchecker::clock_id_t offset_x = offset_clocks.refcount() + x - 1;
    tchecker::clock_id_t offset_y0 = offset_clocks.refcount() + y0 - 1;
    tchecker::clock_id_t offset_y1 = offset_clocks.refcount() + y1 - 1;
    tchecker::clock_id_t offset_z = offset_clocks.refcount() + z - 1;
    
    REQUIRE( offset_clocks.id("$x") == offset_x );
    REQUIRE( offset_clocks.id("$y[0]") == offset_y0 );
    REQUIRE( offset_clocks.id("$y[1]") == offset_y1 );
    REQUIRE( offset_clocks.id("$z") == offset_z );
    
    REQUIRE( offset_clocks.refmap()[offset_x] == P1 );
    REQUIRE( offset_clocks.refmap()[offset_y0] == P1 );
    REQUIRE( offset_clocks.refmap()[offset_y1] == P3 );
    REQUIRE( offset_clocks.refmap()[offset_z] == P3 );
  }
  
}
