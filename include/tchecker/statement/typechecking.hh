/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STATEMENT_TYPECHECKING_HH
#define TCHECKER_STATEMENT_TYPECHECKING_HH

#include <functional>
#include <memory>
#include <string>

#include "tchecker/statement/statement.hh"
#include "tchecker/statement/typed_statement.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"

/*!
 \file typechecking.hh
 \brief Type checking for statements
 */

namespace tchecker {

/*!
 \brief Type checking for statements
 \param stmt : statement
 \param localvars: local variables
 \param intvars : integer variables
 \param clocks : clock variables
 \param error : error logging function
 \return typed clone of stmt
 \post errors have been reported calling function error
 */
std::shared_ptr<tchecker::typed_statement_t> typecheck(
    tchecker::statement_t const & stmt, tchecker::integer_variables_t const & localvars,
    tchecker::integer_variables_t const & intvars, tchecker::clock_variables_t const & clocks,
    std::function<void(std::string const &)> error = [](std::string const &) {});

} // end of namespace tchecker

#endif // TCHECKER_STATEMENT_TYPECHECKING_HH
