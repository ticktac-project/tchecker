/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <memory>

#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/clockbounds/solver.hh"
#include "tchecker/parsing/declaration.hh"
#include "tchecker/ta/system.hh"
#include "utils.hh"

TEST_CASE("system with only one clock", "[clockbounds]")
{
  std::string model = "system:only_one_clock \n\
  clock:1:x\n";

  std::unique_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  tchecker::ta::system_t system{*sysdecl};

  std::unique_ptr<tchecker::clockbounds::clockbounds_t> clockbounds{tchecker::clockbounds::compute_clockbounds(system)};
  REQUIRE(clockbounds.get() != nullptr);

  SECTION("Global LU clock bounds")
  {
    tchecker::clockbounds::map_t * L = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));
    tchecker::clockbounds::map_t * U = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));

    clockbounds->global_lu(*L, *U);

    REQUIRE((*L)[0] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[0] == tchecker::clockbounds::NO_BOUND);

    tchecker::clockbounds::deallocate_map(L);
    tchecker::clockbounds::deallocate_map(U);
  }

  SECTION("Global M bounds")
  {
    tchecker::clockbounds::map_t * M = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));

    clockbounds->global_m(*M);

    REQUIRE((*M)[0] == tchecker::clockbounds::NO_BOUND);

    tchecker::clockbounds::deallocate_map(M);
  }
}

TEST_CASE("system with one process", "[clockbounds]")
{
  std::string model = "system:one_process \n\
  clock:1:x \n\
  clock:1:y \n\
  event:a \n\
  \n\
  process:P \n\
  location:P:q0{initial: true} \n\
  location:P:q1{invariant: x<3} \n\
  location:P:q2 \n\
  location:P:q3{invariant: y<1} \n\
  edge:P:q0:q1:a{provided: y>0 : do: x=0} \n\
  edge:P:q0:q2:a{provided: x>1} \n\
  edge:P:q2:q3:a\n";

  std::unique_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  tchecker::ta::system_t system{*sysdecl};

  std::unique_ptr<tchecker::clockbounds::clockbounds_t> clockbounds{tchecker::clockbounds::compute_clockbounds(system)};
  REQUIRE(clockbounds.get() != nullptr);

  tchecker::process_id_t const P = system.process_id("P");
  tchecker::loc_id_t const q0 = system.location(P, "q0")->id();
  tchecker::loc_id_t const q1 = system.location(P, "q1")->id();
  tchecker::loc_id_t const q2 = system.location(P, "q2")->id();
  tchecker::loc_id_t const q3 = system.location(P, "q3")->id();

  tchecker::clock_id_t const x = system.clock_id("x");
  tchecker::clock_id_t const y = system.clock_id("y");

  tchecker::clockbounds::map_t * L = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));
  tchecker::clockbounds::map_t * U = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));
  tchecker::clockbounds::map_t * M = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));

  SECTION("Local LU clock bounds")
  {
    // q0
    clockbounds->local_lu(q0, *L, *U);
    REQUIRE((*L)[x] == 1);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == 0);
    REQUIRE((*U)[y] == 1);

    // q1
    clockbounds->local_lu(q1, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == 3);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == tchecker::clockbounds::NO_BOUND);

    // q2
    clockbounds->local_lu(q2, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == 1);

    // q3
    clockbounds->local_lu(q3, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == 1);
  }

  SECTION("Global LU clock bounds")
  {
    clockbounds->global_lu(*L, *U);
    REQUIRE((*L)[x] == 1);
    REQUIRE((*U)[x] == 3);
    REQUIRE((*L)[y] == 0);
    REQUIRE((*U)[y] == 1);
  }

  SECTION("Local M clock bounds")
  {
    // q0
    clockbounds->local_m(q0, *M);
    REQUIRE((*M)[x] == 1);
    REQUIRE((*M)[y] == 1);

    // q1
    clockbounds->local_m(q1, *M);
    REQUIRE((*M)[x] == 3);
    REQUIRE((*M)[y] == tchecker::clockbounds::NO_BOUND);

    // q2
    clockbounds->local_m(q2, *M);
    REQUIRE((*M)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[y] == 1);

    // q3
    clockbounds->local_m(q3, *M);
    REQUIRE((*M)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[y] == 1);
  }

  SECTION("Global M clock bounds")
  {
    clockbounds->global_m(*M);
    REQUIRE((*M)[x] == 3);
    REQUIRE((*M)[y] == 1);
  }

  tchecker::clockbounds::deallocate_map(L);
  tchecker::clockbounds::deallocate_map(U);
  tchecker::clockbounds::deallocate_map(M);
}

TEST_CASE("system with clock array", "[clockbounds]")
{
  std::string model = "system:clock_array \n\
  clock:2:x \n\
  clock:1:y \n\
  int:1:0:3:1:i \n\
  event:a \n\
  \n\
  process:P \n\
  location:P:q0{initial: true} \n\
  location:P:q1{invariant: x[i]<3} \n\
  location:P:q2 \n\
  location:P:q3{invariant: y<1} \n\
  edge:P:q0:q1:a{provided: y>0 : do: x[0]=0} \n\
  edge:P:q0:q2:a{provided: x[1]>1} \n\
  edge:P:q2:q3:a{provided: x[1]<7}\n";

  std::unique_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  tchecker::ta::system_t system{*sysdecl};

  std::unique_ptr<tchecker::clockbounds::clockbounds_t> clockbounds{tchecker::clockbounds::compute_clockbounds(system)};
  REQUIRE(clockbounds.get() != nullptr);

  tchecker::process_id_t const P = system.process_id("P");
  tchecker::loc_id_t const q0 = system.location(P, "q0")->id();
  tchecker::loc_id_t const q1 = system.location(P, "q1")->id();
  tchecker::loc_id_t const q2 = system.location(P, "q2")->id();
  tchecker::loc_id_t const q3 = system.location(P, "q3")->id();

  tchecker::clock_id_t const x = system.clock_id("x");
  tchecker::clock_id_t const x0 = x + 0;
  tchecker::clock_id_t const x1 = x + 1;
  tchecker::clock_id_t const y = system.clock_id("y");

  tchecker::clockbounds::map_t * L = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));
  tchecker::clockbounds::map_t * U = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));
  tchecker::clockbounds::map_t * M = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));

  SECTION("Local LU clock bounds")
  {
    // q0
    clockbounds->local_lu(q0, *L, *U);
    REQUIRE((*L)[x0] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x0] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[x1] == 1);
    REQUIRE((*U)[x1] == 7);
    REQUIRE((*L)[y] == 0);
    REQUIRE((*U)[y] == 1);

    // q1
    clockbounds->local_lu(q1, *L, *U);
    REQUIRE((*L)[x0] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x0] == 3);
    REQUIRE((*L)[x1] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x1] == 3);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == tchecker::clockbounds::NO_BOUND);

    // q2
    clockbounds->local_lu(q2, *L, *U);
    REQUIRE((*L)[x0] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x0] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[x1] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x1] == 7);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == 1);

    // q3
    clockbounds->local_lu(q3, *L, *U);
    REQUIRE((*L)[x0] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x0] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[x1] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x1] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == 1);
  }

  SECTION("Global LU clock bounds")
  {
    clockbounds->global_lu(*L, *U);
    REQUIRE((*L)[x0] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x0] == 3);
    REQUIRE((*L)[x1] == 1);
    REQUIRE((*U)[x1] == 7);
    REQUIRE((*L)[y] == 0);
    REQUIRE((*U)[y] == 1);
  }

  SECTION("Local M clock bounds")
  {
    // q0
    clockbounds->local_m(q0, *M);
    REQUIRE((*M)[x0] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[x1] == 7);
    REQUIRE((*M)[y] == 1);

    // q1
    clockbounds->local_m(q1, *M);
    REQUIRE((*M)[x0] == 3);
    REQUIRE((*M)[x1] == 3);
    REQUIRE((*M)[y] == tchecker::clockbounds::NO_BOUND);

    // q2
    clockbounds->local_m(q2, *M);
    REQUIRE((*M)[x0] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[x1] == 7);
    REQUIRE((*M)[y] == 1);

    // q3
    clockbounds->local_m(q3, *M);
    REQUIRE((*M)[x0] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[x1] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[y] == 1);
  }

  SECTION("Global M clock bounds")
  {
    clockbounds->global_m(*M);
    REQUIRE((*M)[x0] == 3);
    REQUIRE((*M)[x1] == 7);
    REQUIRE((*M)[y] == 1);
  }

  tchecker::clockbounds::deallocate_map(L);
  tchecker::clockbounds::deallocate_map(U);
  tchecker::clockbounds::deallocate_map(M);
}

TEST_CASE("system with two processes, no shared clocks", "[clockbounds]")
{
  std::string model = "system:two_processes \n\
  clock:1:x \n\
  clock:1:y \n\
  event:a \n\
  \n\
  process:P \n\
  location:P:p0{initial: true} \n\
  location:P:p1{invariant: x<2} \n\
  location:P:p2 \n\
  location:P:p3{invariant: y<6} \n\
  edge:P:p0:p1:a{provided: y>1 : do: x=0} \n\
  edge:P:p0:p2:a{provided: x>8} \n\
  edge:P:p2:p3:a{do: x=0} \n\
  \n\
  clock:1:z \n\
  \n\
  process:Q \n\
  location:Q:q0{initial: true : invariant: z < 10} \n\
  location:Q:q1{invariant: z < 3} \n\
  location:Q:q2{invariant: z < 8} \n\
  edge:Q:q0:q1:a{provided: z > 0} \n\
  edge:Q:q1:q2:a{do: z = 0} \n ";

  std::unique_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  tchecker::ta::system_t system{*sysdecl};

  std::unique_ptr<tchecker::clockbounds::clockbounds_t> clockbounds{tchecker::clockbounds::compute_clockbounds(system)};
  REQUIRE(clockbounds.get() != nullptr);

  tchecker::process_id_t const P = system.process_id("P");
  tchecker::loc_id_t const p0 = system.location(P, "p0")->id();
  tchecker::loc_id_t const p1 = system.location(P, "p1")->id();
  tchecker::loc_id_t const p2 = system.location(P, "p2")->id();
  tchecker::loc_id_t const p3 = system.location(P, "p3")->id();

  tchecker::process_id_t const Q = system.process_id("Q");
  tchecker::loc_id_t const q0 = system.location(Q, "q0")->id();
  tchecker::loc_id_t const q1 = system.location(Q, "q1")->id();
  tchecker::loc_id_t const q2 = system.location(Q, "q2")->id();

  tchecker::clock_id_t const x = system.clock_id("x");
  tchecker::clock_id_t const y = system.clock_id("y");
  tchecker::clock_id_t const z = system.clock_id("z");

  tchecker::clockbounds::map_t * L = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));
  tchecker::clockbounds::map_t * U = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));
  tchecker::clockbounds::map_t * M = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));

  SECTION("Local LU clock bounds")
  {
    // p0
    clockbounds->local_lu(p0, *L, *U);
    REQUIRE((*L)[x] == 8);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == 1);
    REQUIRE((*U)[y] == 6);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == tchecker::clockbounds::NO_BOUND);

    // p1
    clockbounds->local_lu(p1, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == 2);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == tchecker::clockbounds::NO_BOUND);

    // p2
    clockbounds->local_lu(p2, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == 6);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == tchecker::clockbounds::NO_BOUND);

    // p3
    clockbounds->local_lu(p3, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == 6);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == tchecker::clockbounds::NO_BOUND);

    // q0
    clockbounds->local_lu(q0, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[z] == 0);
    REQUIRE((*U)[z] == 10);

    // q1
    clockbounds->local_lu(q1, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == 3);

    // q2
    clockbounds->local_lu(q2, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == 8);
  }

  SECTION("Global LU clock bounds")
  {
    clockbounds->global_lu(*L, *U);
    REQUIRE((*L)[x] == 8);
    REQUIRE((*U)[x] == 2);
    REQUIRE((*L)[y] == 1);
    REQUIRE((*U)[y] == 6);
    REQUIRE((*L)[z] == 0);
    REQUIRE((*U)[z] == 10);
  }

  SECTION("Local M clock bounds")
  {
    // p0
    clockbounds->local_m(p0, *M);
    REQUIRE((*M)[x] == 8);
    REQUIRE((*M)[y] == 6);
    REQUIRE((*M)[z] == tchecker::clockbounds::NO_BOUND);

    // p1
    clockbounds->local_m(p1, *M);
    REQUIRE((*M)[x] == 2);
    REQUIRE((*M)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[z] == tchecker::clockbounds::NO_BOUND);

    // p2
    clockbounds->local_m(p2, *M);
    REQUIRE((*M)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[y] == 6);
    REQUIRE((*M)[z] == tchecker::clockbounds::NO_BOUND);

    // p3
    clockbounds->local_m(p3, *M);
    REQUIRE((*M)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[y] == 6);
    REQUIRE((*M)[z] == tchecker::clockbounds::NO_BOUND);

    // q0
    clockbounds->local_m(q0, *M);
    REQUIRE((*M)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[z] == 10);

    // q1
    clockbounds->local_m(q1, *M);
    REQUIRE((*M)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[z] == 3);

    // q2
    clockbounds->local_m(q2, *M);
    REQUIRE((*M)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[z] == 8);
  }

  SECTION("Global M clock bounds")
  {
    clockbounds->global_m(*M);
    REQUIRE((*M)[x] == 8);
    REQUIRE((*M)[y] == 6);
    REQUIRE((*M)[z] == 10);
  }

  tchecker::clockbounds::deallocate_map(L);
  tchecker::clockbounds::deallocate_map(U);
  tchecker::clockbounds::deallocate_map(M);
}

TEST_CASE("system with two processes, with shared clocks", "[clockbounds]")
{
  // Network in Fig. 1 of
  // Static guards analysis in timed automata verification", Behrmann,
  // Bouyer, Fleury and Larsen, TACAS 2003
  std::string model = "system:two_processes_shared_clock \n\
  clock:1:x \n\
  clock:1:y \n\
  clock:1:z \n\
  event:a \n\
  \n\
  process:A1 \n\
  location:A1:l1{initial: true} \n\
  location:A1:l2{invariant: x<=14} \n\
  location:A1:l3{invariant: x<=14} \n\
  location:A1:l4 \n\
  edge:A1:l1:l2:a{provided: x<=5} \n\
  edge:A1:l2:l3:a{do: z=1+y} \n\
  edge:A1:l3:l2:a{provided: y>=5 : do: x=0} \n\
  edge:A1:l3:l4:a{do: y=0} \n\
  edge:A1:l4:l1:a{provided: y>=1000000} \n\
  \n\
  process:A2 \n\
  location:A2:n1{initial: true} \n\
  location:A2:n2 \n\
  edge:A2:n1:n2:a{provided: z<8 : do: x=3+z; z=0} \n\
  edge:A2:n2:n1:a{provided: z>=4} \n ";

  std::unique_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  tchecker::ta::system_t system{*sysdecl};

  std::unique_ptr<tchecker::clockbounds::clockbounds_t> clockbounds{tchecker::clockbounds::compute_clockbounds(system)};
  REQUIRE(clockbounds.get() != nullptr);

  tchecker::process_id_t const A1 = system.process_id("A1");
  tchecker::loc_id_t const l1 = system.location(A1, "l1")->id();
  tchecker::loc_id_t const l2 = system.location(A1, "l2")->id();
  tchecker::loc_id_t const l3 = system.location(A1, "l3")->id();
  tchecker::loc_id_t const l4 = system.location(A1, "l4")->id();

  tchecker::process_id_t const A2 = system.process_id("A2");
  tchecker::loc_id_t const n1 = system.location(A2, "n1")->id();
  tchecker::loc_id_t const n2 = system.location(A2, "n2")->id();

  tchecker::clock_id_t const x = system.clock_id("x");
  tchecker::clock_id_t const y = system.clock_id("y");
  tchecker::clock_id_t const z = system.clock_id("z");

  tchecker::clockbounds::map_t * L = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));
  tchecker::clockbounds::map_t * U = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));
  tchecker::clockbounds::map_t * M = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));

  SECTION("Local LU clock bounds")
  {
    // l1
    clockbounds->local_lu(l1, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == 14);
    REQUIRE((*L)[y] == 5);
    REQUIRE((*U)[y] == 10);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == tchecker::clockbounds::NO_BOUND);

    // l2
    clockbounds->local_lu(l2, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == 14);
    REQUIRE((*L)[y] == 5);
    REQUIRE((*U)[y] == 10);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == tchecker::clockbounds::NO_BOUND);

    // l3
    clockbounds->local_lu(l3, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == 14);
    REQUIRE((*L)[y] == 5);
    REQUIRE((*U)[y] == 10);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == tchecker::clockbounds::NO_BOUND);

    // l4
    clockbounds->local_lu(l4, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == 14);
    REQUIRE((*L)[y] == 1000000);
    REQUIRE((*U)[y] == 10);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == tchecker::clockbounds::NO_BOUND);

    // n1
    clockbounds->local_lu(n1, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == 11);

    // n2
    clockbounds->local_lu(n2, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[z] == 4);
    REQUIRE((*U)[z] == 11);
  }

  SECTION("Global LU clock bounds")
  {
    clockbounds->global_lu(*L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == 14);
    REQUIRE((*L)[y] == 1000000);
    REQUIRE((*U)[y] == 10);
    REQUIRE((*L)[z] == 4);
    REQUIRE((*U)[z] == 11);
  }

  SECTION("Local M clock bounds")
  {
    // l1
    clockbounds->local_m(l1, *M);
    REQUIRE((*M)[x] == 14);
    REQUIRE((*M)[y] == 10);
    REQUIRE((*M)[z] == tchecker::clockbounds::NO_BOUND);

    // l2
    clockbounds->local_m(l2, *M);
    REQUIRE((*M)[x] == 14);
    REQUIRE((*M)[y] == 10);
    REQUIRE((*M)[z] == tchecker::clockbounds::NO_BOUND);

    // l3
    clockbounds->local_m(l3, *M);
    REQUIRE((*M)[x] == 14);
    REQUIRE((*M)[y] == 10);
    REQUIRE((*M)[z] == tchecker::clockbounds::NO_BOUND);

    // l4
    clockbounds->local_m(l4, *M);
    REQUIRE((*M)[x] == 14);
    REQUIRE((*M)[y] == 1000000);
    REQUIRE((*M)[z] == tchecker::clockbounds::NO_BOUND);

    // n1
    clockbounds->local_m(n1, *M);
    REQUIRE((*M)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[z] == 11);

    // n2
    clockbounds->local_m(n2, *M);
    REQUIRE((*M)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[z] == 11);
  }

  SECTION("Global M clock bounds")
  {
    clockbounds->global_m(*M);
    REQUIRE((*M)[x] == 14);
    REQUIRE((*M)[y] == 1000000);
    REQUIRE((*M)[z] == 11);
  }

  tchecker::clockbounds::deallocate_map(L);
  tchecker::clockbounds::deallocate_map(U);
  tchecker::clockbounds::deallocate_map(M);
}

TEST_CASE("system with two processes, shared clocks, and sequential assignments", "[clockbounds]")
{
  std::string model = "system:two_processes_shared_clock_seq_assign \n\
  clock:1:x \n\
  clock:1:y \n\
  clock:1:z \n\
  event:e \n\
  event:a \n\
  \n\
  process:P \n\
  location:P:p0{initial: true} \n\
  location:P:p1 \n\
  location:P:p2 \n\
  location:P:p3 \n\
  edge:P:p0:p1:e{provided: z>1} \n\
  edge:P:p1:p2:a{do: y=z} \n\
  edge:P:p2:p3:e{provided: x<1} \n\
  \n\
  process:Q \n\
  location:Q:q0{initial: true} \n\
  location:Q:q1 \n\
  edge:Q:q0:q1:a{do: x=y} \n\
  \n\
  sync:P@a:Q@a \n";

  std::unique_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  tchecker::ta::system_t system{*sysdecl};

  std::unique_ptr<tchecker::clockbounds::clockbounds_t> clockbounds{tchecker::clockbounds::compute_clockbounds(system)};
  REQUIRE(clockbounds.get() != nullptr);

  tchecker::process_id_t const P = system.process_id("P");
  tchecker::loc_id_t const p0 = system.location(P, "p0")->id();
  tchecker::loc_id_t const p1 = system.location(P, "p1")->id();
  tchecker::loc_id_t const p2 = system.location(P, "p2")->id();
  tchecker::loc_id_t const p3 = system.location(P, "p3")->id();

  tchecker::process_id_t const Q = system.process_id("Q");
  tchecker::loc_id_t const q0 = system.location(Q, "q0")->id();
  tchecker::loc_id_t const q1 = system.location(Q, "q1")->id();

  tchecker::clock_id_t const x = system.clock_id("x");
  tchecker::clock_id_t const y = system.clock_id("y");
  tchecker::clock_id_t const z = system.clock_id("z");

  tchecker::clockbounds::map_t * L = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));
  tchecker::clockbounds::map_t * U = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));
  tchecker::clockbounds::map_t * M = tchecker::clockbounds::allocate_map(system.clocks_count(tchecker::VK_FLATTENED));

  SECTION("Local LU clock bounds")
  {
    // p0
    clockbounds->local_lu(p0, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == 1);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[z] == 1);
    REQUIRE((*U)[z] == 1);

    // p1
    clockbounds->local_lu(p1, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == 1);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == 1);

    // p2
    clockbounds->local_lu(p2, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == 1);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == tchecker::clockbounds::NO_BOUND);

    // p3
    clockbounds->local_lu(p3, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == tchecker::clockbounds::NO_BOUND);

    // q0
    clockbounds->local_lu(q0, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == 1);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == tchecker::clockbounds::NO_BOUND);

    // q1
    clockbounds->local_lu(q1, *L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*L)[z] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[z] == tchecker::clockbounds::NO_BOUND);
  }

  SECTION("Global LU clock bounds")
  {
    clockbounds->global_lu(*L, *U);
    REQUIRE((*L)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[x] == 1);
    REQUIRE((*L)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*U)[y] == 1);
    REQUIRE((*L)[z] == 1);
    REQUIRE((*U)[z] == 1);
  }

  SECTION("Local M clock bounds")
  {
    // p0
    clockbounds->local_m(p0, *M);
    REQUIRE((*M)[x] == 1);
    REQUIRE((*M)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[z] == 1);

    // p1
    clockbounds->local_m(p1, *M);
    REQUIRE((*M)[x] == 1);
    REQUIRE((*M)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[z] == 1);

    // p2
    clockbounds->local_m(p2, *M);
    REQUIRE((*M)[x] == 1);
    REQUIRE((*M)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[z] == tchecker::clockbounds::NO_BOUND);

    // p3
    clockbounds->local_m(p3, *M);
    REQUIRE((*M)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[z] == tchecker::clockbounds::NO_BOUND);

    // q0
    clockbounds->local_m(q0, *M);
    REQUIRE((*M)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[y] == 1);
    REQUIRE((*M)[z] == tchecker::clockbounds::NO_BOUND);

    // q1
    clockbounds->local_m(q1, *M);
    REQUIRE((*M)[x] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[y] == tchecker::clockbounds::NO_BOUND);
    REQUIRE((*M)[z] == tchecker::clockbounds::NO_BOUND);
  }

  SECTION("Global M clock bounds")
  {
    clockbounds->global_m(*M);
    REQUIRE((*M)[x] == 1);
    REQUIRE((*M)[y] == 1);
    REQUIRE((*M)[z] == 1);
  }

  tchecker::clockbounds::deallocate_map(L);
  tchecker::clockbounds::deallocate_map(U);
  tchecker::clockbounds::deallocate_map(M);
}