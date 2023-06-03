/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <memory>

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/dbm/dbm.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/zg/semantics.hh"

#define DBM(i, j)    dbm[(i)*dim + (j)]
#define DBM1(i, j)   dbm1[(i)*dim + (j)]
#define DBM2(i, j)   dbm2[(i)*dim + (j)]
#define ID_TO_DBM(i) (i + 1)

#define OUTPUT_DEBUG(dbm, dbm2, dim)                                                                                           \
  do {                                                                                                                         \
    std::cout << "dbm:" << std::endl;                                                                                          \
    tchecker::dbm::output_matrix(std::cout, dbm, dim);                                                                         \
    std::cout << std::endl << "dbm2" << std::endl;                                                                             \
    tchecker::dbm::output_matrix(std::cout, dbm2, dim);                                                                        \
    std::cout << std::endl;                                                                                                    \
  } while (0);

TEST_CASE("standard semantics: initial zone", "[zg semantics]")
{
  std::unique_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(tchecker::zg::STANDARD_SEMANTICS)};

  tchecker::clock_id_t const dim = 4;
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;
  tchecker::clock_id_t const z = 2;

  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::db_t dbm2[dim * dim];

  tchecker::clock_constraint_container_t src_invariant;

  SECTION("initial zone, delay allowed, no invariant")
  {
    bool const delay_allowed = true;
    tchecker::state_status_t status = semantics->initial(dbm, dim, delay_allowed, src_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    tchecker::dbm::zero(dbm2, dim);
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("initial zone, no delay, no invariant")
  {
    bool const delay_allowed = false;
    tchecker::state_status_t status = semantics->initial(dbm, dim, delay_allowed, src_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    tchecker::dbm::zero(dbm2, dim);
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("initial zone, delay allowed, satisfied invariant")
  {
    bool const delay_allowed = true;
    // x <= 1 && z <= 0
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 1});
    src_invariant.push_back(tchecker::clock_constraint_t{z, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 0});
    tchecker::state_status_t status = semantics->initial(dbm, dim, delay_allowed, src_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: x=0 && y=0 && z=0
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(z), 0) = tchecker::dbm::LE_ZERO;
    tchecker::dbm::tighten(dbm2, dim);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("initial zone, delay allowed, unsatisfied invariant")
  {
    bool const delay_allowed = true;
    // x - y < 0
    src_invariant.push_back(tchecker::clock_constraint_t{x, y, tchecker::clock_constraint_t::LT, 0});
    tchecker::state_status_t status = semantics->initial(dbm, dim, delay_allowed, src_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED);
  }
}

TEST_CASE("standard semantics: final zone", "[zg semantics]")
{
  std::unique_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(tchecker::zg::STANDARD_SEMANTICS)};

  tchecker::clock_id_t const dim = 4;
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;
  tchecker::clock_id_t const z = 2;

  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::db_t dbm2[dim * dim];

  tchecker::clock_constraint_container_t tgt_invariant;

  SECTION("final zone, delay allowed, no invariant")
  {
    // delay
    bool const delay_allowed = true;

    tchecker::state_status_t status = semantics->final(dbm, dim, delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    tchecker::dbm::universal_positive(dbm2, dim);
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("final zone, no delay, no invariant")
  {
    // no delay
    bool const delay_allowed = false;

    tchecker::state_status_t status = semantics->final(dbm, dim, delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    tchecker::dbm::universal_positive(dbm2, dim);
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("final zone, delay allowed, satisfied invariant")
  {
    // delay
    bool const delay_allowed = true;

    // tgt invariant: x <= 1 && z > 7
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 1});
    tgt_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, z, tchecker::clock_constraint_t::LT, -7});

    tchecker::state_status_t status = semantics->final(dbm, dim, delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: x <= 1 && z > 7
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    DBM2(0, ID_TO_DBM(z)) = tchecker::dbm::db(tchecker::dbm::LT, -7);
    tchecker::dbm::tighten(dbm2, dim);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("final zone, delay allowed, unsatisfied invariant")
  {
    // delay
    bool const delay_allowed = true;

    // tgt invariant: x < 1 && y >= 1 && x >= y
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 1});
    tgt_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::clock_constraint_t::LE, -1});
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, x, tchecker::clock_constraint_t::LE, 0});

    tchecker::state_status_t status = semantics->final(dbm, dim, delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED);
  }
}

TEST_CASE("standard semantics: next zone", "[zg semantics]")
{
  std::unique_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(tchecker::zg::STANDARD_SEMANTICS)};

  tchecker::clock_id_t const dim = 3;
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;

  // src zone: 1<=x<3 && 2<y<6 && x-y<1 && y-x<5
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  tchecker::clock_constraint_container_t zone;
  zone.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LE, -1});
  zone.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 3});
  zone.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::clock_constraint_t::LT, -2});
  zone.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 6});
  tchecker::dbm::constrain(dbm, dim, zone);

  // expected zone
  tchecker::dbm::db_t dbm2[dim * dim];

  // transition elements
  tchecker::clock_constraint_container_t src_invariant, guard, tgt_invariant;
  tchecker::clock_reset_container_t clkreset;
  bool src_delay_allowed = true, tgt_delay_allowed = true;

  SECTION("next zone: true guard, true invariants, no reset, delays allowed")
  {
    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 1 <= x && 2 < y && x-y < 1 && y-x < 5
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, 1);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, 5);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("next zone: satisfied guard, true invariants, no reset, delays allowed")
  {
    // guard: 2<=x && x==y
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LE, -2});
    guard.push_back(tchecker::clock_constraint_t{x, y, tchecker::clock_constraint_t::LE, 0});
    guard.push_back(tchecker::clock_constraint_t{y, x, tchecker::clock_constraint_t::LE, 0});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 2 < x && x == y
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 0);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("next zone: unsatisfied guard")
  {
    // guard: x-y > 3
    guard.push_back(tchecker::clock_constraint_t{y, x, tchecker::clock_constraint_t::LT, -3});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_GUARD_VIOLATED);
  }

  SECTION("next zone: satisfied guard, satisfied source invariants, true target invariant, no reset, delays allowed")
  {
    // src invariant: 5<y
    src_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::clock_constraint_t::LT, -5});

    // guard: x<2
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 2});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 1<=x<2 && 5<y<7 && x-y<-3 && y-x<5
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LT, 2);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -5);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LT, 7);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -3);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, 5);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("next zone: satisfied guard, satisfied source invariants, true target invariant, no reset, delays not allowed")
  {
    // no delay
    src_delay_allowed = false;
    tgt_delay_allowed = false;

    // src invariant: x<=2
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 2});

    // guard: x>=2 && y<4
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LE, -2});
    guard.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 4});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected zone: 2<=x<=2 && 2<y<4 && x-y<0 && y-x<2
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LT, 4);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, 0);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, 2);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("next zone: unsatisfied guard after src invariant")
  {
    // src invariant: y<4
    src_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 4});

    // guard: y-x>=3
    guard.push_back(tchecker::clock_constraint_t{x, y, tchecker::clock_constraint_t::LE, -3});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_GUARD_VIOLATED);
  }

  SECTION("next zone: satisfied guard and source invariants, true target invariant, reset to 0, delays allowed")
  {
    // src invariant: 0<x (satisfied by src dbm)
    src_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LT, 0});

    // guard: x<4 && y>=3
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 4});
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::clock_constraint_t::LE, -3});

    // reset: x:=0
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 0<=x<=0 && 3<=y<9 && x-y<=-3 && y-x<9
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, -3);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LT, 9);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, -3);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, 9);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("next zone: satisfied guard and source invariants, true target invariant, reset y:=x+2, delays allowed")
  {
    // src invariant: x<2 && y<=3
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 2});
    src_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 3});

    // guard: 1<x && y<3
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LT, -1});
    guard.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 3});

    // reset: y:=x+2
    clkreset.push_back(tchecker::clock_reset_t{y, x, 2});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 1<x<2 && 3<y<4 && y-x<=2 && x-y<=-2
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LT, 2);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -3);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LT, 4);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 2);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("next zone: satisfied guard, source invariants and target invariant, no reset, delays not allowed")
  {
    // no delays
    src_delay_allowed = false;
    tgt_delay_allowed = false;

    // src invariant: y<=5
    src_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 5});

    // guard: 2<=x<4
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LE, -2});
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 4});

    // tgt invariant: y<5
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 5});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 2<=x<3 && 2<y<5 && x-y<1 && y-x<3
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LT, 3);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LT, 5);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, 1);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, 3);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("next zone: unsatisfied tgt invariant")
  {
    // guard: 2<=x
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LE, -2});

    // tgt invariant: x<2
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 2});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED);
  }
}

TEST_CASE("standard semantics: previous zone", "[zg semantics]")
{
  std::unique_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(tchecker::zg::STANDARD_SEMANTICS)};

  tchecker::clock_id_t const dim = 3;
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;

  // src zone: 3<=x<=4 && 2<y<=5 && x-y<=1 && y-x<2
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  tchecker::clock_constraint_container_t zone;
  zone.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LE, -3});
  zone.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 4});
  zone.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::clock_constraint_t::LT, -2});
  zone.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 5});
  zone.push_back(tchecker::clock_constraint_t{x, y, tchecker::clock_constraint_t::LE, 1});
  zone.push_back(tchecker::clock_constraint_t{y, x, tchecker::clock_constraint_t::LT, 2});
  tchecker::dbm::constrain(dbm, dim, zone);

  // expected zone
  tchecker::dbm::db_t dbm2[dim * dim];

  // transition elements
  tchecker::clock_constraint_container_t src_invariant, guard, tgt_invariant;
  tchecker::clock_reset_container_t clkreset;
  bool src_delay_allowed = true, tgt_delay_allowed = true;

  SECTION("previous zone: true guard, true invariants, no reset, delays allowed")
  {
    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 0<=x<=4 && 0<=y<=5 && x-y<=1 && y-x<2
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LE, 4);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LE, 5);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, 2);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: true guard and src invariant, satisfied tgt invariant, no reset, delays allowed")
  {
    // tgt invariant: y-x<=1
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, x, tchecker::clock_constraint_t::LE, 1});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 0<=x<=4 && 0<=y<=5 && x-y<=1 && y-x<=1
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LE, 4);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LE, 5);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 1);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: true guard and src invariant, satisfied tgt invariant, no reset, delays not allowed")
  {
    // no delay
    src_delay_allowed = false;
    tgt_delay_allowed = false;

    // tgt invariant: y<3
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 3});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 3<=x<4 && 2<y<3 && x-y<=1 && y-x<0
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, -3);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LT, 4);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LT, 3);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, 0);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: true guard and src invariant, unsatisfied tgt invariant, no reset, delays allowed")
  {
    // tgt invariant: y<=2
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 2});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED);
  }

  SECTION("previous zone: true guard and src invariant, satisfied tgt invariant, reset x:=3, delays allowed")
  {
    // tgt invariant: y<3
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 3});

    // reset: x:=3
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 3});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected zone: 0<=x && 0<=y<3 && y-x<3
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LT, 3);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, 3);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: true guard and src invariant, satisfied tgt invariant, impossible reset x:=0, delays allowed")
  {
    // tgt invariant: y<3
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, x, tchecker::clock_constraint_t::LT, 3});

    // reset: x:=0
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_RESET_FAILED);
  }

  SECTION("previous zone: true src invariant, satisfied guard and tgt invariant, no reset, delays allowed")
  {
    // tgt invariant: y<4
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 4});

    // guard: x==y
    guard.push_back(tchecker::clock_constraint_t{x, y, tchecker::clock_constraint_t::LE, 0});
    guard.push_back(tchecker::clock_constraint_t{y, x, tchecker::clock_constraint_t::LE, 0});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 0<=x<4 && 0<=y<4 && x-y<=0 && y-x<=0
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LT, 4);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LT, 4);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::LE_ZERO;

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: true src invariant, impossible guard, no reset, delays allowed")
  {
    // tgt invariant: x<4 && 3<y
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 3});

    // guard: x==y
    guard.push_back(tchecker::clock_constraint_t{x, y, tchecker::clock_constraint_t::LE, 0});
    guard.push_back(tchecker::clock_constraint_t{y, x, tchecker::clock_constraint_t::LE, 0});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_GUARD_VIOLATED);
  }

  SECTION("previous zone: satisfied src invariant, guard and tgt invariant, reset y:=4, delays allowed")
  {
    // tgt invariant: y<=4
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 4});

    // reset: y:=4
    clkreset.push_back(tchecker::clock_reset_t{y, tchecker::REFCLOCK_ID, 4});

    // guard: x<4 && y>2
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 4});
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::clock_constraint_t::LT, -2});

    // src invariant: y<=7
    src_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 7});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 0<=x<4 && 0<=y<=7 && x-y<2 && y-x<=4
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LT, 4);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LE, 7);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, 2);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 4);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: impossible src invariant, true guard and satisfied tgt invariant, no reset, delays not allowed")
  {
    // no delay
    src_delay_allowed = false;
    tgt_delay_allowed = false;

    // tgt invariant: x<4 && y<=3
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 4});
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 3});

    // src invariant: x-y<0
    src_invariant.push_back(tchecker::clock_constraint_t{x, y, tchecker::clock_constraint_t::LT, 0});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED);
  }
}

TEST_CASE("elapsed semantics: initial zone", "[zg semantics]")
{
  std::unique_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(tchecker::zg::ELAPSED_SEMANTICS)};

  tchecker::clock_id_t const dim = 4;
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;
  tchecker::clock_id_t const z = 2;

  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::db_t dbm2[dim * dim];

  tchecker::clock_constraint_container_t src_invariant;

  SECTION("initial zone, delay allowed, no invariant")
  {
    // delay
    bool const delay_allowed = true;

    tchecker::state_status_t status = semantics->initial(dbm, dim, delay_allowed, src_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: (x>=0) && (x==y) && (x==z)
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(x), ID_TO_DBM(z)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(z), ID_TO_DBM(x)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(y), ID_TO_DBM(z)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(z), ID_TO_DBM(y)) = tchecker::dbm::LE_ZERO;

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("initial zone, no delay, no invariant")
  {
    // no delay
    bool const delay_allowed = false;

    tchecker::state_status_t status = semantics->initial(dbm, dim, delay_allowed, src_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: x==y==z==0
    tchecker::dbm::zero(dbm2, dim);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("initial zone, delay allowed, satisfied invariant")
  {
    // delay
    bool const delay_allowed = true;

    // src invariant: x <= 8
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 8});

    tchecker::state_status_t status = semantics->initial(dbm, dim, delay_allowed, src_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: x==y==z<=8
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(x), ID_TO_DBM(z)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(z), ID_TO_DBM(x)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(y), ID_TO_DBM(z)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(z), ID_TO_DBM(y)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LE, 8);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LE, 8);
    DBM2(ID_TO_DBM(z), 0) = tchecker::dbm::db(tchecker::dbm::LE, 8);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("initial zone, delay allowed, unsatisfied invariant")
  {
    // delay
    bool const delay_allowed = true;

    // src invariant: x > 0
    src_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LT, 0});

    tchecker::state_status_t status = semantics->initial(dbm, dim, delay_allowed, src_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED);
  }
}

TEST_CASE("elapsed semantics: final zone", "[zg semantics]")
{
  std::unique_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(tchecker::zg::ELAPSED_SEMANTICS)};

  tchecker::clock_id_t const dim = 4;
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;
  tchecker::clock_id_t const z = 2;

  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::db_t dbm2[dim * dim];

  tchecker::clock_constraint_container_t tgt_invariant;

  SECTION("final zone, delay allowed, no invariant")
  {
    // delay
    bool const delay_allowed = true;

    tchecker::state_status_t status = semantics->final(dbm, dim, delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm
    tchecker::dbm::universal_positive(dbm2, dim);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("final zone, no delay, no invariant")
  {
    // no delay
    bool const delay_allowed = false;

    tchecker::state_status_t status = semantics->final(dbm, dim, delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm
    tchecker::dbm::universal_positive(dbm2, dim);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("final zone, delay allowed, satisfied invariant")
  {
    // delay
    bool const delay_allowed = true;

    // tgt invariant: x <= 1 && z > 7
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 1});
    tgt_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, z, tchecker::clock_constraint_t::LT, -7});

    tchecker::state_status_t status = semantics->final(dbm, dim, delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 0 <= x <= 1 && 0 <= y && 7 < z && x-y <= 1 && x-z < -6
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    DBM2(0, ID_TO_DBM(z)) = tchecker::dbm::db(tchecker::dbm::LT, -7);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(z)) = tchecker::dbm::db(tchecker::dbm::LT, -6);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("final zone, delay allowed, unsatisfied invariant")
  {
    // delay
    bool const delay_allowed = true;

    // tgt invariant; x < 1 && y >= 1 && x >= y
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 1});
    tgt_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::clock_constraint_t::LE, -1});
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, x, tchecker::clock_constraint_t::LE, 0});

    tchecker::state_status_t status = semantics->final(dbm, dim, delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED);
  }
}

TEST_CASE("elapsed semantics: next zone", "[zg semantics]")
{
  std::unique_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(tchecker::zg::ELAPSED_SEMANTICS)};

  tchecker::clock_id_t const dim = 3;
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;

  // src zone: 2<=x<=5 && 3<=y<=8 && x-y<1 && y-x<=5
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  DBM(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, -2);
  DBM(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LE, 5);
  DBM(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, -3);
  DBM(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LE, 8);
  DBM(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, 1);
  DBM(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 5);

  // expected zone
  tchecker::dbm::db_t dbm2[dim * dim];

  // transition elements
  tchecker::clock_constraint_container_t src_invariant, guard, tgt_invariant;
  tchecker::clock_reset_container_t clkreset;
  bool src_delay_allowed = true, tgt_delay_allowed = true;

  SECTION("next zone: true guard, true invariants, no reset, delays allowed")
  {
    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 2<=x && 3<=y && x-y<1 && y-x<=5
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, -3);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, 1);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 5);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("next zone: satisfied src invariant, true guard and tgt invariant, no reset, delays allowed")
  {
    // src invariant: x<4 && 4<=y
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 4});
    src_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::clock_constraint_t::LE, -4});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 2<=x && 4<=y && x-y<0 && y-x<=5
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, -4);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, 0);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 5);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("next zone: unsatisfied src invariant")
  {
    // src invariant: x<-1
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, -1});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED);
  }

  SECTION("next zone: satisfied guard and source invariants, true target invariant, no reset, delays allowed")
  {
    // src invariant: 5<y
    src_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::clock_constraint_t::LT, -5});

    // guard: y-x == 2
    guard.push_back(tchecker::clock_constraint_t{x, y, tchecker::clock_constraint_t::LE, -2});
    guard.push_back(tchecker::clock_constraint_t{y, x, tchecker::clock_constraint_t::LE, 2});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 3<x && 5<y && x-y<=-2 && y-x<=2
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, -3);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -5);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 2);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("next zone: unsatisfied guard after satisfied src invariant")
  {
    // src invariant: x<=2
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 2});

    // guard: 0<x-y
    guard.push_back(tchecker::clock_constraint_t{y, x, tchecker::clock_constraint_t::LT, 0});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_GUARD_VIOLATED);
  }

  SECTION("next zone: satisfied guard and source invariant, true target invariant, reset x:=0, delays allowed")
  {
    // src invariant: x<=2
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 2});

    // guard: x==2 && y>4
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LE, -2});
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 2});
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::clock_constraint_t::LT, -4});

    // reset x:=0
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 0<=x && 4<y && x-y<-4 && y-x<=7
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -4);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -4);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 7);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("next zone: satisfied guard, source invariant and target invariant, reset x:=y+1, delays allowed")
  {
    // src invariant: x<=4
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 4});

    // guard: x==3 && 5<y
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LE, -3});
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 3});
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::clock_constraint_t::LT, -5});

    // reset x:=y+1
    clkreset.push_back(tchecker::clock_reset_t{x, y, 1});

    // tgt invariant: x<7
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 7});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 6<x<7 && 5<y<6 && x-y<=1 && y-x<=-1
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, -6);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LT, 7);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -5);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LT, 6);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, -1);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("next zone: satisfied guard, source invariants and target invariant, reset y:=0, delays not allowed")
  {
    // no delays
    src_delay_allowed = false;
    tgt_delay_allowed = false;

    // src invariant: y<=5
    src_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 5});

    // guard: 2<x<=4
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LT, -2});
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 4});

    // tgt invariant: y<5
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 5});

    // reset: y:=0
    clkreset.push_back(tchecker::clock_reset_t{y, tchecker::REFCLOCK_ID, 0});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 2<x<=4 && 0<=y<=0 && x-y<=4 && y-x<-2
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LE, 4);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LE, 0);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LE, 4);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, -2);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("next zone: satisfied guard, and source invariant, reset y:=0, unsatisfied tgt invariant")
  {
    // guard: 3<x
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LT, -3});

    // reset
    clkreset.push_back(tchecker::clock_reset_t{y, tchecker::REFCLOCK_ID, 0});

    // tgt invariant: x<3
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 3});

    tchecker::state_status_t status =
        semantics->next(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED);
  }
}

TEST_CASE("elapsed semantics: previous zone", "[zg semantics]")
{
  std::unique_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(tchecker::zg::ELAPSED_SEMANTICS)};

  tchecker::clock_id_t const dim = 3;
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;

  // src zone: 2<=x && 1<y && y-x<=2
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  DBM(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, -2);
  DBM(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -1);
  DBM(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 2);

  // expected zone
  tchecker::dbm::db_t dbm2[dim * dim];

  // transition elements
  tchecker::clock_constraint_container_t src_invariant, guard, tgt_invariant;
  tchecker::clock_reset_container_t clkreset;
  bool src_delay_allowed = true, tgt_delay_allowed = true;

  SECTION("previous zone: true guard, true invariants, no reset, delays allowed")
  {
    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 0<=x && 0<=y && y-x<=2
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::LE_ZERO;
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 2);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: satisfied tgt invariant, true guard and src invariant, no reset, delays allowed")
  {
    // tgt invariant: x<=5 && y<4
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 5});
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 4});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 0<=x<=5 && 0<=y<4 && x-y<4 && y-x<2
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LE, 5);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LT, 4);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, 4);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, 2);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: unsatisfied src invariant")
  {
    // tgt invariant: x<1
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 1});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED);
  }

  SECTION("previous zone: satisfied target invariant, true guard and src invariant, no reset, delays not allowed")
  {
    // no delays
    tgt_delay_allowed = false;

    // tgt invariant: y<5
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 5});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 2<=x && 1<y<5 && y-x<=2
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LT, 5);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 2);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: satisfied target invariant, true guard and src invariant, reset x:=0, delays allowed")
  {
    // tgt invariant: x<=5 && y<=5
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 5});
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 5});

    // reset x:=0
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 0<=x && 0<=y<=2 && y-x<=2
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::LE_ZERO;
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 2);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: satisfied target invariant, true guard and src invariant, reset x:=3; y:=2, delays not allowed")
  {
    // no delays
    tgt_delay_allowed = false;

    // tgt invariant: x<=5 && y<=5
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 5});
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 5});

    // reset x:=3; y:=2
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 3});
    clkreset.push_back(tchecker::clock_reset_t{y, tchecker::REFCLOCK_ID, 2});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 0<=x && 0<=y
    tchecker::dbm::universal_positive(dbm2, dim);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: satisfied target invariant, true guard and src invariant, reset x:=y+4, delays allowed")
  {
    // tgt invariant: 3<x && y<=2
    tgt_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LT, -3});
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 2});

    // reset x:=y+4
    clkreset.push_back(tchecker::clock_reset_t{x, y, 4});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 0<=x && 0<=y<=2 && y-x<=2
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::LE_ZERO;
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::LE_ZERO;
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 2);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: true target invariant, guard and src invariant, impossible reset, delays not allowed")
  {
    // no delay
    tgt_delay_allowed = false;

    // reset y:=0
    clkreset.push_back(tchecker::clock_reset_t{y, tchecker::REFCLOCK_ID, 0});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_RESET_FAILED);
  }

  SECTION("previous zone: satisfied target invariant and guard, true source invariant, reset x:=0, delays allowed")
  {
    // tgt invariant: y<=5
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 5});

    // reset x:=0
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    // guard: 1<y
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::clock_constraint_t::LT, -1});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 0<=x && 1<y<=2 && y-x<=2
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::LE_ZERO;
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    DBM2(ID_TO_DBM(y), 0) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    DBM2(ID_TO_DBM(y), ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LE, 2);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: satisfied target invariant, unsatisfied guard, true source invariant, no reset, delays allowed")
  {
    // tgt invariant: y<=2
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 2});

    // guard: 3<=y-x
    guard.push_back(tchecker::clock_constraint_t{x, y, tchecker::clock_constraint_t::LT, -3});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_GUARD_VIOLATED);
  }

  SECTION("previous zone: satisfied target invariant, guard, and source invariant, reset y:=0, delays allowed")
  {
    // tgt invariant: y<=5
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 5});

    // reset y:=0
    clkreset.push_back(tchecker::clock_reset_t{y, tchecker::REFCLOCK_ID, 0});

    // guard: 1<x<4
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LT, -1});
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 4});

    // src invariant: 2<y
    src_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::clock_constraint_t::LT, -2});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 1<x<4 && 2<y && x-y<2
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0, ID_TO_DBM(x)) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    DBM2(ID_TO_DBM(x), 0) = tchecker::dbm::db(tchecker::dbm::LT, 4);
    DBM2(0, ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    DBM2(ID_TO_DBM(x), ID_TO_DBM(y)) = tchecker::dbm::db(tchecker::dbm::LT, 2);

    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }

  SECTION("previous zone: satisfied target invariant, guard, unsatisfied source invariant, no reset, delays not allowed")
  {
    // no delay
    tgt_delay_allowed = false;

    // tgt invariant: y<=5
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 5});

    // guard: 2<x<=5
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::clock_constraint_t::LT, -2});
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LE, 5});

    // src invariant: x<2
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::clock_constraint_t::LT, 2});

    tchecker::state_status_t status =
        semantics->prev(dbm, dim, src_delay_allowed, src_invariant, guard, clkreset, tgt_delay_allowed, tgt_invariant);
    REQUIRE(status == tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED);
  }
}