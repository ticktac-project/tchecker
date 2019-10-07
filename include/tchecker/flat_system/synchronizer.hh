/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYNCHRONIZER_HH
#define TCHECKER_SYNCHRONIZER_HH

#include <functional>
#include <vector>

#include <boost/container/flat_map.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "tchecker/flat_system/vedge.hh"
#include "tchecker/flat_system/vloc.hh"
#include "tchecker/system/system.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file synchronizer.hh
 \brief Synchronization product of processes
 */

namespace tchecker {
  
  namespace details {
    
    /*!
     \class location_edges_map_t
     \brief Map: location ID -> edges (multimap)
     */
    template <class EDGE>
    class location_edges_map_t {
      /*!< Type of collection of edges */
      using edge_collection_t = std::vector<EDGE const *>;
      
      /*!< Type of map: location ID -> collection of edges */
      using map_t = std::vector<edge_collection_t>;
    public:
      /*!
       \brief Constructor
       \param locations_count : number of locations
       \note the domain of the map is [0..locations_count)
       */
      explicit location_edges_map_t(std::size_t locations_count)
      : _locations_count(locations_count),
      _map(_locations_count, edge_collection_t())
      {}
      
      /*!
       \brief Copy constructor
       \param m : location to edges map
       \post this is a copy of m
       */
      location_edges_map_t(tchecker::details::location_edges_map_t<EDGE> const & m) = default;
      
      /*!
       \brief Move constructor
       \param m : location to edges map
       \post m has been moved to this
       */
      location_edges_map_t(tchecker::details::location_edges_map_t<EDGE> && m) = default;
      
      /*!
       \brief Destructor
       */
      ~location_edges_map_t() = default;
      
      /*!
       \brief Assignment operator
       \param m : location to edges map
       \post this is a copy of m
       */
      tchecker::details::location_edges_map_t<EDGE> & operator= (tchecker::details::location_edges_map_t<EDGE> const & m) = default;
      
      /*!
       \brief Move assignment operator
       \param m : location to edges map
       \post m has been moved to this
       */
      tchecker::details::location_edges_map_t<EDGE> & operator= (tchecker::details::location_edges_map_t<EDGE> && m) = default;
      
      /*!
       \brief Add to the map
       \param loc_id : location ID
       \param edge : edge
       \pre 0 <= loc_id < locations count, and edge != nullptr
       (checked by assertion)
       \post edge has been added to the set of edges that are mapped to loc_id
       */
      void add(tchecker::loc_id_t loc_id, EDGE const * edge)
      {
        assert( loc_id < _locations_count );
        assert( edge != nullptr );
        _map[loc_id].push_back(edge);
      }
      
      /*!
       \brief Erase from the map
       \param loc_id : location ID
       \param edge : edge
       \pre 0 <= loc_id < locations count, and edge != nullptr
       (checked by assertion)
       \post edge has been erased from the set of edges that are mapped to loc_id
       */
      void erase(tchecker::loc_id_t loc_id, EDGE const * edge)
      {
        assert( loc_id < _locations_count );
        assert( edge != nullptr );
        _map[loc_id].erase(edge);
      }
      
      /*!
       \brief Accessor
       \return locations count
       \note the domain of the map is [0..locations count)
       */
      inline std::size_t locations_count() const
      {
        return _locations_count;
      }
      
      /*!
       \brief Type of edge iterator
       */
      using edge_collection_const_iterator_t = typename edge_collection_t::const_iterator;
      
      /*!
       \brief Accessor
       \param loc_id : location identifier
       \pre 0 <= loc_id < locations_count (checked by assertion)
       \return range over the collection of edges that are mapped to loc_id
       \note constant-time complexity
       */
      tchecker::range_t<edge_collection_const_iterator_t> edges(tchecker::loc_id_t loc_id) const
      {
        assert( loc_id < _locations_count );
        return tchecker::make_range(_map[loc_id].begin(), _map[loc_id].end());
      }
    protected:
      tchecker::loc_id_t _locations_count;  /*!< Locations count (domain) */
      map_t _map;                           /*!< Map */
    };
    
    
    
    /*!
     \brief Type of edge iterator in locations
     \tparam VLOC : type of tuple of locations
     */
    template <class VLOC>
    using loc_edge_iterator_t = typename VLOC::loc_t::const_edge_iterator_t;
    
    /*!
     \brief Type of edge range in locations
     \tparam VLOC : type of tuple of locations
     */
    template <class VLOC>
    using loc_edge_range_t = tchecker::range_t<loc_edge_iterator_t<VLOC>>;
    
    
    
    /*!
     \class synchronous_edges_iterator_t
     \brief Iterator over synchronous edges from a tuple of locations
     \tparam VLOC : type of tuple of location
     \tparam EDGE : type of edges
     \tparam SYNC_ITERATOR : type of iterator over synchronizations
     */
    template <class VLOC, class EDGE, class SYNC_ITERATOR>
    class synchronous_edges_iterator_t {
    public:
      /*!
       \brief Constructor
       \param vloc : tuple of locations
       \param loc_event_edges : map location ID * event ID -> edges
       \param sync_begin : iterator on first synchronization
       \param sync_end : past-the-end iterator on synchronizations
       \note this keeps a pointer to vloc and a pointer to loc_event_edges
       */
      synchronous_edges_iterator_t
      (VLOC const & vloc,
       loc_edge_range_t<VLOC> (*loc_event_edges) (typename VLOC::loc_t const *, tchecker::event_id_t),
       SYNC_ITERATOR const & sync_begin,
       SYNC_ITERATOR const & sync_end)
      : _vloc(&vloc),
      _loc_event_edges(loc_event_edges),
      _sync_it(sync_begin),
      _sync_end(sync_end)
      {
        advance_while_empty_cartesian_product();
      }
      
      /*!
       \brief Copy constructor
       \param it : synchronous edges iterator
       \post this is a copy of it
       */
      synchronous_edges_iterator_t(tchecker::details::synchronous_edges_iterator_t<VLOC, EDGE, SYNC_ITERATOR> const &) = default;
      
      /*!
       \brief Move constructor
       \param it : synchronous edges iterator
       \post it has been moved to this
       */
      synchronous_edges_iterator_t(tchecker::details::synchronous_edges_iterator_t<VLOC, EDGE, SYNC_ITERATOR> &&) = default;
      
      /*!
       \brief Destructor
       */
      ~synchronous_edges_iterator_t() = default;
      // NB: DO NOT DESTRUCT _vloc WHICH IS A REFERENCE (see constructor)
      
      /*!
       \brief Assignment operator
       \param it : synchronous edges iterator
       \post this is a copy of it
       */
      tchecker::details::synchronous_edges_iterator_t<VLOC, EDGE, SYNC_ITERATOR> &
      operator= (tchecker::details::synchronous_edges_iterator_t<VLOC, EDGE, SYNC_ITERATOR> const &) = default;
      
      /*!
       \brief Move assignment operator
       \param it : synchronous edges iterator
       \post it has been moved to this
       */
      tchecker::details::synchronous_edges_iterator_t<VLOC, EDGE, SYNC_ITERATOR> &
      operator= (tchecker::details::synchronous_edges_iterator_t<VLOC, EDGE, SYNC_ITERATOR> &&) = default;
      
      /*!
       \brief Equality check
       \param it : synchronous edges iterator
       \return true if this and it are equal, false otherwise
       */
      bool operator== (tchecker::details::synchronous_edges_iterator_t<VLOC, EDGE, SYNC_ITERATOR> const & it) const
      {
        return ((_vloc == it._vloc) &&
                (_loc_event_edges == it._loc_event_edges) &&
                (_sync_it == it._sync_it) &&
                (_sync_end == it._sync_end) &&
                (_cartesian_it == it._cartesian_it));
      }
      
      /*!
       \brief Disequality check
       \param it : synchronous edges iterator
       \return true if this and it are different, false otherwise
       */
      bool operator!= (tchecker::details::synchronous_edges_iterator_t<VLOC, EDGE, SYNC_ITERATOR> const & it) const
      {
        return (! (*this == it));
      }
      
      /*!
       \brief Fast end-of-range check
       \return true if this is past-the-end, false otherwise
       \note this is more efficient that checking disequality w.r.t. past-the-end
       iterator
       */
      bool at_end() const
      {
        return (_sync_it == _sync_end);
      }
      
      /*!
       \brief Iterator over synchronized edges
       */
      using synchronized_edges_iterator_t =
      typename tchecker::cartesian_iterator_t<loc_edge_iterator_t<VLOC>>::values_iterator_t;
      
      /*!
       \brief Accessor
       \pre not at_end()  (checked by assertion)
       \return current synchronous edge
       \note the returned range is invalidated by operator++
       \note for returned range, checking begin().at_end() is more efficient than
       checking begin()==end()
       */
      tchecker::range_t<synchronized_edges_iterator_t> operator*()
      {
        assert( ! at_end() );
        return (_cartesian_it.operator*());
      }
      
      /*!
       \brief Increment (next)
       \pre not at_end()  (checked by assertion)
       \post this points to the next element (if any)
       \return this after incrementation
       \note operator++ invalidates all ranges returned by operator*
       */
      tchecker::details::synchronous_edges_iterator_t<VLOC, EDGE, SYNC_ITERATOR> & operator++ ()
      {
        assert( ! at_end() );
        ++ _cartesian_it;
        if (_cartesian_it.at_end()) {
          ++ _sync_it;
          advance_while_empty_cartesian_product();
        }
        return *this;
      }
    protected:
      /*!
       \brief Fills cartesian product
       \post either this range is at_end(), or _cartesian_it has been filled
       with ranges of edges corresponding to synchronzation pointed by _sync_it
       */
      void advance_while_empty_cartesian_product()
      {
        while (!at_end() && _cartesian_it.at_end()) {
          _cartesian_it.clear();
          auto constraints = _sync_it->synchronization_constraints();
          for (auto const & constr : constraints) {
            auto edges = _loc_event_edges((*_vloc)[constr.pid()], constr.event_id());
            if ((constr.strength() == tchecker::SYNC_WEAK) && (edges.begin() == edges.end()))
              continue;
            _cartesian_it.push_back(edges);
          }
        }
      }
      
      
      VLOC const * _vloc;                                                       /*!< Tuple of locations */
      loc_edge_range_t<VLOC> (*_loc_event_edges) (typename VLOC::loc_t const *,
                                                  tchecker::event_id_t);        /*!< Map: (location, event) to edges */
      SYNC_ITERATOR _sync_it;                                                   /*!< Iterator on first synchronization */
      SYNC_ITERATOR _sync_end;                                                  /*!< Past-the-end iterator on synchronizations */
      tchecker::cartesian_iterator_t<loc_edge_iterator_t<VLOC>> _cartesian_it;  /*!< Cartesian iterator */
    };
    
    
    
    
    /*!
     \brief Type of enabled synchronizations filter
     */
    using enabled_sync_filter_t = std::function<bool(tchecker::synchronization_t const &)>;
    
    /*!
     \brief Type of iterator over enabled synchronizations
     */
    template <class SYSTEM>
    using enabled_syncs_iterator_t = boost::filter_iterator<enabled_sync_filter_t, tchecker::const_sync_iterator_t>;
    
    /*!
     \brief Type of iterator over location's asynchronous edges
     */
    template <class SYSTEM>
    using location_asynchronous_edges_iterator_t =
    typename tchecker::details::location_edges_map_t<typename SYSTEM::edge_t>::edge_collection_const_iterator_t;
    
    /*!
     \brief Type of map: location -> asynchronous edges
     */
    template <class SYSTEM, class VLOC>
    using location_to_asynchronous_edges_map_t =
    std::function<tchecker::range_t<location_asynchronous_edges_iterator_t<SYSTEM>>(typename VLOC::loc_t const *)>;
    
    /*!
     \brief Type of iterator over asynchronous edges
     */
    template <class SYSTEM, class VLOC>
    using asynchronous_edges_iterator_t =
    tchecker::join_iterator_t
    <boost::transform_iterator<location_to_asynchronous_edges_map_t<SYSTEM, VLOC>, typename VLOC::const_iterator_t>>;
    
    
    
    
    /*!
     \class synchronizer_iterator_t
     \tparam SYSTEM : type of system
     \tparam VLOC : type of tuple of locations
     \brief Iterator over synchronous and asynchrounous edges from a tuple of
     locations
     */
    template <class SYSTEM, class VLOC>
    class synchronizer_iterator_t {
      /*!
       \brief Type of iterator over synchronous edges
       */
      using sync_edges_iterator_t
      = tchecker::details::synchronous_edges_iterator_t
      <VLOC, typename SYSTEM::edge_t, tchecker::details::enabled_syncs_iterator_t<SYSTEM>>;
      
      /*!
       \brief Type of iterator over asynchonous edges
       */
      using async_edges_iterator_t = tchecker::details::asynchronous_edges_iterator_t<SYSTEM, VLOC>;
    public:
      /*!
       \brief Constructor
       \param sync_it : iterator over synchronous edges
       \param async_it : iterator over asynchronous edges
       */
      synchronizer_iterator_t(sync_edges_iterator_t const & sync_it, async_edges_iterator_t const & async_it)
      : _sync_it(sync_it), _async_it(async_it)
      {}
      
      /*!
       \brief Copy constructor
       */
      synchronizer_iterator_t(tchecker::details::synchronizer_iterator_t<SYSTEM, VLOC> const &) = default;
      
      /*!
       \brief Move constructor
       */
      synchronizer_iterator_t(tchecker::details::synchronizer_iterator_t<SYSTEM, VLOC> &&) = default;
      
      /*!
       \brief Destructor
       */
      ~synchronizer_iterator_t() = default;
      
      /*!
       \brief Assignment operator
       */
      tchecker::details::synchronizer_iterator_t<SYSTEM, VLOC> &
      operator= (tchecker::details::synchronizer_iterator_t<SYSTEM, VLOC> const &) = default;
      
      /*!
       \brief Move assignment operator
       */
      tchecker::details::synchronizer_iterator_t<SYSTEM, VLOC> &
      operator= (tchecker::details::synchronizer_iterator_t<SYSTEM, VLOC> &&) = default;
      
      /*!
       \brief Equality check
       \param it : iterator
       \return true if this and it point to the same synchronous and
       asynchronous iterators, false otherwise
       */
      bool operator== (tchecker::details::synchronizer_iterator_t<SYSTEM, VLOC> const & it) const
      {
        return ((_sync_it == it._sync_it) && (_async_it == it._async_it));
      }
      
      /*!
       \brief Disequality check
       \param it : iterator
       \return false if this and it point to the same synchronous and
       asynchronous iterators, true otherwise
       */
      bool operator!= (tchecker::details::synchronizer_iterator_t<SYSTEM, VLOC> const & it) const
      {
        return !(*this == it);
      }
      
      /*!
       \brief Accessor
       \return true if this is at the end of the range, false otherwise
       \note using at_end() is more efficient than equality check w.r.t.
       past-the-end iterator
       */
      bool at_end() const
      {
        return (_sync_it.at_end() && _async_it.at_end());
      }
      
      /*!
       \brief Type of iterator over tuples of edges
       */
      using vedge_iterator_t =
      tchecker::vedge_iterator_t<typename SYSTEM::edge_t, typename sync_edges_iterator_t::synchronized_edges_iterator_t>;
      
      /*!
       \brief Accessor
       \pre not at_end() (checked by assertion)
       \return Range of iterator over tuple of edges pointed by this
       \note return range is invalidated by operator++
       */
      tchecker::range_t<vedge_iterator_t> operator* ()
      {
        assert( ! at_end() );
        
        if (! _sync_it.at_end()) {
          vedge_iterator_t begin((*_sync_it).begin()), end((*_sync_it).end());
          return tchecker::make_range(begin, end);
        }
        
        vedge_iterator_t begin(*_async_it), end(*_async_it + 1);
        return tchecker::make_range(begin, end);
      }
      
      /*!
       \brief Move to next
       \pre not at_end() (checked by assertion)
       \post this points to next tuple of edges (if any)
       \return this after increment
       \note invaldates ranges returned by operator*
       */
      tchecker::details::synchronizer_iterator_t<SYSTEM, VLOC> & operator++ ()
      {
        assert( ! at_end() );
        if (! _sync_it.at_end())
          ++ _sync_it;
        else
          ++ _async_it;
        return *this;
      }
    private:
      sync_edges_iterator_t _sync_it;   /*!< Iterator on synchronous edges */
      async_edges_iterator_t _async_it; /*!< Iterator on asynchronous edges */
    };
    
    
  } // end of namespace details
  
  
  
  
  
  
  /*!
   \class synchronizer_t
   \tparam SYSTEM : type of system, must inherit from tchecker::system_t
   \brief Compute synchronous and asynchronous edges in a system
   */
  template <class SYSTEM>
  class synchronizer_t {
    static_assert(std::is_base_of<tchecker::system_t<typename SYSTEM::loc_t, typename SYSTEM::edge_t>, SYSTEM>::value, "");
    
    /*!
     \brief Type of iterator over enabled synchronizations
     */
    using enabled_syncs_iterator_t = tchecker::details::enabled_syncs_iterator_t<SYSTEM>;
    
    /*!
     \brief Type of iterator of synchronous edges
     */
    template <class VLOC>
    using synchronous_edges_iterator_t =
    tchecker::details::synchronous_edges_iterator_t<VLOC, typename SYSTEM::edge_t, enabled_syncs_iterator_t>;
    
    /*!
     \brief Type of iterator over asynchronous edges
     */
    template <class VLOC>
    using asynchronous_edges_iterator_t = tchecker::details::asynchronous_edges_iterator_t<SYSTEM, VLOC>;
    
    /*!
     \brief Type of iterator over location's asynchronous edges
     */
    using location_asynchronous_edges_iterator_t = tchecker::details::location_asynchronous_edges_iterator_t<SYSTEM>;
    
    /*!
     \brief Type of map: location -> asynchronous edges
     */
    template <class VLOC>
    using location_to_asynchronous_edges_map_t = tchecker::details::location_to_asynchronous_edges_map_t<SYSTEM, VLOC>;
    
  public:
    /*!
     \brief Type of iterator
     */
    template <class VLOC>
    using iterator_t = tchecker::details::synchronizer_iterator_t<SYSTEM, VLOC>;
    
    /*!
     \brief Constructor
     \param system : system of processes
     \note this stores a reference to system, and pointers to edges in
     system
     */
    explicit synchronizer_t(SYSTEM const & system)
    : _system(system),
    _asynchronous_outgoing_edges_map(system.locations_count()),
    _asynchronous_incoming_edges_map(system.locations_count())
    {
      auto edges = _system.edges();
      for (typename SYSTEM::edge_t const * edge : edges) {
        if (_system.asynchronous(edge->pid(), edge->event_id())) {
          _asynchronous_outgoing_edges_map.add(edge->src()->id(), edge);
          _asynchronous_incoming_edges_map.add(edge->tgt()->id(), edge);
        }
      }
    }
    
    /*!
     \brief Copy constructor
     */
    synchronizer_t(tchecker::synchronizer_t<SYSTEM> const &) = default;
    
    /*!
     \brief Move constructor
     */
    synchronizer_t(tchecker::synchronizer_t<SYSTEM> &&) = default;
    
    /*!
     \brief Destructor
     */
    ~synchronizer_t() = default;
    // NB: do not delete edges in maps (references)
    
    /*!
     \brief Assignment operator
     */
    tchecker::synchronizer_t<SYSTEM> & operator= (tchecker::synchronizer_t<SYSTEM> const &) = default;
    
    /*!
     \brief Move-assignment operator
     */
    tchecker::synchronizer_t<SYSTEM> & operator= (tchecker::synchronizer_t<SYSTEM> &&) = default;
    
    /*!
     \brief Accessor
     \param vloc : tuple of locations
     \return range of synchronous and asynchronous outgoing edges from vloc
     \note Using method at_end() is more efficient than checking equality of
     iterators for returned range
     \note operator++ invalidates the ranges returned by operator* of iterators
     for returned range
     */
    template <class VLOC>
    tchecker::range_t<iterator_t<VLOC>> outgoing_edges(VLOC const & vloc) const
    {
      return edges(vloc, loc_outgoing_event<VLOC>, loc_event_outgoing_edges<VLOC>, _asynchronous_outgoing_edges_map);
    }
    
    
    /*!
     \brief Accessor
     \param vloc : tuple of locations
     \return range of synchronous and asynchronous incoming edges to vloc
     \note Using method at_end() is more efficient than checking equality of
     iterators for returned range
     \note operator++ invalidates the ranges returned by operator* of iterators
     for returned range
     */
    template <class VLOC>
    tchecker::range_t<iterator_t<VLOC>> incoming_edges(VLOC const & vloc) const
    {
      return edges(vloc, loc_incoming_event<VLOC>, loc_event_incoming_edges<VLOC>, _asynchronous_incoming_edges_map);
    }
  protected:
    /*!
     \brief Accessor
     \param loc : location
     \param event_id : event ID
     \return true if loc has an outgoing edge with event_id, false otherwise
     */
    template <class VLOC>
    static inline bool loc_outgoing_event(typename VLOC::loc_t const * loc, tchecker::event_id_t event_id)
    {
      assert( loc != nullptr );
      return loc->outgoing_event(event_id);
    }
    
    /*!
     \brief Accessor
     \param loc : location
     \param event_id : event ID
     \return true if loc has an incoming edge with event_id, false otherwise
     */
    template <class VLOC>
    static inline bool loc_incoming_event(typename VLOC::loc_t const * loc, tchecker::event_id_t event_id)
    {
      assert( loc != nullptr );
      return loc->incoming_event(event_id);
    }
    
    /*!
     \brief Accessor
     \param loc : location
     \param event_id : event ID
     \return range of outgoing edges with event_id from loc
     \throw std::invalid_argument : if loc has no outgoing edge with event_id
     */
    template <class VLOC>
    static inline tchecker::range_t<tchecker::details::loc_edge_iterator_t<VLOC>>
    loc_event_outgoing_edges(typename VLOC::loc_t const * loc, tchecker::event_id_t event_id)
    {
      return loc->outgoing_edges(event_id);
    }
    
    /*!
     \brief Accessor
     \param loc : location
     \param event_id : event ID
     \return range of incoming edges with event_id to loc
     \throw std::invalid_argument : if loc has no incoming edge with event_id
     */
    template <class VLOC>
    static inline tchecker::range_t<tchecker::details::loc_edge_iterator_t<VLOC>>
    loc_event_incoming_edges(typename VLOC::loc_t const * loc, tchecker::event_id_t event_id)
    {
      return loc->incoming_edges(event_id);
    }
    
    /*!
     \brief Filter enabled synchronizations
     \tparam VLOC : type of tuple of locations
     \param vloc : tuple of locations
     \param begin : iterator on first synchronization
     \param end : past-the-end iterator on synchronizations
     \param loc_has_event : map location ID * event ID -> bool
     \return Range of synchronizations in [begin..end) that are enabled in vloc
     w.r.t. loc_has_event
     */
    template <class VLOC>
    tchecker::range_t<enabled_syncs_iterator_t>
    filter_enabled_syncs(VLOC const & vloc,
                         tchecker::const_sync_iterator_t const & begin,
                         tchecker::const_sync_iterator_t const & end,
                         bool (*loc_has_event) (typename VLOC::loc_t const *, tchecker::event_id_t)) const
    {
      auto enabled_sync = [&vloc, loc_has_event] (tchecker::synchronization_t const & sync) -> bool
      {
        bool all_weak_disabled = true;
        auto constraints = sync.synchronization_constraints();
        for (auto const & constr : constraints) {
          bool weak = (constr.strength() == tchecker::SYNC_WEAK);
          bool event = loc_has_event(vloc[constr.pid()], constr.event_id());
          all_weak_disabled &= (weak && !event);
          
          if (!weak && !event)
            return false;
        }
        return !all_weak_disabled;
      };
      
      enabled_syncs_iterator_t enabled_syncs_begin(enabled_sync, begin, end), enabled_syncs_end(enabled_sync, end, end);
      
      return tchecker::make_range(enabled_syncs_begin, enabled_syncs_end);
    }
    
    /*!
     \brief Accessor
     \tparam VLOC : type of tuples of locations
     \param vloc : tuple of locations
     \param loc_has_event : map location * event ID -> bool
     \param loc_event_edges : map location * event ID -> edges
     \return Range of enabled synchronous edges in vloc w.r.t. loc_has_event
     ans loc_event_edges
     \note Using method at_end() is more efficient than checking equality of
     iterators for returned range
     */
    template <class VLOC>
    tchecker::range_t<synchronous_edges_iterator_t<VLOC>> synchronous_edges
    (VLOC const & vloc,
     bool (*loc_has_event) (typename VLOC::loc_t const *, tchecker::event_id_t),
     tchecker::details::loc_edge_range_t<VLOC> (*loc_event_edges) (typename VLOC::loc_t const *, tchecker::event_id_t)) const
    {
      auto syncs = _system.synchronizations();
      
      auto enabled_syncs = filter_enabled_syncs<VLOC>(vloc, syncs.begin(), syncs.end(), loc_has_event);
      
      synchronous_edges_iterator_t<VLOC>
      begin(vloc, loc_event_edges, enabled_syncs.begin(), enabled_syncs.end()),
      end(vloc, loc_event_edges, enabled_syncs.end(), enabled_syncs.end());
      
      return tchecker::make_range(begin, end);
    }
    
    /*!
     \brief Accessor
     \tparam VLOC : type of tuple of locations
     \param vloc : tuple of locations
     \param loc_edges_map : map location ID -> edges
     \return Range of asynchronous edges in vloc according to edges in
     loc_edges_map
     */
    template <class VLOC>
    tchecker::range_t<asynchronous_edges_iterator_t<VLOC>>
    asynchronous_edges(VLOC const & vloc,
                       tchecker::details::location_edges_map_t<typename SYSTEM::edge_t> const & loc_edges_map) const
    {
      auto loc_edges = [&] (typename VLOC::loc_t const * loc) {
        return loc_edges_map.edges(loc->id());
      };
      
      boost::transform_iterator<location_to_asynchronous_edges_map_t<VLOC>, typename VLOC::const_iterator_t>
      loc_edges_begin(vloc.begin(), loc_edges),
      loc_edges_end(vloc.end(), loc_edges);
      
      auto get_sub_range = [] (typename asynchronous_edges_iterator_t<VLOC>::iterator_t const & it) {
        return it->iterators();
      };
      
      asynchronous_edges_iterator_t<VLOC>
      begin(loc_edges_begin, loc_edges_end, get_sub_range),
      end(loc_edges_end, loc_edges_end, get_sub_range);
      
      return tchecker::make_range(begin, end);
    }
    
    /*!
     \brief Accessor
     \param vloc : tuple of locations
     \param loc_has_event : map location * event ID -> bool
     \param loc_event_edges : map location * event ID -> eges
     \param loc_edges_map : map location ID -> edges
     \return range of enabled vedges on vloc according to loc_has_event,
     loc_event_edges and loc_edges_map
     \note Using method at_end() is more efficient than checking equality of
     iterators for returned range
     */
    template <class VLOC>
    tchecker::range_t<iterator_t<VLOC>>
    edges(VLOC const & vloc,
          bool (*loc_has_event) (typename VLOC::loc_t const *, tchecker::event_id_t),
          tchecker::details::loc_edge_range_t<VLOC> (*loc_event_edges) (typename VLOC::loc_t const *, tchecker::event_id_t),
          tchecker::details::location_edges_map_t<typename SYSTEM::edge_t> const & loc_edges_map) const
    {
      auto sync_edges = synchronous_edges<VLOC>(vloc, loc_has_event, loc_event_edges);
      
      auto async_edges = asynchronous_edges<VLOC>(vloc, loc_edges_map);
      
      iterator_t<VLOC> begin(sync_edges.begin(), async_edges.begin()), end(sync_edges.end(), async_edges.end());
      
      return tchecker::make_range(begin, end);
    }
    
    
    SYSTEM const & _system;                                /*!< System of processes */
    tchecker::details::location_edges_map_t<typename SYSTEM::edge_t>
    _asynchronous_outgoing_edges_map;                      /*!< Map: location ID -> asynchronous outgoing edges */
    tchecker::details::location_edges_map_t<typename SYSTEM::edge_t>
    _asynchronous_incoming_edges_map;                      /*!< Map: location ID -> asynchronous incoming edges */
  };
  
} // end of namespace tchecker




/*!
 \brief Iterator traits for tchecker::details::synchronizer_iterator_t
 */
template <class SYSTEM, class VLOC>
struct std::iterator_traits<tchecker::details::synchronizer_iterator_t<SYSTEM, VLOC>> {
  using difference_type = nullptr_t;
  
  using value_type = tchecker::range_t<typename tchecker::details::synchronizer_iterator_t<SYSTEM, VLOC>::vedge_iterator_t>;
  
  using pointer = value_type *;
  
  using reference = value_type &;
  
  using iterator_category = std::forward_iterator_tag;
};

#endif //TCHECKER_SYNCHRONIZER_HH
