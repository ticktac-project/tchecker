/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <boost/dynamic_bitset.hpp>

#include "tchecker/syncprod/syncprod.hh"
#include "tchecker/syncprod/system.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/utils/log.hh"

#include "testutils/utils.hh"

TEST_CASE("Labels in tuple of locations", "[labels]")
{
  std::string model = "system:labels \n\
  \n\
  process:P1 \n\
  location:P1:l0{initial:} \n\
  location:P1:l1{initial: : labels: a,b} \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial: : labels: a} \n\
  location:P2:l1{initial:} \n\
  \n\
  process:P3 \n\
  location:P3:l0{initial:} \n\
  location:P3:l1{initial: : labels: b,c,d} \n\
  ";

  std::shared_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  assert(sysdecl != nullptr);

  tchecker::syncprod::system_t system{*sysdecl};

  tchecker::process_id_t const P1 = system.process_id("P1");
  tchecker::process_id_t const P2 = system.process_id("P2");
  tchecker::process_id_t const P3 = system.process_id("P3");

  tchecker::loc_id_t const P1_l0 = system.location(P1, "l0")->id();
  tchecker::loc_id_t const P1_l1 = system.location(P1, "l1")->id();
  tchecker::loc_id_t const P2_l0 = system.location(P2, "l0")->id();
  tchecker::loc_id_t const P2_l1 = system.location(P2, "l1")->id();
  tchecker::loc_id_t const P3_l0 = system.location(P3, "l0")->id();
  tchecker::loc_id_t const P3_l1 = system.location(P3, "l1")->id();

  tchecker::label_id_t const a = system.label_id("a");
  tchecker::label_id_t const b = system.label_id("b");
  tchecker::label_id_t const c = system.label_id("c");
  tchecker::label_id_t const d = system.label_id("d");

  tchecker::vloc_t * vloc =
      tchecker::vloc_allocate_and_construct(static_cast<tchecker::process_id_t>(system.processes_count()),
                                            static_cast<tchecker::process_id_t>(system.processes_count()));

  SECTION("No label")
  {
    (*vloc)[P1] = P1_l0;
    (*vloc)[P2] = P2_l1;
    (*vloc)[P3] = P3_l0;

    boost::dynamic_bitset<> labels = tchecker::syncprod::labels(system, *vloc);
    REQUIRE(labels.size() == system.labels_count());
    REQUIRE(labels.none());
  }

  SECTION("One label")
  {
    (*vloc)[P1] = P1_l0;
    (*vloc)[P2] = P2_l0;
    (*vloc)[P3] = P3_l0;

    boost::dynamic_bitset<> labels = tchecker::syncprod::labels(system, *vloc);
    REQUIRE(labels.size() == system.labels_count());
    REQUIRE(labels.count() == 1);
    REQUIRE(labels[a]);
    REQUIRE(!labels[b]);
    REQUIRE(!labels[c]);
    REQUIRE(!labels[d]);
  }

  SECTION("Multiple labels")
  {
    (*vloc)[P1] = P1_l1;
    (*vloc)[P2] = P2_l0;
    (*vloc)[P3] = P3_l0;

    boost::dynamic_bitset<> labels = tchecker::syncprod::labels(system, *vloc);
    REQUIRE(labels.size() == system.labels_count());
    REQUIRE(labels.count() == 2);
    REQUIRE(labels[a]);
    REQUIRE(labels[b]);
    REQUIRE(!labels[c]);
    REQUIRE(!labels[d]);
  }

  SECTION("All labels")
  {
    (*vloc)[P1] = P1_l1;
    (*vloc)[P2] = P2_l0;
    (*vloc)[P3] = P3_l1;

    boost::dynamic_bitset<> labels = tchecker::syncprod::labels(system, *vloc);
    REQUIRE(labels.size() == system.labels_count());
    REQUIRE(labels.count() == 4);
    REQUIRE(labels[a]);
    REQUIRE(labels[b]);
    REQUIRE(labels[c]);
    REQUIRE(labels[d]);
  }

  tchecker::vloc_destruct_and_deallocate(vloc);
}

TEST_CASE("Labels from comma-separated strings", "[labels]")
{
  std::string model = "system:labels \n\
  \n\
  process:P1 \n\
  location:P1:l0{initial:} \n\
  location:P1:l1{initial: : labels: a,b} \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial: : labels: a} \n\
  location:P2:l1{initial:} \n\
  \n\
  process:P3 \n\
  location:P3:l0{initial:} \n\
  location:P3:l1{initial: : labels: b,c,d} \n\
  ";

  std::shared_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  assert(sysdecl != nullptr);

  tchecker::syncprod::system_t system{*sysdecl};

  tchecker::label_id_t const a = system.label_id("a");
  tchecker::label_id_t const b = system.label_id("b");
  tchecker::label_id_t const c = system.label_id("c");
  tchecker::label_id_t const d = system.label_id("d");

  SECTION("No label")
  {
    boost::dynamic_bitset<> labels = system.labels("");
    REQUIRE(labels.none());
  }

  SECTION("One label")
  {
    boost::dynamic_bitset<> labels = system.labels("b");
    REQUIRE(labels.count() == 1);
    REQUIRE(!labels[a]);
    REQUIRE(labels[b]);
    REQUIRE(!labels[c]);
    REQUIRE(!labels[d]);
  }

  SECTION("Multiple label")
  {
    boost::dynamic_bitset<> labels = system.labels("a,b,d");
    REQUIRE(labels.count() == 3);
    REQUIRE(labels[a]);
    REQUIRE(labels[b]);
    REQUIRE(!labels[c]);
    REQUIRE(labels[d]);
  }

  SECTION("All labels")
  {
    boost::dynamic_bitset<> labels = system.labels("c,a,d,b");
    REQUIRE(labels.count() == 4);
    REQUIRE(labels[a]);
    REQUIRE(labels[b]);
    REQUIRE(labels[c]);
    REQUIRE(labels[d]);
  }

  SECTION("Bad label") { REQUIRE_THROWS_AS(system.labels("a,c,s,d"), std::invalid_argument); }
}
