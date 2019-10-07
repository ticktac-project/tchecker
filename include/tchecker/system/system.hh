/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_HH
#define TCHECKER_SYSTEM_HH

#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include <boost/container/flat_map.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/system/edge.hh"
#include "tchecker/system/loc.hh"
#include "tchecker/system/synchronization.hh"
#include "tchecker/utils/index.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file system.hh
 \brief System of processes
 */


namespace tchecker {
  
  // Forward declaration
  template <class LOC, class EDGE> class system_t;
  
  
  /*!
   \class event_index_t
   \brief Index of event names
   */
  class event_index_t : public tchecker::autokey_index_t<tchecker::event_id_t, std::string> {
  public:
    using tchecker::autokey_index_t<tchecker::event_id_t, std::string>::autokey_index_t;
  };
  
  
  
  
  /*!
   \class process_index_t
   \brief Index of process names
   */
  class process_index_t : public tchecker::autokey_index_t<tchecker::process_id_t, std::string> {
  public:
    using tchecker::autokey_index_t<tchecker::process_id_t, std::string>::autokey_index_t;
  };
  
  
  
  
  /*!
   \brief Type of iterator over synchronization vectors
   */
  using const_sync_iterator_t = std::vector<tchecker::synchronization_t>::const_iterator;
  
  
  
  
  /*!
   \class system_t
   \tparam LOC : type of locations, should derive from tchecker::loc_t
   \tparam EDGE : type of edges, should derive from tchecker::edge_t
   \brief System of processes
   \note locations and edges cannot be added on purpose: one should use
   specialized derived classes in order to build models
   */
  template <class LOC, class EDGE>
  class system_t {
    
    static_assert(std::is_base_of<tchecker::edge_t<LOC>, EDGE>::value, "bad EDGE type");
    static_assert(std::is_base_of<tchecker::loc_t<EDGE>, LOC>::value, "bad LOC type");
    
  protected:
    /*!
     \brief Type of key for map process+loc name -> loc
     */
    using loc_map_key_t = std::tuple<tchecker::process_id_t, std::string>;
    
    /*!
     \brief Type of map process+loc name -> loc
     */
    using loc_map_t = boost::container::flat_map<loc_map_key_t, LOC *>;
    
    /*!
     \brief Type of multimap process_id -> loc
     */
    using loc_multimap_t = boost::container::flat_multimap<tchecker::process_id_t, LOC *>;
  public:
    /*!
     \brief Type of locations
     */
    using loc_t = LOC;
    
    /*!
     \brief Type of edges
     */
    using edge_t = EDGE;
    
    /*!
     \brief Constructor
     \param name : new name
     \pre name is not empty
     \throw std::invalid_argument : if the precondition is violated
     */
    explicit system_t(std::string const & name) : _name(name)
    {
      if (_name.empty())
        throw std::invalid_argument("error, empty name");
    }
    
    /*!
     \brief Copy constructor
     \param system : a system
     \post this is a copy of system
     */
    system_t(tchecker::system_t<LOC, EDGE> const & system)
    : _name(system._name),
    _events(system._events),
    _processes(system._processes),
    _syncs(system._syncs)
    {
      system.clone_locations_and_edges(*this);
    }
    
    
    /*!
     \brief Move constructor
     \param system : a system
     \post system has been moved to this
     */
    system_t(tchecker::system_t<LOC, EDGE> && system) = default;
    
    
    /*!
     \brief Destructor
     */
    ~system_t()
    {
      for (LOC * loc : _locs)
        delete loc;
      for (EDGE * edge : _edges)
        delete edge;
      // NB: pointers in _locs_map and _initial_locs_map are references
    }
    
    /*!
     \brief Assignment operator
     \param system : a system
     \post this is a copy of system
     */
    tchecker::system_t<LOC, EDGE> & operator= (tchecker::system_t<LOC, EDGE> const & system)
    {
      if (this != &system) {
        _name = system._name;
        _events = system._events;
        _processes = system._processes;
        for (LOC const * loc : _locs)
          delete loc;
        _locs.clear();
        _locs_map.clear();
        _initial_locs.clear();
        for (EDGE const * edge : _edges)
          delete edge;
        _edges.clear();
        system.clone_locations_and_edges(*this);
        _syncs = system._syncs;
      }
      return *this;
    }
    
    
    /*!
     \brief Move assignment operator
     \system : a system
     \post system has been noved to this
     */
    tchecker::system_t<LOC, EDGE> & operator= (tchecker::system_t<LOC, EDGE> && system) = default;
    
    /*!
     \brief Accessor
     \return system name
     */
    inline std::string const & name() const
    {
      return _name;
    }
    
    /*!
     \brief Accessor
     \return event index
     */
    inline tchecker::event_index_t const & events() const
    {
      return _events;
    }
    
    /*!
     \brief Accessor
     \return process index
     */
    inline tchecker::process_index_t const & processes() const
    {
      return _processes;
    }
    
    // iterator over locations
  private:
    /*!
     \brief Type of proxy for const iterator over locations
     */
    using const_loc_iterator_proxy_t = std::function< LOC const * (LOC *) >;
    
    /*!
     \brief Proxy for const iterator over locations
     */
    static LOC const * const_loc_iterator_proxy(LOC * loc)
    {
      return (loc);
    }
  public:
    /*!
     \brief Const iterator over locations
     */
    using const_loc_iterator_t = boost::transform_iterator<const_loc_iterator_proxy_t, typename std::vector<LOC *>::const_iterator>;
    
    /*!
     \brief Accessor
     \return range of locations
     */
    inline tchecker::range_t<const_loc_iterator_t> locations() const
    {
      const_loc_iterator_t
      begin(_locs.begin(), &system_t<LOC, EDGE>::const_loc_iterator_proxy),
      end(_locs.end(), &system_t<LOC, EDGE>::const_loc_iterator_proxy);
      
      return tchecker::make_range(begin, end);
    }
    
    /*!
     \brief Accessor
     \param id : location id
     \pre 0 <= id < locations_count()
     \return location with identifier id
     \throw std::invalid_argument : if the precondition is violated
     */
    inline LOC const * location(tchecker::loc_id_t id) const
    {
      if (id >= _locs.size())
        throw std::invalid_argument("id out of range");
      return _locs[id];
    }
    
    /*!
     \brief Accessor
     \param process_name : process name
     \param name : location name
     \pre process_name and name are declared names of a process and a
     location in that process respectively
     \return location with name in process process_name
     \throw std::invalid_argument : if the precondition is violated
     */
    inline LOC * location(std::string const & process_name, std::string const & name) const
    {
      auto p = _locs_map.find(locs_map_key(process_name, name));
      if (p == _locs_map.end())
        throw std::invalid_argument("unknown location");
      return p->second;
    }
    
    // iterator over initial locations
  private:
    /*!
     \brief Type of proxy for const iterator over locations
     */
    using const_initial_loc_iterator_proxy_t = std::function< LOC const *(typename loc_multimap_t::const_iterator::reference) >;
    
    /*!
     \brief Proxy for const iterator over locations
     */
    static LOC const * const_initial_loc_iterator_proxy(typename loc_multimap_t::const_iterator::reference r)
    {
      return r.second;
    }
  public:
    /*!
     \brief Const iterator over initial locations
     */
    using const_initial_loc_iterator_t =
    boost::transform_iterator<const_initial_loc_iterator_proxy_t, typename loc_multimap_t::const_iterator>;
    
    /*!
     \brief Accessor
     \param pid : process identifier
     \return range of initial locations for process pid
     \throw std::invalid_argument : if pid is not a declared process
     */
    inline tchecker::range_t<const_initial_loc_iterator_t> initial_locations(tchecker::process_id_t pid) const
    {
      if ( pid >= _processes.size() )
        throw std::invalid_argument("invalid process identifier");
      
      auto range = _initial_locs.equal_range(pid);
      
      const_initial_loc_iterator_t
      begin(range.first, &system_t<LOC,EDGE>::const_initial_loc_iterator_proxy),
      end(range.second, &system_t<LOC,EDGE>::const_initial_loc_iterator_proxy);
      
      return tchecker::make_range(begin, end);
    }
    
    // iterator over edges
  private:
    /*!
     \brief Type of proxy for const iterator over edges
     */
    using const_edge_iterator_proxy_t = std::function< EDGE const *(EDGE *) >;
    
    /*!
     \brief Proxy for const iterator over edges
     */
    static EDGE const * const_edge_iterator_proxy(EDGE * edge)
    {
      return edge;
    }
  public:
    /*!
     \brief Const iterator over edges
     */
    using const_edge_iterator_t =
    boost::transform_iterator<const_edge_iterator_proxy_t, typename std::vector<EDGE *>::const_iterator>;
    
    /*!
     \brief Accessor
     \return range of edges
     */
    inline tchecker::range_t<const_edge_iterator_t> edges() const
    {
      const_edge_iterator_t
      begin(_edges.begin(), &system_t<LOC, EDGE>::const_edge_iterator_proxy),
      end(_edges.end(), &system_t<LOC, EDGE>::const_edge_iterator_proxy);
      
      return tchecker::make_range(begin, end);
    }
    
    /*!
     \brief Accessor
     \param id : edge id
     \pre 0 <= id < edges_count()
     \return edge with identifier id
     \throw std::invalid_argument : if the precondition is violated
     */
    inline EDGE const * edge(tchecker::edge_id_t id) const
    {
      if (id >= _edges.size())
        throw std::invalid_argument("id out of range");
      return _edges[id];
    }
    
    // iterator over synchronizations
    
    /*!
     \brief Accessor
     \return range of synchronizations
     */
    inline tchecker::range_t<tchecker::const_sync_iterator_t> synchronizations() const
    {
      return tchecker::make_range(_syncs.begin(), _syncs.end());
    }
    
    /*!
     \brief Accessor
     \param id : synchronization id
     \pre 0 <= id < synchronizations_count()
     \return synchronization with identifier id
     \throw std::invalid_argument : if the precondition is violated
     */
    inline tchecker::synchronization_t const & synchronization(tchecker::sync_id_t id) const
    {
      if (id >= _syncs.size())
        throw std::invalid_argument("id out of range");
      return _syncs[id];
    }
    
    /*!
     \brief Accessor
     \param pid : process identifier
     \param event_id : event identifier
     \return true if event_id is asynchronous for process pid (i.e. there is
     no synchronization with pid@event_id), false otherwise
     */
    bool asynchronous(tchecker::process_id_t pid, tchecker::event_id_t event_id) const
    {
      for (tchecker::synchronization_t const & sync : _syncs) {
        auto constraints = sync.synchronization_constraints();
        for (tchecker::sync_constraint_t const & constr : constraints)
          if ((constr.pid() == pid) && (constr.event_id() == event_id))
            return false;
      }
      return true;
    }
    
    /*!
     \brief Accessor
     \return number of events
     \note all values between 0 and the returned value-1 are valid event
     identifiers
     */
    inline tchecker::event_id_t events_count() const
    {
      return static_cast<tchecker::event_id_t>(_events.size());
    }
    
    /*!
     \brief Accessor
     \return number of processes
     \note all values between 0 and the returned value-1 are valid process
     identifiers
     */
    inline tchecker::process_id_t processes_count() const
    {
      return static_cast<tchecker::process_id_t>(_processes.size());
    }
    
    /*!
     \brief Accessor
     \return number of locations
     \note all values between 0 and the returned value-1 are valid location
     identifiers
     */
    inline tchecker::loc_id_t locations_count() const
    {
      return static_cast<tchecker::loc_id_t>(_locs.size());
    }
    
    /*!
     \brief Accessor
     \return number of edges
     \note all values between 0 and the returned value-1 are valid edge
     identifiers
     */
    inline tchecker::edge_id_t edges_count() const
    {
      return static_cast<tchecker::edge_id_t>(_edges.size());
    }
    
    /*!
     \brief Accessor
     \return number of synchronization vectors
     \note all values between 0 and the returned value-1 are valid
     synchronization vector identifiers
     */
    inline tchecker::sync_id_t synchronizations_count() const
    {
      return static_cast<tchecker::sync_id_t>(_syncs.size());
    }
    
    
    // system construction
    
    /*!
     \brief Add an event
     \param name : event name
     \pre event name is not declared yet
     \post event name has been added
     \throw std::invalid_argument : if the precondition is violated
     */
    void add_event(std::string const & name)
    {
      try {
        _events.add(name);
      }
      catch (...) {
        throw;
      }
    }
    
    /*!
     \brief Add a process
     \param name : process name
     \pre process name is not declared yet
     \return the ID of process name
     \throw std::invalid_argument : if the precondition is violated
     */
    void add_process(std::string const & name)
    {
      try {
        _processes.add(name);
      }
      catch (...) {
        throw;
      }
    }
    
    // adding synchronization
  private:
    /*!
     \brief synchronization tuple
     */
    using sync_str_tuple_t = std::tuple<std::string, std::string, tchecker::sync_strength_t>;
    
    
    /*!
     \brief transformed synchronization tuple
     */
    using sync_id_tuple_t = std::tuple<tchecker::process_id_t, tchecker::event_id_t, tchecker::sync_strength_t>;
    
    /*!
     \brief Type of proxy for const tranformation iterator on synchronizations
     */
    using const_sync_trans_iterator_proxy_t = std::function< sync_id_tuple_t(sync_str_tuple_t const &) >;
    
    /*!
     \brief Proxy for const transformation iterator over synchronizations
     */
    sync_id_tuple_t const_sync_trans_iterator_proxy(sync_str_tuple_t const & t)
    {
      std::string const & process_name = std::get<0>(t);
      std::string const & event_name = std::get<1>(t);
      tchecker::sync_strength_t strength = std::get<2>(t);
      
      try {
        tchecker::process_id_t pid = _processes.key(process_name);
        tchecker::event_id_t event_id = _events.key(event_name);
        
        return std::make_tuple(pid, event_id, strength);
      }
      catch (...) {
        throw;
      }
    }
  public:
    /*!
     \brief Add a synchronization
     \tparam SYNCS_ITER : type of iterator on tuples
     (process name, event name, strength)
     \param sync : range of tuples (process name, event name, strength)
     \pre process names in sync are declared processes, event names in sync
     are declared events, and process names appear at most once in sync
     \throw std::invalid_argument : if the precondition is violated
     \note SYNCS_ITER is supposed to dereference to std::tuple<std::string,
     std::string, enum tchecker::sync_strength_t>
     */
    template <class SYNCS_ITER>
    void add_synchronization(tchecker::range_t<SYNCS_ITER> const & sync)
    {
      static_assert(std::is_same<typename SYNCS_ITER::value_type, sync_str_tuple_t>::value, "bad iterator");
      
      auto proxy = std::bind(&tchecker::system_t<LOC, EDGE>::const_sync_trans_iterator_proxy, this, std::placeholders::_1);
      
      boost::transform_iterator<const_sync_trans_iterator_proxy_t, SYNCS_ITER> begin(sync.begin(), proxy), end(sync.end(), proxy);
      
      try {
        assert(_syncs.size() <= std::numeric_limits<tchecker::sync_id_t>::max());
        _syncs.emplace_back(_syncs.size(), tchecker::make_range(begin, end));
      }
      catch (...) {
        throw;
      }
    }
  protected:
    /*!
     \brief Accessor
     \return Next location ID (1 + biggest ID among declared locations)
     */
    inline tchecker::loc_id_t next_loc_id() const
    {
      return static_cast<tchecker::loc_id_t>(_locs.size());
    }
    
    /*!
     \brief Accessor
     \return Next edge ID (1 + biggest ID among declared edges)
     */
    inline tchecker::edge_id_t next_edge_id() const
    {
      return static_cast<tchecker::edge_id_t>(_edges.size());
    }
    
    /*!
     \brief Add a location
     \param loc : location
     \pre loc is not nullptr, loc is not declared yet, loc ID is the next
     available location ID, loc PID is a declared process, and the lists of
     incoming/outgoing edges are empty
     \post loc has been added
     \throw std::invalid_argument : if the precondition is violated
     */
    void add_location(LOC * loc)
    {
      try {
        // Check the precondition
        if (loc == nullptr)
          throw std::invalid_argument("nullptr loc");
        
        auto loc_key = locs_map_key(loc->pid(), loc->name());
        
        if (_locs_map.find(loc_key) != _locs_map.end())
          throw std::invalid_argument("location already declared");
        
        if (loc->id() != next_loc_id())
          throw std::invalid_argument("invalid location ID");
        
        if (loc->pid() >= _processes.size())
          throw std::invalid_argument("invalid location PID");
        
        auto range = loc->outgoing_edges();
        if (range.begin() != range.end())
          throw std::invalid_argument("non empty outgoing edges");
        
        range = loc->incoming_edges();
        if (range.begin() != range.end())
          throw std::invalid_argument("non empty incoming edges");
        
        // Add location
        _locs.push_back(loc);
        
        _locs_map.insert( std::make_pair(loc_key, loc) );
        
        if (loc->initial())
          _initial_locs.insert( std::make_pair(loc->pid(), loc) );
      }
      catch (...) {
        throw;
      }
    }
    
    /*!
     \brief Add an edge
     \param edge : edge
     \param src : location
     \param tgt : location
     \pre edge is not nullptr, src is not nullptr, tgt is not nullptr, edge ID
     is the nest available edge ID, edge PID is a declared process, edge source 
     and target locations belong to edge PID, edge event is a declard event, and
     edge has source location src and target location tgt
     \post edge has been added to the system, and to its source and target
     locations (src and tgt)
     \throw std::invalid_argument : if the precondition is violated
     */
    void add_edge(EDGE * edge, LOC * src, LOC * tgt)
    {
      try {
        // Check the precondition
        if (edge == nullptr)
          throw std::invalid_argument("nullptr edge");
        
        if (src == nullptr)
          throw std::invalid_argument("nullptr source location");
        
        if (tgt == nullptr)
          throw std::invalid_argument("nullptr target location");
        
        if (edge->id() != next_edge_id())
          throw std::invalid_argument("invalid edge ID");
        
        if (edge->pid() >= _processes.size())
          throw std::invalid_argument("invalid edge PID");
        
        if (edge->pid() != edge->src()->pid())
          throw std::invalid_argument("invalid source location");
        
        if (edge->pid() != edge->tgt()->pid())
          throw std::invalid_argument("invalid target location");
        
        if (edge->event_id() >= _events.size())
          throw std::invalid_argument("invalid event ID");
        
        if ( edge->src() != src)
          throw std::invalid_argument("bad source location");
        
        if ( edge->tgt() != tgt)
          throw std::invalid_argument("bad target location");
        
        
        // Add edge
        _edges.push_back(edge);
        src->add_outgoing_edge(edge);
        tgt->add_incoming_edge(edge);
      }
      catch (...) {
        throw;
      }
    }
    
    /*!
     \brief Key constructor for location map
     \param pid : process identifier
     \param name : location name
     \return key for location name in process pid
     */
    static inline loc_map_key_t locs_map_key(tchecker::process_id_t pid, std::string const & name)
    {
      return std::make_tuple(pid, name);
    }
    
    /*!
     \brief Key constructor for location map
     \param process_name : process name
     \param name : location name
     \pre process_name is a declared process
     \return key for location name in process process_name
     \throw std::invalid_argument : if the precondition is violated
     */
    inline loc_map_key_t locs_map_key(std::string const & process_name, std::string const & name) const
    {
      return locs_map_key(_processes.key(process_name), name);
    }
    
    /*!
     \brief Clone locations and edges
     \param clone : a system
     \pre clone has all events and processes defined with same ID as in this
     system, but no location and no edge
     \post all locations and edges in this system have been cloned in clone
     \throw std::invalid_argument : if clone already has locations or edges,
     or if some event or process in this system is not defined in clone
     */
    void clone_locations_and_edges(tchecker::system_t<LOC, EDGE> & clone) const
    {
      if ( ! clone._locs.empty() || ! clone._locs_map.empty() || ! clone._initial_locs.empty() )
        throw std::invalid_argument("cloned system already has locations");
      if (! clone._edges.empty())
        throw std::invalid_argument("cloned system already has edges");
      
      for (LOC const * loc : _locs) {
        LOC * new_loc = new LOC(*loc);
        new_loc->clear_events_and_edges();
        clone.add_location(new_loc);
      }
      
      for (EDGE const * edge : _edges) {
        EDGE * new_edge = new EDGE(*edge);
        LOC * src = clone._locs[edge->src()->id()], * tgt = clone._locs[edge->tgt()->id()];
        new_edge->reset_src(src);
        new_edge->reset_tgt(tgt);
        clone.add_edge(new_edge, src, tgt);
      }
    }
    
    std::string _name;                                /*!< System name */
    event_index_t _events;                            /*!< Event index */
    process_index_t _processes;                       /*!< Process index */
    std::vector<LOC *> _locs;                         /*!< Locations */
    loc_map_t _locs_map;                              /*!< Map process:loc names -> loc */
    loc_multimap_t _initial_locs;                     /*!< Map pid -> {initial locs} */
    std::vector<EDGE *> _edges;                       /*!< Edges */
    std::vector<tchecker::synchronization_t> _syncs;  /*!< Synchronization vectors */
  };
  
} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_HH
