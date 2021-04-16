/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <string>
#include <vector>

#include "tchecker/variables/clocks.hh"

TEST_CASE("translation of clock constraints", "[clocks]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
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
    tchecker::clock_constraint_t c =
        r.translate(tchecker::clock_constraint_t(_x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 1));
    REQUIRE(c.id1() == x);
    REQUIRE(c.id2() == t0);
    REQUIRE(c.comparator() == tchecker::clock_constraint_t::LE);
    REQUIRE(c.value() == 1);
  }

  SECTION("Lower-bound constraint")
  {
    tchecker::clock_constraint_t c =
        r.translate(tchecker::clock_constraint_t(tchecker::REFCLOCK_ID, _z1, tchecker::clock_constraint_t::LE, -3));
    REQUIRE(c.id1() == t2);
    REQUIRE(c.id2() == z1);
    REQUIRE(c.comparator() == tchecker::clock_constraint_t::LE);
    REQUIRE(c.value() == -3);
  }

  SECTION("Diagonal constraint, same reference clock")
  {
    tchecker::clock_constraint_t c = r.translate(tchecker::clock_constraint_t(_z1, _z2, tchecker::clock_constraint_t::LT, 19));
    REQUIRE(c.id1() == z1);
    REQUIRE(c.id2() == z2);
    REQUIRE(c.comparator() == tchecker::clock_constraint_t::LT);
    REQUIRE(c.value() == 19);
  }

  SECTION("Diagonal constraint, distinct reference clocks")
  {
    tchecker::clock_constraint_t c = r.translate(tchecker::clock_constraint_t(_y, _x, tchecker::clock_constraint_t::LE, -5));
    REQUIRE(c.id1() == y);
    REQUIRE(c.id2() == x);
    REQUIRE(c.comparator() == tchecker::clock_constraint_t::LE);
    REQUIRE(c.value() == -5);
  }
}

TEST_CASE("translation of clock resets", "[clocks]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
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