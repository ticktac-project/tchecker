/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/expression/typed_expression.hh"
#include "tchecker/expression/type_inference.hh"

namespace tchecker {

/* enum expression_type_t */

std::ostream & operator<<(std::ostream & os, enum tchecker::expression_type_t type)
{
  switch (type) {
  case EXPR_TYPE_BAD:
    return os << "BAD";
  case EXPR_TYPE_INTTERM:
    return os << "INTTERM";
  case EXPR_TYPE_INTVAR:
    return os << "INTVAR";
  case EXPR_TYPE_INTARRAY:
    return os << "INTARRAY";
  case EXPR_TYPE_LOCALINTVAR:
    return os << "LOCALINTVAR";
  case EXPR_TYPE_LOCALINTARRAY:
    return os << "LOCALINTARRAY";
  case EXPR_TYPE_INTLVALUE:
    return os << "INTLVALUE";
  case EXPR_TYPE_CLKVAR:
    return os << "CLKVAR";
  case EXPR_TYPE_CLKARRAY:
    return os << "CLKARRAY";
  case EXPR_TYPE_CLKLVALUE:
    return os << "CLKLVALUE";
  case EXPR_TYPE_CLKDIFF:
    return os << "CLKDIFF";
  case EXPR_TYPE_INTCLKSUM:
    return os << "INTCLKSUM";
  case EXPR_TYPE_ATOMIC_PREDICATE:
    return os << "ATOMIC_PREDICATE";
  case EXPR_TYPE_CLKCONSTR_SIMPLE:
    return os << "CLKCONSTR_SIMPLE";
  case EXPR_TYPE_CLKCONSTR_DIAGONAL:
    return os << "CLKCONSTR_DIAGONAL";
  case EXPR_TYPE_CONJUNCTIVE_FORMULA:
    return os << "CONJUNCTIVE_FORMULA";
  default:
    throw std::runtime_error("incomplete switch statement");
  }
}

/* typed_expression_t */

typed_expression_t::typed_expression_t(enum tchecker::expression_type_t type) : _type(type) {}

/* typed_lvalue_expression_t */

typed_lvalue_expression_t::typed_lvalue_expression_t(enum tchecker::expression_type_t type) : tchecker::typed_expression_t(type)
{
}

/* typed_int_expression_t */

typed_int_expression_t::typed_int_expression_t(tchecker::expression_type_t type, tchecker::integer_t value)
    : tchecker::typed_expression_t(type), tchecker::int_expression_t(value)
{
}

tchecker::typed_int_expression_t * typed_int_expression_t::clone() const
{
  return new tchecker::typed_int_expression_t(_type, _value);
}

void typed_int_expression_t::visit(tchecker::typed_expression_visitor_t & v) const { v.visit(*this); }

void typed_int_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

/* typed_var_expression_t */

typed_var_expression_t::typed_var_expression_t(enum tchecker::expression_type_t type, std::string const & name,
                                               tchecker::variable_id_t id, tchecker::variable_size_t size)
    : tchecker::typed_lvalue_expression_t(type), tchecker::var_expression_t(name), _id(id), _size(size)
{
  if (size < 1)
    throw std::invalid_argument("variable size should be >= 1");
}

tchecker::typed_var_expression_t * typed_var_expression_t::clone() const
{
  return new tchecker::typed_var_expression_t(_type, _name, _id, _size);
}

void typed_var_expression_t::visit(tchecker::typed_expression_visitor_t & v) const { v.visit(*this); }

void typed_var_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

/* typed_bounded_var_expression_t */

typed_bounded_var_expression_t::typed_bounded_var_expression_t(enum tchecker::expression_type_t type, std::string const & name,
                                                               tchecker::variable_id_t id, tchecker::variable_size_t size,
                                                               tchecker::integer_t min, tchecker::integer_t max)
    : tchecker::typed_var_expression_t(type, name, id, size), _min(min), _max(max)
{
  if (_min > _max)
    throw std::invalid_argument("variable min should be <= max");
}

tchecker::typed_bounded_var_expression_t * typed_bounded_var_expression_t::clone() const
{
  return new tchecker::typed_bounded_var_expression_t(_type, _name, _id, _size, _min, _max);
}

void typed_bounded_var_expression_t::visit(tchecker::typed_expression_visitor_t & v) const { v.visit(*this); }

void typed_bounded_var_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

/* typed_array_expression_t */

typed_array_expression_t::typed_array_expression_t(enum tchecker::expression_type_t type,
                                                   std::shared_ptr<tchecker::typed_var_expression_t const> const & variable,
                                                   std::shared_ptr<tchecker::typed_expression_t const> const & offset)
    : tchecker::typed_lvalue_expression_t(type), tchecker::array_expression_t(variable, offset)
{
}

tchecker::typed_var_expression_t const & typed_array_expression_t::variable() const
{
  return dynamic_cast<tchecker::typed_var_expression_t const &>(tchecker::array_expression_t::variable());
}

std::shared_ptr<tchecker::typed_var_expression_t const> typed_array_expression_t::variable_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_var_expression_t const>(tchecker::array_expression_t::variable_ptr());
}

tchecker::typed_expression_t const & typed_array_expression_t::offset() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(tchecker::array_expression_t::offset());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_array_expression_t::offset_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(tchecker::array_expression_t::offset_ptr());
}

tchecker::typed_array_expression_t * typed_array_expression_t::clone() const
{
  std::shared_ptr<tchecker::typed_var_expression_t const> variable_clone{variable().clone()};
  std::shared_ptr<tchecker::typed_expression_t const> offset_clone{offset().clone()};
  return new tchecker::typed_array_expression_t(_type, variable_clone, offset_clone);
}

void typed_array_expression_t::visit(tchecker::typed_expression_visitor_t & v) const { v.visit(*this); }

void typed_array_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

/* typed_par_expression_t */

typed_par_expression_t::typed_par_expression_t(enum tchecker::expression_type_t type,
                                               std::shared_ptr<tchecker::typed_expression_t const> const & expr)
    : tchecker::typed_expression_t(type), tchecker::par_expression_t(expr)
{
}

tchecker::typed_expression_t const & typed_par_expression_t::expr() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(tchecker::par_expression_t::expr());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_par_expression_t::expr_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(tchecker::par_expression_t::expr_ptr());
}

tchecker::typed_par_expression_t * typed_par_expression_t::clone() const
{
  std::shared_ptr<tchecker::typed_expression_t const> expr_clone{expr().clone()};
  return new tchecker::typed_par_expression_t(_type, expr_clone);
}

void typed_par_expression_t::visit(tchecker::typed_expression_visitor_t & v) const { v.visit(*this); }

void typed_par_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

/* typed_binary_expression_t */

typed_binary_expression_t::typed_binary_expression_t(enum tchecker::expression_type_t type, enum tchecker::binary_operator_t op,
                                                     std::shared_ptr<tchecker::typed_expression_t const> const & left,
                                                     std::shared_ptr<tchecker::typed_expression_t const> const & right)
    : tchecker::typed_expression_t(type), tchecker::binary_expression_t(op, left, right)
{
}

tchecker::typed_expression_t const & typed_binary_expression_t::left_operand() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(tchecker::binary_expression_t::left_operand());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_binary_expression_t::left_operand_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(tchecker::binary_expression_t::left_operand_ptr());
}

tchecker::typed_expression_t const & typed_binary_expression_t::right_operand() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(tchecker::binary_expression_t::right_operand());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_binary_expression_t::right_operand_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(tchecker::binary_expression_t::right_operand_ptr());
}

tchecker::typed_binary_expression_t * typed_binary_expression_t::clone() const
{
  std::shared_ptr<tchecker::typed_expression_t> left_clone{left_operand().clone()};
  std::shared_ptr<tchecker::typed_expression_t> right_clone{right_operand().clone()};
  return new tchecker::typed_binary_expression_t(_type, _op, left_clone, right_clone);
}

void typed_binary_expression_t::visit(tchecker::typed_expression_visitor_t & v) const { v.visit(*this); }

void typed_binary_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

/* typed_unary_expression_t */

typed_unary_expression_t::typed_unary_expression_t(enum tchecker::expression_type_t type, enum tchecker::unary_operator_t op,
                                                   std::shared_ptr<tchecker::typed_expression_t const> const & operand)
    : tchecker::typed_expression_t(type), tchecker::unary_expression_t(op, operand)
{
}

tchecker::typed_expression_t const & typed_unary_expression_t::operand() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(tchecker::unary_expression_t::operand());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_unary_expression_t::operand_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(tchecker::unary_expression_t::operand_ptr());
}

tchecker::typed_unary_expression_t * typed_unary_expression_t::clone() const
{
  std::shared_ptr<tchecker::typed_expression_t const> operand_clone{operand().clone()};
  return new tchecker::typed_unary_expression_t(_type, _operator, operand_clone);
}

void typed_unary_expression_t::visit(tchecker::typed_expression_visitor_t & v) const { v.visit(*this); }

void typed_unary_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

/* typed_simple_clkconstr_expression_t */

typed_simple_clkconstr_expression_t::typed_simple_clkconstr_expression_t(
    enum tchecker::expression_type_t type, enum tchecker::binary_operator_t op,
    std::shared_ptr<tchecker::typed_expression_t const> const & left,
    std::shared_ptr<tchecker::typed_expression_t const> const & right)
    : tchecker::typed_binary_expression_t(type, op, left, right)
{
  if (!tchecker::clock_valued(left->type()))
    throw std::invalid_argument("expecting a clock-valued left expression");
  if (!tchecker::integer_valued(right->type()))
    throw std::invalid_argument("expecting an integer-valued right expression");
}

tchecker::typed_lvalue_expression_t const & typed_simple_clkconstr_expression_t::clock() const
{
  return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(left_operand());
}

std::shared_ptr<tchecker::typed_lvalue_expression_t const> typed_simple_clkconstr_expression_t::clock_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_lvalue_expression_t const>(left_operand_ptr());
}

tchecker::typed_expression_t const & typed_simple_clkconstr_expression_t::bound() const { return right_operand(); }

std::shared_ptr<tchecker::typed_expression_t const> typed_simple_clkconstr_expression_t::bound_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(right_operand_ptr());
}

void typed_simple_clkconstr_expression_t::visit(tchecker::typed_expression_visitor_t & v) const { v.visit(*this); }

void typed_simple_clkconstr_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

/* typed_diagonal_clkconstr_expression_t */

typed_diagonal_clkconstr_expression_t::typed_diagonal_clkconstr_expression_t(
    enum tchecker::expression_type_t type, enum tchecker::binary_operator_t op,
    std::shared_ptr<tchecker::typed_expression_t const> const & left,
    std::shared_ptr<tchecker::typed_expression_t const> const & right)
    : tchecker::typed_binary_expression_t(type, op, left, right)
{
  if (left->type() != tchecker::EXPR_TYPE_CLKDIFF)
    throw std::invalid_argument("expecting a clock difference left expression");
  if (!tchecker::integer_valued(right->type()))
    throw std::invalid_argument("expecting an integer-valued right expresison");
}

tchecker::typed_lvalue_expression_t const & typed_diagonal_clkconstr_expression_t::first_clock() const
{
  auto const & diagonal = dynamic_cast<tchecker::typed_binary_expression_t const &>(left_operand()); // left expr is x - y
  return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(diagonal.left_operand());
}

std::shared_ptr<tchecker::typed_lvalue_expression_t const> typed_diagonal_clkconstr_expression_t::first_clock_ptr() const
{
  auto const & diagonal = dynamic_cast<tchecker::typed_binary_expression_t const &>(left_operand()); // left expr is x - y
  return std::dynamic_pointer_cast<tchecker::typed_lvalue_expression_t const>(diagonal.left_operand_ptr());
}

tchecker::typed_lvalue_expression_t const & typed_diagonal_clkconstr_expression_t::second_clock() const
{
  auto const & diagonal = dynamic_cast<tchecker::typed_binary_expression_t const &>(left_operand()); // left expr is x - y
  return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(diagonal.right_operand());
}

std::shared_ptr<tchecker::typed_lvalue_expression_t const> typed_diagonal_clkconstr_expression_t::second_clock_ptr() const
{
  auto const & diagonal = dynamic_cast<tchecker::typed_binary_expression_t const &>(left_operand()); // left expr is x - y
  return std::dynamic_pointer_cast<tchecker::typed_lvalue_expression_t const>(diagonal.right_operand_ptr());
}

tchecker::typed_expression_t const & typed_diagonal_clkconstr_expression_t::bound() const { return right_operand(); }

std::shared_ptr<tchecker::typed_expression_t const> typed_diagonal_clkconstr_expression_t::bound_ptr() const
{
  return right_operand_ptr();
}

void typed_diagonal_clkconstr_expression_t::visit(tchecker::typed_expression_visitor_t & v) const { v.visit(*this); }

void typed_diagonal_clkconstr_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

/* typed_ite_expression_t */

typed_ite_expression_t::typed_ite_expression_t(tchecker::expression_type_t type,
                                               std::shared_ptr<tchecker::typed_expression_t const> const & cond,
                                               std::shared_ptr<tchecker::typed_expression_t const> const & then_value,
                                               std::shared_ptr<tchecker::typed_expression_t const> const & else_value)
    : tchecker::typed_expression_t(type), tchecker::ite_expression_t(cond, then_value, else_value)
{
  if (!tchecker::integer_valued(then_value->type()))
    throw std::invalid_argument("expecting integer-valued then expression");
  if (!tchecker::integer_valued(else_value->type()))
    throw std::invalid_argument("expecting integer-valued else expression");
}

tchecker::typed_expression_t const & typed_ite_expression_t::condition() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(tchecker::ite_expression_t::condition());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_ite_expression_t::condition_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(tchecker::ite_expression_t::condition_ptr());
}

tchecker::typed_expression_t const & typed_ite_expression_t::then_value() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(tchecker::ite_expression_t::then_value());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_ite_expression_t::then_value_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(tchecker::ite_expression_t::then_value_ptr());
}

tchecker::typed_expression_t const & typed_ite_expression_t::else_value() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(tchecker::ite_expression_t::else_value());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_ite_expression_t::else_value_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(tchecker::ite_expression_t::else_value_ptr());
}

tchecker::typed_ite_expression_t * typed_ite_expression_t::clone() const
{
  std::shared_ptr<tchecker::typed_expression_t const> condition_clone{condition().clone()};
  std::shared_ptr<tchecker::typed_expression_t const> then_value_clone{then_value().clone()};
  std::shared_ptr<tchecker::typed_expression_t const> else_value_clone{else_value().clone()};
  return new tchecker::typed_ite_expression_t(_type, condition_clone, then_value_clone, else_value_clone);
}

void typed_ite_expression_t::visit(tchecker::typed_expression_visitor_t & v) const { v.visit(*this); }

void typed_ite_expression_t::visit(tchecker::expression_visitor_t & v) const { v.visit(*this); }

} // end of namespace tchecker
