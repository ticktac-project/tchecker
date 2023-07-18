/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_REFZG_HH
#define TCHECKER_REFZG_HH

#include <cstdlib>
#include <memory>

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/refzg/allocators.hh"
#include "tchecker/refzg/semantics.hh"
#include "tchecker/refzg/state.hh"
#include "tchecker/refzg/transition.hh"
#include "tchecker/refzg/zone.hh"
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

/*!
 \file refzg.hh
 \brief Zone graphs with reference clocks
 */

namespace tchecker {

namespace refzg {

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
inline tchecker::refzg::initial_range_t initial_edges(tchecker::ta::system_t const & system)
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
 \param zone : a DBM zone with reference clocks
 \param vedge : tuple of edges
 \param invariant : clock constraint container for initial state invariant
 \param semantics : a zone semantics with reference clocks
 \param spread : spread bound over reference clocks
 \param initial_range : range of initial state valuations
 \pre the size of vloc and vedge is equal to the size of initial_range.
 initial_range has been obtained from system.
 initial_range yields the initial locations of all the processes ordered by
 increasing process identifier
 \post vloc has been initialized to the tuple of initial locations in
 initial_range, intval has been initialized to the initial valuation of bounded
 integer variables, vedge has been initialized to an empty tuple of edges.
 clock constraints from initial_range invariant have been added to invariant
 zone has been initialized to the initial set of clock valuations according to
 semantics and spread.
 \return tchecker::STATE_OK if initialization succeeded,
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if the initial valuation of
 integer variables does not satisfy invariant.
 tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the initial zone does not
 contain a clock valuation that satisfy invariant.
 tchecker::STATE_CLOCKS_EMPTY_SPREAD if the initial bounded-spread zone is
 empty.
 tchecker::STATE_ZONE_EMPTY_SYNC if the resulting zone contains no synchronized
 valuation
 \throw std::runtime_error : if evaluation of invariant throws an exception
 \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
 */
tchecker::state_status_t initial(tchecker::ta::system_t const & system,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                 tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                 tchecker::clock_constraint_container_t & invariant, tchecker::refzg::semantics_t & semantics,
                                 tchecker::integer_t spread, tchecker::refzg::initial_value_t const & initial_range);

/*!
 \brief Compute initial state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param semantics : a zone semantics
 \param spread : spread bound over reference clocks
 \param v : initial iterator value
 \post s has been initialized from v and spread, and t is an empty transition
 \return tchecker::STATE_OK if initialization of s and t succeeded, see
 tchecker::refzg::initial for returned values when initialization fails
 \throw std::invalid_argument : if s and v have incompatible sizes
 \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
*/
inline tchecker::state_status_t initial(tchecker::ta::system_t const & system, tchecker::refzg::state_t & s,
                                        tchecker::refzg::transition_t & t, tchecker::refzg::semantics_t & semantics,
                                        tchecker::integer_t spread, tchecker::refzg::initial_value_t const & v)
{
  return tchecker::refzg::initial(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(),
                                  t.src_invariant_container(), semantics, spread, v);
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
inline tchecker::refzg::final_range_t final_edges(tchecker::ta::system_t const & system, boost::dynamic_bitset<> const & labels)
{
  return tchecker::ta::final_edges(system, labels);
}

/*!
 \brief Dereference type for iterator over final states
 */
using final_value_t = tchecker::ta::final_value_t;

// Final states

/*!
 \brief Compute final state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone with reference clocks
 \param vedge : tuple of edges
 \param invariant : clock constraint container for final state invariant
 \param semantics : a zone semantics with reference clocks
 \param spread : spread bound over reference clocks
 \param final_range : range of final edges
 \pre the size of vloc and vedge is equal to the size of final_range.
 final_range has been obtained from system.
 final_range yields the locations of all the processes ordered by
 increasing process identifier as well as a valuation ordered by increasing
 bounded integer variable identifiers
 \post vloc has been initialized to the tuple of locations in final_range,
 intval has been initialized to the valuation of bounded integer variables
 in final_range,
 vedge has been initialized to an empty tuple of edges.
 clock constraints from final_range invariant have been added to invariant
 zone has been initialized to the final set of clock valuations according to
 semantics and spread.
 \return tchecker::STATE_OK if computation succeeded,
 tchecker::STATE_INTVARS_TGT_INVARIANT_VIOLATED if the valuation of
 integer variables does not satisfy invariant.
 tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if the zone does not
 contain a clock valuation that satisfy invariant.
 tchecker::STATE_CLOCKS_EMPTY_SPREAD if the bounded-spread zone is empty.
 tchecker::STATE_ZONE_EMPTY_SYNC if the zone contains no synchronized valuation
 \throw std::runtime_error : if evaluation of invariant throws an exception
 \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
 */
tchecker::state_status_t final(tchecker::ta::system_t const & system,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                               tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                               tchecker::clock_constraint_container_t & invariant, tchecker::refzg::semantics_t & semantics,
                               tchecker::integer_t spread, tchecker::refzg::final_value_t const & final_range);

/*!
 \brief Compute final state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param semantics : a zone semantics
 \param spread : spread bound over reference clocks
 \param v : final iterator value
 \post s has been initialized from v and spread, and t is an empty transition
 \return tchecker::STATE_OK if computation of s and t succeeded, see
 tchecker::refzg::final for returned values when computation fails
 \throw std::invalid_argument : if s and v have incompatible sizes
 \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
*/
inline tchecker::state_status_t final(tchecker::ta::system_t const & system, tchecker::refzg::state_t & s,
                                      tchecker::refzg::transition_t & t, tchecker::refzg::semantics_t & semantics,
                                      tchecker::integer_t spread, tchecker::refzg::final_value_t const & v)
{
  return tchecker::refzg::final(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.src_invariant_container(),
                                semantics, spread, v);
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
inline tchecker::refzg::outgoing_edges_range_t
outgoing_edges(tchecker::ta::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
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
 \param zone : a DBM zone with reference clocks
 \param vedge : tuple of edges
 \param src_invariant : clock constraint container for invariant of vloc before
 it is updated
 \param guard : clock constraint container for guard of vedge
 \param reset : clock resets container for clock resets of vedge
 \param tgt_invariant : clock constaint container for invariant of vloc after it
 is updated
 \param semantics : a semantics over zones with reference clocks
 \param spread : spread bound over reference clocks
 \param edges : tuple of edge from vloc (range of synchronized/asynchronous
 edges)
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
 The zone has been updated according to semantics and spread from src_invariant,
 guard, reset, tgt_invariant (and delay)
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
 satisfy the invariant of updated vloc.
 tchecker::STATE_CLOCKS_EMPTY_SYNC if synchronizing the reference clocks of the
 processes involved in vedge yields an empty zone
 tchecker::STATE_CLOCKS_EMPTY_SPREAD if the resulting spread-bounded zone is
 empty
 tchecker::STATE_ZONE_EMPTY_SYNC if the resulting zone contains no synchronized
 valuation
 \throw std::invalid_argument : if a pid in edges is greater or equal to the
 size of vloc
 \throw std::runtime_error : if the guard in edges generates clock resets, or if
 the statements in edges generate clock constraints, or if the invariant in
 updated vloc generates clock resets
 \throw std::runtime_error : if evaluation of invariants, guards or statements
 throws an exception
 \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
 */
tchecker::state_status_t next(tchecker::ta::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                              tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::clock_constraint_container_t & src_invariant,
                              tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                              tchecker::clock_constraint_container_t & tgt_invariant, tchecker::refzg::semantics_t & semantics,
                              tchecker::integer_t spread, tchecker::refzg::outgoing_edges_value_t const & edges);

/*!
 \brief Compute next state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param semantics : a zone semantics
 \param spread : spread bound over reference clocks
 \param v : outgoing edge value
 \post s have been updated from v according to semantics and spread, and t is
 the set of edges in v
 \return status of state s after update (see tchecker::refzg::next)
 \throw std::invalid_argument : if s and v have incompatible size
 \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
*/
inline tchecker::state_status_t next(tchecker::ta::system_t const & system, tchecker::refzg::state_t & s,
                                     tchecker::refzg::transition_t & t, tchecker::refzg::semantics_t & semantics,
                                     tchecker::integer_t spread, tchecker::refzg::outgoing_edges_value_t const & v)
{
  return tchecker::refzg::next(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.src_invariant_container(),
                               t.guard_container(), t.reset_container(), t.tgt_invariant_container(), semantics, spread, v);
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
inline tchecker::refzg::incoming_edges_range_t
incoming_edges(tchecker::ta::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
{
  return tchecker::ta::incoming_edges(system, vloc);
}

/*!
 \brief Type of incoming edges range value (range of synchronized/asynchronous edges)
 */
using incoming_edges_value_t = tchecker::ta::incoming_edges_value_t;

// Previous states

/*!
 \brief Compute previous state
 \param system : a system
 \param vloc : tuple of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone with reference clocks
 \param vedge : tuple of edges
 \param src_invariant : clock constraint container for source invariant (w.r.t. edge)
 \param guard : clock constraint container for guard of vedge
 \param reset : clock resets container for clock resets of vedge
 \param tgt_invariant : clock constaint container for target invariant (w.r.t. edge)
 \param semantics : a semantics over zones with reference clocks
 \param spread : spread bound over reference clocks
 \param edges : range of incoming edges to vloc (range of synchronized/asynchronous
 edges)
 \pre the target locations in edges match the locations in vloc.
 No process has more than one edge in edges.
 The pid of every process in edges is less than the size of vloc
 \post the locations in vloc have been updated to source locations of the
 processes involved in edges, and they have been left unchanged for the other
 processes.
 The values of variables in intval have been updated according to the statements
 in edges.
 Clock constraints from the invariants of vloc before it is updated have been
 pushed to tgt_invariant.
 Clock constraints from the guards in edges have been pushed into guard.
 Clock resets from the statements in edges have been pushed into reset.
 Clock constraints from the invariants in the updated vloc have been pushed
 into src_invariant.
 The zone has been updated according to semantics and spread from src_invariant,
 guard, reset, tgt_invariant (and delay)
 \return tchecker::STATE_OK if state computation succeeded,
 tchecker::STATE_INCOMPATIBLE_EDGE if the target locations in edges do not match
 vloc, or if the bounded integer variables valuation in edges does not yield intval
 after following edges
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if the valuation in edges does not
 satisfy the invariant in vloc,
 tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if the zone does not satisfy the
 source invariant
 tchecker::STATE_INTVARS_GUARD_VIOLATED if the bounded integer valuation in edges
 does not satisfy the guard of edges,
 tchecker::STATE_CLOCKS_GUARD_VIOLATED if the zone updated w.r.t. src_invariant
 does not satisfy the guard of the edges
 tchecker::STATE_INTVARS_STATEMENT_FAILED if statements in edges cannot be
 applied to intval
 tchecker::STATE_INTVARS_TGT_INVARIANT_VIOLATED if intval does not
 satisfy the target invariant
 tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED if zone does not
 satisfy the target invariant
 tchecker::STATE_CLOCKS_EMPTY_SYNC if synchronizing the reference clocks of the
 processes involved in vedge yields an empty zone
 tchecker::STATE_CLOCKS_EMPTY_SPREAD if the resulting spread-bounded zone is
 empty
 tchecker::STATE_ZONE_EMPTY_SYNC if the resulting zone contains no synchronized
 valuation
 \throw std::invalid_argument : if a pid in edges is greater or equal to the
 size of vloc
 \throw std::runtime_error : if the guard in edges generates clock resets, or if
 the statements in edges generate clock constraints, or if the invariants
 generate clock resets
 \throw std::runtime_error : if evaluation of invariants, guards or statements
 throws an exception
 \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
 */
tchecker::state_status_t prev(tchecker::ta::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                              tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::clock_constraint_container_t & src_invariant,
                              tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                              tchecker::clock_constraint_container_t & tgt_invariant, tchecker::refzg::semantics_t & semantics,
                              tchecker::integer_t spread, tchecker::refzg::incoming_edges_value_t const & edges);

/*!
 \brief Compute previous state and transition
 \param system : a system
 \param s : state
 \param t : transition
 \param semantics : a zone semantics
 \param spread : spread bound over reference clocks
 \param v : value of incoming edge range
 \post s have been updated from v according to semantics and spread, and t is
 the set of edges in v
 \return status of state s after update (see tchecker::refzg::prev)
 \throw std::invalid_argument : if s and v have incompatible size
 \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
*/
inline tchecker::state_status_t prev(tchecker::ta::system_t const & system, tchecker::refzg::state_t & s,
                                     tchecker::refzg::transition_t & t, tchecker::refzg::semantics_t & semantics,
                                     tchecker::integer_t spread, tchecker::refzg::incoming_edges_value_t const & v)
{
  return tchecker::refzg::prev(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(), t.src_invariant_container(),
                               t.guard_container(), t.reset_container(), t.tgt_invariant_container(), semantics, spread, v);
}

// Inspector

/*!
  \brief Computes the set of labels of a state
  \param system : a system
  \param s : a state
  \return the set of labels on state s
  */
boost::dynamic_bitset<> labels(tchecker::ta::system_t const & system, tchecker::refzg::state_t const & s);

/*!
 \brief Checks is a state is a valid final state
 \param system : a system
 \param s : a state
 \return true if s has a non-empty zone that contains a synchronized valuation,
 false otherwise
*/
bool is_valid_final(tchecker::ta::system_t const & system, tchecker::refzg::state_t const & s);

/*!
 \brief Checks if a zone with reference clocks contains an initial valuation
 \param system : a system
 \param zone : a DBM zone with reference clocks
 \return true if zone contains the initial valuation where all clocks are equal to zero,
 false otherwise
*/
bool is_initial(tchecker::ta::system_t const & system, tchecker::refzg::zone_t const & zone);

/*!
 \brief Checks if a state is initial
 \param system : a system
 \param s : a state
 \pre s is a state computed from system
 \return true if s is an initial state in system, false otherwise
*/
bool is_initial(tchecker::ta::system_t const & system, tchecker::refzg::state_t const & s);

// Attributes

/*!
 \brief Accessor to state attributes as strings
 \param system : a system
 \param s : a state
 \param m : a map of string pairs (key, value)
 \post attributes of state s have been added to map m
 */
void attributes(tchecker::ta::system_t const & system, tchecker::refzg::state_t const & s,
                std::map<std::string, std::string> & m);

/*!
 \brief Accessor to transition attributes as strings
 \param system : a system
 \param t : a transition
 \param m : a map of string pairs (key, value)
 \post attributes of transition t have been added to map m
 */
void attributes(tchecker::ta::system_t const & system, tchecker::refzg::transition_t const & t,
                std::map<std::string, std::string> & m);

// Initialize

/*!
 \brief Initialization from attributes
 \param system : a system
 \param vloc : a vector of locations
 \param intval : valuation of bounded integer variables
 \param zone : a DBM zone with reference clocks
 \param vedge : a vector of edges
 \param invariant : clock constraint container for state invariant
 \param spread : spread bound over reference clocks
 \param attributes : map of attributes
 \pre attributes["vloc"] is defined and follows the syntax required by function
 tchecker::from_string(tchecker::vloc_t &, tchecker::system::system_t const &, std::string const &);
 attributes["inval"] is defined and follows the syntax required by function
 tchecker::from_string(tchecker::intval_t &, tchecker::system::system_t const &, std::string const &);
 attributes["zone"] is defined and follows the syntax required by function
 tchecker::from_string(tchecker::clock_constraint_container_t &, tchecker::clock_variables_t const &,
 std::string const &);
 over reference clocks $0,...,$k and clocks
 \post vloc has been initialized from attributes["vloc"]
 intval has been initialized from attributes["intval"]
 zone has been initialized from attributes["zone"], invariant and spread
 vedge has been initialized to the empty vector of edges
 and invariant contains the invariant in vloc
 \return tchecker::STATE_OK if initialization succeeded
 tchecker::STATE_BAD if initialization failed
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if attributes["intval"] does not satisfy the invariant in
 vloc
 tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if attributes["zone"] intersected with the invariant in
 vloc is empty
 tchecker::STATE_CLOCKS_EMPTY_SPREAD if attributes["zone"] intersected with the invariant in
 vloc, and spread-bounded is empty
 \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for no spread
 */
tchecker::state_status_t initialize(tchecker::ta::system_t const & system,
                                    tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                    tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                    tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone,
                                    tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                    tchecker::clock_constraint_container_t & invariant, tchecker::integer_t spread,
                                    std::map<std::string, std::string> const & attributes);

/*!
 \brief Initialization from attributes
 \param system : a system
 \param s : state
 \param t : transition
 \param spread : spread bound over reference clocks
 \param attributes : map of attributes
 \post s and t have been initialized from attributes["vloc"], attributes["intval"], attributes["zone"] and
 spread
 the src invariant container in t contains the invariant of state s
 \return tchecker::STATE_OK if initialization succeeded
 tchecker::STATE_BAD otherwise
 tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED if attributes["intval"] does not satisfy the invariant in
 attributes["vloc"]
 tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED if attributes["zone"] intersected with the invariant in
 vloc is empty
 tchecker::STATE_CLOCKS_EMPTY_SPREAD if attributes["zone"] intersected with the invariant in
 vloc, and spread-bounded is empty
*/
inline tchecker::state_status_t initialize(tchecker::ta::system_t const & system, tchecker::refzg::state_t & s,
                                           tchecker::refzg::transition_t & t, tchecker::integer_t spread,
                                           std::map<std::string, std::string> const & attributes)
{
  return tchecker::refzg::initialize(system, s.vloc_ptr(), s.intval_ptr(), s.zone_ptr(), t.vedge_ptr(),
                                     t.src_invariant_container(), spread, attributes);
}

// refzg_t

/*!
 \class refzg_t
 \brief Transition system of the zone graph with reference clocks over system of
 timed processes with state and transition allocation
 \note all returned states and transitions are deallocated automatically
 */
class refzg_t final
    : public tchecker::ts::fwd_t<tchecker::refzg::state_sptr_t, tchecker::refzg::const_state_sptr_t,
                                 tchecker::refzg::transition_sptr_t, tchecker::refzg::const_transition_sptr_t>,
      public tchecker::ts::bwd_t<tchecker::refzg::state_sptr_t, tchecker::refzg::const_state_sptr_t,
                                 tchecker::refzg::transition_sptr_t, tchecker::refzg::const_transition_sptr_t>,
      public tchecker::ts::fwd_impl_t<tchecker::refzg::state_sptr_t, tchecker::refzg::const_state_sptr_t,
                                      tchecker::refzg::transition_sptr_t, tchecker::refzg::const_transition_sptr_t,
                                      tchecker::refzg::initial_range_t, tchecker::refzg::outgoing_edges_range_t,
                                      tchecker::refzg::initial_value_t, tchecker::refzg::outgoing_edges_value_t>,
      public tchecker::ts::bwd_impl_t<tchecker::refzg::state_sptr_t, tchecker::refzg::const_state_sptr_t,
                                      tchecker::refzg::transition_sptr_t, tchecker::refzg::const_transition_sptr_t,
                                      tchecker::refzg::final_range_t, tchecker::refzg::incoming_edges_range_t,
                                      tchecker::refzg::final_value_t, tchecker::refzg::incoming_edges_value_t>,
      public tchecker::ts::builder_t<tchecker::refzg::state_sptr_t, tchecker::refzg::transition_sptr_t>,
      public tchecker::ts::inspector_t<tchecker::refzg::const_state_sptr_t, tchecker::refzg::const_transition_sptr_t>,
      public tchecker::ts::sharing_t<tchecker::refzg::state_sptr_t, tchecker::refzg::transition_sptr_t> {
public:
  // Inherited types
  using fwd_t = tchecker::ts::fwd_t<tchecker::refzg::state_sptr_t, tchecker::refzg::const_state_sptr_t,
                                    tchecker::refzg::transition_sptr_t, tchecker::refzg::const_transition_sptr_t>;
  using bwd_t = tchecker::ts::bwd_t<tchecker::refzg::state_sptr_t, tchecker::refzg::const_state_sptr_t,
                                    tchecker::refzg::transition_sptr_t, tchecker::refzg::const_transition_sptr_t>;
  using fwd_impl_t = tchecker::ts::fwd_impl_t<tchecker::refzg::state_sptr_t, tchecker::refzg::const_state_sptr_t,
                                              tchecker::refzg::transition_sptr_t, tchecker::refzg::const_transition_sptr_t,
                                              tchecker::refzg::initial_range_t, tchecker::refzg::outgoing_edges_range_t,
                                              tchecker::refzg::initial_value_t, tchecker::refzg::outgoing_edges_value_t>;
  using bwd_impl_t = tchecker::ts::bwd_impl_t<tchecker::refzg::state_sptr_t, tchecker::refzg::const_state_sptr_t,
                                              tchecker::refzg::transition_sptr_t, tchecker::refzg::const_transition_sptr_t,
                                              tchecker::refzg::final_range_t, tchecker::refzg::incoming_edges_range_t,
                                              tchecker::refzg::final_value_t, tchecker::refzg::incoming_edges_value_t>;
  using builder_t = tchecker::ts::builder_t<tchecker::refzg::state_sptr_t, tchecker::refzg::transition_sptr_t>;
  using inspector_t = tchecker::ts::inspector_t<tchecker::refzg::const_state_sptr_t, tchecker::refzg::const_transition_sptr_t>;
  using sharing_t = tchecker::ts::sharing_t<tchecker::refzg::state_sptr_t, tchecker::refzg::transition_sptr_t>;

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
   \param r : reference clock variables
   \param semantics : a semantics over zones with reference clocks
   \param spread : spread bound over reference clocks
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash tables
   \note all states and transitions are pool allocated and deallocated
   automatically
   \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
   */
  refzg_t(std::shared_ptr<tchecker::ta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
          std::shared_ptr<tchecker::reference_clock_variables_t const> const & r,
          std::shared_ptr<tchecker::refzg::semantics_t> const & semantics, tchecker::integer_t spread, std::size_t block_size,
          std::size_t table_size = 65536);

  /*!
   \brief Copy constructor (deleted)
   */
  refzg_t(tchecker::refzg::refzg_t const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  refzg_t(tchecker::refzg::refzg_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~refzg_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::refzg::refzg_t & operator=(tchecker::refzg::refzg_t const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::refzg::refzg_t & operator=(tchecker::refzg::refzg_t &&) = delete;

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
   \brief Initial state and transition
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
  virtual outgoing_edges_range_t outgoing_edges(tchecker::refzg::const_state_sptr_t const & s);

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
  virtual void next(tchecker::refzg::const_state_sptr_t const & s, outgoing_edges_value_t const & out_edge,
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
  virtual void next(tchecker::refzg::const_state_sptr_t const & s, std::vector<sst_t> & v,
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
  virtual incoming_edges_range_t incoming_edges(tchecker::refzg::const_state_sptr_t const & s);

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
  virtual void prev(tchecker::refzg::const_state_sptr_t const & s, incoming_edges_value_t const & in_edge,
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
  virtual void prev(tchecker::refzg::const_state_sptr_t const & s, std::vector<sst_t> & v,
                    tchecker::state_status_t mask = tchecker::STATE_OK);

  // Builder

  /*!
   \brief State/transition building from attributes
   \param attributes : a map of attributes
   \param v : container
   \param mask : mask on states
   \post all tuples (status, s, t) where s and t have been initialized from attributes,
   and status matches mask (i.e. status & mask != 0) have been pushed to v
   \pre see tchecker::refzg::initialize
   \post a triple <status, s, t> has been pushed to v (if status matches mask), where the vector of
   locations in s has been initialized from attributes["vloc"], the integer valuation in s has been
   initialized from attributes["intval"], the zone in s has been initialized from attributes["zone"]
   and the invariant in the vector of locations in s, the vector of edges in t is empty and the source
   invariant in t has been initialized to the invariant in vloc
  */
  virtual void build(std::map<std::string, std::string> const & attributes, std::vector<sst_t> & v,
                     tchecker::state_status_t mask = tchecker::STATE_OK);

  // Inspector

  /*!
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  virtual boost::dynamic_bitset<> labels(tchecker::refzg::const_state_sptr_t const & s) const;

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post attributes of state s have been added to map m
   */
  virtual void attributes(tchecker::refzg::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post attributes of transition t have been added to map m
   */
  virtual void attributes(tchecker::refzg::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const;

  /*!
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true is s has a non-empty zone that contains a synchronized
   valuation, false otherwise
  */
  virtual bool is_valid_final(tchecker::refzg::const_state_sptr_t const & s) const;

  /*!
   \brief Checks if a state is initial
   \param s : a state
   \return true if s is an initial state, false otherwise
  */
  virtual bool is_initial(tchecker::refzg::const_state_sptr_t const & s) const;

  // Sharing

  /*!
   \brief Share state components
   \param s : a state
   \post internal components in s have been shared
   \note THE RESULTING STATE SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::refzg::state_sptr_t & s);

  /*!
   \brief Share transition components
   \param t : a transition
   \post internal components in t have been shared
   \note THE RESULTING TRANSITION SHOULD NOT BE MODIFIED
  */
  virtual void share(tchecker::refzg::transition_sptr_t & t);

  /*!
   \brief Accessor
   \return Shared pointer to underlying system of timed processes
  */
  inline std::shared_ptr<tchecker::ta::system_t const> const & system_ptr() const { return _system; }

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

  /*!
   \brief Accessor
   \return Spread
  */
  inline tchecker::integer_t spread() const { return _spread; }

private:
  std::shared_ptr<tchecker::ta::system_t const> _system;              /*!< System of timed processes */
  enum tchecker::ts::sharing_type_t _sharing_type;                    /*!< Sharing of state/transition components */
  std::shared_ptr<tchecker::reference_clock_variables_t const> _r;    /*!< Reference clock variables */
  std::shared_ptr<tchecker::refzg::semantics_t> _semantics;           /*!< Zone semantics */
  tchecker::integer_t _spread;                                        /*!< Spread bound over reference clocks */
  tchecker::refzg::state_pool_allocator_t _state_allocator;           /*!< Pool allocator of states */
  tchecker::refzg::transition_pool_allocator_t _transition_allocator; /*! Pool allocator of transitions */
};

/*!
 \brief Compute initial state of a zone graph with reference clocks from a tuple of locations
 \param refzg : zone graph with reference clocks
 \param vloc : tuple of locations
 \param mask : state status mask
 \return the initial state of refzg with tuple of locations vloc and status
 compatible with mask if any, nullptr otherwise
 */
tchecker::refzg::state_sptr_t initial(tchecker::refzg::refzg_t & refzg, tchecker::vloc_t const & vloc,
                                      tchecker::state_status_t mask = tchecker::STATE_OK);

/*!
 \brief Compute next state and transition from a tuple of edges
 \param refzg : zone graph with reference clocks
 \param s : state
 \param vedge : tuple of edges
 \param mask : next state status mask
 \return the pair (nexts, nextt) of successor state and transition of s
 along tuple of edges vedge if any, (nullptr, nullptr) otherwise
 */
std::tuple<tchecker::refzg::state_sptr_t, tchecker::refzg::transition_sptr_t>
next(tchecker::refzg::refzg_t & refzg, tchecker::refzg::const_state_sptr_t const & s, tchecker::vedge_t const & vedge,
     tchecker::state_status_t mask = tchecker::STATE_OK);

/* Factory */

/*!
 \brief Type of reference clocks
*/
enum reference_clock_variables_type_t {
  SINGLE_REFERENCE_CLOCKS,  /*!< Single reference clocks (synchronous time) */
  PROCESS_REFERENCE_CLOCKS, /*!< One reference clock par process (asynchronous time) */
};

/*!
 \brief Factory of zone graphs with reference clocks
 \param system : system of timed processes
 \param sharing_type : type of sharing
 \param refclocks_type : type of reference clocks
 \param semantics_type : type of semantics over zones with reference clocks
 \param spread : spread bound over reference clocks
 \param block_size : number of objects allocated in a block
 \param table_size : size of hash tables
 \return a zone graph over system with zone semantics and spread bound
 defined from semantics_type and spread, reference clocks defined from
 refclocks_type, and allocation of block_size objects at a time
 \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
 */
tchecker::refzg::refzg_t * factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                                   enum tchecker::ts::sharing_type_t sharing_type,
                                   enum tchecker::refzg::reference_clock_variables_type_t refclocks_type,
                                   enum tchecker::refzg::semantics_type_t semantics_type, tchecker::integer_t spread,
                                   std::size_t block_size, std::size_t table_size);

} // end of namespace refzg

} // end of namespace tchecker

#endif // TCHECKER_REFZG_HH
