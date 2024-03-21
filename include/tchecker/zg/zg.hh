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
#include "tchecker/ts/builder.hh"
#include "tchecker/ts/bwd.hh"
#include "tchecker/ts/fwd.hh"
#include "tchecker/ts/inspector.hh"
#include "tchecker/ts/sharing.hh"
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

// Initial edges

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

// Initial states

/*!
 \brief Compute initial state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone
 \param vedge : tuple of edges
 \param sync_id : synchronization identifier
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
 sync_id has been set to tchecker::NO_SYNC.
 clock constraints from initial_range invariant have been aded to invariant
 zone has been initialized to the initial set of clock valuations according to
 semantics and extrapolation.
 \return tchecker::STATE_OK if initialization succeeded,
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if the initial valuation of integer
 variables does not satisfy invariant
 tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the initial zone is empty
 \throw std::runtime_error : if evaluation of invariant throws an exception
 */
tchecker::state_status_t initial(tchecker::ta::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                                 tchecker::intval_sptr_t const & intval, tchecker::zg::zone_sptr_t const & zone,
                                 tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
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
  return tchecker::zg::initial(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.sync_id(),
                               t.src_invariant_container(), semantics, extrapolation, v);
}

// Final edges

/*!
 \brief Type of iterator over final edges
 \note this iterator ranges over the set of tuples of process locations and bounded integer variables valuations
 in the given system, the size of which is exponential in the number of locations and processes, as well as in the
 domains of bounded integer variables
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

// Final states

/*!
 \brief Compute final state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone
 \param vedge : tuple of edges
 \param sync_id : synchronization identifier
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
 sync_id has been set to tchecker::NO_SYNC.
 clock constraints from final_range invariant have been aded to invariant
 zone has been initialized to the final set of clock valuations according to
 semantics and extrapolation.
 \return tchecker::STATE_OK if computation succeeded,
 tchecker::STATE_INTVARS_TGT_INVARIANT_VIOLATED if the valuation of integer variables
 does not satisfy invariant
 tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if the final zone is empty
 \throw std::runtime_error : if evaluation of invariant throws an exception
 */
tchecker::state_status_t final(tchecker::ta::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                               tchecker::intval_sptr_t const & intval, tchecker::zg::zone_sptr_t const & zone,
                               tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
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
  return tchecker::zg::final(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.sync_id(),
                             t.src_invariant_container(), semantics, extrapolation, v);
}

// Outgoing edges

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
inline tchecker::zg::outgoing_edges_range_t outgoing_edges(tchecker::ta::system_t const & system,
                                                           tchecker::const_vloc_sptr_t const & vloc)
{
  return tchecker::ta::outgoing_edges(system, vloc);
}

/*!
 \brief Type of outgoing vedge (range of synchronized/asynchronous edges)
 */
using outgoing_edges_value_t = tchecker::ta::outgoing_edges_value_t;

// Next states

/*!
 \brief Compute next state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone
 \param vedge : tuple of edges
 \param sync_id : synchronization identifier
 \param src_invariant : clock constraint container for invariant of vloc before
 it is updated
 \param guard : clock constraint container for guard of vedge
 \param reset : clock resets container for clock resets of vedge
 \param tgt_invariant : clock constaint container for invariant of vloc after it
 is updated
 \param semantics : a zone semantics
 \param extrapolation : an extrapolation
 \param sync_edges : tuple of edge from vloc (range of synchronized/asynchronous edges)
 \pre the source location of edges in sync_edges match the locations in vloc.
 No process has more than one edge in sync_edges.
 The pid of every process involved in sync_edges is less than the size of vloc
 \post the locations in vloc have been updated to target locations of the
 processes involved in sync_edges, and they have been left unchanged for the other
 processes.
 The values of variables in intval have been updated according to the statements
 of the edges in sync_edges.
 Clock constraints from the invariants of vloc before it is updated have been
 pushed to src_invariant.
 Clock constraints from the guards of the edges in sync_edges have been pushed into guard.
 Clock resets from the statements of the edges in sync_edges have been pushed into reset.
 Clock constraints from the invariants in the updated vloc have been pushed
 into tgt_invariant.
 The zone has been updated according to semantics and extrapolation from
 src_invariant, guard, reset, tgt_invariant (and delay)
 vedge is the tuple of edges in sync_edges.
 sync_id has been set to the identifier if the synchronization that is instantiated by vedge,
 or tchecker::NO_SYNC if vedge is asynchronous.
 \return tchecker::STATE_OK if state computation succeeded,
 tchecker::STATE_INCOMPATIBLE_EDGE if the source locations of the edges in sync_edges do not match
 vloc,
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if the valuation intval does not
 satisfy the invariant in vloc,
 tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the zone does not satisfy the
 invariant in vloc
 tchecker::STATE_INTVARS_GUARD_VIOLATED if the values in intval do not satisfy
 the guard of the edges in sync_edges,
 tchecker::STATE_CLOCKS_GUARD_VIOLATED if the zone updated w.r.t. src_invariant
 does not satisfy the guard of the edges in sync_edges
 tchecker::STATE_INTVARS_STATEMENT_FAILED if statements of the edges in sync_edges cannot be
 applied to intval
 tchecker::STATE_INTVARS_TGT_INVARIANT_VIOLATED if the updated intval does not
 satisfy the invariant of updated vloc.
 tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if the updated zone does not
 satisfy the invariant of updated vloc
 \throw std::invalid_argument : if the pid of a process involved in sync_edges is greater or equal to the
 size of vloc
 \throw std::runtime_error : if the guard of an edge in ynsc_edges generates clock resets, or if
 the statements of an edge in sync_edges generate clock constraints, or if the invariant in
 updated vloc generates clock resets
 \throw std::runtime_error : if evaluation of invariants, guards or statements
 throws an exception
 */
tchecker::state_status_t next(tchecker::ta::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                              tchecker::intval_sptr_t const & intval, tchecker::zg::zone_sptr_t const & zone,
                              tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                              tchecker::clock_constraint_container_t & src_invariant,
                              tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                              tchecker::clock_constraint_container_t & tgt_invariant, tchecker::zg::semantics_t & semantics,
                              tchecker::zg::extrapolation_t & extrapolation,
                              tchecker::zg::outgoing_edges_value_t const & sync_edges);

/*!
 \brief Compute next state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param semantics : a zone semantics
 \param extrapolation : an extrapolation
 \param sync_edges : tuple of edge from s (range of synchronized/asynchronous edges)
 \post s have been updated from sync_edges according to semantics and extrapolation, and
 t is the set of edges and synchronization identifier in sync_edges
 \return status of state s after update (see tchecker::zg::next)
 \throw std::invalid_argument : if s and v have incompatible size
*/
inline tchecker::state_status_t next(tchecker::ta::system_t const & system, tchecker::zg::state_t & s,
                                     tchecker::zg::transition_t & t, tchecker::zg::semantics_t & semantics,
                                     tchecker::zg::extrapolation_t & extrapolation,
                                     tchecker::zg::outgoing_edges_value_t const & sync_edges)
{
  return tchecker::zg::next(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.sync_id(),
                            t.src_invariant_container(), t.guard_container(), t.reset_container(), t.tgt_invariant_container(),
                            semantics, extrapolation, sync_edges);
}

// Incoming edges

/*!
 \brief Type of iterator over incoming edges
 \note this iterator ranges over the set of tuple of process locations and bounded integer
 variables valuations in the given system, the size of which is exponential in the number
 of locations and processes, as well as in the domains of bounded integer variables
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
inline tchecker::zg::incoming_edges_range_t incoming_edges(tchecker::ta::system_t const & system,
                                                           tchecker::const_vloc_sptr_t const & vloc)
{
  return tchecker::ta::incoming_edges(system, vloc);
}

/*!
 \brief Type of incoming tuple of edges (range of synchronized/asynchronous edges)
 */
using incoming_edges_value_t = tchecker::ta::incoming_edges_value_t;

// Previous states

/*!
 \brief Compute previous state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone
 \param vedge : tuple of edges
 \param sync_id : synchronization identifier
 \param src_invariant : clock constraint container for source invariant
 \param guard : clock constraint container for guard of vedge
 \param reset : clock resets container for clock resets of vedge
 \param tgt_invariant : clock constaint container for target invariant
 \param semantics : a zone semantics
 \param extrapolation : an extrapolation
 \param v : tuple of incoming edges to vloc (range of synchronized/asynchronous edges) and integer variables pre-valuations
 \pre the target locations of the edges in v match the locations in vloc.
 No process has more than one edge in v.
 The pid of every process involved in the edges in v is less than the size of vloc
 \post the locations in vloc have been updated to source locations of the
 processes involved in v, and they have been left unchanged for the other
 processes.
 The values of variables in intval have been updated according to the statements
 of the edges in v.
 Clock constraints from the source invariant have been pushed to src_invariant.
 Clock constraints from the guards of the edges in v have been pushed into guard.
 Clock resets from the statements of the edges in v have been pushed into reset.
 Clock constraints from the target invariants have been pushed into tgt_invariant.
 The zone has been updated according to semantics and extrapolation from
 src_invariant, guard, reset, tgt_invariant (and delay)
 vedge is the tuple of edges in v.
 sync_id has been set to the identifier if the synchronization that is instantiated by vedge,
 or tchecker::NO_SYNC if vedge is asynchronous.
 \return tchecker::STATE_OK if state computation succeeded,
 tchecker::STATE_INCOMPATIBLE_EDGE if the target locations of the edges in v do not match
 vloc or if the bounded integer variables pre-valuation in v does not transform into
 intval going through the edges in v,
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if the updated intval does not satisfy
 the source invariant
 tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the updated zone does not satisfy the
 source invariant,
 tchecker::STATE_INTVARS_GUARD_VIOLATED if the updated intval do not satisfy the guard
 of the edges in v,
 tchecker::STATE_CLOCKS_GUARD_VIOLATED if the updated zone does not satisfy the guard
 of the edges in v,
 tchecker::STATE_INTVARS_STATEMENT_FAILED if statements of the edges in v cannot be
 applied to intval
 tchecker::STATE_INTVARS_TGT_INVARIANT_VIOLATED if intval does not satisfy the target
 invariant
 tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if the zone does not satisfy the target
 invariant
 \throw std::invalid_argument : if the pid of a process with an edge in v is greater or equal to the
 size of vloc
 \throw std::runtime_error : if the guard of an edge in v generates clock resets, or if
 the statement of an edge in v generate clock constraints, or if the invariant in
 updated vloc generates clock resets
 \throw std::runtime_error : if evaluation of invariants, guards or statements
 throws an exception
 */
tchecker::state_status_t prev(tchecker::ta::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                              tchecker::intval_sptr_t const & intval, tchecker::zg::zone_sptr_t const & zone,
                              tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                              tchecker::clock_constraint_container_t & src_invariant,
                              tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                              tchecker::clock_constraint_container_t & tgt_invariant, tchecker::zg::semantics_t & semantics,
                              tchecker::zg::extrapolation_t & extrapolation, tchecker::zg::incoming_edges_value_t const & v);

/*!
 \brief Compute previous state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param semantics : a zone semantics
 \param extrapolation : an extrapolation
 \param v : tuple of incoming edges to s (range of synchronized/asynchronous edges) and integer variables pre-valuations
 \post s have been updated from v according to semantics and extrapolation, and
 t is the set of edges and synchronization identifier in v
 \return status of state s after update (see tchecker::zg::prev)
 \throw std::invalid_argument : if s and v have incompatible size
*/
inline tchecker::state_status_t prev(tchecker::ta::system_t const & system, tchecker::zg::state_t & s,
                                     tchecker::zg::transition_t & t, tchecker::zg::semantics_t & semantics,
                                     tchecker::zg::extrapolation_t & extrapolation,
                                     tchecker::zg::incoming_edges_value_t const & v)
{
  return tchecker::zg::prev(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.sync_id(),
                            t.src_invariant_container(), t.guard_container(), t.reset_container(), t.tgt_invariant_container(),
                            semantics, extrapolation, v);
}

// Inspector

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
 \brief Checks if a zone contains an initial valuation
 \param system : a system
 \param zone : a DBM zone
 \pre the dimension of zone corresponds to the number of flattened clock variables
 in system plus one (checked by assertion)
 \return true if zone contains the initial valuation where all clocks are equal to zero,
 false otherwise
*/
bool is_initial(tchecker::ta::system_t const & system, tchecker::zg::zone_t const & zone);

/*!
 \brief Checks if a state is initial
 \param system : a system
 \param s : a state
 \pre s is a state computed from system
 \return true if s is an initial state in system, false otherwise
*/
bool is_initial(tchecker::ta::system_t const & system, tchecker::zg::state_t const & s);

// Attributes

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

// Initialize

/*!
 \brief Initialization from attributes
 \param system : a system
 \param vloc : a vector of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone
 \param vedge : a vector of edges
 \param sync_id : synchronization identifier
 \param invariant : clock constraint container for state invariant
 \param attributes : map of attributes
 \pre attributes["vloc"] is defined and follows the syntax required by function
 tchecker::from_string(tchecker::vloc_t &, tchecker::system::system_t const &, std::string const &);
 attributes["inval"] is defined and follows the syntax required by function
 tchecker::from_string(tchecker::intval_t &, tchecker::system::system_t const &, std::string const &);
 attributes["zone"] is defined and follows the syntax required by function
 tchecker::from_string(tchecker::clock_constraint_container_t &, tchecker::clock_variables_t const &,
 std::string const &);
 \post vloc has been initialized from attributes["vloc"]
 intval has been initialized from attributes["intval"]
 zone has been initialized from attributes["zone"] and invariant
 vedge has been initialized to the empty vector of edges
 sync_id has been set to tchecker::NO_SYNC
 and invariant contains the invariant in vloc
 \return tchecker::STATE_OK if initialization succeeded
 tchecker::STATE_BAD if initialization failed
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if attributes["intval"] does not satisfy the invariant in
 vloc
 tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if attributes["zone"] intersected with the invariant in
 vloc is empty
 */
tchecker::state_status_t initialize(tchecker::ta::system_t const & system, tchecker::vloc_sptr_t const & vloc,
                                    tchecker::intval_sptr_t const & intval, tchecker::zg::zone_sptr_t const & zone,
                                    tchecker::vedge_sptr_t const & vedge, tchecker::sync_id_t & sync_id,
                                    tchecker::clock_constraint_container_t & invariant,
                                    std::map<std::string, std::string> const & attributes);

/*!
 \brief Initialization from attributes
 \param system : a system
 \param s : state
 \param t : transition
 \param attributes : map of attributes
 \post s and t have been initialized from attributes["vloc"], attributes["intval"] and attributes["zone"]
 the src invariant container in t contains the invariant of state s
 \return tchecker::STATE_OK if initialization succeeded
 tchecker::STATE_BAD otherwise
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if attributes["intval"] does not satisfy the invariant in
 attributes["vloc"]
 tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if attributes["zone"] intersected with the invariant in
 vloc is empty
*/
inline tchecker::state_status_t initialize(tchecker::ta::system_t const & system, tchecker::zg::state_t & s,
                                           tchecker::zg::transition_t & t,
                                           std::map<std::string, std::string> const & attributes)
{
  return tchecker::zg::initialize(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.sync_id(),
                                  t.src_invariant_container(), attributes);
}

// zg_t

/*!
 \class zg_t
 \brief Transition system of the zone graph over system of timed processes with
 state and transition allocation
 \note all returned states and transitions are deallocated automatically
 */
class zg_t final : public tchecker::ts::fwd_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                              tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t>,
                   public tchecker::ts::bwd_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                              tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t>,
                   public tchecker::ts::fwd_impl_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                                   tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t,
                                                   tchecker::zg::initial_range_t, tchecker::zg::outgoing_edges_range_t,
                                                   tchecker::zg::initial_value_t, tchecker::zg::outgoing_edges_value_t>,
                   public tchecker::ts::bwd_impl_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                                   tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t,
                                                   tchecker::zg::final_range_t, tchecker::zg::incoming_edges_range_t,
                                                   tchecker::zg::final_value_t, tchecker::zg::incoming_edges_value_t>,
                   public tchecker::ts::builder_t<tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t>,
                   public tchecker::ts::inspector_t<tchecker::zg::const_state_sptr_t, tchecker::zg::const_transition_sptr_t>,
                   public tchecker::ts::sharing_t<tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t> {

public:
  // Inherited types
  using fwd_t = tchecker::ts::fwd_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                    tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t>;
  using bwd_t = tchecker::ts::bwd_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                    tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t>;
  using fwd_impl_t = tchecker::ts::fwd_impl_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                              tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t,
                                              tchecker::zg::initial_range_t, tchecker::zg::outgoing_edges_range_t,
                                              tchecker::zg::initial_value_t, tchecker::zg::outgoing_edges_value_t>;
  using bwd_impl_t = tchecker::ts::bwd_impl_t<tchecker::zg::state_sptr_t, tchecker::zg::const_state_sptr_t,
                                              tchecker::zg::transition_sptr_t, tchecker::zg::const_transition_sptr_t,
                                              tchecker::zg::final_range_t, tchecker::zg::incoming_edges_range_t,
                                              tchecker::zg::final_value_t, tchecker::zg::incoming_edges_value_t>;
  using builder_t = tchecker::ts::builder_t<tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t>;
  using inspector_t = tchecker::ts::inspector_t<tchecker::zg::const_state_sptr_t, tchecker::zg::const_transition_sptr_t>;
  using sharing_t = tchecker::ts::sharing_t<tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t>;

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
   \param semantics : a zone semantics
   \param extrapolation : a zone extrapolation
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash tables
   \note all states and transitions are pool allocated and deallocated automatically
   */
  zg_t(std::shared_ptr<tchecker::ta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
       std::shared_ptr<tchecker::zg::semantics_t> const & semantics,
       std::shared_ptr<tchecker::zg::extrapolation_t> const & extrapolation, std::size_t block_size, std::size_t table_size);

  /*!
   \brief Copy constructor (deleted)
   */
  zg_t(tchecker::zg::zg_t const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  zg_t(tchecker::zg::zg_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~zg_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::zg::zg_t & operator=(tchecker::zg::zg_t const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::zg::zg_t & operator=(tchecker::zg::zg_t &&) = delete;

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
  virtual outgoing_edges_range_t outgoing_edges(tchecker::zg::const_state_sptr_t const & s);

  /*!
   \brief Next state and transition
   \param s : state
   \param out_edge : outgoing edge value
   \param v : container
   \param mask : mask on next states
   \post triples (status, s', t') have been added to v, for each transition s -t'-> s' along outgoing
   edge out_edge such that status matches mask (i.e. status & mask != 0)
   \note states and transitions that are added to v are deallocated automatically
   \note states and transitions share their internal components if sharing_type is tchecker::ts::SHARING
   */
  virtual void next(tchecker::zg::const_state_sptr_t const & s, outgoing_edges_value_t const & out_edge, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

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
  virtual void next(tchecker::zg::const_state_sptr_t const & s, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

  // Backward

  /*!
   \brief Accessor
   \param labels : final states labels
   \return Final edges
   \note the return range goes across all tuples of process locations and bounded integer variables
   valuations in the given system. The number of such tuples is exponential in the number of locations
   and processes, as well as in the domains of bounded integer variables
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
  \note complexity is exponential in the number of locations and processes, as well as in the doamins of
  bounded integer variables in the underlying system
  */
  virtual void final(boost::dynamic_bitset<> const & labels, std::vector<sst_t> & v,
                     tchecker::state_status_t mask = tchecker::STATE_OK);

  /*!
   \brief Accessor
   \param s : state
   \return incoming edges to state s
   \note the return range goes across all tuples of process locations and bounded integer variabkes
   valuations in the given system. The number of such tuples is exponential in the number of locations
   and processes, as well as the domains of bounded integer variables
   */
  virtual incoming_edges_range_t incoming_edges(tchecker::zg::const_state_sptr_t const & s);

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
  virtual void prev(tchecker::zg::const_state_sptr_t const & s, incoming_edges_value_t const & in_edge, std::vector<sst_t> & v,
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
  \note complexity is exponential in the number of locations and processes, as well as the domains of
  bounded integer valuations in the underlying system
  */
  virtual void prev(tchecker::zg::const_state_sptr_t const & s, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

  // Builder

  /*!
   \brief State/transition building from attributes
   \param attributes : a map of attributes
   \param v : container
   \param mask : mask on states
   \post all tuples (status, s, t) where s and t have been initialized from attributes,
   and status matches mask (i.e. status & mask != 0) have been pushed to v
   \pre see tchecker::zg::initialize
   \post a triple <status, s, t> has been pushed to v (if status matches mask), where the vector of
   locations in s has been initialized from attributes["vloc"], the integer valuation in s has been
   initialized from attributes["intval"], the zone in s has been initialized from attributes["zone"]
   and the invariant in the vector of locations in s, the vector of edges in t is empty and the source
   invariant in t has been initialized to the invariant in vloc
  */
  virtual void build(std::map<std::string, std::string> const & attributes, std::vector<sst_t> & v,
                     tchecker::state_status_t mask = tchecker::STATE_OK);

  // split

  /*!
   \brief Split a state according to a clock constraint
   \param s : state
   \param c : clock constraint
   \param v : vector of states
   \pre the clock identifier in c are expressed over system clocks (with reference clock
   tchecker::REFCLOCK_ID)
   \post a copy of s has been added to v if it satisfies c or if it satisfies the negation of c
   otherwise, s has been splitted into s1, that satisfies c, and s2 that does not satisfy
   c, then s1 and s2 have been added to v
   */
  void split(tchecker::zg::const_state_sptr_t const & s, tchecker::clock_constraint_t const & c,
             std::vector<tchecker::zg::state_sptr_t> & v);

  /*!
   \brief Split a state according to a list of clock constraints
   \param s : state
   \param constraints : clock constraints
   \param v : vector of states
   \post s has been successively split w.r.t. every constraint in constraints. All resulting states
   have been added to v
   */
  void split(tchecker::zg::const_state_sptr_t const & s, tchecker::clock_constraint_container_t const & constraints,
             std::vector<tchecker::zg::state_sptr_t> & v);

  // Inspector

  /*!
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  virtual boost::dynamic_bitset<> labels(tchecker::zg::const_state_sptr_t const & s) const;

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
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true if s has a non-empty zone, false otherwise
  */
  virtual bool is_valid_final(tchecker::zg::const_state_sptr_t const & s) const;

  /*!
   \brief Checks if a state is initial
   \param s : a state
   \return true if s is an initial state, false otherwise
  */
  virtual bool is_initial(tchecker::zg::const_state_sptr_t const & s) const;

  // Sharing

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
  inline std::shared_ptr<tchecker::ta::system_t const> system_ptr() const { return _system; }

  /*!
   \brief Accessor
   \return Underlying system of timed processes
   */
  inline tchecker::ta::system_t const & system() const { return *_system; }

  /*!
   \brief Accessor
   \return sharing type of this synchronized product
  */
  inline enum tchecker::ts::sharing_type_t sharing_type() const { return _sharing_type; }

private:
  /*!
   \brief Clone and constrain a state
   \param s : a state
   \param c : a clock constraint
   \return a clone of s with its zone intersected with c
   */
  tchecker::zg::state_sptr_t clone_and_constrain(tchecker::zg::const_state_sptr_t const & s,
                                                 tchecker::clock_constraint_t const & c);

  std::shared_ptr<tchecker::ta::system_t const> _system;           /*!< System of timed processes */
  enum tchecker::ts::sharing_type_t _sharing_type;                 /*!< Sharing of state/transition components */
  std::shared_ptr<tchecker::zg::semantics_t> _semantics;           /*!< Zone semantics */
  std::shared_ptr<tchecker::zg::extrapolation_t> _extrapolation;   /*!< Zone extrapolation */
  tchecker::zg::state_pool_allocator_t _state_allocator;           /*!< Pool allocator of states */
  tchecker::zg::transition_pool_allocator_t _transition_allocator; /*! Pool allocator of transitions */
};

/*!
 \brief Compute initial state of a zone graph from a tuple of locations
 \param zg : zone graph
 \param vloc : tuple of locations
 \param mask : state status mask
 \return the initial state of zg with tuple of locations vloc and status
 compatible with mask if any, nullptr otherwise
 */
tchecker::zg::state_sptr_t initial(tchecker::zg::zg_t & zg, tchecker::vloc_t const & vloc,
                                   tchecker::state_status_t mask = tchecker::STATE_OK);

/*!
 \brief Compute next state and transition from a tuple of edges
 \param zg : zone graph
 \param s : state
 \param vedge : tuple of edges
 \param mask : next state status mask
 \return the pair (nexts, nextt) of successor state and transition of s
 along tuple of edges vedge if any, (nullptr, nullptr) otherwise
 */
std::tuple<tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t>
next(tchecker::zg::zg_t & zg, tchecker::zg::const_state_sptr_t const & s, tchecker::vedge_t const & vedge,
     tchecker::state_status_t mask = tchecker::STATE_OK);

/*!
 \brief Factory of zone graphs with clock bounds computed from system
 \param system : system of timed processes
 \param sharing_type : type of sharing
 \param semantics_type : type of zone semantics
 \param extrapolation_type : type of zone extrapolation
 \param block_size : number of objects allocated in a block
 \param table_size : size of hash tables
 \return a zone graph over system with zone semantics and zone extrapolation
 defined from semantics_type and extrapolation_type, and allocation of
 block_size objects at a time, nullptr if clock bounds cannot be inferred from
 system
 \throw std::runtime_error : if clock bounds cannot be computed for system (only if extrapolation_type requires
 clock bounds computation from system)
 */
tchecker::zg::zg_t * factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                             enum tchecker::ts::sharing_type_t sharing_type, enum tchecker::zg::semantics_type_t semantics_type,
                             enum tchecker::zg::extrapolation_type_t extrapolation_type, std::size_t block_size,
                             std::size_t table_size);

/*!
 \brief Factory of zone graphs with given clock bounds
 \param system : system of timed processes
 \param sharing_type : type of sharing
 \param semantics_type : type of zone semantics
 \param extrapolation_type : type of zone extrapolation
 \param clock_bounds : clock bounds
 \param block_size : number of objects allocated in a block
 \param table_size : size of hash tables
 \return a zone graph over system with zone semantics and zone extrapolation
 defined from semantics_type, extrapolation_type and clock_bounds, and allocation
 of block_size objects at a time, nullptr if clock bounds cannot be inferred from
 system
 */
tchecker::zg::zg_t * factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                             enum tchecker::ts::sharing_type_t sharing_type, enum tchecker::zg::semantics_type_t semantics_type,
                             enum tchecker::zg::extrapolation_type_t extrapolation_type,
                             tchecker::clockbounds::clockbounds_t const & clock_bounds, std::size_t block_size,
                             std::size_t table_size);

} // end of namespace zg

} // end of namespace tchecker

#endif // TCHECKER_ZG_HH
