/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_EXPRESSION_TYPE_INFERENCE_HH
#define TCHECKER_EXPRESSION_TYPE_INFERENCE_HH

#include "tchecker/expression/typed_expression.hh"

/*!
 \file type_inference.hh
 \brief Type inference for expressions
 */

namespace tchecker {
  
  /*!
   \brief Type inference
   \param type : type to check
   \return true if type dereferences to an integer value, false otherwise
   */
  bool integer_dereference(enum tchecker::expression_type_t type);
  
  
  /*!
   \brief Type inference
   \param type : type to check
   \return true if type represents an integer value, false otherwise
   */
  bool integer_valued(enum tchecker::expression_type_t type);
  
  
  /*!
   \brief Type inference
   \param type : type to check
   \return true if type represents an assignable integer variable, false
   otherwise
   */
  bool integer_assignable(enum tchecker::expression_type_t type);
  
  
  /*!
   \brief Type inference
   \param type : type to check
   \return true if type dereferences to a clock value, false otherwise
   */
  bool clock_dereference(enum tchecker::expression_type_t type);
  
  
  /*!
   \brief Type inference
   \param type : type to check
   \return true if type represents a clock value, false otherwise
   */
  bool clock_valued(enum tchecker::expression_type_t type);
  
  
  /*!
   \brief Type inference
   \param type : type to check
   \return true if type represents an assignable clock variable, false
   otherwise
   */
  bool clock_assignable(enum tchecker::expression_type_t type);
  
  
  /*!
   \brief Type inference
   \param type : type to check
   \return true if type represents a boolean value, false otherwise
   */
  bool bool_valued(enum tchecker::expression_type_t type);
  
  
  
  
  /*!
   \brief Type inference
   \param type : expression type
   \return type of expression (expr)
   */
  enum tchecker::expression_type_t type_par(enum tchecker::expression_type_t type);
  
  
  /*!
   \brief Type inference
   \param left : expression type
   \param right : expression type
   \return type of expression left && right
   */
  enum tchecker::expression_type_t type_land(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right);
  
  
  /*!
   \brief Type inference
   \param type : expression type
   \param right : expression type
   \return type of expression !expr
   */
  enum tchecker::expression_type_t type_lnot(enum tchecker::expression_type_t type);
  
  
  /*!
   \brief Type inference
   \param left : expression type
   \param right : expression type
   \return type of expression left < right
   */
  enum tchecker::expression_type_t type_lt(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right);
  
  
  /*!
   \brief Type inference
   \param left : expression type
   \param right : expression type
   \return type of expression left <= right
   */
  enum tchecker::expression_type_t type_le(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right);
  
  
  /*!
   \brief Type inference
   \param left : expression type
   \param right : expression type
   \return type of expression left >= right
   */
  enum tchecker::expression_type_t type_ge(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right);
  
  
  /*!
   \brief Type inference
   \param left : expression type
   \param right : expression type
   \return type of expression left > right
   */
  enum tchecker::expression_type_t type_gt(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right);
  
  
  /*!
   \brief Type inference
   \param left : expression type
   \param right : expression type
   \return type of expression left == right
   */
  enum tchecker::expression_type_t type_eq(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right);
  
  
  /*!
   \brief Type inference
   \param left : expression type
   \param right : expression type
   \return type of expression left != right
   */
  enum tchecker::expression_type_t type_neq(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right);
  
  
  /*!
   \brief Type inference
   \param left : expression type
   \param right : expression type
   \return type of expression left + right
   */
  enum tchecker::expression_type_t type_plus(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right);
  
  
  /*!
   \brief Type inference
   \param left : expression type
   \param right : expression type
   \return type of expression left - right
   */
  enum tchecker::expression_type_t type_minus(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right);
  
  
  /*!
   \brief Type inference
   \param left : expression type
   \param right : expression type
   \return type of expression left * right
   */
  enum tchecker::expression_type_t type_times(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right);
  
  
  /*!
   \brief Type inference
   \param left : expression type
   \param right : expression type
   \return type of expression left / right
   */
  enum tchecker::expression_type_t type_div(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right);
  
  
  /*!
   \brief Type inference
   \param left : expression type
   \param right : expression type
   \return type of expression left % right
   */
  enum tchecker::expression_type_t type_mod(enum tchecker::expression_type_t left, enum tchecker::expression_type_t right);
  
  
  /*!
   \brief Type inference
   \param type : expression type
   \return type of expression -type
   */
  enum tchecker::expression_type_t type_neg(enum tchecker::expression_type_t type);
  
  
  /*!
   \brief Type inference
   \param op : binary operator
   \param left : expression type
   \param right : expression type
   \return type of expression left op right
   */
  enum tchecker::expression_type_t type_binary(enum tchecker::binary_operator_t op,
                                               enum tchecker::expression_type_t left,
                                               enum tchecker::expression_type_t right);
  
  
  /*!
   \brief Type inference
   \param op : unary operator
   \param expr_type : expression type
   \return type of expression op expr_type
   */
  enum tchecker::expression_type_t type_unary(enum tchecker::unary_operator_t op, enum tchecker::expression_type_t expr_type);

  /*!
   \brief Type inference
   \param cond : expression type
   \param then_value : expression type
   \param else_value : expression type
   \return type of expression if cond then then_value else else_value
   */
  enum tchecker::expression_type_t type_ite(enum tchecker::expression_type_t cond,
                                            enum tchecker::expression_type_t then_value,
                                            enum tchecker::expression_type_t else_value);

} // end of namespace tchecker

#endif // TCHECKER_EXPRESSION_TYPE_INFERENCE_HH
