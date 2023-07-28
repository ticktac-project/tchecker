/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <numeric>

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/dbm/dbm.hh"
#include "tchecker/utils/ordering.hh"

namespace tchecker {

namespace dbm {

#define DBM(i, j)  dbm[(i)*dim + (j)]
#define DBM1(i, j) dbm1[(i)*dim + (j)]
#define DBM2(i, j) dbm2[(i)*dim + (j)]
#define M(i)       (i == 0 ? 0 : m[i - 1])
#define L(i)       (i == 0 ? 0 : l[i - 1])
#define U(i)       (i == 0 ? 0 : u[i - 1])

void copy(tchecker::dbm::db_t * dbm1, tchecker::dbm::db_t * dbm2, tchecker::clock_id_t dim)
{
  std::memcpy(dbm1, dbm2, dim * dim * sizeof(*dbm2));
}

void universal(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  for (tchecker::clock_id_t i = 0; i < dim; ++i) {
    for (tchecker::clock_id_t j = 0; j < dim; ++j)
      DBM(i, j) = tchecker::dbm::LT_INFINITY;
    DBM(i, i) = tchecker::dbm::LE_ZERO;
  }

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

void universal_positive(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  for (tchecker::clock_id_t i = 0; i < dim; ++i)
    for (tchecker::clock_id_t j = 0; j < dim; ++j)
      DBM(i, j) = ((i == j) || (i == 0) ? tchecker::dbm::LE_ZERO : tchecker::dbm::LT_INFINITY);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_positive(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

void empty(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  tchecker::dbm::universal(dbm, dim);
  DBM(0, 0) = tchecker::dbm::LT_ZERO;
}

void zero(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  for (tchecker::clock_id_t i = 0; i < dim; ++i)
    for (tchecker::clock_id_t j = 0; j < dim; ++j)
      DBM(i, j) = tchecker::dbm::LE_ZERO;
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

bool is_consistent(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  for (tchecker::clock_id_t i = 0; i < dim; ++i)
    if (DBM(i, i) != tchecker::dbm::LE_ZERO)
      return false;
  return true;
}

bool is_empty_0(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  return (DBM(0, 0) < tchecker::dbm::LE_ZERO);
}

bool is_universal(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  // <inf everywhere except <=0 on diagonal
  for (tchecker::clock_id_t i = 0; i < dim; ++i) {
    if (DBM(i, i) != tchecker::dbm::LE_ZERO)
      return false;

    for (tchecker::clock_id_t j = 0; j < dim; ++j) {
      if ((i != j) && (DBM(i, j) != tchecker::dbm::LT_INFINITY))
        return false;
    }
  }
  return true;
}

bool is_positive(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  for (tchecker::clock_id_t x = 0; x < dim; ++x)
    if (DBM(0, x) > tchecker::dbm::LE_ZERO)
      return false;
  return true;
}

bool is_universal_positive(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  for (tchecker::clock_id_t y = 0; y < dim; ++y)
    if (DBM(0, y) != tchecker::dbm::LE_ZERO)
      return false;
  for (tchecker::clock_id_t x = 1; x < dim; ++x) {
    if ((DBM(x, 0) != tchecker::dbm::LT_INFINITY) || (DBM(x, x) != tchecker::dbm::LE_ZERO))
      return false;
    for (tchecker::clock_id_t y = x + 1; y < dim; ++y)
      if ((DBM(x, y) != tchecker::dbm::LT_INFINITY) || (DBM(y, x) != tchecker::dbm::LT_INFINITY))
        return false;
  }
  return true;
}

bool contains_zero(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(is_consistent(dbm, dim));
  assert(is_tight(dbm, dim));

  for (tchecker::clock_id_t x = 0; x < dim; ++x)
    if (DBM(0, x) != tchecker::dbm::LE_ZERO)
      return false;
  return true;
}

bool is_tight(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  for (tchecker::clock_id_t i = 0; i < dim; ++i)
    for (tchecker::clock_id_t j = 0; j < dim; ++j)
      for (tchecker::clock_id_t k = 0; k < dim; ++k)
        if (tchecker::dbm::sum(DBM(i, k), DBM(k, j)) < DBM(i, j))
          return false;
  return true;
}

enum tchecker::dbm::status_t tighten(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);

  for (tchecker::clock_id_t k = 0; k < dim; ++k) {
    for (tchecker::clock_id_t i = 0; i < dim; ++i) {
      if ((i == k) || (DBM(i, k) == tchecker::dbm::LT_INFINITY)) // optimization
        continue;
      for (tchecker::clock_id_t j = 0; j < dim; ++j)
        DBM(i, j) = tchecker::dbm::min(tchecker::dbm::sum(DBM(i, k), DBM(k, j)), DBM(i, j));
      if (DBM(i, i) < tchecker::dbm::LE_ZERO) {
        DBM(0, 0) = tchecker::dbm::LT_ZERO;
        return tchecker::dbm::EMPTY;
      }
    }
  }
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
  return tchecker::dbm::NON_EMPTY;
}

enum tchecker::dbm::status_t tighten(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x,
                                     tchecker::clock_id_t y)
{
  assert(dbm != nullptr);
  assert(dim >= 1);

  if (DBM(x, y) == tchecker::dbm::LT_INFINITY)
    return tchecker::dbm::MAY_BE_EMPTY;

  for (tchecker::clock_id_t i = 0; i < dim; ++i) {
    // tighten i->y w.r.t. i->x->y
    if (i != x) {
      tchecker::dbm::db_t db_ixy = tchecker::dbm::sum(DBM(i, x), DBM(x, y));
      if (db_ixy < DBM(i, y))
        DBM(i, y) = db_ixy;
    }

    // tighten i->j w.r.t. i->y->j
    for (tchecker::clock_id_t j = 0; j < dim; ++j)
      DBM(i, j) = tchecker::dbm::min(DBM(i, j), tchecker::dbm::sum(DBM(i, y), DBM(y, j)));

    if (DBM(i, i) < tchecker::dbm::LE_ZERO) {
      DBM(0, 0) = tchecker::dbm::LT_ZERO;
      return tchecker::dbm::EMPTY;
    }
  }

  return tchecker::dbm::MAY_BE_EMPTY;
}

enum tchecker::dbm::status_t constrain(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x,
                                       tchecker::clock_id_t y, tchecker::ineq_cmp_t cmp, tchecker::integer_t value)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
  assert(x < dim);
  assert(y < dim);

  tchecker::dbm::db_t db = tchecker::dbm::db(cmp, value);
  if (db >= DBM(x, y))
    return tchecker::dbm::NON_EMPTY;

  DBM(x, y) = db;

  auto res = tchecker::dbm::tighten(dbm, dim, x, y);

  if (res == tchecker::dbm::MAY_BE_EMPTY)
    res = tchecker::dbm::NON_EMPTY; // since dbm was tight before

  assert((res == tchecker::dbm::EMPTY) || tchecker::dbm::is_consistent(dbm, dim));
  assert((res == tchecker::dbm::EMPTY) || tchecker::dbm::is_tight(dbm, dim));

  return res;
}

enum tchecker::dbm::status_t constrain(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim,
                                       tchecker::clock_constraint_container_t const & constraints)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));

  for (tchecker::clock_constraint_t const & c : constraints) {
    tchecker::clock_id_t id1 = (c.id1() == tchecker::REFCLOCK_ID ? 0 : c.id1() + 1);
    tchecker::clock_id_t id2 = (c.id2() == tchecker::REFCLOCK_ID ? 0 : c.id2() + 1);
    if (tchecker::dbm::constrain(dbm, dim, id1, id2, c.comparator(), c.value()) == tchecker::dbm::EMPTY)
      return tchecker::dbm::EMPTY;
  }
  return tchecker::dbm::NON_EMPTY;
}

bool is_equal(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim)
{
  assert(dbm1 != nullptr);
  assert(dbm2 != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_tight(dbm1, dim));
  assert(tchecker::dbm::is_tight(dbm2, dim));

  for (tchecker::clock_id_t i = 0; i < dim; ++i)
    for (tchecker::clock_id_t j = 0; j < dim; ++j)
      if (DBM1(i, j) != DBM2(i, j))
        return false;
  return true;
}

bool is_le(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim)
{
  assert(dbm1 != nullptr);
  assert(dbm2 != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_tight(dbm1, dim));
  assert(tchecker::dbm::is_tight(dbm2, dim));

  for (tchecker::clock_id_t i = 0; i < dim; ++i)
    for (tchecker::clock_id_t j = 0; j < dim; ++j)
      if (DBM1(i, j) > DBM2(i, j))
        return false;
  return true;
}

void reset(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::clock_id_t y,
           tchecker::integer_t value)
{
  if (y == 0)
    reset_to_value(dbm, dim, x, value);
  else if (value == 0)
    reset_to_clock(dbm, dim, x, y);
  else
    reset_to_sum(dbm, dim, x, y, value);
}

void reset(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_reset_container_t const & resets)
{
  for (tchecker::clock_reset_t const & r : resets) {
    tchecker::clock_id_t lid = (r.left_id() == tchecker::REFCLOCK_ID ? 0 : r.left_id() + 1);
    tchecker::clock_id_t rid = (r.right_id() == tchecker::REFCLOCK_ID ? 0 : r.right_id() + 1);
    tchecker::dbm::reset(dbm, dim, lid, rid, r.value());
  }
}

void reset_to_value(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::integer_t value)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
  assert(x < dim);
  assert(0 <= value);

  // set x == value
  DBM(x, 0) = tchecker::dbm::db(tchecker::LE, value);
  DBM(0, x) = tchecker::dbm::db(tchecker::LE, -value);

  // tighten: x->y is set to x->0->y and y->x to y->0->x for all y!=0
  for (tchecker::clock_id_t y = 1; y < dim; ++y) {
    DBM(x, y) = tchecker::dbm::sum(DBM(x, 0), DBM(0, y));
    DBM(y, x) = tchecker::dbm::sum(DBM(y, 0), DBM(0, x));
  }

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

void reset_to_clock(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::clock_id_t y)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
  assert(x < dim);
  assert(0 < y);
  assert(y < dim);

  // x is identified to y w.r.t. all clocks z
  for (tchecker::clock_id_t z = 0; z < dim; ++z) {
    DBM(x, z) = DBM(y, z);
    DBM(z, x) = DBM(z, y);
  }
  DBM(x, x) = tchecker::dbm::LE_ZERO; // cheaper than testing in loop

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

void reset_to_sum(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::clock_id_t y,
                  tchecker::integer_t value)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
  assert(x < dim);
  assert(y < dim);
  assert(0 <= value);

  // NB: this is a reset not a constraint. Hence, resetting x:=y+value
  // only impacts difference bounds involving x (i.e. x-z and z-x
  // for all z including y and 0). Other difference bounds u-v for
  // any u,v distinct from x are not touched since the values of u
  // and v are not modified when x is reset to y+value

  // If x == y:
  // x' - x <= v & x - z <= d & z = z' --> x' - z' <= d+v
  // x - x' <= -v & z - x <= d & z = z' --> z' - x' <= d-v

  // If x != y:
  // x - y <= v & y - z <= d --> x - z <= d+v
  // y - x <= -v & z - y <= d --> z - x <= d-v
  for (tchecker::clock_id_t z = 0; z < dim; ++z) {
    DBM(x, z) = tchecker::dbm::add(DBM(y, z), value);
    DBM(z, x) = tchecker::dbm::add(DBM(z, y), -value);
  }
  DBM(x, x) = tchecker::dbm::LE_ZERO; // cheaper than testing in loop

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

void free_clock(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
  assert(x < dim);

  // The naive algorithm consists in setting all constraint involving x to <=inf, then tighten
  // Observe that the new bound for y-x can be obtained as the sum of bounds for y-0 and 0-x.
  // But 0-x is <=0 (due to unreset), so the bound for y-x is simply the same as for y-0
  for (tchecker::clock_id_t y = 0; y < dim; ++y) {
    DBM(x, y) = tchecker::dbm::LT_INFINITY;
    DBM(y, x) = DBM(y, 0);
  }
  DBM(x, x) = tchecker::dbm::LE_ZERO;

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

void free_clock(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_reset_t const & reset)
{
  assert(reset.left_id() != tchecker::REFCLOCK_ID);
  tchecker::clock_id_t const x = reset.left_id() + 1; // translation of clock id from system to dbm
  tchecker::dbm::free_clock(dbm, dim, x);
}

void free_clock(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_reset_container_t const & resets)
{
  for (tchecker::clock_reset_t const & reset : resets)
    free_clock(dbm, dim, reset);
}

void open_up(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));

  for (tchecker::clock_id_t i = 1; i < dim; ++i)
    DBM(i, 0) = tchecker::dbm::LT_INFINITY;

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

void open_down(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));

  // The new dbm is obtained by setting all DBM(0, i) to <=0, then tightening
  // This is equivalent to setting DBM(0,i) to the min of all DBM(j,i)
  for (tchecker::clock_id_t i = 1; i < dim; ++i) {
    tchecker::dbm::db_t min = tchecker::dbm::LT_INFINITY;
    for (tchecker::clock_id_t j = 1; j < dim; ++j)
      min = tchecker::dbm::min(min, DBM(j, i));
    DBM(0, i) = min;
  }

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

enum tchecker::dbm::status_t intersection(tchecker::dbm::db_t * dbm, tchecker::dbm::db_t const * dbm1,
                                          tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim)
{
  assert(dim >= 1);
  assert(dbm != nullptr);
  assert(dbm1 != nullptr);
  assert(dbm2 != nullptr);
  assert(tchecker::dbm::is_consistent(dbm1, dim));
  assert(tchecker::dbm::is_consistent(dbm2, dim));
  assert(tchecker::dbm::is_tight(dbm1, dim));
  assert(tchecker::dbm::is_tight(dbm2, dim));

  for (tchecker::clock_id_t i = 0; i < dim; ++i)
    for (tchecker::clock_id_t j = 0; j < dim; ++j)
      DBM(i, j) = tchecker::dbm::min(DBM1(i, j), DBM2(i, j));

  return tchecker::dbm::tighten(dbm, dim);
}

void extra_m(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t const * m)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_positive(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));

  bool modified = false;

  // let DBM(i,j) be (#,cij)
  // DBM(i,j) becomes  <inf    if  cij > M(i)
  //                   <-M(j)  if -cij > M(j)
  //          unchanged otherwise

  // i=0 (first row), only the second case applies
  for (tchecker::clock_id_t j = 1; j < dim; ++j) {
    tchecker::integer_t Mj = M(j);
    assert(Mj < tchecker::dbm::INF_VALUE);
    if (DBM(0, j) == tchecker::dbm::LE_ZERO)
      continue;
    tchecker::integer_t c0j = tchecker::dbm::value(DBM(0, j));
    if (-c0j > Mj) {
      DBM(0, j) = (Mj == -tchecker::dbm::INF_VALUE ? tchecker::dbm::LE_ZERO : tchecker::dbm::db(tchecker::LT, -Mj));
      modified = true;
    }
  }

  // i>0, both cases apply
  for (tchecker::clock_id_t i = 1; i < dim; ++i) {
    tchecker::integer_t Mi = M(i);
    assert(Mi < tchecker::dbm::INF_VALUE);

    for (tchecker::clock_id_t j = 0; j < dim; ++j) {
      tchecker::integer_t Mj = M(j);
      assert(Mj < tchecker::dbm::INF_VALUE);

      if (i == j)
        continue;
      if (DBM(i, j) == tchecker::dbm::LT_INFINITY)
        continue;

      tchecker::integer_t cij = tchecker::dbm::value(DBM(i, j));
      if (cij > Mi) {
        DBM(i, j) = tchecker::dbm::LT_INFINITY;
        modified = true;
      }
      else if (-cij > Mj) {
        DBM(i, j) = (Mj == -tchecker::dbm::INF_VALUE ? tchecker::dbm::LT_INFINITY : tchecker::dbm::db(tchecker::LT, -Mj));
        modified = true;
      }
    }
  }

  if (modified)
    tchecker::dbm::tighten(dbm, dim);

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_positive(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

void extra_m_plus(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t const * m)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_positive(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));

  bool modified = false;

  // let DBM(i,j) be (#,cij)
  // DBM(i,j) becomes  <inf    if  cij > M(i)
  //                   <inf    if -c0i > M(i)
  //                   <inf    if -c0j > M(j), i != 0
  //                   <-M(j)  if -cij > M(j), i  = 0
  //          unchanged otherwise

  // NB: the first line (i.e. 0-line) must be modified last to keep
  // c0i and c0j intact for 2nd and 3rd cases

  // i > 0, all cases except the 4th apply
  for (tchecker::clock_id_t i = 1; i < dim; ++i) {
    tchecker::integer_t Mi = M(i);
    assert(Mi < tchecker::dbm::INF_VALUE);

    tchecker::integer_t c0i = tchecker::dbm::value(DBM(0, i));

    if (-c0i > Mi) { // 2nd case
      for (tchecker::clock_id_t j = 0; j < dim; ++j) {
        if (i == j)
          continue;
        if (DBM(i, j) == tchecker::dbm::LT_INFINITY)
          continue;

        DBM(i, j) = tchecker::dbm::LT_INFINITY;
        modified = true;
      }
    }
    else { // 1st and 3rd cases apply
      for (tchecker::clock_id_t j = 0; j < dim; ++j) {
        tchecker::integer_t Mj = M(j);
        assert(Mj < tchecker::dbm::INF_VALUE);

        if (i == j)
          continue;
        if (DBM(i, j) == tchecker::dbm::LT_INFINITY)
          continue;

        tchecker::integer_t c0j = tchecker::dbm::value(DBM(0, j));
        tchecker::integer_t cij = tchecker::dbm::value(DBM(i, j));

        if (cij > Mi || -c0j > Mj) {
          DBM(i, j) = tchecker::dbm::LT_INFINITY;
          modified = true;
        }
      }
    }
  }

  // i = 0, only the 4th case apply
  assert(M(0) < tchecker::dbm::INF_VALUE);
  for (tchecker::clock_id_t j = 1; j < dim; ++j) {
    tchecker::integer_t Mj = M(j);
    assert(Mj < tchecker::dbm::INF_VALUE);

    tchecker::integer_t c0j = tchecker::dbm::value(DBM(0, j));
    if (-c0j > Mj) {
      DBM(0, j) = (Mj == -tchecker::dbm::INF_VALUE ? tchecker::dbm::LE_ZERO : tchecker::dbm::db(tchecker::LT, -Mj));
      modified = true;
    }
  }

  if (modified)
    tchecker::dbm::tighten(dbm, dim);

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_positive(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

void extra_lu(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t const * l, tchecker::integer_t const * u)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_positive(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));

  bool modified = false;

  // let DBM(i,j) be (#,cij)
  // DBM(i,j) becomes  <inf    if  cij > L(i)
  //                   <-U(j)  if -cij > U(j)
  //          unchanged otherwise

  // i=0 (first row), only second case applies
  for (tchecker::clock_id_t j = 1; j < dim; ++j) {
    tchecker::integer_t Uj = U(j);
    assert(Uj < tchecker::dbm::INF_VALUE);
    if (DBM(0, j) == tchecker::dbm::LE_ZERO)
      continue;
    tchecker::integer_t c0j = tchecker::dbm::value(DBM(0, j));
    if (-c0j > Uj) {
      DBM(0, j) = (Uj == -tchecker::dbm::INF_VALUE ? tchecker::dbm::LE_ZERO : tchecker::dbm::db(tchecker::LT, -Uj));
      modified = true;
    }
  }

  // i>0, both cases apply
  for (tchecker::clock_id_t i = 1; i < dim; ++i) {
    tchecker::integer_t Li = L(i);
    assert(Li < tchecker::dbm::INF_VALUE);
    assert(U(i) < tchecker::dbm::INF_VALUE);

    for (tchecker::clock_id_t j = 0; j < dim; ++j) {
      assert(L(j) < tchecker::dbm::INF_VALUE);
      tchecker::integer_t Uj = U(j);
      assert(Uj < tchecker::dbm::INF_VALUE);

      if (i == j)
        continue;
      if (DBM(i, j) == tchecker::dbm::LT_INFINITY)
        continue;

      tchecker::integer_t cij = tchecker::dbm::value(DBM(i, j));
      if (cij > Li) {
        DBM(i, j) = tchecker::dbm::LT_INFINITY;
        modified = true;
      }
      else if (-cij > Uj) {
        DBM(i, j) = (Uj == -tchecker::dbm::INF_VALUE ? tchecker::dbm::LT_INFINITY : tchecker::dbm::db(tchecker::LT, -Uj));
        modified = true;
      }
    }
  }

  if (modified)
    tchecker::dbm::tighten(dbm, dim);

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_positive(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

void extra_lu_plus(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t const * l,
                   tchecker::integer_t const * u)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_positive(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));

  bool modified = false;

  // let DBM(i,j) be (#,cij)
  // DBM(i,j) becomes  <inf    if  cij > L(i)
  //                   <inf    if -c0i > L(i)
  //                   <inf    if -c0j > U(j), i != 0
  //                   <-U(j)  if -c0j > U(j), i  = 0
  //          unchanged otherwise

  // NB: the first line (i.e. 0-line) must be modified last to keep
  // c0i and c0j intact for 2nd, 3rd and 4th cases

  // i > 0, all cases except the 4th apply
  for (tchecker::clock_id_t i = 1; i < dim; ++i) {
    tchecker::integer_t Li = L(i);
    assert(Li < tchecker::dbm::INF_VALUE);
    assert(U(i) < tchecker::dbm::INF_VALUE);

    tchecker::integer_t c0i = tchecker::dbm::value(DBM(0, i));

    if (-c0i > Li) { // 2nd case
      for (tchecker::clock_id_t j = 0; j < dim; ++j) {
        if (i == j)
          continue;
        if (DBM(i, j) == tchecker::dbm::LT_INFINITY)
          continue;

        DBM(i, j) = tchecker::dbm::LT_INFINITY;
        modified = true;
      }
    }
    else { // 1st and 3rd cases apply
      for (tchecker::clock_id_t j = 0; j < dim; ++j) {
        assert(L(j) < tchecker::dbm::INF_VALUE);
        tchecker::integer_t Uj = U(j);
        assert(Uj < tchecker::dbm::INF_VALUE);

        if (i == j)
          continue;
        if (DBM(i, j) == tchecker::dbm::LT_INFINITY)
          continue;

        tchecker::integer_t c0j = tchecker::dbm::value(DBM(0, j));
        tchecker::integer_t cij = tchecker::dbm::value(DBM(i, j));

        if (cij > Li || -c0j > Uj) {
          DBM(i, j) = tchecker::dbm::LT_INFINITY;
          modified = true;
        }
      }
    }
  }

  // i = 0, only the 4th case apply
  assert(L(0) < tchecker::dbm::INF_VALUE);
  assert(U(0) < tchecker::dbm::INF_VALUE);

  for (tchecker::clock_id_t j = 1; j < dim; ++j) {
    assert(L(j) < tchecker::dbm::INF_VALUE);
    tchecker::integer_t Uj = U(j);
    assert(Uj < tchecker::dbm::INF_VALUE);

    tchecker::integer_t c0j = tchecker::dbm::value(DBM(0, j));
    if (-c0j > Uj) {
      DBM(0, j) = (Uj == -tchecker::dbm::INF_VALUE ? tchecker::dbm::LE_ZERO : tchecker::dbm::db(tchecker::LT, -Uj));
      modified = true;
    }
  }

  if (modified)
    tchecker::dbm::tighten(dbm, dim);

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_positive(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

bool is_alu_le(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim,
               tchecker::integer_t const * l, tchecker::integer_t const * u)
{
  assert(dbm1 != nullptr);
  assert(dbm2 != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm1, dim));
  assert(tchecker::dbm::is_consistent(dbm2, dim));
  assert(tchecker::dbm::is_positive(dbm1, dim));
  assert(tchecker::dbm::is_positive(dbm2, dim));
  assert(tchecker::dbm::is_tight(dbm1, dim));
  assert(tchecker::dbm::is_tight(dbm2, dim));

  // dbm1 not included in aLU(dbm2) if there is x and y s.t.
  //     dbm1[0x] >= (<= -U(x))
  // &&  dbm2[yx] < dbm1[yx]
  // &&  dbm2[yx] + (< -L(y)) < dbm1[0x]

  for (tchecker::clock_id_t x = 0; x < dim; ++x) {
    tchecker::integer_t Ux = U(x);
    assert(Ux < tchecker::dbm::INF_VALUE);

    // Skip x as 1st condition cannot be satisfied
    if (Ux == -tchecker::dbm::INF_VALUE)
      continue;

    // Check 1st condition
    if (DBM1(0, x) < tchecker::dbm::db(tchecker::LE, -Ux))
      continue;

    for (tchecker::clock_id_t y = 0; y < dim; ++y) {
      tchecker::integer_t Ly = L(y);
      assert(Ly < tchecker::dbm::INF_VALUE);

      if (x == y)
        continue;

      // Skip y as 3rd condition cannot be satisfied
      if (Ly == -tchecker::dbm::INF_VALUE)
        continue;

      // Check 2nd and 3rd conditions
      if (DBM2(y, x) < DBM1(y, x) && tchecker::dbm::sum(DBM2(y, x), tchecker::dbm::db(tchecker::LT, -Ly)) < DBM1(0, x))
        return false;
    }
  }

  return true;
}

bool is_am_le(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim,
              tchecker::integer_t const * m)
{
  return tchecker::dbm::is_alu_le(dbm1, dbm2, dim, m, m);
}

std::size_t hash(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
{
  std::size_t seed = 0;
  for (tchecker::clock_id_t k = 0; k < dim * dim; ++k)
    boost::hash_combine(seed, tchecker::dbm::hash(dbm[k]));
  return seed;
}

std::ostream & output_matrix(std::ostream & os, tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);

  for (tchecker::clock_id_t i = 0; i < dim; ++i) {
    for (tchecker::clock_id_t j = 0; j < dim; ++j) {
      tchecker::dbm::output(os, DBM(i, j));
      os << '\t';
    }
    os << std::endl;
  }

  return os;
}

std::ostream & output(std::ostream & os, tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim,
                      std::function<std::string(tchecker::clock_id_t)> clock_name)
{
  assert(dbm != nullptr);
  assert(dim >= 1);

  os << "(";

  // output: x # c (first row/column)
  for (tchecker::clock_id_t j = 1; j < dim; ++j) {
    tchecker::dbm::db_t c0j = DBM(0, j), cj0 = DBM(j, 0);
    if (j > 1)
      os << " && ";
    // xj = k
    if (tchecker::dbm::sum(c0j, cj0) == tchecker::dbm::LE_ZERO)
      os << clock_name(j) << "==" << tchecker::dbm::value(cj0);
    // k1 <= xj <= k2
    else {
      os << -tchecker::dbm::value(c0j) << tchecker::dbm::comparator_str(c0j) << clock_name(j);
      if (cj0 != tchecker::dbm::LT_INFINITY)
        os << tchecker::dbm::comparator_str(cj0) << tchecker::dbm::value(cj0);
    }
  }

  // output: x-y # c (other rows/columns)
  for (tchecker::clock_id_t i = 1; i < dim; ++i) {
    for (tchecker::clock_id_t j = i + 1; j < dim; ++j) {
      tchecker::dbm::db_t cij = DBM(i, j), cji = DBM(j, i);
      // xi == xj + k
      if (tchecker::dbm::sum(cij, cji) == tchecker::dbm::LE_ZERO) {
        os << " && ";
        os << clock_name(i) << "==" << clock_name(j);
        tchecker::integer_t vij = tchecker::dbm::value(cij);
        if (vij > 0)
          os << "+" << tchecker::dbm::value(cij);
        else if (vij < 0)
          os << "-" << -tchecker::dbm::value(cij);
      }
      // k1 <= xi - xj <= k2
      else if ((cij != tchecker::dbm::LT_INFINITY) || (cji != tchecker::dbm::LT_INFINITY)) {
        os << " && ";

        if (cji != tchecker::dbm::LT_INFINITY)
          os << -tchecker::dbm::value(cji) << tchecker::dbm::comparator_str(cji);

        os << clock_name(i) << "-" << clock_name(j);

        if (cij != tchecker::dbm::LT_INFINITY)
          os << tchecker::dbm::comparator_str(cij) << tchecker::dbm::value(cij);
      }
    }
  }

  os << ")";

  return os;
}

int lexical_cmp(tchecker::dbm::db_t const * dbm1, tchecker::clock_id_t dim1, tchecker::dbm::db_t const * dbm2,
                tchecker::clock_id_t dim2)
{
  assert(dbm1 != nullptr);
  assert(dbm2 != nullptr);
  assert(dim1 >= 1);
  assert(dim2 >= 1);
  return tchecker::lexical_cmp(dbm1, dbm1 + dim1 * dim1, dbm2, dbm2 + dim2 * dim2, tchecker::dbm::db_cmp);
}

// NB: first implementation provided by Ocan Sankur, added overflow/underflow checks (FH)
void scale_up(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t factor)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));

  if (factor <= 0)
    throw std::invalid_argument("tchecker::dbm::scale_up: factor should be positive");

  for (tchecker::clock_id_t x = 0; x < dim; ++x) {
    for (tchecker::clock_id_t y = 0; y < dim; ++y) {
      if (DBM(x, y) == tchecker::dbm::LT_INFINITY)
        continue;
      tchecker::integer_t value = tchecker::dbm::value(DBM(x, y));

      if (tchecker::dbm::MAX_VALUE / factor < value)
        throw std::overflow_error("tchecker::dbm::scale_up: overflow error");
      if (tchecker::dbm::MIN_VALUE / factor > value)
        throw std::underflow_error("tchecker::dbm::scale_up: underflow error");

      DBM(x, y) = tchecker::dbm::db(tchecker::dbm::comparator(DBM(x, y)), value * factor);
    }
  }

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

void scale_down(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t factor)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));

  if (factor <= 0)
    throw std::invalid_argument("tchecker::dbm::scale_down: factor should be positive");

  for (tchecker::clock_id_t x = 0; x < dim; ++x) {
    for (tchecker::clock_id_t y = 0; y < dim; ++y) {
      if (DBM(x, y) == tchecker::dbm::LT_INFINITY)
        continue;
      tchecker::integer_t value = tchecker::dbm::value(DBM(x, y));
      if (value % factor != 0)
        throw std::invalid_argument("tchecker::dbm::scale_down: dbm is not divisible by factor");

      DBM(x, y) = tchecker::dbm::db(tchecker::dbm::comparator(DBM(x, y)), value / factor);
    }
  }

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}

bool has_fixed_value(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
  assert(x < dim);

  return (tchecker::dbm::comparator(DBM(x, 0)) == tchecker::LE) && (tchecker::dbm::comparator(DBM(0, x)) == tchecker::LE) &&
         (tchecker::dbm::value(DBM(0, x)) == -tchecker::dbm::value(DBM(x, 0)));
}

bool admits_integer_value(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
  assert(x < dim);

  return (DBM(x, 0) == tchecker::dbm::LT_INFINITY) ||
         !((tchecker::dbm::comparator(DBM(x, 0)) == tchecker::LT) && (tchecker::dbm::comparator(DBM(0, x)) == tchecker::LT) &&
           (-(tchecker::dbm::value(DBM(x, 0)) - 1) == tchecker::dbm::value(DBM(0, x))));
}

// NB: implementation provided by Ocan Sankur
bool is_single_valuation(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));

  for (tchecker::clock_id_t x = 1; x < dim; ++x)
    if (!tchecker::dbm::has_fixed_value(dbm, dim, x))
      return false;

  return true;
}

/* NB: first implementation provided by Ocan Sankur, several modifications (FH)
 * Algorithm is as follows:
 * 1- check if some clock does not admit an integer value (e.g. 0<x<1)
 * 2- if yes, scale up the dbm
 * 3- then, choose a clock that does not have a fixed value yet, and choose an integer value for this clock
 * 4- repeat from step (1) until every clock has a fixed value (i.e. dbm is single valued)
 * NB: the scale up amounts to assign rational values to clocks, hence the returned factor is the
 * denominator of the value of each clock in the resulting DBM
 *
 * To illustrate, consider the constraints: (1<x<3 && 2<y<4 && 1<y-x<3). Observe that x and y admit an integer
 * value: x=2 on the one hand, and y=3 on the other hand. But those value are not compatible w.r.t. the
 * constraints over y-x. So picking value x=2 result in the constraints: (2<=x<=2 && 3<y<4 && 1<y-x<2).
 * We first need to scale up the DBM, say by factor 2: (4<=x<=4 && 6<y<8 && 2<y-x<4). Now we can choose y=7,
 * which yields the dbm: (4<=x<=4 && 7<=y<=7 && 3<=y-x<=3) with factor=2, which encods the clock valuation
 * x=4/2=2 and y=7/2
 */
tchecker::integer_t constrain_to_single_valuation(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
  assert(tchecker::dbm::is_positive(dbm, dim));

  tchecker::integer_t factor = 1;

  for (tchecker::clock_id_t x = 1; x < dim; ++x) {
    // find clock which has not fixed value yet
    if (tchecker::dbm::has_fixed_value(dbm, dim, x))
      continue;

    // if selected clock does not admit integer value, scale dbm up
    if (!tchecker::dbm::admits_integer_value(dbm, dim, x)) {
      if (factor > std::numeric_limits<tchecker::integer_t>::max() / 2)
        throw std::overflow_error("tchecker::dbm::constrain_to_single_valuation: scale factor overflow");
      factor *= 2;
      tchecker::dbm::scale_up(dbm, dim, factor);
    }

    // then, choose integer value for selected clock
    if (tchecker::dbm::value(DBM(0, x)) < -tchecker::dbm::MAX_VALUE)
      throw std::overflow_error("tchecker::dbm::constrain_to_single_valuation: integer value overflow");

    tchecker::integer_t v = -tchecker::dbm::value(DBM(0, x)) + (tchecker::dbm::comparator(DBM(0, x)) == tchecker::LE ? 0 : 1);
    tchecker::dbm::constrain(dbm, dim, 0, x, tchecker::LE, -v);
    tchecker::dbm::constrain(dbm, dim, x, 0, tchecker::LE, v);
  }

  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));

  return factor;
}

tchecker::integer_t gcd(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));

  tchecker::integer_t d = 0;
  for (tchecker::clock_id_t x = 0; x < dim; ++x)
    for (tchecker::clock_id_t y = 0; y < dim; ++y) {
      if (DBM(x, y) == tchecker::dbm::LT_INFINITY)
        continue;
      d = std::gcd(d, tchecker::dbm::value(DBM(x, y)));
      if (d == 1)
        break;
    }

  return d;
}

bool satisfies(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim, tchecker::clockval_t const & clockval)
{
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
  assert(clockval.size() == dim);

  for (tchecker::clock_id_t x = 0; x < dim; ++x)
    for (tchecker::clock_id_t y = 0; y < dim; ++y) {
      if (x == y)
        continue;
      if (DBM(x, y) == tchecker::dbm::LT_INFINITY)
        continue;
      if (!tchecker::satisfies(clockval, x, y, tchecker::dbm::comparator(DBM(x, y)), tchecker::dbm::value(DBM(x, y))))
        return false;
    }
  return true;
}

} // end of namespace dbm

} // end of namespace tchecker
