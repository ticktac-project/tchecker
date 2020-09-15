/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <stdexcept>
#include <string>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/ta/ta.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/variables/access.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/static_analysis.hh"

TEST_CASE("Reference clock variables with no reference clock")
{
  std::vector<std::string> refclocks;
  REQUIRE_THROWS_AS(tchecker::reference_clock_variables_t(refclocks.begin(), refclocks.end()), std::invalid_argument);
}

TEST_CASE("Reference clock variables from empty access map", "[reference clock variables]")
{
  tchecker::flat_clock_variables_t flat_clocks;
  tchecker::variable_access_map_t m;

  SECTION("No clock - single reference clock")
  {
    tchecker::reference_clock_variables_t reference_clocks = tchecker::single_reference_clocks(flat_clocks);

    REQUIRE(reference_clocks.refcount() == 1);
    REQUIRE(reference_clocks.size() - reference_clocks.refcount() == 0);
  }

  SECTION("No clock - process reference clocks")
  {
    tchecker::process_id_t proc_count = 3;
    tchecker::reference_clock_variables_t reference_clocks = tchecker::process_reference_clocks(m, proc_count, flat_clocks);

    REQUIRE(reference_clocks.refcount() == proc_count);
    REQUIRE(reference_clocks.size() - reference_clocks.refcount() == 0);
  }

  SECTION("Unaccessed clocks - single reference clocks")
  {
    flat_clocks.declare("x", tchecker::clock_info_t{1});
    tchecker::reference_clock_variables_t reference_clocks = tchecker::single_reference_clocks(flat_clocks);

    REQUIRE(reference_clocks.refcount() == 1);
    REQUIRE(reference_clocks.size() - reference_clocks.refcount() == 1);
  }

  SECTION("Unaccessed clocks - process reference clocks")
  {
    tchecker::process_id_t proc_count = 2;

    tchecker::flat_clock_variables_t flat_clocks;
    flat_clocks.declare("x", tchecker::clock_info_t{1});

    REQUIRE_THROWS_AS(tchecker::process_reference_clocks(m, proc_count, flat_clocks), std::invalid_argument);
  }
}

TEST_CASE("Reference clock variables from system - no array", "[reference clock variables]")
{
  std::string declarations = "system:access_map_no_clock_array \n\
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

  tchecker::ta::system_t system(*sysdecl);
  tchecker::variable_access_map_t vaccess_map = tchecker::variable_access(system);

  tchecker::process_id_t P1 = system.process_id("P1");
  tchecker::process_id_t P2 = system.process_id("P2");
  tchecker::clock_id_t x = system.clock_id("x");
  tchecker::clock_id_t y = system.clock_id("y");

  tchecker::reference_clock_variables_t reference_clocks =
      tchecker::process_reference_clocks(vaccess_map, system.processes_count(), system.clock_variables().flattened());

  SECTION("Check reference clocks")
  {
    REQUIRE(reference_clocks.refcount() == system.processes_count());
    REQUIRE(reference_clocks.refmap()[P1] == P1);
    REQUIRE(reference_clocks.refmap()[P2] == P2);
  }

  SECTION("Check clocks")
  {
    REQUIRE(reference_clocks.size() == reference_clocks.refcount() + 2); // x, y

    tchecker::clock_id_t xx = reference_clocks.refcount() + x;
    tchecker::clock_id_t yy = reference_clocks.refcount() + y;

    REQUIRE(reference_clocks.translate_system_clock(x) == xx);
    REQUIRE(reference_clocks.translate_system_clock(y) == yy);

    REQUIRE(reference_clocks.id("$x") == xx);
    REQUIRE(reference_clocks.id("$y") == yy);

    REQUIRE(reference_clocks.refmap()[xx] == reference_clocks.refclock_of_system_clock(x));
    REQUIRE(reference_clocks.refmap()[yy] == reference_clocks.refclock_of_system_clock(y));

    REQUIRE(reference_clocks.refmap()[xx] == P1);
    REQUIRE(reference_clocks.refmap()[yy] == P1);
  }
}

TEST_CASE("Offset clock variables from system - array", "[offset clock variables]")
{
  std::string declarations = "system:access_map_clock_array \n\
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

  tchecker::ta::system_t system(*sysdecl);
  tchecker::variable_access_map_t vaccess_map = tchecker::variable_access(system);

  tchecker::process_id_t P1 = system.process_id("P1");
  tchecker::process_id_t P2 = system.process_id("P2");
  tchecker::process_id_t P3 = system.process_id("P3");
  tchecker::clock_id_t x = system.clock_variables().flattened().id("x");
  tchecker::clock_id_t y0 = system.clock_variables().flattened().id("y[0]");
  tchecker::clock_id_t y1 = system.clock_variables().flattened().id("y[1]");
  tchecker::clock_id_t z = system.clock_variables().flattened().id("z");

  tchecker::reference_clock_variables_t reference_clocks =
      tchecker::process_reference_clocks(vaccess_map, system.processes_count(), system.clock_variables().flattened());

  SECTION("Check reference clocks")
  {
    REQUIRE(reference_clocks.refcount() == system.processes_count());
    REQUIRE(reference_clocks.refmap()[P1] == P1);
    REQUIRE(reference_clocks.refmap()[P2] == P2);
    REQUIRE(reference_clocks.refmap()[P3] == P3);
  }

  SECTION("Check offset clocks")
  {
    REQUIRE(reference_clocks.size() == reference_clocks.refcount() + 4); // x, y[], z

    tchecker::clock_id_t xx = reference_clocks.refcount() + x;
    tchecker::clock_id_t yy0 = reference_clocks.refcount() + y0;
    tchecker::clock_id_t yy1 = reference_clocks.refcount() + y1;
    tchecker::clock_id_t zz = reference_clocks.refcount() + z;

    REQUIRE(reference_clocks.translate_system_clock(x) == xx);
    REQUIRE(reference_clocks.translate_system_clock(y0) == yy0);
    REQUIRE(reference_clocks.translate_system_clock(y1) == yy1);
    REQUIRE(reference_clocks.translate_system_clock(z) == zz);

    REQUIRE(reference_clocks.id("$x") == xx);
    REQUIRE(reference_clocks.id("$y[0]") == yy0);
    REQUIRE(reference_clocks.id("$y[1]") == yy1);
    REQUIRE(reference_clocks.id("$z") == zz);

    REQUIRE(reference_clocks.refmap()[xx] == reference_clocks.refclock_of_system_clock(x));
    REQUIRE(reference_clocks.refmap()[yy0] == reference_clocks.refclock_of_system_clock(y0));
    REQUIRE(reference_clocks.refmap()[yy1] == reference_clocks.refclock_of_system_clock(y1));
    REQUIRE(reference_clocks.refmap()[zz] == reference_clocks.refclock_of_system_clock(z));

    REQUIRE(reference_clocks.refmap()[xx] == P1);
    REQUIRE(reference_clocks.refmap()[yy0] == P1);
    REQUIRE(reference_clocks.refmap()[yy1] == P3);
    REQUIRE(reference_clocks.refmap()[zz] == P3);
  }
}
