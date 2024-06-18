/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/expression/typechecking.hh"
#include "tchecker/statement/type_inference.hh"
#include "tchecker/statement/typechecking.hh"
#include <tchecker/expression/static_analysis.hh>
#include <tchecker/expression/type_inference.hh>

namespace tchecker {

namespace details {

/*!
 \class statement_typechecker_t
 \brief Statement typechecking visitor
 */
class statement_typechecker_t : public tchecker::statement_visitor_t {
public:
  /*!
   \brief Constructor
   \param localvars : local variables
   \param intvars : integer variables
   \param clocks : clock variables
   \param error : error logging function
   */
  statement_typechecker_t(tchecker::integer_variables_t const & localvars, tchecker::integer_variables_t const & intvars,
                          tchecker::clock_variables_t const & clocks, std::function<void(std::string const &)> error)
      : _typed_stmt(nullptr), _localvars(localvars), _intvars(intvars), _clocks(clocks), _error(error)
  {
  }

  /*!
   \brief Copy constructor (DELETED)
   */
  statement_typechecker_t(tchecker::details::statement_typechecker_t const &) = delete;

  /*!
   \brief Move constructor (DELETED)
   */
  statement_typechecker_t(tchecker::details::statement_typechecker_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~statement_typechecker_t() = default;

  /*!
   \brief Assignment operator (DELETED)
   */
  tchecker::details::statement_typechecker_t & operator=(tchecker::details::statement_typechecker_t const &) = delete;

  /*!
   \brief Move assignment operator (DELETED)
   */
  tchecker::details::statement_typechecker_t & operator=(tchecker::details::statement_typechecker_t &&) = delete;

  /*!
   \brief Get and clear typed statement
   \return typed statement
   \post clear internal typed statement
   */
  std::shared_ptr<tchecker::typed_statement_t> acquire_typed_statement()
  {
    auto acquired_typed_stmt = _typed_stmt;
    _typed_stmt = nullptr;
    return acquired_typed_stmt;
  }

  /*!
   \brief Visitor
   \param stmt : statement
   \post _type_stmt points to a typed clone of this
   */
  virtual void visit(tchecker::nop_statement_t const & stmt)
  {
    _typed_stmt = std::make_shared<tchecker::typed_nop_statement_t>(tchecker::STMT_TYPE_NOP);
  }

  /*!
   \brief Visitor
   \param stmt : assignment statement
   \post _type_stmt points to a typed clone of this
   */
  virtual void visit(tchecker::assign_statement_t const & stmt)
  {
    // Left and right values
    auto typed_lvalue = std::dynamic_pointer_cast<tchecker::typed_lvalue_expression_t const>(
        tchecker::typecheck(stmt.lvalue(), _localvars, _intvars, _clocks, _error));

    std::shared_ptr<tchecker::typed_expression_t const> typed_rvalue =
        tchecker::typecheck(stmt.rvalue(), _localvars, _intvars, _clocks, _error);

    // Typed statement
    enum tchecker::statement_type_t stmt_type = type_assign(typed_lvalue->type(), typed_rvalue->type());

    switch (stmt_type) {
    case STMT_TYPE_CLKASSIGN_INT:
      _typed_stmt = std::make_shared<tchecker::typed_int_to_clock_assign_statement_t>(stmt_type, typed_lvalue, typed_rvalue);
      break;
    case STMT_TYPE_CLKASSIGN_CLK:
      _typed_stmt = std::make_shared<tchecker::typed_clock_to_clock_assign_statement_t>(
          stmt_type, typed_lvalue, std::dynamic_pointer_cast<tchecker::typed_lvalue_expression_t const>(typed_rvalue));
      break;
    case STMT_TYPE_CLKASSIGN_SUM:
      _typed_stmt = std::make_shared<tchecker::typed_sum_to_clock_assign_statement_t>(stmt_type, typed_lvalue, typed_rvalue);
      break;
    default: // either STMT_TYPE_INTASSIGN or STMT_TYPE_BAD
      _typed_stmt = std::make_shared<tchecker::typed_assign_statement_t>(stmt_type, typed_lvalue, typed_rvalue);
      break;
    }

    // Report bad type
    if (stmt_type != tchecker::STMT_TYPE_BAD)
      return;

    if ((typed_lvalue->type() != tchecker::EXPR_TYPE_BAD) && (typed_rvalue->type() != tchecker::EXPR_TYPE_BAD))
      _error("in statement " + stmt.to_string() + ", invalid assignement");
  }

  /*!
   \brief Visitor
   \param stmt : sequence statement
   \post _type_stmt points to a typed clone of this
   */
  virtual void visit(tchecker::sequence_statement_t const & stmt)
  {
    // First and second statements
    stmt.first().visit(*this);
    std::shared_ptr<tchecker::typed_statement_t const> typed_first{acquire_typed_statement()};

    stmt.second().visit(*this);
    std::shared_ptr<tchecker::typed_statement_t const> typed_second{acquire_typed_statement()};

    // Typed statement
    enum tchecker::statement_type_t stmt_type = type_seq(typed_first->type(), typed_second->type());

    _typed_stmt = std::make_shared<tchecker::typed_sequence_statement_t>(stmt_type, typed_first, typed_second);

    // Report bad type
    if (stmt_type != tchecker::STMT_TYPE_BAD)
      return;

    if ((typed_first->type() != tchecker::STMT_TYPE_BAD) && (typed_second->type() != tchecker::STMT_TYPE_BAD))
      _error("in statement " + stmt.to_string() + ", invalid sequence of " + stmt.first().to_string() + " then " +
             stmt.second().to_string());
  }

  /*!
   \brief Visitor
   \param stmt : if statement
   \post _type_stmt points to a typed clone of this
   */
  virtual void visit(tchecker::if_statement_t const & stmt)
  {
    std::shared_ptr<tchecker::typed_expression_t const> typed_cond{
        tchecker::typecheck(stmt.condition(), _localvars, _intvars, _clocks, _error)};

    stmt.then_stmt().visit(*this);
    std::shared_ptr<tchecker::typed_statement_t const> typed_then{acquire_typed_statement()};

    stmt.else_stmt().visit(*this);
    std::shared_ptr<tchecker::typed_statement_t const> typed_else{acquire_typed_statement()};

    // Typed statement
    enum tchecker::statement_type_t stmt_type = type_if(typed_cond->type(), typed_then->type(), typed_else->type());

    _typed_stmt = std::make_shared<tchecker::typed_if_statement_t>(stmt_type, typed_cond, typed_then, typed_else);

    // Report bad type
    if (stmt_type != tchecker::STMT_TYPE_BAD)
      return;

    if (!tchecker::bool_valued(typed_cond->type()))
      _error("in statement " + stmt.to_string() + ", not a Boolean condition '" + stmt.condition().to_string());
    else
      _error("invalid if-then-else statement " + stmt.to_string());
  }

  /*!
   \brief Visitor
   \param stmt : while statement
   \post _type_stmt points to a typed clone of this
   */
  virtual void visit(tchecker::while_statement_t const & stmt)
  {
    std::shared_ptr<tchecker::typed_expression_t const> typed_cond{
        tchecker::typecheck(stmt.condition(), _localvars, _intvars, _clocks, _error)};

    tchecker::integer_variables_t lvars(_localvars);
    stmt.statement().visit(*this);
    _localvars = lvars;

    std::shared_ptr<tchecker::typed_statement_t const> typed_stmt{acquire_typed_statement()};

    // Typed statement
    enum tchecker::statement_type_t stmt_type = type_while(typed_cond->type(), typed_stmt->type());

    _typed_stmt = std::make_shared<tchecker::typed_while_statement_t>(stmt_type, typed_cond, typed_stmt);
    ;

    // Report bad type
    if (stmt_type != tchecker::STMT_TYPE_BAD)
      return;

    if (!tchecker::bool_valued(typed_cond->type()))
      _error("in statement " + stmt.to_string() + ", not a Boolean condition '" + stmt.condition().to_string());
    else
      _error("invalid while statement " + stmt.to_string());
  }

  /*!
   \brief Visitor
   \param stmt : local var statement
   \post _type_stmt points to a typed clone of this
   */
  virtual void visit(tchecker::local_var_statement_t const & stmt)
  {
    enum tchecker::statement_type_t stmt_type;

    std::string name = stmt.variable().name();
    if (_localvars.is_variable(name)) {
      stmt_type = tchecker::STMT_TYPE_BAD;
      _error("local variable already exists: " + name);
    }
    else if (_intvars.is_variable(name)) {
      stmt_type = tchecker::STMT_TYPE_BAD;
      _error("local variable already exists: " + name);
    }
    else {
      stmt_type = tchecker::STMT_TYPE_LOCAL_INT;
      _localvars.declare(name, 1, tchecker::int_minval, tchecker::int_maxval, 0);
    }

    auto variable = std::dynamic_pointer_cast<tchecker::typed_var_expression_t const>(
        tchecker::typecheck(stmt.variable(), _localvars, _intvars, _clocks, _error));

    std::shared_ptr<tchecker::typed_expression_t const> init{
        tchecker::typecheck(stmt.initial_value(), _localvars, _intvars, _clocks, _error)};

    if (!tchecker::integer_valued(init->type()))
      stmt_type = tchecker::STMT_TYPE_BAD;

    _typed_stmt = std::make_shared<tchecker::typed_local_var_statement_t>(stmt_type, variable, init);
  }

  /*!
   \brief Visitor
   \param stmt : local array statement
   \post _type_stmt points to a typed clone of this
   */
  virtual void visit(tchecker::local_array_statement_t const & stmt)
  {
    enum tchecker::statement_type_t stmt_type = tchecker::STMT_TYPE_BAD;

    std::string name = stmt.variable().name();
    std::shared_ptr<tchecker::typed_expression_t const> szexpr{
        tchecker::typecheck(stmt.size(), _localvars, _intvars, _clocks, _error)};

    if (!integer_valued(szexpr->type())) {
      _error("array size is not an integer: " + szexpr->to_string());
    }
    else if (_localvars.is_variable(name)) {
      _error("local variable already exists: " + name);
    }
    else if (_intvars.is_variable(name)) {
      _error("local variable already exists as a global one: " + name);
    }
    else {
      try {
        auto size = tchecker::const_evaluate(stmt.size());
        stmt_type = tchecker::STMT_TYPE_LOCAL_ARRAY;
        szexpr = std::make_shared<tchecker::typed_int_expression_t>(EXPR_TYPE_INTTERM, size);
        _localvars.declare(name, size, tchecker::int_minval, tchecker::int_maxval, 0);
      }
      catch (...) {
        _error("can't compute array size:" + stmt.to_string());
      }
    }
    auto variable = std::dynamic_pointer_cast<tchecker::typed_var_expression_t const>(
        tchecker::typecheck(stmt.variable(), _localvars, _intvars, _clocks, _error));
    _typed_stmt = std::make_shared<tchecker::typed_local_array_statement_t>(stmt_type, variable, szexpr);
  }

protected:
  std::shared_ptr<tchecker::typed_statement_t> _typed_stmt; /*!< Typed statement */
  tchecker::integer_variables_t _localvars;                 /*!< Integer variables */
  tchecker::integer_variables_t const & _intvars;           /*!< Integer variables */
  tchecker::clock_variables_t const & _clocks;              /*!< Clock variables */
  std::function<void(std::string const &)> _error;          /*!< Error logging func */
};

} // end of namespace details

std::shared_ptr<tchecker::typed_statement_t> typecheck(tchecker::statement_t const & stmt,
                                                       tchecker::integer_variables_t const & localvars,
                                                       tchecker::integer_variables_t const & intvars,
                                                       tchecker::clock_variables_t const & clocks,
                                                       std::function<void(std::string const &)> error)
{
  tchecker::details::statement_typechecker_t v(localvars, intvars, clocks, error);
  stmt.visit(v);
  return v.acquire_typed_statement();
}

} // end of namespace tchecker
