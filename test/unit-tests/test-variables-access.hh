/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <iterator>

#include "tchecker/basictypes.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/variables/access.hh"
#include "tchecker/variables/static_analysis.hh"

TEST_CASE("Empty access map", "[access map]")
{
  tchecker::variable_access_map_t m;

  SECTION("has no shared variable") { REQUIRE(!m.has_shared_variable()); }

  SECTION("empty range of processes for given variable")
  {
    auto range = m.accessing_processes(0, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }

  SECTION("empty range of variables for given process")
  {
    auto range = m.accessed_variables(2, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }
}

TEST_CASE("Non empty access map", "[access map]")
{
  tchecker::variable_access_map_t m;
  m.add(0, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ, 1);
  m.add(0, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE, 2);
  m.add(0, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE, 3);
  m.add(1, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ, 2);
  m.add(2, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ, 2);
  m.add(1, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE, 3);
  m.add(3, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ, 3);
  m.add(3, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE, 3);

  SECTION("has shared variable") { REQUIRE(m.has_shared_variable()); }

  SECTION("clock 0 is only read by process 1")
  {
    auto range = m.accessing_processes(0, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == 1);

    tchecker::process_id_t pid;
    REQUIRE_NOTHROW(pid = m.accessing_process(0, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ));
    REQUIRE(pid == 1);
  }

  SECTION("clock 0 is accessed by two processes")
  {
    auto range = m.accessing_processes(0, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 2);

    bool accessed_by_1 = false, accessed_by_3 = false;
    for (tchecker::process_id_t pid : range) {
      accessed_by_1 |= (pid == 1);
      accessed_by_3 |= (pid == 3);
    }
    REQUIRE(accessed_by_1);
    REQUIRE(accessed_by_3);
  }

  SECTION("clock 2 is only accessed by process 2")
  {
    tchecker::process_id_t pid;
    REQUIRE_NOTHROW(pid = m.accessing_process(2, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY));
    REQUIRE(pid == 2);
  }

  SECTION("clock 3 is only accessed by process 3")
  {
    auto range = m.accessing_processes(3, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == 3);
  }

  SECTION("process 3 writes three clocks")
  {
    auto range = m.accessed_variables(3, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 3);

    bool writes_0 = false, writes_1 = false, writes_3 = false;
    for (tchecker::variable_id_t vid : range) {
      writes_0 |= (vid == 0);
      writes_1 |= (vid == 1);
      writes_3 |= (vid == 3);
    }
    REQUIRE(writes_0);
    REQUIRE(writes_1);
    REQUIRE(writes_3);
  }

  SECTION("process 2 accesses two bounded integer variables")
  {
    auto range = m.accessed_variables(2, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY);
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

TEST_CASE("variable access map computation - empty system", "[access map]")
{
  std::string declarations = "system:access_map_empty \n\
  ";

  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{tchecker::test::parse(declarations)};

  REQUIRE(sysdecl != nullptr);

  tchecker::ta::system_t system(*sysdecl);
  tchecker::variable_access_map_t map = tchecker::variable_access(system);

  REQUIRE(!map.has_shared_variable());
}

TEST_CASE("variable access map computation - 1 process", "[access map]")
{
  std::string declarations = "system:access_map_1_process \n\
  event:a \n\
  \n\
  int:1:1:1:1:i \n\
  clock:1:x \n\
  \n\
  process:P \n\
  location:P:l0{initial:} \n\
  location:P:l1{invariant: x<=1} \n\
  edge:P:l0:l1:a{provided: i>0 : do: x=0} \n\
  ";

  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{tchecker::test::parse(declarations)};

  REQUIRE(sysdecl != nullptr);

  tchecker::ta::system_t system(*sysdecl);
  tchecker::variable_access_map_t map = tchecker::variable_access(system);

  REQUIRE(!map.has_shared_variable());

  tchecker::process_id_t P = system.process_id("P");
  tchecker::intvar_id_t i = system.intvar_id("i");
  tchecker::clock_id_t x = system.clock_id("x");

  SECTION("processes doing ANY access to variable i")
  {
    auto range = map.accessing_processes(i, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P);
    REQUIRE(map.accessing_process(i, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY) == P);
  }

  SECTION("processes doing READ access to variable i")
  {
    auto range = map.accessing_processes(i, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P);
    REQUIRE(map.accessing_process(i, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ) == P);
  }

  SECTION("processes doing WRITE access to varoable i")
  {
    auto range = map.accessing_processes(i, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }

  SECTION("processes doing ANY access to variable x")
  {
    auto range = map.accessing_processes(x, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P);
    REQUIRE(map.accessing_process(x, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY) == P);
  }

  SECTION("processes doing READ access to variable x")
  {
    auto range = map.accessing_processes(x, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P);
    REQUIRE(map.accessing_process(x, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ) == P);
  }

  SECTION("processes doing WRITE access to variable x")
  {
    auto range = map.accessing_processes(x, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P);
    REQUIRE(map.accessing_process(x, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE) == P);
  }

  SECTION("integer variables ANY-accessed by process P")
  {
    auto range = map.accessed_variables(P, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == i);
  }

  SECTION("integer variables READ-accessed by process P")
  {
    auto range = map.accessed_variables(P, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == i);
  }

  SECTION("integer variables WRITE-accessed by process P")
  {
    auto range = map.accessed_variables(P, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }

  SECTION("clocks ANY-accessed by process P")
  {
    auto range = map.accessed_variables(P, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == x);
  }

  SECTION("clocks READ-accessed by process P")
  {
    auto range = map.accessed_variables(P, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == x);
  }

  SECTION("clocks WRITE-accessed by process P")
  {
    auto range = map.accessed_variables(P, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == x);
  }

  SECTION("processes doing ANY access to non-existing varable")
  {
    auto range = map.accessing_processes(x + 1, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }

  SECTION("variable READ-accessed by non-existing process")
  {
    auto range = map.accessed_variables(P + 1, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }
}

TEST_CASE("variable access map computation - 2 processes, no shared variable", "[access map]")
{
  std::string declarations = "system:access_map_2_processes_no_shared \n\
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
  tchecker::variable_access_map_t map = tchecker::variable_access(system);

  REQUIRE(!map.has_shared_variable());

  tchecker::process_id_t P1 = system.process_id("P1");
  tchecker::process_id_t P2 = system.process_id("P2");
  tchecker::intvar_id_t i = system.intvar_id("i");
  tchecker::clock_id_t x = system.clock_id("x");
  tchecker::clock_id_t y = system.clock_id("y");

  SECTION("processes doing ANY access to variable i")
  {
    auto range = map.accessing_processes(i, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P2);
  }

  SECTION("processes doing READ access to variable i")
  {
    auto range = map.accessing_processes(i, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P2);
  }

  SECTION("processes doing WRITE access to variable i")
  {
    auto range = map.accessing_processes(i, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }

  SECTION("processes doing ANY access to variable x")
  {
    auto range = map.accessing_processes(x, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P1);
  }

  SECTION("processes doing READ access to variable x")
  {
    auto range = map.accessing_processes(x, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P1);
  }

  SECTION("processes doing WRITE access to variable x")
  {
    auto range = map.accessing_processes(x, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P1);
  }

  SECTION("processes doing ANY access to variable y")
  {
    auto range = map.accessing_processes(y, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P1);
  }

  SECTION("processes doing READ access to variable y")
  {
    auto range = map.accessing_processes(y, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P1);
  }

  SECTION("processes doing WRITE access to variable y")
  {
    auto range = map.accessing_processes(y, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }

  SECTION("integer variables ANY-accessed by process P1")
  {
    auto range = map.accessed_variables(P1, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }

  SECTION("integer variables READ-accessed by process P1")
  {
    auto range = map.accessed_variables(P1, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }

  SECTION("integer variables WRITE-accessed by process P1")
  {
    auto range = map.accessed_variables(P1, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }

  SECTION("clock variables ANY-accessed by process P1")
  {
    auto range = map.accessed_variables(P1, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 2);

    bool access_x = false, access_y = false;
    for (tchecker::clock_id_t clock_id : range) {
      access_x |= (clock_id == x);
      access_y |= (clock_id == y);
    }
    REQUIRE(access_x);
    REQUIRE(access_y);
  }

  SECTION("clock variables READ-accessed by process P1")
  {
    auto range = map.accessed_variables(P1, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 2);

    bool access_x = false, access_y = false;
    for (tchecker::clock_id_t clock_id : range) {
      access_x |= (clock_id == x);
      access_y |= (clock_id == y);
    }
    REQUIRE(access_x);
    REQUIRE(access_y);
  }

  SECTION("clock variables WRITE-accessed by process P1")
  {
    auto range = map.accessed_variables(P1, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == x);
  }

  SECTION("integer variables ANY-accessed by process P2")
  {
    auto range = map.accessed_variables(P2, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == i);
  }

  SECTION("integer variables READ-accessed by process P2")
  {
    auto range = map.accessed_variables(P2, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == i);
  }

  SECTION("integer variables WRITE-accessed by process P2")
  {
    auto range = map.accessed_variables(P2, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }

  SECTION("clock variables ANY-accessed by process P2")
  {
    auto range = map.accessed_variables(P2, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }

  SECTION("clock variables READ-accessed by process P2")
  {
    auto range = map.accessed_variables(P2, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }

  SECTION("clock variables WRITE-accessed by process P2")
  {
    auto range = map.accessed_variables(P2, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }
}

TEST_CASE("variable access map computation - 3 processes, array, shared variables", "[access map]")
{
  std::string declarations = "system:access_map_3_processes_shared \n\
  event:a \n\
  \n\
  int:1:1:1:1:i \n\
  int:3:0:1:0:t \n\
  clock:1:x \n\
  clock:2:y \n\
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
  location:P3:l1{invariant: y[i]<=2} \n\
  location:P3:l2 \n\
  edge:P3:l0:l1:a{provided: t[0]==1} \n\
  edge:P3:l1:l2:a{do: y[i+1]=0; i=1} \n\
  ";

  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{tchecker::test::parse(declarations)};

  REQUIRE(sysdecl != nullptr);

  tchecker::ta::system_t system(*sysdecl);
  tchecker::variable_access_map_t map = tchecker::variable_access(system);

  REQUIRE(map.has_shared_variable());

  tchecker::process_id_t P1 = system.process_id("P1");
  tchecker::process_id_t P2 = system.process_id("P2");
  tchecker::process_id_t P3 = system.process_id("P3");
  tchecker::intvar_id_t i = system.intvar_id("i");
  tchecker::intvar_id_t t = system.intvar_id("t");
  tchecker::intvar_id_t t0 = t + 0;
  tchecker::intvar_id_t t1 = t + 1;
  tchecker::intvar_id_t t2 = t + 2;
  tchecker::clock_id_t x = system.clock_id("x");
  tchecker::clock_id_t y = system.clock_id("y");
  tchecker::clock_id_t y0 = y + 0;
  tchecker::clock_id_t y1 = y + 1;

  SECTION("processes doing ANY access to variable i")
  {
    auto range = map.accessing_processes(i, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 2);

    bool access_P2 = false, access_P3 = false;
    for (tchecker::process_id_t pid : range) {
      access_P2 |= (pid == P2);
      access_P3 |= (pid == P3);
    }

    REQUIRE(access_P2);
    REQUIRE(access_P3);
  }

  SECTION("processes doing READ access to variable i")
  {
    auto range = map.accessing_processes(i, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 2);

    bool access_P2 = false, access_P3 = false;
    for (tchecker::process_id_t pid : range) {
      access_P2 |= (pid == P2);
      access_P3 |= (pid == P3);
    }

    REQUIRE(access_P2);
    REQUIRE(access_P3);
  }

  SECTION("processes doing WRITE access to variable i")
  {
    auto range = map.accessing_processes(i, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P3);
  }

  SECTION("processes doing ANY access to variable t[0]")
  {
    auto range = map.accessing_processes(t0, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 2);

    bool access_P2 = false, access_P3 = false;
    for (tchecker::process_id_t pid : range) {
      access_P2 |= (pid == P2);
      access_P3 |= (pid == P3);
    }

    REQUIRE(access_P2);
    REQUIRE(access_P3);
  }

  SECTION("processes doing READ access to variable t[0]")
  {
    auto range = map.accessing_processes(t0, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P3);
  }

  SECTION("processes doing WRITE access to variable t[0]")
  {
    auto range = map.accessing_processes(t0, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P2);
  }

  SECTION("processes doing ANY access to variable t[1]")
  {
    auto range = map.accessing_processes(t1, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P2);
  }

  SECTION("processes doing READ access to variable t[1]")
  {
    auto range = map.accessing_processes(t1, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 0);
  }

  SECTION("processes doing WRITE access to variable t[1]")
  {
    auto range = map.accessing_processes(t1, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P2);
  }

  SECTION("processes doing ANY access to variable t[2]")
  {
    auto range = map.accessing_processes(t2, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P2);
  }

  SECTION("processes doing READ access to variable t[2]")
  {
    auto range = map.accessing_processes(t2, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);
    REQUIRE(range.begin() == range.end());
  }

  SECTION("processes doing WRITE access to variable t[2]")
  {
    auto range = map.accessing_processes(t2, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P2);
  }

  SECTION("processes doing ANY access to variable x")
  {
    auto range = map.accessing_processes(x, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P1);
  }

  SECTION("processes doing READ access to variable x")
  {
    auto range = map.accessing_processes(x, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P1);
  }

  SECTION("processes doing WRITE access to variable x")
  {
    auto range = map.accessing_processes(x, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P1);
  }

  SECTION("processes doing ANY access to variable y[0]")
  {
    auto range = map.accessing_processes(y0, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 2);

    bool access_P1 = false, access_P3 = false;
    for (tchecker::process_id_t pid : range) {
      access_P1 |= (pid == P1);
      access_P3 |= (pid == P3);
    }

    REQUIRE(access_P1);
    REQUIRE(access_P3);
  }

  SECTION("processes doing READ access to variable y[0]")
  {
    auto range = map.accessing_processes(y0, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 2);

    bool access_P1 = false, access_P3 = false;
    for (tchecker::process_id_t pid : range) {
      access_P1 |= (pid == P1);
      access_P3 |= (pid == P3);
    }

    REQUIRE(access_P1);
    REQUIRE(access_P3);
  }

  SECTION("processes doing WRITE access to variable y[0]")
  {
    auto range = map.accessing_processes(y0, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P3);
  }

  SECTION("processes doing ANY access to variable y[1]")
  {
    auto range = map.accessing_processes(y1, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P3);
  }

  SECTION("processes doing READ access to variable y[1]")
  {
    auto range = map.accessing_processes(y1, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P3);
  }

  SECTION("processes doing WRITE access to variable y[1]")
  {
    auto range = map.accessing_processes(y1, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == P3);
  }

  SECTION("integer variables ANY-accessed by process P1")
  {
    auto range = map.accessed_variables(P1, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY);
    REQUIRE(range.begin() == range.end());
  }

  SECTION("integer variables READ-accessed by process P1")
  {
    auto range = map.accessed_variables(P1, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);
    REQUIRE(range.begin() == range.end());
  }

  SECTION("integer variables WRITE-accessed by process P1")
  {
    auto range = map.accessed_variables(P1, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE);
    REQUIRE(range.begin() == range.end());
  }

  SECTION("clock variables ANY-accessed by process P1")
  {
    auto range = map.accessed_variables(P1, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(std::distance(range.begin(), range.end()) == 2);

    bool access_x = false, access_y0 = false;
    for (tchecker::variable_id_t id : range) {
      access_x |= (id == x);
      access_y0 |= (id == y0);
    }

    REQUIRE(access_x);
    REQUIRE(access_y0);
  }

  SECTION("clock variables READ-accessed by process P1")
  {
    auto range = map.accessed_variables(P1, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 2);

    bool access_x = false, access_y0 = false;
    for (tchecker::variable_id_t id : range) {
      access_x |= (id == x);
      access_y0 |= (id == y0);
    }

    REQUIRE(access_x);
    REQUIRE(access_y0);
  }

  SECTION("clock variables WRITE-accessed by process P1")
  {
    auto range = map.accessed_variables(P1, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == x);
  }

  SECTION("integer variables ANY-accessed by process P2")
  {
    auto range = map.accessed_variables(P2, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY);

    long t_size = system.integer_variables().info(t).size();

    REQUIRE(std::distance(range.begin(), range.end()) == 1 + t_size); // i + t[]

    bool access_x = false, access_t[t_size];
    for (long i = 0; i < t_size; ++i)
      access_t[i] = false;

    for (tchecker::intvar_id_t id : range) {
      access_x |= (id == x);
      for (long i = 0; i < t_size; ++i)
        access_t[i] |= (id == t + i);
    }

    REQUIRE(access_x);
    for (long i = 0; i < t_size; ++i)
      REQUIRE(access_t[i]);
  }

  SECTION("integer variables READ-accessed by process P2")
  {
    auto range = map.accessed_variables(P2, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);
    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == i);
  }

  SECTION("integer variables WRITE-accessed by process P2")
  {
    auto range = map.accessed_variables(P2, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE);

    long t_size = system.integer_variables().info(t).size();

    REQUIRE(std::distance(range.begin(), range.end()) == t_size); // t[]

    bool access_t[t_size];
    for (long i = 0; i < t_size; ++i)
      access_t[i] = false;

    for (tchecker::intvar_id_t id : range) {
      for (long i = 0; i < t_size; ++i)
        access_t[i] |= (id == t + i);
    }

    for (long i = 0; i < t_size; ++i)
      REQUIRE(access_t[i]);
  }

  SECTION("clock variables ANY-accessed by process P2")
  {
    auto range = map.accessed_variables(P2, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
    REQUIRE(range.begin() == range.end());
  }

  SECTION("clock variables READ-accessed by process P2")
  {
    auto range = map.accessed_variables(P2, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ);
    REQUIRE(range.begin() == range.end());
  }

  SECTION("clock variables WRITE-accessed by process P2")
  {
    auto range = map.accessed_variables(P2, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE);
    REQUIRE(range.begin() == range.end());
  }

  SECTION("integer variables ANY-accessed by process P3")
  {
    auto range = map.accessed_variables(P3, tchecker::VTYPE_INTVAR, tchecker::VACCESS_ANY);

    REQUIRE(std::distance(range.begin(), range.end()) == 2);

    bool access_i = false, access_t0 = false;
    for (tchecker::intvar_id_t id : range) {
      access_i |= (id == i);
      access_t0 |= (id == t0);
    }

    REQUIRE(access_i);
    REQUIRE(access_t0);
  }

  SECTION("integer variables READ-accessed by process P3")
  {
    auto range = map.accessed_variables(P3, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ);

    REQUIRE(std::distance(range.begin(), range.end()) == 2);

    bool access_i = false, access_t0 = false;
    for (tchecker::intvar_id_t id : range) {
      access_i |= (id == i);
      access_t0 |= (id == t0);
    }

    REQUIRE(access_i);
    REQUIRE(access_t0);
  }

  SECTION("integer variables WRITE-accessed by process P3")
  {
    auto range = map.accessed_variables(P3, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE);

    REQUIRE(std::distance(range.begin(), range.end()) == 1);
    REQUIRE(*range.begin() == i);
  }

  SECTION("clock variables ANY-accessed by process P3")
  {
    auto range = map.accessed_variables(P3, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);

    long y_size = system.clock_variables().info(y).size();

    REQUIRE(std::distance(range.begin(), range.end()) == y_size); // y[]

    bool access_y[y_size];
    for (long i = 0; i < y_size; ++i)
      access_y[i] = false;

    for (tchecker::intvar_id_t id : range) {
      for (long i = 0; i < y_size; ++i)
        access_y[i] |= (id == y + i);
    }

    for (long i = 0; i < y_size; ++i)
      REQUIRE(access_y[i]);
  }

  SECTION("clock variables READ-accessed by process P3")
  {
    auto range = map.accessed_variables(P3, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ);

    long y_size = system.clock_variables().info(y).size();

    REQUIRE(std::distance(range.begin(), range.end()) == y_size); // y[]

    bool access_y[y_size];
    for (long i = 0; i < y_size; ++i)
      access_y[i] = false;

    for (tchecker::intvar_id_t id : range) {
      for (long i = 0; i < y_size; ++i)
        access_y[i] |= (id == y + i);
    }

    for (long i = 0; i < y_size; ++i)
      REQUIRE(access_y[i]);
  }

  SECTION("clock variables WRITE-accessed by process P3")
  {
    auto range = map.accessed_variables(P3, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE);

    long y_size = system.clock_variables().info(y).size();

    REQUIRE(std::distance(range.begin(), range.end()) == y_size); // y[]

    bool access_y[y_size];
    for (long i = 0; i < y_size; ++i)
      access_y[i] = false;

    for (tchecker::intvar_id_t id : range) {
      for (long i = 0; i < y_size; ++i)
        access_y[i] |= (id == y + i);
    }

    for (long i = 0; i < y_size; ++i)
      REQUIRE(access_y[i]);
  }
}
