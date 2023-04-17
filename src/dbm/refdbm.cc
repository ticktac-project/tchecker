/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <limits>
#include <vector>

#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/dbm/refdbm.hh"
#include "tchecker/utils/ordering.hh"

#define DBM(i, j)       dbm[(i)*dim + (j)]
#define RDBM(i, j)      rdbm[(i)*rdim + (j)]
#define RDBM1(i, j)     rdbm1[(i)*rdim + (j)]
#define RDBM2(i, j)     rdbm2[(i)*rdim + (j)]
#define LTE_RDBM1(i, j) (j < refcount ? tchecker::dbm::LT_INFINITY : RDBM1(i, j))
#define LTE_RDBM2(i, j) (j < refcount ? tchecker::dbm::LT_INFINITY : RDBM2(i, j))
#define L(x)            l[x - refcount];
#define U(x)            u[x - refcount];
#define M(x)            m[x - refcount];

namespace tchecker {

namespace refdbm {

void universal(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);
  tchecker::dbm::universal(rdbm, r.size());
  assert(tchecker::refdbm::is_tight(rdbm, r));
  assert(tchecker::refdbm::is_consistent(rdbm, r));
}

void universal_positive(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);

  tchecker::refdbm::universal(rdbm, r);
  // clocks are non-negative: x>=0 <=> X>=r(X) <=> r(X)-X<=0
  tchecker::clock_id_t const rdim = r.size();
  std::vector<tchecker::clock_id_t> const & refmap = r.refmap();
  for (tchecker::clock_id_t i = r.refcount(); i < rdim; ++i)
    RDBM(refmap[i], i) = tchecker::dbm::LE_ZERO;
  assert(tchecker::refdbm::is_tight(rdbm, r));
  assert(tchecker::refdbm::is_consistent(rdbm, r));
}

void empty(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);
  tchecker::dbm::empty(rdbm, r.size());
}

void zero(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);
  tchecker::dbm::zero(rdbm, r.size());
  assert(tchecker::refdbm::is_tight(rdbm, r));
  assert(tchecker::refdbm::is_consistent(rdbm, r));
}

bool is_empty_0(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);
  return tchecker::dbm::is_empty_0(rdbm, r.size());
}

bool is_universal(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_tight(rdbm, r));
  return tchecker::dbm::is_universal(rdbm, r.size());
}

bool is_positive(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r)
{
  tchecker::clock_id_t const rdim = r.size();

  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_tight(rdbm, r));

  // r(X)-X are less-or-equal to <=0, (i.e. r(X)<=X)
  std::vector<tchecker::clock_id_t> const & refmap = r.refmap();
  for (tchecker::clock_id_t i = r.refcount(); i < rdim; ++i)
    if (RDBM(refmap[i], i) > tchecker::dbm::LE_ZERO)
      return false;
  return true;
}

bool is_universal_positive(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_tight(rdbm, r));

  tchecker::clock_id_t const rdim = r.size();
  std::vector<tchecker::clock_id_t> const & refmap = r.refmap();
  // <inf everywhere, except <=0 on the diagonal and for RX-X
  for (tchecker::clock_id_t i = 0; i < rdim; ++i) {
    for (tchecker::clock_id_t j = 0; j < rdim; ++j) {
      tchecker::clock_id_t const tj = refmap[j];
      tchecker::dbm::db_t const expected = ((i == j) || (i == tj) ? tchecker::dbm::LE_ZERO : tchecker::dbm::LT_INFINITY);
      if (RDBM(i, j) != expected)
        return false;
    }
  }
  return true;
}

bool is_open_up(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));
  tchecker::clock_id_t const rdim = r.size();
  tchecker::clock_id_t const refcount = r.refcount();
  // X-R is <inf for every (offset or reference) clock X and any reference clock R
  for (tchecker::clock_id_t x = 0; x < rdim; ++x)
    for (tchecker::clock_id_t t = 0; t < refcount; ++t) {
      if (x == t)
        continue;
      if (RDBM(x, t) != tchecker::dbm::LT_INFINITY)
        return false;
    }
  return true;
}

bool is_tight(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);
  return tchecker::dbm::is_tight(rdbm, r.size());
}

enum tchecker::dbm::status_t tighten(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm, r));
  enum tchecker::dbm::status_t status = tchecker::dbm::tighten(rdbm, r.size());
  assert((status == tchecker::dbm::EMPTY) || tchecker::refdbm::is_tight(rdbm, r));
  assert((status == tchecker::dbm::EMPTY) || tchecker::refdbm::is_consistent(rdbm, r));
  return status;
}

bool is_consistent(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);
  return tchecker::dbm::is_consistent(rdbm, r.size());
}

bool is_synchronized(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));

  boost::dynamic_bitset<> sync_ref_clocks{r.refcount()};
  sync_ref_clocks.set();
  return tchecker::refdbm::is_synchronized(rdbm, r, sync_ref_clocks);
}

bool is_synchronized(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r,
                     boost::dynamic_bitset<> const & sync_ref_clocks)
{
  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));
  assert(sync_ref_clocks.size() == r.refcount());

  tchecker::clock_id_t const rdim = r.size();

  auto t1 = sync_ref_clocks.find_first();
  auto t2 = sync_ref_clocks.find_next(t1);
  while (t2 != sync_ref_clocks.npos) {
    assert(t1 < r.refcount());
    assert(t2 < r.refcount());

    if (RDBM(t1, t2) != tchecker::dbm::LE_ZERO || RDBM(t2, t1) != tchecker::dbm::LE_ZERO)
      return false;

    t1 = t2;
    t2 = sync_ref_clocks.find_next(t1);
  }
  return true;
}

bool is_synchronizable(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r)
{
  // rdbm has a synchronized valuation iff there is no negative weight between
  // two reference clocks
  tchecker::clock_id_t const rdim = r.size();
  tchecker::clock_id_t const refcount = r.refcount();

  for (tchecker::clock_id_t t1 = 0; t1 < refcount; ++t1) {
    for (tchecker::clock_id_t t2 = 0; t2 < refcount; ++t2) {
      if (t1 == t2)
        continue;
      if (RDBM(t1, t2) < tchecker::dbm::LE_ZERO)
        return false;
    }
  }
  return true;
}

bool is_equal(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
              tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm1 != nullptr);
  assert(rdbm2 != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm1, r));
  assert(tchecker::refdbm::is_consistent(rdbm2, r));
  assert(tchecker::refdbm::is_tight(rdbm1, r));
  assert(tchecker::refdbm::is_tight(rdbm2, r));
  return tchecker::dbm::is_equal(rdbm1, rdbm2, r.size());
}

bool is_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
           tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm1 != nullptr);
  assert(rdbm2 != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm1, r));
  assert(tchecker::refdbm::is_consistent(rdbm2, r));
  assert(tchecker::refdbm::is_tight(rdbm1, r));
  assert(tchecker::refdbm::is_tight(rdbm2, r));
  return tchecker::dbm::is_le(rdbm1, rdbm2, r.size());
}

bool is_alu_star_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
                    tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * l,
                    tchecker::integer_t const * u)
{
  static_assert(tchecker::dbm::INF_VALUE != tchecker::clockbounds::NO_BOUND);

  assert(rdbm1 != nullptr);
  assert(rdbm2 != nullptr);
  assert(tchecker::refdbm::is_tight(rdbm1, r));
  assert(tchecker::refdbm::is_tight(rdbm2, r));
  assert(tchecker::refdbm::is_consistent(rdbm1, r));
  assert(tchecker::refdbm::is_consistent(rdbm2, r));

  // Z is not included in aLU*(Z') if there exists two clocks x, y such that:
  //     Z'{y,x} < Z{y,x}
  // and (<=,Ux) + Z{r(x),x} >= (<=,0) if x is a clock (not a reference clock)
  // and (<,-Ly) + Z'{y,x} < Z{r(y),x} if y is a clock (not a reference clock)

  std::size_t const rdim = r.size();
  std::size_t const refcount = r.refcount();
  std::vector<tchecker::clock_id_t> const & refmap = r.refmap();

  for (tchecker::clock_id_t y = 0; y < rdim; ++y) {
    tchecker::integer_t const Ly = y < refcount ? tchecker::dbm::INF_VALUE : L(y);

    // optimization: 3rd condition is false
    if (Ly == tchecker::clockbounds::NO_BOUND)
      continue;

    for (tchecker::clock_id_t x = 0; x < rdim; ++x) {
      tchecker::integer_t const Ux = x < refcount ? tchecker::dbm::INF_VALUE : U(x);

      // optimization: 2nd condition is false
      if (Ux == tchecker::clockbounds::NO_BOUND)
        continue;

      // first condition: Z'{y,x} < Z{y,x}
      if (RDBM2(y, x) >= RDBM1(y, x))
        continue;

      // second condition (<=,Ux) + Z{r(x),x} >= (<=,0) if x is a clock (not a
      // reference clock)
      if (x >= refcount) {
        tchecker::dbm::db_t const le_Ux = tchecker::dbm::db(tchecker::dbm::LE, Ux);
        tchecker::clock_id_t const rx = refmap[x];

        if (tchecker::dbm::sum(le_Ux, RDBM1(rx, x)) < tchecker::dbm::LE_ZERO)
          continue;
      }

      // third condition (<,-Ly) + Z'{y,x} < Z{r(y),x} if y is a clock (not a
      // reference clock)
      if (y >= refcount) {
        tchecker::dbm::db_t const lt_minus_Ly = tchecker::dbm::db(tchecker::dbm::LT, -Ly);
        tchecker::clock_id_t const ry = refmap[y];

        if (tchecker::dbm::sum(lt_minus_Ly, RDBM2(y, x)) >= RDBM1(ry, x))
          continue;
      }

      // all 3 conditions satisfied
      return false;
    }
  }

  return true;
}

bool is_am_star_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
                   tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * m)
{
  return is_alu_star_le(rdbm1, rdbm2, r, m, m);
}

bool is_time_elapse_alu_star_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
                                tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * l,
                                tchecker::integer_t const * u)
{
  assert(rdbm1 != nullptr);
  assert(rdbm2 != nullptr);
  assert(tchecker::refdbm::is_tight(rdbm1, r));
  assert(tchecker::refdbm::is_tight(rdbm2, r));
  assert(tchecker::refdbm::is_consistent(rdbm1, r));
  assert(tchecker::refdbm::is_consistent(rdbm2, r));

  // In Z' = time-elapse(Z), we have:
  // - Z'{y,t} = (<,infinity) if t is a reference clock and y is a clock or a
  //   reference clock, with y != t
  // - Z'{y,x} = Z{y,x}       otherwise
  //
  // Recall the inclusion test for aLU*:
  // Z is not included in aLU*(Z') if there exists two clocks x, y such that:
  //     Z'{y,x} < Z{y,x}
  // and (<=,Ux) + Z{r(x),x} >= (<=,0) if x is a clock (not a reference clock)
  // and (<,-Ly) + Z'{y,x} < Z{r(y),x} if y is a clock (not a reference clock)
  //
  // The 1st and 3rd conditions are false in a time-elapse zone if x is a
  // reference clock. So we only need to check for any clock or reference clock
  // y, and any clock x (not reference clock)

  std::size_t const rdim = r.size();
  std::size_t const refcount = r.refcount();
  std::vector<tchecker::clock_id_t> const & refmap = r.refmap();

  for (tchecker::clock_id_t y = 0; y < rdim; ++y) {
    tchecker::integer_t const Ly = y < refcount ? tchecker::dbm::INF_VALUE : L(y);

    // optimization: 3rd condition is false
    if (Ly == tchecker::clockbounds::NO_BOUND)
      continue;

    for (tchecker::clock_id_t x = refcount; x < rdim; ++x) {
      tchecker::integer_t const Ux = U(x);

      // optimization: 2nd condition is false
      if (Ux == tchecker::clockbounds::NO_BOUND)
        continue;

      // first condition: Z'{y,x} < Z{y,x}
      if (RDBM2(y, x) >= RDBM1(y, x))
        continue;

      // second condition (<=,Ux) + Z{r(x),x} >= (<=,0) for clock x
      tchecker::dbm::db_t const le_Ux = tchecker::dbm::db(tchecker::dbm::LE, Ux);
      tchecker::clock_id_t const rx = refmap[x];

      if (tchecker::dbm::sum(le_Ux, RDBM1(rx, x)) < tchecker::dbm::LE_ZERO)
        continue;

      // third condition (<,-Ly) + Z'{y,x} < Z{r(y),x} if y is a clock (not a
      // reference clock)
      if (y >= refcount) {
        tchecker::dbm::db_t const lt_minus_Ly = tchecker::dbm::db(tchecker::dbm::LT, -Ly);
        tchecker::clock_id_t const ry = refmap[y];

        if (tchecker::dbm::sum(lt_minus_Ly, RDBM2(y, x)) >= RDBM1(ry, x))
          continue;
      }

      // all 3 conditions satisfied
      return false;
    }
  }

  return true;
}

bool is_time_elapse_am_star_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
                               tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * m)
{
  return tchecker::refdbm::is_time_elapse_alu_star_le(rdbm1, rdbm2, r, m, m);
}

bool is_sync_alu_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
                    tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * l,
                    tchecker::integer_t const * u)
{
  assert(rdbm1 != nullptr);
  assert(rdbm2 != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm1, r));
  assert(tchecker::refdbm::is_consistent(rdbm2, r));
  assert(tchecker::refdbm::is_positive(rdbm1, r));
  assert(tchecker::refdbm::is_positive(rdbm2, r));
  assert(tchecker::refdbm::is_tight(rdbm1, r));
  assert(tchecker::refdbm::is_tight(rdbm2, r));

  // We apply the technique described in appendix C of Govind Rajanbabu's PhD thesis
  // "Partial-order reduction for timed automata", Université de Bordeaux, 2021
  //
  // let dbm1=local_time_elapse(rdbm1), dbm2=local_time_elapse(rdbm2)
  //
  // Let min_tx1 = min {dbm1[t,x] | t ref clock, x offset clock}
  // and min_tx2 = min {dbm2[t,x] | t ref clock, x offset clock}
  //
  // dbm1 not included in aLU(dbm2) if:
  // - either there is an offset clock x s.t.
  //     min_tx1 >= (<= -U(x))
  // &&  min_tx2 < min_tx1
  // - or there are two offset clocks x and y s.t.
  //     min_tx1 >= (<= -U(x))
  // &&  dbm2[y,x] < dbm1[y,x]
  // &&  dbm2[y,x] + (< -L(y)) < min_tx1

  std::size_t const rdim = r.size();
  std::size_t const refcount = r.refcount();

  for (tchecker::clock_id_t x = refcount; x < rdim; ++x) {
    tchecker::integer_t Ux = U(x);
    assert(Ux < tchecker::dbm::INF_VALUE);

    // Skip x as 1st condition cannot be satisfied
    if (Ux == -tchecker::dbm::INF_VALUE)
      continue;

    // Compute min_tx1 = min {dbm1[t,x] | t ref clock}
    tchecker::dbm::db_t min_tx1 = LTE_RDBM1(0, x);
    for (tchecker::clock_id_t t = 1; t < refcount; ++t)
      min_tx1 = tchecker::dbm::min(min_tx1, LTE_RDBM1(t, x));

    // Check 1st condition
    if (min_tx1 < tchecker::dbm::db(tchecker::dbm::LE, -Ux))
      continue;

    // Compute min_tx2 = min {dbm2[t,x] | t ref clock}
    tchecker::dbm::db_t min_tx2 = LTE_RDBM2(0, x);
    for (tchecker::clock_id_t t = 1; t < refcount; ++t)
      min_tx2 = tchecker::dbm::min(min_tx2, LTE_RDBM2(t, x));

    // Check 2nd condition (of first case above)
    if (min_tx2 < min_tx1)
      return false;

    for (tchecker::clock_id_t y = refcount; y < rdim; ++y) {
      tchecker::integer_t Ly = L(y);
      assert(Ly < tchecker::dbm::INF_VALUE);

      if (x == y)
        continue;

      // Skip y as 3rd condition cannot be satisfied
      if (Ly == -tchecker::dbm::INF_VALUE)
        continue;

      // Check 2nd and 3rd conditions (of second case above)
      if (LTE_RDBM2(y, x) < LTE_RDBM1(y, x) &&
          tchecker::dbm::sum(LTE_RDBM2(y, x), tchecker::dbm::db(tchecker::dbm::LT, -Ly)) < min_tx1)
        return false;
    }
  }

  return true;
}

bool is_sync_am_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
                   tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * m)
{
  return tchecker::refdbm::is_sync_alu_le(rdbm1, rdbm2, r, m, m);
}

std::size_t hash(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);
  return tchecker::dbm::hash(rdbm, r.size());
}

enum tchecker::dbm::status_t constrain(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                       tchecker::clock_id_t x, tchecker::clock_id_t y, tchecker::dbm::comparator_t cmp,
                                       tchecker::integer_t value)
{
  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_tight(rdbm, r));
  assert(x < r.size());
  assert(y < r.size());
  return tchecker::dbm::constrain(rdbm, r.size(), x, y, cmp, value);
}

enum tchecker::dbm::status_t constrain(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                       tchecker::clock_constraint_t const & c)
{
  tchecker::clock_constraint_t translated = r.translate(c);
  tchecker::dbm::comparator_t cmp =
      (translated.comparator() == tchecker::clock_constraint_t::LE ? tchecker::dbm::LE : tchecker::dbm::LT);
  return tchecker::refdbm::constrain(rdbm, r, translated.id1(), translated.id2(), cmp, translated.value());
}

enum tchecker::dbm::status_t constrain(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                       tchecker::clock_constraint_container_t const & cc)
{
  for (tchecker::clock_constraint_t const & c : cc) {
    enum tchecker::dbm::status_t status = tchecker::refdbm::constrain(rdbm, r, c);
    if (status == tchecker::dbm::EMPTY)
      return status;
  }
  return tchecker::dbm::NON_EMPTY;
}

enum tchecker::dbm::status_t synchronize(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r)
{
  return tchecker::refdbm::bound_spread(rdbm, r, 0);
}

enum tchecker::dbm::status_t synchronize(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                         boost::dynamic_bitset<> const & sync_ref_clocks)
{
  return tchecker::refdbm::bound_spread(rdbm, r, 0, sync_ref_clocks);
}

tchecker::integer_t const UNBOUNDED_SPREAD = std::numeric_limits<tchecker::integer_t>::max();

enum tchecker::dbm::status_t bound_spread(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                          tchecker::integer_t spread)
{
  boost::dynamic_bitset<> ref_clocks{r.refcount()};
  ref_clocks.set();
  return tchecker::refdbm::bound_spread(rdbm, r, spread, ref_clocks);
}

enum tchecker::dbm::status_t bound_spread(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                          tchecker::integer_t spread, boost::dynamic_bitset<> const & ref_clocks)
{
  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));
  assert(r.refcount() == ref_clocks.size());

  if (spread == tchecker::refdbm::UNBOUNDED_SPREAD)
    return tchecker::dbm::NON_EMPTY;

  tchecker::dbm::db_t const le_spread = tchecker::dbm::db(tchecker::dbm::LE, spread);

  tchecker::clock_id_t const rdim = r.size();

  for (auto t1 = ref_clocks.find_first(); t1 != ref_clocks.npos; t1 = ref_clocks.find_next(t1)) {
    assert(t1 < r.refcount());
    for (auto t2 = ref_clocks.find_first(); t2 != ref_clocks.npos; t2 = ref_clocks.find_next(t2)) {
      assert(t2 < r.refcount());
      RDBM(t1, t2) = tchecker::dbm::min(RDBM(t1, t2), le_spread);
    }
    RDBM(t1, t1) = tchecker::dbm::LE_ZERO;
  }

  // Optimized tightening: Floyd-Warshall algorithm w.r.t. reference clocks in ref_clocks
  for (auto t = ref_clocks.find_first(); t != ref_clocks.npos; t = ref_clocks.find_next(t)) {
    assert(t < r.refcount());

    for (tchecker::clock_id_t x = 0; x < rdim; ++x) {
      if (x == t || RDBM(x, t) == tchecker::dbm::LT_INFINITY)
        continue; // optimization

      for (tchecker::clock_id_t y = 0; y < rdim; ++y) {
        if (y == t || RDBM(t, y) == tchecker::dbm::LT_INFINITY)
          continue; // optimization

        RDBM(x, y) = tchecker::dbm::min(tchecker::dbm::sum(RDBM(x, t), RDBM(t, y)), RDBM(x, y));
      }

      if (RDBM(x, x) < tchecker::dbm::LE_ZERO) {
        RDBM(0, 0) = tchecker::dbm::LT_ZERO;
        return tchecker::dbm::EMPTY;
      }
    }
  }

  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));

  return tchecker::dbm::NON_EMPTY;
}

void reset_to_reference_clock(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                              tchecker::clock_id_t x)
{
  tchecker::clock_id_t const rdim = r.size();

  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));
  assert(x < rdim);

  tchecker::clock_id_t const tx = r.refmap()[x];

  if (tx == x)
    return;

  // x is identified to r(x) w.r.t. all clocks z
  for (tchecker::clock_id_t z = 0; z < rdim; ++z) {
    RDBM(x, z) = RDBM(tx, z);
    RDBM(z, x) = RDBM(z, tx);
  }
  RDBM(x, x) = tchecker::dbm::LE_ZERO; // cheaper than testing in loop

  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));
}

void reset(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r, tchecker::clock_reset_t const & reset)
{
  assert(reset.left_id() < r.size() - r.refcount());
  assert(reset.right_id() == tchecker::REFCLOCK_ID);
  assert(reset.value() == 0);

  tchecker::clock_reset_t translated = r.translate(reset);
  return tchecker::refdbm::reset_to_reference_clock(rdbm, r, translated.left_id());
}

void reset(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
           tchecker::clock_reset_container_t const & rc)
{
  for (tchecker::clock_reset_t const & reset : rc)
    tchecker::refdbm::reset(rdbm, r, reset);
}

void asynchronous_open_up(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r)
{
  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));

  // x - r < inf for all clock x and reference clock r (including x being a
  // reference clock)
  tchecker::clock_id_t const rdim = r.size();
  tchecker::clock_id_t const refcount = r.refcount();

  for (tchecker::clock_id_t t = 0; t < refcount; ++t) {
    for (tchecker::clock_id_t x = 0; x < rdim; ++x)
      RDBM(x, t) = tchecker::dbm::LT_INFINITY;
    RDBM(t, t) = tchecker::dbm::LE_ZERO;
  }

  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));
}

void asynchronous_open_up(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                          boost::dynamic_bitset<> const & delay_allowed)
{
  tchecker::clock_id_t const rdim = r.size();
  tchecker::clock_id_t const refcount = r.refcount();

  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));
  assert(refcount == delay_allowed.size());

  // x - t < inf for all x and t s.t. delay is allowed for t (including x being
  // another reference clocks)
  for (tchecker::clock_id_t t = 0; t < refcount; ++t) {
    if (!delay_allowed[t])
      continue;

    for (tchecker::clock_id_t x = 0; x < rdim; ++x)
      RDBM(x, t) = tchecker::dbm::LT_INFINITY;
    RDBM(t, t) = tchecker::dbm::LE_ZERO;
  }

  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));
}

void to_dbm(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r, tchecker::dbm::db_t * dbm,
            tchecker::clock_id_t dim)
{
  tchecker::clock_id_t const rdim = r.size();
  tchecker::clock_id_t const refcount = r.refcount();

  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));
  assert(tchecker::refdbm::is_synchronized(rdbm, r));
  assert(dbm != nullptr);
  assert(dim == rdim - refcount + 1);

  for (tchecker::clock_id_t i = 1; i < dim; ++i) {
    tchecker::clock_id_t const reference_i = r.refclock_of_system_clock(i - 1); // i-1 translates to system clocks
    tchecker::clock_id_t const offset_i = r.translate_system_clock(i - 1);
    DBM(0, i) = RDBM(reference_i, offset_i);
    DBM(i, 0) = RDBM(offset_i, reference_i);
    for (tchecker::clock_id_t j = i + 1; j < dim; ++j) {
      tchecker::clock_id_t offset_j = r.translate_system_clock(j - 1);
      DBM(i, j) = RDBM(offset_i, offset_j);
      DBM(j, i) = RDBM(offset_j, offset_i);
    }
    DBM(i, i) = RDBM(offset_i, offset_i);
  }
  DBM(0, 0) = tchecker::dbm::LE_ZERO;

  assert(tchecker::dbm::is_tight(dbm, dim));
  assert(tchecker::dbm::is_consistent(dbm, dim));
}

std::ostream & output_matrix(std::ostream & os, tchecker::dbm::db_t const * rdbm,
                             tchecker::reference_clock_variables_t const & r)
{
  return tchecker::dbm::output_matrix(os, rdbm, r.size());
}

std::ostream & output(std::ostream & os, tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r)
{
  tchecker::clock_id_t const rdim = r.size();
  bool first = true;

  os << "(";

  for (tchecker::clock_id_t i = 0; i < rdim; ++i) {
    for (tchecker::clock_id_t j = i + 1; j < rdim; ++j) {
      tchecker::dbm::db_t cij = RDBM(i, j), cji = RDBM(j, i);
      // vi == vj + k
      if (tchecker::dbm::sum(cij, cji) == tchecker::dbm::LE_ZERO) {
        if (!first)
          os << " & ";
        first = false;

        os << r.name(i) << "=" << r.name(j);
        tchecker::integer_t vij = tchecker::dbm::value(cij);
        if (vij > 0)
          os << "+" << tchecker::dbm::value(cij);
        else if (vij < 0)
          os << "-" << -tchecker::dbm::value(cij);
      }
      // k1 <= xi - xj <= k2
      else if ((cij != tchecker::dbm::LT_INFINITY) || (cji != tchecker::dbm::LT_INFINITY)) {
        if (!first)
          os << " & ";
        first = false;

        if (cji != tchecker::dbm::LT_INFINITY)
          os << -tchecker::dbm::value(cji) << tchecker::dbm::comparator_str(cji);

        os << r.name(i) << "-" << r.name(j);

        if (cij != tchecker::dbm::LT_INFINITY)
          os << tchecker::dbm::comparator_str(cij) << tchecker::dbm::value(cij);
      }
    }
  }

  os << ")";

  return os;
}

int lexical_cmp(tchecker::dbm::db_t const * rdbm1, tchecker::reference_clock_variables_t const & r1,
                tchecker::dbm::db_t const * rdbm2, tchecker::reference_clock_variables_t const & r2)
{
  assert(rdbm1 != nullptr);
  assert(rdbm2 != nullptr);
  tchecker::clock_id_t const rdim1 = r1.size();
  tchecker::clock_id_t const rdim2 = r2.size();
  return tchecker::lexical_cmp(rdbm1, rdbm1 + rdim1 * rdim1, rdbm2, rdbm2 + rdim2 * rdim2, tchecker::dbm::db_cmp);
}

} // end of namespace refdbm

} // end of namespace tchecker
