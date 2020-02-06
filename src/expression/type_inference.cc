/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/expression/type_inference.hh"
#include "tchecker/expression/typed_expression.hh"

namespace tchecker {
  
  bool integer_dereference(enum tchecker::expression_type_t type)
  {
    return ((type == tchecker::EXPR_TYPE_INTVAR) || (type == tchecker::EXPR_TYPE_INTARRAY) ||
            (type == tchecker::EXPR_TYPE_LOCALINTVAR) || (type == tchecker::EXPR_TYPE_LOCALINTARRAY));
  }
  
  
  bool integer_valued(enum tchecker::expression_type_t type)
  {
    return ((type == tchecker::EXPR_TYPE_INTTERM) ||
            (type == tchecker::EXPR_TYPE_INTVAR) ||
            (type == tchecker::EXPR_TYPE_INTLVALUE) ||
            (type == tchecker::EXPR_TYPE_LOCALINTVAR) ||
            (type == tchecker::EXPR_TYPE_LOCALINTLVALUE)
            );
  }
  
  
  bool integer_assignable(enum tchecker::expression_type_t type)
  {
    return ((type == tchecker::EXPR_TYPE_INTVAR) || (type == tchecker::EXPR_TYPE_INTLVALUE) ||
            (type == tchecker::EXPR_TYPE_LOCALINTVAR) || (type == tchecker::EXPR_TYPE_LOCALINTLVALUE));

  }
  
  
  bool clock_dereference(enum tchecker::expression_type_t type)
  {
    return ((type == tchecker::EXPR_TYPE_CLKVAR) || (type == tchecker::EXPR_TYPE_CLKARRAY));
  }
  
  
  bool clock_valued(enum tchecker::expression_type_t type)
  {
    return ((type == tchecker::EXPR_TYPE_CLKVAR) || (type == tchecker::EXPR_TYPE_CLKLVALUE));
  }
  
  
  bool clock_assignable(enum tchecker::expression_type_t type)
  {
    return ((type == tchecker::EXPR_TYPE_CLKVAR) || (type == tchecker::EXPR_TYPE_CLKLVALUE));
  }
  
  
  bool bool_valued(enum tchecker::expression_type_t type)
  {
    return (integer_valued(type) ||
            (type == tchecker::EXPR_TYPE_CLKCONSTR_SIMPLE) ||
            (type == tchecker::EXPR_TYPE_CLKCONSTR_DIAGONAL) ||
            (type == tchecker::EXPR_TYPE_ATOMIC_PREDICATE) ||
            (type == tchecker::EXPR_TYPE_CONJUNCTIVE_FORMULA));
  }
  
  
  enum tchecker::expression_type_t type_par(enum tchecker::expression_type_t type)
  {
    return type;
  }
  
  
  enum tchecker::expression_type_t type_land(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right)
  {
    if (bool_valued(left) && bool_valued(right))
      return tchecker::EXPR_TYPE_CONJUNCTIVE_FORMULA;
    
    return tchecker::EXPR_TYPE_BAD;
  }
  
  
  enum tchecker::expression_type_t type_lnot(enum tchecker::expression_type_t type)
  {
    if (integer_valued(type) || (type == tchecker::EXPR_TYPE_ATOMIC_PREDICATE))
      return tchecker::EXPR_TYPE_ATOMIC_PREDICATE;
    
    return tchecker::EXPR_TYPE_BAD;
  }
  
  
  enum tchecker::expression_type_t type_binary_predicate(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right)
  {
    if (integer_valued(left) && integer_valued(right))
      return tchecker::EXPR_TYPE_ATOMIC_PREDICATE;
    if (clock_valued(left) && integer_valued(right))
      return tchecker::EXPR_TYPE_CLKCONSTR_SIMPLE;
    if ((left == tchecker::EXPR_TYPE_CLKDIFF) && integer_valued(right))
      return tchecker::EXPR_TYPE_CLKCONSTR_DIAGONAL;
    
    return tchecker::EXPR_TYPE_BAD;
  }
  
  
  enum tchecker::expression_type_t type_lt(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right)
  {
    return type_binary_predicate(left, right);
  }
  
  
  enum tchecker::expression_type_t type_le(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right)
  {
    return type_binary_predicate(left, right);
  }
  
  
  enum tchecker::expression_type_t type_ge(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right)
  {
    return type_binary_predicate(left, right);
  }
  
  
  enum tchecker::expression_type_t type_gt(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right)
  {
    return type_binary_predicate(left, right);
  }
  
  
  enum tchecker::expression_type_t type_eq(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right)
  {
    return type_binary_predicate(left, right);
  }
  
  
  enum tchecker::expression_type_t type_neq(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right)
  {
    if (integer_valued(left) && integer_valued(right))
      return tchecker::EXPR_TYPE_ATOMIC_PREDICATE;
    
    return tchecker::EXPR_TYPE_BAD;
  }
  
  
  enum tchecker::expression_type_t type_plus(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right)
  {
    if (integer_valued(left) && integer_valued(right))
      return tchecker::EXPR_TYPE_INTTERM;
    if (integer_valued(left) && clock_valued(right))
      return tchecker::EXPR_TYPE_INTCLKSUM;
    
    return tchecker::EXPR_TYPE_BAD;
  }
  
  
  enum tchecker::expression_type_t type_minus(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right)
  {
    if (integer_valued(left) && integer_valued(right))
      return tchecker::EXPR_TYPE_INTTERM;
    if (clock_valued(left) && clock_valued(right))
      return tchecker::EXPR_TYPE_CLKDIFF;
    
    return tchecker::EXPR_TYPE_BAD;
  }
  
  
  enum tchecker::expression_type_t type_binary_operator(enum tchecker::expression_type_t left,
                                                        enum tchecker::expression_type_t right)
  {
    if (integer_valued(left) && integer_valued(right))
      return tchecker::EXPR_TYPE_INTTERM;
    
    return tchecker::EXPR_TYPE_BAD;
  }
  
  
  enum tchecker::expression_type_t type_times(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right)
  {
    return type_binary_operator(left, right);
  }
  
  
  enum tchecker::expression_type_t type_div(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right)
  {
    return type_binary_operator(left, right);
  }
  
  
  enum tchecker::expression_type_t type_mod(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right)
  {
    return type_binary_operator(left, right);
  }
  
  
  enum tchecker::expression_type_t type_neg(enum tchecker::expression_type_t type)
  {
    if (integer_valued(type))
      return tchecker::EXPR_TYPE_INTTERM;
    
    return tchecker::EXPR_TYPE_BAD;
  }
  
  
  enum tchecker::expression_type_t type_binary(enum tchecker::binary_operator_t op,
                                               enum tchecker::expression_type_t left,
                                               enum tchecker::expression_type_t right)
  {
    switch(op) {
      case tchecker::EXPR_OP_LAND:   return type_land(left, right);
      case tchecker::EXPR_OP_LT:     return type_lt(left, right);
      case tchecker::EXPR_OP_LE:     return type_le(left, right);
      case tchecker::EXPR_OP_EQ:     return type_eq(left, right);
      case tchecker::EXPR_OP_NEQ:    return type_neq(left, right);
      case tchecker::EXPR_OP_GE:     return type_ge(left, right);
      case tchecker::EXPR_OP_GT:     return type_gt(left, right);
      case tchecker::EXPR_OP_MINUS:  return type_minus(left, right);
      case tchecker::EXPR_OP_PLUS:   return type_plus(left, right);
      case tchecker::EXPR_OP_TIMES:  return type_times(left, right);
      case tchecker::EXPR_OP_DIV:    return type_div(left, right);
      case tchecker::EXPR_OP_MOD:    return type_mod(left, right);
      default:                       throw std::runtime_error("incomplete switch statement");
    }
  }
  
  
  enum tchecker::expression_type_t type_unary(enum tchecker::unary_operator_t op, enum tchecker::expression_type_t expr_type)
  {
    switch(op) {
      case tchecker::EXPR_OP_NEG:    return type_neg(expr_type);
      case tchecker::EXPR_OP_LNOT:   return type_lnot(expr_type);
      default:                       throw std::runtime_error("incomplete switch statement");
    }
  }

  enum tchecker::expression_type_t type_ite(enum tchecker::expression_type_t cond,
                                            enum tchecker::expression_type_t then_value,
                                            enum tchecker::expression_type_t else_value)
  {
    if (! bool_valued (cond))
      return tchecker::EXPR_TYPE_BAD;

    if (integer_valued(then_value) && integer_valued(else_value))
      return tchecker::EXPR_TYPE_INTTERM;

    return tchecker::EXPR_TYPE_BAD;

  }

} // end of namespace tchecker
