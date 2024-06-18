/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/system/static_analysis.hh"
#include "tchecker/system/synchronization.hh"

namespace tchecker {

namespace system {

/* process_events_map_t */

process_events_map_t::process_events_map_t(tchecker::process_id_t proc_count) : _map(proc_count) {}

void process_events_map_t::insert(tchecker::process_id_t pid, tchecker::event_id_t event_id)
{
  assert(pid < _map.capacity());
  _map[pid].insert(event_id);
}

bool process_events_map_t::contains(tchecker::process_id_t pid, tchecker::event_id_t event_id)
{
  if (pid >= _map.size())
    return false;
  return (_map[pid].find(event_id) != _map[pid].end());
}

/* weakly synchronized events */

tchecker::system::process_events_map_t weakly_synchronized_events(tchecker::system::system_t const & system)
{
  tchecker::system::process_events_map_t weak_sync_map(system.processes_count());
  for (tchecker::system::synchronization_t const & sync : system.synchronizations()) {
    for (tchecker::system::sync_constraint_t const & sc : sync.synchronization_constraints())
      if (sc.strength() == tchecker::SYNC_WEAK)
        weak_sync_map.insert(sc.pid(), sc.event_id());
  }
  return weak_sync_map;
}

bool every_process_has_initial_location(tchecker::system::system_t const & system)
{
  for (tchecker::process_id_t const pid : system.processes_identifiers())
    if (system.initial_locations(pid).empty())
      return false;
  return true;
}

bool is_deterministic(tchecker::system::system_t const & system)
{
  // Check that each process has only one initial location
  for (tchecker::process_id_t const pid : system.processes_identifiers()) {
    auto initial_locations = system.initial_locations(pid);
    if (std::distance(initial_locations.begin(), initial_locations.end()) > 1)
      return false;
  }

  // Check that every location has at most one outgoing edge for each event
  boost::dynamic_bitset<> outevents(system.events_count(), 0);
  for (tchecker::system::loc_const_shared_ptr_t l : system.locations()) {
    outevents.reset();
    for (tchecker::system::edge_const_shared_ptr_t e : system.outgoing_edges(l->id())) {
      if (outevents[e->event_id()])
        return false;
      outevents[e->event_id()] = 1;
    }
  }
  return true;
}

} // end of namespace system

} // end of namespace tchecker
