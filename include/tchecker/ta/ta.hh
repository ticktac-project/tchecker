/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_HH
#define TCHECKER_TA_HH

#include <cstdlib>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/syncprod/syncprod.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/ta/allocators.hh"
#include "tchecker/ta/state.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/ta/transition.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"

/*!
 \file ta.hh
 \brief Timed automata
 */

namespace tchecker {

namespace ta {

/*!
 \brief Type of iterator over initial states
 */
using initial_iterator_t = tchecker::syncprod::initial_iterator_t;

/*!
\brief Type of range of iterators over inital states
*/
using initial_range_t = tchecker::syncprod::initial_range_t;

/*!
 \brief Accessor to initial edges
 \param system : a system
 \return initial edges
 */
inline tchecker::ta::initial_range_t initial_edges(tchecker::ta::system_t const & system)
{
  return tchecker::syncprod::initial_edges(system.as_syncprod_system());
}

/*!
 \brief Dereference type for iterator over initial states
 */
using initial_value_t = tchecker::syncprod::initial_value_t;

/*!
 \brief Compute initial state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param vedge : tuple of edges
 \param invariant : clock constraint container for initial state invariant
 \param initial_range : range of initial state valuations
 \pre the size of vloc and vedge is equal to the size of initial_range.
 initial_range has been obtained from system.
 initial_range yields the initial locations of all the processes ordered by increasing process identifier
 \post vloc has been initialized to the tuple of initial locations in initial_range,
 intval has been initialized to the initial valuation of bounded integer variables,
 vedge has been initialized to an empty tuple of edges.
 clock constraints from initial_range invariant have been aded to invariant
 \return tchecker::STATE_OK if initialization succeeded
 STATE_SRC_INVARIANT_VIOLATED if the initial valuation of integer variables does not satisfy invariant
 \throw std::runtime_error : if evaluation of invariant throws an exception
 */
tchecker::state_status_t initial(tchecker::ta::system_t const & system,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                 tchecker::clock_constraint_container_t & invariant,
                                 tchecker::ta::initial_value_t const & initial_range);

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
inline tchecker::state_status_t initial(tchecker::ta::system_t const & system, tchecker::ta::state_t & s,
                                        tchecker::ta::transition_t & t, tchecker::ta::initial_value_t const & v)
{
  return tchecker::ta::initial(system, s.vloc_ptr(), s.intval_ptr(), t.vedge_ptr(), t.tgt_invariant_container(), v);
}

/*!
 \brief Type of iterator over outgoing edges
 */
using outgoing_edges_iterator_t = tchecker::syncprod::outgoing_edges_iterator_t;

/*!
\brief Type of range of outgoing edges
*/
using outgoing_edges_range_t = tchecker::syncprod::outgoing_edges_range_t;

/*!
 \brief Accessor to outgoing edges
 \param system : a system
 \param vloc : tuple of locations
 \return range of outgoing synchronized and asynchronous edges from vloc in system
 */
inline tchecker::ta::outgoing_edges_range_t
outgoing_edges(tchecker::ta::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
{
  return tchecker::syncprod::outgoing_edges(system.as_syncprod_system(), vloc);
}

/*!
 \brief Type of outgoing vedge (range of synchronized/asynchronous edges)
 */
using outgoing_edges_value_t = tchecker::syncprod::outgoing_edges_value_t;

/*!
 \brief Compute next state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param vedge : tuple of edges
 \param src_invariant : clock constraint container for invariant of vloc before it is updated
 \param guard : clock constraint container for guard of vedge
 \param reset : clock resets container for clock resets of vedge
 \param tgt_invariant : clock constaint container for invariant of vloc after it is updated
 \param edges : tuple of edge from vloc (range of synchronized/asynchronous edges)
 \pre the source location in edges match the locations in vloc.
 No process has more than one edge in edges.
 The pid of every process in edges is less than the size of vloc
 \post the locations in vloc have been updated to target locations of the
 processes involved in edges, and they have been left unchanged for the other processes.
 The values of variables in intval have been updated according to the statements in edges.
 Clock constraints from the invariants of vloc before it is updated have been pushed to src_invariant.
 Clock constraints from the guards in edges have been pushed into guard.
 Clock resets from the statements in edges have been pushed into reset.
 And clock constraints from the invariants in the updated vloc have been pushed into tgt_invariant
 \return STATE_OK if state computation succeeded,
 STATE_INCOMPATIBLE_EDGE if the source locations in edges do not match vloc,
 STATE_SRC_INVARIANT_VIOLATED if the valuation intval does not satisfy the invariant in vloc,
 STATE_GUARD_VIOLATED if the values in intval do not satisfy the guard of edges,
 STATE_STATEMENT_FAILED if statements in edges cannot be applied to intval
 STATE_TGT_INVARIANT_VIOLATED if the updated intval does not satisfy the invariant of updated vloc.
 \throw std::invalid_argument : if a pid in edges is greater or equal to the size of vloc
 \throw std::runtime_error : if the guard in edges generates clock resets, or if the statements in edges generate clock
 constraints, or if the invariant in updated vloc generates clock resets
 \throw std::runtime_error : if evaluation of invariants, guards or statements throws an exception
 */
tchecker::state_status_t next(tchecker::ta::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::clock_constraint_container_t & src_invariant,
                              tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                              tchecker::clock_constraint_container_t & tgt_invariant,
                              tchecker::ta::outgoing_edges_value_t const & edges);

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
inline tchecker::state_status_t next(tchecker::ta::system_t const & system, tchecker::ta::state_t & s,
                                     tchecker::ta::transition_t & t, tchecker::ta::outgoing_edges_value_t const & v)
{
  return tchecker::ta::next(system, s.vloc_ptr(), s.intval_ptr(), t.vedge_ptr(), t.src_invariant_container(),
                            t.guard_container(), t.reset_container(), t.tgt_invariant_container(), v);
}

/*!
 \brief Checks if time can elapse in a tuple of locations
 \param system : a system of timed processes
 \param vloc : tuple of locations
 \return true if time delay is allowed in vloc, false otherwise
 */
bool delay_allowed(tchecker::ta::system_t const & system, tchecker::vloc_t const & vloc);

/*!
 \brief Compute the set of reference clocks that can let time elapse in a tuple
 of locations
 \param system : a system of timed processes
 \param r : reference clocks
 \param vloc : tuple of locations
 \return a dynamic bitset of size r.refcount() that contains all reference clocks
 that can delay from vloc
 */
boost::dynamic_bitset<> delay_allowed(tchecker::ta::system_t const & system, tchecker::reference_clock_variables_t const & r,
                                      tchecker::vloc_t const & vloc);

/*!
 \brief Compute the set of reference clocks that should synchronize on a tuple
 of edges
 \param system : a system of timed processes
 \param r : reference clocks
 \param vedge : tuple of edges
 \return a dynamic bitset of size r.refcount() that contains all reference
 clocks that should synchronize on vedge
 */
boost::dynamic_bitset<> sync_refclocks(tchecker::ta::system_t const & system, tchecker::reference_clock_variables_t const & r,
                                       tchecker::vedge_t const & vedge);

/*!
 \brief Checks if a state satisfies a set of labels
 \param system : a system of timed processes
 \param s : a state
 \param labels : a set of labels
 \return true if labels is not empty and labels is included in the set of
 labels of state s, false otherwise
*/
bool satisfies(tchecker::ta::system_t const & system, tchecker::ta::state_t const & s, boost::dynamic_bitset<> const & labels);

/*!
 \brief Accessor to state attributes as strings
 \param system : a system
 \param s : a state
 \param m : a map of string pairs (key, value)
 \post the tuple of locations and integer variables valuation in s have been
 added to map m
 */
void attributes(tchecker::ta::system_t const & system, tchecker::ta::state_t const & s, std::map<std::string, std::string> & m);

/*!
 \brief Accessor to transition attributes as strings
 \param system : a system
 \param t : a transition
 \param m : a map of string pairs (key, value)
 \post the tuple of edges in t has been added to map m
 */
void attributes(tchecker::ta::system_t const & system, tchecker::ta::transition_t const & t,
                std::map<std::string, std::string> & m);

/*!
 \class ta_t
 \brief Timed automaton over a system of synchronized timed processes
 */
class ta_t final : public tchecker::ts::full_ts_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t,
                                                  tchecker::ta::transition_sptr_t, tchecker::ta::const_transition_sptr_t,
                                                  tchecker::ta::initial_range_t, tchecker::ta::outgoing_edges_range_t,
                                                  tchecker::ta::initial_value_t, tchecker::ta::outgoing_edges_value_t> {
public:
  /*!
   \brief Constructor
   \param system : a system of timed processes
   \param block_size : number of objects allocated in a block
   \note all states and transitions are pool allocated and deallocated automatically
   */
  ta_t(std::shared_ptr<tchecker::ta::system_t const> const & system, std::size_t block_size);

  /*!
   \brief Copy constructor (deleted)
   */
  ta_t(tchecker::ta::ta_t const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  ta_t(tchecker::ta::ta_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~ta_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::ta::ta_t & operator=(tchecker::ta::ta_t const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::ta::ta_t & operator=(tchecker::ta::ta_t &&) = delete;

  using tchecker::ts::full_ts_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t, tchecker::ta::transition_sptr_t,
                                tchecker::ta::const_transition_sptr_t, tchecker::ta::initial_range_t,
                                tchecker::ta::outgoing_edges_range_t, tchecker::ta::initial_value_t,
                                tchecker::ta::outgoing_edges_value_t>::status;

  using tchecker::ts::full_ts_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t, tchecker::ta::transition_sptr_t,
                                tchecker::ta::const_transition_sptr_t, tchecker::ta::initial_range_t,
                                tchecker::ta::outgoing_edges_range_t, tchecker::ta::initial_value_t,
                                tchecker::ta::outgoing_edges_value_t>::state;

  using tchecker::ts::full_ts_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t, tchecker::ta::transition_sptr_t,
                                tchecker::ta::const_transition_sptr_t, tchecker::ta::initial_range_t,
                                tchecker::ta::outgoing_edges_range_t, tchecker::ta::initial_value_t,
                                tchecker::ta::outgoing_edges_value_t>::transition;

  /*!
   \brief Accessor
   \return range of initial edges
   */
  virtual tchecker::ta::initial_range_t initial_edges();

  /*!
   \brief Initial state and transition
   \param init_edge : initial state valuation
   \param v : container
   \post triples (status, s, t) have been added to v, for each initial state s
   and initial transition t that are initialized from init_edge.
   */
  virtual void initial(tchecker::ta::initial_value_t const & init_edge, std::vector<sst_t> & v);

  /*!
   \brief Accessor
   \param s : state
   \return outgoing edges from state s
   */
  virtual tchecker::ta::outgoing_edges_range_t outgoing_edges(tchecker::ta::const_state_sptr_t const & s);

  /*!
   \brief Next state and transition
   \param s : state
   \param out_edge : outgoing edge value
   \param v : container
   \post triples (status, s', t') have been added to v, for each successor state
   s' and transition t from s to s' along outgoing edge out_edge
   */
  virtual void next(tchecker::ta::const_state_sptr_t const & s, tchecker::ta::outgoing_edges_value_t const & out_edge,
                    std::vector<sst_t> & v);

  using tchecker::ts::full_ts_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t, tchecker::ta::transition_sptr_t,
                                tchecker::ta::const_transition_sptr_t, tchecker::ta::initial_range_t,
                                tchecker::ta::outgoing_edges_range_t, tchecker::ta::initial_value_t,
                                tchecker::ta::outgoing_edges_value_t>::initial;

  using tchecker::ts::full_ts_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t, tchecker::ta::transition_sptr_t,
                                tchecker::ta::const_transition_sptr_t, tchecker::ta::initial_range_t,
                                tchecker::ta::outgoing_edges_range_t, tchecker::ta::initial_value_t,
                                tchecker::ta::outgoing_edges_value_t>::next;

  /*!
   \brief Checks if a state satisfies a set of labels
   \param s : a state
   \param labels : a set of labels
   \return true if labels is not empty and labels is included in the set of
   labels of state s, false otherwise
   */
  virtual bool satisfies(tchecker::ta::const_state_sptr_t const & s, boost::dynamic_bitset<> const & labels) const;

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post attributes of state s have been added to map m
   */
  virtual void attributes(tchecker::ta::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post attributes of transition t have been added to map m
   */
  virtual void attributes(tchecker::ta::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor
   \return Underlying system of timed processes
   */
  tchecker::ta::system_t const & system() const;

private:
  std::shared_ptr<tchecker::ta::system_t const> _system;           /*!< System of timed processes */
  tchecker::ta::state_pool_allocator_t _state_allocator;           /*!< Pool allocator of states */
  tchecker::ta::transition_pool_allocator_t _transition_allocator; /*! Pool allocator of transitions */
};

} // end of namespace ta

} // end of namespace tchecker

#endif // TCHECKER_TA_HH
