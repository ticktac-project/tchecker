/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYNTAX_CHECK_HH
#define TCHECKER_SYNTAX_CHECK_HH

#include <ostream>

#include "tchecker/parsing/declaration.hh"

namespace tchecker {

namespace tck_syntax {

/*!
 \brief Check timed automaton syntax from a system declaration
 \param os : output stream
 \param sysdecl : system declaration
 \return true if sysdecl contains a syntactically correct declaration of a
 system of timed automata, false otherwise
 \post syntax errors have been reported to os
 warning messages have been issued to os:
 - for attributes which have not been interpreted
 - for processes which have no initial location
*/
bool syntax_check_ta(std::ostream & os, tchecker::parsing::system_declaration_t const & sysdecl);

} // namespace tck_syntax

} // namespace tchecker

#endif // TCHECKER_SYNTAX_CHECK_HH