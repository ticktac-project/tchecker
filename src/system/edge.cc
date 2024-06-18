/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <stdexcept>

#include "tchecker/system/edge.hh"

namespace tchecker {

namespace system {

/* edge_t */

edge_t::edge_t(tchecker::process_id_t pid, tchecker::edge_id_t id, tchecker::loc_id_t src, tchecker::loc_id_t tgt,
               tchecker::event_id_t event_id, tchecker::system::attributes_t const & attributes)
    : _pid(pid), _id(id), _src(src), _tgt(tgt), _event_id(event_id), _attributes(attributes)
{
}

/* edges_collection_const_iterator_t */

tchecker::system::edges_collection_const_iterator_t::edges_collection_const_iterator_t(
    tchecker::system::edges_collection_t::const_iterator const & it)
    : tchecker::system::edges_collection_t::const_iterator(it)
{
}

/* loc_edges_maps_t */

tchecker::system::edges_collection_t const loc_edges_maps_t::_empty_edges;

void loc_edges_maps_t::clear()
{
  _loc_to_events.clear();
  _loc_to_edges.clear();
  _loc_event_to_edges.clear();
}

void loc_edges_maps_t::add_edge(tchecker::loc_id_t loc, tchecker::system::edge_shared_ptr_t const & edge)
{
  tchecker::event_id_t event = edge->event_id();

  if (loc >= _loc_to_edges.size())
    _loc_to_edges.resize(loc + 1);
  _loc_to_edges[loc].push_back(edge);

  if (loc >= _loc_to_events.size())
    _loc_to_events.resize(loc + 1);
  if (event >= _loc_to_events[loc].size())
    _loc_to_events[loc].resize(event + 1);
  _loc_to_events[loc][event] = 1;

  if (loc >= _loc_event_to_edges.size())
    _loc_event_to_edges.resize(loc + 1);
  if (event >= _loc_event_to_edges[loc].size())
    _loc_event_to_edges[loc].resize(event + 1);
  _loc_event_to_edges[loc][event].push_back(edge);
}

tchecker::range_t<tchecker::system::edges_collection_const_iterator_t> loc_edges_maps_t::edges(tchecker::loc_id_t loc) const
{
  if (loc >= _loc_to_edges.size())
    return tchecker::make_range<tchecker::system::edges_collection_const_iterator_t>(_empty_edges.begin(), _empty_edges.end());
  return tchecker::make_range<tchecker::system::edges_collection_const_iterator_t>(_loc_to_edges[loc].begin(),
                                                                                   _loc_to_edges[loc].end());
}

tchecker::range_t<tchecker::system::edges_collection_const_iterator_t> loc_edges_maps_t::edges(tchecker::loc_id_t loc,
                                                                                               tchecker::event_id_t event) const
{
  if (loc >= _loc_event_to_edges.size() || event >= _loc_event_to_edges[loc].size())
    return tchecker::make_range<tchecker::system::edges_collection_const_iterator_t>(_empty_edges.begin(), _empty_edges.end());
  return tchecker::make_range<tchecker::system::edges_collection_const_iterator_t>(_loc_event_to_edges[loc][event].begin(),
                                                                                   _loc_event_to_edges[loc][event].end());
}

bool loc_edges_maps_t::event(tchecker::loc_id_t loc, tchecker::event_id_t event) const
{
  return (loc < _loc_to_events.size() && event < _loc_to_events[loc].size() && _loc_to_events[loc][event]);
}

/* proc_edges_map_t */

tchecker::system::edges_collection_t const proc_edges_maps_t::_empty_edges;

void proc_edges_maps_t::clear()
{
  _proc_to_events.clear();
  _proc_to_edges.clear();
  _proc_event_to_edges.clear();
}

void proc_edges_maps_t::add_edge(tchecker::process_id_t pid, tchecker::system::edge_shared_ptr_t const & edge)
{
  tchecker::event_id_t event_id = edge->event_id();

  if (pid >= _proc_to_edges.size())
    _proc_to_edges.resize(pid + 1);
  _proc_to_edges[pid].push_back(edge);

  if (pid >= _proc_to_events.size())
    _proc_to_events.resize(pid + 1);
  if (event_id >= _proc_to_events[pid].size())
    _proc_to_events[pid].resize(event_id + 1);
  _proc_to_events[pid][event_id] = 1;

  if (pid >= _proc_event_to_edges.size())
    _proc_event_to_edges.resize(pid + 1);
  if (event_id >= _proc_event_to_edges[pid].size())
    _proc_event_to_edges[pid].resize(event_id + 1);
  _proc_event_to_edges[pid][event_id].push_back(edge);
}

tchecker::range_t<tchecker::system::edges_collection_const_iterator_t>
proc_edges_maps_t::edges(tchecker::process_id_t pid) const
{
  if (pid >= _proc_to_edges.size())
    return tchecker::make_range<tchecker::system::edges_collection_const_iterator_t>(_empty_edges.begin(), _empty_edges.end());
  return tchecker::make_range<tchecker::system::edges_collection_const_iterator_t>(_proc_to_edges[pid].begin(),
                                                                                   _proc_to_edges[pid].end());
}

tchecker::range_t<tchecker::system::edges_collection_const_iterator_t>
proc_edges_maps_t::edges(tchecker::process_id_t pid, tchecker::event_id_t event_id) const
{
  if (pid >= _proc_event_to_edges.size() || event_id >= _proc_event_to_edges[pid].size())
    return tchecker::make_range<tchecker::system::edges_collection_const_iterator_t>(_empty_edges.begin(), _empty_edges.end());
  return tchecker::make_range<tchecker::system::edges_collection_const_iterator_t>(_proc_event_to_edges[pid][event_id].begin(),
                                                                                   _proc_event_to_edges[pid][event_id].end());
}

bool proc_edges_maps_t::event(tchecker::process_id_t pid, tchecker::event_id_t event_id) const
{
  return (pid < _proc_to_events.size() && event_id < _proc_to_events[pid].size() && _proc_to_events[pid][event_id]);
}

/* edges_t */

static tchecker::system::edges_collection_t const empty_edges; /*!< Empty collection of edges (for processes with no edges) */

edges_t::edges_t() : _loc_edges_maps(tchecker::system::EDGE_DIRECTION_COUNT)
{
  for (unsigned i = 0; i < tchecker::system::EDGE_DIRECTION_COUNT; ++i)
    _loc_edges_maps[i] = std::shared_ptr<tchecker::system::loc_edges_maps_t>(new tchecker::system::loc_edges_maps_t());
}

edges_t::edges_t(tchecker::system::edges_t const & edges) : _loc_edges_maps(tchecker::system::EDGE_DIRECTION_COUNT)
{
  for (unsigned i = 0; i < tchecker::system::EDGE_DIRECTION_COUNT; ++i)
    _loc_edges_maps[i] = std::shared_ptr<tchecker::system::loc_edges_maps_t>(new tchecker::system::loc_edges_maps_t());
  add_edges(edges);
}

edges_t::edges_t(tchecker::system::edges_t && edges)
    : _edges(std::move(edges._edges)), _loc_edges_maps(std::move(edges._loc_edges_maps)),
      _proc_edges_map(std::move(edges._proc_edges_map))
{
}

edges_t::~edges_t() { clear(); }

tchecker::system::edges_t & edges_t::operator=(tchecker::system::edges_t const & edges)
{
  if (this != &edges) {
    clear();
    add_edges(edges);
  }
  return *this;
}

tchecker::system::edges_t & edges_t::operator=(tchecker::system::edges_t && edges)
{
  if (this != &edges) {
    clear();
    _edges = std::move(edges._edges);
    _loc_edges_maps = std::move(edges._loc_edges_maps);
    _proc_edges_map = std::move(edges._proc_edges_map);
  }
  return *this;
}

void edges_t::clear()
{
  _proc_edges_map.clear();
  _loc_edges_maps.clear();
  _edges.clear();
}

void edges_t::add_edge(tchecker::process_id_t pid, tchecker::loc_id_t src, tchecker::loc_id_t tgt,
                       tchecker::event_id_t event_id, tchecker::system::attributes_t const & attributes)
{
  tchecker::edge_id_t id = _edges.size();

  if (!tchecker::valid_edge_id(id))
    throw std::runtime_error("add_edge: invalid edge identifier");

  tchecker::system::edge_shared_ptr_t edge(new tchecker::system::edge_t(pid, id, src, tgt, event_id, attributes));
  _edges.push_back(edge);
  assert(_edges.back()->id() == _edges.size() - 1);

  _loc_edges_maps[tchecker::system::INCOMING_EDGE]->add_edge(tgt, edge);
  _loc_edges_maps[tchecker::system::OUTGOING_EDGE]->add_edge(src, edge);

  _proc_edges_map.add_edge(pid, edge);
}

tchecker::system::edges_t::edges_identifiers_range_t edges_t::edges_identifiers() const
{
  return tchecker::make_integer_range<tchecker::event_id_t>(0, edges_count());
}

tchecker::range_t<tchecker::system::edges_t::const_iterator_t> edges_t::edges() const
{
  return tchecker::make_range(const_iterator_t(_edges.begin()), const_iterator_t(_edges.end()));
}

tchecker::range_t<tchecker::system::edges_t::const_iterator_t> edges_t::outgoing_edges(tchecker::loc_id_t loc) const
{
  return _loc_edges_maps[tchecker::system::OUTGOING_EDGE]->edges(loc);
}

tchecker::range_t<tchecker::system::edges_t::const_iterator_t> edges_t::outgoing_edges(tchecker::loc_id_t loc,
                                                                                       tchecker::event_id_t event) const
{
  return _loc_edges_maps[tchecker::system::OUTGOING_EDGE]->edges(loc, event);
}

tchecker::range_t<tchecker::system::edges_t::const_iterator_t> edges_t::incoming_edges(tchecker::loc_id_t loc) const
{
  return _loc_edges_maps[tchecker::system::INCOMING_EDGE]->edges(loc);
}

tchecker::range_t<tchecker::system::edges_t::const_iterator_t> edges_t::incoming_edges(tchecker::loc_id_t loc,
                                                                                       tchecker::event_id_t event) const
{
  return _loc_edges_maps[tchecker::system::INCOMING_EDGE]->edges(loc, event);
}

bool edges_t::outgoing_event(tchecker::loc_id_t loc, tchecker::event_id_t event) const
{
  return _loc_edges_maps[tchecker::system::OUTGOING_EDGE]->event(loc, event);
}

bool edges_t::incoming_event(tchecker::loc_id_t loc, tchecker::event_id_t event) const
{
  return _loc_edges_maps[tchecker::system::INCOMING_EDGE]->event(loc, event);
}

std::shared_ptr<tchecker::system::loc_edges_maps_t const> edges_t::outgoing_edges_maps() const
{
  return _loc_edges_maps[tchecker::system::OUTGOING_EDGE];
}

std::shared_ptr<tchecker::system::loc_edges_maps_t const> edges_t::incoming_edges_maps() const
{
  return _loc_edges_maps[tchecker::system::INCOMING_EDGE];
}

void edges_t::add_edges(tchecker::system::edges_t const & edges)
{
  for (tchecker::system::edge_const_shared_ptr_t edge : edges._edges)
    add_edge(edge->pid(), edge->src(), edge->tgt(), edge->event_id(), edge->attributes());
}

tchecker::range_t<tchecker::system::edges_t::const_iterator_t> edges_t::edges(tchecker::process_id_t pid) const
{
  return _proc_edges_map.edges(pid);
}

tchecker::range_t<tchecker::system::edges_t::const_iterator_t> edges_t::edges(tchecker::process_id_t pid,
                                                                              tchecker::event_id_t event_id) const
{
  return _proc_edges_map.edges(pid, event_id);
}

bool edges_t::is_edge(tchecker::edge_id_t id) const { return id < _edges.size(); }

} // end of namespace system

} // end of namespace tchecker
