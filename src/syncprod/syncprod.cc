/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/syncprod/syncprod.hh"

namespace tchecker {

namespace syncprod {

/* Semantics functions */

tchecker::syncprod::initial_range_t initial_edges(tchecker::syncprod::system_t const & system)
{
  tchecker::process_id_t processes_count = system.processes_count();

  tchecker::syncprod::initial_iterator_t begin;
  for (tchecker::process_id_t pid = 0; pid < processes_count; ++pid)
    begin.push_back(system.initial_locations(pid));

  return tchecker::make_range(begin, tchecker::past_the_end_iterator);
}

tchecker::state_status_t initial(tchecker::syncprod::system_t const & system,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
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

tchecker::range_t<tchecker::syncprod::edges_iterator_t> outgoing_edges_iterator_t::operator*()
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
    if (involves_committed_process(*_it))
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

tchecker::syncprod::outgoing_edges_range_t
outgoing_edges(tchecker::syncprod::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
{
  tchecker::range_t<tchecker::syncprod::vloc_synchronized_edges_iterator_t, tchecker::end_iterator_t> sync_edges(
      tchecker::syncprod::outgoing_synchronized_edges(system, vloc));

  tchecker::range_t<tchecker::syncprod::vloc_asynchronous_edges_iterator_t, tchecker::end_iterator_t> async_edges(
      tchecker::syncprod::outgoing_asynchronous_edges(system, vloc));

  tchecker::syncprod::outgoing_edges_iterator_t begin(sync_edges.begin(), async_edges.begin(),
                                                      committed_processes(system, vloc));

  return tchecker::make_range(begin, tchecker::past_the_end_iterator);
}

tchecker::state_status_t next(tchecker::syncprod::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::syncprod::outgoing_edges_value_t const & edges)
{
  auto size = vloc->size();
  if (size != vedge->size())
    throw std::invalid_argument("incompatible vloc and vedge");

  for (tchecker::process_id_t pid = 0; pid < size; ++pid)
    (*vedge)[pid] = tchecker::NO_EDGE;

  for (tchecker::system::edge_const_shared_ptr_t const & edge : edges) {
    if (edge->pid() >= size)
      throw std::invalid_argument("incompatible edges");
    if ((*vloc)[edge->pid()] != edge->src())
      return tchecker::STATE_INCOMPATIBLE_EDGE;
    (*vloc)[edge->pid()] = edge->tgt();
    (*vedge)[edge->pid()] = edge->id();
  }
  return tchecker::STATE_OK;
}

boost::dynamic_bitset<> committed_processes(tchecker::syncprod::system_t const & system,
                                            tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
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

bool satisfies(tchecker::syncprod::system_t const & system, tchecker::vloc_t const & vloc,
               boost::dynamic_bitset<> const & labels)
{
  if (labels.none())
    return false;
  return labels.is_subset_of(tchecker::syncprod::labels(system, vloc));
}

bool satisfies(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t const & s,
               boost::dynamic_bitset<> const & labels)
{
  return tchecker::syncprod::satisfies(system, s.vloc(), labels);
}

/* is_initial */

bool is_initial(tchecker::syncprod::system_t const & system, tchecker::vloc_t const & vloc)
{
  for (tchecker::loc_id_t id : vloc)
    if (!system.is_initial_location(id))
      return false;
  return true;
}

/* attributes */

void attributes(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t const & s,
                std::map<std::string, std::string> & m)
{
  m["vloc"] = tchecker::to_string(s.vloc(), system.as_system_system());
}

void attributes(tchecker::syncprod::system_t const & system, tchecker::syncprod::transition_t const & t,
                std::map<std::string, std::string> & m)
{
  m["vedge"] = tchecker::to_string(t.vedge(), system.as_system_system());
}

/* syncprod_t */

syncprod_t::syncprod_t(std::shared_ptr<tchecker::syncprod::system_t const> const & system, std::size_t block_size)
    : _system(system), _state_allocator(block_size, block_size, _system->processes_count()),
      _transition_allocator(block_size, block_size, _system->processes_count())
{
}

tchecker::syncprod::initial_range_t syncprod_t::initial_edges() { return tchecker::syncprod::initial_edges(*_system); }

void syncprod_t::initial(tchecker::syncprod::initial_value_t const & init_edge, std::vector<sst_t> & v)
{
  tchecker::syncprod::state_sptr_t s = _state_allocator.construct();
  tchecker::syncprod::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::syncprod::initial(*_system, *s, *t, init_edge);
  v.push_back(std::make_tuple(status, s, t));
}

tchecker::syncprod::outgoing_edges_range_t syncprod_t::outgoing_edges(tchecker::syncprod::const_state_sptr_t const & s)
{
  return tchecker::syncprod::outgoing_edges(*_system, s->vloc_ptr());
}

void syncprod_t::next(tchecker::syncprod::const_state_sptr_t const & s,
                      tchecker::syncprod::outgoing_edges_value_t const & out_edge, std::vector<sst_t> & v)
{
  tchecker::syncprod::state_sptr_t nexts = _state_allocator.clone(*s);
  tchecker::syncprod::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::syncprod::next(*_system, *nexts, *t, out_edge);
  v.push_back(std::make_tuple(status, nexts, t));
}

bool syncprod_t::satisfies(tchecker::syncprod::const_state_sptr_t const & s, boost::dynamic_bitset<> const & labels) const
{
  return tchecker::syncprod::satisfies(*_system, *s, labels);
}

void syncprod_t::attributes(tchecker::syncprod::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const
{
  tchecker::syncprod::attributes(*_system, *s, m);
}

void syncprod_t::attributes(tchecker::syncprod::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const
{
  tchecker::syncprod::attributes(*_system, *t, m);
}

tchecker::syncprod::system_t const & syncprod_t::system() const { return *_system; }

} // end of namespace syncprod

} // end of namespace tchecker
