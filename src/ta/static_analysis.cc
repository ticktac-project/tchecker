/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/expression/static_analysis.hh"
#include "tchecker/system/static_analysis.hh"
#include "tchecker/ta/static_analysis.hh"
#include "tchecker/ta/system.hh"

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

} // end of namespace ta

} // end of namespace tchecker
