/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/expression/static_analysis.hh"

namespace tchecker {

// Evaluation of constant expressions

namespace details {

/*!
 \class const_expression_evaluator_t
 \brief Expression visitor for evaluation of constant expressions
 */
class const_expression_evaluator_t : public tchecker::expression_visitor_t {
public:
  /*!
   \brief Constructor
   */
  const_expression_evaluator_t() : _value(0) {}

  /*!
   \brief Copy constructor
   \param evaluator : an evaluator
   \post this is a copy of evaluator
   */
  const_expression_evaluator_t(tchecker::details::const_expression_evaluator_t const & evaluator) : _value(evaluator._value) {}

  /*!
   \brief Move constructor
   \param evaluator : an evaluator
   \post evaluator has been moved to this
   */
  const_expression_evaluator_t(tchecker::details::const_expression_evaluator_t && evaluator)
      : _value(std::move(evaluator._value))
  {
  }

  /*!
   \brief Destructor
   */
  virtual ~const_expression_evaluator_t() = default;

  /*!
   \brief Assignment operator
   \param evaluator : an evaluator
   \post this is a copy of evaluator
   \return this
   */
  tchecker::details::const_expression_evaluator_t & operator=(tchecker::details::const_expression_evaluator_t const & evaluator)
  {
    if (this != &evaluator)
      _value = evaluator._value;
    return *this;
  }

  /*!
   \brief Move-assignment operator
   \param evaluator : an evaluator
   \post evaluator has been moved to this
   \return this
   */
  tchecker::details::const_expression_evaluator_t & operator=(tchecker::details::const_expression_evaluator_t && evaluator)
  {
    if (this != &evaluator)
      _value = std::move(evaluator._value);
    return *this;
  }

  /*!
   \brief Accessor
   \return value of last visited expression
   */
  inline tchecker::integer_t value() const { return _value; }

  /*!
   \brief Visitor
   \param expr : expression
   \post set _value to expr's value
   */
  virtual void visit(tchecker::int_expression_t const & expr) { _value = expr.value(); }

  /*!
   \brief Visitor
   \param expr : expression
   \throw std::invalid_argument
   */
  virtual void visit(tchecker::var_expression_t const & expr) { throw std::invalid_argument("not a constant expression"); }

  /*!
   \brief Visitor
   \param expr : expression
   \throw std::invalid_argument
   */
  virtual void visit(tchecker::array_expression_t const & expr) { throw std::invalid_argument("not a constant expression"); }

  /*!
   \brief Visitor
   \param expr : expression
   \post visits expr's sub expression
   */
  virtual void visit(tchecker::par_expression_t const & expr) { expr.expr().visit(*this); }

  /*!
   \brief Visitor
   \param expr : expression
   \throw set _value to result of applying unary operator to recursive evaluation of operand expression
   */
  virtual void visit(tchecker::unary_expression_t const & expr)
  {
    expr.operand().visit(*this);

    switch (expr.unary_operator()) {
    case tchecker::EXPR_OP_NEG:
      _value = -_value;
      break;
    case tchecker::EXPR_OP_LNOT:
      _value = !_value;
      break;
    default:
      throw std::runtime_error("incomplete switch statement");
    }
  }

  /*!
   \brief Visitor
   \param expr : expression
   \throw set _value to result of applying binary operator to recursive evaluation of operand expressions
   */
  virtual void visit(tchecker::binary_expression_t const & expr)
  {
    expr.left_operand().visit(*this);
    tchecker::integer_t left = _value;

    expr.right_operand().visit(*this);
    tchecker::integer_t right = _value;

    switch (expr.binary_operator()) {
    case tchecker::EXPR_OP_LAND:
      _value = left && right;
      break;
    case tchecker::EXPR_OP_LT:
      _value = left < right;
      break;
    case tchecker::EXPR_OP_LE:
      _value = left <= right;
      break;
    case tchecker::EXPR_OP_EQ:
      _value = left == right;
      break;
    case tchecker::EXPR_OP_NEQ:
      _value = left != right;
      break;
    case tchecker::EXPR_OP_GE:
      _value = left >= right;
      break;
    case tchecker::EXPR_OP_GT:
      _value = left > right;
      break;
    case tchecker::EXPR_OP_MINUS:
      _value = left - right;
      break;
    case tchecker::EXPR_OP_PLUS:
      _value = left + right;
      break;
    case tchecker::EXPR_OP_TIMES:
      _value = left * right;
      break;
    case tchecker::EXPR_OP_DIV:
      _value = left / right;
      break;
    case tchecker::EXPR_OP_MOD:
      _value = left % right;
      break;
    default:
      throw std::runtime_error("incomplete switch statement");
    }
  }

  /*!
   \brief Visitor
   \param expr : expression
   \throw set _value to result of applying if-then-else operator to
          recursive evaluation of operand expressions
   */
  virtual void visit(tchecker::ite_expression_t const & expr)
  {
    expr.condition().visit(*this);
    if (_value)
      expr.then_value().visit(*this);
    else
      expr.else_value().visit(*this);
  }

protected:
  tchecker::integer_t _value; /*!< Expression value */
};

} // end of namespace details

/* has_const_value */

bool has_const_value(tchecker::expression_t const & expr)
{
  try {
    const_evaluate(expr);
  }
  catch (...) {
    return false;
  }
  return true;
}

/* const_evaluate */

tchecker::integer_t const_evaluate(tchecker::expression_t const & expr)
{
  tchecker::details::const_expression_evaluator_t evaluator;

  try {
    expr.visit(evaluator);
    return evaluator.value();
  }
  catch (...) {
    throw;
  }
}

tchecker::integer_t const_evaluate(tchecker::expression_t const & expr, tchecker::integer_t value)
{
  try {
    return const_evaluate(expr);
  }
  catch (std::invalid_argument const & e) {
    return value;
  }
  catch (...) {
    throw;
  }
}

// Base variable extraction

namespace details {

/*!
 \class base_variable_ids_extractor_t
 \brief Extract base variable IDs in a typed lvalue expression (tchecker::typed_var_expression_t,
 tchecker::typed_bounded_var_expression_t or tchecker::typed_array_expression_t)
 \note base variables are simple variables and base names of array expression (i.e. x in x[e])
 \throw std::invalid_argument : if the expression is not an lvalue expression
 */
class base_variable_ids_extractor_t : public tchecker::typed_expression_visitor_t {
public:
  /*!
   \brief Constructor
   */
  base_variable_ids_extractor_t() : _first(0), _size(0), _variable_type(tchecker::EXPR_TYPE_BAD) {}

  /*!
   \brief Copy constructor
   */
  base_variable_ids_extractor_t(tchecker::details::base_variable_ids_extractor_t const & extractor)
      : _first(extractor._first), _size(extractor._size), _variable_type(extractor._variable_type)
  {
  }

  /*!
   \brief Move constructor
   */
  base_variable_ids_extractor_t(tchecker::details::base_variable_ids_extractor_t && extractor)
      : _first(std::move(extractor._first)), _size(std::move(extractor._size)),
        _variable_type(std::move(extractor._variable_type))
  {
  }

  /*!
   \brief Destructor
   */
  ~base_variable_ids_extractor_t() = default;

  /*!
   \brief Assignment operator
   \param extractor : an extractor
   \post this is a copy of extractor
   */
  tchecker::details::base_variable_ids_extractor_t &
  operator=(tchecker::details::base_variable_ids_extractor_t const & extractor)
  {
    if (this != &extractor) {
      _first = extractor._first;
      _size = extractor._size;
      _variable_type = extractor._variable_type;
    }
    return *this;
  }

  /*!
   \brief Move-assignment operator
   \param extractor : an extractor
   \post extractor has been moved to this
   */
  tchecker::details::base_variable_ids_extractor_t & operator=(tchecker::details::base_variable_ids_extractor_t && extractor)
  {
    if (this != &extractor) {
      _first = std::move(extractor._first);
      _size = std::move(extractor._size);
      _variable_type = std::move(extractor._variable_type);
    }
    return *this;
  }

  /*!
   \brief Accessor
   \return the range of variable IDs for the last expression visited
   */
  tchecker::range_t<tchecker::variable_id_t> range() const { return tchecker::make_range(_first, _first + _size); }

  /*!
   \brief Accessor
   \return the type of the base variable
   */
  enum tchecker::expression_type_t variable_type() const { return _variable_type; }

  /*!
   \brief Visitor
   \post first and size have been set to the range of IDs in expr
   */
  virtual void visit(tchecker::typed_var_expression_t const & expr)
  {
    _first = expr.id();
    _size = expr.size();
    _variable_type = expr.type();
  }

  /*!
   \brief Visitor
   \post first and size have been set to the range of IDs in expr
   */
  virtual void visit(tchecker::typed_bounded_var_expression_t const & expr)
  {
    _first = expr.id();
    _size = expr.size();
    _variable_type = expr.type();
  }

  /*!
   \brief Visitor
   \post expr's variable expression has been visited
   */
  virtual void visit(tchecker::typed_array_expression_t const & expr)
  {
    expr.variable().visit(*this);
    // adjust the range of IDs if the offset of expr can be statically computed
    try {
      tchecker::integer_t offset = tchecker::const_evaluate(expr.offset());
      if ((0 <= offset) && (offset < (tchecker::integer_t)_size)) {
        _first += offset;
        _size = 1;
      }
    }
    catch (...) {
    }
    _variable_type = expr.variable().type();
  }

  // Other visitors
  virtual void visit(tchecker::typed_int_expression_t const &) { throw std::invalid_argument("not an lvalue expression"); }

  virtual void visit(tchecker::typed_par_expression_t const &) { throw std::invalid_argument("not an lvalue expression"); }

  virtual void visit(tchecker::typed_binary_expression_t const &) { throw std::invalid_argument("not an lvalue expression"); }

  virtual void visit(tchecker::typed_unary_expression_t const &) { throw std::invalid_argument("not an lvalue expression"); }

  virtual void visit(tchecker::typed_simple_clkconstr_expression_t const &)
  {
    throw std::invalid_argument("not an lvalue expression");
  }

  virtual void visit(tchecker::typed_diagonal_clkconstr_expression_t const &)
  {
    throw std::invalid_argument("not an lvalue expression");
  }

  virtual void visit(tchecker::typed_ite_expression_t const &) { throw std::invalid_argument("not an lvalue expression"); }

protected:
  tchecker::variable_id_t _first;                  /*!< ID of first variable */
  tchecker::variable_id_t _size;                   /*!< Number of variables */
  enum tchecker::expression_type_t _variable_type; /*!< Type of base variable */
};

} // end of namespace details

/* extract_lvalue_variable_ids */

tchecker::range_t<tchecker::variable_id_t> extract_lvalue_variable_ids(tchecker::typed_lvalue_expression_t const & expr)
{
  tchecker::details::base_variable_ids_extractor_t extractor;
  try {
    expr.visit(extractor);
    return extractor.range();
  }
  catch (...) {
    throw;
  }
}

/* extract_lvalue_base_variable_ids */

void extract_lvalue_base_variable_ids(tchecker::typed_lvalue_expression_t const & expr,
                                      std::unordered_set<tchecker::clock_id_t> & clocks,
                                      std::unordered_set<tchecker::intvar_id_t> & intvars)
{
  tchecker::details::base_variable_ids_extractor_t extractor;
  try {
    expr.visit(extractor);
    tchecker::range_t<tchecker::variable_id_t> ids = extractor.range();
    enum tchecker::expression_type_t type = extractor.variable_type();

    if ((type == tchecker::EXPR_TYPE_CLKVAR) || (type == tchecker::EXPR_TYPE_CLKARRAY))
      for (tchecker::clock_id_t id = ids.begin(); id != ids.end(); ++id)
        clocks.insert(id);
    else if ((type == tchecker::EXPR_TYPE_INTVAR) || (type == tchecker::EXPR_TYPE_INTARRAY))
      for (tchecker::intvar_id_t id = ids.begin(); id != ids.end(); ++id)
        intvars.insert(id);
  }
  catch (...) {
    throw;
  }
}

/* extract_lvalue_offset_variable_ids */

namespace details {

/*!
 \class extract_offset_variables_visitor_t
 \brief Visitor of typed expressions for extraction of variable IDs from the offset of array expressions
 \note does nothing if the visited expression is not of type tchecker::typed_array_expression_t
 */
class extract_offset_variables_visitor_t : public tchecker::typed_expression_visitor_t {
public:
  /*!
   \brief Constructor
   \param clocks : set of clock IDs
   \param intvars : set of integer variable IDs
   */
  extract_offset_variables_visitor_t(std::unordered_set<tchecker::clock_id_t> & clocks,
                                     std::unordered_set<tchecker::intvar_id_t> & intvars)
      : _clocks(clocks), _intvars(intvars)
  {
  }

  /*!
   \brief Copy constructor
   */
  extract_offset_variables_visitor_t(tchecker::details::extract_offset_variables_visitor_t const &) = default;

  /*!
   \brief Destructor
   */
  virtual ~extract_offset_variables_visitor_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::details::extract_offset_variables_visitor_t &
  operator=(tchecker::details::extract_offset_variables_visitor_t const &) = delete;

  /*!
   \brief Move assignment operator (deleted)
   */
  tchecker::details::extract_offset_variables_visitor_t &
  operator=(tchecker::details::extract_offset_variables_visitor_t &&) = delete;

  /*!
   \brief Nothing to do (not an array)
   */
  virtual void visit(tchecker::typed_var_expression_t const & expr) {}

  /*!
   \brief Nothing to do (not an array)
   */
  virtual void visit(tchecker::typed_bounded_var_expression_t const & expr) {}

  /*!
   \brief extract variable IDs from array offset
   */
  virtual void visit(tchecker::typed_array_expression_t const & expr)
  {
    tchecker::extract_variables(expr.offset(), _clocks, _intvars);
  }

  /* Other visitors (throw: not an lvalue expression) */

  virtual void visit(tchecker::typed_int_expression_t const &) { throw std::invalid_argument("not an lvalue expression"); }

  virtual void visit(tchecker::typed_par_expression_t const & expr) { throw std::invalid_argument("not an lvalue expression"); }

  virtual void visit(tchecker::typed_binary_expression_t const & expr)
  {
    throw std::invalid_argument("not an lvalue expression");
  }

  virtual void visit(tchecker::typed_unary_expression_t const & expr)
  {
    throw std::invalid_argument("not an lvalue expression");
  }

  virtual void visit(tchecker::typed_simple_clkconstr_expression_t const & expr)
  {
    throw std::invalid_argument("not an lvalue expression");
  }

  virtual void visit(tchecker::typed_diagonal_clkconstr_expression_t const & expr)
  {
    throw std::invalid_argument("not an lvalue expression");
  }

  virtual void visit(tchecker::typed_ite_expression_t const & expr) { throw std::invalid_argument("not an lvalue expression"); }

private:
  std::unordered_set<tchecker::clock_id_t> & _clocks;   /*!< Set of clock IDs */
  std::unordered_set<tchecker::intvar_id_t> & _intvars; /*!< Set of integer variable IDs */
};

} // end of namespace details

void extract_lvalue_offset_variable_ids(tchecker::typed_lvalue_expression_t const & expr,
                                        std::unordered_set<tchecker::clock_id_t> & clocks,
                                        std::unordered_set<tchecker::intvar_id_t> & intvars)
{
  tchecker::details::extract_offset_variables_visitor_t v(clocks, intvars);
  expr.visit(v);
}

/* extract_variables */

namespace details {

/*!
 \class extract_variables_visitor_t
 \brief Visitor of typed expressions for extraction of variabels IDs
 */
class extract_variables_visitor_t : public tchecker::typed_expression_visitor_t {
public:
  /*!
   \brief Constructor
   \param clocks : set of clock IDs
   \param intvars : set of integer variable IDs
   */
  extract_variables_visitor_t(std::unordered_set<tchecker::clock_id_t> & clocks,
                              std::unordered_set<tchecker::intvar_id_t> & intvars)
      : _clocks(clocks), _intvars(intvars)
  {
  }

  /*!
   \brief Copy constructor
   */
  extract_variables_visitor_t(tchecker::details::extract_variables_visitor_t const &) = default;

  /*!
   \brief Destructor
   */
  virtual ~extract_variables_visitor_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::details::extract_variables_visitor_t & operator=(tchecker::details::extract_variables_visitor_t const &) = delete;

  /*!
   \brief Move assignment operator (deleted)
   */
  tchecker::details::extract_variables_visitor_t & operator=(tchecker::details::extract_variables_visitor_t &&) = delete;

  /*!
   \brief Add (expr.id() |-> _pid) to the map corresponding to expr.type()
   */
  virtual void visit(tchecker::typed_var_expression_t const & expr) { extract_variable_with_type(expr.id(), expr.type()); }

  /*!
   \brief Add (expr.id() |-> _pid) to the map corresponding to expr.type()
   */
  virtual void visit(tchecker::typed_bounded_var_expression_t const & expr)
  {
    extract_variable_with_type(expr.id(), expr.type());
  }

  /*!
   \brief if expr.offset() can be statically evaluated, then add (expr.id()+expr.offset() |-> _pid) to the map
   corresponding to expr.type(), otherwise, add add (expr.id()+k |-> _pid) to the right map for all k in the
   domain of expr.id()
   */
  virtual void visit(tchecker::typed_array_expression_t const & expr)
  {
    // add variables from offset
    expr.offset().visit(*this);

    // add base variables depending on const evaluation of offset
    tchecker::typed_var_expression_t const & expr_variable = expr.variable();
    tchecker::integer_t offset;
    try {
      offset = tchecker::const_evaluate(expr.offset());
    }
    catch (...) {
      tchecker::integer_t size = static_cast<tchecker::integer_t>(expr_variable.size());
      for (offset = 0; offset < size; ++offset)
        extract_variable_with_type(expr_variable.id() + offset, expr_variable.type());
      return;
    }
    extract_variable_with_type(expr_variable.id() + offset, expr_variable.type());
  }

  /* Other visitors (recursion) */

  virtual void visit(tchecker::typed_int_expression_t const &) {}

  virtual void visit(tchecker::typed_par_expression_t const & expr) { expr.expr().visit(*this); }

  virtual void visit(tchecker::typed_binary_expression_t const & expr)
  {
    expr.left_operand().visit(*this);
    expr.right_operand().visit(*this);
  }

  virtual void visit(tchecker::typed_unary_expression_t const & expr) { expr.operand().visit(*this); }

  virtual void visit(tchecker::typed_simple_clkconstr_expression_t const & expr)
  {
    expr.left_operand().visit(*this);
    expr.right_operand().visit(*this);
  }

  virtual void visit(tchecker::typed_diagonal_clkconstr_expression_t const & expr)
  {
    expr.left_operand().visit(*this);
    expr.right_operand().visit(*this);
  }

  virtual void visit(tchecker::typed_ite_expression_t const & expr)
  {
    expr.condition().visit(*this);
    expr.then_value().visit(*this);
    expr.else_value().visit(*this);
  }

private:
  /*!
   \brief Add id |-> _pid to the right map according to type
   */
  void extract_variable_with_type(tchecker::variable_id_t id, enum tchecker::expression_type_t type)
  {
    if ((type == tchecker::EXPR_TYPE_INTVAR) || (type == tchecker::EXPR_TYPE_INTARRAY))
      _intvars.insert(id);
    else if ((type == tchecker::EXPR_TYPE_CLKVAR) || (type == tchecker::EXPR_TYPE_CLKARRAY))
      _clocks.insert(id);
    else
      throw std::invalid_argument("typed expression is not well-typed");
  }

  std::unordered_set<tchecker::clock_id_t> & _clocks;   /*!< Set of clock IDs */
  std::unordered_set<tchecker::intvar_id_t> & _intvars; /*!< Set of integer variable IDs */
};

} // end of namespace details

void extract_variables(tchecker::typed_expression_t const & expr, std::unordered_set<tchecker::clock_id_t> & clocks,
                       std::unordered_set<tchecker::intvar_id_t> & intvars)
{
  tchecker::details::extract_variables_visitor_t v(clocks, intvars);
  expr.visit(v);
}

/* has_clock_constraints */

namespace details {

/*!
 \class clock_constraint_visitor_t
 \brief Visitor of typed expressions for computing types of contained clock constraints
 */
class clock_constraint_visitor_t : public tchecker::typed_expression_visitor_t {
public:
  /*!
   \brief Constructor
   */
  clock_constraint_visitor_t() : _has_clock_constraints{.simple = false, .diagonal = false} {}

  /*!
   \brief Copy constructor
   */
  clock_constraint_visitor_t(tchecker::details::clock_constraint_visitor_t &) = default;

  /*!
   \brief Destructor
   */
  virtual ~clock_constraint_visitor_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::details::clock_constraint_visitor_t & operator=(tchecker::details::clock_constraint_visitor_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::details::clock_constraint_visitor_t & operator=(tchecker::details::clock_constraint_visitor_t &&) = default;

  /*!
   \brief Accessor
   \return type of clock constraints in the last visited expression
   */
  inline tchecker::has_clock_constraints_t const & has_clock_constraints() const { return _has_clock_constraints; }

  /*!
   \brief Treat diagonal clock constraint
   \post flag _contain_diagonal has been set to true
   */
  virtual void visit(tchecker::typed_diagonal_clkconstr_expression_t const & expr) { _has_clock_constraints.diagonal = true; }

  /*!
   \brief Treat simple clock constraint
   \post flag _contains_simple has been set to true
   */
  virtual void visit(tchecker::typed_simple_clkconstr_expression_t const & expr) { _has_clock_constraints.simple = true; }

  /* Other visitors: recursion or do nothing */

  virtual void visit(tchecker::typed_var_expression_t const & expr) {}

  virtual void visit(tchecker::typed_bounded_var_expression_t const & expr) {}

  virtual void visit(tchecker::typed_array_expression_t const & expr) {}

  virtual void visit(tchecker::typed_int_expression_t const &) {}

  virtual void visit(tchecker::typed_par_expression_t const & expr) { expr.expr().visit(*this); }

  virtual void visit(tchecker::typed_binary_expression_t const & expr)
  {
    expr.left_operand().visit(*this);
    expr.right_operand().visit(*this);
  }

  virtual void visit(tchecker::typed_unary_expression_t const & expr) { expr.operand().visit(*this); }

  virtual void visit(tchecker::typed_ite_expression_t const & expr) {}

private:
  tchecker::has_clock_constraints_t _has_clock_constraints; /*!< Type of clock constraints found in last visited expression */
};

} // end of namespace details

tchecker::has_clock_constraints_t has_clock_constraints(tchecker::typed_expression_t const & expr)
{
  tchecker::details::clock_constraint_visitor_t v;
  expr.visit(v);
  return v.has_clock_constraints();
}

} // end of namespace tchecker
