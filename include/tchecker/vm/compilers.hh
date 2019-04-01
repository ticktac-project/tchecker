/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VM_COMPILERS_HH
#define TCHECKER_VM_COMPILERS_HH

#include "tchecker/expression/typed_expression.hh"
#include "tchecker/statement/typed_statement.hh"
#include "tchecker/vm/vm.hh"


/*!
 \file compilers.hh
 \brief Compilers of expressions and statements to VM's bytecode
 */

namespace tchecker {
  
  /*!
   \brief Expression compiler
   \param expr : expression
   \return null-terminated bytecode for expr
   \throw std::invalid_argument : if expr has type tchecker::EXPR_TYPE_BAD
   \throw std::runtime_error : if expr cannot be compiled
   \note expr is compiler as a right-value expression
   \note the caller is responsible for deleting[] the returned value
   */
  tchecker::bytecode_t * compile(tchecker::typed_expression_t const & expr);
  
  
  /*!
   \brief Statement compiler
   \param stmt : statement
   \return null-terminated bytecode for stmt
   \throw std::invalid_argument : if stmt has type tchecker::STMT_TYPE_BAD
   \throw std::runtime_error : if stmt cannot be compiled
   \note the caller is responsible for deleting[] the returned value
   */
  tchecker::bytecode_t * compile(tchecker::typed_statement_t const & stmt);
  
  
} // end of namespace tchecker

#endif // TCHECKER_VM_COMPILERS_HH
