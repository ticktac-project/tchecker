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
#include "tchecker/ta/edges_iterators.hh"
#include "tchecker/ta/state.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/ta/transition.hh"
#include "tchecker/ts/bwd.hh"
#include "tchecker/ts/fwd.hh"
#include "tchecker/ts/inspector.hh"
#include "tchecker/ts/sharing.hh"
#include "tchecker/utils/iterator.hh"
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

using final_iterator_t = tchecker::ta::edges_valuations_iterator_t<tchecker::syncprod::final_range_t,
                                                                   tchecker::flat_integer_variables_valuations_range_t>;

/*!
\brief Type of range of iterators over final states
*/
using final_range_t = tchecker::range_t<tchecker::ta::final_iterator_t, tchecker::end_iterator_t>;

/*!
 \brief Accessor to final edges
 \param system : a system
 \param labels : a set of labels
 \return range of final edges, i.e. edges to tuple of locations that match labels
 */
tchecker::ta::final_range_t final_edges(tchecker::ta::system_t const & system, boost::dynamic_bitset<> const & labels);

/*!
 \brief Dereference type for iterator over final edges
 */
using final_value_t = std::tuple<tchecker::syncprod::final_value_t, tchecker::flat_integer_variables_valuations_value_t>;

/*!
 \brief Compute final state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param vedge : tuple of edges
 \param invariant : clock constraint container for final state invariant
 \param final_value : range of final edges valuations
 \pre the size of vloc and vedge is equal to the size of final edge range in final_value.
 final_value has been obtained from system.
 final_value yields the final location of all the processes ordered by increasing process identifier and
 the valuation of every bounded integer flattened variable ordered by increasing variable identifier
 \post vloc has been initialized to the tuple of final locations in final_value,
 intval has been initialized to the final valuation of bounded integer variables,
 vedge has been initialized to an empty tuple of edges.
 clock constraints from final_range invariant have been aded to invariant
 \return tchecker::STATE_OK if computation succeeded
 STATE_TGT_INVARIANT_VIOLATED if the final valuation of integer variables does not satisfy invariant
 \throw std::runtime_error : if evaluation of invariant throws an exception
 */
tchecker::state_status_t final(tchecker::ta::system_t const & system,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                               tchecker::clock_constraint_container_t & invariant,
                               tchecker::ta::final_value_t const & final_value);

/*!
\brief Compute final state and transition
\param system : a system
\param s : state
\param t : transition
\param v : final iterator value
\post s has been initialized from v, and t is an empty transition
\return tchecker::STATE_OK if computation succeeded
 STATE_TGT_INVARIANT_VIOLATED if the final valuation of integer variables does not satisfy invariant
\throw std::invalid_argument : if s and v have incompatible sizes
*/
inline tchecker::state_status_t final(tchecker::ta::system_t const & system, tchecker::ta::state_t & s,
                                      tchecker::ta::transition_t & t, tchecker::ta::final_value_t const & v)
{
  return tchecker::ta::final(system, s.vloc_ptr(), s.intval_ptr(), t.vedge_ptr(), t.tgt_invariant_container(), v);
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
 \brief Type of iterator over incoming edges
 */
using incoming_edges_iterator_t =
    tchecker::ta::edges_valuations_iterator_t<tchecker::syncprod::incoming_edges_range_t,
                                              tchecker::flat_integer_variables_valuations_range_t>;

/*!
\brief Type of range of incoming edges
*/
using incoming_edges_range_t = tchecker::range_t<tchecker::ta::incoming_edges_iterator_t, tchecker::end_iterator_t>;

/*!
 \brief Accessor to incoming edges
 \param system : a system
 \param vloc : tuple of locations
 \return range of incoming synchronized and asynchronous edges to vloc in system
 */
tchecker::ta::incoming_edges_range_t
incoming_edges(tchecker::ta::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc);

/*!
 \brief Dereference type for iterator over incoming edges
 */
using incoming_edges_value_t =
    std::tuple<tchecker::syncprod::incoming_edges_value_t, tchecker::flat_integer_variables_valuations_value_t>;

/*!
 \brief Compute previous state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param vedge : tuple of edges
 \param src_invariant : clock constraint container for invariant in source state
 \param guard : clock constraint container for guard of vedge
 \param reset : clock resets container for clock resets of vedge
 \param tgt_invariant : clock constaint container for invariant in target state
 \param v : valeu of incoming edge to vloc (range of synchronized/asynchronous edges)
 \pre the target location in edges match the locations in vloc.
 No process has more than one edge in v.
 The pid of every process in v is less than the size of vloc
 \post the locations in vloc have been updated to source locations of the
 processes involved in v, and they have been left unchanged for the other processes.
 The values of variables in intval have been updated according to the statements in v.
 Clock constraints from the invariants in source state have been pushed to src_invariant.
 Clock constraints from the guards in v have been pushed into guard.
 Clock resets from the statements in v have been pushed into reset.
 And clock constraints from the invariants in target have been pushed into tgt_invariant
 \return STATE_OK if state computation succeeded,
 STATE_INCOMPATIBLE_EDGE if the target locations in v do not match vloc or if the bounded integer
 valuation in v does not transform into intval going through v,
 STATE_SRC_INVARIANT_VIOLATED if the new valuation intval does not satisfy the invariant in the source state
 STATE_GUARD_VIOLATED if the new values in intval do not satisfy the guard of v,
 STATE_STATEMENT_FAILED if statements in v cannot be applied to the new intval
 STATE_TGT_INVARIANT_VIOLATED if the new intval does not satisfy the invariant of target state
 \throw std::invalid_argument : if a pid in v is greater or equal to the size of vloc
 \throw std::runtime_error : if the guard in v generates clock resets, or if the statements in v generate clock
 constraints, or if the invariant in updated vloc generates clock resets
 \throw std::runtime_error : if evaluation of invariants, guards or statements throws an exception
 */
tchecker::state_status_t prev(tchecker::ta::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::clock_constraint_container_t & src_invariant,
                              tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                              tchecker::clock_constraint_container_t & tgt_invariant,
                              tchecker::ta::incoming_edges_value_t const & v);

/*!
\brief Compute previous state and transition
\param system : a system
\param s : state
\param t : transition
\param v : incoming edge value
\post s have been updated from v, and t is the set of edges in v
\return status of state s after update
\throw std::invalid_argument : if s and v have incompatible size
*/
inline tchecker::state_status_t prev(tchecker::ta::system_t const & system, tchecker::ta::state_t & s,
                                     tchecker::ta::transition_t & t, tchecker::ta::incoming_edges_value_t const & v)
{
  return tchecker::ta::prev(system, s.vloc_ptr(), s.intval_ptr(), t.vedge_ptr(), t.src_invariant_container(),
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
  \brief Computes the set of labels of a state
  \param system : a system
  \param s : a state
  \return the set of labels on state s
  */
boost::dynamic_bitset<> labels(tchecker::ta::system_t const & system, tchecker::ta::state_t const & s);

/*!
 \brief Checks is a state is a valid final state
 \param system : a system
 \param s : a state
 \return true
*/
bool is_valid_final(tchecker::ta::system_t const & system, tchecker::ta::state_t const & s);

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
 \brief Transition system of the timed automaton over of timed processes with
 state and transition allocation
 \note all returned states and transitions are deallocated automatically
 */
class ta_t final : public tchecker::ts::fwd_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t,
                                              tchecker::ta::transition_sptr_t, tchecker::ta::const_transition_sptr_t>,
                   public tchecker::ts::bwd_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t,
                                              tchecker::ta::transition_sptr_t, tchecker::ta::const_transition_sptr_t>,
                   public tchecker::ts::fwd_impl_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t,
                                                   tchecker::ta::transition_sptr_t, tchecker::ta::const_transition_sptr_t,
                                                   tchecker::ta::initial_range_t, tchecker::ta::outgoing_edges_range_t,
                                                   tchecker::ta::initial_value_t, tchecker::ta::outgoing_edges_value_t>,
                   public tchecker::ts::bwd_impl_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t,
                                                   tchecker::ta::transition_sptr_t, tchecker::ta::const_transition_sptr_t,
                                                   tchecker::ta::final_range_t, tchecker::ta::incoming_edges_range_t,
                                                   tchecker::ta::final_value_t, tchecker::ta::incoming_edges_value_t>,
                   public tchecker::ts::inspector_t<tchecker::ta::const_state_sptr_t, tchecker::ta::const_transition_sptr_t>,
                   public tchecker::ts::sharing_t<tchecker::ta::state_sptr_t, tchecker::ta::transition_sptr_t> {
public:
  // Inherited types
  using fwd_t = tchecker::ts::fwd_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t,
                                    tchecker::ta::transition_sptr_t, tchecker::ta::const_transition_sptr_t>;
  using bwd_t = tchecker::ts::bwd_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t,
                                    tchecker::ta::transition_sptr_t, tchecker::ta::const_transition_sptr_t>;
  using fwd_impl_t = tchecker::ts::fwd_impl_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t,
                                              tchecker::ta::transition_sptr_t, tchecker::ta::const_transition_sptr_t,
                                              tchecker::ta::initial_range_t, tchecker::ta::outgoing_edges_range_t,
                                              tchecker::ta::initial_value_t, tchecker::ta::outgoing_edges_value_t>;
  using bwd_impl_t = tchecker::ts::bwd_impl_t<tchecker::ta::state_sptr_t, tchecker::ta::const_state_sptr_t,
                                              tchecker::ta::transition_sptr_t, tchecker::ta::const_transition_sptr_t,
                                              tchecker::ta::final_range_t, tchecker::ta::incoming_edges_range_t,
                                              tchecker::ta::final_value_t, tchecker::ta::incoming_edges_value_t>;
  using inspector_t = tchecker::ts::inspector_t<tchecker::ta::const_state_sptr_t, tchecker::ta::const_transition_sptr_t>;
  using sharing_t = tchecker::ts::sharing_t<tchecker::ta::state_sptr_t, tchecker::ta::transition_sptr_t>;

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
   \param table_size : size of hash tables\note all states and transitions are pool allocated and deallocated automatically.
   Components within states and transitions are shared if sharing_type is tchecker::ts::SHARING
   */
  ta_t(std::shared_ptr<tchecker::ta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
       std::size_t block_size, std::size_t table_size);

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

  using fwd_t::state;
  using fwd_t::status;
  using fwd_t::transition;

  // Forward

  /*!
   \brief Accessor
   \return range of initial edges
   */
  virtual initial_range_t initial_edges();

  /*!
   \brief Initial state and transition from an initial edge
   \param init_edge : initial edge valuation
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
  virtual outgoing_edges_range_t outgoing_edges(tchecker::ta::const_state_sptr_t const & s);

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
  virtual void next(tchecker::ta::const_state_sptr_t const & s, tchecker::ta::outgoing_edges_value_t const & out_edge,
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
  virtual void next(tchecker::ta::const_state_sptr_t const & s, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

  // Backward

  /*!
   \brief Accessor
   \param labels : final states labels
   \return Final edges
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
  */
  virtual void final(boost::dynamic_bitset<> const & labels, std::vector<sst_t> & v,
                     tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
   \brief Accessor
   \param s : state
   \return incoming edges to state s
   */
  virtual incoming_edges_range_t incoming_edges(tchecker::ta::const_state_sptr_t const & s);

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
  virtual void prev(tchecker::ta::const_state_sptr_t const & s, incoming_edges_value_t const & in_edge, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
  \brief Previous states and transitions with selected status
  \param s : state
  \param v : container
  \param mask : mask on next states
  \post all tuples (status, s', t) such that s' -t-> s is a transition and the
  status of s' matches mask (i.e. status & mask != 0) have been pushed to v
  \note states and transitions that are added to v are deallocated automatically
  \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
  */
  virtual void prev(tchecker::ta::const_state_sptr_t const & s, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

  // Inspector

  /*!
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  virtual boost::dynamic_bitset<> labels(tchecker::ta::const_state_sptr_t const & s) const;

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
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true if a run ending in s is a valid run, false otherwise
  */
  virtual bool is_valid_final(tchecker::ta::const_state_sptr_t const & s) const;

  /*!
   \brief Checks if a state is initial
   \param s : a state
   \return true if s is an initial state, false otherwise
  */
  virtual bool is_initial(tchecker::ta::const_state_sptr_t const & s) const;

  // Sharing

  /*!
   \brief Share state components
   \param s : a state
   \post internal components in s have been shared
   \note THE RESULTING STATE SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::ta::state_sptr_t & s);

  /*!
   \brief Share transition components
   \param t : a transition
   \post internal components in t have been shared
   \note THE RESULTING TRANSITION SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::ta::transition_sptr_t & t);

  /*!
   \brief Accessor
   \return Underlying system of timed processes
   */
  inline tchecker::ta::system_t const & system() const { return *_system; }

private:
  std::shared_ptr<tchecker::ta::system_t const> _system;           /*!< System of timed processes */
  enum tchecker::ts::sharing_type_t _sharing_type;                 /*!< Sharing of state/transition components */
  tchecker::ta::state_pool_allocator_t _state_allocator;           /*!< Pool allocator of states */
  tchecker::ta::transition_pool_allocator_t _transition_allocator; /*! Pool allocator of transitions */
};

} // end of namespace ta

} // end of namespace tchecker

#endif // TCHECKER_TA_HH
