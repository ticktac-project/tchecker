/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <ostream>
#include <set>
#include <string>

#include "syntax-check.hh"
#include "tchecker/basictypes.hh"
#include "tchecker/system/attribute.hh"
#include "tchecker/system/system.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/utils/log.hh"

namespace tchecker {

namespace tck_syntax {

/*!
 \brief Inspect a set of attributes and report all unknown attributes
 \param os : output stream
 \param attributes : set of attributes
 \param expected_attributes : known attributes
 \post warning messages have been emitted to os for every attribute in attributes that
 does not belong to expected_attributes
*/
static void inspect_attributes(std::ostream & os, tchecker::system::attributes_t const & attributes,
                               std::set<std::string> const & expected_attributes)
{
  for (auto && attr : attributes.range())
    if (expected_attributes.find(attr.key()) == expected_attributes.end())
      os << tchecker::log_warning << attr.parsing_position().key_position() << " unknown attribute " << attr.key() << std::endl;
}

/*!
 \brief Inspect a system and report all unknown attributes
 \param os : output stream
 \param system : a system of timed processes
 \param known_attr : map from attribute category to known attribute keys
 \post Warning messages have been emitted to os:
 - for each unknown attribute according to attr_key_map
 - and each process without an initial location
*/
static void inspect_attributes(std::ostream & os, tchecker::system::system_t const & system,
                               tchecker::system::attribute_keys_map_t const & known_attr)
{
  // System
  tchecker::tck_syntax::inspect_attributes(os, system.attributes(), known_attr[tchecker::system::ATTR_SYSTEM]);

  // Clocks
  tchecker::clock_id_t const clocks_count = system.clocks_count(tchecker::VK_DECLARED);
  for (tchecker::clock_id_t clock_id = 0; clock_id < clocks_count; ++clock_id)
    tchecker::tck_syntax::inspect_attributes(os, system.clock_attributes(clock_id), known_attr[tchecker::system::ATTR_CLOCK]);

  // Integer variables
  tchecker::intvar_id_t const intvars_count = system.intvars_count(tchecker::VK_DECLARED);
  for (tchecker::intvar_id_t intvar_id = 0; intvar_id < intvars_count; ++intvar_id)
    tchecker::tck_syntax::inspect_attributes(os, system.intvar_attributes(intvar_id),
                                             known_attr[tchecker::system::ATTR_INTVAR]);

  // Events
  tchecker::event_id_t const events_count = system.events_count();
  for (tchecker::event_id_t event_id = 0; event_id < events_count; ++event_id)
    tchecker::tck_syntax::inspect_attributes(os, system.event_attributes(event_id), known_attr[tchecker::system::ATTR_EVENT]);

  // Processes
  tchecker::process_id_t const process_count = system.processes_count();
  for (tchecker::process_id_t pid = 0; pid < process_count; ++pid) {
    tchecker::tck_syntax::inspect_attributes(os, system.process_attributes(pid), known_attr[tchecker::system::ATTR_PROCESS]);

    tchecker::range_t<tchecker::system::locs_t::const_iterator_t> initial_locations = system.initial_locations(pid);
    if (initial_locations.empty())
      os << tchecker::log_warning << "process " + system.process_name(pid) << " has no initial location" << std::endl;
  }

  // Locations
  tchecker::loc_id_t const locations_count = system.locations_count();
  for (tchecker::loc_id_t loc_id = 0; loc_id < locations_count; ++loc_id) {
    std::shared_ptr<tchecker::system::loc_t const> loc = system.location(loc_id);
    tchecker::tck_syntax::inspect_attributes(os, loc->attributes(), known_attr[tchecker::system::ATTR_LOCATION]);
  }

  // Edges
  tchecker::edge_id_t const edges_count = system.edges_count();
  for (tchecker::edge_id_t edge_id = 0; edge_id < edges_count; ++edge_id) {
    std::shared_ptr<tchecker::system::edge_t const> edge = system.edge(edge_id);
    tchecker::tck_syntax::inspect_attributes(os, edge->attributes(), known_attr[tchecker::system::ATTR_EDGE]);
  }

  // Synchronizations
  tchecker::sync_id_t const syncs_count = system.synchronizations_count();
  for (tchecker::sync_id_t sync_id = 0; sync_id < syncs_count; ++sync_id)
    tchecker::tck_syntax::inspect_attributes(os, system.synchronization(sync_id).attributes(),
                                             known_attr[tchecker::system::ATTR_SYNC]);
}

/*
  Check syntax
*/
bool syntax_check_ta(std::ostream & os, tchecker::parsing::system_declaration_t const & sysdecl)
{
  try {
    tchecker::ta::system_t system(sysdecl);
    inspect_attributes(os, system.as_system_system(), tchecker::ta::system_t::known_attributes());
  }
  catch (std::exception & e) {
    os << tchecker::log_error << e.what() << std::endl;
  }

  tchecker::log_output_count(os);
  if (tchecker::log_error_count() > 0)
    return false;

  return true;
}

} // namespace tck_syntax

} // namespace tchecker