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
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
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
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");
  r.declare("z3", "$2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::refdbm::universal_positive(rdbm, r);

  std::vector<tchecker::clock_id_t> const & refmap = r.refmap();
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
  std::vector<std::string> refclocks{"$0", "$1", "$2", "$3"};
  tchecker::reference_clock_variables_t r(refclocks);
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
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
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
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
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
  std::vector<std::string> refclocks{"$0", "$1", "$2", "$3"};
  tchecker::reference_clock_variables_t r(refclocks);
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
  std::vector<std::string> refclocks{"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks);
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
  std::vector<std::string> refclocks{"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks);
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

TEST_CASE("is_open_up", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("z1", "$2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];

  tchecker::clock_id_t const x1 = r.id("x1");
  tchecker::clock_id_t const tx1 = r.refmap()[x1];
  tchecker::clock_id_t const y2 = r.id("y2");
  tchecker::clock_id_t const ty2 = r.refmap()[y2];
  tchecker::clock_id_t const z1 = r.id("z1");
  tchecker::clock_id_t const tz1 = r.refmap()[z1];

  SECTION("Universal positive DBM is open up")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    REQUIRE(tchecker::refdbm::is_open_up(rdbm, r));
  }

  SECTION("Zero DBM is not open up")
  {
    tchecker::refdbm::zero(rdbm, r);
    REQUIRE_FALSE(tchecker::refdbm::is_open_up(rdbm, r));
  }

  SECTION("DBM with lower bounds is open up")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(tx1, x1) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    RDBM(ty2, y2) = tchecker::dbm::db(tchecker::dbm::LE, -6);
    RDBM(x1, z1) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    tchecker::refdbm::tighten(rdbm, r);

    REQUIRE(tchecker::refdbm::is_open_up(rdbm, r));
  }

  SECTION("DBM with upper bounds on offset clocks and reference clocks")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(y2, ty2) = tchecker::dbm::db(tchecker::dbm::LT, 4);
    tchecker::refdbm::tighten(rdbm, r);

    REQUIRE_FALSE(tchecker::refdbm::is_open_up(rdbm, r));
  }

  SECTION("DBM with upper bounds on reference clocks")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(tz1, ty2) = tchecker::dbm::db(tchecker::dbm::LT, 1);
    tchecker::refdbm::tighten(rdbm, r);

    REQUIRE_FALSE(tchecker::refdbm::is_open_up(rdbm, r));
  }

  SECTION("DBM with upper bounds on offset clocks")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(z1, y2) = tchecker::dbm::db(tchecker::dbm::LT, 1);
    tchecker::refdbm::tighten(rdbm, r);

    REQUIRE(tchecker::refdbm::is_open_up(rdbm, r));
  }
}

TEST_CASE("is_tight", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
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
    tchecker::clock_id_t const t0 = r.id("$0");
    tchecker::clock_id_t const t1 = r.id("$1");
    RDBM(t0, t1) = tchecker::dbm::LE_ZERO;
    RDBM(t1, t0) = tchecker::dbm::LE_ZERO;

    REQUIRE_FALSE(tchecker::refdbm::is_tight(rdbm, r));
  }
}

TEST_CASE("is_synchronized", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const t2 = r.id("$2");

  boost::dynamic_bitset<> sync_ref_clocks{r.refcount()};

  SECTION("Zero DBM with reference clocks, check all sync")
  {
    tchecker::refdbm::zero(rdbm, r);
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r));
  }

  SECTION("Zero DBM with reference clocks, check 1 ref clock sync")
  {
    tchecker::refdbm::zero(rdbm, r);
    sync_ref_clocks.reset();
    sync_ref_clocks.set(t0);
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r, sync_ref_clocks));
  }

  SECTION("Zero DBM with reference clocks, check two ref clock sync")
  {
    tchecker::refdbm::zero(rdbm, r);
    sync_ref_clocks.reset();
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r, sync_ref_clocks));
  }

  SECTION("Universal positive DBM with reference clocks, check all ref clocks sync")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r));
  }

  SECTION("Universal positive DBM with reference clocks, check 1 ref clocks sync")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    sync_ref_clocks.reset();
    sync_ref_clocks.set(t2);
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r, sync_ref_clocks));
  }

  SECTION("Universal positive DBM with reference clocks, check two ref clocks not sync")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    sync_ref_clocks.reset();
    sync_ref_clocks.set(t1);
    sync_ref_clocks.set(t2);
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r, sync_ref_clocks));
  }

  SECTION("DBM with reference clocks, 2 reference clocks synchronized, check all not sync")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(t0, t1) = tchecker::dbm::LE_ZERO;
    RDBM(t1, t0) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm, r);
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r));
  }

  SECTION("DBM with reference clocks, 2 reference clocks synchronized, check one ref sync")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(t0, t1) = tchecker::dbm::LE_ZERO;
    RDBM(t1, t0) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm, r);
    sync_ref_clocks.reset();
    sync_ref_clocks.set(t1);
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r, sync_ref_clocks));
  }

  SECTION("DBM with reference clocks, 2 reference clocks synchronized, check two ref sync")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(t0, t1) = tchecker::dbm::LE_ZERO;
    RDBM(t1, t0) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm, r);
    sync_ref_clocks.reset();
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);
    REQUIRE(tchecker::refdbm::is_synchronized(rdbm, r, sync_ref_clocks));
  }

  SECTION("DBM with reference clocks, 2 reference clocks synchronized, check two ref not sync")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(t0, t1) = tchecker::dbm::LE_ZERO;
    RDBM(t1, t0) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm, r);
    sync_ref_clocks.reset();
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t2);
    REQUIRE_FALSE(tchecker::refdbm::is_synchronized(rdbm, r, sync_ref_clocks));
  }
}

TEST_CASE("is_synchronizable", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::dbm::db_t rdbm[rdim * rdim];

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const t2 = r.id("$2");
  tchecker::clock_id_t const x1 = r.id("x1");
  tchecker::clock_id_t const x2 = r.id("x2");
  tchecker::clock_id_t const y = r.id("y");
  tchecker::clock_id_t const z1 = r.id("z1");
  tchecker::clock_id_t const z2 = r.id("z2");

  SECTION("Zero DBM with reference clocks is synchronizable")
  {
    tchecker::refdbm::zero(rdbm, r);
    REQUIRE(tchecker::refdbm::is_synchronizable(rdbm, r));
  }

  SECTION("Universal positive DBM with reference clocks is synchronizable")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    REQUIRE(tchecker::refdbm::is_synchronizable(rdbm, r));
  }

  SECTION("Some synchronizable DBM with reference clocks")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(x1, x2) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    RDBM(x1, y) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    RDBM(z1, z2) = tchecker::dbm::db(tchecker::dbm::LE, 5);
    tchecker::refdbm::tighten(rdbm, r);
    REQUIRE_FALSE(tchecker::refdbm::is_empty_0(rdbm, r));
    REQUIRE(tchecker::refdbm::is_synchronizable(rdbm, r));
  }

  SECTION("Some non-synchronizable DBM with reference clocks")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(t1, t2) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    tchecker::refdbm::tighten(rdbm, r);
    REQUIRE_FALSE(tchecker::refdbm::is_empty_0(rdbm, r));
    REQUIRE_FALSE(tchecker::refdbm::is_synchronizable(rdbm, r));
  }

  SECTION("Some, non-trivial, non-synchronizable DBM with reference clocks")
  {
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(x1, t0) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    RDBM(y, x1) = tchecker::dbm::db(tchecker::dbm::LT, -5);
    RDBM(t1, y) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    RDBM(t1, t0) = tchecker::dbm::db(tchecker::dbm::LE, 4);
    tchecker::refdbm::tighten(rdbm, r);
    REQUIRE_FALSE(tchecker::refdbm::is_empty_0(rdbm, r));
    REQUIRE_FALSE(tchecker::refdbm::is_synchronizable(rdbm, r));
  }
}

TEST_CASE("is_equal", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
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
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
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

TEST_CASE("is_alu_star_le", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const x = r.id("x");
  tchecker::clock_id_t const y = r.id("y");
  tchecker::clock_id_t const z = r.id("z");
  tchecker::clock_id_t const tx = r.refmap()[x];
  tchecker::clock_id_t const ty = r.refmap()[y];
  tchecker::clock_id_t const tz = r.refmap()[z];

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

    REQUIRE(tchecker::refdbm::is_alu_star_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("Zero DBM is aLU*-subsumed by universal positive DBM")
  {
    tchecker::refdbm::zero(rdbm1, r);
    tchecker::refdbm::universal_positive(rdbm2, r);

    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = 0;
      u[i] = 0;
    }

    REQUIRE(tchecker::refdbm::is_alu_star_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("Universal DBM is not aLU*-subsumed by universal positive DBM with 0 clock bounds")
  {
    tchecker::refdbm::universal(rdbm1, r);
    tchecker::refdbm::universal_positive(rdbm2, r);

    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = 0;
      u[i] = 0;
    }

    REQUIRE_FALSE(tchecker::refdbm::is_alu_star_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("Universal DBM is aLU*-subsumed by universal positive DBM with no clock bounds")
  {
    tchecker::refdbm::universal(rdbm1, r);
    tchecker::refdbm::universal_positive(rdbm2, r);

    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = tchecker::clockbounds::NO_BOUND;
      u[i] = tchecker::clockbounds::NO_BOUND;
    }

    REQUIRE(tchecker::refdbm::is_alu_star_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("Universal DBM is not aLU*-subsumed by zero DBM with no clock bounds, due to reference clocks")
  {
    tchecker::refdbm::universal(rdbm1, r);
    tchecker::refdbm::zero(rdbm2, r);

    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = tchecker::clockbounds::NO_BOUND;
      u[i] = tchecker::clockbounds::NO_BOUND;
    }

    REQUIRE_FALSE(tchecker::refdbm::is_alu_star_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("DBM aLU*-subsumed due to no clock bounds")
  {
    // rx == rz && z == rz && 1 <= x - rx < 2 && 1 < y - ry <= 3
    tchecker::refdbm::universal_positive(rdbm1, r);
    RDBM1(tx, tz) = tchecker::dbm::LE_ZERO;
    RDBM1(tz, tx) = tchecker::dbm::LE_ZERO;
    RDBM1(z, tz) = tchecker::dbm::LE_ZERO;
    RDBM1(tz, z) = tchecker::dbm::LE_ZERO;
    RDBM1(tx, x) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM1(x, tx) = tchecker::dbm::db(tchecker::dbm::LT, 2);
    RDBM1(ty, y) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    RDBM1(y, ty) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    tchecker::refdbm::tighten(rdbm1, r);

    // rx = rz && z == rz && x - rx >= 3 && y - ry > 1
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(tx, tz) = tchecker::dbm::LE_ZERO;
    RDBM2(tz, tx) = tchecker::dbm::LE_ZERO;
    RDBM2(z, tz) = tchecker::dbm::LE_ZERO;
    RDBM2(tz, z) = tchecker::dbm::LE_ZERO;
    RDBM2(tx, x) = tchecker::dbm::db(tchecker::dbm::LE, -3);
    RDBM2(ty, y) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    tchecker::refdbm::tighten(rdbm2, r);

    // No clock bounds
    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = tchecker::clockbounds::NO_BOUND;
      u[i] = tchecker::clockbounds::NO_BOUND;
    }

    REQUIRE(tchecker::refdbm::is_alu_star_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("DBM not aLU*-subsumed due to clock bounds")
  {
    // rx == rz && z == rz && 1 <= x - rx < 2 && 1 < y - ry <= 3
    tchecker::refdbm::universal_positive(rdbm1, r);
    RDBM1(tx, tz) = tchecker::dbm::LE_ZERO;
    RDBM1(tz, tx) = tchecker::dbm::LE_ZERO;
    RDBM1(z, tz) = tchecker::dbm::LE_ZERO;
    RDBM1(tz, z) = tchecker::dbm::LE_ZERO;
    RDBM1(tx, x) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM1(x, tx) = tchecker::dbm::db(tchecker::dbm::LT, 2);
    RDBM1(ty, y) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    RDBM1(y, ty) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    tchecker::refdbm::tighten(rdbm1, r);

    // rx = rz && z == rz && x - rx >= 3 && y - ry > 1
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(tx, tz) = tchecker::dbm::LE_ZERO;
    RDBM2(tz, tx) = tchecker::dbm::LE_ZERO;
    RDBM2(z, tz) = tchecker::dbm::LE_ZERO;
    RDBM2(tz, z) = tchecker::dbm::LE_ZERO;
    RDBM2(tx, x) = tchecker::dbm::db(tchecker::dbm::LE, -3);
    RDBM2(ty, y) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    tchecker::refdbm::tighten(rdbm2, r);

    // Clock bounds
    for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
      l[i] = 3;
      u[i] = 3;
    }

    REQUIRE_FALSE(tchecker::refdbm::is_alu_star_le(rdbm1, rdbm2, r, l, u));
  }

  SECTION("DBM aLU*-subsumed")
  {
    // x - rx > 3
    tchecker::refdbm::universal_positive(rdbm1, r);
    RDBM1(tx, x) = tchecker::dbm::db(tchecker::dbm::LT, -3);
    tchecker::refdbm::tighten(rdbm1, r);

    // z - rz < 2 && x > y
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(z, tz) = tchecker::dbm::db(tchecker::dbm::LT, 2);
    RDBM2(y, x) = tchecker::dbm::LT_ZERO;
    tchecker::refdbm::tighten(rdbm2, r);

    // Clock bounds
    l[x - refcount] = 1;
    l[y - refcount] = 2;
    l[z - refcount] = tchecker::clockbounds::NO_BOUND;

    u[x - refcount] = 1;
    u[y - refcount] = 2;
    u[z - refcount] = tchecker::clockbounds::NO_BOUND;

    REQUIRE(tchecker::refdbm::is_alu_star_le(rdbm1, rdbm2, r, l, u));
  }
}

TEST_CASE("is_sync_alu_le", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const x = r.id("x");
  tchecker::clock_id_t const y = r.id("y");
  tchecker::clock_id_t const z = r.id("z");
  tchecker::clock_id_t const tx = r.refmap()[x];
  tchecker::clock_id_t const ty = r.refmap()[y];

  tchecker::clock_id_t const rdim = r.size();
  tchecker::clock_id_t const refcount = r.refcount();
  tchecker::clock_id_t const offset_dim = rdim - refcount;

  tchecker::dbm::db_t rdbm1[rdim * rdim], rdbm2[rdim * rdim];

  tchecker::integer_t l[offset_dim], u[offset_dim];

  SECTION("Some DBM sync-aLU-subsumed")
  {
    // x >= 4 && y >= 2
    tchecker::refdbm::universal_positive(rdbm1, r);
    RDBM1(tx, x) = tchecker::dbm::db(tchecker::dbm::LT, -4);
    RDBM1(ty, y) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    tchecker::refdbm::tighten(rdbm1, r);

    // y >= 1 && x >= y
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(ty, y) = tchecker::dbm::db(tchecker::dbm::LE, -1);
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
    RDBM1(tx, x) = tchecker::dbm::db(tchecker::dbm::LT, -4);
    RDBM1(ty, y) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    tchecker::refdbm::tighten(rdbm1, r);

    // y > 2 && x >= y
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(ty, y) = tchecker::dbm::db(tchecker::dbm::LT, -2); // here
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

TEST_CASE("aLU* vs. sync-aLU vs. time-elapse-aLU*", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x1", "$0");
  r.declare("x2", "$1");

  tchecker::clock_id_t const x1 = r.id("x1");
  tchecker::clock_id_t const x2 = r.id("x2");
  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const t2 = r.id("$2");

  tchecker::clock_id_t const rdim = r.size();
  tchecker::clock_id_t const refcount = r.refcount();
  tchecker::clock_id_t const offset_dim = rdim - refcount;

  tchecker::dbm::db_t rdbm1[rdim * rdim], rdbm2[rdim * rdim];

  tchecker::integer_t l[offset_dim], u[offset_dim];
  l[x1 - refcount] = tchecker::clockbounds::NO_BOUND;
  l[x2 - refcount] = tchecker::clockbounds::NO_BOUND;
  u[x1 - refcount] = 10;
  u[x2 - refcount] = 10;

  tchecker::dbm::db_t le_10 = tchecker::dbm::db(tchecker::dbm::LE, 10);

  SECTION("Test case inspired from Fischer model")
  {
    //       $0    $1    $2   $x1   $x2
    // $0         <=10        <=0   <=0
    // $1                           <=0
    // $2   <=10  <=10        <=0   <=0
    // $x1  <=10  <=10              <=0
    // $x2        <=10
    tchecker::refdbm::universal(rdbm1, r);
    RDBM1(t0, t1) = le_10;
    RDBM1(t0, x1) = tchecker::dbm::LE_ZERO;
    RDBM1(t0, x2) = tchecker::dbm::LE_ZERO;
    RDBM1(t1, x2) = tchecker::dbm::LE_ZERO;
    RDBM1(t2, t0) = le_10;
    RDBM1(t2, t1) = le_10;
    RDBM1(t2, x1) = tchecker::dbm::LE_ZERO;
    RDBM1(t2, x2) = tchecker::dbm::LE_ZERO;
    RDBM1(x1, t0) = le_10;
    RDBM1(x1, t1) = le_10;
    RDBM1(x1, x2) = tchecker::dbm::LE_ZERO;
    RDBM1(x2, t1) = le_10;
    tchecker::refdbm::tighten(rdbm1, r);

    //      $0   $1   $2   $x1   $x2
    // $0                  <=0
    // $1   <=10           <=0    <=0
    // $2   <=10 <=10      <=0    <=0
    // $x1  <=10
    // $x2  <=10 <=10      <=0
    tchecker::refdbm::universal(rdbm2, r);
    RDBM2(t0, x1) = tchecker::dbm::LE_ZERO;
    RDBM2(t1, t0) = le_10;
    RDBM2(t1, x1) = tchecker::dbm::LE_ZERO;
    RDBM2(t1, x2) = tchecker::dbm::LE_ZERO;
    RDBM2(t2, t0) = le_10;
    RDBM2(t2, t1) = le_10;
    RDBM2(t2, x1) = tchecker::dbm::LE_ZERO;
    RDBM2(t2, x2) = tchecker::dbm::LE_ZERO;
    RDBM2(x1, t0) = le_10;
    RDBM2(x2, t0) = le_10;
    RDBM2(x2, t1) = le_10;
    RDBM2(x2, x1) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm2, r);

    REQUIRE_FALSE(tchecker::refdbm::is_alu_star_le(rdbm1, rdbm2, r, l, u));
    REQUIRE_FALSE(tchecker::refdbm::is_alu_star_le(rdbm2, rdbm1, r, l, u));

    REQUIRE_FALSE(tchecker::refdbm::is_time_elapse_alu_star_le(rdbm1, rdbm2, r, l, u));
    REQUIRE_FALSE(tchecker::refdbm::is_time_elapse_alu_star_le(rdbm2, rdbm1, r, l, u));

    REQUIRE(tchecker::refdbm::is_sync_alu_le(rdbm1, rdbm2, r, l, u));
    REQUIRE(tchecker::refdbm::is_sync_alu_le(rdbm2, rdbm1, r, l, u));
  }

  SECTION("DBMs that only differ in the reference clocks part")
  {
    //       $0    $1    $2   $x1   $x2
    // $0         <=10
    // $1
    // $2
    // $x1
    // $x2
    tchecker::refdbm::universal(rdbm1, r);
    RDBM1(t0, t1) = le_10;
    tchecker::refdbm::tighten(rdbm1, r);

    //      $0   $1   $2   $x1   $x2
    // $0
    // $1   <=10
    // $2
    // $x1
    // $x2
    tchecker::refdbm::universal(rdbm2, r);
    RDBM2(t1, t0) = le_10;
    tchecker::refdbm::tighten(rdbm2, r);

    REQUIRE_FALSE(tchecker::refdbm::is_alu_star_le(rdbm1, rdbm2, r, l, u));
    REQUIRE_FALSE(tchecker::refdbm::is_alu_star_le(rdbm2, rdbm1, r, l, u));

    REQUIRE(tchecker::refdbm::is_time_elapse_alu_star_le(rdbm1, rdbm2, r, l, u));
    REQUIRE(tchecker::refdbm::is_time_elapse_alu_star_le(rdbm2, rdbm1, r, l, u));

    // NB: cannot check sync-aLU on this example since zones are not positive
  }
}

TEST_CASE("hash", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
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
  std::vector<std::string> refclocks{"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");

  tchecker::clock_id_t const rdim = r.size();

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const x = r.id("x");
  tchecker::clock_id_t const y1 = r.id("y1");
  tchecker::clock_id_t const y2 = r.id("y2");

  std::vector<tchecker::clock_id_t> const & refmap = r.refmap();

  tchecker::clock_id_t const _x = 0;
  tchecker::clock_id_t const _y1 = 1;
  tchecker::clock_id_t const _y2 = 2;

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

    status = tchecker::refdbm::constrain(rdbm1, r, t0, t1, tchecker::dbm::LE, 7);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    status = tchecker::refdbm::constrain(rdbm1, r, t1, x, tchecker::dbm::LT, 4);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    status = tchecker::refdbm::constrain(rdbm1, r, y1, y2, tchecker::dbm::LE, -1);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);

    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::dbm::LE, 7);
    status = tchecker::refdbm::tighten(rdbm2, r);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::refdbm::is_tight(rdbm2, r));

    RDBM2(t1, x) = tchecker::dbm::db(tchecker::dbm::LT, 4);
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

    tchecker::dbm::status_t status = tchecker::refdbm::constrain(rdbm, r, t0, t0, tchecker::dbm::LT, 1);

    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::refdbm::is_tight(rdbm, r));
    REQUIRE(tchecker::refdbm::is_universal(rdbm, r));
  }

  SECTION("Constrain that makes the DBM empty")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);

    tchecker::dbm::status_t status = tchecker::refdbm::constrain(rdbm, r, t1, t1, tchecker::dbm::LT, 0);

    REQUIRE(status == tchecker::dbm::EMPTY);
    REQUIRE(tchecker::refdbm::is_empty_0(rdbm, r));
  }

  SECTION("Constrain w.r.t. clock constraint over system clocks")
  {
    tchecker::dbm::db_t rdbm1[rdim * rdim], rdbm2[rdim * rdim];
    tchecker::refdbm::universal(rdbm1, r);
    tchecker::refdbm::universal(rdbm2, r);

    tchecker::clock_constraint_t c{_x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 3};
    tchecker::dbm::status_t status = tchecker::refdbm::constrain(rdbm1, r, c);

    RDBM2(x, refmap[x]) = tchecker::dbm::db(tchecker::dbm::LT, 3);

    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::refdbm::is_tight(rdbm1, r));
    REQUIRE(tchecker::refdbm::is_equal(rdbm1, rdbm2, r));
  }

  SECTION("Constrain w.r.t. collection of clock constraints over system clocks")
  {
    tchecker::dbm::db_t rdbm1[rdim * rdim], rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm1, r);
    tchecker::refdbm::universal_positive(rdbm2, r);

    tchecker::clock_constraint_container_t cc;
    cc.emplace_back(_x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 4);
    cc.emplace_back(_y1, _y2, tchecker::clock_constraint_t::LE, 1);
    cc.emplace_back(tchecker::REFCLOCK_ID, _y1, tchecker::clock_constraint_t::LT, -2);

    tchecker::dbm::status_t status = tchecker::refdbm::constrain(rdbm1, r, cc);

    RDBM2(x, refmap[x]) = tchecker::dbm::db(tchecker::dbm::LE, 4);
    status = tchecker::refdbm::tighten(rdbm2, r);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::refdbm::is_tight(rdbm2, r));

    RDBM2(y1, y2) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    status = tchecker::refdbm::tighten(rdbm2, r);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::refdbm::is_tight(rdbm2, r));

    RDBM2(refmap[y1], y1) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    status = tchecker::refdbm::tighten(rdbm2, r);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::refdbm::is_tight(rdbm2, r));

    REQUIRE(tchecker::refdbm::is_tight(rdbm1, r));
    REQUIRE(tchecker::refdbm::is_equal(rdbm1, rdbm2, r));
  }
}

TEST_CASE("synchronize DBMs with reference clocks", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("y3", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const rdim = r.size();

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
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
    RDBM(x1, t0) = tchecker::dbm::LE_ZERO;
    RDBM(y1, x1) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM(t1, y1) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm, r);

    auto status = tchecker::refdbm::synchronize(rdbm, r);
    REQUIRE(status == tchecker::dbm::EMPTY);
  }
}

TEST_CASE("synchronize DBMs with reference clocks, partial synchronization", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("y3", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const rdim = r.size();

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const t2 = r.id("$2");
  tchecker::clock_id_t const x1 = r.id("x1");
  tchecker::clock_id_t const y1 = r.id("y1");

  boost::dynamic_bitset<> sync_ref_clocks{r.refcount()};

  SECTION("Synchronizing no clock leaves the DBM unchanged")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);

    sync_ref_clocks.reset();
    auto status = tchecker::refdbm::synchronize(rdbm, r, sync_ref_clocks);

    REQUIRE(status == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::refdbm::is_universal_positive(rdbm, r));
  }

  SECTION("Synchronizing yields empty DBM")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(x1, t0) = tchecker::dbm::LE_ZERO;
    RDBM(y1, x1) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM(t1, y1) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm, r);

    sync_ref_clocks.reset();
    sync_ref_clocks[t0] = 1;
    sync_ref_clocks[t1] = 1;

    auto status = tchecker::refdbm::synchronize(rdbm, r, sync_ref_clocks);
    REQUIRE(status == tchecker::dbm::EMPTY);
  }

  SECTION("Synchronizing yields non-empty DBM")
  {
    tchecker::dbm::db_t rdbm1[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm1, r);
    RDBM1(x1, t0) = tchecker::dbm::LE_ZERO;
    RDBM1(y1, x1) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM1(t1, y1) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm1, r);

    sync_ref_clocks.reset();
    sync_ref_clocks[t0] = 1;
    sync_ref_clocks[t2] = 1;

    auto status = tchecker::refdbm::synchronize(rdbm1, r, sync_ref_clocks);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(x1, t0) = tchecker::dbm::LE_ZERO;
    RDBM2(y1, x1) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM2(t1, y1) = tchecker::dbm::LE_ZERO;
    RDBM2(t0, t2) = tchecker::dbm::LE_ZERO;
    RDBM2(t2, t0) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm1, rdbm2, r));
  }
}

TEST_CASE("Spread bounding DBMs with reference clocks", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");

  tchecker::clock_id_t const rdim = r.size();

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const t2 = r.id("$2");

  SECTION("Spread-bounding the universal positive DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm1[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm1, r);

    tchecker::integer_t const spread = 2;

    auto status = tchecker::refdbm::bound_spread(rdbm1, r, spread);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm2, r);
    for (tchecker::clock_id_t t1 = 0; t1 < r.refcount(); ++t1) {
      for (tchecker::clock_id_t t2 = 0; t2 < r.refcount(); ++t2)
        RDBM2(t1, t2) = MIN(RDBM2(t1, t2), tchecker::dbm::db(tchecker::dbm::LE, spread));
      RDBM2(t1, t1) = tchecker::dbm::LE_ZERO;
    }
    tchecker::refdbm::tighten(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm1, rdbm2, r));
  }

  SECTION("Spread-bounding a DBM that has a minimal spread, yields empty zone")
  {
    tchecker::dbm::db_t rdbm1[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm1, r);
    RDBM1(t0, t1) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    tchecker::refdbm::tighten(rdbm1, r);

    auto status = tchecker::refdbm::bound_spread(rdbm1, r, 1);
    REQUIRE(status == tchecker::dbm::EMPTY);
    REQUIRE(tchecker::refdbm::is_empty_0(rdbm1, r));
  }

  SECTION("Spread-bounding a DBM with negative spread yields empty zone")
  {
    tchecker::dbm::db_t rdbm1[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm1, r);

    auto status = tchecker::refdbm::bound_spread(rdbm1, r, -1);
    REQUIRE(status == tchecker::dbm::EMPTY);
    REQUIRE(tchecker::refdbm::is_empty_0(rdbm1, r));
  }

  SECTION("Spread-bounding a DBM tightens clock constraints")
  {
    // x1 = x2 = t0 & y1 = y2 = t1 & z1 = z2 = t2 & (ti - tj <= 2 for all i,j)
    tchecker::dbm::db_t rdbm1[rdim * rdim];
    tchecker::refdbm::universal(rdbm1, r);

    for (tchecker::clock_id_t t = 0; t != r.refcount(); ++t) {
      for (tchecker::clock_id_t tt = 0; tt != r.refcount(); ++tt)
        RDBM1(t, tt) = tchecker::dbm::db(tchecker::dbm::LE, 2);
      RDBM1(t, t) = tchecker::dbm::LE_ZERO;
    }

    std::vector<tchecker::clock_id_t> const & refmap = r.refmap();
    for (tchecker::clock_id_t u = r.refcount(); u < r.size(); ++u) {
      RDBM1(u, refmap[u]) = tchecker::dbm::LE_ZERO;
      RDBM1(refmap[u], u) = tchecker::dbm::LE_ZERO;
    }

    tchecker::refdbm::tighten(rdbm1, r);

    // bound spread to 1
    tchecker::integer_t const spread = 1;
    auto status = tchecker::refdbm::bound_spread(rdbm1, r, spread);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);

    // the new zone should be: x1 = x2 = t0 & y1 = y2 = t1 & z1 = z2 = t2 &
    // (ti - tj <= 1 for all i,j)
    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal(rdbm2, r);

    for (tchecker::clock_id_t t = 0; t != r.refcount(); ++t) {
      for (tchecker::clock_id_t tt = 0; tt != r.refcount(); ++tt)
        RDBM2(t, tt) = tchecker::dbm::db(tchecker::dbm::LE, spread);
      RDBM2(t, t) = tchecker::dbm::LE_ZERO;
    }

    for (tchecker::clock_id_t u = r.refcount(); u < r.size(); ++u) {
      RDBM2(u, refmap[u]) = tchecker::dbm::LE_ZERO;
      RDBM2(refmap[u], u) = tchecker::dbm::LE_ZERO;
    }

    tchecker::refdbm::tighten(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm1, rdbm2, r));
  }

  SECTION("Partial spread-bounding a DBM partially tightens clock constraints")
  {
    // x1 = x2 = t0 & y1 = y2 = t1 & z1 = z2 = t2 & (ti - tj <= 2 for all i,j)
    tchecker::dbm::db_t rdbm1[rdim * rdim];
    tchecker::refdbm::universal(rdbm1, r);

    for (tchecker::clock_id_t t = 0; t != r.refcount(); ++t) {
      for (tchecker::clock_id_t tt = 0; tt != r.refcount(); ++tt)
        RDBM1(t, tt) = tchecker::dbm::db(tchecker::dbm::LE, 2);
      RDBM1(t, t) = tchecker::dbm::LE_ZERO;
    }

    std::vector<tchecker::clock_id_t> const & refmap = r.refmap();
    for (tchecker::clock_id_t u = r.refcount(); u < r.size(); ++u) {
      RDBM1(u, refmap[u]) = tchecker::dbm::LE_ZERO;
      RDBM1(refmap[u], u) = tchecker::dbm::LE_ZERO;
    }

    tchecker::refdbm::tighten(rdbm1, r);

    // bound spread to 1 for t0 and t1
    tchecker::integer_t const spread = 1;
    boost::dynamic_bitset<> ref_clocks{r.refcount()};
    ref_clocks.reset();
    ref_clocks.set(t0);
    ref_clocks.set(t1);

    auto status = tchecker::refdbm::bound_spread(rdbm1, r, spread, ref_clocks);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);

    // the new zone should be: x1 = x2 = t0 & y1 = y2 = t1 & z1 = z2 = t2 &
    // (ti - tj <= 1 for all i,j in {0,1}) & (ti - t2 <= 2) & (t2 - ti <= 2)
    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal(rdbm2, r);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::dbm::LE, spread);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::dbm::LE, spread);
    RDBM2(t0, t2) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    RDBM2(t2, t0) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    RDBM2(t1, t2) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    RDBM2(t2, t1) = tchecker::dbm::db(tchecker::dbm::LE, 2);

    for (tchecker::clock_id_t u = r.refcount(); u < r.size(); ++u) {
      RDBM2(u, refmap[u]) = tchecker::dbm::LE_ZERO;
      RDBM2(refmap[u], u) = tchecker::dbm::LE_ZERO;
    }

    tchecker::refdbm::tighten(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm1, rdbm2, r));
  }
}

TEST_CASE("Reset to reference clock on DBMs with reference clocks", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");

  tchecker::clock_id_t const rdim = r.size();

  tchecker::clock_id_t const x = r.id("x");
  tchecker::clock_id_t const tx = r.refmap()[x];
  tchecker::clock_id_t const y1 = r.id("y1");
  tchecker::clock_id_t const ty1 = r.refmap()[y1];
  tchecker::clock_id_t const y2 = r.id("y2");
  tchecker::clock_id_t const ty2 = r.refmap()[y2];
  tchecker::clock_id_t const z1 = r.id("z1");
  tchecker::clock_id_t const tz1 = r.refmap()[z1];
  tchecker::clock_id_t const z2 = r.id("z2");

  tchecker::clock_id_t const _x = 0;
  tchecker::clock_id_t const _y2 = 2;
  tchecker::clock_id_t const _z1 = 3;

  SECTION("reset one clock to reference clock on universal positive DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);

    tchecker::refdbm::reset_to_reference_clock(rdbm, r, x);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(x, tx) = tchecker::dbm::LE_ZERO;

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
    RDBM2(x, tx) = tchecker::dbm::LE_ZERO;
    RDBM2(y1, ty1) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("reset to reference clock on DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(tz1, z1) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    RDBM(z1, tz1) = tchecker::dbm::db(tchecker::dbm::LE, 5);
    tchecker::refdbm::tighten(rdbm, r);

    tchecker::refdbm::reset_to_reference_clock(rdbm, r, z1);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(z1, tz1) = tchecker::dbm::LE_ZERO;
    RDBM2(z1, z2) = tchecker::dbm::LE_ZERO; // tightening as rz1 = rz2

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("reset to reference clock on synchronized DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    tchecker::refdbm::synchronize(rdbm, r);
    RDBM(ty1, y1) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    RDBM(y1, ty1) = tchecker::dbm::db(tchecker::dbm::LE, 5);
    tchecker::refdbm::tighten(rdbm, r);

    tchecker::refdbm::reset_to_reference_clock(rdbm, r, y1);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm2, r);
    tchecker::refdbm::synchronize(rdbm2, r);
    for (tchecker::clock_id_t j = 0; j < rdim; ++j)
      RDBM2(y1, j) = tchecker::dbm::LE_ZERO; // reset of x + tightening

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("reset to zero from clock reset on DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(tz1, z1) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    RDBM(z1, tz1) = tchecker::dbm::db(tchecker::dbm::LE, 5);
    tchecker::refdbm::tighten(rdbm, r);

    tchecker::clock_reset_t cr{_z1, tchecker::REFCLOCK_ID, 0};
    tchecker::refdbm::reset(rdbm, r, cr);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(z1, tz1) = tchecker::dbm::LE_ZERO;
    RDBM2(z1, z2) = tchecker::dbm::LE_ZERO; // tightening as tz1 = tz2
    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("reset to zero from clock reset collection on DBM with reference clocks")
  {
    tchecker::dbm::db_t rdbm[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm, r);
    RDBM(tz1, z1) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    RDBM(z1, tz1) = tchecker::dbm::db(tchecker::dbm::LE, 5);
    RDBM(tx, x) = tchecker::dbm::db(tchecker::dbm::LE, -7);
    RDBM(x, tx) = tchecker::dbm::db(tchecker::dbm::LT, 15);
    RDBM(ty2, y2) = tchecker::dbm::db(tchecker::dbm::LT, 0);
    RDBM(y2, ty2) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    tchecker::refdbm::tighten(rdbm, r);

    tchecker::clock_reset_container_t rc;
    rc.emplace_back(_z1, tchecker::REFCLOCK_ID, 0);
    rc.emplace_back(_x, tchecker::REFCLOCK_ID, 0);
    rc.emplace_back(_y2, tchecker::REFCLOCK_ID, 0);

    tchecker::refdbm::reset(rdbm, r, rc);

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(z1, tz1) = tchecker::dbm::LE_ZERO;
    RDBM2(tz1, z1) = tchecker::dbm::LE_ZERO;
    RDBM2(x, tx) = tchecker::dbm::LE_ZERO;
    RDBM2(tx, x) = tchecker::dbm::LE_ZERO;
    RDBM2(y2, ty2) = tchecker::dbm::LE_ZERO;
    RDBM2(ty2, y2) = tchecker::dbm::LE_ZERO;
    tchecker::refdbm::tighten(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }
}

TEST_CASE("asynchronous_open_up on DBMs with reference clocks", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const t2 = r.id("$2");
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
    RDBM(t0, t1) = tchecker::dbm::LE_ZERO;
    RDBM(t1, t0) = tchecker::dbm::LE_ZERO;
    RDBM(t1, y) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM(t2, t1) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    RDBM(y, t1) = tchecker::dbm::db(tchecker::dbm::LE, 2);
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
    RDBM(t0, t1) = tchecker::dbm::LE_ZERO;
    RDBM(t0, x) = tchecker::dbm::LE_ZERO;
    RDBM(t1, t0) = tchecker::dbm::LE_ZERO;
    RDBM(t1, y) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    RDBM(t2, t1) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    RDBM(t2, z1) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    RDBM(y, t1) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    RDBM(y, x) = tchecker::dbm::db(tchecker::dbm::LE, 8);
    RDBM(z1, z2) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    RDBM(z2, z1) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    tchecker::refdbm::tighten(rdbm, r);

    boost::dynamic_bitset<> delay_allowed(refcount, 0);
    delay_allowed[t1] = 1;

    tchecker::dbm::db_t rdbm2[rdim * rdim];
    std::memcpy(rdbm2, rdbm, rdim * rdim * sizeof(*rdbm2));
    for (tchecker::clock_id_t x = 0; x < rdim; ++x)
      RDBM2(x, t1) = (x == t1 ? tchecker::dbm::LE_ZERO : tchecker::dbm::LT_INFINITY);

    tchecker::refdbm::asynchronous_open_up(rdbm, r, delay_allowed);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }
}

TEST_CASE("to_dbm on DBMs with reference clocks", "[refdbm]")
{
  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const t2 = r.id("$2");
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
    RDBM(t1, y) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    RDBM(t2, z) = tchecker::dbm::db(tchecker::dbm::LE, -1);
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
