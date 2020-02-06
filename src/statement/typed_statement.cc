/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <tuple>

#include "tchecker/statement/typed_statement.hh"

namespace tchecker {
  
  /* enum statement_type_t */
  
  std::ostream & operator<< (std::ostream & os, enum tchecker::statement_type_t type)
  {
    switch (type) {
      case STMT_TYPE_BAD:           return os << "BAD";
      case STMT_TYPE_NOP:           return os << "NOP";
      case STMT_TYPE_INTASSIGN:     return os << "INTASSIGN";
      case STMT_TYPE_CLKASSIGN_INT: return os << "CLKASSIGN_INT";
      case STMT_TYPE_CLKASSIGN_CLK: return os << "CLKASSIGN_CLK";
      case STMT_TYPE_CLKASSIGN_SUM: return os << "CLKASSIGN_SUM";
      case STMT_TYPE_SEQ:           return os << "SEQ";
      case STMT_TYPE_IF:            return os << "IF";
      case STMT_TYPE_WHILE:         return os << "WHILE";
      case STMT_TYPE_LOCAL_INT:     return os << "LOCAL_INT";
      case STMT_TYPE_LOCAL_ARRAY:   return os << "LOCAL_ARRAY";
      default:                      throw std::runtime_error("incomplete switch statement");
    }
  }
  
  
  
  
  /* typed_statement_t */
  
  typed_statement_t::typed_statement_t(enum tchecker::statement_type_t type)
  : _type(type)
  {}
  
  
  void typed_statement_t::visit(tchecker::typed_statement_visitor_t & v) const
  {
    this->do_visit(v);
  }
  
  
  
  
  /* typed_nop_statement_t */
  
  tchecker::statement_t * typed_nop_statement_t::do_clone() const
  {
    return new typed_nop_statement_t(_type);
  }
  
  
  void typed_nop_statement_t::do_visit(tchecker::typed_statement_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  /* typed_assign_statement_t */
  
  typed_assign_statement_t::typed_assign_statement_t(enum tchecker::statement_type_t type,
                                                     tchecker::typed_lvalue_expression_t const * lvalue,
                                                     tchecker::typed_expression_t const * rvalue)
  : tchecker::make_typed_statement_t<tchecker::assign_statement_t>(type, lvalue, rvalue)
  {
    if (lvalue->size() != 1)
      throw std::invalid_argument("invalid lvalue expression");
  }
  
  
  tchecker::statement_t * typed_assign_statement_t::do_clone() const
  {
    auto * const lvalue_clone = dynamic_cast<tchecker::typed_lvalue_expression_t *>(_lvalue->clone());
    auto * const rvalue_clone = dynamic_cast<tchecker::typed_expression_t *>(_rvalue->clone());
    return new typed_assign_statement_t(_type, lvalue_clone, rvalue_clone);
  }
  
  
  void typed_assign_statement_t::do_visit(tchecker::typed_statement_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  /* typed_int_to_clock_assign_statement_t */
  
  typed_int_to_clock_assign_statement_t::typed_int_to_clock_assign_statement_t
  (enum tchecker::statement_type_t type,
   tchecker::typed_lvalue_expression_t const * lvalue,
   tchecker::typed_expression_t const * rvalue)
  : tchecker::typed_assign_statement_t(type, lvalue, rvalue)
  {}
  
  
  void typed_int_to_clock_assign_statement_t::do_visit(tchecker::typed_statement_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  /* typed_clock_to_clock_assign_statement_t */
  
  typed_clock_to_clock_assign_statement_t::typed_clock_to_clock_assign_statement_t
  (enum tchecker::statement_type_t type,
   tchecker::typed_lvalue_expression_t const * lvalue,
   tchecker::typed_lvalue_expression_t const * rvalue)
  : tchecker::typed_assign_statement_t(type, lvalue, rvalue)
  {}
  
  
  void typed_clock_to_clock_assign_statement_t::do_visit(tchecker::typed_statement_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  /* typed_sum_to_clock_assign_statement_t */
  
  typed_sum_to_clock_assign_statement_t::typed_sum_to_clock_assign_statement_t
  (enum tchecker::statement_type_t type,
   tchecker::typed_lvalue_expression_t const * lvalue,
   tchecker::typed_expression_t const * rvalue)
  : tchecker::typed_assign_statement_t(type, lvalue, rvalue)
  {
    if (rvalue->type() != tchecker::EXPR_TYPE_INTCLKSUM)
      throw std::invalid_argument("rvalue should have type tchecker::EXPR_TYPE_INTCLKSUM");
  }
  
  
  void typed_sum_to_clock_assign_statement_t::do_visit(tchecker::typed_statement_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  
  
  
  /* typed_sequence_statement_t */
  
  typed_sequence_statement_t::typed_sequence_statement_t(enum tchecker::statement_type_t type,
                                                         tchecker::typed_statement_t const * first,
                                                         tchecker::typed_statement_t const * second)
  : tchecker::make_typed_statement_t<tchecker::sequence_statement_t>(type, first, second)
  {}
  
  
  tchecker::statement_t * typed_sequence_statement_t::do_clone() const
  {
    tchecker::typed_statement_t * const first_clone = dynamic_cast<tchecker::typed_statement_t *>(_first->clone());
    tchecker::typed_statement_t * const second_clone = dynamic_cast<tchecker::typed_statement_t *>(_second->clone());
    return new typed_sequence_statement_t(_type, first_clone, second_clone);
  }
  
  
  void typed_sequence_statement_t::do_visit(tchecker::typed_statement_visitor_t & v) const
  {
    v.visit(*this);
  }
  
  /* typed_if_statement_t */

  typed_if_statement_t::typed_if_statement_t(enum tchecker::statement_type_t type,
                                             tchecker::typed_expression_t const * cond,
                                             tchecker::typed_statement_t const * then_stmt,
                                             tchecker::typed_statement_t const * else_stmt)
  : tchecker::make_typed_statement_t<tchecker::if_statement_t>(type, cond, then_stmt, else_stmt)
  {}


  tchecker::statement_t * typed_if_statement_t::do_clone() const
  {
    tchecker::typed_expression_t * const cond_clone =
        dynamic_cast<tchecker::typed_expression_t *>(_condition->clone());
    tchecker::typed_statement_t * const then_clone =
        dynamic_cast<tchecker::typed_statement_t *>(_then_stmt->clone());
    tchecker::typed_statement_t * const else_clone =
        dynamic_cast<tchecker::typed_statement_t *>(_else_stmt->clone());
    return new typed_if_statement_t(_type, cond_clone, then_clone, else_clone);
  }


  void typed_if_statement_t::do_visit(tchecker::typed_statement_visitor_t & v) const
  {
    v.visit(*this);
  }

  /* typed_while_statement_t */

  typed_while_statement_t::typed_while_statement_t(enum tchecker::statement_type_t type,
                                                   tchecker::typed_expression_t const * cond,
                                                   tchecker::typed_statement_t const * stmt)
  : tchecker::make_typed_statement_t<tchecker::while_statement_t>(type, cond, stmt)
  {}


  tchecker::statement_t * typed_while_statement_t::do_clone() const
  {
    tchecker::typed_expression_t * const cond_clone =
        dynamic_cast<tchecker::typed_expression_t *>(_condition->clone());
    tchecker::typed_statement_t * const stmt_clone =
        dynamic_cast<tchecker::typed_statement_t *>(_stmt->clone());
    return new typed_while_statement_t(_type, cond_clone, stmt_clone);
  }


  void typed_while_statement_t::do_visit(tchecker::typed_statement_visitor_t & v) const
  {
    v.visit(*this);
  }

  /* typed_local_var_statement_t */

  typed_local_var_statement_t::typed_local_var_statement_t(enum tchecker::statement_type_t type,
                                                           tchecker::typed_var_expression_t const *variable)
  : tchecker::make_typed_statement_t<tchecker::local_var_statement_t>(type, variable)
  {}

  typed_local_var_statement_t::typed_local_var_statement_t(enum tchecker::statement_type_t type,
                                                           tchecker::typed_var_expression_t const *variable,
                                                           tchecker::typed_expression_t const *init)
  : tchecker::make_typed_statement_t<tchecker::local_var_statement_t>(type, variable, init)
  {}


  tchecker::statement_t * typed_local_var_statement_t::do_clone() const
  {
    auto var = dynamic_cast<typed_var_expression_t const *>(_variable->clone());
    auto init = dynamic_cast<typed_expression_t const *>(_initial_value->clone());

    return new typed_local_var_statement_t(_type, var, init);
  }


  void typed_local_var_statement_t::do_visit(tchecker::typed_statement_visitor_t & v) const
  {
    v.visit(*this);
  }

  /* typed_local_array_statement_t */

  typed_local_array_statement_t::typed_local_array_statement_t(enum tchecker::statement_type_t type,
                                                               tchecker::typed_var_expression_t const *variable,
                                                               tchecker::typed_expression_t const *size)

  : tchecker::make_typed_statement_t<tchecker::local_array_statement_t>(type, variable, size)
  {}


  tchecker::statement_t * typed_local_array_statement_t::do_clone() const
  {
    tchecker::typed_var_expression_t * const variable =
        dynamic_cast<tchecker::typed_var_expression_t *>(_variable->clone());
    tchecker::typed_expression_t * const size =
        dynamic_cast<tchecker::typed_expression_t *>(_size->clone());

    return new typed_local_array_statement_t(_type, variable, size);
  }


  void typed_local_array_statement_t::do_visit(tchecker::typed_statement_visitor_t & v) const
  {
    v.visit(*this);
  }

} // end of namespace tchecker

