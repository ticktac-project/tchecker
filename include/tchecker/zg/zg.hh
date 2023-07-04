/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_HH
#define TCHECKER_ZG_HH

#include <cstdlib>

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/ta/ta.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"
#include "tchecker/zg/allocators.hh"
#include "tchecker/zg/extrapolation.hh"
#include "tchecker/zg/semantics.hh"
#include "tchecker/zg/state.hh"
#include "tchecker/zg/transition.hh"
#include "tchecker/zg/zone.hh"

/*!
 \file zg.hh
 \brief Zone graphs
 */

namespace tchecker {

namespace zg {

/*!
 \brief Type of iterator over initial states
 */
using initial_iterator_t = tchecker::ta::initial_iterator_t;

/*!
\brief Type of range of iterators over inital states
*/
using initial_range_t = tchecker::ta::initial_range_t;

/*!
 \brief Accessor to initial edges
 \param system : a system
 \return initial edges
 */
inline tchecker::zg::initial_range_t initial_edges(tchecker::ta::system_t const & system)
{
  return tchecker::ta::initial_edges(system);
}

/*!
 \brief Dereference type for iterator over initial states
 */
using initial_value_t = tchecker::ta::initial_value_t;

/*!
 \brief Compute initial state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone
 \param vedge : tuple of edges
 \param invariant : clock constraint container for initial state invariant
 \param semantics : a zone semantics
 \param extrapolation : an extrapolation
 \param initial_range : range of initial state valuations
 \pre the size of vloc and vedge is equal to the size of initial_range.
 initial_range has been obtained from system.
 initial_range yields the initial locations of all the processes ordered by increasing process identifier
 \post vloc has been initialized to the tuple of initial locations in initial_range,
 intval has been initialized to the initial valuation of bounded integer variables,
 vedge has been initialized to an empty tuple of edges.
 clock constraints from initial_range invariant have been aded to invariant
 zone has been initialized to the initial set of clock valuations according to
 semantics and extrapolation.
 \return tchecker::STATE_OK if initialization succeeded,
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if the initial valuation of integer
 variables does not satisfy invariant
 tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the initial zone is empty
 \throw std::runtime_error : if evaluation of invariant throws an exception
 */
tchecker::state_status_t initial(tchecker::ta::system_t const & system,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                 tchecker::intrusive_shared_ptr_t<tchecker::zg::shared_zone_t> const & zone,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                 tchecker::clock_constraint_container_t & invariant, tchecker::zg::semantics_t & semantics,
                                 tchecker::zg::extrapolation_t & extrapolation,
                                 tchecker::zg::initial_value_t const & initial_range);

/*!
 \brief Compute initial state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param semantics : a zone semantics
 \param extrapolation : an extrapolation
 \param v : initial iterator value
 \post s has been initialized from v, and t is an empty transition
 \return tchecker::STATE_OK if initialization of s and t succeeded, see
 tchecker::zg::initial for returned values when initialization fails
 \throw std::invalid_argument : if s and v have incompatible sizes
*/
inline tchecker::state_status_t initial(tchecker::ta::system_t const & system, tchecker::zg::state_t & s,
                                        tchecker::zg::transition_t & t, tchecker::zg::semantics_t & semantics,
                                        tchecker::zg::extrapolation_t & extrapolation, tchecker::zg::initial_value_t const & v)
{
  return tchecker::zg::initial(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.src_invariant_container(),
                               semantics, extrapolation, v);
}

/*!
 \brief Type of iterator over final edges
 */
using final_iterator_t = tchecker::ta::final_iterator_t;

/*!
\brief Type of range of iterators over final edges
*/
using final_range_t = tchecker::ta::final_range_t;

/*!
 \brief Accessor to final edges
 \param system : a system
 \param labels : a set of labels
 \return range of final edges, i.e. edges to tuple of locations that match labels
 */
inline tchecker::zg::final_range_t final_edges(tchecker::ta::system_t const & system, boost::dynamic_bitset<> const & labels)
{
  return tchecker::ta::final_edges(system, labels);
}

/*!
 \brief Dereference type for iterator over final edges
 */
using final_value_t = tchecker::ta::final_value_t;

/*!
 \brief Compute final state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone
 \param vedge : tuple of edges
 \param invariant : clock constraint container for initial state invariant
 \param semantics : a zone semantics
 \param extrapolation : an extrapolation
 \param final_range : range of final edges value
 \pre the size of vloc and vedge is equal to the size of final_range.
 final_range has been obtained from system.
 final_range yields a final location to all the processes ordered by increasing process identifier
 \post vloc has been initialized to the tuple locations in final_range,
 intval has been initialized to the valuation of bounded integer variables in final_range.
 vedge has been initialized to an empty tuple of edges.
 clock constraints from final_range invariant have been aded to invariant
 zone has been initialized to the final set of clock valuations according to
 semantics and extrapolation.
 \return tchecker::STATE_OK if computation succeeded,
 tchecker::STATE_INTVARS_TGT_INVARIANT_VIOLATED if the valuation of integer variables
 does not satisfy invariant
 tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if the final zone is empty
 \throw std::runtime_error : if evaluation of invariant throws an exception
 */
tchecker::state_status_t final(tchecker::ta::system_t const & system,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                               tchecker::intrusive_shared_ptr_t<tchecker::zg::shared_zone_t> const & zone,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                               tchecker::clock_constraint_container_t & invariant, tchecker::zg::semantics_t & semantics,
                               tchecker::zg::extrapolation_t & extrapolation, tchecker::zg::final_value_t const & final_range);

/*!
 \brief Compute final state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param semantics : a zone semantics
 \param extrapolation : an extrapolation
 \param v : final iterator value
 \post s has been initialized from v, and t is an empty transition
 \return tchecker::STATE_OK if initialization of s and t succeeded, see
 tchecker::zg::final for returned values when computation fails
 \throw std::invalid_argument : if s and v have incompatible sizes
*/
inline tchecker::state_status_t final(tchecker::ta::system_t const & system, tchecker::zg::state_t & s,
                                      tchecker::zg::transition_t & t, tchecker::zg::semantics_t & semantics,
                                      tchecker::zg::extrapolation_t & extrapolation, tchecker::zg::final_value_t const & v)
{
  return tchecker::zg::final(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.src_invariant_container(),
                             semantics, extrapolation, v);
}

/*!
 \brief Type of iterator over outgoing edges
 */
using outgoing_edges_iterator_t = tchecker::ta::outgoing_edges_iterator_t;

/*!
\brief Type of range of outgoing edges
*/
using outgoing_edges_range_t = tchecker::ta::outgoing_edges_range_t;

/*!
 \brief Accessor to outgoing edges
 \param system : a system
 \param vloc : tuple of locations
 \return range of outgoing synchronized and asynchronous edges from vloc in system
 */
inline tchecker::zg::outgoing_edges_range_t
outgoing_edges(tchecker::ta::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
{
  return tchecker::ta::outgoing_edges(system, vloc);
}

/*!
 \brief Type of outgoing vedge (range of synchronized/asynchronous edges)
 */
using outgoing_edges_value_t = tchecker::ta::outgoing_edges_value_t;

/*!
 \brief Compute next state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone
 \param vedge : tuple of edges
 \param src_invariant : clock constraint container for invariant of vloc before
 it is updated
 \param guard : clock constraint container for guard of vedge
 \param reset : clock resets container for clock resets of vedge
 \param tgt_invariant : clock constaint container for invariant of vloc after it
 is updated
 \param semantics : a zone semantics
 \param extrapolation : an extrapolation
 \param edges : tuple of edge from vloc (range of synchronized/asynchronous edges)
 \pre the source location in edges match the locations in vloc.
 No process has more than one edge in edges.
 The pid of every process in edges is less than the size of vloc
 \post the locations in vloc have been updated to target locations of the
 processes involved in edges, and they have been left unchanged for the other
 processes.
 The values of variables in intval have been updated according to the statements
 in edges.
 Clock constraints from the invariants of vloc before it is updated have been
 pushed to src_invariant.
 Clock constraints from the guards in edges have been pushed into guard.
 Clock resets from the statements in edges have been pushed into reset.
 Clock constraints from the invariants in the updated vloc have been pushed
 into tgt_invariant.
 The zone has been updated according to semantics and extrapolation from
 src_invariant, guard, reset, tgt_invariant (and delay)
 \return tchecker::STATE_OK if state computation succeeded,
 tchecker::STATE_INCOMPATIBLE_EDGE if the source locations in edges do not match
 vloc,
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if the valuation intval does not
 satisfy the invariant in vloc,
 tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the zone does not satisfy the
 invariant in vloc
 tchecker::STATE_INTVARS_GUARD_VIOLATED if the values in intval do not satisfy
 the guard of edges,
 tchecker::STATE_CLOCKS_GUARD_VIOLATED if the zone updated w.r.t. src_invariant
 does not satisfy the guard of the edges
 tchecker::STATE_INTVARS_STATEMENT_FAILED if statements in edges cannot be
 applied to intval
 tchecker::STATE_INTVARS_TGT_INVARIANT_VIOLATED if the updated intval does not
 satisfy the invariant of updated vloc.
 tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if the updated zone does not
 satisfy the invariant of updated vloc
 \throw std::invalid_argument : if a pid in edges is greater or equal to the
 size of vloc
 \throw std::runtime_error : if the guard in edges generates clock resets, or if
 the statements in edges generate clock constraints, or if the invariant in
 updated vloc generates clock resets
 \throw std::runtime_error : if evaluation of invariants, guards or statements
 throws an exception
 */
tchecker::state_status_t next(tchecker::ta::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                              tchecker::intrusive_shared_ptr_t<tchecker::zg::shared_zone_t> const & zone,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::clock_constraint_container_t & src_invariant,
                              tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                              tchecker::clock_constraint_container_t & tgt_invariant, tchecker::zg::semantics_t & semantics,
                              tchecker::zg::extrapolation_t & extrapolation,
                              tchecker::zg::outgoing_edges_value_t const & edges);

/*!
 \brief Compute next state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param semantics : a zone semantics
 \param extrapolation : an extrapolation
 \param v : outgoing edge value
 \post s have been updated from v according to semantics and extrapolation, and
 t is the set of edges in v
 \return status of state s after update (see tchecker::zg::next)
 \throw std::invalid_argument : if s and v have incompatible size
*/
inline tchecker::state_status_t next(tchecker::ta::system_t const & system, tchecker::zg::state_t & s,
                                     tchecker::zg::transition_t & t, tchecker::zg::semantics_t & semantics,
                                     tchecker::zg::extrapolation_t & extrapolation,
                                     tchecker::zg::outgoing_edges_value_t const & v)
{
  return tchecker::zg::next(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.src_invariant_container(),
                            t.guard_container(), t.reset_container(), t.tgt_invariant_container(), semantics, extrapolation, v);
}

/*!
 \brief Type of iterator over incoming edges
 */
using incoming_edges_iterator_t = tchecker::ta::incoming_edges_iterator_t;

/*!
\brief Type of range of incoming edges
*/
using incoming_edges_range_t = tchecker::ta::incoming_edges_range_t;

/*!
 \brief Accessor to incoming edges
 \param system : a system
 \param vloc : tuple of locations
 \return range of incoming synchronized and asynchronous edges to vloc in system
 */
inline tchecker::zg::incoming_edges_range_t
incoming_edges(tchecker::ta::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
{
  return tchecker::ta::incoming_edges(system, vloc);
}

/*!
 \brief Type of incoming tuple of edges (range of synchronized/asynchronous edges)
 */
using incoming_edges_value_t = tchecker::ta::incoming_edges_value_t;

/*!
 \brief Compute previous state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone
 \param vedge : tuple of edges
 \param src_invariant : clock constraint container for source invariant
 \param guard : clock constraint container for guard of vedge
 \param reset : clock resets container for clock resets of vedge
 \param tgt_invariant : clock constaint container for target invariant
 \param semantics : a zone semantics
 \param extrapolation : an extrapolation
 \param edges : tuple of incoming edges to vloc (range of synchronized/asynchronous edges)
 \pre the target locations in edges match the locations in vloc.
 No process has more than one edge in edges.
 The pid of every process in edges is less than the size of vloc
 \post the locations in vloc have been updated to source locations of the
 processes involved in edges, and they have been left unchanged for the other
 processes.
 The values of variables in intval have been updated according to the statements
 in edges.
 Clock constraints from the source invariant have been pushed to src_invariant.
 Clock constraints from the guards in edges have been pushed into guard.
 Clock resets from the statements in edges have been pushed into reset.
 Clock constraints from the target invariants have been pushed into tgt_invariant.
 The zone has been updated according to semantics and extrapolation from
 src_invariant, guard, reset, tgt_invariant (and delay)
 \return tchecker::STATE_OK if state computation succeeded,
 tchecker::STATE_INCOMPATIBLE_EDGE if the target locations in edges do not match
 vloc or if the bounded integer variables valuation in edges does not transform into
 intval going through edges,
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if the updated intval does not satisfy
 the source invariant
 tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the updated zone does not satisfy the
 source invariant,
 tchecker::STATE_INTVARS_GUARD_VIOLATED if the updated intval do not satisfy the guard
 of edges,
 tchecker::STATE_CLOCKS_GUARD_VIOLATED if the updated zone does not satisfy the guard
 of the edges
 tchecker::STATE_INTVARS_STATEMENT_FAILED if statements in edges cannot be
 applied to intval
 tchecker::STATE_INTVARS_TGT_INVARIANT_VIOLATED if intval does not satisfy the target
 invariant
 tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if the zone does not satisfy the target
 invariant
 \throw std::invalid_argument : if a pid in edges is greater or equal to the
 size of vloc
 \throw std::runtime_error : if the guard in edges generates clock resets, or if
 the statements in edges generate clock constraints, or if the invariant in
 updated vloc generates clock resets
 \throw std::runtime_error : if evaluation of invariants, guards or statements
 throws an exception
 */
tchecker::state_status_t prev(tchecker::ta::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                              tchecker::intrusive_shared_ptr_t<tchecker::zg::shared_zone_t> const & zone,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::clock_constraint_container_t & src_invariant,
                              tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                              tchecker::clock_constraint_container_t & tgt_invariant, tchecker::zg::semantics_t & semantics,
                              tchecker::zg::extrapolation_t & extrapolation,
                              tchecker::zg::incoming_edges_value_t const & edges);

/*!
 \brief Compute previous state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param semantics : a zone semantics
 \param extrapolation : an extrapolation
 \param v : incoming edge value
 \post s have been updated from v according to semantics and extrapolation, and
 t is the set of edges in v
 \return status of state s after update (see tchecker::zg::prev)
 \throw std::invalid_argument : if s and v have incompatible size
*/
inline tchecker::state_status_t prev(tchecker::ta::system_t const & system, tchecker::zg::state_t & s,
                                     tchecker::zg::transition_t & t, tchecker::zg::semantics_t & semantics,
                                     tchecker::zg::extrapolation_t & extrapolation,
                                     tchecker::zg::incoming_edges_value_t const & v)
{
  return tchecker::zg::prev(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.src_invariant_container(),
                            t.guard_container(), t.reset_container(), t.tgt_invariant_container(), semantics, extrapolation, v);
}

/*!
 \brief Computes the set of labels of a state
 \param system : a system
 \param s : a state
 \return the set of labels on state s
*/
boost::dynamic_bitset<> labels(tchecker::ta::system_t const & system, tchecker::zg::state_t const & s);

/*!
 \brief Checks is a state is a valid final state
 \param system : a system
 \param s : a state
 \return true if s has a non-empty zone, false otherwise
*/
bool is_valid_final(tchecker::ta::system_t const & system, tchecker::zg::state_t const & s);

/*!
 \brief Accessor to state attributes as strings
 \param system : a system
 \param s : a state
 \param m : a map of string pairs (key, value)
 \post the tuple of locations, the integer variables valuation and the zone in
 s have been added to map m
 */
void attributes(tchecker::ta::system_t const & system, tchecker::zg::state_t const & s, std::map<std::string, std::string> & m);

/*!
 \brief Accessor to transition attributes as strings
 \param system : a system
 \param t : a transition
 \param m : a map of string pairs (key, value)
 \post the tuple of edges in t has been added to map m
 */
void attributes(tchecker::ta::system_t const & system, tchecker::zg::transition_t const & t,
                std::map<std::string, std::string> & m);

/*!
 \class zg_impl_t
 \brief Low-level transition system implementation of a zone graph of a timed automaton
 */
class zg_impl_t final : public tchecker::ts::ts_impl_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                                       tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t,
                                                       tchecker::zg::initial_range_t, tchecker::zg::outgoing_edges_range_t,
                                                       tchecker::zg::initial_value_t, tchecker::zg::outgoing_edges_value_t> {
public:
  // Inherited types
  using ts_impl_t = tchecker::ts::ts_impl_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                            tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t,
                                            tchecker::zg::initial_range_t, tchecker::zg::outgoing_edges_range_t,
                                            tchecker::zg::initial_value_t, tchecker::zg::outgoing_edges_value_t>;
  using sst_t = ts_impl_t::sst_t;
  using state_t = ts_impl_t::state_t;
  using const_state_t = ts_impl_t::const_state_t;
  using transition_t = ts_impl_t::transition_t;
  using const_transition_t = ts_impl_t::const_transition_t;
  using initial_range_t = ts_impl_t::initial_range_t;
  using initial_value_t = ts_impl_t::initial_value_t;
  using outgoing_edges_range_t = ts_impl_t::outgoing_edges_range_t;
  using outgoing_edges_value_t = ts_impl_t::outgoing_edges_value_t;

  /*!
   \brief Constructor
   \param system : a system of timed processes
   \param semantics : a zone semantics
   \param extrapolation : a zone extrapolation
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash tables
   \note all states and transitions are pool allocated and deallocated automatically
   */
  zg_impl_t(std::shared_ptr<tchecker::ta::system_t const> const & system,
            std::shared_ptr<tchecker::zg::semantics_t> const & semantics,
            std::shared_ptr<tchecker::zg::extrapolation_t> const & extrapolation, std::size_t block_size,
            std::size_t table_size);

  /*!
   \brief Copy constructor (deleted)
   */
  zg_impl_t(tchecker::zg::zg_impl_t const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  zg_impl_t(tchecker::zg::zg_impl_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~zg_impl_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::zg::zg_impl_t & operator=(tchecker::zg::zg_impl_t const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::zg::zg_impl_t & operator=(tchecker::zg::zg_impl_t &&) = delete;

  using ts_impl_t::state;
  using ts_impl_t::status;
  using ts_impl_t::transition;

  /*!
   \brief Accessor
   \return range of initial edges
   */
  virtual tchecker::zg::initial_range_t initial_edges();

  /*!
   \brief Initial state and transition
   \param init_edge : initial state valuation
   \param v : container
   \post triples (status, s, t) have been added to v, for each initial state s
   and initial transition t that are initialized from init_edge.
   */
  virtual void initial(tchecker::zg::initial_value_t const & init_edge, std::vector<sst_t> & v);

  using ts_impl_t::initial;

  /*!
   \brief Accessor
   \param s : state
   \return outgoing edges from state s
   */
  virtual tchecker::zg::outgoing_edges_range_t outgoing_edges(tchecker::zg::const_state_sptr_t const & s);

  /*!
   \brief Next state and transition
   \param s : state
   \param out_edge : outgoing edge value
   \param v : container
   \post triples (status, s', t') have been added to v, for each successor state
   s' and transition t from s to s' along outgoing edge out_edge
   */
  virtual void next(tchecker::zg::const_state_sptr_t const & s, tchecker::zg::outgoing_edges_value_t const & out_edge,
                    std::vector<sst_t> & v);

  using ts_impl_t::next;

  /*!
    \brief Checks if a state satisfies a set of labels
    \param s : a state
    \param labels : a set of labels
    \return true if labels is not empty and labels is included in the set of
    labels of state s, false otherwise
    */
  virtual boost::dynamic_bitset<> labels(tchecker::zg::const_state_sptr_t const & s) const;

  /*!
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true if s has a non-empty zone, false otherwise
  */
  virtual bool is_valid_final(tchecker::zg::const_state_sptr_t const & s) const;

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post attributes of state s have been added to map m
   */
  virtual void attributes(tchecker::zg::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post attributes of transition t have been added to map m
   */
  virtual void attributes(tchecker::zg::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const;

  /*!
   \brief Share state components
   \param s : a state
   \post internal components in s have been shared
   \note THE RESULTING STATE SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::zg::state_sptr_t & s);

  /*!
   \brief Share transition components
   \param t : a transition
   \post internal components in t have been shared
   \note THE RESULTING TRANSITION SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::zg::transition_sptr_t & t);

  /*!
   \brief Accessor
   \return Pointer to underlying system of timed processes
  */
  std::shared_ptr<tchecker::ta::system_t const> system_ptr() const;

  /*!
   \brief Accessor
   \return Underlying system of timed processes
   */
  tchecker::ta::system_t const & system() const;

private:
  std::shared_ptr<tchecker::ta::system_t const> _system;           /*!< System of timed processes */
  std::shared_ptr<tchecker::zg::semantics_t> _semantics;           /*!< Zone semantics */
  std::shared_ptr<tchecker::zg::extrapolation_t> _extrapolation;   /*!< Zone extrapolation */
  tchecker::zg::state_pool_allocator_t _state_allocator;           /*!< Pool allocator of states */
  tchecker::zg::transition_pool_allocator_t _transition_allocator; /*! Pool allocator of transitions */
};

/*!
 \class zg_t
 \brief Zone graph of a timed automaton with states and transitions allocation
 \note all returned states and transitions deallocated automatically
 */
class zg_t final : public tchecker::ts::make_ts_from_impl_t<tchecker::zg::zg_impl_t> {
public:
  using tchecker::ts::make_ts_from_impl_t<tchecker::zg::zg_impl_t>::make_ts_from_impl_t;

  /*!
   \brief Destructor
  */
  virtual ~zg_t() = default;

  /*!
   \brief Accessor
   \return Pointer to underlying system of timed processes
  */
  std::shared_ptr<tchecker::ta::system_t const> system_ptr() const;

  /*!
   \brief Accessor
   \return Underlying system of timed processes
   */
  tchecker::ta::system_t const & system() const;
};

/*!
 \class sharing_zg_t
 \brief Zone graph of a timed automaton with states and transitions allocation,
 as well as states and transitions sharing
 \note all returned states and transitions deallocated automatically
 */
class sharing_zg_t final : public tchecker::ts::make_sharing_ts_from_impl_t<tchecker::zg::zg_impl_t> {
public:
  using tchecker::ts::make_sharing_ts_from_impl_t<tchecker::zg::zg_impl_t>::make_sharing_ts_from_impl_t;

  /*!
   \brief Destructor
  */
  virtual ~sharing_zg_t() = default;

  /*!
   \brief Accessor
   \return Pointer to underlying system of timed processes
  */
  std::shared_ptr<tchecker::ta::system_t const> system_ptr() const;

  /*!
   \brief Accessor
   \return Underlying system of timed processes
   */
  tchecker::ta::system_t const & system() const;
};

/*!
 \brief Compute initial state of a zone graph from a tuple of locations
 \param zg : zone graph
 \param vloc : tuple of locations
 \param mask : state status mask
 \return the initial state of zg with tuple of locations vloc and status
 compatible with mask if any, nullptr otherwise
 */
template <class ZG>
tchecker::zg::state_sptr_t initial(ZG & zg, tchecker::vloc_t const & vloc, tchecker::state_status_t mask = tchecker::STATE_OK)
{
  std::vector<typename ZG::sst_t> v;
  zg.initial(v, mask);
  for (auto && [status, s, t] : v) {
    if (s->vloc() == vloc)
      return s;
  }
  return nullptr;
}

/*!
 \brief Compute next state and transition from a tuple of edges
 \param zg : zone graph
 \param s : state
 \param vedge : tuple of edges
 \param mask : next state status mask
 \return the pair (nexts, nextt) of successor state and transition of s
 along tuple of edges vedge if any, (nullptr, nullptr) otherwise
 */
template <class ZG>
std::tuple<tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t>
next(ZG & zg, tchecker::zg::const_state_sptr_t const & s, tchecker::vedge_t const & vedge,
     tchecker::state_status_t mask = tchecker::STATE_OK)
{
  std::vector<typename ZG::sst_t> v;
  zg.next(s, v, mask);
  for (auto && [status, nexts, nextt] : v)
    if (nextt->vedge() == vedge)
      return std::make_tuple(nexts, nextt);
  return std::make_tuple(nullptr, nullptr);
}

/*!
 \brief Factory of zone graphs
 \param system : system of timed processes
 \param semantics_type : type of zone semantics
 \param extrapolation_type : type of zone extrapolation
 \param block_size : number of objects allocated in a block
 \param table_size : size of hash tables
 \return a zone graph over system with zone semantics and zone extrapolation
 defined from semantics_type and extrapolation_type, and allocation of
 block_size objects at a time, nullptr if clock bounds cannot be inferred from
 system
 */
tchecker::zg::zg_t * factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                             enum tchecker::zg::semantics_type_t semantics_type,
                             enum tchecker::zg::extrapolation_type_t extrapolation_type, std::size_t block_size,
                             std::size_t table_size);

/*!
 \brief Factory of zone graphs
 \param system : system of timed processes
 \param semantics_type : type of zone semantics
 \param extrapolation_type : type of zone extrapolation
 \param clock_bounds : clock bounds
 \param block_size : number of objects allocated in a block
 \param table_size : size of hash tables
 \return a zone graph over system with zone semantics and zone extrapolation
 defined from semantics_type, extrapolation_type and clock_bounds, and
 allocation of block_size objects at a time
 */
tchecker::zg::zg_t * factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                             enum tchecker::zg::semantics_type_t semantics_type,
                             enum tchecker::zg::extrapolation_type_t extrapolation_type,
                             tchecker::clockbounds::clockbounds_t const & clock_bounds, std::size_t block_size,
                             std::size_t table_size);

/*!
 \brief Factory of zone graphs with states/transitions sharing
 \param system : system of timed processes
 \param semantics_type : type of zone semantics
 \param extrapolation_type : type of zone extrapolation
 \param block_size : number of objects allocated in a block
 \param table_size : size of hash tables
 \return a zone graph over system with zone semantics and zone extrapolation
 defined from semantics_type and extrapolation_type, and allocation of
 block_size objects at a time, nullptr if clock bounds cannot be inferred from
 system
 \note the states and transitions computed by the returned zone graph share
 internal components
 */
tchecker::zg::sharing_zg_t * factory_sharing(std::shared_ptr<tchecker::ta::system_t const> const & system,
                                             enum tchecker::zg::semantics_type_t semantics_type,
                                             enum tchecker::zg::extrapolation_type_t extrapolation_type, std::size_t block_size,
                                             std::size_t table_size);

/*!
 \brief Factory of zone graphs with states/transitions sharing
 \param system : system of timed processes
 \param semantics_type : type of zone semantics
 \param extrapolation_type : type of zone extrapolation
 \param clock_bounds : clock bounds
 \param block_size : number of objects allocated in a block
 \param table_size : size of hash tables
 \return a zone graph over system with zone semantics and zone extrapolation
 defined from semantics_type, extrapolation_type and clock_bounds, and
 allocation of block_size objects at a time
 \note the states and transitions computed by the returned zone graph share
 internal components
 */
tchecker::zg::sharing_zg_t * factory_sharing(std::shared_ptr<tchecker::ta::system_t const> const & system,
                                             enum tchecker::zg::semantics_type_t semantics_type,
                                             enum tchecker::zg::extrapolation_type_t extrapolation_type,
                                             tchecker::clockbounds::clockbounds_t const & clock_bounds, std::size_t block_size,
                                             std::size_t table_size);

} // end of namespace zg

} // end of namespace tchecker

#endif // TCHECKER_ZG_HH
