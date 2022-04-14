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

#include "tchecker/basictypes.hh"
#include "tchecker/syncprod/allocators.hh"
#include "tchecker/syncprod/edges_iterators.hh"
#include "tchecker/syncprod/state.hh"
#include "tchecker/syncprod/system.hh"
#include "tchecker/syncprod/transition.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/ts/ts.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file syncprod.hh
 \brief Synchronised product of a system of processes
 */

namespace tchecker {

namespace syncprod {

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

/*!
 \brief Compute initial state
 \param system : a system
 \param vloc : tuple of locations
 \param vedge : tuple of edges
 \param initial_range : range of initial locations
 \pre the size of vloc and vedge is equal to the size of initial_range.
 initial_range has been obtained from system.
 initial_range yields the initial locations of all the processes ordered by increasing process identifier
 \post vloc has been initialized to the tuple of initial locations in initial_range.
 vedge has been initialized to an empty tuple of edges
 \return tchecker::STATE_OK
 \throw std::invalid_argument : if the size of vloc, vedge and initial_range do not coincide
 */
tchecker::state_status_t initial(tchecker::syncprod::system_t const & system,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
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
  return tchecker::syncprod::initial(system, s.vloc_ptr(), t.vedge_ptr(), v);
}

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
  \brief Dereference operator
  \pre not at_end() (checked by assertion)
  \return Range of iterator over collection of edges pointed to by this
  \note return range is invalidated by operator++
  */
  tchecker::range_t<tchecker::syncprod::edges_iterator_t> operator*();

  /*!
   \brief Move to next
   \pre not at_end() (checked by assertion)
   \post this points to next tuple of edges (if any) that moves a committed
   process if any, or next edge if no committed process
   \return this after increment
   \note invaldates ranges returned by operator*
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
tchecker::syncprod::outgoing_edges_range_t
outgoing_edges(tchecker::syncprod::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc);

/*!
 \brief Type of outgoing vedge
 \note type dereferenced by outgoing_edges_iterator_t, corresponds to tchecker::vedge_iterator_t
 */
using outgoing_edges_value_t = tchecker::range_t<tchecker::syncprod::edges_iterator_t>;

/*!
 \brief Compute next state
 \param vloc : tuple of locations
 \param vedge : tuple of edges
 \param edges : range of edges in a asynchronous/synchronized edge from vloc
 \pre the source locations of edges match the locations in vloc,
 no process has more than one edge in vedge,
 and the pid of every process in vedge is less than the size of vloc
 \post the locations in vloc have been updated to target locations of edges for processes involved in edges.
 They and have been left unchanged for the other processes.
 vedge contains the identifiers of the edges in edges
 \return tchecker::STATE_OK if the sources locations in edges match the locations in vloc,
 tchecker::STATE_INCOMPATIBLE_EDGE otherwise
 \throw std::invalid_argument : if the sizes of vloc and vedge do not match, or
 if the pid of an edge in edges is greater or equal to the size of vloc/vedge
 */
tchecker::state_status_t next(tchecker::syncprod::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::syncprod::outgoing_edges_value_t const & edges);

/*!
\brief Compute next state and transition
\param system : a system
\param s : state
\param t : transition
\param v : outgoing edge value
\post s have been updated from v, and t is the set of edges in v
\return status of state s after update
\throw std::invalid_argument : if s and v have incompatible size
*/
inline tchecker::state_status_t next(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t & s,
                                     tchecker::syncprod::transition_t & t, tchecker::syncprod::outgoing_edges_value_t const & v)
{
  return tchecker::syncprod::next(system, s.vloc_ptr(), t.vedge_ptr(), v);
}

/*!
 \brief Compute set of committed processes in a vloc
 \param system : a system
 \param vloc : tuple of locations
 \return the set of processes from system that are committed in vloc
*/
boost::dynamic_bitset<> committed_processes(tchecker::syncprod::system_t const & system,
                                            tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc);

/*!
 \brief Compute labels in a tuple of locations
 \param system : a system of timed processes
 \param vloc : tuple of locations
 \return a dynamic bitset of size system.labels_count() that contains all labels
 on locations in vloc
 */
boost::dynamic_bitset<> labels(tchecker::syncprod::system_t const & system, tchecker::vloc_t const & vloc);

/*!
 \brief Checks if a vloc satisfies a set of labels
 \param system : a system
 \param vloc : tuple of locations
 \param labels : set of labels
 \return true if labels is not empty and labels is included in the set of labels
 in vloc, false otherwise
*/
bool satisfies(tchecker::syncprod::system_t const & system, tchecker::vloc_t const & vloc,
               boost::dynamic_bitset<> const & labels);

/*!
 \brief Checks if a state satisfies a set of labels
 \param system : a system
 \param s : a state
 \param labels : set of labels
 \return true if labels is not empty and labels is included in the set of labels
 in s, false otherwise
*/
bool satisfies(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t const & s,
               boost::dynamic_bitset<> const & labels);

/*!
 \brief Checks if a tuple of locations is initial
 \param system : a system
 \param vloc : tuple of locations
 \return true if all locations in vloc are initial in system, false otherwise
 */
bool is_initial(tchecker::syncprod::system_t const & system, tchecker::vloc_t const & vloc);

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

/*!
 \class syncprod_t
 \brief Synchronized product of timed processes with state and transition
 allocation
 \note all returned states and transitions deallocated automatically
 */
class syncprod_t final
    : public tchecker::ts::full_ts_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                                     tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t,
                                     tchecker::syncprod::initial_range_t, tchecker::syncprod::outgoing_edges_range_t,
                                     tchecker::syncprod::initial_value_t, tchecker::syncprod::outgoing_edges_value_t> {
public:
  /*!
   \brief Constructor
   \param system : a system of timed processes
   \param block_size : number of objects allocated in a block
   \note all states and transitions are pool allocated and deallocated automatically
   */
  syncprod_t(std::shared_ptr<tchecker::syncprod::system_t const> const & system, std::size_t block_size);

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

  using tchecker::ts::full_ts_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                                tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t,
                                tchecker::syncprod::initial_range_t, tchecker::syncprod::outgoing_edges_range_t,
                                tchecker::syncprod::initial_value_t, tchecker::syncprod::outgoing_edges_value_t>::status;

  using tchecker::ts::full_ts_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                                tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t,
                                tchecker::syncprod::initial_range_t, tchecker::syncprod::outgoing_edges_range_t,
                                tchecker::syncprod::initial_value_t, tchecker::syncprod::outgoing_edges_value_t>::state;

  using tchecker::ts::full_ts_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                                tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t,
                                tchecker::syncprod::initial_range_t, tchecker::syncprod::outgoing_edges_range_t,
                                tchecker::syncprod::initial_value_t, tchecker::syncprod::outgoing_edges_value_t>::transition;

  /*!
   \brief Accessor
   \return initial edges
   */
  virtual tchecker::syncprod::initial_range_t initial_edges();

  /*!
   \brief Initial state and transition
   \param init_edge : initial state valuation
   \param v : container
   \post triples (status, s, t) have been added to v, for each initial state s
   and initial transition t that are initialized from init_edge.
   \note states and transitions that are added to v are deallocated automatically
   */
  virtual void initial(tchecker::syncprod::initial_value_t const & init_edge, std::vector<sst_t> & v);

  /*!
   \brief Accessor
   \param s : state
   \return outgoing edges from state s
   */
  virtual tchecker::syncprod::outgoing_edges_range_t outgoing_edges(tchecker::syncprod::const_state_sptr_t const & s);

  /*!
   \brief Next state and transition
   \param s : state
   \param out_edge : outgoing edge value
   \param v : container
   \post triples (status, s', t') have been added to v, for each successor state
   s' and transition t from s to s' along outgoing edge out_edge
   */
  virtual void next(tchecker::syncprod::const_state_sptr_t const & s,
                    tchecker::syncprod::outgoing_edges_value_t const & out_edge, std::vector<sst_t> & v);

  using tchecker::ts::full_ts_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                                tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t,
                                tchecker::syncprod::initial_range_t, tchecker::syncprod::outgoing_edges_range_t,
                                tchecker::syncprod::initial_value_t, tchecker::syncprod::outgoing_edges_value_t>::initial;

  using tchecker::ts::full_ts_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                                tchecker::syncprod::transition_sptr_t, tchecker::syncprod::const_transition_sptr_t,
                                tchecker::syncprod::initial_range_t, tchecker::syncprod::outgoing_edges_range_t,
                                tchecker::syncprod::initial_value_t, tchecker::syncprod::outgoing_edges_value_t>::next;

  /*!
  \brief Checks if a state satisfies a set of labels
  \param s : a state
  \param labels : a set of labels
  \return true if labels is not empty and labels is included in the set of
  labels of state s, false otherwise
   */
  virtual bool satisfies(tchecker::syncprod::const_state_sptr_t const & s, boost::dynamic_bitset<> const & labels) const;

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
   \brief Accessor
   \return Underlying system of timed processes
   */
  tchecker::syncprod::system_t const & system() const;

private:
  std::shared_ptr<tchecker::syncprod::system_t const> _system;           /*!< System of timed processes */
  tchecker::syncprod::state_pool_allocator_t _state_allocator;           /*!< Allocator of states */
  tchecker::syncprod::transition_pool_allocator_t _transition_allocator; /*!< Allocator of transitions */
};

} // end of namespace syncprod

} // end of namespace tchecker

#endif // TCHECKER_SYNCPROD_SYNCPROD_HH
