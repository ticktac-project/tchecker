/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_HH
#define TCHECKER_SYSTEM_HH

#include <limits>
#include <string>

#include "tchecker/basictypes.hh"
#include "tchecker/parsing/declaration.hh"
#include "tchecker/system/attribute.hh"
#include "tchecker/system/clock.hh"
#include "tchecker/system/edge.hh"
#include "tchecker/system/event.hh"
#include "tchecker/system/intvar.hh"
#include "tchecker/system/loc.hh"
#include "tchecker/system/process.hh"
#include "tchecker/system/synchronization.hh"

/*!
 \file system.hh
 \brief System of processes
 */

namespace tchecker {

namespace system {

/*!
 \class system_t
 \brief System of processes
 */
class system_t : private tchecker::system::clocks_t,
                 private tchecker::system::edges_t,
                 private tchecker::system::events_t,
                 private tchecker::system::intvars_t,
                 private tchecker::system::locs_t,
                 private tchecker::system::processes_t,
                 private tchecker::system::synchronizations_t {
public:
  /*!
   \brief Constructor
   \param name : system name
   \param attributes : system attributes
   */
  system_t(std::string const & name, tchecker::system::attributes_t const & attributes = tchecker::system::attributes_t());

  /*!
   \brief Constructor
   \param sysdecl : System declaration
   */
  system_t(tchecker::parsing::system_declaration_t const & sysdecl);

  /*!
   \brief Copy constructor
   */
  system_t(tchecker::system::system_t const &) = default;

  /*!
   \brief Move constructor
   */
  system_t(tchecker::system::system_t &&) = default;

  /*!
   \brief Destructor
   */
  ~system_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::system::system_t & operator=(tchecker::system::system_t const &) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::system::system_t & operator=(tchecker::system::system_t &&) = default;

  // System

  /*!
   \brief Accessor
   \return system name
   */
  inline std::string const & name() const { return _name; }

  /*!
   \brief Accessor
   \return system attributes
   */
  inline tchecker::system::attributes_t const & attributes() const { return _attributes; }

  /*!
   \brief Accessor
   \return map of known attributes that are interpreted by this system
  */
  static tchecker::system::attribute_keys_map_t const & known_attributes();

  // Clocks

  /*!
   \brief Add a clock (see tchecker::system::clocks_t::add_clock)
   \throw std::invalid_argument : if there is another declared variable name (of any type)
   */
  void add_clock(std::string const & name, tchecker::clock_id_t size = 1,
                 tchecker::system::attributes_t const & attributes = tchecker::system::attributes_t());

  using tchecker::system::clocks_t::clock_attributes;
  using tchecker::system::clocks_t::clock_id;
  using tchecker::system::clocks_t::clock_name;
  using tchecker::system::clocks_t::clock_variables;
  using tchecker::system::clocks_t::clocks_count;
  using tchecker::system::clocks_t::clocks_identifiers;
  using tchecker::system::clocks_t::is_clock;

  // Edges

  /*!
   \brief Add an edge (see tchecker::system::edges_t::add_edge)
   \throw std::invalid_argument : if pid, src, tgt or event_id do not correspond to a declared process/location/event
   */
  void add_edge(tchecker::process_id_t pid, tchecker::loc_id_t src, tchecker::loc_id_t tgt, tchecker::event_id_t event_id,
                tchecker::system::attributes_t const & attributes = tchecker::system::attributes_t());

  using tchecker::system::edges_t::edge;
  using tchecker::system::edges_t::edges;
  using tchecker::system::edges_t::edges_count;
  using tchecker::system::edges_t::edges_identifiers;
  using tchecker::system::edges_t::incoming_edges;
  using tchecker::system::edges_t::incoming_edges_maps;
  using tchecker::system::edges_t::incoming_event;
  using tchecker::system::edges_t::is_edge;
  using tchecker::system::edges_t::outgoing_edges;
  using tchecker::system::edges_t::outgoing_edges_maps;
  using tchecker::system::edges_t::outgoing_event;

  // Events
  using tchecker::system::events_t::add_event;
  using tchecker::system::events_t::event_attributes;
  using tchecker::system::events_t::event_id;
  using tchecker::system::events_t::event_name;
  using tchecker::system::events_t::events_count;
  using tchecker::system::events_t::events_identifiers;
  using tchecker::system::events_t::is_event;

  // Bounded integer variables

  /*!
   \brief Add a bounded integer variable (see tchecker::system::intvars_t::add_integer_variable)
   \throw std::invalid_argument : if there is another declared variable name (of any type)
   */
  void add_intvar(std::string const & name, tchecker::intvar_id_t size = 1,
                  tchecker::integer_t min = std::numeric_limits<tchecker::integer_t>::min(),
                  tchecker::integer_t max = std::numeric_limits<tchecker::integer_t>::max(),
                  tchecker::integer_t initial = std::numeric_limits<tchecker::integer_t>::min(),
                  tchecker::system::attributes_t const & attributes = tchecker::system::attributes_t());

  using tchecker::system::intvars_t::integer_variables;
  using tchecker::system::intvars_t::intvar_attributes;
  using tchecker::system::intvars_t::intvar_id;
  using tchecker::system::intvars_t::intvar_name;
  using tchecker::system::intvars_t::intvars_count;
  using tchecker::system::intvars_t::intvars_identifiers;
  using tchecker::system::intvars_t::is_intvar;

  // Locations

  /*!
   \brief Add a location (see tchecker::system::locs_t::add_location)
   \throw std::invalid_argument : if pid is not a declared process, of if name is already a declared location of process pid
   \note if attribute `initial` is in attributes, then the location is added as an initial location of process pid
   */
  void add_location(tchecker::process_id_t pid, std::string const & name,
                    tchecker::system::attributes_t const & attributes = tchecker::system::attributes_t());

  using tchecker::system::locs_t::initial_locations;
  using tchecker::system::locs_t::is_initial_location;
  using tchecker::system::locs_t::is_location;
  using tchecker::system::locs_t::location;
  using tchecker::system::locs_t::locations;
  using tchecker::system::locs_t::locations_count;
  using tchecker::system::locs_t::locations_identifiers;

  // Processes
  using tchecker::system::processes_t::add_process;
  using tchecker::system::processes_t::is_process;
  using tchecker::system::processes_t::process_attributes;
  using tchecker::system::processes_t::process_id;
  using tchecker::system::processes_t::process_name;
  using tchecker::system::processes_t::processes_count;
  using tchecker::system::processes_t::processes_identifiers;

  // Synchronizations

  /*!
   \brief Add a synchronization (see tchecker::system::synchronizations_t::add_synchronization)
   \throw std::invalid_argument : id a process or an event in v is not declared
   */
  void add_synchronization(std::vector<tchecker::system::sync_constraint_t> const & v,
                           tchecker::system::attributes_t const & attributes = tchecker::system::attributes_t());

  using tchecker::system::synchronizations_t::synchronization;
  using tchecker::system::synchronizations_t::synchronizations;
  using tchecker::system::synchronizations_t::synchronizations_count;
  using tchecker::system::synchronizations_t::synchronizations_identifiers;

private:
  /*!
   \brief Check existence of variable
   \param name : variable name
   \return true if name is a declared variable (of any type), false otherwise
   */
  bool has_variable(std::string const & name);

  std::string _name;                          /*!< System name */
  tchecker::system::attributes_t _attributes; /*!< System attributes */
};

} // end of namespace system

/*!
 \brief String representation of a synchronization
 \param sync_id : synchronization identifier
 \param system : system of processes
 \pre sync_id is a valid synchronization identifier in system
 \return a string representation of synchronization sync_id in system
 \throw std::invalid_argument : if id is not a valid synchronization identifier in system
 */
std::string to_string(tchecker::sync_id_t sync_id, tchecker::system::system_t const & system);

} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_HH
