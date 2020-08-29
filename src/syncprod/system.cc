/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <stack>
#include <tuple>
#include <unordered_set>

#include "tchecker/basictypes.hh"
#include "tchecker/syncprod/allocators.hh"
#include "tchecker/syncprod/syncprod.hh"
#include "tchecker/syncprod/system.hh"
#include "tchecker/system/synchronization.hh"

namespace tchecker {

namespace syncprod {

tchecker::syncprod::system_t::asynchronous_edges_collection_t const tchecker::syncprod::system_t::_empty_async_edges;

system_t::system_t(tchecker::parsing::system_declaration_t const & sysdecl) : tchecker::system::system_t(sysdecl)
{
  extract_asynchronous_edges();
}

system_t::system_t(tchecker::system::system_t const & system) : tchecker::system::system_t(system)
{
  extract_asynchronous_edges();
}

bool system_t::is_asynchronous(tchecker::system::edge_t const & edge) const
{
  tchecker::process_id_t pid = edge.pid();
  tchecker::event_id_t event_id = edge.event_id();
  for (tchecker::system::synchronization_t const & sync : synchronizations())
    for (tchecker::system::sync_constraint_t const & constr : sync.synchronization_constraints())
      if (constr.pid() == pid && constr.event_id() == event_id && sync.size() > 1)
        return false;
  return true;
}

system_t::asynchronous_edges_const_iterator_t::asynchronous_edges_const_iterator_t(
    asynchronous_edges_collection_t::const_iterator const & it)
    : asynchronous_edges_collection_t::const_iterator(it)
{
}

tchecker::system::edge_const_shared_ptr_t system_t::asynchronous_edges_const_iterator_t::operator*() const
{
  return asynchronous_edges_collection_t::const_iterator::operator*();
}

tchecker::range_t<tchecker::syncprod::system_t::asynchronous_edges_const_iterator_t>
system_t::asynchronous_outgoing_edges(tchecker::loc_id_t loc) const
{
  if (loc >= _async_outgoing_edges.size())
    return tchecker::make_range(asynchronous_edges_const_iterator_t(_empty_async_edges.begin()),
                                asynchronous_edges_const_iterator_t(_empty_async_edges.end()));
  return tchecker::make_range(asynchronous_edges_const_iterator_t(_async_outgoing_edges[loc].begin()),
                              asynchronous_edges_const_iterator_t(_async_outgoing_edges[loc].end()));
}

tchecker::range_t<tchecker::syncprod::system_t::asynchronous_edges_const_iterator_t>
system_t::asynchronous_incoming_edges(tchecker::loc_id_t loc) const
{
  if (loc >= _async_incoming_edges.size())
    return tchecker::make_range(asynchronous_edges_const_iterator_t(_empty_async_edges.begin()),
                                asynchronous_edges_const_iterator_t(_empty_async_edges.end()));
  return tchecker::make_range(asynchronous_edges_const_iterator_t(_async_incoming_edges[loc].begin()),
                              asynchronous_edges_const_iterator_t(_async_incoming_edges[loc].end()));
}

void system_t::extract_asynchronous_edges()
{
  for (tchecker::system::edge_const_shared_ptr_t const & edge : edges())
    if (is_asynchronous(*edge))
      add_asynchronous_edge(edge);
}

void system_t::add_asynchronous_edge(tchecker::system::edge_const_shared_ptr_t const & edge)
{
  assert(is_asynchronous(*edge));
  tchecker::loc_id_t src = edge->src(), tgt = edge->tgt();
  if (src >= _async_outgoing_edges.size())
    _async_outgoing_edges.resize(src + 1);
  _async_outgoing_edges[src].push_back(edge);
  if (tgt >= _async_incoming_edges.size())
    _async_incoming_edges.resize(tgt + 1);
  _async_incoming_edges[tgt].push_back(edge);
}

/*!
 \class synchronizer_t
 \brief Synchronizes a system of timed processes as a system with a unique synchronized process
 */
class synchronizer_t {
public:
  /*!
   \brief Constructor
   \param system : a system of timed processes
   \param separator : separator for names
   \param process_name : name of synchronized process
   */
  synchronizer_t(tchecker::syncprod::system_t const & system, std::string const & separator, std::string const & process_name)
      : _system(system), _separator(separator), _process_name(process_name), _product(_system.name(), _system.attributes()),
        _salloc(1000000, 1000000, _system.processes_count()), _talloc(1000000, 1000000, _system.processes_count())
  {
    integer_variables();
    clock_variables();
    process();
    locations_edges_events();
  }

  /*!
   \brief Accessor
   \return Synchronized product of given system of timed processes
   */
  tchecker::system::system_t synchronized_product() const { return _product; }

private:
  using state_ptr_t = tchecker::intrusive_shared_ptr_t<tchecker::syncprod::shared_state_t>;
  using transition_ptr_t = tchecker::intrusive_shared_ptr_t<tchecker::syncprod::shared_transition_t>;

  /*!
   \brief Compute a name from a synchronization
   */
  std::string namify(tchecker::system::synchronization_t const & sync)
  {
    std::string name;
    auto constraints = sync.synchronization_constraints();
    tchecker::system::synchronization_t::const_iterator_t begin = constraints.begin();
    tchecker::system::synchronization_t::const_iterator_t end = constraints.end();
    for (auto it = begin; it != end; ++it) {
      if (it != begin)
        name += _separator;
      tchecker::system::sync_constraint_t const & constr = *it;
      name += _system.process_name(constr.pid()) + _separator + _system.event_name(constr.event_id());
    }
    return name;
  }

  /*!
   \brief Compute a name from a state (from vloc)
   */
  std::string namify(state_ptr_t const & state)
  {
    std::string name;
    auto begin = state->vloc().begin(), end = state->vloc().end();
    for (auto it = begin; it != end; ++it) {
      if (it != begin)
        name += _separator;
      name += _system.location(*it)->name();
    }
    return name;
  }

  /*!
   \brief Compute attributes from a state (from vloc)
   */
  tchecker::system::attributes_t attributes(state_ptr_t const & state)
  {
    tchecker::system::attributes_t attr;
    for (tchecker::loc_id_t id : state->vloc())
      attr.add_attributes(_system.location(id)->attributes());
    return attr;
  }

  /*!
   \brief Compute a name from a transition (from vedge)
   */
  std::string namify(transition_ptr_t const & transition)
  {
    std::string name;
    auto begin = transition->vedge().begin(), end = transition->vedge().end();
    for (auto it = begin; it != end; ++it) {
      if (it != begin)
        name += _separator;
      name += _system.process_name(_system.edge(*it)->pid()) + _separator + _system.event_name(_system.edge(*it)->event_id());
    }
    return name;
  }

  /*!
   \brief Compute attributes from a transition (from vedge)
   */
  tchecker::system::attributes_t attributes(transition_ptr_t const & transition)
  {
    tchecker::system::attributes_t attr;
    for (tchecker::event_id_t id : transition->vedge())
      attr.add_attributes(_system.edge(id)->attributes());
    return attr;
  }

  /*!
   \brief Add bounded integer variables to synchronized product
   */
  void integer_variables()
  {
    for (tchecker::intvar_id_t id = 0; id < _system.intvars_count(tchecker::VK_DECLARED); ++id) {
      tchecker::intvar_info_t const & info = _system.integer_variables().info(id);
      _product.add_intvar(_system.intvar_name(id), info.size(), info.min(), info.max(), info.initial_value(),
                          _system.intvar_attributes(id));
    }
  }

  /*!
   \brief Add clock variables to synchronized product
   */
  void clock_variables()
  {
    for (tchecker::clock_id_t id = 0; id < _system.clocks_count(tchecker::VK_DECLARED); ++id) {
      tchecker::clock_info_t const & info = _system.clock_variables().info(id);
      _product.add_clock(_system.clock_name(id), info.size(), _system.clock_attributes(id));
    }
  }

  /*!
   \brief Add process to synchronized product
   */
  void process() { _product.add_process(_process_name); }

  /*!
   \brief Add locations, edges and events to synchronized product
   */
  void locations_edges_events()
  {
    tchecker::process_id_t pid = _product.process_id(_process_name);

    std::stack<state_ptr_t> waiting;
    tchecker::syncprod::syncprod_t sp(_system);
    std::vector<std::tuple<state_ptr_t, transition_ptr_t>> v;

    v.clear();
    initial_states(sp, v);
    for (auto && [state, transition] : v) {
      std::string state_name = namify(state);
      if (!_product.is_location(pid, state_name)) {
        _product.add_location(pid, state_name, attributes(state));
        waiting.push(state);
      }
    }

    while (!waiting.empty()) {
      state_ptr_t src = waiting.top();
      waiting.pop();

      tchecker::loc_id_t src_id = _product.location(pid, namify(src))->id();

      v.clear();
      next_states(sp, src, v);
      for (auto && [tgt, transition] : v) {
        std::string tgt_name = namify(tgt);
        if (!_product.is_location(pid, tgt_name)) {
          _product.add_location(pid, tgt_name, attributes(tgt));
          waiting.push(tgt);
        }
        tchecker::loc_id_t tgt_id = _product.location(pid, tgt_name)->id();

        std::string event_name = namify(transition);
        if (!_product.is_event(event_name))
          _product.add_event(event_name);
        tchecker::event_id_t event_id = _product.event_id(event_name);

        _product.add_edge(pid, src_id, tgt_id, event_id, attributes(transition));
      }

      _salloc.collect();
      _talloc.collect();
    }
  }

  /*!
   \brief Compute initial pairs (state, transition)
   \param sp : synchronized product of timed processes
   \param v : vector of (state, transition)
   \post all initial pairs (state, transition) have been added to v
   */
  void initial_states(tchecker::syncprod::syncprod_t & sp, std::vector<std::tuple<state_ptr_t, transition_ptr_t>> & v)
  {
    auto const initial_states = sp.initial();
    for (auto it = initial_states.begin(); it != initial_states.end(); ++it) {
      state_ptr_t state = _salloc.construct();
      transition_ptr_t transition = _talloc.construct();
      sp.initialize(*state, *transition, *it);
      v.push_back(std::make_tuple(state, transition));
    }
  }

  /*!
   \brief Compute next pairs (state, transition)
   \param sp : synchronized product of timed processes
   \param state : a state
   \param v : vector of (state, transition)
   \post all successor pairs (state, transition) of s have been added to v
   */
  void next_states(tchecker::syncprod::syncprod_t & sp, state_ptr_t const & state,
                   std::vector<std::tuple<state_ptr_t, transition_ptr_t>> & v)
  {
    auto const outgoing_edges = sp.outgoing_edges(*state);
    for (auto it = outgoing_edges.begin(); it != outgoing_edges.end(); ++it) {
      state_ptr_t next_state = _salloc.construct_from_state(*state);
      transition_ptr_t transition = _talloc.construct();
      sp.next(*next_state, *transition, *it);
      v.push_back(std::make_tuple(next_state, transition));
    }
  }

  tchecker::syncprod::system_t const & _system;            /*!< Original system of timed processes */
  std::string const & _separator;                          /*!< Separator string */
  std::string const & _process_name;                       /*!< Name of synchronized process */
  tchecker::system::system_t _product;                     /*!< Synchronized product of _system */
  tchecker::syncprod::state_pool_allocator_t _salloc;      /*!< Allocator of states in synchronized product */
  tchecker::syncprod::transition_pool_allocator_t _talloc; /*!< Allocator of transitions in synchronized product */
};

tchecker::system::system_t synchronized_product(tchecker::syncprod::system_t const & system, std::string const & process_name,
                                                std::string const & separator)
{
  tchecker::syncprod::synchronizer_t synchronizer(system, separator, process_name);
  return synchronizer.synchronized_product();
}

} // end of namespace syncprod

} // end of namespace tchecker
