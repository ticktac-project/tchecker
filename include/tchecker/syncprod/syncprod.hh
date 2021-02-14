/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYNCPROD_SYNCPROD_HH
#define TCHECKER_SYNCPROD_SYNCPROD_HH

#include <cstdlib>

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
enum tchecker::state_status_t initial(tchecker::syncprod::system_t const & system,
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
inline enum tchecker::state_status_t initial(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t & s,
                                             tchecker::syncprod::transition_t & t,
                                             tchecker::syncprod::initial_value_t const & v)
{
  return tchecker::syncprod::initial(system, s.vloc_ptr(), t.vedge_ptr(), v);
}

/*!
 \brief Type of iterator over outgoing edges
 */
using outgoing_edges_iterator_t = tchecker::syncprod::vloc_edges_iterator_t;

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
outgoing_edges_range_t outgoing_edges(tchecker::syncprod::system_t const & system,
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
enum tchecker::state_status_t next(tchecker::syncprod::system_t const & system,
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
inline enum tchecker::state_status_t next(tchecker::syncprod::system_t const & system, tchecker::syncprod::state_t & s,
                                          tchecker::syncprod::transition_t & t,
                                          tchecker::syncprod::outgoing_edges_value_t const & v)
{
  return tchecker::syncprod::next(system, s.vloc_ptr(), t.vedge_ptr(), v);
}

/*!
 \class syncprod_t
 \brief Synchronized product of timed processes with state and transition
 allocation
 \note all returned states and transitions deallocated automatically
 */
class syncprod_t final
    : public tchecker::ts::ts_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                                tchecker::syncprod::transition_sptr_t, tchecker::syncprod::initial_range_t,
                                tchecker::syncprod::outgoing_edges_range_t, tchecker::syncprod::initial_value_t,
                                tchecker::syncprod::outgoing_edges_value_t> {
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

  /*!
   \brief Accessor
   \return initial edges
   */
  virtual tchecker::syncprod::initial_range_t initial_edges();

  /*!
   \brief Initial state and transition
   \param v : initial state valuation
   \post state s and transition t have been initialized from v
   \return (status, s, t) where initial state s and initial transition t have
   been computed from v, and status is the status of state s after initialization
   \note s and t are deallocated automatically
   */
  virtual std::tuple<enum tchecker::state_status_t, tchecker::syncprod::state_sptr_t, tchecker::syncprod::transition_sptr_t>
  initial(tchecker::syncprod::initial_value_t const & v);

  /*!
   \brief Accessor
   \param s : state
   \return outgoing edges from state s
   */
  virtual tchecker::syncprod::outgoing_edges_range_t outgoing_edges(tchecker::syncprod::const_state_sptr_t const & s);

  /*!
   \brief Next state and transition
   \param s : state
   \param v : outgoing edge value
   \return (status, s', t) where next state s' and transition t have been
   computed from v, and status is the status of state s'
   \note s' and t are deallocated automatically
   */
  virtual std::tuple<enum tchecker::state_status_t, tchecker::syncprod::state_sptr_t, tchecker::syncprod::transition_sptr_t>
  next(tchecker::syncprod::const_state_sptr_t const & s, tchecker::syncprod::outgoing_edges_value_t const & v);

  /*!
  \brief Initial states and transitions with selected status
  \param v : container
  \param mask : mask on initial states
  \post all tuples (status, s, t) of status, initial states and transitions such
  that status matches mask (i.e. status & mask != 0) have been pushed back into v
  */
  virtual inline void initial(std::vector<sst_t> & v, enum tchecker::state_status_t mask)
  {
    return tchecker::ts::ts_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                              tchecker::syncprod::transition_sptr_t, tchecker::syncprod::initial_range_t,
                              tchecker::syncprod::outgoing_edges_range_t, tchecker::syncprod::initial_value_t,
                              tchecker::syncprod::outgoing_edges_value_t>::initial(v, mask);
  }

  /*!
  \brief Next states and transitions with selected status
  \param s : state
  \param v : container
  \param mask : mask on next states
  \post all tuples (status, s', t) such that s -t-> s' is a transition and the
  status of s' matches mask (i.e. status & mask != 0) have been pushed to v
  */
  virtual inline void next(tchecker::syncprod::const_state_sptr_t const & s, std::vector<sst_t> & v,
                           enum tchecker::state_status_t mask)
  {
    return tchecker::ts::ts_t<tchecker::syncprod::state_sptr_t, tchecker::syncprod::const_state_sptr_t,
                              tchecker::syncprod::transition_sptr_t, tchecker::syncprod::initial_range_t,
                              tchecker::syncprod::outgoing_edges_range_t, tchecker::syncprod::initial_value_t,
                              tchecker::syncprod::outgoing_edges_value_t>::next(s, v, mask);
  }

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
