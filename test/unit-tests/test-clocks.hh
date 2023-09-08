/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/basictypes.hh"
#include "tchecker/variables/clocks.hh"

TEST_CASE("clock constraint negation", "[clocks]")
{
  SECTION("negation of 0 - 1 < 3 if 1 - 0 <= -3")
  {
    tchecker::clock_constraint_t c{0, 1, tchecker::LT, 3};
    tchecker::clock_constraint_t expected{1, 0, tchecker::LE, -3};

    REQUIRE(-c == expected);
  }

  SECTION("negation throws on overflow")
  {
    tchecker::clock_constraint_t c{0, 1, tchecker::LE, std::numeric_limits<tchecker::integer_t>::min()};
    REQUIRE_THROWS_AS(-c, std::invalid_argument);
  }
}

TEST_CASE("clockval initial", "[clocks]")
{
  unsigned short const dim = 4;
  tchecker::clockval_t * clockval = tchecker::clockval_allocate_and_construct(dim);

  tchecker::initial(*clockval);
  for (unsigned short id = 0; id < dim; ++id)
    REQUIRE((*clockval)[id] == 0);

  tchecker::clockval_destruct_and_deallocate(clockval);
}

TEST_CASE("clockval is_initial", "[clocks]")
{
  unsigned short const dim = 4;
  tchecker::clockval_t * clockval = tchecker::clockval_allocate_and_construct(dim);

  SECTION("initial clockval is initial")
  {
    tchecker::initial(*clockval);
    REQUIRE(tchecker::is_initial(*clockval));
  }

  SECTION("non zero clockval is not initial")
  {
    for (unsigned short id = 0; id < dim; ++id)
      (*clockval)[id] = id;
    REQUIRE_FALSE(tchecker::is_initial(*clockval));
  }

  tchecker::clockval_destruct_and_deallocate(clockval);
}

TEST_CASE("clockval satisfies", "[clocks]")
{
  unsigned short const dim = 4;
  tchecker::clockval_t * clockval = tchecker::clockval_allocate_and_construct(dim);

  SECTION("initial clockval satisfies all non-negative constraints")
  {
    tchecker::initial(*clockval);

    REQUIRE(tchecker::satisfies(*clockval, 0, 1, tchecker::LE, 0));
    REQUIRE(tchecker::satisfies(*clockval, 0, 2, tchecker::LE, 3));
    REQUIRE(tchecker::satisfies(*clockval, 3, 1, tchecker::LT, 1289));
    REQUIRE_FALSE(tchecker::satisfies(*clockval, 2, 1, tchecker::LT, 0));
    REQUIRE_FALSE(tchecker::satisfies(*clockval, 1, 0, tchecker::LE, -6));
    REQUIRE_FALSE(tchecker::satisfies(*clockval, 3, 2, tchecker::LT, -1));
  }

  SECTION("some clockval and constraints satisfaction")
  {
    (*clockval)[0] = 0;
    (*clockval)[1] = tchecker::clock_rational_value_t{1, 3};
    (*clockval)[2] = tchecker::clock_rational_value_t{2, 3};
    (*clockval)[3] = 5;

    REQUIRE(tchecker::satisfies(*clockval, 0, 0, tchecker::LE, 0));
    REQUIRE(tchecker::satisfies(*clockval, 0, 1, tchecker::LT, 0));
    REQUIRE_FALSE(tchecker::satisfies(*clockval, 1, 0, tchecker::LE, 0));
    REQUIRE(tchecker::satisfies(*clockval, 1, 0, tchecker::LT, 1));
    REQUIRE(tchecker::satisfies(*clockval, 2, 1, tchecker::LT, 3));
    REQUIRE(tchecker::satisfies(*clockval, 1, 2, tchecker::LE, 0));
    REQUIRE_FALSE(tchecker::satisfies(*clockval, 0, 3, tchecker::LT, -10));
    REQUIRE(tchecker::satisfies(*clockval, 0, 3, tchecker::LT, -1));
    REQUIRE_FALSE(tchecker::satisfies(*clockval, 3, 0, tchecker::LT, 5));
    REQUIRE(tchecker::satisfies(*clockval, 3, 0, tchecker::LE, 20));
    REQUIRE_FALSE(tchecker::satisfies(*clockval, 1, 3, tchecker::LE, -5));
    REQUIRE_FALSE(tchecker::satisfies(*clockval, 3, 1, tchecker::LE, 4));
    REQUIRE(tchecker::satisfies(*clockval, 3, 1, tchecker::LT, 5));
  }

  tchecker::clockval_destruct_and_deallocate(clockval);
}

TEST_CASE("clockval delay", "[clocks]")
{
  unsigned short const dim = 4;

  tchecker::clock_id_t const x = 0;
  tchecker::clock_id_t const y = 1;
  tchecker::clock_id_t const z = 2;

  tchecker::clockval_t * src = tchecker::clockval_allocate_and_construct(dim);
  tchecker::clockval_t * tgt = tchecker::clockval_allocate_and_construct(dim);

  tchecker::clock_constraint_container_t guard, invariant;
  tchecker::clock_reset_container_t reset;

  SECTION("initial clockval to initial clockval, no guard, no invariant, no reset")
  {
    tchecker::initial(*src);
    tchecker::initial(*tgt);

    tchecker::clock_rational_value_t delay = tchecker::delay(*src, invariant, guard, reset, *tgt);
    REQUIRE(delay == 0);
  }

  SECTION("initial clockval to initial clockval, no guard, no invariant, all clocks reset")
  {
    tchecker::initial(*src);
    tchecker::initial(*tgt);

    reset.emplace_back(x, tchecker::REFCLOCK_ID, 0);
    reset.emplace_back(y, tchecker::REFCLOCK_ID, 0);
    reset.emplace_back(z, tchecker::REFCLOCK_ID, 0);

    tchecker::clock_rational_value_t delay = tchecker::delay(*src, invariant, guard, reset, *tgt);
    REQUIRE(delay == 0);
  }

  SECTION("initial clockval to initial clockval, some guard, some invariant, all clocks reset")
  {
    tchecker::initial(*src);
    tchecker::initial(*tgt);

    guard.emplace_back(tchecker::REFCLOCK_ID, x, tchecker::LE, -1);
    guard.emplace_back(tchecker::REFCLOCK_ID, z, tchecker::LT, -2);

    invariant.emplace_back(y, tchecker::REFCLOCK_ID, tchecker::LE, 3);

    reset.emplace_back(x, tchecker::REFCLOCK_ID, 0);
    reset.emplace_back(y, tchecker::REFCLOCK_ID, 0);
    reset.emplace_back(z, tchecker::REFCLOCK_ID, 0);

    tchecker::clock_rational_value_t delay = tchecker::delay(*src, invariant, guard, reset, *tgt);
    REQUIRE(2 < delay);
    REQUIRE(delay <= 3);
  }

  SECTION("some clockval, no guard, no invariant, some clock is not reset")
  {
    (*src)[0] = 0;
    (*src)[1] = tchecker::clock_rational_value_t{1, 4};
    (*src)[2] = 2;
    (*src)[3] = tchecker::clock_rational_value_t{22, 10};

    (*tgt)[0] = 0;
    (*tgt)[1] = tchecker::clock_rational_value_t{7, 8};
    (*tgt)[2] = 0;
    (*tgt)[3] = tchecker::clock_rational_value_t{2825, 1000};

    reset.emplace_back(y, tchecker::REFCLOCK_ID, 0);

    tchecker::clock_rational_value_t delay = tchecker::delay(*src, invariant, guard, reset, *tgt);
    REQUIRE(delay == tchecker::clock_rational_value_t{5, 8});
  }

  SECTION("some clockval, some guard, some invariant, all clocks reset")
  {
    (*src)[0] = 0;
    (*src)[1] = tchecker::clock_rational_value_t{1, 4};
    (*src)[2] = 2;
    (*src)[3] = 1;

    guard.emplace_back(tchecker::REFCLOCK_ID, x, tchecker::LT, -1);

    invariant.emplace_back(y, tchecker::REFCLOCK_ID, tchecker::LT, 3);

    reset.emplace_back(x, tchecker::REFCLOCK_ID, 0);
    reset.emplace_back(y, tchecker::REFCLOCK_ID, 0);
    reset.emplace_back(z, tchecker::REFCLOCK_ID, 0);

    (*tgt)[0] = 0;
    (*tgt)[1] = 0;
    (*tgt)[2] = 0;
    (*tgt)[3] = 0;

    tchecker::clock_rational_value_t delay = tchecker::delay(*src, invariant, guard, reset, *tgt);
    REQUIRE(tchecker::clock_rational_value_t{3, 4} < delay);
    REQUIRE(delay < 1);
  }

  SECTION("impossible delay due to guard and invariant")
  {
    (*src)[0] = 0;
    (*src)[1] = tchecker::clock_rational_value_t{1, 4};
    (*src)[2] = 2;
    (*src)[3] = 1;

    guard.emplace_back(tchecker::REFCLOCK_ID, x, tchecker::LT, -1);

    invariant.emplace_back(y, tchecker::REFCLOCK_ID, tchecker::LE, 2);

    (*tgt)[0] = 0;
    (*tgt)[1] = 0;
    (*tgt)[2] = 0;
    (*tgt)[3] = 0;

    tchecker::clock_rational_value_t delay = tchecker::delay(*src, invariant, guard, reset, *tgt);

    REQUIRE(delay < 0);
  }

  SECTION("impossible delay due to reset to constant")
  {
    (*src)[0] = 0;
    (*src)[1] = tchecker::clock_rational_value_t{1, 4};
    (*src)[2] = 2;
    (*src)[3] = 1;

    reset.emplace_back(x, tchecker::REFCLOCK_ID, 1);

    (*tgt)[0] = 0;
    (*tgt)[1] = 0;
    (*tgt)[2] = 0;
    (*tgt)[3] = 0;

    tchecker::clock_rational_value_t delay = tchecker::delay(*src, invariant, guard, reset, *tgt);

    REQUIRE(delay < 0);
  }

  SECTION("delay with reset to same clock")
  {
    (*src)[0] = 0;
    (*src)[1] = tchecker::clock_rational_value_t{1, 4};
    (*src)[2] = 2;
    (*src)[3] = 1;

    guard.emplace_back(tchecker::REFCLOCK_ID, x, tchecker::LT, -1);

    reset.emplace_back(y, y, 1);
    reset.emplace_back(x, tchecker::REFCLOCK_ID, 0);
    reset.emplace_back(z, tchecker::REFCLOCK_ID, 0);

    (*tgt)[0] = 0;
    (*tgt)[1] = 0;
    (*tgt)[2] = tchecker::clock_rational_value_t{38, 10};
    (*tgt)[3] = 0;

    tchecker::clock_rational_value_t delay = tchecker::delay(*src, invariant, guard, reset, *tgt);

    REQUIRE(delay == tchecker::clock_rational_value_t{8, 10});
  }

  SECTION("delay with reset to other clock")
  {
    (*src)[0] = 0;
    (*src)[1] = tchecker::clock_rational_value_t{1, 4};
    (*src)[2] = 2;
    (*src)[3] = 1;

    guard.emplace_back(tchecker::REFCLOCK_ID, x, tchecker::LT, -1);

    invariant.emplace_back(y, tchecker::REFCLOCK_ID, tchecker::LT, 3);

    reset.emplace_back(y, x, 1);
    reset.emplace_back(x, tchecker::REFCLOCK_ID, 0);
    reset.emplace_back(z, tchecker::REFCLOCK_ID, 0);

    (*tgt)[0] = 0;
    (*tgt)[1] = 0;
    (*tgt)[2] = tchecker::clock_rational_value_t{215, 100};
    (*tgt)[3] = 0;

    tchecker::clock_rational_value_t delay = tchecker::delay(*src, invariant, guard, reset, *tgt);

    REQUIRE(delay == tchecker::clock_rational_value_t{9, 10});
  }

  SECTION("delay with sequence of reset to constant, then reset to clock value")
  {
    (*src)[0] = 0;
    (*src)[1] = tchecker::clock_rational_value_t{1, 4};
    (*src)[2] = 2;
    (*src)[3] = 1;

    guard.emplace_back(tchecker::REFCLOCK_ID, x, tchecker::LT, -1);

    invariant.emplace_back(y, tchecker::REFCLOCK_ID, tchecker::LT, 3);

    reset.emplace_back(z, tchecker::REFCLOCK_ID, 5);
    reset.emplace_back(y, z, 1);

    (*tgt)[0] = 0;
    (*tgt)[1] = tchecker::clock_rational_value_t{6, 5};
    (*tgt)[2] = 6;
    (*tgt)[3] = 5;

    tchecker::clock_rational_value_t delay = tchecker::delay(*src, invariant, guard, reset, *tgt);

    REQUIRE(delay == tchecker::clock_rational_value_t{19, 20});
  }

  SECTION("delay with sequence of resets to clock value")
  {
    (*src)[0] = 0;
    (*src)[1] = tchecker::clock_rational_value_t{1, 4};
    (*src)[2] = 2;
    (*src)[3] = 1;

    guard.emplace_back(tchecker::REFCLOCK_ID, x, tchecker::LT, -1);

    invariant.emplace_back(y, tchecker::REFCLOCK_ID, tchecker::LT, 3);

    reset.emplace_back(y, x, 1);
    reset.emplace_back(x, y, 7);
    reset.emplace_back(z, tchecker::REFCLOCK_ID, 0);

    (*tgt)[0] = 0;
    (*tgt)[1] = tchecker::clock_rational_value_t{915, 100};
    (*tgt)[2] = tchecker::clock_rational_value_t{215, 100};
    (*tgt)[3] = 0;

    tchecker::clock_rational_value_t delay = tchecker::delay(*src, invariant, guard, reset, *tgt);

    REQUIRE(delay == tchecker::clock_rational_value_t{9, 10});
  }

  SECTION("impossible delay due to inconsistent sequence of resets")
  {
    (*src)[0] = 0;
    (*src)[1] = tchecker::clock_rational_value_t{1, 4};
    (*src)[2] = 2;
    (*src)[3] = 1;

    guard.emplace_back(tchecker::REFCLOCK_ID, x, tchecker::LT, -1);

    invariant.emplace_back(y, tchecker::REFCLOCK_ID, tchecker::LT, 3);

    reset.emplace_back(y, x, 1);
    reset.emplace_back(x, y, 7);
    reset.emplace_back(z, tchecker::REFCLOCK_ID, 0);

    (*tgt)[0] = 0;
    (*tgt)[1] = tchecker::clock_rational_value_t{915, 100};
    (*tgt)[2] = tchecker::clock_rational_value_t{205, 100};
    (*tgt)[3] = 0;

    tchecker::clock_rational_value_t delay = tchecker::delay(*src, invariant, guard, reset, *tgt);

    REQUIRE(delay < 0);
  }

  SECTION("impossible delay due to resets to clock that require distinct delays")
  {
    (*src)[0] = 0;
    (*src)[1] = tchecker::clock_rational_value_t{1, 4};
    (*src)[2] = 2;
    (*src)[3] = 1;

    guard.emplace_back(tchecker::REFCLOCK_ID, x, tchecker::LT, -1);

    invariant.emplace_back(y, tchecker::REFCLOCK_ID, tchecker::LT, 3);

    reset.emplace_back(y, x, 1);
    reset.emplace_back(z, x, 0);
    reset.emplace_back(x, tchecker::REFCLOCK_ID, 0);

    (*tgt)[0] = 0;
    (*tgt)[1] = 0;
    (*tgt)[2] = tchecker::clock_rational_value_t{205, 100};
    (*tgt)[3] = tchecker::clock_rational_value_t{123, 100};

    tchecker::clock_rational_value_t delay = tchecker::delay(*src, invariant, guard, reset, *tgt);

    REQUIRE(delay < 0);
  }

  tchecker::clockval_destruct_and_deallocate(tgt);
  tchecker::clockval_destruct_and_deallocate(src);
}