/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/system/output.hh"

namespace tchecker {

namespace system {

std::ostream & operator<<(std::ostream & os, tchecker::system::attributes_t const & attr)
{
  auto r = attr.attributes();
  if (r.begin() == r.end())
    return os;
  os << "{";
  auto begin = r.begin(), end = r.end();
  for (auto it = begin; it != end; ++it) {
    if (it != begin)
      os << " : ";
    os << (*it).first << ":" << (*it).second;
  }
  os << "}";
  return os;
}

std::ostream & operator<<(std::ostream & os, tchecker::system::system_t const & s)
{
  // System name and attributes
  os << "system:" << s.name() << s.attributes() << std::endl;

  // Events
  tchecker::event_id_t events_count = s.events_count();
  for (tchecker::event_id_t id = 0; id < events_count; ++id)
    os << "event:" << s.event_name(id) << s.event_attributes(id) << std::endl;

  // Bounded integer variables
  tchecker::integer_variables_t const & integer_variables = s.integer_variables();
  tchecker::intvar_id_t intvars_count = s.intvars_count(tchecker::VK_DECLARED);
  for (tchecker::intvar_id_t id = 0; id < intvars_count; ++id) {
    tchecker::intvar_info_t const & info = integer_variables.info(id);
    os << "int:" << info.size() << ":" << info.min() << ":" << info.max() << ":";
    os << info.initial_value() << ":";
    os << integer_variables.name(id) << s.intvar_attributes(id) << std::endl;
  }

  // Clocks
  tchecker::clock_variables_t const & clock_variables = s.clock_variables();
  tchecker::clock_id_t clocks_count = s.clocks_count(tchecker::VK_DECLARED);
  for (tchecker::clock_id_t id = 0; id < clocks_count; ++id) {
    os << "clock:" << clock_variables.info(id).size() << ":" << clock_variables.name(id);
    os << s.clock_attributes(id) << std::endl;
  }

  // Processes
  tchecker::process_id_t processes_count = s.processes_count();
  for (tchecker::process_id_t id = 0; id < processes_count; ++id)
    os << "process:" << s.process_name(id) << s.process_attributes(id) << std::endl;

  // Locations
  for (tchecker::system::loc_const_shared_ptr_t loc : s.locations())
    os << "location:" << s.process_name(loc->pid()) << ":" << loc->name() << loc->attributes() << std::endl;

  // Edges
  for (tchecker::system::edge_const_shared_ptr_t edge : s.edges()) {
    os << "edge:" << s.process_name(edge->pid()) << ":" << s.location(edge->src())->name() << ":";
    os << s.location(edge->tgt())->name() << ":" << s.event_name(edge->event_id());
    os << edge->attributes() << std::endl;
  }

  // Synchronizations
  for (tchecker::system::synchronization_t const & sync : s.synchronizations()) {
    os << "sync";
    for (tchecker::system::sync_constraint_t const & constr : sync.synchronization_constraints()) {
      os << ":" << s.process_name(constr.pid()) << "@" << s.event_name(constr.event_id());
      if (constr.strength() == tchecker::SYNC_WEAK)
        os << "?";
    }
    os << sync.attributes() << std::endl;
  }

  return os;
}

} // end of namespace system

} // end of namespace tchecker
