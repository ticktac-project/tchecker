/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>

#include "tchecker/syncprod/syncprod.hh"
#include "tchecker/system/system.hh"

namespace tchecker {

namespace syncprod {

/* Initial states computation */

tchecker::syncprod::initial_range_t initial_edges(tchecker::syncprod::system_t const & system)
{
  tchecker::syncprod::initial_iterator_t begin;
  for (tchecker::process_id_t const pid : system.processes_identifiers())
    begin.push_back(system.initial_locations(pid));

  return tchecker::make_range(begin, tchecker::past_the_end_iterator);
}

tchecker::state_status_t initial(tchecker::syncprod::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                                 tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                                 tchecker::syncprod::initial_value_t const & initial_range)
{
  auto size = vloc->size();
  if (size != vedge->size())
    throw std::invalid_argument("vloc and vedge have incompatible size");

  auto begin = initial_range.begin(), end = initial_range.end();
  tchecker::process_id_t pid = 0;
  for (auto it = begin; it != end; ++it, ++pid) {
    if (pid >= size)
      throw std::invalid_argument("initial range has incompatible size");
    (*vloc)[pid] = (*it)->id();
    (*vedge)[pid] = tchecker::NO_EDGE;
  }
  if (pid != size)
    throw std::invalid_argument("initial range has incompatible size");

  sync_id = tchecker::NO_SYNC;

  return tchecker::STATE_OK;
}

/* Final state computation */

final_iterator_t::final_iterator_t(tchecker::syncprod::system_t const & system, boost::dynamic_bitset<> final_labels)
    : _system(system), _final_labels(final_labels)
{
  // Build cartesian iterator tuple of locations in system
  for (tchecker::process_id_t const pid : system.processes_identifiers())
    _it.push_back(system.locations(pid));

  advance_while_not_final();
}

bool final_iterator_t::operator==(tchecker::syncprod::final_iterator_t const & it) const
{
  return (&_system == &it._system) && (_final_labels == it._final_labels) && (_it == it._it);
}

bool final_iterator_t::operator==(tchecker::end_iterator_t const & it) const { return (_it == it); }

bool final_iterator_t::operator!=(tchecker::syncprod::final_iterator_t const & it) const { return !(*this == it); }

bool final_iterator_t::operator!=(tchecker::end_iterator_t const & it) const { return !(*this == it); }

tchecker::syncprod::final_iterator_t::value_type_t final_iterator_t::operator*() { return *_it; }

tchecker::syncprod::final_iterator_t & final_iterator_t::operator++()
{
  assert(_it != tchecker::past_the_end_iterator);
  ++_it;
  advance_while_not_final();
  return *this;
}

boost::dynamic_bitset<> final_iterator_t::locations_labels(value_type_t const & locations_range) const
{
  boost::dynamic_bitset<> locations_labels(_final_labels.size());
  locations_labels.reset();

  // Compute label set in locations_range
  for (auto && loc : locations_range)
    locations_labels |= _system.labels(loc->id());

  return locations_labels;
}

void final_iterator_t::advance_while_not_final()
{
  while (_it != tchecker::past_the_end_iterator) {
    if (_final_labels <= locations_labels(*_it))
      break;
    ++_it;
  };
}

tchecker::syncprod::final_range_t final_edges(tchecker::syncprod::system_t const & system,
                                              boost::dynamic_bitset<> const & labels)
{
  return tchecker::make_range(tchecker::syncprod::final_iterator_t{system, labels}, tchecker::past_the_end_iterator);
}

tchecker::state_status_t final(tchecker::syncprod::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                               tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                               tchecker::syncprod::final_value_t const & v)
{
  auto size = vloc->size();
  if (size != vedge->size())
    throw std::invalid_argument("tchecker::syncprod::final: vloc and vedge have incompatible size");

  auto begin = v.begin(), end = v.end();
  tchecker::process_id_t pid = 0;
  for (auto it = begin; it != end; ++it, ++pid) {
    if (pid >= size)
      throw std::invalid_argument("tchecker::syncprod::final: v has incompatible size");
    (*vloc)[pid] = (*it)->id();
    (*vedge)[pid] = tchecker::NO_EDGE;
  }
  if (pid != size)
    throw std::invalid_argument("tchecker::syncprod::final: v has incompatible size");

  sync_id = tchecker::NO_SYNC;

  return tchecker::STATE_OK;
}

/*! outgoing_edges_iterator_t */

outgoing_edges_iterator_t::outgoing_edges_iterator_t(tchecker::syncprod::vloc_synchronized_edges_iterator_t const & sync_it,
                                                     tchecker::syncprod::vloc_asynchronous_edges_iterator_t const & async_it,
                                                     boost::dynamic_bitset<> committed_processes)
    : _it(sync_it, async_it), _committed_processes(committed_processes), _committed(_committed_processes.any())
{
  advance_while_not_enabled();
}

outgoing_edges_iterator_t::outgoing_edges_iterator_t(tchecker::syncprod::vloc_edges_iterator_t const & it,
                                                     boost::dynamic_bitset<> committed_processes)
    : _it(it), _committed_processes(committed_processes), _committed(_committed_processes.any())
{
  advance_while_not_enabled();
}

bool outgoing_edges_iterator_t::operator==(tchecker::syncprod::outgoing_edges_iterator_t const & it) const
{
  return (_it == it._it && _committed_processes == it._committed_processes && _committed == it._committed);
}

bool outgoing_edges_iterator_t::operator==(tchecker::end_iterator_t const & it) const { return at_end(); }

tchecker::syncprod::outgoing_edges_iterator_t::sync_edges_t outgoing_edges_iterator_t::operator*()
{
  assert(!at_end());
  return *_it;
}

tchecker::syncprod::outgoing_edges_iterator_t & outgoing_edges_iterator_t::operator++()
{
  assert(!at_end());
  ++_it;
  advance_while_not_enabled();
  return *this;
}

void outgoing_edges_iterator_t::advance_while_not_enabled()
{
  if (!_committed)
    return;
  while (!at_end()) {
    if (involves_committed_process((*_it).edges))
      return;
    ++_it;
  }
}

bool outgoing_edges_iterator_t::involves_committed_process(
    tchecker::range_t<tchecker::syncprod::edges_iterator_t> const & r) const
{
  for (tchecker::system::edge_const_shared_ptr_t const & edge : r)
    if (_committed_processes[edge->pid()])
      return true;
  return false;
}

bool outgoing_edges_iterator_t::at_end() const { return _it == tchecker::past_the_end_iterator; }

/* outgoing edges */

tchecker::syncprod::outgoing_edges_range_t outgoing_edges(tchecker::syncprod::system_t const & system,
                                                          tchecker::const_vloc_sptr_t const & vloc)
{
  tchecker::range_t<tchecker::syncprod::vloc_synchronized_edges_iterator_t, tchecker::end_iterator_t> sync_edges(
      tchecker::syncprod::outgoing_synchronized_edges(system, vloc));

  tchecker::range_t<tchecker::syncprod::vloc_asynchronous_edges_iterator_t, tchecker::end_iterator_t> async_edges(
      tchecker::syncprod::outgoing_asynchronous_edges(system, vloc));

  tchecker::syncprod::outgoing_edges_iterator_t begin(sync_edges.begin(), async_edges.begin(),
                                                      committed_processes(system, vloc));

  return tchecker::make_range(begin, tchecker::past_the_end_iterator);
}

/* next state computation */

tchecker::state_status_t next(tchecker::syncprod::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                              tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                              tchecker::syncprod::outgoing_edges_value_t const & sync_edges)
{
  auto size = vloc->size();
  if (size != vedge->size())
    throw std::invalid_argument("incompatible vloc and vedge");

  for (tchecker::process_id_t pid = 0; pid < size; ++pid)
    (*vedge)[pid] = tchecker::NO_EDGE;

  for (tchecker::system::edge_const_shared_ptr_t const & edge : sync_edges.edges) {
    if (edge->pid() >= size)
      throw std::invalid_argument("incompatible edges");
    if ((*vloc)[edge->pid()] != edge->src())
      return tchecker::STATE_INCOMPATIBLE_EDGE;
    (*vloc)[edge->pid()] = edge->tgt();
    (*vedge)[edge->pid()] = edge->id();
  }

  sync_id = sync_edges.sync_id;

  return tchecker::STATE_OK;
}

/*! incoming_edges_iterator_t */

incoming_edges_iterator_t::incoming_edges_iterator_t(tchecker::syncprod::vloc_synchronized_edges_iterator_t const & sync_it,
                                                     tchecker::syncprod::vloc_asynchronous_edges_iterator_t const & async_it,
                                                     boost::dynamic_bitset<> const & committed_locs,
                                                     boost::dynamic_bitset<> const & committed_processes_tgt)
    : _it(sync_it, async_it), _committed_locs(committed_locs), _committed_processes_tgt(committed_processes_tgt)
{
  advance_while_not_enabled();
}

incoming_edges_iterator_t::incoming_edges_iterator_t(tchecker::syncprod::vloc_edges_iterator_t const & it,
                                                     boost::dynamic_bitset<> const & committed_locs,
                                                     boost::dynamic_bitset<> const & committed_processes_tgt)
    : _it(it), _committed_locs(committed_locs), _committed_processes_tgt(committed_processes_tgt)
{
  advance_while_not_enabled();
}

bool incoming_edges_iterator_t::operator==(tchecker::syncprod::incoming_edges_iterator_t const & it) const
{
  return (_it == it._it && _committed_locs == it._committed_locs) && (_committed_processes_tgt == it._committed_processes_tgt);
}

bool incoming_edges_iterator_t::operator==(tchecker::end_iterator_t const & it) const { return at_end(); }

tchecker::syncprod::incoming_edges_iterator_t::sync_edges_t incoming_edges_iterator_t::operator*()
{
  assert(!at_end());
  return *_it;
}

tchecker::syncprod::incoming_edges_iterator_t & incoming_edges_iterator_t::operator++()
{
  assert(!at_end());
  ++_it;
  advance_while_not_enabled();
  return *this;
}

void incoming_edges_iterator_t::advance_while_not_enabled()
{
  while (!at_end()) {
    if (enabled_wrt_committed_processes((*_it).edges))
      return;
    ++_it;
  }
}

bool incoming_edges_iterator_t::enabled_wrt_committed_processes(
    tchecker::range_t<tchecker::syncprod::edges_iterator_t> const & r) const
{
  boost::dynamic_bitset<> non_involved_committed_procs(_committed_processes_tgt);

  for (tchecker::system::edge_const_shared_ptr_t const & edge : r) {
    if (_committed_locs[edge->src()])
      return true;
    non_involved_committed_procs.reset(edge->pid());
  }

  return non_involved_committed_procs.none();
}

bool incoming_edges_iterator_t::at_end() const { return _it == tchecker::past_the_end_iterator; }

/* incoming edges */

tchecker::syncprod::incoming_edges_range_t incoming_edges(tchecker::syncprod::system_t const & system,
                                                          tchecker::const_vloc_sptr_t const & vloc)
{
  tchecker::range_t<tchecker::syncprod::vloc_synchronized_edges_iterator_t, tchecker::end_iterator_t> sync_edges(
      tchecker::syncprod::incoming_synchronized_edges(system, vloc));

  tchecker::range_t<tchecker::syncprod::vloc_asynchronous_edges_iterator_t, tchecker::end_iterator_t> async_edges(
      tchecker::syncprod::incoming_asynchronous_edges(system, vloc));

  tchecker::syncprod::incoming_edges_iterator_t begin(sync_edges.begin(), async_edges.begin(), system.committed_locations(),
                                                      committed_processes(system, vloc));

  return tchecker::make_range(begin, tchecker::past_the_end_iterator);
}

tchecker::state_status_t prev(tchecker::syncprod::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                              tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                              tchecker::syncprod::incoming_edges_value_t const & sync_edges)
{
  auto size = vloc->size();
  if (size != vedge->size())
    throw std::invalid_argument("tchecker::syncprod::prec: incompatible vloc and vedge");

  for (tchecker::process_id_t pid = 0; pid < size; ++pid)
    (*vedge)[pid] = tchecker::NO_EDGE;

  for (tchecker::system::edge_const_shared_ptr_t const & edge : sync_edges.edges) {
    if (edge->pid() >= size)
      throw std::invalid_argument("tchecker::syncprod::prec: incompatible edges");
    if ((*vloc)[edge->pid()] != edge->tgt())
      return tchecker::STATE_INCOMPATIBLE_EDGE;
    (*vloc)[edge->pid()] = edge->src();
    (*vedge)[edge->pid()] = edge->id();
  }

  sync_id = sync_edges.sync_id;

  return tchecker::STATE_OK;
}

/* computed processes */

boost::dynamic_bitset<> committed_processes(tchecker::syncprod::system_t const & system,
                                            tchecker::const_vloc_sptr_t const & vloc)
{
  boost::dynamic_bitset<> committed(system.processes_count());
  committed.reset();
  for (tchecker::loc_id_t id : *vloc)
    if (system.is_committed(id))
      committed[system.location(id)->pid()] = 1;
  return committed;
}

/* labels */

boost::dynamic_bitset<> labels(tchecker::syncprod::system_t const & system, tchecker::vloc_t const & vloc)
{
  boost::dynamic_bitset<> l(system.labels_count());
  for (tchecker::loc_id_t loc_id : vloc)
    l |= system.labels(loc_id);
  return l;
}

boost::dynamic_bitset<> labels(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t const & s)
{
  return tchecker::syncprod::labels(system, s.vloc());
}

std::string labels_str(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t const & s)
{
  std::stringstream ss;
  boost::dynamic_bitset<> slabels = tchecker::syncprod::labels(system, s);
  std::size_t const first = slabels.find_first();
  for (std::size_t i = first; i != boost::dynamic_bitset<>::npos; i = slabels.find_next(i)) {
    if (i != first)
      ss << ",";
    ss << system.label_name(i);
  }
  return ss.str();
}

/* is_valid_final */

bool is_valid_final(tchecker::syncprod::system_t const & system, tchecker::vloc_t const & vloc) { return true; }

bool is_valid_final(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t const & s)
{
  return tchecker::syncprod::is_valid_final(system, s.vloc());
}

/* is_initial */

bool is_initial(tchecker::syncprod::system_t const & system, tchecker::vloc_t const & vloc)
{
  for (tchecker::loc_id_t id : vloc)
    if (!system.is_initial_location(id))
      return false;
  return true;
}

bool is_initial(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t const & s)
{
  return tchecker::syncprod::is_initial(system, s.vloc());
}

/* attributes */

void attributes(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t const & s,
                std::map<std::string, std::string> & m)
{
  m["vloc"] = tchecker::to_string(s.vloc(), system.as_system_system());
  m["labels"] = tchecker::syncprod::labels_str(system, s);
}

void attributes(tchecker::syncprod::system_t const & system, tchecker::syncprod::transition_t const & t,
                std::map<std::string, std::string> & m)
{
  m["vedge"] = tchecker::to_string(t.vedge(), system.as_system_system());
  m["sync"] = (t.sync_id() == tchecker::NO_SYNC ? "" : tchecker::to_string(t.sync_id(), system.as_system_system()));
}

/* initialize */

tchecker::state_status_t initialize(tchecker::syncprod::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                                    tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                                    std::map<std::string, std::string> const & attributes)
{
  try {
    tchecker::from_string(*vloc, system.as_system_system(), attributes.at("vloc"));
    for (tchecker::process_id_t const pid : system.processes_identifiers())
      (*vedge)[pid] = tchecker::NO_EDGE;
    sync_id = tchecker::NO_SYNC;
    return tchecker::STATE_OK;
  }
  catch (...) {
    return tchecker::STATE_BAD;
  }
}

/* syncprod_t */

syncprod_t::syncprod_t(std::shared_ptr<tchecker::syncprod::system_t const> const & system,
                       enum tchecker::ts::sharing_type_t sharing_type, std::size_t block_size, std::size_t table_size)
    : _system(system), _sharing_type(sharing_type),
      _state_allocator(block_size, block_size, _system->processes_count(), table_size),
      _transition_allocator(block_size, block_size, _system->processes_count(), table_size)
{
}

// Forward

tchecker::syncprod::initial_range_t syncprod_t::initial_edges() { return tchecker::syncprod::initial_edges(*_system); }

void syncprod_t::initial(tchecker::syncprod::initial_value_t const & init_edge, std::vector<sst_t> & v,
                         tchecker::state_status_t mask)
{
  tchecker::syncprod::state_sptr_t s = _state_allocator.construct();
  tchecker::syncprod::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::syncprod::initial(*_system, *s, *t, init_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}

void syncprod_t::initial(std::vector<sst_t> & v, tchecker::state_status_t mask) { tchecker::ts::initial(*this, v, mask); }

tchecker::syncprod::outgoing_edges_range_t syncprod_t::outgoing_edges(tchecker::syncprod::const_state_sptr_t const & s)
{
  return tchecker::syncprod::outgoing_edges(*_system, s->vloc_ptr());
}

void syncprod_t::next(tchecker::syncprod::const_state_sptr_t const & s,
                      tchecker::syncprod::outgoing_edges_value_t const & out_edge, std::vector<sst_t> & v,
                      tchecker::state_status_t mask)
{
  tchecker::syncprod::state_sptr_t nexts = _state_allocator.clone(*s);
  tchecker::syncprod::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::syncprod::next(*_system, *nexts, *t, out_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(nexts);
      share(t);
    }
    v.push_back(std::make_tuple(status, nexts, t));
  }
}

void syncprod_t::next(tchecker::syncprod::const_state_sptr_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::next(*this, s, v, mask);
}

// Backward

final_range_t syncprod_t::final_edges(boost::dynamic_bitset<> const & labels)
{
  return tchecker::syncprod::final_edges(*_system, labels);
}

void syncprod_t::final(final_value_t const & final_edge, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::syncprod::state_sptr_t s = _state_allocator.construct();
  tchecker::syncprod::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::syncprod::final(*_system, *s, *t, final_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}

void syncprod_t::final(boost::dynamic_bitset<> const & labels, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::final(*this, labels, v, mask);
}

incoming_edges_range_t syncprod_t::incoming_edges(tchecker::syncprod::const_state_sptr_t const & s)
{
  return tchecker::syncprod::incoming_edges(*_system, s->vloc_ptr());
}

void syncprod_t::prev(tchecker::syncprod::const_state_sptr_t const & s, incoming_edges_value_t const & in_edge,
                      std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::syncprod::state_sptr_t prevs = _state_allocator.clone(*s);
  tchecker::syncprod::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::syncprod::prev(*_system, *prevs, *t, in_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(prevs);
      share(t);
    }
    v.push_back(std::make_tuple(status, prevs, t));
  }
}

void syncprod_t::prev(tchecker::syncprod::const_state_sptr_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::prev(*this, s, v, mask);
}

// Builder

void syncprod_t::build(std::map<std::string, std::string> const & attributes, std::vector<sst_t> & v,
                       tchecker::state_status_t mask)
{
  tchecker::syncprod::state_sptr_t s = _state_allocator.construct();
  tchecker::syncprod::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::syncprod::initialize(*_system, *s, *t, attributes);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}

// Inspector

boost::dynamic_bitset<> syncprod_t::labels(tchecker::syncprod::const_state_sptr_t const & s) const
{
  return tchecker::syncprod::labels(*_system, *s);
}

void syncprod_t::attributes(tchecker::syncprod::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const
{
  tchecker::syncprod::attributes(*_system, *s, m);
}

void syncprod_t::attributes(tchecker::syncprod::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const
{
  tchecker::syncprod::attributes(*_system, *t, m);
}

bool syncprod_t::is_valid_final(tchecker::syncprod::const_state_sptr_t const & s) const
{
  return tchecker::syncprod::is_valid_final(*_system, *s);
}

bool syncprod_t::is_initial(tchecker::syncprod::const_state_sptr_t const & s) const
{
  return tchecker::syncprod::is_initial(*_system, *s);
}

// Sharing

void syncprod_t::share(tchecker::syncprod::state_sptr_t & s) { _state_allocator.share(s); }

void syncprod_t::share(tchecker::syncprod::transition_sptr_t & t) { _transition_allocator.share(t); }

} // end of namespace syncprod

} // end of namespace tchecker
