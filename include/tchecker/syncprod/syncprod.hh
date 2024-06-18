/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYNCPROD_SYNCPROD_HH
#define TCHECKER_SYNCPROD_SYNCPROD_HH

#include <cstdlib>

#include <boost/dynamic_bitset/dynamic_bitset.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/syncprod/allocators.hh"
#include "tchecker/syncprod/edges_iterators.hh"
#include "tchecker/syncprod/state.hh"
#include "tchecker/syncprod/system.hh"
#include "tchecker/syncprod/transition.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/ts/builder.hh"
#include "tchecker/ts/bwd.hh"
#include "tchecker/ts/fwd.hh"
#include "tchecker/ts/inspector.hh"
#include "tchecker/ts/sharing.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file syncprod.hh
 \brief Synchronised product of a system of processes
 */

namespace tchecker {

namespace syncprod {

/* Initial edges */

/*!
 \brief Type of iterator over initial states
 */
using initial_iterator_t = tchecker::cartesian_iterator_t<tchecker::range_t<tchecker::system::locs_t::const_iterator_t>>;

/*!
\brief Type of range over initial states
*/
using initial_range_t = tchecker::range_t<tchecker::syncprod::initial_iterator_t, tchecker::end_iterator_t>;

/*!
 \brief Accessor to initial edges
 \param system : a system
 \return range of initial edges
 */
initial_range_t initial_edges(tchecker::syncprod::system_t const & system);

/*!
 \brief Dereference type for iterator over initial states
 */
using initial_value_t = std::iterator_traits<tchecker::syncprod::initial_iterator_t>::value_type;

/* Initial state */

/*!
 \brief Compute initial state
 \param system : a system
 \param vloc : tuple of locations
 \param vedge : tuple of edges
 \param sync_id : synchronization identifier
 \param initial_range : range of initial locations
 \pre the size of vloc and vedge is equal to the size of initial_range.
 initial_range has been obtained from system.
 initial_range yields the initial locations of all the processes ordered by increasing process identifier
 \post vloc has been initialized to the tuple of initial locations in initial_range.
 vedge has been initialized to an empty tuple of edges
 sync_id has been set to tchecker::NO_SYNC
 \return tchecker::STATE_OK
 \throw std::invalid_argument : if the size of vloc, vedge and initial_range do not coincide
 */
tchecker::state_status_t initial(tchecker::syncprod::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                                 tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                                 tchecker::syncprod::initial_value_t const & initial_range);

/*!
\brief Compute initial state and transition
\param system : a system
\param s : state
\param t : transition
\param v : initial iterator value
\post s has been initialized from v, and t is an empty transition
\return tchecker::STATE_OK
\throw std::invalid_argument : if s and v have incompatible sizes
*/
inline tchecker::state_status_t initial(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t & s,
                                        tchecker::syncprod::transition_t & t, tchecker::syncprod::initial_value_t const & v)
{
  return tchecker::syncprod::initial(system, s.vloc_ptr(), t.vedge_ptr(), t.sync_id(), v);
}

/* Final edges */

/*!
 \class final_iterator_t
 \brief Type of iterator over final edges
 \note this iterator ranges over the set of tuple of process locations in the given system, the
 size of which is exponential in the number of locations and processes
*/
class final_iterator_t {
  using location_tuples_iterator_t =
      tchecker::cartesian_iterator_t<tchecker::range_t<tchecker::system::locs_t::const_iterator_t>>;

public:
  /*!
   \brief Constructor
   \param system : system of synchronized processes
   \param final_labels : labels on final states
   \post this iterator ranges over final edges in system that match final_labels
  */
  final_iterator_t(tchecker::syncprod::system_t const & system, boost::dynamic_bitset<> final_labels);

  /*!
   \brief Copy contructor
  */
  final_iterator_t(tchecker::syncprod::final_iterator_t const &) = default;

  /*!
   \brief Move contructor
  */
  final_iterator_t(tchecker::syncprod::final_iterator_t &&) = default;

  /*!
   \brief Destructor
  */
  ~final_iterator_t() = default;

  /*!
   \brief Assignment operator
  */
  tchecker::syncprod::final_iterator_t & operator=(tchecker::syncprod::final_iterator_t const &) = delete;

  /*!
   \brief Move assignment operator
  */
  tchecker::syncprod::final_iterator_t & operator=(tchecker::syncprod::final_iterator_t &&) = delete;

  /*!
   \brief Equality check
  */
  bool operator==(tchecker::syncprod::final_iterator_t const & it) const;

  /*!
   \brief Equality check w.r.t. past-the-end iterator
  */
  bool operator==(tchecker::end_iterator_t const & it) const;

  /*!
   \brief Disequality check
  */
  bool operator!=(tchecker::syncprod::final_iterator_t const & it) const;

  /*!
   \brief Disequality check w.r.t. past-the-end iterator
  */
  bool operator!=(tchecker::end_iterator_t const & it) const;

  /*!
   \brief Type of values (range of tchecker::system::loc_const_shared_ptr_t)
  */
  using value_type_t = std::iterator_traits<location_tuples_iterator_t>::value_type;

  /*!
   \brief Dereference operator
  */
  tchecker::syncprod::final_iterator_t::value_type_t operator*();

  /*!
   \brief Increment operator
  */
  tchecker::syncprod::final_iterator_t & operator++();

private:
  /*!
   \brief Compute the set of labels in a range of locations
  */
  boost::dynamic_bitset<> locations_labels(value_type_t const & locations_range) const;

  /*!
   \brief Moves _it over final edges that do not match _final_labels
  */
  void advance_while_not_final();

  tchecker::syncprod::system_t const & _system; /*!< System of synchronized processes */
  boost::dynamic_bitset<> _final_labels;        /*!< Set of labels on final states */
  location_tuples_iterator_t _it;               /*! Iterator over cartesian product of process locations */
};

/*!
 \brief Type of range over final states
*/
using final_range_t = tchecker::range_t<tchecker::syncprod::final_iterator_t, tchecker::end_iterator_t>;

/*!
 \brief Accessor to final edges
 \param system : a system
 \param labels : a set of labels
 \return range of final edges, i.e. edges to tuple of locations that match labels
 */
tchecker::syncprod::final_range_t final_edges(tchecker::syncprod::system_t const & system,
                                              boost::dynamic_bitset<> const & labels);

/*!
 \brief Dereference type for iterator over final edges
 */
using final_value_t = tchecker::syncprod::final_iterator_t::value_type_t;

/* Final states */

/*!
 \brief Compute final state
 \param system : a system
 \param vloc : tuple of locations
 \param vedge : tuple of edges
 \param sync_id : synchronization identifier
 \param v : value from final iterator (range of locations)
 \pre the size of vloc and vedge is equal to the size of v
 v has been obtained from system.
 v yields locations of all the processes ordered by increasing process identifier in a final state
 \post vloc has been initialized to the tuple of locations in v.
 vedge has been initialized to an empty tuple of edges
 sync_id has been set to tchecker::NO_SYNC
 \return tchecker::STATE_OK
 \throw std::invalid_argument : if the size of vloc, vedge and v do not coincide
 */
tchecker::state_status_t final(tchecker::syncprod::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                               tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                               tchecker::syncprod::final_value_t const & v);

/*!
 \brief Compute final state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param v : value from final iterator (range of locations)
 \post s has been initialized from v, and t is an empty transition
 \return tchecker::STATE_OK
 \throw std::invalid_argument : if s and v have incompatible sizes
*/
inline tchecker::state_status_t final(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t & s,
                                      tchecker::syncprod::transition_t & t, tchecker::syncprod::initial_value_t const & v)
{
  return tchecker::syncprod::final(system, s.vloc_ptr(), t.vedge_ptr(), t.sync_id(), v);
}

/* Outgoing edges */

/*!
\class outgoing_edges_iterator_t
\brief Outgoing edges iterator taking committed processes into account. Iterates
over the outgoing edges that involve a committed process (if any), or over all
outgoing edges if no process is committed
*/
class outgoing_edges_iterator_t {
public:
  /*!
  \brief Constructor
  \param sync_it : iterator over synchronized edges
  \param async_it : iterator over asynchronous edges
  \param committed_procs : set of committed processes
  */
  outgoing_edges_iterator_t(tchecker::syncprod::vloc_synchronized_edges_iterator_t const & sync_it,
                            tchecker::syncprod::vloc_asynchronous_edges_iterator_t const & async_it,
                            boost::dynamic_bitset<> committed_processes);

  /*!
  \brief Constructor
  \param it : vloc edges iterator
  \param committed_procs : set of committed processes
  */
  outgoing_edges_iterator_t(tchecker::syncprod::vloc_edges_iterator_t const & it, boost::dynamic_bitset<> committed_processes);

  /*!
  \brief Copy constructor
  */
  outgoing_edges_iterator_t(tchecker::syncprod::outgoing_edges_iterator_t const &) = default;

  /*!
  \brief Move constructor
  */
  outgoing_edges_iterator_t(tchecker::syncprod::outgoing_edges_iterator_t &&) = default;

  /*!
  \brief Destructor
  */
  ~outgoing_edges_iterator_t() = default;

  /*!
  \brief Assignment operator
  */
  tchecker::syncprod::outgoing_edges_iterator_t & operator=(tchecker::syncprod::outgoing_edges_iterator_t const &) = default;

  /*!
  \brief Move-assignment operator
  */
  tchecker::syncprod::outgoing_edges_iterator_t & operator=(tchecker::syncprod::outgoing_edges_iterator_t &&) = default;

  /*!
  \brief Equality predicate
  \param it : iterator
  \return true if underlying vloc edges iterators are equal, and committed
  processes are equal, false otherwise
  */
  bool operator==(tchecker::syncprod::outgoing_edges_iterator_t const & it) const;

  /*!
  \brief Disequality predicate
  \param it : iterator
  \return opposite result to operator==
  */
  inline bool operator!=(tchecker::syncprod::outgoing_edges_iterator_t const & it) const { return !(*this == it); }

  /*!
  \brief Equality predicate w.r.t. past-the-end iterator
  \param it : past-the-end iterator
  \return true if at_end(), false otherwise
  */
  bool operator==(tchecker::end_iterator_t const & it) const;

  /*!
  \brief Disequality predicate w.r.t. past-the-end iterator
  \param it : past-the-end iterator
  \return opposite to operator==
  */
  inline bool operator!=(tchecker::end_iterator_t const & it) const { return !(*this == it); }

  /*!
   \brief Type of synchronized edges, i.e. pairs (synchronization id, range of edges)
   */
  using sync_edges_t = tchecker::syncprod::vloc_edges_iterator_t::sync_edges_t;

  /*!
  \brief Dereference operator
  \pre not at_end() (checked by assertion)
  \return a pair (sync_id, edges) where sync_if is the identifier of the synchronization
  instantiated by the range edges (sync_id is tchecker::NO_SYNC if edges is
  asynchronous)
  \note return synchronized edges is invalidated by operator++
  */
  sync_edges_t operator*();

  /*!
   \brief Move to next
   \pre not at_end() (checked by assertion)
   \post this points to next tuple of edges (if any) that moves a committed
   process if any, or next edge if no committed process
   \return this after increment
   \note invaldates synchronized edges returned by operator*
   */
  tchecker::syncprod::outgoing_edges_iterator_t & operator++();

private:
  /*!
  \brief Move iterator forward
  */
  void advance_while_not_enabled();

  /*!
  \brief Checks if a range of edge involves a committed process
  \param r : range of edges
  \return true if r involves a committed process, false otherwise
  */
  bool involves_committed_process(tchecker::range_t<tchecker::syncprod::edges_iterator_t> const & r) const;

  /*!
  \brief Checks if underlying iterator _it is past-the-end
  */
  bool at_end() const;

  tchecker::syncprod::vloc_edges_iterator_t _it; /*!< Underlying vloc edges iterator */
  boost::dynamic_bitset<> _committed_processes;  /*!< Map : PID -> committed flag */
  bool _committed;                               /*!< Flag : whether _committed_procs has a committed process or not */
};

/*!
\brief Type of range over outgoing edges
*/
using outgoing_edges_range_t = tchecker::range_t<tchecker::syncprod::outgoing_edges_iterator_t, tchecker::end_iterator_t>;

/*!
 \brief Accessor to outgoing edges
 \param system : a system
 \param vloc : tuple of locations
 \return range of outgoing synchronized and asynchronous edges from vloc in system
 */
tchecker::syncprod::outgoing_edges_range_t outgoing_edges(tchecker::syncprod::system_t const & system,
                                                          tchecker::const_vloc_sptr_t const & vloc);

/*!
 \brief Dereference type of outgoing_edges_iterator_t
 */
using outgoing_edges_value_t = tchecker::syncprod::outgoing_edges_iterator_t::sync_edges_t;

/* Next states */

/*!
 \brief Compute next tuples of locations and edges
 \param vloc : tuple of locations
 \param vedge : tuple of edges
 \param sync_id : synchronization identifier
 \param sync_edges : synchronized edges from vloc
 \pre the source locations of the edges in sync_edges match the locations in vloc,
 no process has more than one edge in sync_edges,
 and the pid of every process in sync_edges is less than the size of vloc
 \post vloc has been updated according to target locations of the edges in sync_edges for involves processes,
 and the others are left unchanged
 vedge contains the identifiers of the edges in sync_edges
 vedge is an instance of synchronization sync_id, unless sync_id is tchecker::NO_SYNC (asynchronous vedge)
 \return tchecker::STATE_OK if the source locations in edges match the locations in vloc,
 tchecker::STATE_INCOMPATIBLE_EDGE otherwise
 \throw std::invalid_argument : if the sizes of vloc and vedge do not match, or
 if the pid of an edge in sync_edges is greater or equal to the size of vloc/vedge
 */
tchecker::state_status_t next(tchecker::syncprod::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                              tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                              tchecker::syncprod::outgoing_edges_value_t const & sync_edges);

/*!
\brief Compute next state and transition
\param system : a system
\param s : state
\param t : transition
\param sync_edges : outgoing synchronized edges
\post s have been updated from sync_edges, and t is the set of edges in sync_edges
as well as the synchronization identifier (or tchecker::NO_SYNC if t is asynchronous)
\return status of state s after update
\throw std::invalid_argument : if s and sync_edges have incompatible size
*/
inline tchecker::state_status_t next(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t & s,
                                     tchecker::syncprod::transition_t & t,
                                     tchecker::syncprod::outgoing_edges_value_t const & sync_edges)
{
  return tchecker::syncprod::next(system, s.vloc_ptr(), t.vedge_ptr(), t.sync_id(), sync_edges);
}

/* Incoming edges */

/*!
\class incoming_edges_iterator_t
\brief Incoming edges iterator taking committed processes into account. Iterates
over the incoming edges that involve a committed process (if any), or over all
incoming edges if no process is committed
\note this iterator ranges over the set of tuple of process locations in the given system,
the size of which is exponential in the number of locations and processes
*/
class incoming_edges_iterator_t {
public:
  /*!
  \brief Constructor
  \param sync_it : iterator over synchronized edges
  \param async_it : iterator over asynchronous edges
  \param committed_locs : set of committed locations IDs
  \param committed_processes_tgt : set of committed processes in target tuple of locations
  */
  incoming_edges_iterator_t(tchecker::syncprod::vloc_synchronized_edges_iterator_t const & sync_it,
                            tchecker::syncprod::vloc_asynchronous_edges_iterator_t const & async_it,
                            boost::dynamic_bitset<> const & committed_locs,
                            boost::dynamic_bitset<> const & committed_processes_tgt);

  /*!
  \brief Constructor
  \param it : vloc edges iterator
  \param committed_locs : set of committed locations IDs
  \param committed_processes_tgt : set of committed processes in target tuple of locations
  */
  incoming_edges_iterator_t(tchecker::syncprod::vloc_edges_iterator_t const & it,
                            boost::dynamic_bitset<> const & committed_locs,
                            boost::dynamic_bitset<> const & committed_processes_tgt);

  /*!
  \brief Copy constructor
  */
  incoming_edges_iterator_t(tchecker::syncprod::incoming_edges_iterator_t const &) = default;

  /*!
  \brief Move constructor
  */
  incoming_edges_iterator_t(tchecker::syncprod::incoming_edges_iterator_t &&) = default;

  /*!
  \brief Destructor
  */
  ~incoming_edges_iterator_t() = default;

  /*!
  \brief Assignment operator
  */
  tchecker::syncprod::incoming_edges_iterator_t & operator=(tchecker::syncprod::incoming_edges_iterator_t const &) = default;

  /*!
  \brief Move-assignment operator
  */
  tchecker::syncprod::incoming_edges_iterator_t & operator=(tchecker::syncprod::incoming_edges_iterator_t &&) = default;

  /*!
  \brief Equality predicate
  \param it : iterator
  \return true if underlying vloc edges iterators are equal, and committed
  processes are equal, false otherwise
  */
  bool operator==(tchecker::syncprod::incoming_edges_iterator_t const & it) const;

  /*!
  \brief Disequality predicate
  \param it : iterator
  \return opposite result to operator==
  */
  inline bool operator!=(tchecker::syncprod::incoming_edges_iterator_t const & it) const { return !(*this == it); }

  /*!
  \brief Equality predicate w.r.t. past-the-end iterator
  \param it : past-the-end iterator
  \return true if at_end(), false otherwise
  */
  bool operator==(tchecker::end_iterator_t const & it) const;

  /*!
  \brief Disequality predicate w.r.t. past-the-end iterator
  \param it : past-the-end iterator
  \return opposite to operator==
  */
  inline bool operator!=(tchecker::end_iterator_t const & it) const { return !(*this == it); }

  /*!
   \brief Type of synchronized edges, i.e. pairs (synchronization id, range of edges)
   */
  using sync_edges_t = tchecker::syncprod::vloc_edges_iterator_t::sync_edges_t;

  /*!
  \brief Dereference operator
  \pre not at_end() (checked by assertion)
  \return a pair (sync_id, edges) where sync_if is the identifier of the synchronization
  instantiated by the range edges (sync_id is tchecker::NO_SYNC if edges is
  asynchronous)
  \note return synchronizd edges is invalidated by operator++
  */
  sync_edges_t operator*();

  /*!
   \brief Move to next
   \pre not at_end() (checked by assertion)
   \post this points to next tuple of edges (if any) that moves a committed
   process if any, or next edge if no committed process
   \return this after increment
   \note invaldates synchronized edges returned by operator*
   */
  tchecker::syncprod::incoming_edges_iterator_t & operator++();

private:
  /*!
  \brief Move iterator forward
  */
  void advance_while_not_enabled();

  /*!
  \brief Checks if a range of edge is enabled w.r.t. committed locations
  \param r : range of edges
  \return true if either r involves a committed process, or all the processes
  not involved in r are not committed, false otherwise
  */
  bool enabled_wrt_committed_processes(tchecker::range_t<tchecker::syncprod::edges_iterator_t> const & r) const;

  /*!
  \brief Checks if underlying iterator _it is past-the-end
  */
  bool at_end() const;

  tchecker::syncprod::vloc_edges_iterator_t _it;    /*!< Underlying vloc edges iterator */
  boost::dynamic_bitset<> _committed_locs;          /*!< Map : loc_id_t -> committed flag */
  boost::dynamic_bitset<> _committed_processes_tgt; /*!< Committed processes in target tuple of locations */
};

/*!
\brief Type of range over incoming edges
*/
using incoming_edges_range_t = tchecker::range_t<tchecker::syncprod::incoming_edges_iterator_t, tchecker::end_iterator_t>;

/*!
 \brief Accessor to incoming edges
 \param system : a system
 \param vloc : tuple of locations
 \return range of incoming synchronized and asynchronous edges to vloc in system
 */
tchecker::syncprod::incoming_edges_range_t incoming_edges(tchecker::syncprod::system_t const & system,
                                                          tchecker::const_vloc_sptr_t const & vloc);

/*!
 \brief Type of incoming vedge
 \note type dereferenced by outgoing_edges_iterator_t, corresponds to tchecker::vedge_iterator_t
 */
using incoming_edges_value_t = tchecker::syncprod::incoming_edges_iterator_t::sync_edges_t;

static_assert(std::is_same<tchecker::syncprod::outgoing_edges_value_t, tchecker::syncprod::incoming_edges_value_t>::value,
              "Outgoing and incoming edges values should be the same type");

/* Previous state */

/*!
 \brief Compute previous tuples of locations and edges
 \param vloc : tuple of locations
 \param vedge : tuple of edges
 \param sync_id : synchronization identifier
 \param sync_edges : sychronized edges to vloc
 \pre the target locations of the edges in sync_edges match the locations in vloc,
 no process has more than one edge in sync_edges,
 and the pid of every process in sync_edges is less than the size of vloc
 \post vloc has been updated according to source locations of edges in sync_edges for processes involved in sync_edges.
 vedge contains the edge identifiers of the edges in sync_edges
 vedge is an instance of the synchronization sync_id, unless sync_id is tchecker::NO_SYNC (asynchronous vedge)
 \return tchecker::STATE_OK if the target locations in the edges in sync_edges match the locations in vloc,
 tchecker::STATE_INCOMPATIBLE_EDGE otherwise
 \throw std::invalid_argument : if the sizes of vloc and vedge do not match, or
 if the pid of an edge in sync_edges is greater or equal to the size of vloc/vedge
 */
tchecker::state_status_t prev(tchecker::syncprod::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                              tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                              tchecker::syncprod::outgoing_edges_value_t const & sync_edges);

/*!
\brief Compute previous state and transition
\param system : a system
\param s : state
\param t : transition
\param sync_edges : incoming synchronized edges
\post s have been updated from sync_edges, and t is the set of edges in sync_edges
along with the synchronization identifier in sync_edges
\return status of state s after update
\throw std::invalid_argument : if s and sync_edges have incompatible size
*/
inline tchecker::state_status_t prev(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t & s,
                                     tchecker::syncprod::transition_t & t, tchecker::syncprod::incoming_edges_value_t const & v)
{
  return tchecker::syncprod::prev(system, s.vloc_ptr(), t.vedge_ptr(), t.sync_id(), v);
}

/* Labels */

/*!
 \brief Compute set of committed processes in a vloc
 \param system : a system
 \param vloc : tuple of locations
 \return the set of processes from system that are committed in vloc
*/
boost::dynamic_bitset<> committed_processes(tchecker::syncprod::system_t const & system,
                                            tchecker::const_vloc_sptr_t const & vloc);

/*!
 \brief Compute labels in a tuple of locations
 \param system : a system of timed processes
 \param vloc : tuple of locations
 \return a dynamic bitset of size system.labels_count() that contains all labels
 on locations in vloc
 */
boost::dynamic_bitset<> labels(tchecker::syncprod::system_t const & system, tchecker::vloc_t const & vloc);

/*!
 \brief Compute labels of a state
 \param system : a system
 \param s : a state
 \return a dynamic bitset of size system.labels_count() that contains all labels
 on state s
*/
boost::dynamic_bitset<> labels(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t const & s);

/*!
 \brief Compute string representation of the labels of in state
 \param system : a system
 \param s : a state
 \return a comma-separated list of the labels in state s
*/
std::string labels_str(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t const & s);

/* Inspector */

/*!
 \brief Checks is a tuple of locations is a valid final state
 \param system : a system
 \param vloc : a tuple of locations
 \return true
*/
bool is_valid_final(tchecker::syncprod::system_t const & system, tchecker::vloc_t const & vloc);

/*!
 \brief Checks is a state is a valid final state
 \param system : a system
 \param s : a state
 \return true
*/
bool is_valid_final(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t const & s);

/*!
 \brief Checks if a tuple of locations is initial
 \param system : a system
 \param vloc : tuple of locations
 \return true if all locations in vloc are initial in system, false otherwise
 */
bool is_initial(tchecker::syncprod::system_t const & system, tchecker::vloc_t const & vloc);

/*!
 \brief Checks if a state is initial
 \param system : a system
 \param s : state
 \return true if all locations in s are initial in system, false otherwise
 */
bool is_initial(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t const & s);

/* Attributes */

/*!
 \brief Accessor to state attributes as strings
 \param system : a system
 \param s : a state
 \param m : a map of string pairs (key, value)
 \post attributes of state s have been added to map m
 */
void attributes(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t const & s,
                std::map<std::string, std::string> & m);

/*!
 \brief Accessor to transition attributes as strings
 \param system : a system
 \param t : a transition
 \param m : a map of string pairs (key, value)
 \post attributes of transition t have been added to map m
 */
void attributes(tchecker::syncprod::system_t const & system, tchecker::syncprod::transition_t const & t,
                std::map<std::string, std::string> & m);

/* Initialize */

/*!
 \brief Initialization from attributes
 \param system : a system
 \param vloc : a vector of locations
 \param vedge : a vector of edges
 \param sync_id : synchronization identifier
 \param attributes : map of attributes
 \pre attributes["vloc"] is defined and follows the syntax required by function
 tchecker::from_string(tchecker::vloc_t &, tchecker::system::system_t const &, std::string const &);
 \post vloc has been initialized from attributes["vloc"]
 vedge is the empty vector of edges
 sync_id is tchecker::NO_SYNC
 \return tchecker::STATE_OK if initialization succeeded
 tchecker::STATE_BAD if initialization failed
 */
tchecker::state_status_t initialize(tchecker::syncprod::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                                    tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                                    std::map<std::string, std::string> const & attributes);

/*!
 \brief Initialization from attributes
 \param system : a system
 \param s : state
 \param t : transition
 \param attributes : map of attributes
 \post s has been initialized from attributes["vloc"]
 t is an empty transition
 \return tchecker::STATE_OK if initialization succeeded
 tchecker::STATE_BAD otherwise
*/
inline tchecker::state_status_t initialize(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t & s,
                                           tchecker::syncprod::transition_t & t,
                                           std::map<std::string, std::string> const & attributes)
{
  return tchecker::syncprod::initialize(system, s.vloc_ptr(), t.vedge_ptr(), t.sync_id(), attributes);
}

/* syncprod_t */

/*!
 \class syncprod_t
 \brief Transition system of the synchronized product of timed processes with
 state and transition allocation
 \note all returned states and transitions are deallocated automatically
 */
class syncprod_t final
    : public tchecker::ts::fwd_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                                 tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t>,
      public tchecker::ts::bwd_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                                 tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t>,
      public tchecker::ts::fwd_impl_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                                      tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t,
                                      tchecker::syncprod::initial_range_t, tchecker::syncprod::outgoing_edges_range_t,
                                      tchecker::syncprod::initial_value_t, tchecker::syncprod::outgoing_edges_value_t>,
      public tchecker::ts::bwd_impl_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                                      tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t,
                                      tchecker::syncprod::final_range_t, tchecker::syncprod::incoming_edges_range_t,
                                      tchecker::syncprod::final_value_t, tchecker::syncprod::incoming_edges_value_t>,
      public tchecker::ts::builder_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::transition_sptr_t>,
      public tchecker::ts::inspector_t<tchecker::syncprod::const_state_sptr_t, tchecker::syncprod::const_transition_sptr_t>,
      public tchecker::ts::sharing_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::transition_sptr_t> {
public:
  // Inherited types

  using fwd_t = tchecker::ts::fwd_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                                    tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t>;
  using bwd_t = tchecker::ts::bwd_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                                    tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t>;
  using fwd_impl_t =
      tchecker::ts::fwd_impl_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                               tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t,
                               tchecker::syncprod::initial_range_t, tchecker::syncprod::outgoing_edges_range_t,
                               tchecker::syncprod::initial_value_t, tchecker::syncprod::outgoing_edges_value_t>;
  using bwd_impl_t =
      tchecker::ts::bwd_impl_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                               tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t,
                               tchecker::syncprod::final_range_t, tchecker::syncprod::incoming_edges_range_t,
                               tchecker::syncprod::final_value_t, tchecker::syncprod::incoming_edges_value_t>;
  using builder_t = tchecker::ts::builder_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::transition_sptr_t>;
  using inspector_t =
      tchecker::ts::inspector_t<tchecker::syncprod::const_state_sptr_t, tchecker::syncprod::const_transition_sptr_t>;
  using sharing_t = tchecker::ts::sharing_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::transition_sptr_t>;

  using sst_t = fwd_t::sst_t;
  using state_t = fwd_t::state_t;
  using const_state_t = fwd_t::const_state_t;
  using transition_t = fwd_t::transition_t;
  using const_transition_t = fwd_t::const_transition_t;
  using initial_range_t = fwd_impl_t::initial_range_t;
  using initial_value_t = fwd_impl_t::initial_value_t;
  using outgoing_edges_range_t = fwd_impl_t::outgoing_edges_range_t;
  using outgoing_edges_value_t = fwd_impl_t::outgoing_edges_value_t;
  using final_range_t = bwd_impl_t::final_range_t;
  using final_value_t = bwd_impl_t::final_value_t;
  using incoming_edges_range_t = bwd_impl_t::incoming_edges_range_t;
  using incoming_edges_value_t = bwd_impl_t::incoming_edges_value_t;

  /*!
   \brief Constructor
   \param system : a system of timed processes
   \param sharing_type : type of state/transition sharing
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash tables
   \note all states and transitions are pool allocated and deallocated automatically. Components
   within states and transitions are shared if sharing_type is tchecker::ts::SHARING
   */
  syncprod_t(std::shared_ptr<tchecker::syncprod::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
             std::size_t block_size, std::size_t table_size);

  /*!
   \brief Copy constructor (deleted)
   */
  syncprod_t(tchecker::syncprod::syncprod_t const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  syncprod_t(tchecker::syncprod::syncprod_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~syncprod_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::syncprod::syncprod_t & operator=(tchecker::syncprod::syncprod_t const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::syncprod::syncprod_t & operator=(tchecker::syncprod::syncprod_t &&) = delete;

  using fwd_t::state;
  using fwd_t::status;
  using fwd_t::transition;

  // Forward

  /*!
   \brief Accessor
   \return initial edges
  */
  virtual initial_range_t initial_edges();

  /*!
   \brief Initial state and transition from an initial edge
   \param init_edge : initial state valuation
   \param v : container
   \param mask : mask on next states
   \post triples (status, s, t) have been added to v, for each initial state s
   and initial transition t from init_edge, such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void initial(initial_value_t const & init_edge, std::vector<sst_t> & v,
                       tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
   \brief Initial state and transition with selected status
   \param init_edge : initial state valuation
   \param v : container
   \param mask : mask on next states
   \post triples (status, s, t) have been added to v, for each initial state s
   and initial transition t from init_edge, such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void initial(std::vector<sst_t> & v, tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
   \brief Accessor
   \param s : state
   \return outgoing edges from state s
   */
  virtual outgoing_edges_range_t outgoing_edges(tchecker::syncprod::const_state_sptr_t const & s);

  /*!
   \brief Next state and transition from an outgoing edge
   \param s : state
   \param out_edge : outgoing edge value
   \param v : container
   \param mask : mask on next states
   \post triples (status, s', t') have been added to v, for each transition s -t'-> s' along outgoing
   edge out_edge such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void next(tchecker::syncprod::const_state_sptr_t const & s, outgoing_edges_value_t const & out_edge,
                    std::vector<sst_t> & v, tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
  \brief Next states and transitions with selected status
  \param s : state
  \param v : container
  \param mask : mask on next states
  \post all tuples (status, s', t) such that s -t-> s' is a transition and the
  status of s' matches mask (i.e. status & mask != 0) have been pushed to v
  \note states and transitions that are added to v are deallocated automatically
  \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
  */
  virtual void next(tchecker::syncprod::const_state_sptr_t const & s, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

  // Backward

  /*!
   \brief Accessor
   \param labels : final states labels
   \return Final edges
   \note the return range goes across all tuples of process locations in the given system,
   the size of which is exponential in the number of locations and processes
   */
  virtual final_range_t final_edges(boost::dynamic_bitset<> const & labels);

  /*!
   \brief Final states and transitions from a final edge
   \param final_edge : final edge valuation
   \param v : container
   \param mask : mask on initial states
   \post triples (status, s, t) have been added to v, for each final state s and transition t
   such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void final(final_value_t const & final_edge, std::vector<sst_t> & v,
                     tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
  \brief Final states and transitions with selected status
  \param labels : set of final labels
  \param v : container
  \param mask : mask on initial states
  \post all tuples (status, s, t) of status, states and transitions that satisfy
  labels, and such that status matches mask (i.e. status & mask != 0), have been
  pushed back into v
  \note states and transitions that are added to v are deallocated automatically
  \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
  \note complexity is exponential in the number of locations and processes in the underlying
  system
  */
  virtual void final(boost::dynamic_bitset<> const & labels, std::vector<sst_t> & v,
                     tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
   \brief Accessor
   \param s : state
   \return incoming edges to state s
   \note the return range goes across all tuples of process locations in the given system. The number of
   such tuples is exponential in the number of locations and processes
   */
  virtual incoming_edges_range_t incoming_edges(tchecker::syncprod::const_state_sptr_t const & s);

  /*!
   \brief Previous state and transition from an incoming edge
   \param s : state
   \param in_edge : incoming edge value
   \param v : container
   \param mask : mask on initial states
   \post triples (status, s', t') have been added to v, for each incoming transition s'-t'->s
   along in_edge such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void prev(tchecker::syncprod::const_state_sptr_t const & s, incoming_edges_value_t const & in_edge,
                    std::vector<sst_t> & v, tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
  \brief Previous states and transitions with selected status
  \param s : state
  \param v : container
  \param mask : mask on next states
  \post all tuples (status, s', t) such that s' -t-> s is a transition and the
  status of s' matches mask (i.e. status & mask != 0) have been pushed to v
  \note states and transitions that are added to v are deallocated automatically
  \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
  \note complexity is exponential in the number of locations and processes, as well as the domains of
  bounded integer valuations in the underlying system
  */
  virtual void prev(tchecker::syncprod::const_state_sptr_t const & s, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

  // Builder

  /*!
   \brief State/transition building from attributes
   \param attributes : a map of attributes
   \param v : container
   \param mask : mask on states
   \post all tuples (status, s, t) where s and t have been initialized from attributes,
   and status matches mask (i.e. status & mask != 0) have been pushed to v
   \pre see tchecker::syncprod::initialize
   \post a triple <status, s, t> where the vector of locations in s has been initialized
   from attributes["vloc"] and the vector of edges in t is empty, has been pushed to v
   if status matched mask
  */
  virtual void build(std::map<std::string, std::string> const & attributes, std::vector<sst_t> & v,
                     tchecker::state_status_t mask = tchecker::STATE_OK);

  // Inspector

  /*!
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  virtual boost::dynamic_bitset<> labels(tchecker::syncprod::const_state_sptr_t const & s) const;

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post the tuple of locations in s as been added to m
   */
  virtual void attributes(tchecker::syncprod::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post the tuple of edges in t has been added to t
   */
  virtual void attributes(tchecker::syncprod::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const;

  /*!
  \brief Checks if a state is a valid final state
  \param s : a state
  \return true if a run ending in s is a valid run, false otherwise
  */
  virtual bool is_valid_final(tchecker::syncprod::const_state_sptr_t const & s) const;

  /*!
   \brief Checks if a state is initial
   \param s : a state
   \return true if s is an initial state, false otherwise
  */
  virtual bool is_initial(tchecker::syncprod::const_state_sptr_t const & s) const;

  // Sharing

  /*!
   \brief Share state components
   \param s : a state
   \post internal components in s have been shared
   \note THE RESULTING STATE SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::syncprod::state_sptr_t & s);

  /*!
   \brief Share transition components
   \param t : a transition
   \post internal components in t have been shared
   \note THE RESULTING TRANSITION SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::syncprod::transition_sptr_t & t);

  /*!
   \brief Accessor
   \return Underlying system of timed processes
   */
  inline tchecker::syncprod::system_t const & system() const { return *_system; }

  /*!
   \brief Accessor
   \return sharing type of this synchronized product
  */
  inline enum tchecker::ts::sharing_type_t sharing_type() const { return _sharing_type; }

private:
  std::shared_ptr<tchecker::syncprod::system_t const> _system;           /*!< System of timed processes */
  enum tchecker::ts::sharing_type_t _sharing_type;                       /*!< Sharing of state/transition components */
  tchecker::syncprod::state_pool_allocator_t _state_allocator;           /*!< Allocator of states */
  tchecker::syncprod::transition_pool_allocator_t _transition_allocator; /*!< Allocator of transitions */
};

} // end of namespace syncprod

} // end of namespace tchecker

#endif // TCHECKER_SYNCPROD_SYNCPROD_HH
