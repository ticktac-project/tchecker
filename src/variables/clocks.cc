/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <functional>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif
#include <boost/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/expression/expression.hh"
#include "tchecker/expression/static_analysis.hh"
#include "tchecker/expression/typechecking.hh"
#include "tchecker/expression/typed_expression.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/utils/ordering.hh"
#include "tchecker/variables/clocks.hh"

namespace tchecker {

/* clock_variables_t */

tchecker::clock_id_t clock_variables_t::declare(std::string const & name, tchecker::clock_id_t size)
{
  tchecker::clock_info_t info{size};
  tchecker::clock_id_t id =
      tchecker::array_variables_t<tchecker::clock_id_t, tchecker::clock_info_t, tchecker::clock_index_t>::declare(name, info);
  if (id >= tchecker::REFCLOCK_ID || (tchecker::REFCLOCK_ID - id < size))
    throw std::runtime_error("no clock identifier left");
  return id;
}

/* clock_constraint_t */

clock_constraint_t::clock_constraint_t(tchecker::clock_id_t id1, tchecker::clock_id_t id2, enum tchecker::ineq_cmp_t cmp,
                                       tchecker::integer_t value)
    : _id1(id1), _id2(id2), _cmp(cmp), _value(value)
{
  if (_id1 == tchecker::REFCLOCK_ID && _id2 == tchecker::REFCLOCK_ID)
    throw std::invalid_argument("Invalid clock identifiers in clock constraint");
}

bool operator==(tchecker::clock_constraint_t const & c1, tchecker::clock_constraint_t const & c2)
{
  return (c1.id1() == c2.id1() && c1.id2() == c2.id2() && c1.comparator() == c2.comparator() && c1.value() == c2.value());
}

bool operator!=(tchecker::clock_constraint_t const & c1, tchecker::clock_constraint_t const & c2) { return !(c1 == c2); }

std::size_t hash_value(tchecker::clock_constraint_t const & c)
{
  std::size_t h = 0;
  boost::hash_combine(h, c.id1());
  boost::hash_combine(h, c.id2());
  boost::hash_combine(h, c.comparator());
  boost::hash_combine(h, c.value());
  return h;
}

tchecker::clock_constraint_t operator-(tchecker::clock_constraint_t const & c)
{
  if (c.value() == std::numeric_limits<tchecker::integer_t>::min())
    throw std::invalid_argument("clock constraint negation cannot be represented");
  tchecker::ineq_cmp_t neg_cmp = (c.comparator() == tchecker::LE ? tchecker::LT : tchecker::LE);
  return tchecker::clock_constraint_t{c.id2(), c.id1(), neg_cmp, static_cast<tchecker::integer_t>(-c.value())};
}

std::ostream & operator<<(std::ostream & os, tchecker::clock_constraint_t const & c)
{
  os << c._id1 << "-" << c._id2 << (c._cmp == tchecker::LT ? "<" : "<=") << c._value;
  return os;
}

std::ostream & output(std::ostream & os, tchecker::clock_constraint_t const & c, tchecker::clock_index_t const & index)
{
  tchecker::clock_id_t id1 = c.id1(), id2 = c.id2();
  if (id1 != tchecker::REFCLOCK_ID)
    os << index.value(id1);
  if (id2 != tchecker::REFCLOCK_ID) {
    if (id1 != tchecker::REFCLOCK_ID)
      os << "-";
    os << index.value(id2);
  }
  if (id1 != tchecker::REFCLOCK_ID)
    os << (c.comparator() == tchecker::LT ? "<" : "<=");
  else
    os << (c.comparator() == tchecker::LT ? ">" : ">=");
  os << (id1 != tchecker::REFCLOCK_ID ? c.value() : -c.value());
  return os;
}

std::string to_string(tchecker::clock_constraint_t const & c, tchecker::clock_index_t const & index)
{
  std::stringstream ss;
  tchecker::output(ss, c, index);
  return ss.str();
}

int lexical_cmp(tchecker::clock_constraint_t const & c1, tchecker::clock_constraint_t const & c2)
{
  if (c1.id1() != c2.id1())
    return (c1.id1() < c2.id2() ? -1 : 1);
  if (c1.id2() != c2.id2())
    return (c1.id2() < c2.id2() ? -1 : 1);
  if (c1.comparator() != c2.comparator())
    return (c1.comparator() == tchecker::LT ? -1 : 1);
  return (c1.value() == c2.value() ? 0 : (c1.value() < c2.value() ? -1 : 1));
}

int lexical_cmp(tchecker::clock_constraint_container_t const & c1, tchecker::clock_constraint_container_t const & c2)
{
  return tchecker::lexical_cmp<tchecker::clock_constraint_container_const_iterator_t,
                               tchecker::clock_constraint_container_const_iterator_t,
                               int (*)(tchecker::clock_constraint_t const &, tchecker::clock_constraint_t const &)>(
      c1.begin(), c1.end(), c2.begin(), c2.end(), tchecker::lexical_cmp);
}

std::string to_string(tchecker::clock_constraint_container_t const & c, tchecker::clock_index_t const & index)
{
  std::stringstream ss;
  tchecker::output_clock_constraints(ss, tchecker::make_range(c.begin(), c.end()), index);
  return ss.str();
}

/*!
 \class clock_constraints_visitor_t
 \brief Visitor of typed expression that put every clock constraint expression in a
 clock constraints container, and fails if the expression contains other constraints,
 or if clock identifiers or constants cannot be evaluated statically
 \throw std::runtime_error : if expressions other than clock constraints, conjunctions
 and parenthesis are found
 */
class clock_constraints_visitor_t : public tchecker::typed_expression_visitor_t {
public:
  /*!
   \brief Constructor
   \param c : clock constraint containter
   \post all clock constraints expressions are added to c
   */
  clock_constraints_visitor_t(tchecker::clock_constraint_container_t & c) : _c(c) {}

  /*!
   \brief Destructor
   */
  virtual ~clock_constraints_visitor_t() = default;

  /*!
   \brief Add clock constraints from a simple expression clock # bound
  */
  virtual void visit(tchecker::typed_simple_clkconstr_expression_t const & e)
  {
    try {
      tchecker::clock_id_t clock = clock_id(e.clock());
      tchecker::integer_t value = tchecker::const_evaluate(e.bound());
      add_constraints(clock, tchecker::REFCLOCK_ID, e.binary_operator(), value);
    }
    catch (...) {
      throw std::runtime_error("Syntax error in simple clock constraint: cannot compute clock IDs or constant");
    }
  }

  /*!
   \brief Add clock constraints from diagonal expression first - second # bound
  */
  virtual void visit(tchecker::typed_diagonal_clkconstr_expression_t const & e)
  {
    try {
      tchecker::clock_id_t first = clock_id(e.first_clock());
      tchecker::clock_id_t second = clock_id(e.second_clock());
      tchecker::integer_t value = tchecker::const_evaluate(e.bound());
      add_constraints(first, second, e.binary_operator(), value);
    }
    catch (...) {
      throw std::runtime_error("Syntax error in diagonal clock constraint: cannot compute clock IDs or constant");
    }
  }

  /*!
   \brief Visit expr1 and expr2 within expr1 && expr2, fail if e is not &&
  */
  virtual void visit(tchecker::typed_binary_expression_t const & e)
  {
    if (e.binary_operator() != tchecker::EXPR_OP_LAND)
      throw std::runtime_error("Unexpected binary operator, expecting &&");
    e.left_operand().visit(*this);
    e.right_operand().visit(*this);
  }

  /*!
   \brief Visit expr within (expr)
  */
  virtual void visit(tchecker::typed_par_expression_t const & e) { e.expr().visit(*this); }

  /*!
   \brief Other visitors throw
   */
  virtual void visit(tchecker::typed_int_expression_t const &) { throw std::runtime_error("Unexpected expression"); }
  virtual void visit(tchecker::typed_var_expression_t const &) { throw std::runtime_error("Unexpected expression"); }
  virtual void visit(tchecker::typed_bounded_var_expression_t const &) { throw std::runtime_error("Unexpected expression"); }
  virtual void visit(tchecker::typed_array_expression_t const &) { throw std::runtime_error("Unexpected expression"); }
  virtual void visit(tchecker::typed_unary_expression_t const &) { throw std::runtime_error("Unexpected expression"); }
  virtual void visit(tchecker::typed_ite_expression_t const &) { throw std::runtime_error("Unexpected expression"); }

private:
  /*!
   \brief Compute identifier of clock lvalue represented by an expression
   \param e : expression
   \return the identifier of lvalue represented by e according to _clocks
   \throw std::invalid_argument : if e is not a clock lvalue expression, or if the id
   cannot be computed
  */
  tchecker::clock_id_t clock_id(tchecker::typed_expression_t const & e)
  {
    tchecker::typed_lvalue_expression_t const & typed_e = dynamic_cast<tchecker::typed_lvalue_expression_t const &>(e);
    tchecker::range_t<tchecker::variable_id_t> ids_range = tchecker::extract_lvalue_variable_ids(typed_e);
    if (ids_range.empty())
      throw std::invalid_argument("Cannot determine clock ID");
    auto it = ids_range.begin();
    if (++it != ids_range.end())
      throw std::invalid_argument("Cannot determine clock ID");
    return ids_range.begin();
  }

  /*!
   \brief Add constraints to container from first - second # value
   \param first : first clock ID
   \param second : second clock ID
   \param op : comparator
   \param value : bound
   \post clock constraints corresponding to first - second # value have been added to _c
   \throw std::invalid_argument : if op is not one of EQ, GE, GT, LE and LT
  */
  void add_constraints(tchecker::clock_id_t first, tchecker::clock_id_t second, enum tchecker::binary_operator_t op,
                       tchecker::integer_t value)
  {
    switch (op) {
    case tchecker::EXPR_OP_EQ:
      add_constraints(first, second, tchecker::EXPR_OP_LE, value);
      add_constraints(first, second, tchecker::EXPR_OP_GE, value);
      break;
    case tchecker::EXPR_OP_GE: {
      tchecker::integer_t neg_value = static_cast<tchecker::integer_t>(-value);
      _c.emplace_back(second, first, tchecker::LE, neg_value);
      break;
    }
    case tchecker::EXPR_OP_GT: {
      tchecker::integer_t neg_value = static_cast<tchecker::integer_t>(-value);
      _c.emplace_back(second, first, tchecker::LT, neg_value);
      break;
    }
    case tchecker::EXPR_OP_LE:
      _c.emplace_back(first, second, tchecker::LE, value);
      break;
    case tchecker::EXPR_OP_LT:
      _c.emplace_back(first, second, tchecker::LT, value);
      break;
    default:
      throw std::invalid_argument("Unexpected expression operator in clock constraint");
    }
  }

  tchecker::clock_constraint_container_t & _c; /*!< Clock constraints container */
};

void from_string(tchecker::clock_constraint_container_t & c, tchecker::clock_variables_t const & clocks,
                 std::string const & str)
{
  if (str == "")
    return;

  // parse str as a typed expression
  std::shared_ptr<tchecker::expression_t> expr{tchecker::parsing::parse_expression("", str)};
  if (expr.get() == nullptr)
    throw std::invalid_argument("syntax error in " + str);

  tchecker::integer_variables_t no_integer_variables;
  std::shared_ptr<tchecker::typed_expression_t> typed_expr{
      tchecker::typecheck(*expr, no_integer_variables, no_integer_variables, clocks,
                          [](std::string const & err) { std::cerr << tchecker::log_error << err << std::endl; })};
  if (typed_expr.get() == nullptr)
    throw std::invalid_argument("type error in expression " + str);

  // instantiate a typed expression visitor, add each clock constraint from expression to c
  try {
    tchecker::clock_constraints_visitor_t v(c);
    typed_expr->visit(v);
  }
  catch (...) {
    throw std::invalid_argument("error in " + str);
  }
}

/* clock_reset_t */

clock_reset_t::clock_reset_t(tchecker::clock_id_t left_id, tchecker::clock_id_t right_id, tchecker::integer_t value)
    : _left_id(left_id), _right_id(right_id), _value(value)
{
  if (_left_id == tchecker::REFCLOCK_ID)
    throw std::invalid_argument("reference clock is not a left-value clock");

  if (_value < 0)
    throw std::invalid_argument("reset value should be >= 0");
}

bool operator==(tchecker::clock_reset_t const & r1, tchecker::clock_reset_t const & r2)
{
  return (r1.left_id() == r2.left_id() && r1.right_id() == r2.right_id() && r1.value() == r2.value());
}

bool operator!=(tchecker::clock_reset_t const & r1, tchecker::clock_reset_t const & r2) { return !(r1 == r2); }

std::size_t hash_value(tchecker::clock_reset_t const & r)
{
  std::size_t h = 0;
  boost::hash_combine(h, r.left_id());
  boost::hash_combine(h, r.right_id());
  boost::hash_combine(h, r.value());
  return h;
}

std::ostream & operator<<(std::ostream & os, tchecker::clock_reset_t const & r)
{
  os << r._left_id << "=" << r._right_id << "+" << r._value;
  return os;
}

std::ostream & output(std::ostream & os, tchecker::clock_reset_t const & r, tchecker::clock_index_t const & index)
{
  os << index.value(r.left_id());
  os << "=";
  tchecker::clock_id_t right_id = r.right_id();
  tchecker::integer_t value = r.value();
  if (right_id != tchecker::REFCLOCK_ID) {
    if (value != 0)
      os << value << "+";
    os << index.value(right_id);
  }
  else
    os << value;
  return os;
}

std::string to_string(tchecker::clock_reset_t const & r, tchecker::clock_index_t const & index)
{
  std::stringstream ss;
  tchecker::output(ss, r, index);
  return ss.str();
}

int lexical_cmp(tchecker::clock_reset_t const & r1, tchecker::clock_reset_t const & r2)
{
  if (r1.left_id() != r2.left_id())
    return (r1.left_id() < r2.left_id() ? -1 : 1);
  if (r1.right_id() != r2.right_id())
    return (r1.right_id() < r2.right_id() ? -1 : 1);
  return (r1.value() == r2.value() ? 0 : (r1.value() < r2.value() ? -1 : 1));
}

int lexical_cmp(tchecker::clock_reset_container_t const & c1, tchecker::clock_reset_container_t const & c2)
{
  return tchecker::lexical_cmp<tchecker::clock_reset_container_const_iterator_t,
                               tchecker::clock_reset_container_const_iterator_t,
                               int (*)(tchecker::clock_reset_t const &, tchecker::clock_reset_t const &)>(
      c1.begin(), c1.end(), c2.begin(), c2.end(), tchecker::lexical_cmp);
}

std::string to_string(tchecker::clock_reset_container_t const & c, tchecker::clock_index_t const & index)
{
  std::stringstream ss;
  tchecker::output_clock_resets(ss, tchecker::make_range(c.begin(), c.end()), index);
  return ss.str();
}

void clock_reset_to_constraints(tchecker::clock_reset_t const & r, tchecker::clock_constraint_container_t & cc)
{
  if (r.value() == std::numeric_limits<tchecker::integer_t>::min())
    throw std::overflow_error("tchecker::clock_reset_to_constraints: overflow");
  cc.push_back(tchecker::clock_constraint_t{r.left_id(), r.right_id(), tchecker::LE, r.value()});
  cc.push_back(
      tchecker::clock_constraint_t{r.right_id(), r.left_id(), tchecker::LE, static_cast<tchecker::integer_t>(-r.value())});
}

void clock_resets_to_constraints(tchecker::clock_reset_container_t const & rc, tchecker::clock_constraint_container_t & cc)
{
  for (tchecker::clock_reset_t const & r : rc)
    tchecker::clock_reset_to_constraints(r, cc);
}

/* reference_clock_variables_t */

reference_clock_variables_t::reference_clock_variables_t(std::vector<std::string> const & proc_refname_map)
    : _refcount(0), _procmap(proc_refname_map.size())
{
  if (proc_refname_map.empty())
    throw std::invalid_argument("Empty reference clocks not allowed");
  assert(proc_refname_map.size() < std::numeric_limits<tchecker::process_id_t>::max());
  for (tchecker::process_id_t pid = 0; pid < proc_refname_map.size(); ++pid) {
    tchecker::clock_id_t refid = 0;
    try {
      refid = tchecker::flat_clock_variables_t::id(proc_refname_map[pid]);
    }
    catch (...) {
      refid = declare_reference_clock(proc_refname_map[pid]);
    }
    _procmap[pid] = refid;
  }
}

tchecker::clock_id_t reference_clock_variables_t::declare(std::string const & name, std::string const & refclock)
{
  tchecker::clock_id_t refid = 0;
  try {
    refid = tchecker::flat_clock_variables_t::id(refclock);
  }
  catch (...) {
    throw std::invalid_argument("unknown reference clock " + refclock);
  }
  assert(refid < _refcount);
  tchecker::clock_id_t id = declare(name, refid);
  return id;
}

tchecker::clock_constraint_t reference_clock_variables_t::translate(tchecker::clock_constraint_t const & c) const
{
  assert(c.id1() != tchecker::REFCLOCK_ID || c.id2() != tchecker::REFCLOCK_ID);
  assert(c.id1() == tchecker::REFCLOCK_ID || c.id1() < size() - _refcount);
  assert(c.id2() == tchecker::REFCLOCK_ID || c.id2() < size() - _refcount);

  tchecker::clock_id_t id1 = c.id1();
  id1 = (id1 == tchecker::REFCLOCK_ID ? refclock_of_system_clock(c.id2()) : translate_system_clock(id1));
  tchecker::clock_id_t id2 = c.id2();
  id2 = (id2 == tchecker::REFCLOCK_ID ? refclock_of_system_clock(c.id1()) : translate_system_clock(id2));

  return tchecker::clock_constraint_t{id1, id2, c.comparator(), c.value()};
}

tchecker::clock_reset_t reference_clock_variables_t::translate(tchecker::clock_reset_t const & r) const
{
  assert(r.left_id() < size() - _refcount);
  assert(r.right_id() == tchecker::REFCLOCK_ID || r.right_id() < size() - _refcount);

  tchecker::clock_id_t left_id = translate_system_clock(r.left_id());
  tchecker::clock_id_t right_id = r.right_id();
  right_id = (right_id == tchecker::REFCLOCK_ID ? refclock_of_system_clock(r.left_id()) : translate_system_clock(right_id));

  return tchecker::clock_reset_t{left_id, right_id, r.value()};
}

tchecker::clock_id_t reference_clock_variables_t::declare(std::string const & name, tchecker::clock_id_t refid)
{
  tchecker::clock_id_t size = this->size();
  assert(refid < _refcount || refid == size); // declared reference clock or declaring a reference clock

  tchecker::clock_id_t id = size;
  tchecker::clock_info_t info{1}; // size 1
  tchecker::flat_clock_variables_t::declare(id, name, info);

  _refmap.resize(id + 1);
  _refmap[id] = refid;

  return id;
}

tchecker::clock_id_t reference_clock_variables_t::declare_reference_clock(std::string const & name)
{
  if (_refcount != size())
    throw std::runtime_error("Reference clocks must be declared before clock variables");

  tchecker::clock_id_t id = declare(name, _refcount);
  assert(id == _refcount);
  ++_refcount;

  return id;
}

/* Reference clock builders */

tchecker::reference_clock_variables_t single_reference_clocks(tchecker::flat_clock_variables_t const & flat_clocks,
                                                              tchecker::process_id_t proc_count)
{
  std::string const zero_clock_name = "$0";

  std::vector<std::string> proc_refname_map(proc_count, zero_clock_name);

  tchecker::reference_clock_variables_t reference_clocks(proc_refname_map);

  tchecker::clock_id_t clocks_count = flat_clocks.size();
  for (tchecker::clock_id_t clock_id = 0; clock_id < clocks_count; ++clock_id) {
    std::string const & clock_name = flat_clocks.index().value(clock_id);
    reference_clocks.declare(clock_name, zero_clock_name);
  }

  return reference_clocks;
}

tchecker::reference_clock_variables_t process_reference_clocks(tchecker::variable_access_map_t const & vaccess_map,
                                                               tchecker::flat_clock_variables_t const & flat_clocks,
                                                               tchecker::process_id_t proc_count)
{
  if (proc_count == 0)
    throw std::invalid_argument("number of processes should be > 0");

  tchecker::clock_id_t const flat_clocks_count = flat_clocks.size();

  if (proc_count > std::numeric_limits<tchecker::clock_id_t>::max())
    throw std::invalid_argument("too many processes");

  unsigned long long all_clocks_count = proc_count + flat_clocks_count;
  if (all_clocks_count > std::numeric_limits<tchecker::clock_id_t>::max())
    throw std::invalid_argument("too many offset variables");

  // compute reference clock names
  std::vector<std::string> proc_refname_map;
  for (tchecker::process_id_t pid = 0; pid < proc_count; ++pid)
    proc_refname_map.push_back("$" + std::to_string(pid));

  tchecker::reference_clock_variables_t reference_clocks(proc_refname_map);

  // declare clock variables
  for (tchecker::clock_id_t clock_id = 0; clock_id < flat_clocks_count; ++clock_id) {
    auto accessing_processes = vaccess_map.accessing_processes(clock_id, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);

    if (std::distance(accessing_processes.begin(), accessing_processes.end()) != 1)
      throw std::invalid_argument("bad clock access, cannot build reference clocks");

    tchecker::process_id_t pid = *accessing_processes.begin();
    std::string const & clock_name = flat_clocks.index().value(clock_id);
    reference_clocks.declare("$" + clock_name, proc_refname_map[pid]);
  }

  return reference_clocks;
}

/* clock_variables */

tchecker::clock_variables_t clock_variables(tchecker::reference_clock_variables_t const & refclocks,
                                            tchecker::clock_variables_t const & clocks)
{
  tchecker::clock_variables_t clockvars;

  // declare reference clocks
  for (tchecker::clock_id_t id = 0; id < refclocks.refcount(); ++id)
    clockvars.declare(refclocks.name(id), 1);

  // declare clocks
  for (auto && [id, name] : clocks.index())
    clockvars.declare(name, clocks.info(id).size());

  return clockvars;
}

/* clockval_t */

#define CV_ID(id) (id == tchecker::REFCLOCK_ID ? 0 : id + 1) // converts from system IDs to clockval IDs

tchecker::clockval_t * clockval_allocate_and_construct(unsigned short size, tchecker::clock_rational_value_t value)
{
  char * ptr = new char[tchecker::allocation_size_t<tchecker::clockval_t>::alloc_size(size)];
  tchecker::clockval_t::construct(ptr, size, value);
  return reinterpret_cast<tchecker::clockval_t *>(ptr);
}

tchecker::clockval_t * clockval_clone(tchecker::clockval_t const & clockval)
{
  char * ptr = new char[tchecker::allocation_size_t<tchecker::clockval_t>::alloc_size(clockval.size())];
  tchecker::clockval_t::construct(ptr, clockval);
  return reinterpret_cast<tchecker::clockval_t *>(ptr);
}

void clockval_destruct_and_deallocate(tchecker::clockval_t * v)
{
  tchecker::clockval_t::destruct(v);
  delete[] reinterpret_cast<char *>(v);
}

std::ostream & output(std::ostream & os, tchecker::clockval_t const & clockval,
                      std::function<std::string(tchecker::clock_id_t)> clock_name)
{
  auto const size = clockval.size();
  for (tchecker::clock_id_t x = 0; x < size; ++x) {
    if (x > 0)
      os << ",";
    os << clock_name(x) << "=" << tchecker::to_string(clockval[x]);
  }
  return os;
}

std::string to_string(tchecker::clockval_t const & clockval, std::function<std::string(tchecker::clock_id_t)> clock_name)
{
  std::stringstream sstream;
  output(sstream, clockval, clock_name);
  return sstream.str();
}

int lexical_cmp(tchecker::clockval_t const & clockval1, tchecker::clockval_t const & clockval2)
{
  return tchecker::lexical_cmp(clockval1.begin(), clockval1.end(), clockval2.begin(), clockval2.end(),
                               [](tchecker::clock_rational_value_t v1, tchecker::clock_rational_value_t v2) -> int {
                                 return (v1 < v2 ? -1 : (v1 == v2 ? 0 : 1));
                               });
}

void initial(tchecker::clockval_t & clockval)
{
  for (tchecker::clock_id_t id = 0; id < clockval.size(); ++id)
    clockval[id] = 0;
}

bool is_initial(tchecker::clockval_t const & clockval)
{
  for (tchecker::clock_id_t id = 0; id < clockval.size(); ++id)
    if (clockval[id] != 0)
      return false;
  return true;
}

bool satisfies(tchecker::clockval_t const & clockval, tchecker::clock_id_t id1, tchecker::clock_id_t id2,
               enum tchecker::ineq_cmp_t cmp, tchecker::integer_t value)
{
  if (cmp == tchecker::LT)
    return clockval[id1] - clockval[id2] < value;
  return clockval[id1] - clockval[id2] <= value;
}

bool satisfies(tchecker::clockval_t const & clockval, tchecker::clock_constraint_t const & c)
{
  return tchecker::satisfies(clockval, CV_ID(c.id1()), CV_ID(c.id2()), c.comparator(), c.value());
}

bool satisfies(tchecker::clockval_t const & clockval, tchecker::clock_constraint_container_t const & cc)
{
  for (tchecker::clock_constraint_t const & c : cc)
    if (!tchecker::satisfies(clockval, c))
      return false;
  return true;
}

/* delay */

/*!
 \struct delay_constraint_t
 \brief constraint on delay
*/
struct delay_constraint_t {
  tchecker::clock_rational_value_t bound; /*!< Bound on delay */
  bool strict;                            /*!< Whether the bound is strict (< or >) or not (<= or >=) */
};

/*!
 \brief Build a delay constraint
*/
static tchecker::delay_constraint_t dc(tchecker::clock_rational_value_t b, bool s)
{
  return tchecker::delay_constraint_t{.bound = b, .strict = s};
}

/*!
 \brief Comparison of delay constraints
*/
static bool operator<(tchecker::delay_constraint_t const & dc1, tchecker::delay_constraint_t const & dc2)
{
  return (dc1.bound < dc2.bound) || ((dc1.bound == dc2.bound) && dc1.strict && !dc2.strict);
}

/*!
 \brief Comparison of delay constraints
*/
static bool operator<=(tchecker::delay_constraint_t const & dc1, tchecker::delay_constraint_t const & dc2)
{
  return (dc1.bound < dc2.bound) || ((dc1.bound == dc2.bound) && !dc2.strict);
}

/*!
 \brief Comparison of delay constraints
*/
static bool operator>=(tchecker::delay_constraint_t const & dc1, tchecker::delay_constraint_t const & dc2)
{
  return !(dc1 < dc2);
}

/*!
 \brief Comparison of delay constraints
*/
static bool operator>(tchecker::delay_constraint_t const & dc1, tchecker::delay_constraint_t const & dc2)
{
  return !(dc1 <= dc2);
}

/*!
 \brief Compute the minimum of two delay constraints
*/
static tchecker::delay_constraint_t min(tchecker::delay_constraint_t const & dc1, tchecker::delay_constraint_t const & dc2)
{
  return (dc1 <= dc2 ? dc1 : dc2);
}

/*!
 \brief Compute the maximum of two constraints
*/
static tchecker::delay_constraint_t max(tchecker::delay_constraint_t const & dc1, tchecker::delay_constraint_t const & dc2)
{
  return (dc1 >= dc2 ? dc1 : dc2);
}

/*!
 \brief Output operator on delay constraints
*/
static std::ostream & operator<<(std::ostream & os, tchecker::delay_constraint_t const & dc)
{
  return os << "(" << dc.bound << "," << (dc.strict ? "strict" : "non-strict") << ")";
}

/*!
 \brief Compute lower and upper bound constraints on delay from a clock valuation and clock constraints
*/
static std::tuple<tchecker::delay_constraint_t, tchecker::delay_constraint_t>
delay_constraints(tchecker::clockval_t const & clockval, tchecker::clock_constraint_container_t const & cc)
{
  tchecker::delay_constraint_t lower{0, false}, upper{tchecker::dbm::INF_VALUE, false};

  for (tchecker::clock_constraint_t const & c : cc) {
    if (c.id2() == tchecker::REFCLOCK_ID) // upper delay constraint
      upper = tchecker::min(upper, tchecker::dc(c.value() - clockval[CV_ID(c.id1())], (c.comparator() == tchecker::LT)));
    else if (c.id1() == tchecker::REFCLOCK_ID) // lower delay constraint
      lower = tchecker::max(lower, tchecker::dc((-c.value()) - clockval[CV_ID(c.id2())], (c.comparator() == tchecker::LT)));
  }

  return std::make_tuple(lower, upper);
}

/*!
 \brief Compute lower and upper bound constraints on delay from clock valuations and clock reset
 \pre each clock is reset at most once in reset
 \return lower and upper bound constraints on delay such that (src+delay)[reset]==tgt if any
 \note resets of the form x=y+c induce constraints like delay=tgt[x]-src[y]-c
 \note resets to constants x=c induce no constraint on delay, except that tgt[x] must be equal to c.
 If not, this function returns insatisfiable constraints
 \note clocks which are not reset induce a constraint delay=tgt[x]-src[x]
*/
static std::tuple<tchecker::delay_constraint_t, tchecker::delay_constraint_t>
delay_constraints(tchecker::clockval_t const & src, tchecker::clock_reset_container_t const & reset,
                  tchecker::clockval_t const & tgt)
{
  tchecker::clock_id_t const dim = src.size();

  tchecker::delay_constraint_t lower{0, false}, upper{tchecker::dbm::INF_VALUE, false};

  boost::dynamic_bitset<> non_reset_clocks(dim);
  non_reset_clocks.set();

  for (tchecker::clock_reset_t const & r : reset) {
    assert((r.left_id() < dim) || (r.left_id() == tchecker::REFCLOCK_ID));
    assert((r.right_id() < dim) || (r.right_id() == tchecker::REFCLOCK_ID));
    assert((r.left_id() != tchecker::REFCLOCK_ID) || (r.right_id() != tchecker::REFCLOCK_ID));

    if (r.right_id() == tchecker::REFCLOCK_ID) {
      // reset to constant
      if (tgt[CV_ID(r.left_id())] != r.value())
        upper.bound = lower.bound - 1; // make constraints inconsistent
    }
    else {
      // reset to clock value (+ constant)
      if (non_reset_clocks[CV_ID(r.right_id())]) {
        // induce constraints on delay if right clock has not been reset yet
        tchecker::clock_rational_value_t delay = tgt[CV_ID(r.left_id())] - src[CV_ID(r.right_id())] - r.value();
        lower = tchecker::max(lower, tchecker::dc(delay, false));
        upper = tchecker::min(upper, tchecker::dc(delay, false));
      }
      else if (tgt[CV_ID(r.left_id())] != tgt[CV_ID(r.right_id())] + r.value())
        upper.bound = lower.bound - 1; // make constraints inconsistent if reset does not yield expected value
    }

    assert(non_reset_clocks[CV_ID(r.left_id())]);
    non_reset_clocks[CV_ID(r.left_id())] = 0;
  }

  non_reset_clocks[0] = 0; // clock 0 is "reset" as it always have value 0

  // constraints from clocks which are not reset
  for (tchecker::clock_id_t x = 0; x < dim; ++x)
    if (non_reset_clocks[x]) {
      tchecker::clock_rational_value_t const delay = tgt[x] - src[x];
      lower = tchecker::max(lower, tchecker::dc(delay, false));
      upper = tchecker::min(upper, tchecker::dc(delay, false));
    }

  return std::make_tuple(lower, upper);
}

tchecker::clock_rational_value_t delay(tchecker::clockval_t const & src,
                                       tchecker::clock_constraint_container_t const & invariant,
                                       tchecker::clock_constraint_container_t const & guard,
                                       tchecker::clock_reset_container_t const & reset, tchecker::clockval_t const & tgt)
{
  assert(src.size() == tgt.size());
  assert(src[0] == 0);
  assert(tgt[0] == 0);
  assert(tchecker::satisfies(src, invariant));

  // get lower and upeer constraints on delays from guard, invariant and reset, then choose some delay in-between
  auto && [lower_guard, upper_guard] = tchecker::delay_constraints(src, guard);
  auto && [lower_invariant, upper_invariant] = tchecker::delay_constraints(src, invariant);
  auto && [lower_reset, upper_reset] = tchecker::delay_constraints(src, reset, tgt);

  tchecker::delay_constraint_t const lower = tchecker::max(tchecker::max(lower_guard, lower_invariant), lower_reset);
  tchecker::delay_constraint_t const upper = tchecker::min(tchecker::min(upper_guard, upper_invariant), upper_reset);

  if (upper.bound < lower.bound)
    return -1;
  if ((upper.bound == lower.bound) && (lower.strict || upper.strict))
    return -1;

  tchecker::clock_rational_value_t delay = 0;
  if (!lower.strict)
    delay = lower.bound;
  else if (upper.bound == tchecker::dbm::INF_VALUE)
    delay = lower.bound + tchecker::clock_rational_value_t{5, 10};
  else
    delay = (upper.bound + lower.bound) / 2;

  assert(delay >= 0);

  return delay;
}

} // end of namespace tchecker
