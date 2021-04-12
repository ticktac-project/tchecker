/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <string>
#include <vector>

#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/dbm/refdbm.hh"

/* Tests are provided for functions over DBMs with reference clocks. We do not
 * test functions that are only call the corresponding function over DBMs.
 */

#define DBM(i, j)   dbm[(i)*dim + (j)]
#define DBM2(i, j)  dbm2[(i)*dim + (j)]
#define RDBM(i, j)  rdbm[(i)*rdim + (j)]
#define RDBM1(i, j) rdbm1[(i)*rdim + (j)]
#define RDBM2(i, j) rdbm2[(i)*rdim + (j)]

TEST_CASE("universal", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::refdbm::universal(rdbm, r);
  REQUIRE(tchecker::refdbm::is_universal(rdbm, r));
}

TEST_CASE("universal_positive", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");
  r.declare("z3", "$2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::refdbm::universal_positive(rdbm, r);

  tchecker::clock_id_t const * refmap = r.refmap();
  for (tchecker::clock_id_t i = 0; i < rdim; ++i) {
    for (tchecker::clock_id_t j = 0; j < rdim; ++j) {
      if (i == j || i == refmap[j])
        REQUIRE(RDBM(i, j) == tchecker::dbm::LE_ZERO);
      else
        REQUIRE(RDBM(i, j) == tchecker::dbm::LT_INFINITY);
    }
  }
}

TEST_CASE("empty", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2", "$3"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("z", "$2");
  r.declare("t", "$3");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::refdbm::empty(rdbm, r);
  REQUIRE(tchecker::refdbm::is_empty_0(rdbm, r));
}

TEST_CASE("zero", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("y3", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");
  r.declare("z3", "$2");
  r.declare("z4", "$2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::refdbm::zero(rdbm, r);
  for (tchecker::clock_id_t i = 0; i < rdim; ++i)
    for (tchecker::clock_id_t j = 0; j < rdim; ++j)
      REQUIRE(RDBM(i, j) == tchecker::dbm::LE_ZERO);
}

TEST_CASE("is_empty_0", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];

  SECTION("DBM with negative value at (0,0)")
  {
    RDBM(0, 0) = tchecker::dbm::db(tchecker::dbm::LT, 0);
    REQUIRE(tchecker::refdbm::is_empty_0(rdbm, r));
  }

  SECTION("DBM with non-negative value at (0,0)")
  {
    RDBM(0, 0) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    REQUIRE_FALSE(tchecker::refdbm::is_empty_0(rdbm, r));
  }

  SECTION("Empty DBM")
  {
    tchecker::refdbm::empty(rdbm, r);
    REQUIRE(tchecker::refdbm::is_empty_0(rdbm, r));
  }
}

TEST_CASE("is_universal", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2", "$3"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");
  r.declare("t1", "$3");
  r.declare("t2", "$3");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];

  SECTION("Universal DBM")
  {
    tchecker::refdbm::universal(rdbm, r);
    REQUIRE(tchecker::refdbm::is_universal(rdbm, r));
  }

  SECTION("Positive universal DBM")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    REQUIRE_FALSE(tchecker::refdbm::is_universal(rdbm, r));
  }

  SECTION("Non-universal DBM")
  {
    tchecker::refdbm::empty(rdbm, r);
    REQUIRE_FALSE(tchecker::refdbm::is_universal(rdbm, r));
  }
}

TEST_CASE("is_positive", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];

  SECTION("Universal positive DBM")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    REQUIRE(tchecker::refdbm::is_positive(rdbm, r));
  }

  SECTION("Zero DBM")
  {
    tchecker::refdbm::zero(rdbm, r);
    REQUIRE(tchecker::refdbm::is_positive(rdbm, r));
  }

  SECTION("Universal DBM")
  {
    tchecker::refdbm::universal(rdbm, r);
    REQUIRE_FALSE(tchecker::refdbm::is_positive(rdbm, r));
  }
}

TEST_CASE("is_universal_positive", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("y3", "$1");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];

  SECTION("Universal positive DBM with reference clocks")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    REQUIRE(tchecker::refdbm::is_universal_positive(rdbm, r));
  }

  SECTION("Zero DBM with reference clocks")
  {
    tchecker::refdbm::zero(rdbm, r);
    REQUIRE_FALSE(tchecker::refdbm::is_universal_positive(rdbm, r));
  }

  SECTION("Universal DBM with reference clocks")
  {
    tchecker::refdbm::universal(rdbm, r);
    REQUIRE_FALSE(tchecker::refdbm::is_universal_positive(rdbm, r));
  }
}

TEST_CASE("is_tight", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];

  SECTION("Universal DBM with reference clocks")
  {
    tchecker::refdbm::universal(rdbm, r);
    REQUIRE(tchecker::refdbm::is_tight(rdbm, r));
  }

  SECTION("Non-tight DBM with reference clocks")
  {
    tchecker::refdbm::universal_positive(rdbm, r);

    // Synchronizing reference clocks $0 and $1 make dbm non-tight
    tchecker::clock_id_t const r0 = r.id("$0");
    tchecker::clock_id_t const r1 = r.id("$1");
    RDBM(r0, r1) = tchecker::dbm::LE_ZERO;
    RDBM(r1, r0) = tchecker::dbm::LE_ZERO;

    REQUIRE_FALSE(tchecker::refdbm::is_tight(rdbm, r));
  }
}

TEST_CASE("is_synchronized", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];

  tchecker::clock_id_t const r0 = r.id("$0");
  tchecker::clock_id_t const r1 = r.id("$1");
  tchecker::clock_id_t const r2 = r.id("$2");

  SECTION("Zero DBM with reference clocks")
  {
    tchecker::refdbm::zero(rdbm, r);
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r));
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r, r0, r1));
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r, r1, r0));
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r, r0, r2));
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r, r2, r0));
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r, r1, r2));
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r, r2, r1));
  }

  SECTION("Universal positive DBM with reference clocks")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r));
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r, r0, r1));
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r, r1, r0));
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r, r0, r2));
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r, r2, r0));
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r, r1, r2));
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r, r2, r1));
  }

  SECTION("DBM with reference clocks, 2 reference clocks synchronized")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(r0, r1) = tchecker::dbm::LE_ZERO;
    RDBM(r1, r0) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm, r);

    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r));
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r, r0, r1));
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r, r1, r0));
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r, r0, r2));
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r, r2, r0));
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r, r1, r2));
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r, r2, r1));
  }
}

TEST_CASE("is_equal", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm1[rdim * rdim], rdbm2[rdim * rdim];

  SECTION("Two zero DBM with reference clocks")
  {
    tchecker::refdbm::zero(rdbm1, r);
    tchecker::refdbm::zero(rdbm2, r);
    REQUIRE(tchecker::refdbm::is_equal(rdbm1, rdbm2, r));
  }

  SECTION("Two universal DBM with reference clocks")
  {
    tchecker::refdbm::universal(rdbm1, r);
    tchecker::refdbm::universal(rdbm2, r);
    REQUIRE(tchecker::refdbm::is_equal(rdbm1, rdbm2, r));
  }

  SECTION("Zero DBM with reference clocks vs. universal DBM with reference clocks")
  {
    tchecker::refdbm::zero(rdbm1, r);
    tchecker::refdbm::universal(rdbm2, r);
    REQUIRE_FALSE(tchecker::refdbm::is_equal(rdbm1, rdbm2, r));
  }
}

TEST_CASE("is_le", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("y3", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm_zero[rdim * rdim], rdbm_universal[rdim * rdim], rdbm_universal_positive[rdim * rdim];

  tchecker::refdbm::zero(rdbm_zero, r);
  tchecker::refdbm::universal(rdbm_universal, r);
  tchecker::refdbm::universal_positive(rdbm_universal_positive, r);

  REQUIRE(tchecker::refdbm::is_le(rdbm_zero, rdbm_zero, r));
  REQUIRE(tchecker::refdbm::is_le(rdbm_zero, rdbm_universal, r));
  REQUIRE(tchecker::refdbm::is_le(rdbm_zero, rdbm_universal_positive, r));

  REQUIRE_FALSE(tchecker::refdbm::is_le(rdbm_universal, rdbm_zero, r));
  REQUIRE(tchecker::refdbm::is_le(rdbm_universal, rdbm_universal, r));
  REQUIRE_FALSE(tchecker::refdbm::is_le(rdbm_universal, rdbm_universal_positive, r));

  REQUIRE_FALSE(tchecker::refdbm::is_le(rdbm_universal_positive, rdbm_zero, r));
  REQUIRE(tchecker::refdbm::is_le(rdbm_universal_positive, rdbm_universal, r));
  REQUIRE(tchecker::refdbm::is_le(rdbm_universal_positive, rdbm_universal_positive, r));
}

TEST_CASE("is_alu_le", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x", "$0");
  r.declare("y", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const x = r.id("x");
  tchecker::clock_id_t const y = r.id("y");
  tchecker::clock_id_t const z = r.id("z");
  tchecker::clock_id_t const rx = r.refmap()[x];
  tchecker::clock_id_t const ry = r.refmap()[y];
  tchecker::clock_id_t const rz = r.refmap()[z];

  tchecker::clock_id_t const rdim = r.size();
  tchecker::clock_id_t const refcount = r.refcount();
  tchecker::clock_id_t const offset_dim = rdim - refcount;

  tchecker::dbm::db_t rdbm1[rdim * rdim], rdbm2[rdim * rdim];

  tchecker::integer_t l[offset_dim], u[offset_dim];

  SECTION("Zero DBM is aLU*-subsumed by zero DBM")
  {
    tchecker::refdbm::zero(rdbm1, r);
    tchecker::refdbm::zero(rdbm2, r);

    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = 0;
      u[i] = 0;
    }

    REQUIRE(tchecker::refdbm::is_alu_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("Zero DBM is aLU*-subsumed by universal positive DBM")
  {
    tchecker::refdbm::zero(rdbm1, r);
    tchecker::refdbm::universal_positive(rdbm2, r);

    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = 0;
      u[i] = 0;
    }

    REQUIRE(tchecker::refdbm::is_alu_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("Universal DBM is not aLU*-subsumed by universal positive DBM with 0 clock bounds")
  {
    tchecker::refdbm::universal(rdbm1, r);
    tchecker::refdbm::universal_positive(rdbm2, r);

    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = 0;
      u[i] = 0;
    }

    REQUIRE_FALSE(tchecker::refdbm::is_alu_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("Universal DBM is aLU*-subsumed by universal positive DBM with no clock bounds")
  {
    tchecker::refdbm::universal(rdbm1, r);
    tchecker::refdbm::universal_positive(rdbm2, r);

    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = tchecker::clockbounds::NO_BOUND;
      u[i] = tchecker::clockbounds::NO_BOUND;
    }

    REQUIRE(tchecker::refdbm::is_alu_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("Universal DBM is not aLU*-subsumed by zero DBM with no clock bounds, due to reference clocks")
  {
    tchecker::refdbm::universal(rdbm1, r);
    tchecker::refdbm::zero(rdbm2, r);

    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = tchecker::clockbounds::NO_BOUND;
      u[i] = tchecker::clockbounds::NO_BOUND;
    }

    REQUIRE_FALSE(tchecker::refdbm::is_alu_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("DBM aLU*-subsumed due to no clock bounds")
  {
    // rx == rz && z == rz && 1 <= x - rx < 2 && 1 < y - ry <= 3
    tchecker::refdbm::universal_positive(rdbm1, r);
    RDBM1(rx, rz) = tchecker::dbm::LE_ZERO;
    RDBM1(rz, rx) = tchecker::dbm::LE_ZERO;
    RDBM1(z, rz) = tchecker::dbm::LE_ZERO;
    RDBM1(rz, z) = tchecker::dbm::LE_ZERO;
    RDBM1(rx, x) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM1(x, rx) = tchecker::dbm::db(tchecker::dbm::LT, 2);
    RDBM1(ry, y) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    RDBM1(y, ry) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    tchecker::refdbm::tighten(rdbm1, r);

    // rx = rz && z == rz && x - rx >= 3 && y - ry > 1
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(rx, rz) = tchecker::dbm::LE_ZERO;
    RDBM2(rz, rx) = tchecker::dbm::LE_ZERO;
    RDBM2(z, rz) = tchecker::dbm::LE_ZERO;
    RDBM2(rz, z) = tchecker::dbm::LE_ZERO;
    RDBM2(rx, x) = tchecker::dbm::db(tchecker::dbm::LE, -3);
    RDBM2(ry, y) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    tchecker::refdbm::tighten(rdbm2, r);

    // No clock bounds
    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = tchecker::clockbounds::NO_BOUND;
      u[i] = tchecker::clockbounds::NO_BOUND;
    }

    REQUIRE(tchecker::refdbm::is_alu_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("DBM not aLU*-subsumed due to clock bounds")
  {
    // rx == rz && z == rz && 1 <= x - rx < 2 && 1 < y - ry <= 3
    tchecker::refdbm::universal_positive(rdbm1, r);
    RDBM1(rx, rz) = tchecker::dbm::LE_ZERO;
    RDBM1(rz, rx) = tchecker::dbm::LE_ZERO;
    RDBM1(z, rz) = tchecker::dbm::LE_ZERO;
    RDBM1(rz, z) = tchecker::dbm::LE_ZERO;
    RDBM1(rx, x) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM1(x, rx) = tchecker::dbm::db(tchecker::dbm::LT, 2);
    RDBM1(ry, y) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    RDBM1(y, ry) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    tchecker::refdbm::tighten(rdbm1, r);

    // rx = rz && z == rz && x - rx >= 3 && y - ry > 1
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(rx, rz) = tchecker::dbm::LE_ZERO;
    RDBM2(rz, rx) = tchecker::dbm::LE_ZERO;
    RDBM2(z, rz) = tchecker::dbm::LE_ZERO;
    RDBM2(rz, z) = tchecker::dbm::LE_ZERO;
    RDBM2(rx, x) = tchecker::dbm::db(tchecker::dbm::LE, -3);
    RDBM2(ry, y) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    tchecker::refdbm::tighten(rdbm2, r);

    // Clock bounds
    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = 3;
      u[i] = 3;
    }

    REQUIRE_FALSE(tchecker::refdbm::is_alu_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("DBM aLU*-subsumed")
  {
    // x - rx > 3
    tchecker::refdbm::universal_positive(rdbm1, r);
    RDBM1(rx, x) = tchecker::dbm::db(tchecker::dbm::LT, -3);
    tchecker::refdbm::tighten(rdbm1, r);

    // z - rz < 2 && x > y
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(z, rz) = tchecker::dbm::db(tchecker::dbm::LT, 2);
    RDBM2(y, x) = tchecker::dbm::LT_ZERO;
    tchecker::refdbm::tighten(rdbm2, r);

    // Clock bounds
    l[x - refcount] = 1;
    l[y - refcount] = 2;
    l[z - refcount] = tchecker::clockbounds::NO_BOUND;

    u[x - refcount] = 1;
    u[y - refcount] = 2;
    u[z - refcount] = tchecker::clockbounds::NO_BOUND;

    REQUIRE(tchecker::refdbm::is_alu_le(rdbm1, rdbm2, r, l, u));
  }
}

TEST_CASE("is_sync_alu_le", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x", "$0");
  r.declare("y", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const x = r.id("x");
  tchecker::clock_id_t const y = r.id("y");
  tchecker::clock_id_t const z = r.id("z");
  tchecker::clock_id_t const rx = r.refmap()[x];
  tchecker::clock_id_t const ry = r.refmap()[y];
  tchecker::clock_id_t const rz = r.refmap()[z];

  tchecker::clock_id_t const rdim = r.size();
  tchecker::clock_id_t const refcount = r.refcount();
  tchecker::clock_id_t const offset_dim = rdim - refcount;

  tchecker::dbm::db_t rdbm1[rdim * rdim], rdbm2[rdim * rdim];

  tchecker::integer_t l[offset_dim], u[offset_dim];

  SECTION("Zero DBM is sync-aLU-subsumed by zero DBM")
  {
    tchecker::refdbm::zero(rdbm1, r);
    tchecker::refdbm::zero(rdbm2, r);

    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = 0;
      u[i] = 0;
    }

    REQUIRE(tchecker::refdbm::is_sync_alu_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("Zero DBM is sync-aLU-subsumed by universal positive DBM")
  {
    tchecker::refdbm::zero(rdbm1, r);
    tchecker::refdbm::universal_positive(rdbm2, r);

    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = 0;
      u[i] = 0;
    }

    REQUIRE(tchecker::refdbm::is_sync_alu_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("Universal positive DBM is not sync-aLU-subsumed by zero DBM with 0 clock bounds")
  {
    tchecker::refdbm::universal_positive(rdbm1, r);
    tchecker::refdbm::zero(rdbm2, r);

    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = 0;
      u[i] = 0;
    }

    REQUIRE_FALSE(tchecker::refdbm::is_sync_alu_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("Universal positive DBM is sync-aLU-subsumed by zero DBM with no clock bound")
  {
    tchecker::refdbm::universal_positive(rdbm1, r);
    tchecker::refdbm::zero(rdbm2, r);

    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = tchecker::clockbounds::NO_BOUND;
      u[i] = tchecker::clockbounds::NO_BOUND;
    }

    REQUIRE(tchecker::refdbm::is_sync_alu_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("Some DBM sync-aLU-subsumed")
  {
    // x >= 4 && y >= 2
    tchecker::refdbm::universal_positive(rdbm1, r);
    RDBM1(rx, x) = tchecker::dbm::db(tchecker::dbm::LT, -4);
    RDBM1(ry, y) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    tchecker::refdbm::tighten(rdbm1, r);

    // y >= 1 && x >= y
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(ry, y) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM2(y, x) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm2, r);

    l[x - refcount] = 2;
    l[y - refcount] = 3;
    l[z - refcount] = tchecker::clockbounds::NO_BOUND;

    u[x - refcount] = 2;
    u[y - refcount] = 3;
    u[z - refcount] = tchecker::clockbounds::NO_BOUND;

    REQUIRE(tchecker::refdbm::is_sync_alu_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("Some DBM not sync-aLU-subsumed due to constraint strictness")
  {
    // x >= 4 && y >= 2
    tchecker::refdbm::universal_positive(rdbm1, r);
    RDBM1(rx, x) = tchecker::dbm::db(tchecker::dbm::LT, -4);
    RDBM1(ry, y) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    tchecker::refdbm::tighten(rdbm1, r);

    // y > 2 && x >= y
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(ry, y) = tchecker::dbm::db(tchecker::dbm::LT, -2); // here
    RDBM2(y, x) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm2, r);

    l[x - refcount] = 2;
    l[y - refcount] = 3;
    l[z - refcount] = tchecker::clockbounds::NO_BOUND;

    u[x - refcount] = 2;
    u[y - refcount] = 3;
    u[z - refcount] = tchecker::clockbounds::NO_BOUND;

    REQUIRE_FALSE(tchecker::refdbm::is_sync_alu_le(rdbm1, rdbm2, r, l, u));
  }
}

TEST_CASE("hash", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");

  tchecker::clock_id_t const rdim = r.size();

  SECTION("Zero DBMs have same hash code")
  {
    tchecker::dbm::db_t rdbm1[rdim * rdim], rdbm2[rdim * rdim];
    tchecker::refdbm::zero(rdbm1, r);
    tchecker::refdbm::zero(rdbm2, r);
    REQUIRE(tchecker::refdbm::hash(rdbm1, r) == tchecker::refdbm::hash(rdbm2, r));
  }

  SECTION("Universal DBMs have same hash code")
  {
    tchecker::dbm::db_t rdbm1[rdim * rdim], rdbm2[rdim * rdim];
    tchecker::refdbm::universal(rdbm1, r);
    tchecker::refdbm::universal(rdbm2, r);
    REQUIRE(tchecker::refdbm::hash(rdbm1, r) == tchecker::refdbm::hash(rdbm2, r));
  }

  SECTION("Distinct DBMs have distinct hash codes")
  {
    tchecker::dbm::db_t rdbm1[rdim * rdim], rdbm2[rdim * rdim];
    tchecker::refdbm::zero(rdbm1, r);
    tchecker::refdbm::universal(rdbm2, r);
    REQUIRE(tchecker::refdbm::hash(rdbm1, r) != tchecker::refdbm::hash(rdbm2, r));
  }
}

TEST_CASE("constrain, for reference DBMs", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");

  tchecker::clock_id_t const rdim = r.size();

  tchecker::clock_id_t const r0 = r.id("$0");
  tchecker::clock_id_t const r1 = r.id("$1");
  tchecker::clock_id_t const x = r.id("x");
  tchecker::clock_id_t const y1 = r.id("y1");
  tchecker::clock_id_t const y2 = r.id("y2");

  tchecker::clock_id_t const * refmap = r.refmap();

  SECTION("Constrain with a constrain smaller than DBM")
  {
    tchecker::dbm::db_t rdbm1[rdim * rdim], rdbm2[rdim * rdim];
    tchecker::refdbm::universal(rdbm1, r);
    tchecker::refdbm::universal(rdbm2, r);

    tchecker::dbm::status_t status = tchecker::refdbm::constrain(rdbm1, r, x, refmap[x], tchecker::dbm::LE, 1);

    RDBM2(x, refmap[x]) = tchecker::dbm::db(tchecker::dbm::LE, 1);

    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::refdbm::is_tight(rdbm1, r));
    REQUIRE(tchecker::refdbm::is_equal(rdbm1, rdbm2, r));
  }

  SECTION("Apply several constraints smaller than DBM")
  {
    tchecker::dbm::db_t rdbm1[rdim * rdim], rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm1, r);
    tchecker::refdbm::universal_positive(rdbm2, r);

    tchecker::dbm::status_t status;

    status = tchecker::refdbm::constrain(rdbm1, r, r0, r1, tchecker::dbm::LE, 7);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    status = tchecker::refdbm::constrain(rdbm1, r, r1, x, tchecker::dbm::LT, 4);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    status = tchecker::refdbm::constrain(rdbm1, r, y1, y2, tchecker::dbm::LE, -1);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);

    RDBM2(r0, r1) = tchecker::dbm::db(tchecker::dbm::LE, 7);
    status = tchecker::refdbm::tighten(rdbm2, r);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::refdbm::is_tight(rdbm2, r));

    RDBM2(r1, x) = tchecker::dbm::db(tchecker::dbm::LT, 4);
    status = tchecker::refdbm::tighten(rdbm2, r);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::refdbm::is_tight(rdbm2, r));

    RDBM2(y1, y2) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    status = tchecker::refdbm::tighten(rdbm2, r);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::refdbm::is_tight(rdbm2, r));

    REQUIRE(tchecker::refdbm::is_tight(rdbm1, r));
    REQUIRE(tchecker::refdbm::is_equal(rdbm1, rdbm2, r));
  }

  SECTION("Constrain with a constrain larger than DBM")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal(rdbm, r);

    tchecker::dbm::status_t status = tchecker::refdbm::constrain(rdbm, r, r0, r0, tchecker::dbm::LT, 1);

    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::refdbm::is_tight(rdbm, r));
    REQUIRE(tchecker::refdbm::is_universal(rdbm, r));
  }

  SECTION("Constrain that makes the DBM empty")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);

    tchecker::dbm::status_t status = tchecker::refdbm::constrain(rdbm, r, r1, r1, tchecker::dbm::LT, 0);

    REQUIRE(status == tchecker::dbm::EMPTY);
    REQUIRE(tchecker::refdbm::is_empty_0(rdbm, r));
  }
}

TEST_CASE("synchronize DBMs with reference clocks", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("y3", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const rdim = r.size();

  tchecker::clock_id_t const r0 = r.id("$0");
  tchecker::clock_id_t const r1 = r.id("$1");
  tchecker::clock_id_t const x1 = r.id("x1");
  tchecker::clock_id_t const y1 = r.id("y1");

  SECTION("Synchronizing yields non-empty synchronized DBM")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    auto status = tchecker::refdbm::synchronize(rdbm, r);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r));
  }

  SECTION("Synchronizing yields empty DBM")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(x1, r0) = tchecker::dbm::LE_ZERO;
    RDBM(y1, x1) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM(r1, y1) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm, r);

    auto status = tchecker::refdbm::synchronize(rdbm, r);
    REQUIRE(status == tchecker::dbm::EMPTY);
  }
}

TEST_CASE("Reset to reference clock on DBMs with reference clocks", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");

  tchecker::clock_id_t const rdim = r.size();

  tchecker::clock_id_t const x = r.id("x");
  tchecker::clock_id_t const rx = r.refmap()[x];
  tchecker::clock_id_t const y1 = r.id("y1");
  tchecker::clock_id_t const ry1 = r.refmap()[y1];
  tchecker::clock_id_t const z1 = r.id("z1");
  tchecker::clock_id_t const rz1 = r.refmap()[z1];
  tchecker::clock_id_t const z2 = r.id("z2");

  SECTION("reset one clock to reference clock on universal positive DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);

    tchecker::refdbm::reset_to_reference_clock(rdbm, r, x);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(x, rx) = tchecker::dbm::LE_ZERO;

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("reset two clocks to reference clock on universal positive DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);

    tchecker::refdbm::reset_to_reference_clock(rdbm, r, x);
    tchecker::refdbm::reset_to_reference_clock(rdbm, r, y1);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(x, rx) = tchecker::dbm::LE_ZERO;
    RDBM2(y1, ry1) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("reset to reference clock on DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(rz1, z1) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    RDBM(z1, rz1) = tchecker::dbm::db(tchecker::dbm::LE, 5);
    tchecker::refdbm::tighten(rdbm, r);

    tchecker::refdbm::reset_to_reference_clock(rdbm, r, z1);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(z1, rz1) = tchecker::dbm::LE_ZERO;
    RDBM2(z1, z2) = tchecker::dbm::LE_ZERO; // tightening as rz1 = rz2

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("reset to reference clock on synchronized DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    tchecker::refdbm::synchronize(rdbm, r);
    RDBM(ry1, y1) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    RDBM(y1, ry1) = tchecker::dbm::db(tchecker::dbm::LE, 5);
    tchecker::refdbm::tighten(rdbm, r);

    tchecker::refdbm::reset_to_reference_clock(rdbm, r, y1);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm2, r);
    tchecker::refdbm::synchronize(rdbm2, r);
    for (tchecker::clock_id_t j = 0; j < rdim; ++j)
      RDBM2(y1, j) = tchecker::dbm::LE_ZERO; // reset of x + tightening

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }
}

TEST_CASE("asynchronous_open_up on DBMs with reference clocks", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x", "$0");
  r.declare("y", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");

  tchecker::clock_id_t const r0 = r.id("$0");
  tchecker::clock_id_t const r1 = r.id("$1");
  tchecker::clock_id_t const r2 = r.id("$2");
  tchecker::clock_id_t const x = r.id("x");
  tchecker::clock_id_t const y = r.id("y");
  tchecker::clock_id_t const z1 = r.id("z1");
  tchecker::clock_id_t const z2 = r.id("z2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::clock_id_t const refcount = r.refcount();

  SECTION("asynchronous_open_up on universal positive DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    tchecker::refdbm::asynchronous_open_up(rdbm, r);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("asynchronous_open_up on synchronized universal positive DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    tchecker::refdbm::synchronize(rdbm, r);
    tchecker::refdbm::asynchronous_open_up(rdbm, r);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm2, r);
    for (tchecker::clock_id_t t = 0; t < refcount; ++t)
      for (tchecker::clock_id_t x = refcount; x < rdim; ++x)
        RDBM2(t, x) = tchecker::dbm::LE_ZERO;

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("asynchronous_open_up on DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(r0, r1) = tchecker::dbm::LE_ZERO;
    RDBM(r1, r0) = tchecker::dbm::LE_ZERO;
    RDBM(r1, y) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM(r2, r1) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    RDBM(y, r1) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    RDBM(y, x) = tchecker::dbm::db(tchecker::dbm::LE, 8);
    RDBM(z1, z2) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM(z2, z1) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    tchecker::refdbm::tighten(rdbm, r);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    std::memcpy(rdbm2, rdbm, rdim * rdim * sizeof(*rdbm2));
    for (tchecker::clock_id_t t = 0; t < refcount; ++t)
      for (tchecker::clock_id_t x = 0; x < rdim; ++x)
        RDBM2(x, t) = (x == t ? tchecker::dbm::LE_ZERO : tchecker::dbm::LT_INFINITY);

    tchecker::refdbm::asynchronous_open_up(rdbm, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("asynchronous_open_up on DBM with reference clocks, partial delay allowed")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(r0, r1) = tchecker::dbm::LE_ZERO;
    RDBM(r0, x) = tchecker::dbm::LE_ZERO;
    RDBM(r1, r0) = tchecker::dbm::LE_ZERO;
    RDBM(r1, y) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    RDBM(r2, r1) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    RDBM(r2, z1) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    RDBM(y, r1) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    RDBM(y, x) = tchecker::dbm::db(tchecker::dbm::LE, 8);
    RDBM(z1, z2) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM(z2, z1) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    tchecker::refdbm::tighten(rdbm, r);

    boost::dynamic_bitset<> delay_allowed(refcount, 0);
    delay_allowed[r1] = 1;

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    std::memcpy(rdbm2, rdbm, rdim * rdim * sizeof(*rdbm2));
    for (tchecker::clock_id_t x = 0; x < rdim; ++x)
      RDBM2(x, r1) = (x == r1 ? tchecker::dbm::LE_ZERO : tchecker::dbm::LT_INFINITY);

    tchecker::refdbm::asynchronous_open_up(rdbm, r, delay_allowed);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }
}

TEST_CASE("to_dbm on DBMs with reference clocks", "[refdbm]")
{
  std::vector<std::string> refclocks = {"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks.begin(), refclocks.end());
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const r1 = r.id("$1");
  tchecker::clock_id_t const r2 = r.id("$2");
  tchecker::clock_id_t const x1 = r.id("x1");
  tchecker::clock_id_t const x2 = r.id("x2");
  tchecker::clock_id_t const y = r.id("y");
  tchecker::clock_id_t const z = r.id("z");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::clock_id_t const refcount = r.refcount();

  tchecker::clock_id_t const dim = rdim - refcount + 1;
  tchecker::clock_id_t const _x1 = x1 - refcount + 1;
  tchecker::clock_id_t const _x2 = x2 - refcount + 1;
  tchecker::clock_id_t const _y = y - refcount + 1;
  tchecker::clock_id_t const _z = z - refcount + 1;

  SECTION("synchronized universal positive DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    tchecker::refdbm::synchronize(rdbm, r);

    tchecker::dbm::db_t dbm[dim * dim];
    tchecker::refdbm::to_dbm(rdbm, r, dbm, dim);

    REQUIRE(tchecker::dbm::is_universal_positive(dbm, dim));
  }

  SECTION("synchronized offset DBM, dimension > refcount")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(r1, y) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    RDBM(r2, z) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM(x1, x2) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    RDBM(x2, x1) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    RDBM(z, y) = tchecker::dbm::db(tchecker::dbm::LE, 7);
    tchecker::refdbm::tighten(rdbm, r);
    tchecker::refdbm::synchronize(rdbm, r);

    tchecker::dbm::db_t dbm[dim * dim];
    tchecker::refdbm::to_dbm(rdbm, r, dbm, dim);

    tchecker::dbm::db_t dbm2[dim * dim];
    DBM2(0, 0) = tchecker::dbm::LE_ZERO;
    DBM2(0, _x1) = tchecker::dbm::LE_ZERO;
    DBM2(0, _x2) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    DBM2(0, _y) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    DBM2(0, _z) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    DBM2(_x1, 0) = tchecker::dbm::LT_INFINITY;
    DBM2(_x1, _x1) = tchecker::dbm::LE_ZERO;
    DBM2(_x1, _x2) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    DBM2(_x1, _y) = tchecker::dbm::LT_INFINITY;
    DBM2(_x1, _z) = tchecker::dbm::LT_INFINITY;
    DBM2(_x2, 0) = tchecker::dbm::LT_INFINITY;
    DBM2(_x2, _x1) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    DBM2(_x2, _x2) = tchecker::dbm::LE_ZERO;
    DBM2(_x2, _y) = tchecker::dbm::LT_INFINITY;
    DBM2(_x2, _z) = tchecker::dbm::LT_INFINITY;
    DBM2(_y, 0) = tchecker::dbm::LT_INFINITY;
    DBM2(_y, _x1) = tchecker::dbm::LT_INFINITY;
    DBM2(_y, _x2) = tchecker::dbm::LT_INFINITY;
    DBM2(_y, _y) = tchecker::dbm::LE_ZERO;
    DBM2(_y, _z) = tchecker::dbm::LT_INFINITY;
    DBM2(_z, 0) = tchecker::dbm::LT_INFINITY;
    DBM2(_z, _x1) = tchecker::dbm::LT_INFINITY;
    DBM2(_z, _x2) = tchecker::dbm::LT_INFINITY;
    DBM2(_z, _y) = tchecker::dbm::db(tchecker::dbm::LE, 7);
    DBM2(_z, _z) = tchecker::dbm::LE_ZERO;

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
}
