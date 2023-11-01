/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/expression/static_analysis.hh"
#include "tchecker/statement/static_analysis.hh"
#include "tchecker/system/static_analysis.hh"
#include "tchecker/ta/static_analysis.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/variables/static_analysis.hh"

namespace tchecker {

namespace ta {

bool has_guarded_weakly_synchronized_event(tchecker::ta::system_t const & system)
{
  tchecker::system::process_events_map_t weakly_sync_map =
      tchecker::system::weakly_synchronized_events(system.as_system_system());

  for (tchecker::system::edge_const_shared_ptr_t const & edge : system.edges()) {
    if (weakly_sync_map.contains(edge->pid(), edge->event_id())) {
      try {
        tchecker::const_evaluate(system.guard(edge->id()));
      }
      catch (...) {
        return true;
      }
    }
  }
  return false;
}

bool has_diagonal_constraint(tchecker::ta::system_t const & system)
{
  // Check location invariants
  for (tchecker::system::loc_const_shared_ptr_t const & loc : system.locations()) {
    tchecker::has_clock_constraints_t invariant_has_clock_constraints =
        tchecker::has_clock_constraints(system.invariant(loc->id()));
    if (invariant_has_clock_constraints.diagonal)
      return true;
  }

  // Check edge guards
  for (tchecker::system::edge_const_shared_ptr_t const & edge : system.edges()) {
    tchecker::has_clock_constraints_t guard_has_clock_constrains = tchecker::has_clock_constraints(system.guard(edge->id()));
    if (guard_has_clock_constrains.diagonal)
      return true;
  }

  return false;
}

bool has_non_constant_reset(tchecker::ta::system_t const & system)
{
  // Check edge statements
  for (tchecker::system::edge_const_shared_ptr_t const & edge : system.edges()) {
    tchecker::has_clock_resets_t statement_has_clock_resets = tchecker::has_clock_resets(system.statement(edge->id()));
    if (statement_has_clock_resets.clock || statement_has_clock_resets.sum)
      return true;
  }
  return false;
}

bool has_shared_clocks(tchecker::ta::system_t const & system)
{
  tchecker::variable_access_map_t access_map = tchecker::variable_access(system);
  return access_map.has_shared_variable(tchecker::VTYPE_CLOCK);
}

} // end of namespace ta

} // end of namespace tchecker
