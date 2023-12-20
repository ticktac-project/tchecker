/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_EXPRESSION_TYPECHECKING_HH
#define TCHECKER_EXPRESSION_TYPECHECKING_HH

#include <functional>
#include <memory>
#include <string>

#include "tchecker/expression/expression.hh"
#include "tchecker/expression/typed_expression.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"

/*!
 \file typechecking.hh
 \brief Type checking for expressions
 */

namespace tchecker {

/*!
 \brief Type checking for expressions
 \param expr : expression
 \param localvars : local variables
 \param intvars : integer variables
 \param clocks : clock variables
 \param error : error logging function
 \return typed clone of expr
 \post all errors have been reported calling function error
 */
std::shared_ptr<tchecker::typed_expression_t> typecheck(
    tchecker::expression_t const & expr, tchecker::integer_variables_t const & localvars,
    tchecker::integer_variables_t const & intvars, tchecker::clock_variables_t const & clocks,
    std::function<void(std::string const &)> error = [](std::string const &) {});

} // end of namespace tchecker

#endif // TCHECKER_EXPRESSION_TYPECHECKING_HH
