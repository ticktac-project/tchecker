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
    
    tchecker::range_t<tchecker::syncprod::initial_iterator_t>
    initial(tchecker::syncprod::system_t const & system)
    {
      tchecker::process_id_t processes_count = system.processes_count();
      
      tchecker::syncprod::initial_iterator_t begin;
      for (tchecker::process_id_t pid = 0; pid < processes_count; ++pid)
        begin.push_back(system.initial_locations(pid));
      
      tchecker::syncprod::initial_iterator_t end(begin);
      end.to_end();
      
      return tchecker::make_range(begin, end);
    }
    
    
    enum tchecker::state_status_t
    initialize(tchecker::syncprod::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
               tchecker::syncprod::initial_iterator_value_t const & initial_range)
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
    
    
    
    tchecker::range_t<tchecker::syncprod::outgoing_edges_iterator_t>
    outgoing_edges(tchecker::syncprod::system_t const & system,
                   tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
    {
      tchecker::range_t<tchecker::syncprod::vloc_synchronized_edges_iterator_t>
      sync_edges(tchecker::syncprod::outgoing_synchronized_edges(system, vloc));
      
      tchecker::range_t<tchecker::syncprod::vloc_asynchronous_edges_iterator_t>
      async_edges(tchecker::syncprod::outgoing_asynchronous_edges(system, vloc));
      
      tchecker::syncprod::vloc_edges_iterator_t begin(sync_edges.begin(), async_edges.begin());
      tchecker::syncprod::vloc_edges_iterator_t end(sync_edges.end(), async_edges.end());
      
      return tchecker::make_range(begin, end);
    }
    
    
    
    enum tchecker::state_status_t next(tchecker::syncprod::system_t const & system,
                                       tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                       tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                       tchecker::syncprod::outgoing_edges_iterator_value_t const & edges)
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
    
    syncprod_t::syncprod_t(std::shared_ptr<tchecker::syncprod::system_t const> const & system) : _system(system)
    {}
    

    tchecker::range_t<tchecker::syncprod::initial_iterator_t> syncprod_t::initial()
    {
      return tchecker::syncprod::initial(*_system);
    }
    

    enum tchecker::state_status_t syncprod_t::initialize(tchecker::syncprod::state_t & s,
                                                         tchecker::syncprod::transition_t & t,
                                                         tchecker::syncprod::initial_iterator_value_t const & v)
    {
      return tchecker::syncprod::initialize(*_system, s.vloc_ptr(), t.vedge_ptr(), v);
    }
    

    tchecker::range_t<tchecker::syncprod::outgoing_edges_iterator_t>
    syncprod_t::outgoing_edges(tchecker::syncprod::state_t const & s)
    {
      return tchecker::syncprod::outgoing_edges(*_system, s.vloc_ptr());
    }
    
    
    enum tchecker::state_status_t syncprod_t::next
    (tchecker::syncprod::state_t & s,
     tchecker::syncprod::transition_t & t,
     tchecker::syncprod::outgoing_edges_iterator_value_t const & v)
    {
      return tchecker::syncprod::next(*_system, s.vloc_ptr(), t.vedge_ptr(), v);
    }
    
    
    tchecker::syncprod::system_t const & syncprod_t::system() const
    {
      return *_system;
    }
    
  } // end of namespace syncprod
  
} // end of namespace tchecker
