/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STATEMENT_TYPE_INFERENCE_HH
#define TCHECKER_STATEMENT_TYPE_INFERENCE_HH

#include "tchecker/expression/typed_expression.hh"
#include "tchecker/statement/typed_statement.hh"

/*!
 \file type_inference.hh
 \brief Type inference for statements
 */

namespace tchecker {
  
  /*!
   \brief Type inference
   \param ltype : type of left-value expression
   \param rtype : type of right-value expression
   \return Type of assign statement ltype = rtype
   */
  enum tchecker::statement_type_t type_assign(enum tchecker::expression_type_t ltype, enum tchecker::expression_type_t rtype);
  
  
  /*!
   \brief Type inference
   \param first : type of first statement
   \param second : type of second statement
   \return Type of sequence statement first ; second
   */
  enum tchecker::statement_type_t type_seq(enum tchecker::statement_type_t first, enum tchecker::statement_type_t second);
  
  /*!
   \brief Type inference
   \param cond : type of condition
   \param then_stmt : type of first statement
   \param else_stmt : type of second statement
   \return Type of statement if cond then then_stmt else else_stmt endif
   */
  enum tchecker::statement_type_t type_if(enum tchecker::expression_type_t cond,
                                          enum tchecker::statement_type_t then_stmt,
                                          enum tchecker::statement_type_t else_stmt);
  /*!
   \brief Type inference
   \param cond : type of condition
   \param stmt : type of iterated statement
   \return Type of while cond do stmt done
   */
  enum tchecker::statement_type_t type_while(enum tchecker::expression_type_t cond,
                                             enum tchecker::statement_type_t stmt);

  /*!
   \brief Type inference
   \param variable : type of the local variable expression
   \param init : type of the initial value expression
   \return Type of local var = init
   */
  enum tchecker::statement_type_t type_local(enum tchecker::expression_type_t variable,
                                             enum tchecker::expression_type_t init);

  /*!
   \brief Type inference
   \param variable : type of the local variable expression
   \param size : type of size expression
   \return Type of local var[size]
   */
  enum tchecker::statement_type_t type_local_array(enum tchecker::expression_type_t variable,
                                                   enum tchecker::expression_type_t size);

} // end of namespace tchecker

#endif // TCHECKER_STATEMENT_TYPE_INFERENCE_HH

