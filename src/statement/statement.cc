/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <exception>
#include <sstream>

#include "tchecker/statement/statement.hh"

namespace tchecker {

// statement_t

std::string statement_t::to_string() const
{
  std::stringstream s;
  output(s);
  return s.str();
}

std::ostream & operator<<(std::ostream & os, tchecker::statement_t const & stmt) { return stmt.output(os); }

// nop_statement_t

nop_statement_t::nop_statement_t() = default;

nop_statement_t::~nop_statement_t() = default;

std::ostream & nop_statement_t::output(std::ostream & os) const { return os << "nop"; }

tchecker::nop_statement_t * nop_statement_t::clone() const { return new tchecker::nop_statement_t(); }

void nop_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

// assign_statement_t

assign_statement_t::assign_statement_t(std::shared_ptr<tchecker::lvalue_expression_t const> const & lvalue,
                                       std::shared_ptr<tchecker::expression_t const> const & rvalue)
    : _lvalue(lvalue), _rvalue(rvalue)
{
  if (_lvalue.get() == nullptr)
    throw std::invalid_argument("nullptr lvalue");
  if (_rvalue.get() == nullptr)
    throw std::invalid_argument("nullptr rvalue");
}

assign_statement_t::~assign_statement_t() = default;

std::ostream & assign_statement_t::output(std::ostream & os) const { return os << *_lvalue << "=" << *_rvalue; }

tchecker::assign_statement_t * assign_statement_t::clone() const
{
  std::shared_ptr<tchecker::lvalue_expression_t const> lvalue_clone{_lvalue->clone()};
  std::shared_ptr<tchecker::expression_t const> rvalue_clone{_rvalue->clone()};
  return new tchecker::assign_statement_t(lvalue_clone, rvalue_clone);
}

void assign_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

// sequence_statement_t

sequence_statement_t::sequence_statement_t(std::shared_ptr<tchecker::statement_t const> const & first,
                                           std::shared_ptr<tchecker::statement_t const> const & second)
    : _first(first), _second(second)
{
  if (first.get() == nullptr)
    throw std::invalid_argument("nullptr first statement");
  if (second.get() == nullptr)
    throw std::invalid_argument("nullptr second statement");
}

sequence_statement_t::~sequence_statement_t() = default;

tchecker::sequence_statement_t * sequence_statement_t::clone() const
{
  std::shared_ptr<tchecker::statement_t const> first_clone{_first->clone()};
  std::shared_ptr<tchecker::statement_t const> second_clone{_second->clone()};
  return new tchecker::sequence_statement_t(first_clone, second_clone);
}

void sequence_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

// if_statement_t

if_statement_t::if_statement_t(std::shared_ptr<tchecker::expression_t const> const & cond,
                               std::shared_ptr<tchecker::statement_t const> const & then_stmt,
                               std::shared_ptr<tchecker::statement_t const> const & else_stmt)
    : _condition(cond), _then_stmt(then_stmt), _else_stmt(else_stmt)
{
  if (cond.get() == nullptr)
    throw std::invalid_argument("nullptr condition");
  if (then_stmt.get() == nullptr)
    throw std::invalid_argument("nullptr then statement");
  if (else_stmt.get() == nullptr)
    throw std::invalid_argument("nullptr else statement");
}

if_statement_t::~if_statement_t() = default;

std::ostream & if_statement_t::output(std::ostream & os) const
{
  return os << "if " << *_condition << " then " << *_then_stmt << " else " << *_else_stmt << "endif";
}

tchecker::if_statement_t * if_statement_t::clone() const
{
  std::shared_ptr<tchecker::expression_t const> condition_clone{_condition->clone()};
  std::shared_ptr<tchecker::statement_t const> then_stmt_clone{_then_stmt->clone()};
  std::shared_ptr<tchecker::statement_t const> else_stmt_clone{_else_stmt->clone()};
  return new tchecker::if_statement_t(condition_clone, then_stmt_clone, else_stmt_clone);
}

void if_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

// while_statement_t

while_statement_t::while_statement_t(std::shared_ptr<tchecker::expression_t const> const & cond,
                                     std::shared_ptr<tchecker::statement_t const> const & stmt)
    : _condition(cond), _stmt(stmt)
{
  if (cond.get() == nullptr)
    throw std::invalid_argument("nullptr condition");
  if (stmt.get() == nullptr)
    throw std::invalid_argument("nullptr iterated statement");
}

while_statement_t::~while_statement_t() = default;

std::ostream & while_statement_t::output(std::ostream & os) const
{
  return os << "while " << *_condition << " do " << *_stmt << " done";
}

tchecker::while_statement_t * while_statement_t::clone() const
{
  std::shared_ptr<tchecker::expression_t const> condition_clone{_condition->clone()};
  std::shared_ptr<tchecker::statement_t const> stmt_clone{_stmt->clone()};
  return new tchecker::while_statement_t(condition_clone, stmt_clone);
}

void while_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

// local_var_statement_t

local_var_statement_t::local_var_statement_t(std::shared_ptr<tchecker::var_expression_t const> const & variable)
    : _variable(variable), _initial_value(std::make_shared<tchecker::int_expression_t const>(0))
{
  if (_variable.get() == nullptr)
    throw std::invalid_argument("nullptr local variable");
}

local_var_statement_t::local_var_statement_t(std::shared_ptr<tchecker::var_expression_t const> const & variable,
                                             std::shared_ptr<tchecker::expression_t const> const & init)
    : _variable(variable), _initial_value(init)
{
  if (_variable.get() == nullptr)
    throw std::invalid_argument("nullptr local variable");
  if (_initial_value.get() == nullptr)
    throw std::invalid_argument("nullptr initial value");
}

local_var_statement_t::~local_var_statement_t() = default;

std::ostream & local_var_statement_t::output(std::ostream & os) const
{
  return os << "local " << (*_variable) << " = " << (*_initial_value);
}

tchecker::local_var_statement_t * local_var_statement_t::clone() const
{
  std::shared_ptr<tchecker::var_expression_t const> variable_clone{_variable->clone()};
  std::shared_ptr<tchecker::expression_t const> initial_value_clone{_initial_value->clone()};
  return new tchecker::local_var_statement_t(variable_clone, initial_value_clone);
}

void local_var_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

// local_array_statement_t

local_array_statement_t::local_array_statement_t(std::shared_ptr<tchecker::var_expression_t const> const & variable,
                                                 std::shared_ptr<tchecker::expression_t const> const & size)
    : _variable(variable), _size(size)
{
  if (_variable.get() == nullptr)
    throw std::invalid_argument("nullptr local variable");
  if (_size.get() == nullptr)
    throw std::invalid_argument("nullptr size");
}

local_array_statement_t::~local_array_statement_t() = default;

std::ostream & local_array_statement_t::output(std::ostream & os) const
{
  return os << "local " << *_variable << "[" << *_size << "]";
}

tchecker::local_array_statement_t * local_array_statement_t::clone() const
{
  std::shared_ptr<tchecker::var_expression_t const> variable_clone{_variable->clone()};
  std::shared_ptr<tchecker::expression_t const> size_clone{_size->clone()};
  return new tchecker::local_array_statement_t(variable_clone, size_clone);
}

void local_array_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

} // end of namespace tchecker
