/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <unordered_set>

#include "tchecker/basictypes.hh"
#include "tchecker/expression/static_analysis.hh"
#include "tchecker/expression/typed_expression.hh"
#include "tchecker/statement/static_analysis.hh"
#include "tchecker/statement/typed_statement.hh"
#include "tchecker/variables/access.hh"
#include "tchecker/variables/static_analysis.hh"

namespace tchecker {

/*!
 \brief Add accesses to variables in expression
 \param expr : an expression
 \param pid : a process ID
 \param map : a variable access map
 \post All accesses to variables in `expr` have been added as READ accesses by process `pid` to `map`
 */
static void add_accesses(tchecker::typed_expression_t const & expr, tchecker::process_id_t pid,
                         tchecker::variable_access_map_t & map)
{
  std::unordered_set<tchecker::clock_id_t> clock_ids;
  std::unordered_set<tchecker::intvar_id_t> intvar_ids;

  tchecker::extract_variables(expr, clock_ids, intvar_ids);

  for (tchecker::clock_id_t clock_id : clock_ids)
    map.add(clock_id, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ, pid);
  for (tchecker::intvar_id_t intvar_id : intvar_ids)
    map.add(intvar_id, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ, pid);
}

/*!
 \brief Add accesses to variables in a statement
 \param stmt : a statement
 \param pid : a process ID
 \param map : a variable access map
 \post All accesses to variables in `stmt` have been added as READ (if right-hand side) or WRITE (if left-hand side) accesses by
 process `pid` to `map`
 */
static void add_accesses(tchecker::typed_statement_t const & stmt, tchecker::process_id_t pid,
                         tchecker::variable_access_map_t & map)
{
  std::unordered_set<tchecker::clock_id_t> clock_ids;
  std::unordered_set<tchecker::intvar_id_t> intvar_ids;

  tchecker::extract_read_variables(stmt, clock_ids, intvar_ids);

  for (tchecker::clock_id_t clock_id : clock_ids)
    map.add(clock_id, tchecker::VTYPE_CLOCK, tchecker::VACCESS_READ, pid);
  for (tchecker::intvar_id_t intvar_id : intvar_ids)
    map.add(intvar_id, tchecker::VTYPE_INTVAR, tchecker::VACCESS_READ, pid);

  clock_ids.clear();
  intvar_ids.clear();

  tchecker::extract_written_variables(stmt, clock_ids, intvar_ids);

  for (tchecker::clock_id_t clock_id : clock_ids)
    map.add(clock_id, tchecker::VTYPE_CLOCK, tchecker::VACCESS_WRITE, pid);
  for (tchecker::intvar_id_t intvar_id : intvar_ids)
    map.add(intvar_id, tchecker::VTYPE_INTVAR, tchecker::VACCESS_WRITE, pid);
}

tchecker::variable_access_map_t variable_access(tchecker::ta::system_t const & system)
{
  tchecker::variable_access_map_t map;

  for (tchecker::system::edge_const_shared_ptr_t edge : system.edges()) {
    tchecker::add_accesses(system.guard(edge->id()), edge->pid(), map);
    tchecker::add_accesses(system.statement(edge->id()), edge->pid(), map);
  }

  for (tchecker::system::loc_const_shared_ptr_t loc : system.locations())
    tchecker::add_accesses(system.invariant(loc->id()), loc->pid(), map);

  return map;
}

} // end of namespace tchecker
