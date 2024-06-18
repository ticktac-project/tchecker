/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <tuple>

#include "tchecker/expression/type_inference.hh"
#include "tchecker/statement/typed_statement.hh"

namespace tchecker {

/* enum statement_type_t */

std::ostream & operator<<(std::ostream & os, enum tchecker::statement_type_t type)
{
  switch (type) {
  case STMT_TYPE_BAD:
    return os << "BAD";
  case STMT_TYPE_NOP:
    return os << "NOP";
  case STMT_TYPE_INTASSIGN:
    return os << "INTASSIGN";
  case STMT_TYPE_CLKASSIGN_INT:
    return os << "CLKASSIGN_INT";
  case STMT_TYPE_CLKASSIGN_CLK:
    return os << "CLKASSIGN_CLK";
  case STMT_TYPE_CLKASSIGN_SUM:
    return os << "CLKASSIGN_SUM";
  case STMT_TYPE_SEQ:
    return os << "SEQ";
  case STMT_TYPE_IF:
    return os << "IF";
  case STMT_TYPE_WHILE:
    return os << "WHILE";
  case STMT_TYPE_LOCAL_INT:
    return os << "LOCAL_INT";
  case STMT_TYPE_LOCAL_ARRAY:
    return os << "LOCAL_ARRAY";
  default:
    throw std::runtime_error("incomplete switch statement");
  }
}

/* typed_statement_t */

typed_statement_t::typed_statement_t(enum tchecker::statement_type_t type) : _type(type) {}

typed_statement_t::~typed_statement_t() = default;

/* typed_nop_statement_t */

typed_nop_statement_t::typed_nop_statement_t(enum tchecker::statement_type_t type) : tchecker::typed_statement_t(type) {}

typed_nop_statement_t::~typed_nop_statement_t() = default;

tchecker::typed_nop_statement_t * typed_nop_statement_t::clone() const { return new typed_nop_statement_t(_type); }

void typed_nop_statement_t::visit(tchecker::typed_statement_visitor_t & v) const { v.visit(*this); }

void typed_nop_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

/* typed_assign_statement_t */

typed_assign_statement_t::typed_assign_statement_t(enum tchecker::statement_type_t type,
                                                   std::shared_ptr<tchecker::typed_lvalue_expression_t const> const & lvalue,
                                                   std::shared_ptr<tchecker::typed_expression_t const> const & rvalue)
    : tchecker::typed_statement_t(type), tchecker::assign_statement_t(lvalue, rvalue)
{
  if (lvalue->size() != 1)
    throw std::invalid_argument("invalid lvalue expression");
}

typed_assign_statement_t::~typed_assign_statement_t() = default;

tchecker::typed_lvalue_expression_t const & typed_assign_statement_t::lvalue() const
{
  return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(tchecker::assign_statement_t::lvalue());
}

std::shared_ptr<tchecker::typed_lvalue_expression_t const> typed_assign_statement_t::lvalue_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_lvalue_expression_t const>(tchecker::assign_statement_t::lvalue_ptr());
}

tchecker::typed_expression_t const & typed_assign_statement_t::rvalue() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(tchecker::assign_statement_t::rvalue());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_assign_statement_t::rvalue_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(tchecker::assign_statement_t::rvalue_ptr());
}

tchecker::typed_assign_statement_t * typed_assign_statement_t::clone() const
{
  std::shared_ptr<tchecker::typed_lvalue_expression_t const> lvalue_clone{lvalue().clone()};
  std::shared_ptr<tchecker::typed_expression_t const> rvalue_clone{rvalue().clone()};
  return new typed_assign_statement_t(_type, lvalue_clone, rvalue_clone);
}

void typed_assign_statement_t::visit(tchecker::typed_statement_visitor_t & v) const { v.visit(*this); }

void typed_assign_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

/* typed_int_to_clock_assign_statement_t */

typed_int_to_clock_assign_statement_t::typed_int_to_clock_assign_statement_t(
    enum tchecker::statement_type_t type, std::shared_ptr<tchecker::typed_lvalue_expression_t const> const & lvalue,
    std::shared_ptr<tchecker::typed_expression_t const> const & rvalue)
    : tchecker::typed_assign_statement_t(type, lvalue, rvalue)
{
  if (lvalue->size() != 1)
    throw std::invalid_argument("lvalue of size > 1 is not assignable");
  if (!tchecker::clock_assignable(lvalue->type()))
    throw std::invalid_argument("lvalue is not an assignable clock");
  if (!tchecker::integer_valued(rvalue->type()))
    throw std::invalid_argument("rvalue is not integer valued");
}

typed_int_to_clock_assign_statement_t::~typed_int_to_clock_assign_statement_t() = default;

tchecker::typed_lvalue_expression_t const & typed_int_to_clock_assign_statement_t::clock() const
{
  return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(lvalue());
}

std::shared_ptr<tchecker::lvalue_expression_t const> typed_int_to_clock_assign_statement_t::clock_ptr() const
{
  return lvalue_ptr();
}

tchecker::typed_expression_t const & typed_int_to_clock_assign_statement_t::value() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(rvalue());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_int_to_clock_assign_statement_t::value_ptr() const
{
  return rvalue_ptr();
}

tchecker::typed_int_to_clock_assign_statement_t * typed_int_to_clock_assign_statement_t::clone() const
{
  std::shared_ptr<tchecker::typed_lvalue_expression_t const> lvalue_clone{lvalue().clone()};
  std::shared_ptr<tchecker::typed_expression_t const> rvalue_clone{rvalue().clone()};
  return new tchecker::typed_int_to_clock_assign_statement_t{_type, lvalue_clone, rvalue_clone};
}

void typed_int_to_clock_assign_statement_t::visit(tchecker::typed_statement_visitor_t & v) const { v.visit(*this); }

void typed_int_to_clock_assign_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

/* typed_clock_to_clock_assign_statement_t */

typed_clock_to_clock_assign_statement_t::typed_clock_to_clock_assign_statement_t(
    enum tchecker::statement_type_t type, std::shared_ptr<tchecker::typed_lvalue_expression_t const> const & lvalue,
    std::shared_ptr<tchecker::typed_lvalue_expression_t const> const & rvalue)
    : tchecker::typed_assign_statement_t(type, lvalue, rvalue)
{
  if (lvalue->size() != 1)
    throw std::invalid_argument("lvalue of size > 1 is not assignable");
  if (!tchecker::clock_assignable(lvalue->type()))
    throw std::invalid_argument("lvalue is not an assignable clock");
  if (!tchecker::clock_valued(rvalue->type()))
    throw std::invalid_argument("rvalue is not clock valued");
}

typed_clock_to_clock_assign_statement_t::~typed_clock_to_clock_assign_statement_t() = default;

tchecker::typed_lvalue_expression_t const & typed_clock_to_clock_assign_statement_t::lclock() const
{
  return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(lvalue());
}

std::shared_ptr<tchecker::typed_lvalue_expression_t const> typed_clock_to_clock_assign_statement_t::lclock_ptr() const
{
  return lvalue_ptr();
}

tchecker::typed_lvalue_expression_t const & typed_clock_to_clock_assign_statement_t::rclock() const
{
  return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(rvalue());
}

std::shared_ptr<tchecker::typed_lvalue_expression_t const> typed_clock_to_clock_assign_statement_t::rclock_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_lvalue_expression_t const>(rvalue_ptr());
}

tchecker::typed_clock_to_clock_assign_statement_t * typed_clock_to_clock_assign_statement_t::clone() const
{
  std::shared_ptr<tchecker::typed_lvalue_expression_t const> lvalue_clone{lvalue().clone()};
  std::shared_ptr<tchecker::typed_lvalue_expression_t const> rvalue_clone{rclock().clone()};
  return new tchecker::typed_clock_to_clock_assign_statement_t{_type, lvalue_clone, rvalue_clone};
}

void typed_clock_to_clock_assign_statement_t::visit(tchecker::typed_statement_visitor_t & v) const { v.visit(*this); }

void typed_clock_to_clock_assign_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

/* typed_sum_to_clock_assign_statement_t */

typed_sum_to_clock_assign_statement_t::typed_sum_to_clock_assign_statement_t(
    enum tchecker::statement_type_t type, std::shared_ptr<tchecker::typed_lvalue_expression_t const> const & lvalue,
    std::shared_ptr<tchecker::typed_expression_t const> const & rvalue)
    : tchecker::typed_assign_statement_t(type, lvalue, rvalue)
{
  if (lvalue->size() != 1)
    throw std::invalid_argument("lvalue of size > 1 is not assignable");
  if (!tchecker::clock_assignable(lvalue->type()))
    throw std::invalid_argument("lvalue is not an assignable clock");
  if (rvalue->type() != tchecker::EXPR_TYPE_INTCLKSUM)
    throw std::invalid_argument("rvalue is not the sum of an int-clock sum");
}

typed_sum_to_clock_assign_statement_t::~typed_sum_to_clock_assign_statement_t() = default;

tchecker::typed_lvalue_expression_t const & typed_sum_to_clock_assign_statement_t::lclock() const
{
  return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(lvalue());
}

std::shared_ptr<tchecker::typed_lvalue_expression_t const> typed_sum_to_clock_assign_statement_t::lclock_ptr() const
{
  return lvalue_ptr();
}

tchecker::typed_lvalue_expression_t const & typed_sum_to_clock_assign_statement_t::rclock() const
{
  auto const & sum = dynamic_cast<tchecker::typed_binary_expression_t const &>(rvalue());
  return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(sum.right_operand());
}

std::shared_ptr<tchecker::typed_lvalue_expression_t const> typed_sum_to_clock_assign_statement_t::rclock_ptr() const
{
  auto const & sum = dynamic_cast<tchecker::typed_binary_expression_t const &>(rvalue());
  return std::dynamic_pointer_cast<tchecker::typed_lvalue_expression_t const>(sum.right_operand_ptr());
}

tchecker::typed_expression_t const & typed_sum_to_clock_assign_statement_t::value() const
{
  auto const & sum = dynamic_cast<tchecker::typed_binary_expression_t const &>(rvalue());
  return dynamic_cast<tchecker::typed_expression_t const &>(sum.left_operand());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_sum_to_clock_assign_statement_t::value_ptr() const
{
  auto const & sum = dynamic_cast<tchecker::typed_binary_expression_t const &>(rvalue());
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(sum.left_operand_ptr());
}

tchecker::typed_sum_to_clock_assign_statement_t * typed_sum_to_clock_assign_statement_t::clone() const
{
  std::shared_ptr<typed_lvalue_expression_t const> lvalue_clone{lvalue().clone()};
  std::shared_ptr<typed_expression_t const> rvalue_clone{rvalue().clone()};
  return new tchecker::typed_sum_to_clock_assign_statement_t{_type, lvalue_clone, rvalue_clone};
}

void typed_sum_to_clock_assign_statement_t::visit(tchecker::typed_statement_visitor_t & v) const { v.visit(*this); }

void typed_sum_to_clock_assign_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

/* typed_sequence_statement_t */

typed_sequence_statement_t::typed_sequence_statement_t(enum tchecker::statement_type_t type,
                                                       std::shared_ptr<tchecker::typed_statement_t const> const & first,
                                                       std::shared_ptr<tchecker::typed_statement_t const> const & second)
    : tchecker::typed_statement_t(type), tchecker::sequence_statement_t(first, second)
{
}

typed_sequence_statement_t::~typed_sequence_statement_t() = default;

tchecker::typed_statement_t const & typed_sequence_statement_t::first() const
{
  return dynamic_cast<tchecker::typed_statement_t const &>(tchecker::sequence_statement_t::first());
}

std::shared_ptr<tchecker::typed_statement_t const> typed_sequence_statement_t::first_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_statement_t const>(tchecker::sequence_statement_t::first_ptr());
}

tchecker::typed_statement_t const & typed_sequence_statement_t::second() const
{
  return dynamic_cast<tchecker::typed_statement_t const &>(tchecker::sequence_statement_t::second());
}

std::shared_ptr<tchecker::typed_statement_t const> typed_sequence_statement_t::second_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_statement_t const>(tchecker::sequence_statement_t::second_ptr());
}

tchecker::typed_sequence_statement_t * typed_sequence_statement_t::clone() const
{
  std::shared_ptr<tchecker::typed_statement_t const> first_clone{first().clone()};
  std::shared_ptr<tchecker::typed_statement_t const> second_clone{second().clone()};
  return new tchecker::typed_sequence_statement_t(_type, first_clone, second_clone);
}

void typed_sequence_statement_t::visit(tchecker::typed_statement_visitor_t & v) const { v.visit(*this); }

void typed_sequence_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

/* typed_if_statement_t */

typed_if_statement_t::typed_if_statement_t(enum tchecker::statement_type_t type,
                                           std::shared_ptr<tchecker::typed_expression_t const> const & cond,
                                           std::shared_ptr<tchecker::typed_statement_t const> const & then_stmt,
                                           std::shared_ptr<tchecker::typed_statement_t const> const & else_stmt)
    : tchecker::typed_statement_t(type), tchecker::if_statement_t(cond, then_stmt, else_stmt)
{
}

typed_if_statement_t::~typed_if_statement_t() = default;

tchecker::typed_expression_t const & typed_if_statement_t::condition() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(tchecker::if_statement_t::condition());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_if_statement_t::condition_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(tchecker::if_statement_t::condition_ptr());
}

tchecker::typed_statement_t const & typed_if_statement_t::then_stmt() const
{
  return dynamic_cast<tchecker::typed_statement_t const &>(tchecker::if_statement_t::then_stmt());
}

std::shared_ptr<tchecker::typed_statement_t const> typed_if_statement_t::then_stmt_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_statement_t const>(tchecker::if_statement_t::then_stmt_ptr());
}

tchecker::typed_statement_t const & typed_if_statement_t::else_stmt() const
{
  return dynamic_cast<tchecker::typed_statement_t const &>(tchecker::if_statement_t::else_stmt());
}

std::shared_ptr<tchecker::typed_statement_t const> typed_if_statement_t::else_stmt_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_statement_t const>(tchecker::if_statement_t::else_stmt_ptr());
}

tchecker::typed_if_statement_t * typed_if_statement_t::clone() const
{
  std::shared_ptr<tchecker::typed_expression_t const> cond_clone{condition().clone()};
  std::shared_ptr<tchecker::typed_statement_t const> then_clone{then_stmt().clone()};
  std::shared_ptr<tchecker::typed_statement_t const> else_clone{else_stmt().clone()};
  return new tchecker::typed_if_statement_t(_type, cond_clone, then_clone, else_clone);
}

void typed_if_statement_t::visit(tchecker::typed_statement_visitor_t & v) const { v.visit(*this); }

void typed_if_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

/* typed_while_statement_t */

typed_while_statement_t::typed_while_statement_t(enum tchecker::statement_type_t type,
                                                 std::shared_ptr<tchecker::typed_expression_t const> const & cond,
                                                 std::shared_ptr<tchecker::typed_statement_t const> const & stmt)
    : tchecker::typed_statement_t(type), tchecker::while_statement_t(cond, stmt)
{
}

typed_while_statement_t::~typed_while_statement_t() = default;

tchecker::typed_expression_t const & typed_while_statement_t::condition() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(tchecker::while_statement_t::condition());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_while_statement_t::condition_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(tchecker::while_statement_t::condition_ptr());
}

tchecker::typed_statement_t const & typed_while_statement_t::statement() const
{
  return dynamic_cast<tchecker::typed_statement_t const &>(tchecker::while_statement_t::statement());
}

std::shared_ptr<tchecker::typed_statement_t const> typed_while_statement_t::statement_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_statement_t const>(tchecker::while_statement_t::statement_ptr());
}

tchecker::typed_while_statement_t * typed_while_statement_t::clone() const
{
  std::shared_ptr<tchecker::typed_expression_t const> cond_clone{condition().clone()};
  std::shared_ptr<tchecker::typed_statement_t const> stmt_clone{statement().clone()};
  return new tchecker::typed_while_statement_t(_type, cond_clone, stmt_clone);
}

void typed_while_statement_t::visit(tchecker::typed_statement_visitor_t & v) const { v.visit(*this); }

void typed_while_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

/* typed_local_var_statement_t */

typed_local_var_statement_t::typed_local_var_statement_t(
    enum tchecker::statement_type_t type, std::shared_ptr<tchecker::typed_var_expression_t const> const & variable)
    : tchecker::typed_statement_t(type), tchecker::local_var_statement_t(variable)
{
}

typed_local_var_statement_t::typed_local_var_statement_t(
    enum tchecker::statement_type_t type, std::shared_ptr<tchecker::typed_var_expression_t const> const & variable,
    std::shared_ptr<tchecker::typed_expression_t const> const & init)
    : tchecker::typed_statement_t(type), tchecker::local_var_statement_t(variable, init)
{
}

typed_local_var_statement_t::~typed_local_var_statement_t() = default;

tchecker::typed_var_expression_t const & typed_local_var_statement_t::variable() const
{
  return dynamic_cast<tchecker::typed_var_expression_t const &>(tchecker::local_var_statement_t::variable());
}

std::shared_ptr<tchecker::typed_var_expression_t const> typed_local_var_statement_t::variable_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_var_expression_t const>(tchecker::local_var_statement_t::variable_ptr());
}

tchecker::typed_expression_t const & typed_local_var_statement_t::initial_value() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(tchecker::local_var_statement_t::initial_value());
}

std::shared_ptr<tchecker::typed_expression_t const> typed_local_var_statement_t::initial_value_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(tchecker::local_var_statement_t::initial_value_ptr());
}

tchecker::typed_local_var_statement_t * typed_local_var_statement_t::clone() const
{
  std::shared_ptr<tchecker::typed_var_expression_t const> variable_clone{variable().clone()};
  std::shared_ptr<tchecker::typed_expression_t const> initial_value_clone{initial_value().clone()};
  return new tchecker::typed_local_var_statement_t(_type, variable_clone, initial_value_clone);
}

void typed_local_var_statement_t::visit(tchecker::typed_statement_visitor_t & v) const { v.visit(*this); }

void typed_local_var_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

/* typed_local_array_statement_t */

typed_local_array_statement_t::typed_local_array_statement_t(
    enum tchecker::statement_type_t type, std::shared_ptr<tchecker::typed_var_expression_t const> const & variable,
    std::shared_ptr<tchecker::typed_expression_t const> const & size)
    : tchecker::typed_statement_t(type), tchecker::local_array_statement_t(variable, size)
{
}

typed_local_array_statement_t::~typed_local_array_statement_t() = default;

tchecker::typed_var_expression_t const & typed_local_array_statement_t::variable() const
{
  return dynamic_cast<tchecker::typed_var_expression_t const &>(tchecker::local_array_statement_t::variable());
}

std::shared_ptr<tchecker::typed_var_expression_t const> typed_local_array_statement_t::variable_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_var_expression_t const>(tchecker::local_array_statement_t::variable_ptr());
}

tchecker::typed_expression_t const & typed_local_array_statement_t::size() const
{
  return dynamic_cast<tchecker::typed_expression_t const &>(tchecker::local_array_statement_t::size());
}
std::shared_ptr<tchecker::typed_expression_t const> typed_local_array_statement_t::size_ptr() const
{
  return std::dynamic_pointer_cast<tchecker::typed_expression_t const>(tchecker::local_array_statement_t::size_ptr());
}

tchecker::typed_local_array_statement_t * typed_local_array_statement_t::clone() const
{
  std::shared_ptr<tchecker::typed_var_expression_t const> variable_clone{variable().clone()};
  std::shared_ptr<tchecker::typed_expression_t const> size_clone{size().clone()};
  return new tchecker::typed_local_array_statement_t(_type, variable_clone, size_clone);
}

void typed_local_array_statement_t::visit(tchecker::typed_statement_visitor_t & v) const { v.visit(*this); }

void typed_local_array_statement_t::visit(tchecker::statement_visitor_t & v) const { v.visit(*this); }

} // end of namespace tchecker
