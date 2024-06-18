/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "tchecker/system/system.hh"

namespace tchecker {

namespace system {

/*!
 \class system_builder_t
 \brief builds a system from a system declaration
 */
class system_builder_t : public tchecker::parsing::declaration_visitor_t {
public:
  /*!
   \brief Constructor
   */
  system_builder_t(tchecker::system::system_t & system) : _system(system) {}

  /*!
   \brief Destructor
   */
  virtual ~system_builder_t() = default;

  /*!
   \brief Build system from inner declarations
   */
  virtual void visit(tchecker::parsing::system_declaration_t const & d)
  {
    for (std::shared_ptr<tchecker::parsing::declaration_t> const decl : d.declarations())
      decl->visit(*this);
  }

  /*!
   \brief Add clock variable
   */
  virtual void visit(tchecker::parsing::clock_declaration_t const & d)
  {
    if (std::numeric_limits<tchecker::clock_id_t>::max() < d.size())
      throw std::out_of_range("clock size cannot be represented");
    tchecker::clock_id_t size = d.size();
    _system.add_clock(d.name(), size, d.attributes());
  }

  /*!
   \brief Add bounded integer variable
   */
  virtual void visit(tchecker::parsing::int_declaration_t const & d)
  {
    if (std::numeric_limits<tchecker::intvar_id_t>::max() < d.size())
      throw std::out_of_range("integer variable size cannot be represented");
    tchecker::intvar_id_t size = d.size();
    _system.add_intvar(d.name(), size, d.min(), d.max(), d.init(), d.attributes());
  }

  /*!
   \brief Add a process
   */
  virtual void visit(tchecker::parsing::process_declaration_t const & d) { _system.add_process(d.name(), d.attributes()); }

  /*!
   \brief Add an event
   */
  virtual void visit(tchecker::parsing::event_declaration_t const & d) { _system.add_event(d.name(), d.attributes()); }

  /*!
   \brief Add a location
   */
  virtual void visit(tchecker::parsing::location_declaration_t const & d)
  {
    tchecker::process_id_t pid = _system.process_id(d.process().name());
    _system.add_location(pid, d.name(), d.attributes());
  }

  /*!
   \brief Add an edge
   */
  virtual void visit(tchecker::parsing::edge_declaration_t const & d)
  {
    tchecker::process_id_t pid = _system.process_id(d.process().name());
    tchecker::loc_id_t src = _system.location(pid, d.src().name())->id();
    tchecker::loc_id_t tgt = _system.location(pid, d.tgt().name())->id();
    tchecker::event_id_t event = _system.event_id(d.event().name());
    _system.add_edge(pid, src, tgt, event, d.attributes());
  }

  /*!
   \brief Add a synchronization
   */
  virtual void visit(tchecker::parsing::sync_declaration_t const & d)
  {
    std::vector<tchecker::system::sync_constraint_t> v;
    for (std::shared_ptr<tchecker::parsing::sync_constraint_t> const & c : d.sync_constraints()) {
      tchecker::process_id_t pid = _system.process_id(c->process().name());
      tchecker::event_id_t event = _system.event_id(c->event().name());
      v.emplace_back(pid, event, c->strength());
    }

    _system.add_synchronization(v, d.attributes());
  }

private:
  tchecker::system::system_t & _system; /*!< System to build */
};

/* system_t */

system_t::system_t(std::string const & name, tchecker::system::attributes_t const & attributes)
    : _name(name), _attributes(attributes)
{
}

system_t::system_t(tchecker::parsing::system_declaration_t const & sysdecl)
    : _name(sysdecl.name()), _attributes(sysdecl.attributes())
{
  tchecker::system::system_builder_t builder(*this);
  sysdecl.visit(builder);
}

tchecker::system::attribute_keys_map_t const & system_t::known_attributes()
{
  static tchecker::system::attribute_keys_map_t const known_attr = {{
      {},          // tchecker::system::ATTR_CLOCK
      {},          // tchecker::system::ATTR_EDGE
      {},          // tchecker::system::ATTR_EVENT
      {},          // tchecker::system::ATTR_INTVAR
      {"initial"}, // tchecker::system::ATTR_LOCATION
      {},          // tchecker::system::ATTR_PROCESS
      {},          // tchecker::system::ATTR_SYNC
      {}           // tchecker::system::ATTR_SYSTEM
  }};
  return known_attr;
}

void system_t::add_clock(std::string const & name, tchecker::clock_id_t size, tchecker::system::attributes_t const & attributes)
{
  if (has_variable(name))
    throw std::invalid_argument("Variable " + name + " is already declared");
  tchecker::system::clocks_t::add_clock(name, size, attributes);
}

void system_t::add_edge(tchecker::process_id_t pid, tchecker::loc_id_t src, tchecker::loc_id_t tgt,
                        tchecker::event_id_t event_id, tchecker::system::attributes_t const & attributes)
{
  if (!tchecker::system::processes_t::is_process(pid))
    throw std::invalid_argument("Process " + std::to_string(pid) + " is not declared");
  if (!tchecker::system::locs_t::is_location(src))
    throw std::invalid_argument("Source location " + std::to_string(src) + " is not declared");
  if (!tchecker::system::locs_t::is_location(tgt))
    throw std::invalid_argument("Traget location " + std::to_string(tgt) + " is not declared");
  if (!tchecker::system::events_t::is_event(event_id))
    throw std::invalid_argument("Event " + std::to_string(event_id) + " is not declared");
  tchecker::system::edges_t::add_edge(pid, src, tgt, event_id, attributes);
}

void system_t::add_intvar(std::string const & name, tchecker::intvar_id_t size, tchecker::integer_t min,
                          tchecker::integer_t max, tchecker::integer_t initial,
                          tchecker::system::attributes_t const & attributes)
{
  if (has_variable(name))
    throw std::invalid_argument("Variable " + name + " is already declared");
  tchecker::system::intvars_t::add_intvar(name, size, min, max, initial, attributes);
}

void system_t::add_location(tchecker::process_id_t pid, std::string const & name,
                            tchecker::system::attributes_t const & attributes)
{
  if (!tchecker::system::processes_t::is_process(pid))
    throw std::invalid_argument("Process " + std::to_string(pid) + " is not declared");
  tchecker::system::locs_t::add_location(pid, name, attributes);
}

void system_t::add_synchronization(std::vector<tchecker::system::sync_constraint_t> const & v,
                                   tchecker::system::attributes_t const & attributes)
{
  for (tchecker::system::sync_constraint_t const & c : v) {
    if (!tchecker::system::processes_t::is_process(c.pid()))
      throw std::invalid_argument("Process " + std::to_string(c.pid()) + " is not declared");
    if (!tchecker::system::events_t::is_event(c.event_id()))
      throw std::invalid_argument("Event " + std::to_string(c.event_id()) + " is not declared");
  }
  tchecker::system::synchronizations_t::add_synchronization(v, attributes);
}

bool system_t::has_variable(std::string const & name)
{
  return tchecker::system::clocks_t::is_clock(name) || tchecker::system::intvars_t::is_intvar(name);
}

} // end of namespace system

std::string to_string(tchecker::sync_id_t sync_id, tchecker::system::system_t const & system)
{
  tchecker::system::synchronization_t sync = system.synchronization(sync_id);

  std::stringstream ss;
  ss << "<";
  auto r = sync.synchronization_constraints();
  for (auto it = r.begin(); it != r.end(); ++it) {
    if (it != r.begin())
      ss << ",";
    tchecker::system::sync_constraint_t const & constr = *it;
    ss << system.process_name(constr.pid()) << "@" << system.event_name(constr.event_id());
    if (constr.strength() == tchecker::SYNC_WEAK)
      ss << "?";
  }
  ss << ">";
  return ss.str();
}

} // end of namespace tchecker
