/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STATEMENT_STATIC_ANALYSIS_HH
#define TCHECKER_STATEMENT_STATIC_ANALYSIS_HH

#include <unordered_set>

#include "tchecker/basictypes.hh"
#include "tchecker/statement/clock_updates.hh"
#include "tchecker/statement/typed_statement.hh"

/*!
 \file static_analysis.hh
 \brief Static analysis of statements
 */

namespace tchecker {

/*!
 \brief Extract typed read variables IDs from a statement
 \param stmt : statement
 \param clocks : a set of clock IDs
 \param intvars : a set of integer variable
 \post for every occurrence of a variable x in the right-hand side of stmt, x has been added to clocks if x
 is a clock, and to intvars if x is an integer variable. For array expressions (i.e. x[e]) in the right-hand
 side of stmt such that offset expression e cannot be evaluated statically, all x[k] have been added to the
 set (according to the type of x) for all k in the domain of x
 */
void extract_read_variables(tchecker::typed_statement_t const & stmt, std::unordered_set<tchecker::clock_id_t> & clocks,
                            std::unordered_set<tchecker::intvar_id_t> & intvars);

/*!
 \brief Extract typed written variables IDs from a statement
 \param stmt : statement
 \param clocks : a set of clock IDs
 \param intvars : a set of integer variable
 \post for every occurrence of a variable x in the left-hand side of stmt, x has been added to clocks if x
 is a clock, and to intvars if x is an integer variable. For array expressions (i.e. x[e]) in the left-hand
 side of stmt such that offset expression e cannot be evaluated statically, all x[k] have been added to the
 set (according to the type of x) for all k in the domain of x
 */
void extract_written_variables(tchecker::typed_statement_t const & stmt, std::unordered_set<tchecker::clock_id_t> & clocks,
                               std::unordered_set<tchecker::intvar_id_t> & intvars);

/*!
 \brief Check if a statement declares local variables
 \param stmt : statement
 \post return true if in the scope of stmt some local variables are declared.
 It does not consider local declarations of inner scopes e.g. scopes in the
 'then' or 'else' branches of an If-Then-Else statement. Actually, only
 sequence statements are recursively visited.
 */
bool has_local_declarations(tchecker::typed_statement_t const & stmt);

/*!
 \brief Type of flags of contained clock resets
 */
struct has_clock_resets_t {
  bool constant; /*!< Has reset to constant, i.e. x := c */
  bool clock;    /*!< Has reset to clock, i.e. x := y */
  bool sum;      /*!< Has reset to sun, i.e. x := y + c */
};

/*!
 \brief Compute types of clock resets in a statement
 \param stmt : statement
 \return types of clock resets contained in stmt
 */
tchecker::has_clock_resets_t has_clock_resets(tchecker::typed_statement_t const & stmt);

/*!
 \brief Compute clock updates in a statement
 \param clock_nb : number of clocks
 \param stmt : statement
 \pre every clock identifier in stmt is less than clock_nb
 \return the clock updates map that associate to each clock in [0..clock_nb) the list of it potential updates
 after execution of stmt. The list is empty if updates cannot be determined
 \throw std::invalid_argument : if some clock identifier in stmt is bigger than or equal to clock_nb
 */
tchecker::clock_updates_map_t compute_clock_updates(std::size_t clock_nb, tchecker::typed_statement_t const & stmt);

} // end of namespace tchecker

#endif // TCHECKER_STATEMENT_STATIC_ANALYSIS_HH
