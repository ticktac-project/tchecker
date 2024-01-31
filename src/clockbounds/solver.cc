/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <cstring>
#include <unordered_set>

#include "tchecker/clockbounds/solver.hh"
#include "tchecker/expression/static_analysis.hh"
#include "tchecker/statement/clock_updates.hh"
#include "tchecker/statement/static_analysis.hh"
#include "tchecker/utils/log.hh"
namespace tchecker {

namespace clockbounds {

df_solver_t::df_solver_t(tchecker::ta::system_t const & system)
    : _loc_number(system.locations_count()), _clock_number(system.clock_variables().size(tchecker::VK_FLATTENED)),
      _loc_pid(_loc_number, 0),
      _dim(1 + _loc_number * _clock_number), // 1 variable for each clock in each location, plus 1 for dummy clock 0
      _L(nullptr), _U(nullptr), _updated_L(false), _updated_U(false)
{
  if ((_loc_number > 0) && (_clock_number > 0) && ((_dim < _loc_number) || (_dim < _clock_number)))
    throw std::invalid_argument("invalid number of clocks or locations (overflow)");

  for (tchecker::loc_id_t id = 0; id < _loc_number; ++id)
    _loc_pid[id] = system.location(id)->pid();

  _L = new tchecker::dbm::db_t[_dim * _dim];
  _U = new tchecker::dbm::db_t[_dim * _dim];
  clear();
}

df_solver_t::df_solver_t(tchecker::clockbounds::df_solver_t const & solver)
    : _loc_number(solver._loc_number), _clock_number(solver._clock_number), _loc_pid(solver._loc_pid), _dim(solver._dim),
      _L(nullptr), _U(nullptr), _updated_L(solver._updated_L), _updated_U(solver._updated_U)
{
  _L = new tchecker::dbm::db_t[_dim * _dim];
  _U = new tchecker::dbm::db_t[_dim * _dim];
  memcpy(_L, solver._L, _dim * _dim * sizeof(*_L));
  memcpy(_U, solver._U, _dim * _dim * sizeof(*_U));
}

df_solver_t::df_solver_t(tchecker::clockbounds::df_solver_t && solver)
    : _loc_number(std::move(solver._loc_number)), _clock_number(std::move(solver._clock_number)),
      _loc_pid(std::move(solver._loc_pid)), _dim(std::move(solver._dim)), _L(std::move(solver._L)), _U(std::move(solver._U)),
      _updated_L(std::move(solver._updated_L)), _updated_U(std::move(solver._updated_U))
{
  solver._loc_number = 0;
  solver._clock_number = 0;
  solver._dim = 1;
  solver._L = nullptr;
  solver._U = nullptr;
  solver._updated_L = false;
  solver._updated_U = false;
}

df_solver_t::~df_solver_t()
{
  delete[] _L;
  delete[] _U;
}

tchecker::clockbounds::df_solver_t & df_solver_t::operator=(tchecker::clockbounds::df_solver_t const & solver)
{
  if (this != &solver) {
    _loc_number = solver._loc_number;
    _clock_number = solver._clock_number;
    _loc_pid = solver._loc_pid;
    _dim = solver._dim;

    delete[] _L;
    _L = new tchecker::dbm::db_t[_dim * _dim];
    memcpy(_L, solver._L, _dim * _dim * sizeof(*_L));

    delete[] _U;
    _U = new tchecker::dbm::db_t[_dim * _dim];
    memcpy(_U, solver._U, _dim * _dim * sizeof(*_U));

    _updated_L = solver._updated_L;
    _updated_U = solver._updated_U;
  }

  return *this;
}

tchecker::clockbounds::df_solver_t & df_solver_t::operator=(tchecker::clockbounds::df_solver_t && solver)
{
  if (this != &solver) {
    _loc_number = std::move(solver._loc_number);
    _clock_number = std::move(solver._clock_number);
    _loc_pid = std::move(solver._loc_pid);
    _dim = std::move(solver._dim);
    _L = std::move(solver._L);
    _U = std::move(solver._U);
    _updated_L = std::move(solver._updated_L);
    _updated_U = std::move(solver._updated_U);

    solver._loc_number = 0;
    solver._clock_number = 0;
    solver._dim = 1;
    solver._L = nullptr;
    solver._U = nullptr;
    solver._updated_L = false;
    solver._updated_U = false;
  }

  return *this;
}

tchecker::clock_id_t df_solver_t::clock_number() const { return _clock_number; }

tchecker::clock_id_t df_solver_t::loc_number() const { return _loc_number; }

void df_solver_t::clear()
{
  // <inf everywhere except <=0 on diagonal
  for (tchecker::clock_id_t x = 0; x < _dim; ++x) {
    for (tchecker::clock_id_t y = 0; y < _dim; ++y) {
      _L[x * _dim + y] = tchecker::dbm::LT_INFINITY;
      _U[x * _dim + y] = tchecker::dbm::LT_INFINITY;
    }
    _L[x * _dim + x] = tchecker::dbm::LE_ZERO;
    _U[x * _dim + x] = tchecker::dbm::LE_ZERO;
  }

  _updated_L = false;
  _updated_U = false;
}

void df_solver_t::add_lower_bound_guard(tchecker::loc_id_t l, tchecker::clock_id_t x, tchecker::integer_t c)
{
  assert(l < _loc_number);
  assert(x < _clock_number);
  // L_{l, x} >= c  (i.e. 0 - L_{l, x} <= -c)
  updateL(0, index(l, x), tchecker::LE, -c);
}

void df_solver_t::add_upper_bound_guard(tchecker::loc_id_t l, tchecker::clock_id_t x, tchecker::integer_t c)
{
  assert(l < _loc_number);
  assert(x < _clock_number);
  // U_{l, x} >= c  (i.e. 0 - U_{l ,x} <= -c)
  updateU(0, index(l, x), tchecker::LE, -c);
}

void df_solver_t::add_assignment(tchecker::loc_id_t l1, tchecker::loc_id_t l2, tchecker::clock_id_t x, tchecker::clock_id_t y,
                                 tchecker::integer_t c)
{
  assert(l1 < _loc_number);
  assert(l2 < _loc_number);
  assert(x < _clock_number);
  assert(y < _clock_number || y == tchecker::REFCLOCK_ID);

  // If const assignment, nothing to do
  if (y == tchecker::REFCLOCK_ID)
    return;

  // Propagation over the edge: L_{l2,x} - L_{l1,y} <= c / U_{l2,x} - U_{l1,xy} <= c
  updateL(index(l2, x), index(l1, y), tchecker::LE, c);
  updateU(index(l2, x), index(l1, y), tchecker::LE, c);

  // We need to propagate shared variables only when x != y
  if (x == y)
    return;

  // Propagation across processes: L_{m,x} - L_{l1,y} <= c / U_{m,x} - U_{l1,y} <= c
  // for every location m in another process
  for (tchecker::loc_id_t m = 0; m < _loc_number; ++m)
    if (_loc_pid[m] != _loc_pid[l1]) {
      updateL(index(m, x), index(l1, y), tchecker::LE, c);
      updateU(index(m, x), index(l1, y), tchecker::LE, c);
    }
}

bool df_solver_t::solve(tchecker::clockbounds::local_lu_map_t & map)
{
  if (_clock_number != map.clock_number())
    throw std::invalid_argument("*** solve: invalid number of clocks");
  if (_loc_number != map.loc_number())
    throw std::invalid_argument("*** solve: invalid number of locations");

  bool consistent = ensure_tight();
  if (!consistent)
    return false;

  for (tchecker::loc_id_t loc = 0; loc < _loc_number; ++loc)
    for (tchecker::clock_id_t clock = 0; clock < _clock_number; ++clock) {
      map.L(loc)[clock] = L(loc, clock);
      map.U(loc)[clock] = U(loc, clock);
    }

  return true;
}

std::size_t df_solver_t::index(tchecker::loc_id_t l, tchecker::clock_id_t x) const
{
  assert(l < _loc_number);
  assert(x < _clock_number);
  return 1 + l * _clock_number + x;
}

tchecker::clockbounds::bound_t df_solver_t::L(tchecker::loc_id_t l, tchecker::clock_id_t x) const
{
  assert(l < _loc_number);
  assert(x < _clock_number);
  assert(!_updated_L);
  assert(!tchecker::dbm::is_empty_0(_L, _dim));
  tchecker::dbm::db_t db = _L[0 * _dim + index(l, x)];
  return (db == tchecker::dbm::LT_INFINITY ? tchecker::clockbounds::NO_BOUND : -tchecker::dbm::value(db));
}

tchecker::clockbounds::bound_t df_solver_t::U(tchecker::loc_id_t l, tchecker::clock_id_t x) const
{
  assert(l < _loc_number);
  assert(x < _clock_number);
  assert(!_updated_U);
  assert(!tchecker::dbm::is_empty_0(_U, _dim));
  tchecker::dbm::db_t db = _U[0 * _dim + index(l, x)];
  return (db == tchecker::dbm::LT_INFINITY ? tchecker::clockbounds::NO_BOUND : -tchecker::dbm::value(db));
}

void df_solver_t::updateL(tchecker::clock_id_t i, tchecker::clock_id_t j, tchecker::ineq_cmp_t cmp, tchecker::integer_t value)
{
  assert(i < _dim);
  assert(j < _dim);
  tchecker::dbm::db_t bound = tchecker::dbm::db(cmp, value);
  if (bound < _L[i * _dim + j]) {
    _L[i * _dim + j] = bound;
    _updated_L = true;
  }
}

void df_solver_t::updateU(tchecker::clock_id_t i, tchecker::clock_id_t j, tchecker::ineq_cmp_t cmp, tchecker::integer_t value)
{
  assert(i < _dim);
  assert(j < _dim);
  tchecker::dbm::db_t bound = tchecker::dbm::db(cmp, value);
  if (bound < _U[i * _dim + j]) {
    _U[i * _dim + j] = bound;
    _updated_U = true;
  }
}

bool df_solver_t::ensure_tight()
{
  bool consistent = !tchecker::dbm::is_empty_0(_L, _dim) && !tchecker::dbm::is_empty_0(_U, _dim);

  if (_updated_L)
    consistent &= (tchecker::dbm::tighten(_L, _dim) != tchecker::dbm::EMPTY);
  if (_updated_U)
    consistent &= (tchecker::dbm::tighten(_U, _dim) != tchecker::dbm::EMPTY);

  _updated_L = false;
  _updated_U = false;

  return consistent;
}

/*!
\class df_solver_expr_updater_t
\brief Update solver constraints from expressions
\see tchecker::clockbounds::df_solver_t for constraints generated from expressions
*/
class df_solver_expr_updater_t : public tchecker::typed_expression_visitor_t {
public:
  /*!
  \brief Constructor
  \param src : source location identifier
  \param tgt : target location identifier
  \param solver : a solver
  \note this updates constraints in solver for locations src and tgt
  */
  df_solver_expr_updater_t(tchecker::loc_id_t src, tchecker::loc_id_t tgt,
                           std::shared_ptr<tchecker::clockbounds::df_solver_t> const & solver)
      : _src(src), _tgt(tgt), _solver(solver)
  {
  }

  /*!
  \brief Copy constructor
  \param updater : an updater
  \post this is a copy of updater
  */
  df_solver_expr_updater_t(tchecker::clockbounds::df_solver_expr_updater_t const & updater)
      : _src(updater._src), _tgt(updater._tgt), _solver(updater._solver)
  {
  }

  /*!
  \brief Move constructor
  \param updater : an updater
  \post updater has been moved to this
  */
  df_solver_expr_updater_t(tchecker::clockbounds::df_solver_expr_updater_t && updater)
      : _src(std::move(updater._src)), _tgt(std::move(updater._tgt)), _solver(std::move(updater._solver))
  {
  }

  /*!
  \brief Destructor
  */
  virtual ~df_solver_expr_updater_t() = default;

  /*!
  \brief Assignment operator
  \param updater : an updater
  \post this is a copy of updater
  \return this
  */
  tchecker::clockbounds::df_solver_expr_updater_t & operator=(tchecker::clockbounds::df_solver_expr_updater_t const & updater)
  {
    if (this != &updater) {
      _src = updater._src;
      _tgt = updater._tgt;
      _solver = updater._solver;
    }
    return *this;
  }

  /*!
  \brief Move assignment operator
  \param updater : an updater
  \post updater has been moved to this
  \return this
  */
  tchecker::clockbounds::df_solver_expr_updater_t & operator=(tchecker::clockbounds::df_solver_expr_updater_t && updater)
  {
    if (this != &updater) {
      _src = std::move(updater._src);
      _tgt = std::move(updater._tgt);
      _solver = std::move(updater._solver);
    }
    return *this;
  }

  /*!
  \brief Visitor
  \post left and right operand have been visited if expr is a logical-and expression
  */
  virtual void visit(tchecker::typed_binary_expression_t const & expr) override
  {
    if (expr.binary_operator() == tchecker::EXPR_OP_LAND) {
      expr.left_operand().visit(*this);
      expr.right_operand().visit(*this);
    }
  }

  /*!
  \brief Visitor
  \post sub-expression has been visited
  */
  virtual void visit(tchecker::typed_par_expression_t const & expr) override { expr.expr().visit(*this); }

  /*!
  \brief Visitor
  \post If expr is a lower-bound guard (x>c, x>=c, x==c), then a constraint on clock lower bounds has been added to
  _solver for every clock x (x could be an array) w.r.t. bound c (using method add_lower_bound_guard). If expr is
  an upper-bound guard (x<c, x<=c, x==c), then a constraint on clock upper bounds has been added to _solver for
  every clock x (x could be an array) w.r.t. bound c (using method add_upper_bound_guard).
  */
  virtual void visit(tchecker::typed_simple_clkconstr_expression_t const & expr) override
  {
    tchecker::range_t<tchecker::clock_id_t> clocks = tchecker::extract_lvalue_variable_ids(expr.clock());
    tchecker::integer_t bound = tchecker::const_evaluate(expr.bound(), tchecker::clockbounds::MAX_BOUND);

    if ((expr.binary_operator() == tchecker::EXPR_OP_LT) || (expr.binary_operator() == tchecker::EXPR_OP_LE) ||
        (expr.binary_operator() == tchecker::EXPR_OP_EQ)) {
      for (tchecker::clock_id_t clock = clocks.begin(); clock != clocks.end(); ++clock)
        _solver->add_upper_bound_guard(_src, clock, bound);
    }

    if ((expr.binary_operator() == tchecker::EXPR_OP_GT) || (expr.binary_operator() == tchecker::EXPR_OP_GE) ||
        (expr.binary_operator() == tchecker::EXPR_OP_EQ)) {
      for (tchecker::clock_id_t clock = clocks.begin(); clock != clocks.end(); ++clock)
        _solver->add_lower_bound_guard(_src, clock, bound);
    }
  }

  /*!
  \brief Visitor
  \throw std::runtime_error : as diagonal constraints are not supported by diagonal-free solver
  */
  virtual void visit(tchecker::typed_diagonal_clkconstr_expression_t const & expr) override
  {
    throw std::runtime_error("unsupported diagonal constraints");
  }

  // Other visitors
  virtual void visit(tchecker::typed_int_expression_t const &) override {}
  virtual void visit(tchecker::typed_var_expression_t const &) override {}
  virtual void visit(tchecker::typed_bounded_var_expression_t const &) override {}
  virtual void visit(tchecker::typed_array_expression_t const &) override {}
  virtual void visit(tchecker::typed_unary_expression_t const &) override {}
  virtual void visit(tchecker::typed_ite_expression_t const &) override {}

protected:
  tchecker::loc_id_t _src;                                     /*!< Source location ID */
  tchecker::loc_id_t _tgt;                                     /*!< Target location ID */
  std::shared_ptr<tchecker::clockbounds::df_solver_t> _solver; /*!< Solver */
};

/* add location/edge constraints to solver */

void add_location_constraints(tchecker::typed_expression_t const & inv, tchecker::loc_id_t loc,
                              std::shared_ptr<tchecker::clockbounds::df_solver_t> const & solver)
{
  tchecker::clockbounds::df_solver_expr_updater_t updater(loc, loc, solver);
  inv.visit(updater);
}

void add_edge_constraints(tchecker::typed_expression_t const & guard, tchecker::typed_statement_t const & stmt,
                          tchecker::loc_id_t src, tchecker::loc_id_t tgt,
                          std::shared_ptr<tchecker::clockbounds::df_solver_t> const & solver)
{
  // guard
  tchecker::clockbounds::df_solver_expr_updater_t updater(src, tgt, solver);
  guard.visit(updater);

  // clock updates from statement
  tchecker::clock_id_t const clock_nb = solver->clock_number();
  tchecker::clock_updates_map_t clock_updates = tchecker::compute_clock_updates(clock_nb, stmt);

  for (tchecker::clock_id_t x = 0; x < clock_nb; ++x) {
    if (clock_updates[x].empty())
      throw std::runtime_error("Cannot compute clock updates from statement");
    for (auto && up : clock_updates[x]) {
      tchecker::integer_t v = tchecker::const_evaluate(up.value(), 0); // 0 yields the strongest constraint on clock bounds
      solver->add_assignment(src, tgt, x, up.clock_id(), v);
    }
  }
}

/* compute_clockbounds */

bool compute_clockbounds(tchecker::ta::system_t const & system, tchecker::clockbounds::clockbounds_t & clockbounds)
{
  try {
    std::shared_ptr<tchecker::clockbounds::df_solver_t> solver{new tchecker::clockbounds::df_solver_t{system}};

    for (tchecker::system::loc_const_shared_ptr_t const & loc : system.locations())
      tchecker::clockbounds::add_location_constraints(system.invariant(loc->id()), loc->id(), solver);

    for (tchecker::system::edge_const_shared_ptr_t const & edge : system.edges())
      tchecker::clockbounds::add_edge_constraints(system.guard(edge->id()), system.statement(edge->id()), edge->src(),
                                                  edge->tgt(), solver);

    bool const has_solution = solver->solve(*clockbounds.local_lu_map());
    if (!has_solution)
      return false;
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << e.what() << std::endl;
    return false;
  }

  tchecker::clockbounds::fill_global_lu_map(*clockbounds.global_lu_map(), *clockbounds.local_lu_map());
  tchecker::clockbounds::fill_local_m_map(*clockbounds.local_m_map(), *clockbounds.local_lu_map());
  tchecker::clockbounds::fill_global_m_map(*clockbounds.global_m_map(), *clockbounds.local_lu_map());

  return true;
}

tchecker::clockbounds::clockbounds_t * compute_clockbounds(tchecker::ta::system_t const & system)
{
  tchecker::clockbounds::clockbounds_t * clockbounds =
      new tchecker::clockbounds::clockbounds_t{static_cast<tchecker::loc_id_t>(system.locations_count()),
                                               static_cast<tchecker::clock_id_t>(system.clocks_count(tchecker::VK_FLATTENED))};

  if (tchecker::clockbounds::compute_clockbounds(system, *clockbounds))
    return clockbounds;

  delete clockbounds;
  return nullptr;
}

} // end of namespace clockbounds

} // end of namespace tchecker
