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

#include <boost/algorithm/string.hpp>

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
  compute_committed_locations();
  compute_labels();
}

system_t::system_t(tchecker::system::system_t const & system) : tchecker::system::system_t(system)
{
  extract_asynchronous_edges();
  compute_committed_locations();
  compute_labels();
}

tchecker::system::attribute_keys_map_t const & system_t::known_attributes()
{
  static tchecker::system::attribute_keys_map_t const known_attr{[&]() {
    tchecker::system::attribute_keys_map_t attr(tchecker::system::system_t::known_attributes());
    attr[tchecker::system::ATTR_LOCATION].insert("committed");
    attr[tchecker::system::ATTR_LOCATION].insert("labels");
    return attr;
  }()};
  return known_attr;
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

boost::dynamic_bitset<> const & system_t::labels(tchecker::loc_id_t id) const
{
  assert(is_location(id));
  return _labels[id];
}

boost::dynamic_bitset<> system_t::labels(std::string const & labels) const
{
  boost::dynamic_bitset<> s{this->labels_count()};
  if (labels == "")
    return s;

  std::vector<std::string> v;
  boost::split(v, labels, boost::is_any_of(","));
  for (std::string const & l : v) {
    if (!this->is_label(l))
      throw std::invalid_argument("Unknown label '" + l + "'");
    s.set(this->label_id(l));
  }
  return s;
}

bool system_t::is_committed(tchecker::loc_id_t id) const
{
  assert(is_location(id));
  return _committed[id] == 1;
}

boost::dynamic_bitset<> const & system_t::committed_locations() const { return _committed; }

void system_t::extract_asynchronous_edges()
{
  for (tchecker::system::edge_const_shared_ptr_t const & edge : edges())
    if (is_asynchronous(*edge))
      add_asynchronous_edge(edge);
}

void system_t::compute_committed_locations()
{
  _committed.resize(this->locations_count(), false);

  for (tchecker::loc_id_t const id : this->locations_identifiers()) {
    auto const & attr = tchecker::syncprod::system_t::location(id)->attributes();
    set_committed(id, attr.range("committed"));
  }
}

static void labels_from_attrs(tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & attrs,
                              std::vector<std::string> & labels)
{
  std::vector<std::string> splitted_labels;
  for (auto && attr : attrs) {
    boost::split(splitted_labels, attr.value(), boost::is_any_of(","));
    labels.insert(labels.end(), splitted_labels.begin(), splitted_labels.end());
  }
}

void system_t::compute_labels()
{
  tchecker::system::locs_t::locations_identifiers_range_t const locations_identifiers = this->locations_identifiers();

  // Compute labels index
  std::vector<std::string> labels;
  for (tchecker::loc_id_t const loc_id : locations_identifiers) {
    auto const & attr = tchecker::syncprod::system_t::location(loc_id)->attributes();
    labels_from_attrs(attr.range("labels"), labels);
  }

  for (std::string const & l : labels) {
    if (!tchecker::syncprod::labels_t::is_label(l))
      tchecker::syncprod::labels_t::add_label(l);
  }

  // Set location labels
  tchecker::label_id_t const & labels_count = this->labels_count();

  _labels.clear();
  _labels.resize(this->locations_count());

  for (tchecker::loc_id_t const loc_id : locations_identifiers) {
    _labels[loc_id].resize(labels_count);
    _labels[loc_id].reset();
    auto const & attr = tchecker::syncprod::system_t::location(loc_id)->attributes();
    labels.clear();
    labels_from_attrs(attr.range("labels"), labels);
    for (std::string const & l : labels)
      _labels[loc_id].set(this->label_id(l));
  }
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

void system_t::set_committed(tchecker::loc_id_t id,
                             tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & flags)
{
  if (flags.begin() != flags.end())
    _committed[id] = 1;
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
  synchronizer_t(std::shared_ptr<tchecker::syncprod::system_t const> const & system, std::string const & separator,
                 std::string const & process_name)
      : _system(system), _separator(separator), _process_name(process_name), _product(_system->name(), _system->attributes())
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
      name += _system->process_name(constr.pid()) + _separator + _system->event_name(constr.event_id());
    }
    return name;
  }

  /*!
   \brief Compute a name from a state (from vloc)
   */
  std::string namify(tchecker::syncprod::state_t const & state)
  {
    std::string name;
    auto begin = state.vloc().begin(), end = state.vloc().end();
    for (auto it = begin; it != end; ++it) {
      if (it != begin)
        name += _separator;
      name += _system->location(*it)->name();
    }
    return name;
  }

  /*!
   \brief Compute attributes from a state (from vloc)
   */
  tchecker::system::attributes_t attributes(tchecker::syncprod::state_t const & state)
  {
    tchecker::process_id_t count_initial = 0;
    tchecker::system::attributes_t attributes;
    for (tchecker::loc_id_t id : state.vloc())
      for (auto && attr : _system->location(id)->attributes().range()) {
        if (attr.key() == "initial")
          ++count_initial;
        else
          attributes.add_attribute(attr.key(), attr.value(), attr.parsing_position());
      }
    if (count_initial == state.vloc().size()) // all processes initial
      attributes.add_attribute("initial", "", tchecker::system::attr_parsing_position_t{});
    return attributes;
  }

  /*!
   \brief Compute a name from a transition (from vedge)
   */
  std::string namify(tchecker::syncprod::transition_t const & transition)
  {
    std::string name;
    auto begin = transition.vedge().begin(), end = transition.vedge().end();
    for (auto it = begin; it != end; ++it) {
      if (it != begin)
        name += _separator;
      name +=
          _system->process_name(_system->edge(*it)->pid()) + _separator + _system->event_name(_system->edge(*it)->event_id());
    }
    return name;
  }

  /*!
   \brief Compute attributes from a transition (from vedge)
   */
  tchecker::system::attributes_t attributes(tchecker::syncprod::transition_t const & transition)
  {
    tchecker::system::attributes_t attr;
    for (tchecker::event_id_t id : transition.vedge())
      attr.add_attributes(_system->edge(id)->attributes());
    return attr;
  }

  /*!
   \brief Add bounded integer variables to synchronized product
   */
  void integer_variables()
  {
    for (tchecker::intvar_id_t const id : _system->intvars_identifiers(tchecker::VK_DECLARED)) {
      tchecker::intvar_info_t const & info = _system->integer_variables().info(id);
      _product.add_intvar(_system->intvar_name(id), info.size(), info.min(), info.max(), info.initial_value(),
                          _system->intvar_attributes(id));
    }
  }

  /*!
   \brief Add clock variables to synchronized product
   */
  void clock_variables()
  {
    for (tchecker::clock_id_t const id : _system->clocks_identifiers(tchecker::VK_DECLARED)) {
      tchecker::clock_info_t const & info = _system->clock_variables().info(id);
      _product.add_clock(_system->clock_name(id), info.size(), _system->clock_attributes(id));
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
    std::size_t const block_size = 10000;
    std::size_t const table_size = 65536;

    tchecker::process_id_t pid = _product.process_id(_process_name);

    std::stack<tchecker::syncprod::state_sptr_t> waiting;
    tchecker::syncprod::syncprod_t sp(_system, tchecker::ts::NO_SHARING, block_size, table_size);
    std::vector<tchecker::syncprod::syncprod_t::sst_t> v;

    sp.initial(v);
    for (auto && [status, state, transition] : v) {
      std::string state_name = namify(*state);
      if (!_product.is_location(pid, state_name)) {
        _product.add_location(pid, state_name, attributes(*state));
        waiting.push(state);
      }
    }
    v.clear();

    while (!waiting.empty()) {
      tchecker::syncprod::const_state_sptr_t src = static_cast<tchecker::syncprod::const_state_sptr_t>(waiting.top());
      waiting.pop();

      tchecker::loc_id_t src_id = _product.location(pid, namify(*src))->id();

      sp.next(src, v);
      for (auto && [status, tgt, transition] : v) {
        std::string tgt_name = namify(*tgt);
        if (!_product.is_location(pid, tgt_name)) {
          _product.add_location(pid, tgt_name, attributes(*tgt));
          waiting.push(tgt);
        }
        tchecker::loc_id_t tgt_id = _product.location(pid, tgt_name)->id();

        std::string event_name = namify(*transition);
        if (!_product.is_event(event_name))
          _product.add_event(event_name);
        tchecker::event_id_t event_id = _product.event_id(event_name);

        _product.add_edge(pid, src_id, tgt_id, event_id, attributes(*transition));
      }
      v.clear();
    }
  }

  std::shared_ptr<tchecker::syncprod::system_t const> _system; /*!< System of timed processes */
  std::string _separator;                                      /*!< Separator string */
  std::string _process_name;                                   /*!< Name of synchronized process */
  tchecker::system::system_t _product;                         /*!< Synchronized product of _system */
};

tchecker::system::system_t synchronized_product(std::shared_ptr<tchecker::syncprod::system_t const> const & system,
                                                std::string const & process_name, std::string const & separator)
{
  tchecker::syncprod::synchronizer_t synchronizer(system, separator, process_name);
  return synchronizer.synchronized_product();
}

} // end of namespace syncprod

} // end of namespace tchecker
