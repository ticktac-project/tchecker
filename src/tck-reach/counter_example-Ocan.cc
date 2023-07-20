#include "counter_example.hh"

#define DBM(i, j) dbm[(i)*dim + (j)]

namespace tchecker {

namespace tck_reach {

void tchecker::tck_reach::rational_dbm_t::constrain_to_valuation()
{
  auto dim = _dim;
  auto dbm = _dbm;
  assert(!tchecker::dbm::is_empty_0(dbm, dim));
  assert(dbm != nullptr);
  assert(dim >= 1);
  assert(_factor >= 1);
  for (tchecker::clock_id_t c = 1; c < dim; c++) {
    if (tchecker::dbm::comparator(DBM(0, c)) == tchecker::dbm::LE) {
      DBM(c, 0) = tchecker::dbm::db(tchecker::dbm::LE, -tchecker::dbm::value(DBM(0, c)));
      tchecker::dbm::tighten(dbm, dim);
    }
    else if (tchecker::dbm::comparator(DBM(c, 0)) == tchecker::dbm::LE) {
      DBM(0, c) = tchecker::dbm::db(tchecker::dbm::LE, -tchecker::dbm::value(DBM(c, 0)));
      tchecker::dbm::tighten(dbm, dim);
    }
    else if (DBM(c, 0) == tchecker::dbm::LT_INFINITY ||
             tchecker::dbm::value(DBM(c, 0)) > -tchecker::dbm::value(DBM(0, c)) + 1) {
      DBM(0, c) = tchecker::dbm::db(tchecker::dbm::LE, tchecker::dbm::value(DBM(0, c)) - 1);
      DBM(c, 0) = tchecker::dbm::db(tchecker::dbm::LE, -tchecker::dbm::value(DBM(0, c)));
      tchecker::dbm::tighten(dbm, dim);
    }
    else {
      for (tchecker::clock_id_t x = 0; x < dim; x++) {
        for (tchecker::clock_id_t y = 0; y < dim; y++) {
          if (DBM(x, y) != tchecker::dbm::LT_INFINITY) {
            auto new_value = tchecker::dbm::value(DBM(x, y)) * this->SCALE_FACTOR;
            DBM(x, y) = tchecker::dbm::db(tchecker::dbm::comparator(DBM(x, y)), new_value);
          }
        }
      }
      this->_factor *= this->SCALE_FACTOR;
      return constrain_to_valuation();
    }
  }
  assert(tchecker::dbm::is_consistent(dbm, dim));
  assert(tchecker::dbm::is_tight(dbm, dim));
}
void tchecker::tck_reach::rational_dbm_t::simplify()
{
  auto dim = _dim;
  auto dbm = _dbm;
  tchecker::integer_t div = _factor;
  for (tchecker::clock_id_t x = 0; x < dim; x++) {
    for (tchecker::clock_id_t y = 0; y < dim; y++) {
      if (x == y)
        continue;
      if (dbm[x * dim + y] == tchecker::dbm::LT_INFINITY || tchecker::dbm::value(dbm[x * dim + y]) == 0) {
        continue;
      }
      auto v = tchecker::dbm::value(dbm[x * dim + y]);
      while (div >= this->SCALE_FACTOR) {
        if ((v / div) * div == v) {
          break;
        }
        else {
          div /= this->SCALE_FACTOR;
        }
      }
    }
  }
  for (tchecker::clock_id_t x = 0; x < dim; x++) {
    for (tchecker::clock_id_t y = 0; y < dim; y++) {
      if (x == y)
        continue;
      if (dbm[x * dim + y] == tchecker::dbm::LT_INFINITY || tchecker::dbm::value(dbm[x * dim + y]) == 0) {
        continue;
      }
      dbm[x * dim + y] =
          tchecker::dbm::db(tchecker::dbm::comparator(dbm[x * dim + y]), tchecker::dbm::value(dbm[x * dim + y]) / div);
    }
  }
  this->_factor /= div;
}
bool tchecker::tck_reach::rational_dbm_t::is_single_valuation() const
{
  auto dim = _dim;
  auto dbm = _dbm;
  for (tchecker::clock_id_t x = 1; x < dim; x++) {
    if ((tchecker::dbm::comparator(DBM(x, 0)) != tchecker::dbm::LE) ||
        (tchecker::dbm::comparator(DBM(0, x)) != tchecker::dbm::LE) ||
        (tchecker::dbm::value(DBM(0, x)) != -tchecker::dbm::value(DBM(x, 0)))) {
      return false;
    }
  }
  return true;
}
tchecker::tck_reach::valuation tchecker::tck_reach::rational_dbm_t::get_valuation() const
{
  assert(this->is_single_valuation());
  auto dim = _dim;
  auto dbm = _dbm;

  tchecker::tck_reach::valuation v;
  for (tchecker::clock_id_t x = 1; x < dim; x++) {
    v.push_back(tchecker::dbm::value(DBM(x, 0)) / (double)this->factor());
  }
  return v;
}

/*!
 \brief Multiply all constants of the dbm by factor

 \param dbm : a dbm
 \param dim : dimension of dbm
 \param factor target scale factor
 */
static void scale_dbm_up(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t factor)
{
  for (tchecker::clock_id_t x = 0; x < dim; x++) {
    for (tchecker::clock_id_t y = 0; y < dim; y++) {
      if (x == y)
        continue;
      if (dbm[x * dim + y] == tchecker::dbm::LT_INFINITY) {
        continue;
      }
      auto new_value = tchecker::dbm::value(dbm[x * dim + y]) * factor;
      dbm[x * dim + y] = tchecker::dbm::db(tchecker::dbm::comparator(dbm[x * dim + y]), new_value);
    }
  }
}

void concrete_predecessor(tchecker::tck_reach::rational_dbm_t & rdbm, const tchecker::zg::transition_t & transition,
                          bool tgt_delay_allowed, const tchecker::dbm::db_t * predecessor_zone,
                          tchecker::tck_reach::rational_dbm_t & concrete_predecessor_reset)
{
  assert(rdbm.is_single_valuation());
  tchecker::clock_constraint_container_t tgt_invariant = transition.tgt_invariant_container();
  tchecker::clock_constraint_container_t src_invariant = transition.src_invariant_container();
  tchecker::clock_constraint_container_t guard = transition.guard_container();
  tchecker::clock_reset_container_t reset = transition.reset_container();

  for (auto & c : tgt_invariant) {
    c.value() *= rdbm.factor();
  }
  for (auto & c : src_invariant) {
    c.value() *= rdbm.factor();
  }
  for (auto & c : guard) {
    c.value() *= rdbm.factor();
  }
  for (auto & r : reset) {
    r.value() *= rdbm.factor();
  }

  tchecker::dbm::db_t * d = rdbm.dbm();
  auto dim = rdbm.dimension();

  tchecker::dbm::constrain(d, dim, tgt_invariant);
  if (tgt_delay_allowed) {
    tchecker::dbm::open_down(d, dim);
  }

  tchecker::dbm::unreset(d, dim, reset);
  tchecker::dbm::constrain(d, dim, guard);
  tchecker::dbm::constrain(d, dim, src_invariant);
  if (predecessor_zone != nullptr) {
    tchecker::dbm::db_t * scaled_predecessor_zone = new tchecker::dbm::db_t[dim * dim];
    std::memcpy(scaled_predecessor_zone, predecessor_zone, dim * dim * sizeof(tchecker::dbm::db_t));
    scale_dbm_up(scaled_predecessor_zone, dim, rdbm.factor());
    tchecker::dbm::open_up(scaled_predecessor_zone, dim);
    tchecker::dbm::constrain(scaled_predecessor_zone, dim, src_invariant);
    tchecker::dbm::intersection(d, d, scaled_predecessor_zone, dim);
    delete[] scaled_predecessor_zone;
  }
  rdbm.simplify();
  rdbm.constrain_to_valuation();
  concrete_predecessor_reset = rdbm;
  tchecker::dbm::reset(concrete_predecessor_reset.dbm(), concrete_predecessor_reset.dimension(), reset);
}

} // namespace tck_reach
} // namespace tchecker