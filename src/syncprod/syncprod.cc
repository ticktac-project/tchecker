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

tchecker::syncprod::initial_range_t initial_states(tchecker::syncprod::system_t const & system)
{
  tchecker::process_id_t processes_count = system.processes_count();

  tchecker::syncprod::initial_iterator_t begin;
  for (tchecker::process_id_t pid = 0; pid < processes_count; ++pid)
    begin.push_back(system.initial_locations(pid));

  return tchecker::make_range(begin, tchecker::past_the_end_iterator);
}

enum tchecker::state_status_t initial(tchecker::syncprod::system_t const & system,
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

tchecker::syncprod::outgoing_edges_range_t
outgoing_edges(tchecker::syncprod::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
{
  tchecker::range_t<tchecker::syncprod::vloc_synchronized_edges_iterator_t, tchecker::end_iterator_t> sync_edges(
      tchecker::syncprod::outgoing_synchronized_edges(system, vloc));

  tchecker::range_t<tchecker::syncprod::vloc_asynchronous_edges_iterator_t, tchecker::end_iterator_t> async_edges(
      tchecker::syncprod::outgoing_asynchronous_edges(system, vloc));

  tchecker::syncprod::vloc_edges_iterator_t begin(sync_edges.begin(), async_edges.begin());

  return tchecker::make_range(begin, tchecker::past_the_end_iterator);
}

enum tchecker::state_status_t next(tchecker::syncprod::system_t const & system,
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

/* syncprod_t */

syncprod_t::syncprod_t(std::shared_ptr<tchecker::syncprod::system_t const> const & system, std::size_t block_size)
    : _system(system), _state_allocator(block_size, block_size, _system->processes_count()),
      _transition_allocator(block_size, block_size, _system->processes_count())
{
}

tchecker::syncprod::initial_range_t syncprod_t::initial_states() { return tchecker::syncprod::initial_states(*_system); }

std::tuple<enum tchecker::state_status_t, tchecker::syncprod::state_sptr_t, tchecker::syncprod::transition_sptr_t>
syncprod_t::initial(tchecker::syncprod::initial_value_t const & v)
{
  tchecker::syncprod::state_sptr_t s = _state_allocator.construct();
  tchecker::syncprod::transition_sptr_t t = _transition_allocator.construct();
  enum tchecker::state_status_t status = tchecker::syncprod::initial(*_system, *s, *t, v);
  return std::make_tuple(status, s, t);
}

tchecker::syncprod::outgoing_edges_range_t syncprod_t::outgoing_edges(tchecker::syncprod::const_state_sptr_t const & s)
{
  return tchecker::syncprod::outgoing_edges(*_system, s->vloc_ptr());
}

std::tuple<enum tchecker::state_status_t, tchecker::syncprod::state_sptr_t, tchecker::syncprod::transition_sptr_t>
syncprod_t::next(tchecker::syncprod::const_state_sptr_t const & s, tchecker::syncprod::outgoing_edges_value_t const & v)
{
  tchecker::syncprod::state_sptr_t nexts = _state_allocator.construct_from_state(*s);
  tchecker::syncprod::transition_sptr_t t = _transition_allocator.construct();
  enum tchecker::state_status_t status = tchecker::syncprod::next(*_system, *nexts, *t, v);
  return std::make_tuple(status, nexts, t);
}

tchecker::syncprod::system_t const & syncprod_t::system() const { return *_system; }

} // end of namespace syncprod

} // end of namespace tchecker
