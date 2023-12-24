/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <memory>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/parsing/declaration.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/ta/ta.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/static_analysis.hh"

#include "testutils/utils.hh"

TEST_CASE("delay allowed for all reference clocks", "[delay_allowed]")
{
  std::string model = "system:delay_allowed \n\
  \n\
  process:P1 \n\
  location:P1:l0{initial:} \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial:} \n\
  \n\
  process:P3 \n\
  location:P3:l0{initial:} \n\
  ";

  std::shared_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  assert(sysdecl != nullptr);

  tchecker::ta::system_t system{*sysdecl};

  tchecker::process_id_t const P1 = system.process_id("P1");
  tchecker::process_id_t const P2 = system.process_id("P2");
  tchecker::process_id_t const P3 = system.process_id("P3");

  tchecker::loc_id_t const P1_l0 = system.location(P1, "l0")->id();
  tchecker::loc_id_t const P2_l0 = system.location(P2, "l0")->id();
  tchecker::loc_id_t const P3_l0 = system.location(P3, "l0")->id();

  tchecker::vloc_t * vloc =
      tchecker::vloc_allocate_and_construct(static_cast<tchecker::process_id_t>(system.processes_count()),
                                            static_cast<tchecker::process_id_t>(system.processes_count()));
  (*vloc)[P1] = P1_l0;
  (*vloc)[P2] = P2_l0;
  (*vloc)[P3] = P3_l0;

  SECTION("Synchronous delay allowed") { REQUIRE(tchecker::ta::delay_allowed(system, *vloc)); }

  SECTION("Asynchronous delay allowed, single reference clock")
  {
    tchecker::reference_clock_variables_t r = single_reference_clocks(
        system.clock_variables().flattened(), static_cast<tchecker::process_id_t>(system.processes_count()));

    boost::dynamic_bitset<> delay_allowed = tchecker::ta::delay_allowed(system, r, *vloc);

    REQUIRE(delay_allowed.all());
  }

  SECTION("Asynchronous delay allowed, per process reference clock")
  {
    tchecker::variable_access_map_t va_map = tchecker::variable_access(system);

    tchecker::reference_clock_variables_t r = process_reference_clocks(
        va_map, system.clock_variables().flattened(), static_cast<tchecker::process_id_t>(system.processes_count()));

    boost::dynamic_bitset<> delay_allowed = tchecker::ta::delay_allowed(system, r, *vloc);

    REQUIRE(delay_allowed.all());
  }

  tchecker::vloc_destruct_and_deallocate(vloc);
}

TEST_CASE("delay allowed for some reference clocks", "[delay_allowed]")
{
  std::string model = "system:delay_allowed_some \n\
  \n\
  process:P1 \n\
  location:P1:l0{initial:} \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial: : committed:} \n\
  \n\
  process:P3 \n\
  location:P3:l0{initial: : urgent:} \n\
  ";

  std::shared_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  assert(sysdecl != nullptr);

  tchecker::ta::system_t system{*sysdecl};

  tchecker::process_id_t const P1 = system.process_id("P1");
  tchecker::process_id_t const P2 = system.process_id("P2");
  tchecker::process_id_t const P3 = system.process_id("P3");

  tchecker::loc_id_t const P1_l0 = system.location(P1, "l0")->id();
  tchecker::loc_id_t const P2_l0 = system.location(P2, "l0")->id();
  tchecker::loc_id_t const P3_l0 = system.location(P3, "l0")->id();

  tchecker::vloc_t * vloc =
      tchecker::vloc_allocate_and_construct(static_cast<tchecker::process_id_t>(system.processes_count()),
                                            static_cast<tchecker::process_id_t>(system.processes_count()));
  (*vloc)[P1] = P1_l0;
  (*vloc)[P2] = P2_l0;
  (*vloc)[P3] = P3_l0;

  SECTION("Synchronous delay not allowed") { REQUIRE_FALSE(tchecker::ta::delay_allowed(system, *vloc)); }

  SECTION("Asynchronous delay not allowed, single reference clock")
  {
    tchecker::reference_clock_variables_t r = single_reference_clocks(
        system.clock_variables().flattened(), static_cast<tchecker::process_id_t>(system.processes_count()));

    boost::dynamic_bitset<> delay_allowed = tchecker::ta::delay_allowed(system, r, *vloc);

    REQUIRE(delay_allowed.none());
  }

  SECTION("Asynchronous delay allowed only for reference clock of process P1")
  {
    tchecker::variable_access_map_t va_map = tchecker::variable_access(system);

    tchecker::reference_clock_variables_t r = process_reference_clocks(
        va_map, system.clock_variables().flattened(), static_cast<tchecker::process_id_t>(system.processes_count()));

    boost::dynamic_bitset<> delay_allowed = tchecker::ta::delay_allowed(system, r, *vloc);

    REQUIRE(delay_allowed[P1]);
    REQUIRE_FALSE(delay_allowed[P2]);
    REQUIRE_FALSE(delay_allowed[P3]);
  }

  tchecker::vloc_destruct_and_deallocate(vloc);
}

TEST_CASE("delay allowed for no reference clock", "[delay_allowed]")
{
  std::string model = "system:delay_allowed_some \n\
  \n\
  process:P1 \n\
  location:P1:l0{initial: : urgent:} \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial: : committed:} \n\
  \n\
  process:P3 \n\
  location:P3:l0{initial: : urgent:} \n\
  ";

  std::shared_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  assert(sysdecl != nullptr);

  tchecker::ta::system_t system{*sysdecl};

  tchecker::process_id_t const P1 = system.process_id("P1");
  tchecker::process_id_t const P2 = system.process_id("P2");
  tchecker::process_id_t const P3 = system.process_id("P3");

  tchecker::loc_id_t const P1_l0 = system.location(P1, "l0")->id();
  tchecker::loc_id_t const P2_l0 = system.location(P2, "l0")->id();
  tchecker::loc_id_t const P3_l0 = system.location(P3, "l0")->id();

  tchecker::vloc_t * vloc =
      tchecker::vloc_allocate_and_construct(static_cast<tchecker::process_id_t>(system.processes_count()),
                                            static_cast<tchecker::process_id_t>(system.processes_count()));
  (*vloc)[P1] = P1_l0;
  (*vloc)[P2] = P2_l0;
  (*vloc)[P3] = P3_l0;

  SECTION("Synchronous delay not allowed") { REQUIRE_FALSE(tchecker::ta::delay_allowed(system, *vloc)); }

  SECTION("Asynchronous delay not allowed, single reference clock")
  {
    tchecker::reference_clock_variables_t r = single_reference_clocks(
        system.clock_variables().flattened(), static_cast<tchecker::process_id_t>(system.processes_count()));

    boost::dynamic_bitset<> delay_allowed = tchecker::ta::delay_allowed(system, r, *vloc);

    REQUIRE(delay_allowed.none());
  }

  SECTION("Asynchronous delay not allowed, per process reference clock")
  {
    tchecker::variable_access_map_t va_map = tchecker::variable_access(system);

    tchecker::reference_clock_variables_t r = process_reference_clocks(
        va_map, system.clock_variables().flattened(), static_cast<tchecker::process_id_t>(system.processes_count()));

    boost::dynamic_bitset<> delay_allowed = tchecker::ta::delay_allowed(system, r, *vloc);

    REQUIRE(delay_allowed.none());
  }

  tchecker::vloc_destruct_and_deallocate(vloc);
}
