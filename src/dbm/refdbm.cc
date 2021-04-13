/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <limits>

#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/dbm/refdbm.hh"
#include "tchecker/utils/ordering.hh"

#define DBM(i, j)   dbm[(i)*dim + (j)]
#define RDBM(i, j)  rdbm[(i)*rdim + (j)]
#define RDBM1(i, j) rdbm1[(i)*rdim + (j)]
#define RDBM2(i, j) rdbm2[(i)*rdim + (j)]
#define L(x)        l[x - refcount];
#define U(x)        u[x - refcount];
#define M(x)        m[x - refcount];

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
  // clocks are non-negative: x>=0 <=> X>=RX <=> RX-X<=0
  tchecker::clock_id_t const rdim = r.size();
  tchecker::clock_id_t const * refmap = r.refmap();
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

  // RX-X are less-or-equal to <=0, (i.e. RX<=X)
  tchecker::clock_id_t const * refmap = r.refmap();
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
  tchecker::clock_id_t const * refmap = r.refmap();
  // <inf everywhere, except <=0 on the diagonal and for RX-X
  for (tchecker::clock_id_t i = 0; i < rdim; ++i) {
    for (tchecker::clock_id_t j = 0; j < rdim; ++j) {
      tchecker::clock_id_t const rj = refmap[j];
      tchecker::dbm::db_t const expected = ((i == j) || (i == rj) ? tchecker::dbm::LE_ZERO : tchecker::dbm::LT_INFINITY);
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
    for (tchecker::clock_id_t r = 0; r < refcount; ++r) {
      if (x == r)
        continue;
      if (RDBM(x, r) != tchecker::dbm::LT_INFINITY)
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

  tchecker::clock_id_t const refcount = r.refcount();
  for (tchecker::clock_id_t i = 0; i < refcount - 1; ++i) {
    if (!tchecker::refdbm::is_synchronized(rdbm, r, i, i + 1))
      return false;
  }
  return true;
}

bool is_synchronized(tchecker::dbm::db_t const * rdbm, tchecker::reference_clock_variables_t const & r, tchecker::clock_id_t r1,
                     tchecker::clock_id_t r2)
{
  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));

  tchecker::clock_id_t const rdim = r.size();
  return ((RDBM(r1, r2) == tchecker::dbm::LE_ZERO) && (RDBM(r2, r1) == tchecker::dbm::LE_ZERO));
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

bool is_alu_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
               tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * l, tchecker::integer_t const * u)
{
  assert(rdbm1 != nullptr);
  assert(rdbm2 != nullptr);
  assert(tchecker::refdbm::is_tight(rdbm1, r));
  assert(tchecker::refdbm::is_tight(rdbm2, r));
  assert(tchecker::refdbm::is_consistent(rdbm1, r));
  assert(tchecker::refdbm::is_consistent(rdbm2, r));

  // Z is not included in aLU*(Z') if:
  //    Z{r1,r2} > Z'(r1,r2} for some reference clocks r1, r2
  // or Z{ry,y} >= (<=,-Uy) and Z'{r,y} < Z{r,y} for some reference clock r and offset clock y
  // or Z'{x,r} < Z{x,r} and Z'{x,r} + (<,-Lx) < Z{rx,r} for some reference clock r and offset clock x
  // or Z{ry,y} >= (<=,-Uy) and Z’{x,y} < Z{x,y} and Z’{x,y} + (<,-Lx) < Z{rx,y} for some offset clocks x, y

  std::size_t const rdim = r.size();
  std::size_t const refcount = r.refcount();

  for (tchecker::clock_id_t y = refcount; y < rdim; ++y) {
    tchecker::integer_t const Ly = L(y);
    tchecker::integer_t const Uy = U(y);
    assert(Ly < tchecker::dbm::INF_VALUE);
    assert(Uy < tchecker::dbm::INF_VALUE);

    if (Ly == tchecker::clockbounds::NO_BOUND)
      continue;

    tchecker::clock_id_t const ry = r.refmap()[y];
    tchecker::dbm::db_t const lt_minus_Ly = tchecker::dbm::db(tchecker::dbm::LT, -Ly);

    for (tchecker::clock_id_t r = 0; r < refcount; ++r)
      if ((RDBM2(y, r) < RDBM1(y, r)) && (tchecker::dbm::sum(RDBM2(y, r), lt_minus_Ly) < RDBM1(ry, r)))
        return false;

    if (Uy == tchecker::clockbounds::NO_BOUND)
      continue;

    if (RDBM1(ry, y) < tchecker::dbm::db(tchecker::dbm::LE, -Uy))
      continue;

    for (tchecker::clock_id_t r = 0; r < refcount; ++r)
      if (RDBM2(r, y) < RDBM1(r, y))
        return false;

    for (tchecker::clock_id_t x = refcount; x < rdim; ++x) {
      tchecker::integer_t const Lx = L(x);
      assert(Lx < tchecker::dbm::INF_VALUE);

      if (Lx == tchecker::clockbounds::NO_BOUND)
        continue;

      tchecker::clock_id_t const rx = r.refmap()[x];
      tchecker::dbm::db_t const lt_minus_Lx = tchecker::dbm::db(tchecker::dbm::LT, -Lx);

      if ((RDBM2(x, y) < RDBM1(x, y)) && (tchecker::dbm::sum(RDBM2(x, y), lt_minus_Lx) < RDBM1(rx, y)))
        return false;
    }
  }

  for (tchecker::clock_id_t r1 = 0; r1 < refcount; ++r1)
    for (tchecker::clock_id_t r2 = 0; r2 < refcount; ++r2)
      if (RDBM1(r1, r2) > RDBM2(r1, r2))
        return false;

  return true;
}

bool is_am_le(tchecker::dbm::db_t const * rdbm1, tchecker::dbm::db_t const * rdbm2,
              tchecker::reference_clock_variables_t const & r, tchecker::integer_t const * m)
{
  return is_alu_le(rdbm1, rdbm2, r, m, m);
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
  assert(tchecker::refdbm::is_open_up(rdbm1, r));
  assert(tchecker::refdbm::is_open_up(rdbm2, r));

  // We apply the technique described in appendix C of Govind Rajanbabu's PhD thesis
  // "Partial-order reduction for timed automata", Université de Bordeaux, 2021
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
    tchecker::dbm::db_t min_tx1 = RDBM1(0, x);
    for (tchecker::clock_id_t t = 1; t < refcount; ++t)
      min_tx1 = tchecker::dbm::min(min_tx1, RDBM1(t, x));

    // Check 1st condition
    if (min_tx1 < tchecker::dbm::db(tchecker::dbm::LE, -Ux))
      continue;

    // Compute min_tx2 = min {dbm2[t,x] | t ref clock}
    tchecker::dbm::db_t min_tx2 = RDBM2(0, x);
    for (tchecker::clock_id_t t = 1; t < refcount; ++t)
      min_tx2 = tchecker::dbm::min(min_tx2, RDBM2(t, x));

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
      if (RDBM2(y, x) < RDBM1(y, x) && tchecker::dbm::sum(RDBM2(y, x), tchecker::dbm::db(tchecker::dbm::LT, -Ly)) < min_tx1)
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

enum tchecker::dbm::status_t synchronize(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r)
{
  boost::dynamic_bitset<> sync_ref_clocks{r.refcount()};
  sync_ref_clocks.set();
  return tchecker::refdbm::synchronize(rdbm, r, sync_ref_clocks);
}

enum tchecker::dbm::status_t synchronize(tchecker::dbm::db_t * rdbm, tchecker::reference_clock_variables_t const & r,
                                         boost::dynamic_bitset<> const & sync_ref_clocks)
{
  assert(rdbm != nullptr);
  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));
  assert(r.refcount() == sync_ref_clocks.size());

  tchecker::clock_id_t const rdim = r.size();

  auto t1 = sync_ref_clocks.find_first(); // tchecker::clock_id_t not big enough for npos value
  auto t2 = sync_ref_clocks.find_next(t1);

  if (t1 == sync_ref_clocks.npos || t2 == sync_ref_clocks.npos)
    return tchecker::dbm::NON_EMPTY;

  while (t2 != sync_ref_clocks.npos) {
    assert(t1 < r.refcount());
    assert(t2 < r.refcount());

    RDBM(t1, t2) = tchecker::dbm::min(RDBM(t1, t2), tchecker::dbm::LE_ZERO);
    RDBM(t2, t1) = tchecker::dbm::min(RDBM(t2, t1), tchecker::dbm::LE_ZERO);
    t1 = t2;
    t2 = sync_ref_clocks.find_next(t1);
  }

  // Optimized tightening: Floyd-Warshall algorithm w.r.t. reference clocks in sync_ref_clocks
  for (auto t = sync_ref_clocks.find_first(); t != sync_ref_clocks.npos; t = sync_ref_clocks.find_next(t)) {
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

  tchecker::clock_id_t const rx = r.refmap()[x];

  if (rx == x)
    return;

  // x is identified to rx w.r.t. all clocks z
  for (tchecker::clock_id_t z = 0; z < rdim; ++z) {
    RDBM(x, z) = RDBM(rx, z);
    RDBM(z, x) = RDBM(z, rx);
  }
  RDBM(x, x) = tchecker::dbm::LE_ZERO; // cheaper than testing in loop

  assert(tchecker::refdbm::is_consistent(rdbm, r));
  assert(tchecker::refdbm::is_tight(rdbm, r));
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

  for (tchecker::clock_id_t r = 0; r < refcount; ++r) {
    for (tchecker::clock_id_t x = 0; x < rdim; ++x)
      RDBM(x, r) = tchecker::dbm::LT_INFINITY;
    RDBM(r, r) = tchecker::dbm::LE_ZERO;
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

  // x - r < inf for all x and r s.t. delay is allowed for r (including x being
  // another reference clocks)
  for (tchecker::clock_id_t r = 0; r < refcount; ++r) {
    if (!delay_allowed[r])
      continue;

    for (tchecker::clock_id_t x = 0; x < rdim; ++x)
      RDBM(x, r) = tchecker::dbm::LT_INFINITY;
    RDBM(r, r) = tchecker::dbm::LE_ZERO;
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
