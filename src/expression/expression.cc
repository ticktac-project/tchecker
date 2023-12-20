/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <sstream>

#include "tchecker/expression/expression.hh"

namespace tchecker {

// expression_t

std::string expression_t::to_string() const
{
  std::stringstream s;
  this->output(s);
  return s.str();
}

std::ostream & operator<<(std::ostream & os, tchecker::expression_t const & expr) { return expr.output(os); }

// int_expression_t

int_expression_t::int_expression_t(tchecker::integer_t value) : _value(value) {}

std::ostream & int_expression_t::output(std::ostream & os) const { return os << _value; }

tchecker::int_expression_t * int_expression_t::clone() const { return (new tchecker::int_expression_t(_value)); }

void int_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

// var_expression_t

var_expression_t::var_expression_t(std::string const & name) : _name(name)
{
  if (_name.empty())
    throw std::invalid_argument("empty variable name");
}

std::ostream & var_expression_t::output(std::ostream & os) const { return os << _name; }

tchecker::var_expression_t * var_expression_t::clone() const { return (new tchecker::var_expression_t(_name)); }

void var_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

// array_expression_t

array_expression_t::array_expression_t(std::shared_ptr<tchecker::var_expression_t const> const & variable,
                                       std::shared_ptr<tchecker::expression_t const> const & offset)
    : _variable(variable), _offset(offset)
{
  if (_offset.get() == nullptr)
    throw std::invalid_argument("nullptr offset");
  if (_variable.get() == nullptr)
    throw std::invalid_argument("nullptr variable");
}

array_expression_t::~array_expression_t() = default;

std::ostream & array_expression_t::output(std::ostream & os) const { return os << *_variable << "[" << *_offset << "]"; }

tchecker::array_expression_t * array_expression_t::clone() const
{
  std::shared_ptr<tchecker::var_expression_t const> variable_clone{_variable->clone()};
  std::shared_ptr<tchecker::expression_t const> offset_clone{_offset->clone()};
  return new tchecker::array_expression_t(variable_clone, offset_clone);
}

void array_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

// par_expression_t

par_expression_t::par_expression_t(std::shared_ptr<tchecker::expression_t const> const & expr) : _expr(expr)
{
  if (_expr.get() == nullptr)
    throw std::invalid_argument("nullptr sub expression");
}

par_expression_t::~par_expression_t() = default;

std::ostream & par_expression_t::output(std::ostream & os) const { return os << "(" << *_expr << ")"; }

tchecker::par_expression_t * par_expression_t::clone() const
{
  std::shared_ptr<tchecker::expression_t const> expr_clone{_expr->clone()};
  return new tchecker::par_expression_t(expr_clone);
}

void par_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

// unary_operator_t

std::ostream & operator<<(std::ostream & os, enum unary_operator_t op)
{
  switch (op) {
  case tchecker::EXPR_OP_NEG:
    return os << "-";
  case tchecker::EXPR_OP_LNOT:
    return os << "!";
  default:
    throw std::runtime_error("incomplete switch statement");
  }
}

// unary_expression_t

unary_expression_t::unary_expression_t(enum unary_operator_t op, std::shared_ptr<tchecker::expression_t const> const & expr)
    : _operator(op), _expr(expr)
{
  if (_expr.get() == nullptr)
    throw std::invalid_argument("nullptr expression");
}

unary_expression_t::~unary_expression_t() = default;

std::ostream & unary_expression_t::output(std::ostream & os) const { return os << _operator << *_expr; }

tchecker::unary_expression_t * unary_expression_t::clone() const
{
  std::shared_ptr<tchecker::expression_t const> expr_clone{_expr->clone()};
  return new tchecker::unary_expression_t(_operator, expr_clone);
}

void unary_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

// binary_operator_t

std::ostream & operator<<(std::ostream & os, enum binary_operator_t op)
{
  switch (op) {
  case tchecker::EXPR_OP_LAND:
    return os << "&&";
  case tchecker::EXPR_OP_LT:
    return os << "<";
  case tchecker::EXPR_OP_LE:
    return os << "<=";
  case tchecker::EXPR_OP_EQ:
    return os << "==";
  case tchecker::EXPR_OP_NEQ:
    return os << "!=";
  case tchecker::EXPR_OP_GE:
    return os << ">=";
  case tchecker::EXPR_OP_GT:
    return os << ">";
  case tchecker::EXPR_OP_MINUS:
    return os << "-";
  case tchecker::EXPR_OP_PLUS:
    return os << "+";
  case tchecker::EXPR_OP_TIMES:
    return os << "*";
  case tchecker::EXPR_OP_DIV:
    return os << "/";
  case tchecker::EXPR_OP_MOD:
    return os << "%";
  default:
    throw std::runtime_error("incomplete switch statement");
  }
}

bool predicate(enum tchecker::binary_operator_t op)
{
  return ((op == tchecker::EXPR_OP_LT) || (op == tchecker::EXPR_OP_LE) || (op == tchecker::EXPR_OP_EQ) ||
          (op == tchecker::EXPR_OP_NEQ) || (op == tchecker::EXPR_OP_GE) || (op == tchecker::EXPR_OP_GT));
}

bool is_less(enum tchecker::binary_operator_t op) { return (op == tchecker::EXPR_OP_LE) || (op == tchecker::EXPR_OP_LT); }

enum tchecker::binary_operator_t reverse_cmp(enum tchecker::binary_operator_t op)
{
  switch (op) {
  case tchecker::EXPR_OP_EQ:
    return tchecker::EXPR_OP_EQ;
  case tchecker::EXPR_OP_GE:
    return tchecker::EXPR_OP_LE;
  case tchecker::EXPR_OP_GT:
    return tchecker::EXPR_OP_LT;
  case tchecker::EXPR_OP_LE:
    return tchecker::EXPR_OP_GE;
  case tchecker::EXPR_OP_LT:
    return tchecker::EXPR_OP_GT;
  case tchecker::EXPR_OP_NEQ:
    return tchecker::EXPR_OP_NEQ;
  default:
    throw std::invalid_argument("Cannot reverse non-comparison binary operator");
  }
}

// binary_expression_t

binary_expression_t::binary_expression_t(enum binary_operator_t op, std::shared_ptr<tchecker::expression_t const> const & left,
                                         std::shared_ptr<tchecker::expression_t const> const & right)
    : _op(op), _left(left), _right(right)
{
  if (_left.get() == nullptr)
    throw std::invalid_argument("nullptr left expression");
  if (_right.get() == nullptr)
    throw std::invalid_argument("nullptr right expression");
}

binary_expression_t::~binary_expression_t() = default;

std::ostream & binary_expression_t::output(std::ostream & os) const { return os << *_left << _op << *_right; }

tchecker::binary_expression_t * binary_expression_t::clone() const
{
  std::shared_ptr<tchecker::expression_t const> left_clone{_left->clone()};
  std::shared_ptr<tchecker::expression_t const> right_clone{_right->clone()};
  return new tchecker::binary_expression_t(_op, left_clone, right_clone);
}

void binary_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

// ite_expression_t

ite_expression_t::ite_expression_t(std::shared_ptr<tchecker::expression_t const> const & condition,
                                   std::shared_ptr<tchecker::expression_t const> const & then_value,
                                   std::shared_ptr<tchecker::expression_t const> const & else_value)
    : _condition(condition), _then_value(then_value), _else_value(else_value)
{
  if (_condition.get() == nullptr)
    throw std::invalid_argument("nullptr ite_condition expression");
  if (_then_value.get() == nullptr)
    throw std::invalid_argument("nullptr then_value expression");
  if (_else_value.get() == nullptr)
    throw std::invalid_argument("nullptr else_value expression");
}

ite_expression_t::~ite_expression_t() = default;

std::ostream & ite_expression_t::output(std::ostream & os) const
{
  return os << "if " << condition() << " then " << then_value() << " else " << else_value();
}

tchecker::ite_expression_t * ite_expression_t::clone() const
{
  std::shared_ptr<tchecker::expression_t const> condition_clone{_condition->clone()};
  std::shared_ptr<tchecker::expression_t const> then_value_clone{_then_value->clone()};
  std::shared_ptr<tchecker::expression_t const> else_value_clone{_else_value->clone()};
  return new tchecker::ite_expression_t(condition_clone, then_value_clone, else_value_clone);
}

void ite_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

} // end of namespace tchecker
