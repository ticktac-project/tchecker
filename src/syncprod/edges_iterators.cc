/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <boost/iterator/transform_iterator.hpp>

#include "tchecker/syncprod/edges_iterators.hh"

namespace tchecker {

namespace syncprod {

/* Iterator over asynchronous edges from a tuple of locations */

tchecker::range_t<tchecker::syncprod::vloc_asynchronous_edges_iterator_t, tchecker::end_iterator_t>
outgoing_asynchronous_edges(tchecker::syncprod::system_t const & system,
                            tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
{
  // iterators over ranges of outgoing asynchronous edges from vloc
  auto loc_to_async_edges = [&system](tchecker::loc_id_t loc) { return system.asynchronous_outgoing_edges(loc); };

  tchecker::syncprod::vloc_asynchronous_edges_const_iterator_t vbegin(vloc->begin(), loc_to_async_edges);
  tchecker::syncprod::vloc_asynchronous_edges_const_iterator_t vend(vloc->end(), loc_to_async_edges);

  // join iterator
  auto get_sub_range = [](tchecker::syncprod::vloc_asynchronous_edges_const_iterator_t const & it) { return *it; };

  tchecker::syncprod::vloc_asynchronous_edges_iterator_t join_begin(vbegin, vend, get_sub_range);

  return tchecker::make_range(join_begin, tchecker::past_the_end_iterator);
}

/* vloc_synchronized_edges_iterator_t */

/*!
\brief Checks if a synchronization is enabled from a tuple of locations
\param sync : a synchronization
\param vloc : tuple of locations
\param loc_edges_map : maps location ID -> edges/events
\return true if every process involved in sync has a corresponding transition from vloc according to loc_edges_maps, false
otherwise
*/
static inline bool enabled(tchecker::system::synchronization_t const & sync, tchecker::vloc_t const & vloc,
                           tchecker::system::loc_edges_maps_t const & loc_edges_maps)
{
  for (tchecker::system::sync_constraint_t const & constr : sync.synchronization_constraints()) {
    if (constr.strength() == tchecker::SYNC_STRONG && !loc_edges_maps.event(vloc[constr.pid()], constr.event_id()))
      return false;
  }
  return true;
}

vloc_synchronized_edges_iterator_t::vloc_synchronized_edges_iterator_t(
    tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc,
    std::shared_ptr<tchecker::system::loc_edges_maps_t const> const & loc_edges_maps,
    tchecker::system::synchronizations_t::const_iterator_t const & sync_begin,
    tchecker::system::synchronizations_t::const_iterator_t const & sync_end)
    : _vloc(vloc), _loc_edges_maps(loc_edges_maps), _sync_it(sync_begin), _sync_end(sync_end)
{
  advance_while_empty_cartesian_product();
}

bool vloc_synchronized_edges_iterator_t::operator==(tchecker::syncprod::vloc_synchronized_edges_iterator_t const & it) const
{
  return ((*_vloc == *it._vloc) && (_loc_edges_maps.get() == it._loc_edges_maps.get()) && (_sync_it == it._sync_it) &&
          (_sync_end == it._sync_end) && (_cartesian_it == it._cartesian_it));
}

bool vloc_synchronized_edges_iterator_t::operator!=(tchecker::syncprod::vloc_synchronized_edges_iterator_t const & it) const
{
  return (!(*this == it));
}

bool vloc_synchronized_edges_iterator_t::operator==(tchecker::end_iterator_t const & end) const { return at_end(); }

bool vloc_synchronized_edges_iterator_t::operator!=(tchecker::end_iterator_t const & end) const { return !(*this == end); }

tchecker::syncprod::vloc_synchronized_edges_iterator_t & tchecker::syncprod::vloc_synchronized_edges_iterator_t::operator++()
{
  assert(!at_end());
  ++_cartesian_it;
  if (_cartesian_it == tchecker::past_the_end_iterator) {
    ++_sync_it;
    advance_while_empty_cartesian_product();
  }
  return *this;
}

void tchecker::syncprod::vloc_synchronized_edges_iterator_t::advance_while_empty_cartesian_product()
{
  _cartesian_it.clear();

  while (!at_end()) {
    if (tchecker::syncprod::enabled(*_sync_it, *_vloc, *_loc_edges_maps))
      break;
    ++_sync_it;
  }

  if (at_end())
    return;

  auto constraints = _sync_it->synchronization_constraints();
  for (auto const & constr : constraints) {
    auto edges = _loc_edges_maps->edges((*_vloc)[constr.pid()], constr.event_id());
    if ((constr.strength() == tchecker::SYNC_WEAK) && (edges.begin() == edges.end()))
      continue;
    _cartesian_it.push_back(edges);
  }
}

tchecker::range_t<tchecker::syncprod::vloc_synchronized_edges_iterator_t, tchecker::end_iterator_t>
outgoing_synchronized_edges(tchecker::syncprod::system_t const & system,
                            tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
{
  auto syncs = system.synchronizations();
  tchecker::syncprod::vloc_synchronized_edges_iterator_t begin(vloc, system.outgoing_edges_maps(), syncs.begin(), syncs.end());

  return tchecker::make_range(begin, tchecker::past_the_end_iterator);
}

/* edges_iterator_t */

edges_iterator_t::edges_iterator_t(tchecker::system::edge_const_shared_ptr_t const & edge, bool at_end)
    : _async_edge(edge), _async_at_end(at_end)
{
  assert(edge.get() != nullptr);
}

edges_iterator_t::edges_iterator_t(tchecker::syncprod::vloc_synchronized_edges_iterator_t::edges_iterator_t const & it)
    : _async_edge(nullptr), _async_at_end(false), _sync_it(it)
{
}

bool edges_iterator_t::operator==(tchecker::syncprod::edges_iterator_t const & it) const
{
  return ((_async_edge == it._async_edge) && (_async_at_end == it._async_at_end) && (_sync_it == it._sync_it));
}

bool edges_iterator_t::operator!=(tchecker::syncprod::edges_iterator_t const & it) const { return !(*this == it); }

tchecker::system::edge_const_shared_ptr_t edges_iterator_t::operator*()
{
  if (_async_edge.get() == nullptr)
    return *_sync_it;
  return _async_edge;
}

tchecker::syncprod::edges_iterator_t & edges_iterator_t::operator++()
{
  if (_async_edge.get() == nullptr)
    ++_sync_it;
  else
    _async_at_end = true;
  return *this;
}

/* vloc_edges_iterator_t */

vloc_edges_iterator_t::vloc_edges_iterator_t(tchecker::syncprod::vloc_synchronized_edges_iterator_t const & sync_it,
                                             tchecker::syncprod::vloc_asynchronous_edges_iterator_t const & async_it)
    : _sync_it(sync_it), _async_it(async_it)
{
}

bool vloc_edges_iterator_t::operator==(tchecker::syncprod::vloc_edges_iterator_t const & it) const
{
  return ((_sync_it == it._sync_it) && (_async_it == it._async_it));
}

bool vloc_edges_iterator_t::operator!=(tchecker::syncprod::vloc_edges_iterator_t const & it) const { return !(*this == it); }

bool vloc_edges_iterator_t::operator==(tchecker::end_iterator_t const & end) const { return at_end(); }

bool vloc_edges_iterator_t::operator!=(tchecker::end_iterator_t const & end) const { return !(*this == end); }

bool vloc_edges_iterator_t::at_end() const
{
  return (_sync_it == tchecker::past_the_end_iterator && _async_it == tchecker::past_the_end_iterator);
}

tchecker::range_t<tchecker::syncprod::edges_iterator_t> vloc_edges_iterator_t::operator*()
{
  assert(!at_end());

  if (_sync_it != tchecker::past_the_end_iterator) {
    tchecker::syncprod::edges_iterator_t begin((*_sync_it).begin()), end((*_sync_it).end());
    return tchecker::make_range(begin, end);
  }

  tchecker::syncprod::edges_iterator_t begin(*_async_it, false), end(*_async_it, true);
  return tchecker::make_range(begin, end);
}

tchecker::syncprod::vloc_edges_iterator_t & vloc_edges_iterator_t::operator++()
{
  assert(!at_end());
  if (_sync_it != tchecker::past_the_end_iterator)
    ++_sync_it;
  else
    ++_async_it;
  return *this;
}

} // end of namespace syncprod

} // end of namespace tchecker
