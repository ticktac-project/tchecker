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
  
} // end of namespace tchecker

#endif // TCHECKER_STATEMENT_TYPE_INFERENCE_HH

