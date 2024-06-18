/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_EDGE_HH
#define TCHECKER_SYSTEM_EDGE_HH

#include <cassert>
#include <memory>
#include <vector>

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/system/attribute.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file edge.hh
 \brief System edges
 */

namespace tchecker {

namespace system {

/*!
 \class edge_t
 \brief System edge
 */
class edge_t {
public:
  /*!
   \brief Constructor
   \param pid : process identifier
   \param id : identifier
   \param src : source location
   \param tgt : target location
   \param event_id :  event identifier
   \param attributes : edge attributes
   \pre src!=nullptr, tgt!=nullptr, and src and tgt belong to process pid
   \throw std::invalid_argument : if the precondition is violated
   */
  edge_t(tchecker::process_id_t pid, tchecker::edge_id_t id, tchecker::loc_id_t src, tchecker::loc_id_t tgt,
         tchecker::event_id_t event_id, tchecker::system::attributes_t const & attributes);

  /*!
   \brief Copy constructor
   \param e : edge
   \post this is a copy of e
   */
  edge_t(tchecker::system::edge_t const & e) = default;

  /*!
   \brief Move constructor
   \param e : edge
   \post e has been moved to this
   */
  edge_t(tchecker::system::edge_t && e) = default;

  /*!
   \brief Destructor
   */
  ~edge_t() = default;

  /*!
   \brief Assignement operator
   \param e : edge
   \post this is a copy of e
   */
  tchecker::system::edge_t & operator=(tchecker::system::edge_t const & e) = default;

  /*!
   \brief Move assignement operator
   \param e : edge
   \post this is a copy of e
   */
  tchecker::system::edge_t & operator=(tchecker::system::edge_t && e) = default;

  /*!
   \brief Accessor
   \return process identifier
   */
  inline tchecker::process_id_t pid() const { return _pid; }

  /*!
   \brief Accessor
   \return Identifier
   */
  inline tchecker::edge_id_t id() const { return _id; }

  /*!
   \brief Accessor
   \return Source location
   */
  inline tchecker::loc_id_t src() const { return _src; }

  /*!
   \brief Accessor
   \return Target location
   */
  inline tchecker::loc_id_t tgt() const { return _tgt; }

  /*!
   \brief Accessor
   \return Event
   */
  inline tchecker::event_id_t event_id() const { return _event_id; }

  /*!
   \brief Accessor
   \return Attributes
   */
  inline tchecker::system::attributes_t const & attributes() const { return _attributes; }

private:
  tchecker::process_id_t _pid;                /*!< Process ID */
  tchecker::edge_id_t _id;                    /*!< Identifier */
  tchecker::loc_id_t _src;                    /*!< Source location */
  tchecker::loc_id_t _tgt;                    /*!< Target location */
  tchecker::event_id_t _event_id;             /*!< Event */
  tchecker::system::attributes_t _attributes; /*!< Attributes */
};

/*!
 \brief Type of shared pointer to edge
 */
using edge_shared_ptr_t = std::shared_ptr<tchecker::system::edge_t>;

/*!
 \brief Type of shared pointer to constant edge
 */
using edge_const_shared_ptr_t = std::shared_ptr<tchecker::system::edge_t const>;

/*!
 \brief Type of collection of edges
 */
using edges_collection_t = std::vector<tchecker::system::edge_shared_ptr_t>;

/*!
 \class edges_collection_const_iterator_t
 \brief Type of iterator over collection of edges
 */
class edges_collection_const_iterator_t : public tchecker::system::edges_collection_t::const_iterator {
public:
  /*!
   \brief Default constructor
  */
  edges_collection_const_iterator_t() = default;

  /*!
   \brief Constructor
   */
  edges_collection_const_iterator_t(tchecker::system::edges_collection_t::const_iterator const & it);

  /*!
   \brief Dereference operator
   \return edge marked const
   */
  inline tchecker::system::edge_const_shared_ptr_t operator*() const { return edges_collection_t::const_iterator::operator*(); }
};

/*!
 \class loc_edges_maps_t
 \brief Maps from location IDs to collection of edges and events
 */
class loc_edges_maps_t {
private:
  /*!
   \brief Type of collection of events
   */
  using events_collection_t = boost::dynamic_bitset<>;

public:
  /*!
   \brief Clear all maps
   */
  void clear();

  /*!
   \brief Add an edge
   \param loc : location identifer
   \param edge : an edge
   \post edge has been added to the maps
   */
  void add_edge(tchecker::loc_id_t loc, tchecker::system::edge_shared_ptr_t const & edge);

  /*!
   \brief Accessor
   \param loc : location identifier
   \return range of edges associated to location loc
   */
  tchecker::range_t<tchecker::system::edges_collection_const_iterator_t> edges(tchecker::loc_id_t loc) const;

  /*!
   \brief Accessor
   \param loc : location identifier
   \param event : event identifier
   \return range of edges with event and associated to location loc
   */
  tchecker::range_t<tchecker::system::edges_collection_const_iterator_t> edges(tchecker::loc_id_t loc,
                                                                               tchecker::event_id_t event) const;

  /*!
   \brief Accessor
   \param loc : location identifier
   \param event : event identifier
   \return true if loc is mapped to event (i.e. loc has an edge labelled by event), false otherwise
   */
  bool event(tchecker::loc_id_t loc, tchecker::event_id_t event) const;

private:
  std::vector<tchecker::system::edges_collection_t> _loc_to_edges;  /*!< Map : loc ID -> edges */
  std::vector<events_collection_t> _loc_to_events;                  /*!< Map : loc ID -> events */
  std::vector<std::vector<edges_collection_t>> _loc_event_to_edges; /*!< Map : (loc ID, event ID) -> edges */
  static tchecker::system::edges_collection_t const _empty_edges;   /*!< Empty collection of edges */
};

/*!
 \class proc_edges_maps_t
 \brief Maps from process IDs to collection of edges and events
 */
class proc_edges_maps_t {
private:
  /*!
   \brief Type of collection of events
   */
  using events_collection_t = boost::dynamic_bitset<>;

public:
  /*!
   \brief Clear all maps
   */
  void clear();

  /*!
   \brief Add an edge
   \param pid : process identifier
   \param edge : an edge
   \post edge has been added to the maps
   */
  void add_edge(tchecker::process_id_t pid, tchecker::system::edge_shared_ptr_t const & edge);

  /*!
   \brief Accessor
   \param pid : process identifier
   \return range of edges associated to process pid
   */
  tchecker::range_t<tchecker::system::edges_collection_const_iterator_t> edges(tchecker::process_id_t pid) const;

  /*!
   \brief Accessor
   \param pid : process identifier
   \param event_id : event identifier
   \return range of edges with event event_id in process pid
   */
  tchecker::range_t<tchecker::system::edges_collection_const_iterator_t> edges(tchecker::process_id_t pid,
                                                                               tchecker::event_id_t event_id) const;

  /*!
   \brief Accessor
   \param pid : process identifier
   \param event_id : event identifier
   \return true if process pid has an edge labelled with event event_id, false otherwise
   */
  bool event(tchecker::process_id_t pid, tchecker::event_id_t event) const;

private:
  std::vector<tchecker::system::edges_collection_t> _proc_to_edges;  /*!< Map : process ID -> edges */
  std::vector<events_collection_t> _proc_to_events;                  /*!< Map : process ID -> events */
  std::vector<std::vector<edges_collection_t>> _proc_event_to_edges; /*!< Map : (process ID, event ID) -> edges */
  static tchecker::system::edges_collection_t const _empty_edges;    /*!< Empty collection of edges */
};

/*!
 \brief Direction of edges
 */
enum edge_direction_t {
  INCOMING_EDGE = 0,
  OUTGOING_EDGE = 1,
  EDGE_DIRECTION_COUNT // should come last
};

/*!
 \class edges_t
 \brief Collection of edges
 */
class edges_t {
public:
  /*!
   \brief Constructor
   */
  edges_t();

  /*!
   \brief Copy constructor
   */
  edges_t(tchecker::system::edges_t const &);

  /*!
   \brief Move constructor
   */
  edges_t(tchecker::system::edges_t &&);

  /*!
   \brief Destructor
   */
  ~edges_t();

  /*!
   \brief Assignment operator
   */
  tchecker::system::edges_t & operator=(tchecker::system::edges_t const &);

  /*!
   \brief Move-assignment operator
   */
  tchecker::system::edges_t & operator=(tchecker::system::edges_t &&);

  /*!
   \brief Clear
   \post this is an empty collection of edges
   */
  void clear();

  /*!
   \brief Add edge
   \param pid : process identifier
   \param src : source location
   \param tgt : target location
   \param event_id : event identifier
   \param attributes : edge attributes
   \pre src and tgt belong to process pid
   \post edge src -> tgt with event event_id in process pid has been added
   \throw std::runtime_error : if edge identifiers have been exhausted
   */
  void add_edge(tchecker::process_id_t pid, tchecker::loc_id_t src, tchecker::loc_id_t tgt, tchecker::event_id_t event_id,
                tchecker::system::attributes_t const & attributes);

  /*!
   \brief Accessor
   \return number of edges
   \note all integers in 0..edges_count()-1 are valid edge identifiers
   */
  inline std::size_t edges_count() const { return _edges.size(); }

  /*!
   \brief Type of range of edges identifiers
   */
  using edges_identifiers_range_t = tchecker::integer_range_t<tchecker::edge_id_t>;

  /*!
   \brief Accessor
   \return range of event identifiers 0..edges_count()-1
   */
  edges_identifiers_range_t edges_identifiers() const;

  /*!
   \brief Accessor
   \param edge_id : edge identifier
   \pre edge_id is a valid edge identifier (checked by assertion)
   \return edge with identifier edge_id
   */
  inline tchecker::system::edge_const_shared_ptr_t edge(tchecker::edge_id_t edge_id) const
  {
    assert(edge_id < _edges.size());
    return _edges[edge_id];
  }

  /*!
   \brief Type of const iterator over edges
   */
  using const_iterator_t = tchecker::system::edges_collection_const_iterator_t;

  /*!
   \brief Accessor
   \return range of edges
   */
  tchecker::range_t<tchecker::system::edges_t::const_iterator_t> edges() const;

  /*!
   \brief Accessor
   \param loc : location identifier
   \return range of outgoing edges of location loc
   */
  tchecker::range_t<tchecker::system::edges_t::const_iterator_t> outgoing_edges(tchecker::loc_id_t loc) const;

  /*!
   \brief Accessor
   \param loc : location identifier
   \param event : event identifier
   \return range of outgoing edges labelled event from loc
   \note the range follows the order of addition of edges
   */
  tchecker::range_t<tchecker::system::edges_t::const_iterator_t> outgoing_edges(tchecker::loc_id_t loc,
                                                                                tchecker::event_id_t event) const;

  /*!
   \brief Accessor
   \param loc : location identifier
   \return range of incoming edges of location loc
   */
  tchecker::range_t<tchecker::system::edges_t::const_iterator_t> incoming_edges(tchecker::loc_id_t loc) const;

  /*!
   \brief Accessor
   \param loc : location identifier
   \param event : event identifier
   \return range of incoming edges labelled event to loc
   \note the range follows the order of addition of edges
   */
  tchecker::range_t<tchecker::system::edges_t::const_iterator_t> incoming_edges(tchecker::loc_id_t loc,
                                                                                tchecker::event_id_t event) const;

  /*!
   \brief Accessor
   \param loc : location identifier
   \param event : event identifier
   \return true if loc has an outgoing edge labelled event, false otherwise
   \note Constant-time complexity
   */
  bool outgoing_event(tchecker::loc_id_t loc, tchecker::event_id_t event) const;

  /*!
   \brief Accessor
   \param loc : location identifier
   \param event : event identifier
   \return true if loc has an incoming edge labelled event, false otherwise
   \note Constant-time complexity
   */
  bool incoming_event(tchecker::loc_id_t loc, tchecker::event_id_t event) const;

  /*!
   \brief Accessor
   \return Maps : loc ID -> outgoing edges/events
   */
  std::shared_ptr<tchecker::system::loc_edges_maps_t const> outgoing_edges_maps() const;

  /*!
   \brief Accessor
   \return Maps : loc ID -> incoming edges/events
   */
  std::shared_ptr<tchecker::system::loc_edges_maps_t const> incoming_edges_maps() const;

  /*!
   \brief Accessor
   \param pid : process identifier
   \return range of edges in process pid
   */
  tchecker::range_t<tchecker::system::edges_t::const_iterator_t> edges(tchecker::process_id_t pid) const;

  /*!
   \brief Accessor
   \param pid : process identifier
   \param event_id : event identifier
   \return range of edges in process pid with event event_id
   */
  tchecker::range_t<tchecker::system::edges_t::const_iterator_t> edges(tchecker::process_id_t pid,
                                                                       tchecker::event_id_t event_id) const;

  /*!
   \brief Check validity of edge identifier
   \param id : edge identifier
   \return true is id is an edge identifier, false otherwise
   */
  bool is_edge(tchecker::edge_id_t id) const;

private:
  /*!
   \brief Add edges from a collection of edges
   \param edges : collection of edges
   \post all edges have been added to this collection
   */
  void add_edges(tchecker::system::edges_t const & edges);

  /*!< Collection of edges */
  tchecker::system::edges_collection_t _edges;
  /*!< Map : loc ID to incoming/outgoing edges/events */
  std::vector<std::shared_ptr<tchecker::system::loc_edges_maps_t>> _loc_edges_maps;
  /*!< Map : process ID to edges/events */
  tchecker::system::proc_edges_maps_t _proc_edges_map;
};

} // end of namespace system

} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_EDGE_HH
