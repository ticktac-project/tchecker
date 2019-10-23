/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VARIABLES_STATIC_ANALYSIS_HH
#define TCHECKER_VARIABLES_STATIC_ANALYSIS_HH

#include "tchecker/variables/access.hh"

/*!
 \file static_analysis.hh
 \brief Static analysis for variables
 */

namespace tchecker {

namespace details {

/*!
 \brief Add accesses to variables in expression
 \param expr : an expression
 \param pid : a process ID
 \param map : a variable access map
 \post All accesses to variables in `expr` have been added as READ accesses by process `pid` to `map`
 */
void add_accesses(tchecker::typed_expression_t const & expr, tchecker::process_id_t pid, tchecker::variable_access_map_t & map);


/*!
 \brief Add accesses to variables in a statement
 \param stmt : a statement
 \param pid : a process ID
 \param map : a variable access map
 \post All accesses to variables in `stmt` have been added as READ (if right-hand side) or WRITE (if left-hand side) accesses by process `pid` to `map`
 */
void add_accesses(tchecker::typed_statement_t const & stmt, tchecker::process_id_t pid, tchecker::variable_access_map_t & map);

} // end of namespace details


/*!
 \brief Compute variable access map from a model
 \tparam MODEL : type of model, should inherit from tchecker::fsm::details::model_t
 \param model : a model
 \return the map of variable accesses from model
 */
template <class MODEL>
tchecker::variable_access_map_t variable_access(MODEL const & model)
{
  tchecker::variable_access_map_t map;
  
  for (auto const * edge : model.system().edges()) {
    tchecker::details::add_accesses(model.typed_guard(edge->id()), edge->pid(), map);
    tchecker::details::add_accesses(model.typed_statement(edge->id()), edge->pid(), map);
  }
  
  for (auto const * loc : model.system().locations())
    tchecker::details::add_accesses(model.typed_invariant(loc->id()), loc->pid(), map);
  
  return map;
}
  
} // end of namespace tchecker

#endif // TCHECKER_VARIABLES_STATIC_ANALYSIS_HH
