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
  REQUIRE_THROWS_AS(tchecker::reference_clock_variables_t(refclocks), std::invalid_argument);
}

TEST_CASE("Reference clock variables from empty access map", "[reference clock variables]")
{
  tchecker::flat_clock_variables_t flat_clocks;
  tchecker::variable_access_map_t m;

  SECTION("No clock - single reference clock")
  {
    tchecker::reference_clock_variables_t reference_clocks = tchecker::single_reference_clocks(flat_clocks, 1);

    REQUIRE(reference_clocks.refcount() == 1);
    REQUIRE(reference_clocks.size() - reference_clocks.refcount() == 0);
    REQUIRE(reference_clocks.procmap()[0] == 0);
  }

  SECTION("No clock - process reference clocks")
  {
    tchecker::process_id_t const proc_count = 3;
    tchecker::reference_clock_variables_t reference_clocks = tchecker::process_reference_clocks(m, flat_clocks, proc_count);

    REQUIRE(reference_clocks.refcount() == proc_count);
    REQUIRE(reference_clocks.size() - reference_clocks.refcount() == 0);
    REQUIRE(reference_clocks.procmap()[0] == 0);
    REQUIRE(reference_clocks.procmap()[1] == 1);
    REQUIRE(reference_clocks.procmap()[2] == 2);
  }

  SECTION("Unaccessed clocks - single reference clocks")
  {
    flat_clocks.declare("x", tchecker::clock_info_t{1});
    tchecker::reference_clock_variables_t reference_clocks = tchecker::single_reference_clocks(flat_clocks, 2);

    REQUIRE(reference_clocks.refcount() == 1);
    REQUIRE(reference_clocks.size() - reference_clocks.refcount() == 1);
    REQUIRE(reference_clocks.procmap()[0] == 0);
    REQUIRE(reference_clocks.procmap()[1] == 0);
  }

  SECTION("Unaccessed clocks - process reference clocks")
  {
    tchecker::process_id_t const proc_count = 2;

    tchecker::flat_clock_variables_t flat_clocks;
    flat_clocks.declare("x", tchecker::clock_info_t{1});

    REQUIRE_THROWS_AS(tchecker::process_reference_clocks(m, flat_clocks, proc_count), std::invalid_argument);
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

  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{tchecker::test::parse(declarations)};

  REQUIRE(sysdecl != nullptr);

  tchecker::ta::system_t system(*sysdecl);
  tchecker::variable_access_map_t const vaccess_map = tchecker::variable_access(system);

  tchecker::process_id_t const P1 = system.process_id("P1");
  tchecker::process_id_t const P2 = system.process_id("P2");
  tchecker::clock_id_t const x = system.clock_id("x");
  tchecker::clock_id_t const y = system.clock_id("y");

  tchecker::reference_clock_variables_t const reference_clocks = tchecker::process_reference_clocks(
      vaccess_map, system.clock_variables().flattened(), static_cast<tchecker::process_id_t>(system.processes_count()));

  SECTION("Check reference clocks")
  {
    REQUIRE(reference_clocks.refcount() == system.processes_count());
    REQUIRE(reference_clocks.refmap()[P1] == P1);
    REQUIRE(reference_clocks.refmap()[P2] == P2);
    REQUIRE(reference_clocks.procmap()[P1] == P1);
    REQUIRE(reference_clocks.procmap()[P2] == P2);
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

  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{tchecker::test::parse(declarations)};

  REQUIRE(sysdecl != nullptr);

  tchecker::ta::system_t const system(*sysdecl);
  tchecker::variable_access_map_t const vaccess_map = tchecker::variable_access(system);

  tchecker::process_id_t const P1 = system.process_id("P1");
  tchecker::process_id_t const P2 = system.process_id("P2");
  tchecker::process_id_t const P3 = system.process_id("P3");
  tchecker::clock_id_t const x = system.clock_variables().flattened().id("x");
  tchecker::clock_id_t const y0 = system.clock_variables().flattened().id("y[0]");
  tchecker::clock_id_t const y1 = system.clock_variables().flattened().id("y[1]");
  tchecker::clock_id_t const z = system.clock_variables().flattened().id("z");

  tchecker::reference_clock_variables_t const reference_clocks = tchecker::process_reference_clocks(
      vaccess_map, system.clock_variables().flattened(), static_cast<tchecker::process_id_t>(system.processes_count()));

  SECTION("Check reference clocks")
  {
    REQUIRE(reference_clocks.refcount() == system.processes_count());
    REQUIRE(reference_clocks.refmap()[P1] == P1);
    REQUIRE(reference_clocks.refmap()[P2] == P2);
    REQUIRE(reference_clocks.refmap()[P3] == P3);
    REQUIRE(reference_clocks.procmap()[P1] == P1);
    REQUIRE(reference_clocks.procmap()[P2] == P2);
    REQUIRE(reference_clocks.procmap()[P3] == P3);
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

TEST_CASE("translation of clock constraints", "[clocks]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t2 = r.id("$2");
  tchecker::clock_id_t const x = r.id("x");
  tchecker::clock_id_t const y = r.id("y");
  tchecker::clock_id_t const z1 = r.id("z1");
  tchecker::clock_id_t const z2 = r.id("z2");

  tchecker::clock_id_t const _x = 0;
  tchecker::clock_id_t const _y = 1;
  tchecker::clock_id_t const _z1 = 2;
  tchecker::clock_id_t const _z2 = 3;

  SECTION("Upper-bound constraint")
  {
    tchecker::clock_constraint_t c = r.translate(tchecker::clock_constraint_t(_x, tchecker::REFCLOCK_ID, tchecker::LE, 1));
    REQUIRE(c.id1() == x);
    REQUIRE(c.id2() == t0);
    REQUIRE(c.comparator() == tchecker::LE);
    REQUIRE(c.value() == 1);
  }

  SECTION("Lower-bound constraint")
  {
    tchecker::clock_constraint_t c = r.translate(tchecker::clock_constraint_t(tchecker::REFCLOCK_ID, _z1, tchecker::LE, -3));
    REQUIRE(c.id1() == t2);
    REQUIRE(c.id2() == z1);
    REQUIRE(c.comparator() == tchecker::LE);
    REQUIRE(c.value() == -3);
  }

  SECTION("Diagonal constraint, same reference clock")
  {
    tchecker::clock_constraint_t c = r.translate(tchecker::clock_constraint_t(_z1, _z2, tchecker::LT, 19));
    REQUIRE(c.id1() == z1);
    REQUIRE(c.id2() == z2);
    REQUIRE(c.comparator() == tchecker::LT);
    REQUIRE(c.value() == 19);
  }

  SECTION("Diagonal constraint, distinct reference clocks")
  {
    tchecker::clock_constraint_t c = r.translate(tchecker::clock_constraint_t(_y, _x, tchecker::LE, -5));
    REQUIRE(c.id1() == y);
    REQUIRE(c.id2() == x);
    REQUIRE(c.comparator() == tchecker::LE);
    REQUIRE(c.value() == -5);
  }
}

TEST_CASE("translation of clock resets", "[clocks]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const x1 = r.id("x1");
  tchecker::clock_id_t const x2 = r.id("x2");
  tchecker::clock_id_t const y = r.id("y");
  tchecker::clock_id_t const z = r.id("z");

  tchecker::clock_id_t const _x1 = 0;
  tchecker::clock_id_t const _x2 = 1;
  tchecker::clock_id_t const _y = 2;
  tchecker::clock_id_t const _z = 3;

  SECTION("Reset to reference clock")
  {
    tchecker::clock_reset_t cr = r.translate(tchecker::clock_reset_t(_x1, tchecker::REFCLOCK_ID, 0));
    REQUIRE(cr.left_id() == x1);
    REQUIRE(cr.right_id() == t0);
    REQUIRE(cr.value() == 0);
  }

  SECTION("Reset to reference clock plus some constant")
  {
    tchecker::clock_reset_t cr = r.translate(tchecker::clock_reset_t(_y, tchecker::REFCLOCK_ID, 8));
    REQUIRE(cr.left_id() == y);
    REQUIRE(cr.right_id() == t1);
    REQUIRE(cr.value() == 8);
  }

  SECTION("Reset to other clock")
  {
    tchecker::clock_reset_t cr = r.translate(tchecker::clock_reset_t(_z, _x2, 0));
    REQUIRE(cr.left_id() == z);
    REQUIRE(cr.right_id() == x2);
    REQUIRE(cr.value() == 0);
  }

  SECTION("Reset to other clock, plus some constant")
  {
    tchecker::clock_reset_t cr = r.translate(tchecker::clock_reset_t(_x1, _y, 16));
    REQUIRE(cr.left_id() == x1);
    REQUIRE(cr.right_id() == y);
    REQUIRE(cr.value() == 16);
  }
}