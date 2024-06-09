/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <iostream>
#include <memory>
#include <sstream>

#include "tchecker/parsing/declaration.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/system/system.hh"
#include "tchecker/variables/intvars.hh"

#include "utils.hh"

TEST_CASE("vloc from string", "[from_string]")
{
  std::string model = "system:from_string \n\
  event:a1 \n\
  event:a2 \n\
  event:a3 \n\
  \n\
  process:P1 \n\
  int:1:1:1:1:i1 \n\
  location:P1:l0{initial:} \n\
  location:P1:l1 \n\
  edge:P1:l0:l1:a1{provided: i1>0} \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial:} \n\
  location:P2:l1 \n\
  edge:P2:l0:l1:a2 \n\
  \n\
  process:P3 \n\
  int:1:1:1:1:i3 \n\
  location:P3:l0{initial:} \n\
  location:P3:l1 \n\
  edge:P3:l0:l1:a3{provided: i3<3} \n\
  \n\
  sync:P1@a1:P2@a2\n";

  std::shared_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};

  REQUIRE(sysdecl != nullptr);

  tchecker::system::system_t system{*sysdecl};
  tchecker::process_id_t const processes_count = static_cast<tchecker::process_id_t>(system.processes_count());
  tchecker::vloc_t * vloc = tchecker::vloc_allocate_and_construct(processes_count, processes_count);

  SECTION("Initialize vloc from a valid string")
  {
    REQUIRE_NOTHROW(tchecker::from_string(*vloc, system, "<l0,l1,l0>"));

    tchecker::loc_id_t loc_ids[3] = {0, 3, 4};
    for (tchecker::process_id_t const pid : system.processes_identifiers())
      REQUIRE((*vloc)[pid] == loc_ids[pid]);
  }

  SECTION("Initialize vloc from an invalid string (missing >)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*vloc, system, "<l0,l1,l0"), std::invalid_argument);
  }

  SECTION("Initialize vloc from an invalid string (missing <)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*vloc, system, "l0,l1,l0>"), std::invalid_argument);
  }

  SECTION("Initialize vloc from an invalid string (too many >)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*vloc, system, "<l0,l1,l0>>"), std::invalid_argument);
  }

  SECTION("Initialize vloc from a string with unknown location name")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*vloc, system, "<l4,l1,l0>"), std::invalid_argument);
  }

  SECTION("Initialize vloc from a string with too many location names")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*vloc, system, "<l1,l1,l0,l7>"), std::invalid_argument);
  }

  SECTION("Initialize vloc from a string too few location names")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*vloc, system, "<l0,l0>"), std::invalid_argument);
  }

  tchecker::vloc_destruct_and_deallocate(vloc);
}

TEST_CASE("intval from string", "[from_string]")
{
  std::string model = "system:from_string \n\
  event:a1 \n\
  event:a2 \n\
  event:a3 \n\
  \n\
  int:1:0:2:1:i1 \n\
  int:3:-2:5:0:i3 \n\
  \n\
  process:P1 \n\
  location:P1:l0{initial:} \n\
  location:P1:l1 \n\
  edge:P1:l0:l1:a1{provided: i1>0} \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial:} \n\
  location:P2:l1 \n\
  edge:P2:l0:l1:a2 \n\
  \n\
  process:P3 \n\
  location:P3:l0{initial:} \n\
  location:P3:l1 \n\
  edge:P3:l0:l1:a3{provided: i3[1]<3} \n\
  \n\
  sync:P1@a1:P2@a2\n";

  std::shared_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};

  REQUIRE(sysdecl != nullptr);

  tchecker::system::system_t system{*sysdecl};
  unsigned short const flatvars_count = static_cast<unsigned short>(system.integer_variables().flattened().size());
  tchecker::intval_t * intval = tchecker::intval_allocate_and_construct(flatvars_count, flatvars_count);

  SECTION("Initialize intval from a valid string")
  {
    REQUIRE_NOTHROW(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=1,i3[0]=-1,i3[1]=0,i3[2]=4"));

    REQUIRE((*intval)[0] == 1);
    REQUIRE((*intval)[1] == -1);
    REQUIRE((*intval)[2] == 0);
    REQUIRE((*intval)[3] == 4);
  }

  SECTION("Initialize intval from a valid string (unordered variables)")
  {
    REQUIRE_NOTHROW(tchecker::from_string(*intval, system.integer_variables().flattened(), "i3[1]=2,i3[0]=-1,i1=0,i3[2]=5"));

    REQUIRE((*intval)[0] == 0);
    REQUIRE((*intval)[1] == -1);
    REQUIRE((*intval)[2] == 2);
    REQUIRE((*intval)[3] == 5);
  }

  SECTION("Initialize intval from an invalid string (syntax error: missing lhs)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=2,=3,i3[0]=1,i3[1]=2"),
                      std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (syntax error: missing rhs)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=2,i3[2],i3[0]=1,i3[1]=2"),
                      std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (missing variable)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=2,i3[0]=3"),
                      std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (array variable not indexed)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=2,i3=3,i3[0]=1,i3[1]=3"),
                      std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (unknown variable)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i3[0]=1,i2=5,i1=0,i3[1]=2"),
                      std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (unknown variable: index out of array)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i3[0]=1,i1=1,i3[5]=0,i3[1]=2"),
                      std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (multiple assignments to one variable)")
  {
    REQUIRE_THROWS_AS(
        tchecker::from_string(*intval, system.integer_variables().flattened(), "i3[0]=1,i3[1]=0,i3[2]=-2,i1=1,i3[2]=0"),
        std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (value out of variable range)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=1,i3[0]=0,i3[1]=1,i3[2]=6"),
                      std::invalid_argument);
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=-11,i3[0]=0,i3[1]=1,i3[2]=0"),
                      std::invalid_argument);
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=1,i3[0]=-5,i3[1]=1,i3[2]=6"),
                      std::invalid_argument);
    REQUIRE_THROWS_AS(
        tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=19901,i3[0]=-2,i3[1]=1,i3[2]=4"),
        std::invalid_argument);
  }

  tchecker::intval_destruct_and_deallocate(intval);
}

TEST_CASE("clock constraints from string", "[from_string]")
{
  std::string model = "system:from_string \n\
  event:a1 \n\
  event:a2 \n\
  event:a3 \n\
  \n\
  int:1:0:5:1:i \n\
  int:1:-2:2:0:j \n\
  \n\
  clock:1:x \n\
  clock:2:y \n\
  \n\
  process:P1 \n\
  location:P1:l0{initial:} \n\
  location:P1:l1 \n\
  edge:P1:l0:l1:a1{provided: x>0} \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial:} \n\
  location:P2:l1 \n\
  edge:P2:l0:l1:a2{provided: y[0]==1} \n\
  \n\
  process:P3 \n\
  location:P3:l0{initial:} \n\
  location:P3:l1 \n\
  edge:P3:l0:l1:a3{provided: y[1]<3} \n\
  \n\
  sync:P1@a1:P2@a2\n";

  std::shared_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};

  REQUIRE(sysdecl != nullptr);

  tchecker::system::system_t system{*sysdecl};
  tchecker::clock_constraint_container_t c;

  // redirect std::cerr to local stream to hide parsing messages
  std::ostringstream dev_null;
  auto cerr_buff = std::cerr.rdbuf();
  std::cerr.rdbuf(dev_null.rdbuf());

  SECTION("Initialize container from a valid string")
  {
    REQUIRE_NOTHROW(tchecker::from_string(c, system.clock_variables(), "x==1 && y[0]>=3 && 4>y[1] && x-y[1]==7"));

    tchecker::clock_id_t const x = system.clock_variables().flattened().id("x");
    tchecker::clock_id_t const y0 = system.clock_variables().flattened().id("y[0]");
    tchecker::clock_id_t const y1 = system.clock_variables().flattened().id("y[1]");

    tchecker::clock_constraint_container_t expected_c;
    expected_c.emplace_back(x, tchecker::REFCLOCK_ID, tchecker::LE, 1);
    expected_c.emplace_back(tchecker::REFCLOCK_ID, x, tchecker::LE, -1);
    expected_c.emplace_back(tchecker::REFCLOCK_ID, y0, tchecker::LE, -3);
    expected_c.emplace_back(y1, tchecker::REFCLOCK_ID, tchecker::LT, 4);
    expected_c.emplace_back(x, y1, tchecker::LE, 7);
    expected_c.emplace_back(y1, x, tchecker::LE, -7);

    REQUIRE(c == expected_c);
  }

  SECTION("Initialize container from erroneous string (unknown clock)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(c, system.clock_variables(), "x==1 && z<2"), std::invalid_argument);
  }

  SECTION("Initialize container from erroneous string (array clock without index)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(c, system.clock_variables(), "x==1 && y<2"), std::invalid_argument);
  }

  SECTION("Initialize container from erroneous string (non constant array index)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(c, system.clock_variables(), "y[i]>9 && x==0"), std::invalid_argument);
  }

  SECTION("Initialize container from erroneous string (non constant bound)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(c, system.clock_variables(), "y[0]>j*8 && x==0"), std::invalid_argument);
  }

  SECTION("Initialize container from erroneous string (invalid clock constraint, TChecker syntax limitation)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(c, system.clock_variables(), "y[0]>x+5"), std::invalid_argument);
  }

  SECTION("Initialize container from erroneous string (non constant bound, involving 3 clocks)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(c, system.clock_variables(), "y[0]-x>y[1] && x==0"), std::invalid_argument);
  }

  SECTION("Initialize container from erroneous string (constraints on integer variables)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(c, system.clock_variables(), "y[0]>0 && i<j-6"), std::invalid_argument);
  }

  SECTION("Initialize container from a valid string, index with single reference clock")
  {
    tchecker::reference_clock_variables_t const refclocks = tchecker::single_reference_clocks(
        system.clock_variables().flattened(), static_cast<tchecker::process_id_t>(system.processes_count()));
    tchecker::clock_variables_t const clockvars{tchecker::clock_variables(refclocks, system.clock_variables())};

    REQUIRE_NOTHROW(tchecker::from_string(c, clockvars, "x==1 && y[0]>=3 && 4>y[1] && x-y[1]==7 && y[0]<$0"));

    tchecker::clock_id_t const t0 = clockvars.flattened().id("$0");
    tchecker::clock_id_t const x = clockvars.flattened().id("x");
    tchecker::clock_id_t const y0 = clockvars.flattened().id("y[0]");
    tchecker::clock_id_t const y1 = clockvars.flattened().id("y[1]");

    tchecker::clock_constraint_container_t expected_c;
    expected_c.emplace_back(x, tchecker::REFCLOCK_ID, tchecker::LE, 1);
    expected_c.emplace_back(tchecker::REFCLOCK_ID, x, tchecker::LE, -1);
    expected_c.emplace_back(tchecker::REFCLOCK_ID, y0, tchecker::LE, -3);
    expected_c.emplace_back(y1, tchecker::REFCLOCK_ID, tchecker::LT, 4);
    expected_c.emplace_back(x, y1, tchecker::LE, 7);
    expected_c.emplace_back(y1, x, tchecker::LE, -7);
    expected_c.emplace_back(y0, t0, tchecker::LT, 0);

    REQUIRE(c == expected_c);
  }

  SECTION("Initialize container from a valid string, index with process reference clock")
  {
    tchecker::variable_access_map_t const va_map{tchecker::variable_access(system)};
    tchecker::reference_clock_variables_t const refclocks = tchecker::process_reference_clocks(
        va_map, system.clock_variables().flattened(), static_cast<tchecker::process_id_t>(system.processes_count()));
    tchecker::clock_variables_t const clockvars{tchecker::clock_variables(refclocks, system.clock_variables())};

    REQUIRE_NOTHROW(tchecker::from_string(c, clockvars, "$0-x==4 && $1-$2<1 && y[0]<y[1] && $0>y[1] && y[0]-x>=5"));

    tchecker::clock_id_t const t0 = clockvars.flattened().id("$0");
    tchecker::clock_id_t const t1 = clockvars.flattened().id("$1");
    tchecker::clock_id_t const t2 = clockvars.flattened().id("$2");
    tchecker::clock_id_t const x = clockvars.flattened().id("x");
    tchecker::clock_id_t const y0 = clockvars.flattened().id("y[0]");
    tchecker::clock_id_t const y1 = clockvars.flattened().id("y[1]");

    tchecker::clock_constraint_container_t expected_c;
    expected_c.emplace_back(t0, x, tchecker::LE, 4);
    expected_c.emplace_back(x, t0, tchecker::LE, -4);
    expected_c.emplace_back(t1, t2, tchecker::LT, 1);
    expected_c.emplace_back(y0, y1, tchecker::LT, 0);
    expected_c.emplace_back(y1, t0, tchecker::LT, 0);
    expected_c.emplace_back(x, y0, tchecker::LE, -5);

    REQUIRE(c == expected_c);
  }

  // restore std::cerr buffer
  std::cerr.rdbuf(cerr_buff);
}