/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <memory>
#include <string>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/dbm/refdbm.hh"
#include "tchecker/refzg/semantics.hh"
#include "tchecker/variables/clocks.hh"

#define RDBM(i, j)    rdbm[(i)*rdim + (j)]
#define RDBM1(i, j)   rdbm1[(i)*rdim + (j)]
#define RDBM2(i, j)   rdbm2[(i)*rdim + (j)]
#define ID_TO_RDBM(i) ((i) + refcount)

#define OUTPUT_DEBUG(rdbm, rdbm2, r)                                                                                           \
  do {                                                                                                                         \
    std::cout << "rdbm:" << std::endl;                                                                                         \
    tchecker::refdbm::output_matrix(std::cout, rdbm, r);                                                                       \
    std::cout << std::endl << "rdbm2" << std::endl;                                                                            \
    tchecker::refdbm::output_matrix(std::cout, rdbm2, r);                                                                      \
    std::cout << std::endl;                                                                                                    \
  } while (0);

TEST_CASE("standard semantics: initial zone", "[refzg semantics]")
{
  std::unique_ptr<tchecker::refzg::semantics_t> semantics{
      tchecker::refzg::semantics_factory(tchecker::refzg::STANDARD_SEMANTICS)};

  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const t2 = r.id("$2");
  tchecker::clock_id_t const x1 = 0;
  tchecker::clock_id_t const x2 = 1;
  tchecker::clock_id_t const y1 = 2;
  tchecker::clock_id_t const y2 = 3;
  tchecker::clock_id_t const z = 4;

  tchecker::clock_id_t const rdim = static_cast<tchecker::clock_id_t>(r.size());
  tchecker::clock_id_t const refcount = r.refcount();
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::dbm::db_t rdbm2[rdim * rdim];

  boost::dynamic_bitset<> src_delay_allowed{refcount, 0};          // delays not allowed
  tchecker::clock_constraint_container_t src_invariant;            // true src invariant
  tchecker::integer_t spread = tchecker::refdbm::UNBOUNDED_SPREAD; // no spread

  SECTION("initial zone, true src invariant, delays not allowed, unbounded spread")
  {
    tchecker::state_status_t status = semantics->initial(rdbm, r, src_delay_allowed, src_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: $0 == $1 == $2 == x1 == x2 == y1 == y2 == z
    tchecker::refdbm::zero(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("initial zone, satisfied src invariant, delays not allowed, unbounded spread")
  {
    // src invariant: x1<=4
    src_invariant.push_back(tchecker::clock_constraint_t{x1, tchecker::REFCLOCK_ID, tchecker::LE, 4});

    tchecker::state_status_t status = semantics->initial(rdbm, r, src_delay_allowed, src_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: $0 == $1 == $2 == x1 == x2 == y1 == y2 == z
    tchecker::refdbm::zero(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("initial zone, unsatisfied src invariant, delays not allowed, unbounded spread")
  {
    // src invariant: 0<x
    src_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x1, tchecker::LT, 0});

    tchecker::state_status_t status = semantics->initial(rdbm, r, src_delay_allowed, src_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED);
  }

  SECTION("initial zone, satisfied src invariant, delays allowed, unbounded spread")
  {
    // delay allowed (all reference clocks)
    src_delay_allowed.set();

    // src invariant: z<=1
    src_invariant.push_back(tchecker::clock_constraint_t{z, tchecker::REFCLOCK_ID, tchecker::LE, 1});

    tchecker::state_status_t status = semantics->initial(rdbm, r, src_delay_allowed, src_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: $0 == $1 == $2 == x1 == x2 == y1 == y2 == z
    tchecker::refdbm::zero(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("initial zone, satisfied src invariant, delays allowed, bounded spread")
  {
    // delay allowed (all reference clocks)
    src_delay_allowed.set();

    // src invariant: y2<=5
    src_invariant.push_back(tchecker::clock_constraint_t{y2, tchecker::REFCLOCK_ID, tchecker::LE, 5});

    // bounded spread: 2
    spread = 2;

    tchecker::state_status_t status = semantics->initial(rdbm, r, src_delay_allowed, src_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: $0 == $1 == $2 == x1 == x2 == y1 == y2 == z
    tchecker::refdbm::zero(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }
}

TEST_CASE("standard semantics: final zone", "[refzg semantics]")
{
  std::unique_ptr<tchecker::refzg::semantics_t> semantics{
      tchecker::refzg::semantics_factory(tchecker::refzg::STANDARD_SEMANTICS)};

  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y", "$1");
  r.declare("z1", "$2");
  r.declare("z2", "$2");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const t2 = r.id("$2");
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;
  tchecker::clock_id_t const z1 = 2;
  tchecker::clock_id_t const z2 = 3;

  tchecker::clock_id_t const rdim = static_cast<tchecker::clock_id_t>(r.size());
  tchecker::clock_id_t const refcount = r.refcount();
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::dbm::db_t rdbm2[rdim * rdim];

  boost::dynamic_bitset<> tgt_delay_allowed{refcount, 0};          // delays not allowed
  tchecker::clock_constraint_container_t tgt_invariant;            // true tgt invariant
  tchecker::integer_t spread = tchecker::refdbm::UNBOUNDED_SPREAD; // no spread

  SECTION("final zone, true tgt invariant, delays not allowed, unbounded spread")
  {
    tchecker::state_status_t status = semantics->final(rdbm, r, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<=0 && t1-y<=0 && t2-z1<=0 && t2-z2<=0
    tchecker::refdbm::universal_positive(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("final zone, satisfied tgt invariant, delays not allowed, unbounded spread")
  {
    // tgt invariant: 2<x<7 && y<=4
    tgt_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::LT, -2});
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LT, 7});
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LE, 4});

    tchecker::state_status_t status = semantics->final(rdbm, r, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: 2<x-t0<7 && y-t1<=4
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -2);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LT, 7);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LE, 4);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("final zone, unsatisfied tgt invariant, delays not allowed, unbounded spread")
  {
    // tgt invariant: x<y && y<z1 && z1<x
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, y, tchecker::LT, 0});
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, z1, tchecker::LT, 0});
    tgt_invariant.push_back(tchecker::clock_constraint_t{z1, x, tchecker::LT, 0});

    tchecker::state_status_t status = semantics->final(rdbm, r, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED);
  }

  SECTION("final zone, satisfied tgt invariant, delays allowed, unbounded spread")
  {
    // delay allowed (all reference clocks)
    tgt_delay_allowed.set();

    // tgt invariant: z1-x<0 && x<=4
    tgt_invariant.push_back(tchecker::clock_constraint_t{z1, x, tchecker::LT, 0});
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LE, 4});

    tchecker::state_status_t status = semantics->final(rdbm, r, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<=0 && t1-y<=0 && t2-z1<=0 && t2-z2<=0 && z1-x<0 && x-t0<=4 && t2-x<0 && z1-t0<4 && t2-t0<4
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::LE_ZERO;
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::LE_ZERO;
    RDBM2(t2, ID_TO_RDBM(z1)) = tchecker::dbm::LE_ZERO;
    RDBM2(t2, ID_TO_RDBM(z2)) = tchecker::dbm::LE_ZERO;
    RDBM2(ID_TO_RDBM(z1), ID_TO_RDBM(x)) = tchecker::dbm::LT_ZERO;
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t2, ID_TO_RDBM(x)) = tchecker::dbm::LT_ZERO;
    RDBM2(ID_TO_RDBM(z1), t0) = tchecker::dbm::db(tchecker::LT, 4);
    RDBM2(t2, t0) = tchecker::dbm::db(tchecker::LT, 4);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("final zone, satisfied tgt invariant, delays allowed, bounded spread")
  {
    // delay allowed (all reference clocks)
    tgt_delay_allowed.set();

    // src invariant: y<=5
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LE, 5});

    // bounded spread: 3
    spread = 3;

    tchecker::state_status_t status = semantics->final(rdbm, r, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm : t0-x<=0 & t1-y<=0 & t2-z1<=0 & t2-z2<=0
    //       & t0-t1<=3 & t1-t0<=3 & t0-t2<=3 & t2-t0<=3 & t1-t2<=3 & t2-t1<=3
    //       & t0-y<=3 & t0-z1<=3 & t0-z2<=3 & t1-x<=3 & t1-z1<=3 & t1-z2<=3 & t2-x<=3 & t2-y<=3
    //       & y-t1<=5
    //       & y-t0<=8 & y-t2<=8 & y-x<=8 & y-z1<=8 & y-z2<=8
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::LE_ZERO;
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::LE_ZERO;
    RDBM2(t2, ID_TO_RDBM(z1)) = tchecker::dbm::LE_ZERO;
    RDBM2(t2, ID_TO_RDBM(z2)) = tchecker::dbm::LE_ZERO;
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t0, t2) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t2, t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t1, t2) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t2, t1) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t0, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t0, ID_TO_RDBM(z1)) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t0, ID_TO_RDBM(z2)) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t1, ID_TO_RDBM(z1)) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t1, ID_TO_RDBM(z2)) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t2, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t2, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 8);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LE, 5);
    RDBM2(ID_TO_RDBM(y), t2) = tchecker::dbm::db(tchecker::LE, 8);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 8);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(z1)) = tchecker::dbm::db(tchecker::LE, 8);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(z2)) = tchecker::dbm::db(tchecker::LE, 8);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }
}

TEST_CASE("standard semantics: next zone", "[refzg semantics]")
{
  std::unique_ptr<tchecker::refzg::semantics_t> semantics{
      tchecker::refzg::semantics_factory(tchecker::refzg::STANDARD_SEMANTICS)};

  std::vector<std::string> refclocks{"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y", "$1");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;

  tchecker::clock_id_t const rdim = static_cast<tchecker::clock_id_t>(r.size());
  tchecker::clock_id_t const refcount = r.refcount();

  // source zone: t0-x<-1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::refdbm::universal_positive(rdbm, r);
  tchecker::clock_constraint_container_t zone;
  zone.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::LT, -1});
  zone.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LE, 3});
  zone.push_back(tchecker::clock_constraint_t{y, x, tchecker::LE, 1});
  tchecker::refdbm::constrain(rdbm, r, zone);

  // expected zone
  tchecker::dbm::db_t rdbm2[rdim * rdim];

  boost::dynamic_bitset<> src_delay_allowed{refcount, 0};              // src delays not allowed
  boost::dynamic_bitset<> tgt_delay_allowed{refcount, 0};              // tgt delays not allowed
  boost::dynamic_bitset<> sync_ref_clocks{refcount, 0};                // no sync ref clocks
  tchecker::clock_constraint_container_t src_invariant, tgt_invariant; // true src and tgt invariant
  tchecker::clock_constraint_container_t guard;                        // true guard
  tchecker::clock_reset_container_t clkreset;                          // no reset
  tchecker::integer_t spread = tchecker::refdbm::UNBOUNDED_SPREAD;     // no spread

  SECTION("next zone, true src invariant, guard and tgt invariant, no sync, no reset, no delay, unbounded spread")
  {
    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-x<=1 & t1-t0<=4
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 4);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, sat. src invariant, true guard and tgt invariant, no sync, no reset, no delay, unbounded spread")
  {
    // src invariant: y<2
    src_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LT, 2});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1 & y-t1<2
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::LE_ZERO;
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LT, 2);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, sat. src invariant, true guard and tgt invariant, no sync, no reset, delay t0, unbounded spread")
  {
    // delay allowed for t0
    src_delay_allowed.set(t0);

    // src invariant: x<10
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LT, 10});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<10 & y-x<=1 & t1-y<=0 & t1-x<=1 & y-t0<11 & t1-t0<11
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LT, 10);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::LE_ZERO;
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LT, 11);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LT, 11);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, unsat. src invariant, true guard and tgt invariant, no sync, no reset, no delay, unbounded spread")
  {
    // src invariant: x<=1
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LE, 1});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED);
  }

  SECTION("next zone, sat. src invariant and guard, true tgt invariant, no sync, no reset, delay {t0, t1}, unbounded spread")
  {
    // delay allowed for {t0, t1}
    src_delay_allowed.set(t0);
    src_delay_allowed.set(t1);

    // src invariant: x<10
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LT, 10});

    // guard: y==2
    guard.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LE, 2});
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::LE, -2});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & y-x<=1 & t1-y<=-2 & t1-x<=-1 & y-t1<=2 & x-t0<10 & y-t0<11 & t1-t0<9
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, -2);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, -1);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LT, 10);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LT, 11);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LT, 9);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, sat. src invariant, unsat guard, true tgt invariant, no sync, no reset, delay {t0, t1}, unbounded spread")
  {
    // delay allowed for {t0, t1}
    src_delay_allowed.set(t0);
    src_delay_allowed.set(t1);

    // src invariant: x<10
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LT, 10});

    // guard: x>67
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::LT, -67});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_GUARD_VIOLATED);
  }

  SECTION("next zone, sat. guard, true src and tgt invariant, sync {t0, t1}, no reset, no delay, unbounded spread")
  {
    // guard: 2<=x<3
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LT, 3});
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::LE, -2});

    // sync {t0, t1}
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<=-2 & x-t0<3 & y-x<=1 & t1-y<=0 & y-t0<4 & t1-t0<=0 & t0-t1<=0
    //               & x-t1<3 & y-t1<4 & t1-x<=-2 & t0-y<=0 & x-y<3
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, -2);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LT, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LT, 4);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), t1) = tchecker::dbm::db(tchecker::LT, 3);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LT, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, -2);
    RDBM2(t0, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LT, 3);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, unsat. sync {t0, t1}")
  {
    // constraint zone with t0<t1 to get unsatisfiable sync
    enum tchecker::dbm::status_t status = tchecker::refdbm::constrain(rdbm, r, t0, t1, tchecker::LT, 0);
    REQUIRE(status == tchecker::dbm::NON_EMPTY);

    // sync {t0, t1}
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);

    tchecker::state_status_t next_status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                           clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(next_status == tchecker::STATE_CLOCKS_EMPTY_SYNC);
  }

  SECTION("next zone, sat. guard, true src and tgt invariant, sync {t0, t1}, reset x:=0, delay {t1}, unbounded spread")
  {
    // delay {t1}
    src_delay_allowed.set(t1);

    // guard: 2<=x<3
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LT, 3});
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::LE, -2});

    // sync {t0, t1}
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);

    // reset x:=0
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-t1<=0 & t1-t0<=0 & t0-x<=0 & t1-x<=0 & x-t0<=0 & x-t1<=0 & y-t0<4 & y-t1<4 & t1-y<=0
    //        & t0-y<=0 & y-x<4 & x-y<=0
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), t1) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LT, 4);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LT, 4);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, 4);
    RDBM2(ID_TO_RDBM(x), ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, true guard, src and tgt invariant, no sync, reset y:=0; x:=0, delay {t1}, unbounded spread")
  {
    // delay {t1}
    src_delay_allowed.set(t1);

    // reset y:=0 ;x:=0
    clkreset.push_back(tchecker::clock_reset_t{y, tchecker::REFCLOCK_ID, 0});
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<=0 & x-t0<=0 & t1-y<=0 & y-t1<=0 & t1-t0<=4 & t1-x<=4 & y-t0<=4 & y-x<=4
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 4);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, true guard and src inv., sat. tgt inv., no sync, no reset, delay {t0}, unbounded spread")
  {
    // delay {t0}
    src_delay_allowed.set(t0);

    // tgt invariant x<=2 & y<1
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LE, 2});
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LT, 1});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & y-x<=1 & t1-y<=0 & t1-x<=1 & x-t0<=2 & y-t1<1 & y-t0<=3 & t1-t0<=3
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LT, 1);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 3);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, unsat. tgt inv.")
  {
    // tgt invariant x<=1
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LE, 1});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED);
  }

  SECTION("next zone, true guard and src inv., sat. tgt inv., no sync, no reset, delay {t0}, bounded spread")
  {
    // delay {t0}
    src_delay_allowed.set(t0);

    // bounded spread
    spread = 1;

    // tgt invariant x<=2 & y<1
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LE, 2});
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LT, 1});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & t1-y<=0 & x-t0<=2 & y-t1<1 & t0-t1<=1 & t1-t0<=1
    //      &  t0-y<=1 & x-t1<=3 & t1-x<0 & y-t0<2 & x-y<=3 & y-x<1
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LT, 1);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t0, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(x), t1) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, 0);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LT, 2);
    RDBM2(ID_TO_RDBM(x), ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, 1);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, impossible bounded spread")
  {
    // src invariant: x-y<=0 & y<=0
    src_invariant.push_back(tchecker::clock_constraint_t{x, y, tchecker::LE, 0});
    src_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LE, 0});

    // bounded spread
    spread = 1;

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_EMPTY_SPREAD);
  }
}

TEST_CASE("standard semantics: next zone, non synchronizable", "[refzg semantics]")
{
  std::unique_ptr<tchecker::refzg::semantics_t> semantics{
      tchecker::refzg::semantics_factory(tchecker::refzg::SYNC_STANDARD_SEMANTICS)};

  std::vector<std::string> refclocks{"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y", "$1");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;

  tchecker::clock_id_t const rdim = static_cast<tchecker::clock_id_t>(r.size());
  tchecker::clock_id_t const refcount = r.refcount();

  // source zone: t0<t1
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::refdbm::universal_positive(rdbm, r);
  RDBM(t0, t1) = tchecker::dbm::LT_ZERO;
  tchecker::refdbm::tighten(rdbm, r);

  boost::dynamic_bitset<> src_delay_allowed{refcount, 0};              // src delays not allowed
  boost::dynamic_bitset<> tgt_delay_allowed{refcount, 0};              // tgt delays not allowed
  boost::dynamic_bitset<> sync_ref_clocks{refcount, 0};                // no sync ref clocks
  tchecker::clock_constraint_container_t src_invariant, tgt_invariant; // true src and tgt invariant
  tchecker::clock_constraint_container_t guard;                        // true guard
  tchecker::clock_reset_container_t clkreset;                          // no reset
  tchecker::integer_t spread = tchecker::refdbm::UNBOUNDED_SPREAD;     // no spread

  tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard, clkreset,
                                                    tgt_delay_allowed, tgt_invariant, spread);
  REQUIRE(status == tchecker::STATE_ZONE_EMPTY_SYNC);
}

TEST_CASE("standard semantics: previous zone", "[refzg semantics]")
{
  std::unique_ptr<tchecker::refzg::semantics_t> semantics{
      tchecker::refzg::semantics_factory(tchecker::refzg::STANDARD_SEMANTICS)};

  std::vector<std::string> refclocks{"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y", "$1");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;

  tchecker::clock_id_t const rdim = static_cast<tchecker::clock_id_t>(r.size());
  tchecker::clock_id_t const refcount = r.refcount();

  // source zone: t0-x<-1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1
  //      t0    t1    x     y
  // t0   <=0   <     <-1   <
  // t1   <=4   <=0   <=1   <=0
  // x    <=3   <     <=0   <
  // y    <=4   <     <=1   <=0
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::refdbm::universal_positive(rdbm, r);
  tchecker::clock_constraint_container_t zone;
  zone.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::LT, -1});
  zone.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LE, 3});
  zone.push_back(tchecker::clock_constraint_t{y, x, tchecker::LE, 1});
  tchecker::refdbm::constrain(rdbm, r, zone);

  // expected zone
  tchecker::dbm::db_t rdbm2[rdim * rdim];

  boost::dynamic_bitset<> src_delay_allowed{refcount, 0};              // src delays not allowed
  boost::dynamic_bitset<> tgt_delay_allowed{refcount, 0};              // tgt delays not allowed
  boost::dynamic_bitset<> sync_ref_clocks{refcount, 0};                // no sync ref clocks
  tchecker::clock_constraint_container_t src_invariant, tgt_invariant; // true src and tgt invariant
  tchecker::clock_constraint_container_t guard;                        // true guard
  tchecker::clock_reset_container_t clkreset;                          // no reset
  tchecker::integer_t spread = tchecker::refdbm::UNBOUNDED_SPREAD;     // no spread

  SECTION("previous zone, true src invariant, guard and tgt invariant, no sync, no reset, no delay, unbounded spread")
  {
    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, true src invariant, guard, sat. tgt invariant, no sync, no reset, no delay, unbounded spread")
  {
    // tgt invariant: y<2
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LT, 2});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1 & y-t1<2
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LT, 2);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, unsat. tgt invariant")
  {
    // tgt invariant: x<1
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LT, 1});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED);
  }

  SECTION("previous zone, true src invariant, guard, sat. tgt invariant, no sync, reset y:=0, no delay, unbounded spread")
  {
    // tgt invariant: y<2
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LT, 2});

    // reset y:=0
    clkreset.push_back(tchecker::clock_reset_t{y, tchecker::REFCLOCK_ID, 0});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<=3 & t1-y<=0 & t1-t0<=4 & t1-x<=1
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, true src invariant, guard, sat. tgt invariant, no sync, no reset, tgt delay, unbounded spread")
  {
    // tgt delay {t0, t1} (shall have no effect)
    tgt_delay_allowed.set(t0);
    tgt_delay_allowed.set(t1);

    // tgt invariant: y<2
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LT, 2});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1 & y-t1<2
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LT, 2);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, impossible reset x:=0")
  {
    // reset x:=0
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_RESET_FAILED);
  }

  SECTION("previous zone, true src invariant, guard, sat. tgt invariant, sync, no reset, no delay, unbounded spread")
  {
    // sync {t0,t1}
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);

    // tgt invariant: y<2
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LT, 2});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<=3 & t1-y<=0 & t1-x<-1 & y-t1<2 & t0-t1<=0 & t1-t0<=0
    //             & x-t1<=3 & t0-y<=0 & y-t0<2 & x-y<=3 & y-x<1
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LT, 2);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), t1) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t0, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LT, 2);
    RDBM2(ID_TO_RDBM(x), ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, 1);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, impossible sync")
  {
    // constrain zone to have t0<t1
    tchecker::refdbm::constrain(rdbm, r, t0, t1, tchecker::LT, 0);

    // sync {t0,t1}
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_EMPTY_SYNC);
  }

  SECTION("previous zone, true src and tgt invariant, sat. guard, no sync, no reset, no delay, unbounded spread")
  {
    // guard: x<2 & y<=6
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LT, 2});
    guard.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LE, 6});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<2 & y-t1<=6 & t1-y<=0 & y-t0<3 & t1-x<=1 & y-x<=1 & t1-t0<3
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LT, 2);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LE, 6);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LT, 3);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LT, 3);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, unsat. guard")
  {
    // guard: x<1
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LT, 1});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_GUARD_VIOLATED);
  }

  SECTION("previous zone, sat. src invariant, true guard and tgt invariant, no sync, no reset, no delay, unbounded spread")
  {
    // src invariant: x<2
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LT, 2});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<2 & y-x<=1 & t1-y<=0 & y-t0<3 & t1-t0<3 & t1-x<=1
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LT, 2);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LT, 3);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LT, 3);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, sat. src invariant, true guard and tgt invariant, no sync, no reset, delay {t1}, unbounded spread")
  {
    // delay {t1}
    src_delay_allowed.set(t1);

    // src invariant: y<=3
    guard.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LE, 3});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1 & y-t1<=3
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LE, 3);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  // delay + bounded spread
  SECTION("previous zone, sat. src invariant, true guard and tgt invariant, no sync, no reset, delay {t0. t1}, bounded spread")
  {
    // delay {t0, t1}
    src_delay_allowed.set(t0);
    src_delay_allowed.set(t1);

    // src invariant: y<=3
    guard.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LE, 3});

    // spead
    spread = 1;

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<=0 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t1<=3 & y-t0<=4 & t1-x<=1
    // & t1-t0<=1 & t0-t1<=1 & x-t1<=4 & t0-y<=1 & x-y<=4
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(x), t1) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t0, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(x), ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 4);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, unsat. src invariant")
  {
    // src invariant: x<=0
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LE, 0});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED);
  }
}

TEST_CASE("standard semantics: previous zone, non synchronizable", "[refzg semantics]")
{
  std::unique_ptr<tchecker::refzg::semantics_t> semantics{
      tchecker::refzg::semantics_factory(tchecker::refzg::SYNC_STANDARD_SEMANTICS)};

  std::vector<std::string> refclocks{"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y", "$1");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;

  tchecker::clock_id_t const rdim = static_cast<tchecker::clock_id_t>(r.size());
  tchecker::clock_id_t const refcount = r.refcount();

  // source zone: t0<t1
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::refdbm::universal_positive(rdbm, r);
  RDBM(t0, t1) = tchecker::dbm::LT_ZERO;
  tchecker::refdbm::tighten(rdbm, r);

  boost::dynamic_bitset<> src_delay_allowed{refcount, 0};              // src delays not allowed
  boost::dynamic_bitset<> tgt_delay_allowed{refcount, 0};              // tgt delays not allowed
  boost::dynamic_bitset<> sync_ref_clocks{refcount, 0};                // no sync ref clocks
  tchecker::clock_constraint_container_t src_invariant, tgt_invariant; // true src and tgt invariant
  tchecker::clock_constraint_container_t guard;                        // true guard
  tchecker::clock_reset_container_t clkreset;                          // no reset
  tchecker::integer_t spread = tchecker::refdbm::UNBOUNDED_SPREAD;     // no spread

  tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard, clkreset,
                                                    tgt_delay_allowed, tgt_invariant, spread);
  REQUIRE(status == tchecker::STATE_ZONE_EMPTY_SYNC);
}

TEST_CASE("elapsed semantics: initial zone", "[refzg semantics]")
{
  std::unique_ptr<tchecker::refzg::semantics_t> semantics{
      tchecker::refzg::semantics_factory(tchecker::refzg::ELAPSED_SEMANTICS)};

  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const t2 = r.id("$2");
  tchecker::clock_id_t const x1 = 0;
  tchecker::clock_id_t const x2 = 1;
  tchecker::clock_id_t const y1 = 2;
  tchecker::clock_id_t const y2 = 3;
  tchecker::clock_id_t const z = 4;

  tchecker::clock_id_t const rdim = static_cast<tchecker::clock_id_t>(r.size());
  tchecker::clock_id_t const refcount = r.refcount();
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::dbm::db_t rdbm2[rdim * rdim];

  boost::dynamic_bitset<> src_delay_allowed{refcount, 0};          // delays not allowed
  tchecker::clock_constraint_container_t src_invariant;            // true src invariant
  tchecker::integer_t spread = tchecker::refdbm::UNBOUNDED_SPREAD; // no spread

  SECTION("initial zone, true src invariant, delays not allowed, unbounded spread")
  {
    tchecker::state_status_t status = semantics->initial(rdbm, r, src_delay_allowed, src_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0=t1=t2=x1=x2=xy1=y2=z (zero zone)
    tchecker::refdbm::zero(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("initial zone, satisfied src invariant, delays allowed, unbounded spread")
  {
    // delay allowed {t0, t1, t2}
    src_delay_allowed.set(t0);
    src_delay_allowed.set(t1);
    src_delay_allowed.set(t2);

    // src invariant: y1<=7
    src_invariant.push_back(tchecker::clock_constraint_t{y1, tchecker::REFCLOCK_ID, tchecker::LE, 7});

    tchecker::state_status_t status = semantics->initial(rdbm, r, src_delay_allowed, src_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x1<=0 & t0-x2<=0 & t1-y1<=0 & t1-y2<=0 & t2-z<=0 & x1=x2=y1=y2=z
    //     & t0-y1<=0 & t0-y2<=0 & t0-z<=0
    //     & t1-x1<=0 & t1-x2<=0 & t1-z<=0
    //     & t2-x1<=0 & t2-x2<=0 & t2-y1<=0 & t2-y2<=0
    //     & y1-t1<=7 & y1-y2<=0 & y1-x1<=0 & y1-x2<=0 & y1-z<=0
    //     & t0-t1<=7 & t2-t1<=7 & x1-t1<=7 & x2-t1<=7 & y2-t1<=7 & z-t1<=7
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t2, ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x1), ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x1), ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x1), ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x1), ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x2), ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x2), ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x2), ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x2), ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y1), ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y1), ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y1), ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y1), ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y2), ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y2), ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y2), ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y2), ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(z), ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(z), ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(z), ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(z), ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t2, ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t2, ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t2, ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t2, ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y1), t1) = tchecker::dbm::db(tchecker::LE, 7);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LE, 7);
    RDBM2(t2, t1) = tchecker::dbm::db(tchecker::LE, 7);
    RDBM2(ID_TO_RDBM(x1), t1) = tchecker::dbm::db(tchecker::LE, 7);
    RDBM2(ID_TO_RDBM(x2), t1) = tchecker::dbm::db(tchecker::LE, 7);
    RDBM2(ID_TO_RDBM(y2), t1) = tchecker::dbm::db(tchecker::LE, 7);
    RDBM2(ID_TO_RDBM(z), t1) = tchecker::dbm::db(tchecker::LE, 7);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("initial zone, unsatisfied src invariant, delays not allowed, unbounded spread")
  {
    // src invariant: x1<0
    src_invariant.push_back(tchecker::clock_constraint_t{x1, tchecker::REFCLOCK_ID, tchecker::LT, 0});

    tchecker::state_status_t status = semantics->initial(rdbm, r, src_delay_allowed, src_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED);
  }

  SECTION("initial zone, satisfied src invariant, delays allowed, bounded spread")
  {
    // delay allowed for {t0}
    src_delay_allowed.set(t0);

    // src invariant: x1<=3
    src_invariant.push_back(tchecker::clock_constraint_t{x1, tchecker::REFCLOCK_ID, tchecker::LE, 3});

    // bounded spread: 2
    spread = 2;

    tchecker::state_status_t status = semantics->initial(rdbm, r, src_delay_allowed, src_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // src: t0=t1=t2=x1=x2=y1=y2=z
    // delay t0: t0-x1<=0 & t1=t2=x1=x2=y1=y2=z
    // src inv: t0-x1<=0 & x1-t0<=3 & t1=t2=x1=x2=y1=y2=z
    // spread: t0-x1<=0 & x1-t0<=2 & t1-t0<=2 & t2-t0<=2 & t1=t2=x1=x2=y1=y2=z
    tchecker::refdbm::zero(rdbm2, r);
    RDBM2(ID_TO_RDBM(x1), t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t2, t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(x2), t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(y1), t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(y2), t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(z), t0) = tchecker::dbm::db(tchecker::LE, 2);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }
}

TEST_CASE("elapsed semantics: final zone", "[refzg semantics]")
{
  std::unique_ptr<tchecker::refzg::semantics_t> semantics{
      tchecker::refzg::semantics_factory(tchecker::refzg::ELAPSED_SEMANTICS)};

  std::vector<std::string> refclocks{"$0", "$1", "$2"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x1", "$0");
  r.declare("x2", "$0");
  r.declare("y1", "$1");
  r.declare("y2", "$1");
  r.declare("z", "$2");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const t2 = r.id("$2");
  tchecker::clock_id_t const x1 = 0;
  tchecker::clock_id_t const x2 = 1;
  tchecker::clock_id_t const y1 = 2;
  tchecker::clock_id_t const y2 = 3;
  tchecker::clock_id_t const z = 4;

  tchecker::clock_id_t const rdim = static_cast<tchecker::clock_id_t>(r.size());
  tchecker::clock_id_t const refcount = r.refcount();
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::dbm::db_t rdbm2[rdim * rdim];

  boost::dynamic_bitset<> tgt_delay_allowed{refcount, 0};          // delays not allowed
  tchecker::clock_constraint_container_t tgt_invariant;            // true tgt invariant
  tchecker::integer_t spread = tchecker::refdbm::UNBOUNDED_SPREAD; // no spread

  SECTION("final zone, true tgt invariant, delays not allowed, unbounded spread")
  {
    tchecker::state_status_t status = semantics->final(rdbm, r, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0<=x1 & t0<=x2 & t1<=y1 & t1<=y2 & t2<=z (universal positive)
    tchecker::refdbm::universal_positive(rdbm2, r);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("final zone, satisfied tgt invariant, delays not allowed, unbounded spread")
  {
    // tgt invariant: y1<=7 & z==8
    tgt_invariant.push_back(tchecker::clock_constraint_t{y1, tchecker::REFCLOCK_ID, tchecker::LE, 7});
    tgt_invariant.push_back(tchecker::clock_constraint_t{z, tchecker::REFCLOCK_ID, tchecker::LE, 8});
    tgt_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, z, tchecker::LE, -8});

    tchecker::state_status_t status = semantics->final(rdbm, r, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x1<=0 & t0-x2<=0 & & t1-y1<=0 & y1-t1<=7 & t1-y2<=0 & y1-y2<=7 & t2-z<=-8 & z-t2<=8
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y1), t1) = tchecker::dbm::db(tchecker::LE, 7);
    RDBM2(t1, ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y1), ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LE, 7);
    RDBM2(t2, ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LE, -8);
    RDBM2(ID_TO_RDBM(z), t2) = tchecker::dbm::db(tchecker::LE, 8);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("final zone, unsatisfied tgt invariant, delays not allowed, unbounded spread")
  {
    // tgt invariant: x1<0
    tgt_invariant.push_back(tchecker::clock_constraint_t{x1, tchecker::REFCLOCK_ID, tchecker::LT, 0});

    tchecker::state_status_t status = semantics->final(rdbm, r, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED);
  }

  SECTION("final zone, satisfied tgt invariant, delays allowed, unbounded spread")
  {
    // delay allowed (all reference clocks)
    tgt_delay_allowed.set();

    // tgt invariant: x2<=1
    tgt_invariant.push_back(tchecker::clock_constraint_t{x2, tchecker::REFCLOCK_ID, tchecker::LE, 1});

    tchecker::state_status_t status = semantics->final(rdbm, r, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x1<=0 & t0-x2<=0 x2-t0<=1 & x2-x1<=1 & t1-y1<=0 & t1-y2<=0 & t2-z<=0
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x2), t0) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(x2), ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t2, ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LE, 0);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("final zone, satisfied tgt invariant, delays allowed, bounded spread")
  {
    // delay allowed for t0
    tgt_delay_allowed.set(t0);

    // tgt invariant: y2<=5 & 1<=x1<7
    tgt_invariant.push_back(tchecker::clock_constraint_t{y2, tchecker::REFCLOCK_ID, tchecker::LE, 5});
    tgt_invariant.push_back(tchecker::clock_constraint_t{x1, tchecker::REFCLOCK_ID, tchecker::LT, 7});
    tgt_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x1, tchecker::LE, -1});

    // bounded spread: 2
    spread = 2;

    tchecker::state_status_t status = semantics->final(rdbm, r, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x2<=0 & t1-y1<=0 & t1-y2<=0 & t2-z<=0 & y2-t1<=5 & t0-x1<=-1 & x1-t0<7 & y2-y1<=5 & x1-x2<7
    // & t0-t1<=2 & t1-t0<=2 & t0-t2<=2 & t2-t0<=2 & t1-t2<=2 & t2-t1<=2
    // & t0-y1<=2 & t0-y2<=2 & x1-t1<9 & t1-x2<=2 & y2-t0<=7 & t1-x1<=1 & t0-z<=2 & x1-t2<9
    // & t2-x2<=2 & t2-x1<=1 & t1-z<=2 & y2-t2<=7 & t2-y1<=2 & t2-y2<=2
    // & x1-y1<9 & x1-y2<9 & y2-x2<=7 & y2-x1<=6 & x1-z<9 & y2-z<=7
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t2, ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y2), t1) = tchecker::dbm::db(tchecker::LE, 5);
    RDBM2(t0, ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, -1);
    RDBM2(ID_TO_RDBM(x1), t0) = tchecker::dbm::db(tchecker::LT, 7);
    RDBM2(ID_TO_RDBM(y2), ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LE, 5);
    RDBM2(ID_TO_RDBM(x1), ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LT, 7);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t0, t2) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t2, t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t1, t2) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t2, t1) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t0, ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t0, ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(x1), t1) = tchecker::dbm::db(tchecker::LT, 9);
    RDBM2(t1, ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(y2), t0) = tchecker::dbm::db(tchecker::LE, 7);
    RDBM2(t1, ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t0, ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(x1), t2) = tchecker::dbm::db(tchecker::LT, 9);
    RDBM2(t2, ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t2, ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(y2), t2) = tchecker::dbm::db(tchecker::LE, 7);
    RDBM2(t2, ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t2, ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(x1), ID_TO_RDBM(y1)) = tchecker::dbm::db(tchecker::LT, 9);
    RDBM2(ID_TO_RDBM(x1), ID_TO_RDBM(y2)) = tchecker::dbm::db(tchecker::LT, 9);
    RDBM2(ID_TO_RDBM(y2), ID_TO_RDBM(x2)) = tchecker::dbm::db(tchecker::LE, 7);
    RDBM2(ID_TO_RDBM(y2), ID_TO_RDBM(x1)) = tchecker::dbm::db(tchecker::LE, 6);
    RDBM2(ID_TO_RDBM(x1), ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LT, 9);
    RDBM2(ID_TO_RDBM(y2), ID_TO_RDBM(z)) = tchecker::dbm::db(tchecker::LE, 7);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }
}

TEST_CASE("elapsed semantics: next zone", "[refzg semantics]")
{
  std::unique_ptr<tchecker::refzg::semantics_t> semantics{
      tchecker::refzg::semantics_factory(tchecker::refzg::ELAPSED_SEMANTICS)};

  std::vector<std::string> refclocks{"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y", "$1");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;

  tchecker::clock_id_t const rdim = static_cast<tchecker::clock_id_t>(r.size());
  tchecker::clock_id_t const refcount = r.refcount();

  // source zone: t0-x<-1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::refdbm::universal_positive(rdbm, r);
  tchecker::clock_constraint_container_t zone;
  zone.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::LT, -1});
  zone.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LE, 3});
  zone.push_back(tchecker::clock_constraint_t{y, x, tchecker::LE, 1});
  tchecker::refdbm::constrain(rdbm, r, zone);

  // expected zone
  tchecker::dbm::db_t rdbm2[rdim * rdim];

  boost::dynamic_bitset<> src_delay_allowed{refcount, 0};              // src delays not allowed
  boost::dynamic_bitset<> tgt_delay_allowed{refcount, 0};              // tgt delays not allowed
  boost::dynamic_bitset<> sync_ref_clocks{refcount, 0};                // no sync ref clocks
  tchecker::clock_constraint_container_t src_invariant, tgt_invariant; // true src and tgt invariant
  tchecker::clock_constraint_container_t guard;                        // true guard
  tchecker::clock_reset_container_t clkreset;                          // no reset
  tchecker::integer_t spread = tchecker::refdbm::UNBOUNDED_SPREAD;     // no spread

  SECTION("next zone, true src invariant, guard and tgt invariant, no sync, no reset, no delay, unbounded spread")
  {
    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, sat. src invariant, true guard and tgt invariant, no sync, no reset, no delay, unbounded spread")
  {
    // src invariant: y<2
    src_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LT, 2});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1 & y-t1<2
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LT, 2);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, unsat. src invariant")
  {
    // src invariant: x<1
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LT, 1});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED);
  }

  SECTION("next zone, sat. guard, true src and tgt invariant, no sync, no reset, no delay, unbounded spread")
  {
    // guard: x==y
    guard.push_back(tchecker::clock_constraint_t{x, y, tchecker::LE, 0});
    guard.push_back(tchecker::clock_constraint_t{y, x, tchecker::LE, 0});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<=3 & t1-y<=0 & x-y<=0 & y-x<=0 & t0-y<-1 & y-t0<=3 & t1-x<=0 & t1-t0<=3
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 3);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, unsat. guard")
  {
    // guard: x>7
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::LT, -7});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_GUARD_VIOLATED);
  }

  SECTION("next zone, sat. src invariant and guard, true tgt invariant, sync {t0, t1}, no reset, no delay, unbounded spread")
  {
    // src invariant: y<=2
    src_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LE, 2});

    // guard: x==y
    guard.push_back(tchecker::clock_constraint_t{x, y, tchecker::LE, 0});
    guard.push_back(tchecker::clock_constraint_t{y, x, tchecker::LE, 0});

    // synchronisation {t0, t1}
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & y-t1<=2 & x-y<=0 & y-x<=0 & t0-y<-1 & t0-t1<=0 & t1-t0<=0
    //             & t1-x<-1 & y-t0<=2 & t1-y<-1 & x-t0<=2 & x-t1<=2
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(x), ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(x), t1) = tchecker::dbm::db(tchecker::LE, 2);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, impossible sync")
  {
    // constrain dbm with t0<t1
    tchecker::refdbm::constrain(rdbm, r, t0, t1, tchecker::LT, 0);

    // synchronization {t0, t1}
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_EMPTY_SYNC);
  }

  SECTION("next zone, sat. src invariant and guard, true tgt invariant, no sync, reset {x}, no delay, unbounded spread")
  {
    // src invariant: y<=2
    src_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LE, 2});

    // guard: x==y
    guard.push_back(tchecker::clock_constraint_t{x, y, tchecker::LE, 0});
    guard.push_back(tchecker::clock_constraint_t{y, x, tchecker::LE, 0});

    // reset {x}
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t1-y<=0 & y-t1<=2 & y-t0<=3 & t0-y<-1 & t1-t0<=3 & t0-t1<1
    // & t0-x<=0 & x-t0<=0 & y-x<=3 & t1-x<=3 & x-y<-1 & x-t1<1
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t0, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LT, 1);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(x), ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t1) = tchecker::dbm::db(tchecker::LT, 1);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, true src invariant and guard, sat. tgt invariant, no sync, reset {y}, no delay, unbounded spread")
  {
    // reset {y}
    clkreset.push_back(tchecker::clock_reset_t{y, tchecker::REFCLOCK_ID, 0});

    // tgt invariant: x<=2
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LE, 2});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & t1-x<=1 & t1-y<=0 & y-t1<=0 & y-x<=1 & x-t0<=2 & t1-t0<=3 & y-t0<=3
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 3);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, sat. src invariant, guard, and tgt invariant, sync {t0, t1}, reset {x}, delay {t0}, unbounded spread")
  {
    // src invariant: y<7
    src_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LT, 7});

    // guard: y>1
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::LT, -1});

    // sync {t0, t1}
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);

    // reset {x}
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    // tgt invariant: y<=2
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LE, 2});

    // tgt delay: {t0}
    tgt_delay_allowed.set(t0);

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t1-y<-1 & t0-t1<=0 & t0-y<-1 & t0-x<=0 & x-t1<=0 & x-y<-1 & t1-x<=0 & y-t1<=2 & y-x<=2
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), t1) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 2);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("next zone, unsat. tgt invariant")
  {
    // reset {x}
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    // tgt invariant: x>0
    tgt_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::LT, 0});

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED);
  }

  SECTION("next zone, sat. src invariant, guard, and tgt invariant, sync {t0, t1}, reset {x}, delay {t0}, bounded spread")
  {
    // src invariant: y<7
    src_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LT, 7});

    // guard: y>1
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::LT, -1});

    // sync {t0, t1}
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);

    // reset {x}
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    // tgt invariant: y<=2
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LE, 2});

    // tgt delay: {t0}
    tgt_delay_allowed.set(t0);

    // spread
    spread = 1;

    tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t1-y<-1 & t0-t1<=0 & t0-y<-1 & t0-x<=0 & x-t1<=0 & x-y<-1 & t1-x<=0 & y-t1<=2 & y-x<=2
    // & t1-t0<=1 & x-t0<=1 & y-t0<=3
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), t1) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 3);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }
}

TEST_CASE("elapsed semantics: next zone, non synchronizable", "[refzg semantics]")
{
  std::unique_ptr<tchecker::refzg::semantics_t> semantics{
      tchecker::refzg::semantics_factory(tchecker::refzg::SYNC_ELAPSED_SEMANTICS)};

  std::vector<std::string> refclocks{"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y", "$1");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;

  tchecker::clock_id_t const rdim = static_cast<tchecker::clock_id_t>(r.size());
  tchecker::clock_id_t const refcount = r.refcount();

  // source zone: t0<t1
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::refdbm::universal_positive(rdbm, r);
  RDBM(t0, t1) = tchecker::dbm::LT_ZERO;
  tchecker::refdbm::tighten(rdbm, r);

  boost::dynamic_bitset<> src_delay_allowed{refcount, 0};              // src delays not allowed
  boost::dynamic_bitset<> tgt_delay_allowed{refcount, 0};              // tgt delays not allowed
  boost::dynamic_bitset<> sync_ref_clocks{refcount, 0};                // no sync ref clocks
  tchecker::clock_constraint_container_t src_invariant, tgt_invariant; // true src and tgt invariant
  tchecker::clock_constraint_container_t guard;                        // true guard
  tchecker::clock_reset_container_t clkreset;                          // no reset
  tchecker::integer_t spread = tchecker::refdbm::UNBOUNDED_SPREAD;     // no spread

  tchecker::state_status_t status = semantics->next(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard, clkreset,
                                                    tgt_delay_allowed, tgt_invariant, spread);
  REQUIRE(status == tchecker::STATE_ZONE_EMPTY_SYNC);
}

TEST_CASE("elapsed semantics: previous zone", "[refzg semantics]")
{
  std::unique_ptr<tchecker::refzg::semantics_t> semantics{
      tchecker::refzg::semantics_factory(tchecker::refzg::ELAPSED_SEMANTICS)};

  std::vector<std::string> refclocks{"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y", "$1");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;

  tchecker::clock_id_t const rdim = static_cast<tchecker::clock_id_t>(r.size());
  tchecker::clock_id_t const refcount = r.refcount();

  // source zone: t0-x<-1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::refdbm::universal_positive(rdbm, r);
  tchecker::clock_constraint_container_t zone;
  zone.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::LT, -1});
  zone.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LE, 3});
  zone.push_back(tchecker::clock_constraint_t{y, x, tchecker::LE, 1});
  tchecker::refdbm::constrain(rdbm, r, zone);

  // expected zone
  tchecker::dbm::db_t rdbm2[rdim * rdim];

  boost::dynamic_bitset<> src_delay_allowed{refcount, 0};              // src delays not allowed
  boost::dynamic_bitset<> tgt_delay_allowed{refcount, 0};              // tgt delays not allowed
  boost::dynamic_bitset<> sync_ref_clocks{refcount, 0};                // no sync ref clocks
  tchecker::clock_constraint_container_t src_invariant, tgt_invariant; // true src and tgt invariant
  tchecker::clock_constraint_container_t guard;                        // true guard
  tchecker::clock_reset_container_t clkreset;                          // no reset
  tchecker::integer_t spread = tchecker::refdbm::UNBOUNDED_SPREAD;     // no spread

  SECTION("previous zone, true src invariant, guard and tgt invariant, no sync, no reset, no delay, unbounded spread")
  {
    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, true src invariant and guard, sat. tgt invariant, no sync, no reset, no delay, unbounded spread")
  {
    // tgt invariant y<6
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LT, 6});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1 & y-t1<6
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LT, 6);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, unsat. tgt invariant")
  {
    // tgt invariant x<=0
    tgt_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LE, 0});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED);
  }

  SECTION("previous zone, true src invariant and guard, sat. tgt invariant, no sync, no reset, delay {t0}, unbounded spread")
  {
    // tgt invariant y<6
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LT, 6});

    // delay {t0}
    tgt_delay_allowed.set(t0);

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<=0 & x-t0<=3 & y-x<=1 & t1-y<=0 & y-t0<=4 & t1-t0<=4 & t1-x<=1 & y-t1<6
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 4);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LT, 6);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, true src invariant and guard, sat. tgt invariant, no sync, reset {x}, delay {t0}, unbounded spread")
  {
    // tgt invariant y<6
    tgt_invariant.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LT, 6});

    // delay {t0}
    tgt_delay_allowed.set(t0);

    // reset {x}
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t1-y<=0 & y-t1<6 & y-t0<=1 & t1-t0<=1 & t0-x<=0 & y-x<=1 & t1-x<=1
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LT, 6);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, impossible reset {x}")
  {
    // reset {x}
    clkreset.push_back(tchecker::clock_reset_t{x, tchecker::REFCLOCK_ID, 0});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_RESET_FAILED);
  }

  SECTION("previous zone, sat. guard, true src and tgt invariant, no sync, reset {y}, no delay, unbounded spread")
  {
    // reset {y}
    clkreset.push_back(tchecker::clock_reset_t{y, tchecker::REFCLOCK_ID, 0});

    // guard: y=7 & 1<x<2
    guard.push_back(tchecker::clock_constraint_t{y, tchecker::REFCLOCK_ID, tchecker::LE, 7});
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, y, tchecker::LE, -7});
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LT, 2});
    guard.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::LT, -1});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & t1-x<=1 & y-t1<=7 & t1-y<=-7 & x-t0<2 & y-t0<10 & y-x<=8 & t1-t0<3
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 1);
    RDBM2(ID_TO_RDBM(y), t1) = tchecker::dbm::db(tchecker::LE, 7);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, -7);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LT, 2);
    RDBM2(ID_TO_RDBM(y), t0) = tchecker::dbm::db(tchecker::LT, 10);
    RDBM2(ID_TO_RDBM(y), ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LE, 8);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LT, 3);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, unsat. guard")
  {
    // guard: x<1
    guard.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LT, 1});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_GUARD_VIOLATED);
  }

  SECTION("previous zone, true src and tgt invariant and guard, sync {t0, t1}, reset {y}, no delay, unbounded spread")
  {
    // reset {y}
    clkreset.push_back(tchecker::clock_reset_t{y, tchecker::REFCLOCK_ID, 0});

    // synchronization {t0, t1}
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & x-t0<=3 & t1-y<=0 & t0-t1<=0 & t1-t0<=0 & x-t1<=3 & t0-y<=0 & t1-x<-1 & x-y<=3
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(ID_TO_RDBM(x), t1) = tchecker::dbm::db(tchecker::LE, 3);
    RDBM2(t0, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 3);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  // impossible sync
  SECTION("previous zone, impossible sync {t0, t1}")
  {
    // constrain t0<t1
    tchecker::refdbm::constrain(rdbm, r, t0, t1, tchecker::LT, 0);

    // synchronization {t0, t1}
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_EMPTY_SYNC);
  }

  SECTION("previous zone, true guard and tgt invariant, sat src inv, sync {t0, t1}, reset {y}, no delay, unbounded spread")
  {
    // reset {y}
    clkreset.push_back(tchecker::clock_reset_t{y, tchecker::REFCLOCK_ID, 0});

    // synchronization {t0, t1}
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);

    // src invariant: x<=2
    src_invariant.push_back(tchecker::clock_constraint_t{x, tchecker::REFCLOCK_ID, tchecker::LE, 2});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_OK);

    // expected dbm: t0-x<-1 & t1-y<=0 & t0-t1<=0 & t1-t0<=0 & t0-y<=0 & t1-x<-1 & x-t0<=2 & x-t1<=2 & x-y<=2
    tchecker::refdbm::universal_positive(rdbm2, r);
    RDBM2(t0, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(t1, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, t1) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, t0) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t0, ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 0);
    RDBM2(t1, ID_TO_RDBM(x)) = tchecker::dbm::db(tchecker::LT, -1);
    RDBM2(ID_TO_RDBM(x), t0) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(x), t1) = tchecker::dbm::db(tchecker::LE, 2);
    RDBM2(ID_TO_RDBM(x), ID_TO_RDBM(y)) = tchecker::dbm::db(tchecker::LE, 2);

    REQUIRE(tchecker::refdbm::is_equal(rdbm, rdbm2, r));
  }

  SECTION("previous zone, unsat. src invariant")
  {
    // reset {y}
    clkreset.push_back(tchecker::clock_reset_t{y, tchecker::REFCLOCK_ID, 0});

    // synchronization {t0, t1}
    sync_ref_clocks.set(t0);
    sync_ref_clocks.set(t1);

    // src invariant: x>5
    src_invariant.push_back(tchecker::clock_constraint_t{tchecker::REFCLOCK_ID, x, tchecker::LT, -5});

    tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard,
                                                      clkreset, tgt_delay_allowed, tgt_invariant, spread);
    REQUIRE(status == tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED);
  }
}

TEST_CASE("elapsed semantics: previous zone, non synchronizable", "[refzg semantics]")
{
  std::unique_ptr<tchecker::refzg::semantics_t> semantics{
      tchecker::refzg::semantics_factory(tchecker::refzg::SYNC_ELAPSED_SEMANTICS)};

  std::vector<std::string> refclocks{"$0", "$1"};
  tchecker::reference_clock_variables_t r(refclocks);
  r.declare("x", "$0");
  r.declare("y", "$1");

  tchecker::clock_id_t const t0 = r.id("$0");
  tchecker::clock_id_t const t1 = r.id("$1");
  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;

  tchecker::clock_id_t const rdim = static_cast<tchecker::clock_id_t>(r.size());
  tchecker::clock_id_t const refcount = r.refcount();

  // source zone: t0<t1
  tchecker::dbm::db_t rdbm[rdim * rdim];
  tchecker::refdbm::universal_positive(rdbm, r);
  RDBM(t0, t1) = tchecker::dbm::LT_ZERO;
  tchecker::refdbm::tighten(rdbm, r);

  boost::dynamic_bitset<> src_delay_allowed{refcount, 0};              // src delays not allowed
  boost::dynamic_bitset<> tgt_delay_allowed{refcount, 0};              // tgt delays not allowed
  boost::dynamic_bitset<> sync_ref_clocks{refcount, 0};                // no sync ref clocks
  tchecker::clock_constraint_container_t src_invariant, tgt_invariant; // true src and tgt invariant
  tchecker::clock_constraint_container_t guard;                        // true guard
  tchecker::clock_reset_container_t clkreset;                          // no reset
  tchecker::integer_t spread = tchecker::refdbm::UNBOUNDED_SPREAD;     // no spread

  tchecker::state_status_t status = semantics->prev(rdbm, r, src_delay_allowed, src_invariant, sync_ref_clocks, guard, clkreset,
                                                    tgt_delay_allowed, tgt_invariant, spread);
  REQUIRE(status == tchecker::STATE_ZONE_EMPTY_SYNC);
}