/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_LOC_HH
#define TCHECKER_SYSTEM_LOC_HH

#include <string>
#include <vector>

#include <boost/dynamic_bitset.hpp>
#include <boost/container/flat_map.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file loc.hh
 \brief System locations
 */

namespace tchecker {
  
  /*!
   \class loc_t
   \tparam EDGE : type of edge, should derive from tchecker::edge_t
   \brief Location
   */
  template <class EDGE>
  class loc_t {
  public:
    /*!
     \brief Type of edges
     */
    using edge_t = EDGE;
    
    /*!
     \brief Constructor
     \param pid : process identifier
     \param id : location identifier
     \param name : location name
     \pre name is not empty
     \throw std::invalid_argument : if the precondition is violated
     */
    loc_t(tchecker::process_id_t pid, tchecker::loc_id_t id, std::string const & name)
    : _pid(pid), _id(id), _name(name)
    {
      if (_name.empty())
        throw std::invalid_argument("empty name");
    }
    
    /*!
     \brief Copy constructor
     \param loc : location
     \post this is a copy of loc
     */
    loc_t(tchecker::loc_t<EDGE> const & loc) = default;
    // NB: edges stored in internal data structures are not cloned (references)
    
    /*!
     \brief Move constructor
     \param loc : location
     \post loc has been moved to this
     */
    loc_t(tchecker::loc_t<EDGE> && loc) = default;
    
    /*!
     \brief Destructor
     */
    ~loc_t() = default;
    // NB: edges stored in internal data structures should not be deleted (references)
    
    /*!
     \brief Assignement operator
     \param loc : location
     \post this is a copy of loc
     */
    tchecker::loc_t<EDGE> & operator= (tchecker::loc_t<EDGE> const & loc) = default;
    // NB: edges stored in internal data structures are not cloned (references)
    
    /*!
     \brief Move assignement operator
     \param loc : location
     \post this is a copy of loc
     */
    tchecker::loc_t<EDGE> & operator= (tchecker::loc_t<EDGE> && loc) = default;
    
    /*!
     \brief Accessor
     \return process identifier
     */
    inline tchecker::process_id_t pid() const
    {
      return _pid;
    }
    
    /*!
     \brief Accessor
     \return Identifier
     */
    inline tchecker::loc_id_t id() const
    {
      return _id;
    }
    
    /*!
     \brief Accessor
     \return Name
     */
    inline std::string const & name() const
    {
      return _name;
    }
    
    /*!
     \brief Adds an outgoing edge
     \param edge : edge to add
     \pre this is the source location of edge
     \post edge has been added to this location as an outgoing edge
     \throw std::invalid_argument : if the precondition is violated
     \note this keeps a pointer to edge, but it never deletes edge
     */
    void add_outgoing_edge(EDGE const * edge)
    {
      if (edge->src() != this)
        throw std::invalid_argument("not the source location of edge");
      
      _outgoing_edges.push_back(edge);
      
      tchecker::event_id_t event_id = edge->event_id();
      
      if (event_id >= _outgoing_events.size())
        _outgoing_events.resize(event_id + 1, false);
      _outgoing_events[event_id] = true;
      
      _outgoing_edges_map[event_id].push_back(edge);
    }
    
    /*!
     \brief Adds an incoming edge
     \param edge : edge to add
     \pre this is the target location of edge
     \post edge has been added to this location as an incoming edge
     \throw std::invalid_argument : if the precondition is violated
     \note this keeps a pointer to edge, but it never deletes edge
     */
    void add_incoming_edge(EDGE const * edge)
    {
      if (edge->tgt() != this)
        throw std::invalid_argument("not the target location of edge");
      
      _incoming_edges.push_back(edge);
      
      tchecker::event_id_t event_id = edge->event_id();
      
      if (event_id >= _incoming_events.size())
        _incoming_events.resize(event_id + 1);
      _incoming_events[event_id] = true;
      
      _incoming_edges_map[event_id].push_back(edge);
    }
    
    /*!
     \brief Edge iterator
     */
    using const_edge_iterator_t = typename std::vector<EDGE const *>::const_iterator;
    
    /*!
     \brief Accessor
     \return Range of outgoing edges
     \note the range follows the order of addition of edges
     */
    inline tchecker::range_t<const_edge_iterator_t> outgoing_edges() const
    {
      return tchecker::make_range(_outgoing_edges.begin(), _outgoing_edges.end());
    }
    
    /*!
     \brief Accessor
     \param event_id : event ID
     \return range of outgoing edges with event_id
     \note the range follows the order of addition of edges
     */
    inline tchecker::range_t<const_edge_iterator_t> outgoing_edges(tchecker::event_id_t event_id) const
    {
      if ( ! outgoing_event(event_id) )
        return tchecker::make_range(_no_edge.begin(), _no_edge.end());
      auto const & edges = _outgoing_edges_map.at(event_id);
      return tchecker::make_range(edges.begin(), edges.end());
    }
    
    /*!
     \brief Accessor
     \return Range of incoming edges
     \note the range follows the order of addition of edges
     */
    inline tchecker::range_t<const_edge_iterator_t> incoming_edges() const
    {
      return tchecker::make_range(_incoming_edges.begin(), _incoming_edges.end());
    }
    
    /*!
     \brief Accessor
     \param event_id : event ID
     \return range of incoming edges with event_id
     \note the range follows the order of addition of edges
     */
    inline tchecker::range_t<const_edge_iterator_t> incoming_edges(tchecker::event_id_t event_id) const
    {
      if ( ! incoming_event(event_id) )
        return tchecker::make_range(_no_edge.begin(), _no_edge.end());
      auto const & edges = _incoming_edges_map.at(event_id);
      return tchecker::make_range(edges.begin(), edges.end());
    }
    
    /*!
     \brief Accessor
     \param event_id : event ID
     \return true if this location has an outgoing edge with event_id, false otherwise
     \note Constant-time complexity
     */
    inline bool outgoing_event(tchecker::event_id_t event_id) const
    {
      return ( (event_id < _outgoing_events.size()) && _outgoing_events[event_id] );
    }
    
    /*!
     \brief Accessor
     \param event_id : event ID
     \return true if this location has an incoming edge with event_id, false otherwise
     \note Constant-time complexity
     */
    inline bool incoming_event(tchecker::event_id_t event_id) const
    {
      return ( (event_id < _incoming_events.size()) && _incoming_events[event_id] );
    }
  private:
    template <class L, class E> friend class tchecker::system_t;
    
    /*!
     \brief Clear events and edges
     \post this location has no incoming/outgoing event and edge
     */
    void clear_events_and_edges()
    {
      _incoming_edges.clear();
      _outgoing_edges.clear();
      _incoming_events.clear();
      _outgoing_events.clear();
      _incoming_edges_map.clear();
      _outgoing_edges_map.clear();
      // NB: edges should not be deleted (references)
    }
    
    /*!
     \brief Type of map: event ID -> collection of edges
     */
    using event_edge_map_t = boost::container::flat_map<tchecker::event_id_t, std::vector<EDGE const *>>;
    
    tchecker::process_id_t _pid;                       /*!< Process ID */
    tchecker::loc_id_t _id;                            /*!< Identifier */
    std::string _name;                                 /*!< Name */
    std::vector<EDGE const *> _incoming_edges;         /*!< Incoming edges */
    std::vector<EDGE const *> _outgoing_edges;         /*!< Outgoing edges */
    boost::dynamic_bitset<> _incoming_events;          /*!< Incoming events */
    boost::dynamic_bitset<> _outgoing_events;          /*!< Outgoing events */
    event_edge_map_t _incoming_edges_map;              /*!< Map: event ID -> incoming edges */
    event_edge_map_t _outgoing_edges_map;              /*!< Map: event ID -> outgoing edges */
    static std::vector<EDGE const *> const _no_edge;   /*!< Empty set of edges (see outgoing_edges and incoming_edges) */
  };
  
  
  template <class EDGE>
  std::vector<EDGE const *> const loc_t<EDGE>::_no_edge(0, nullptr);   /*! Instantiation of static variable */
  
} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_LOC_HH
