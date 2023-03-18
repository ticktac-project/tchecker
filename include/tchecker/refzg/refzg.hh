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
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"

/*!
 \file refzg.hh
 \brief Zone graphs with reference clocks
 */

namespace tchecker {

namespace refzg {

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

/*!
 \class refzg_impl_t
 \brief Low-level transition system implementation of zone graph with reference
 clocks of a timed automaton
 */
class refzg_impl_t final
    : public tchecker::ts::ts_impl_t<tchecker::refzg::state_sptr_t, tchecker::refzg::const_state_sptr_t,
                                     tchecker::refzg::transition_sptr_t, tchecker::refzg::const_transition_sptr_t,
                                     tchecker::refzg::initial_range_t, tchecker::refzg::outgoing_edges_range_t,
                                     tchecker::refzg::initial_value_t, tchecker::refzg::outgoing_edges_value_t> {
public:
  // Inherited types
  using ts_impl_t = tchecker::ts::ts_impl_t<tchecker::refzg::state_sptr_t, tchecker::refzg::const_state_sptr_t,
                                            tchecker::refzg::transition_sptr_t, tchecker::refzg::const_transition_sptr_t,
                                            tchecker::refzg::initial_range_t, tchecker::refzg::outgoing_edges_range_t,
                                            tchecker::refzg::initial_value_t, tchecker::refzg::outgoing_edges_value_t>;
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
   \param r : reference clock variables
   \param semantics : a semantics over zones with reference clocks
   \param spread : spread bound over reference clocks
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash tables
   \note all states and transitions are pool allocated and deallocated
   automatically
   \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
   */
  refzg_impl_t(std::shared_ptr<tchecker::ta::system_t const> const & system,
               std::shared_ptr<tchecker::reference_clock_variables_t const> const & r,
               std::shared_ptr<tchecker::refzg::semantics_t> const & semantics, tchecker::integer_t spread,
               std::size_t block_size, std::size_t table_size = 65536);

  /*!
   \brief Copy constructor (deleted)
   */
  refzg_impl_t(tchecker::refzg::refzg_impl_t const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  refzg_impl_t(tchecker::refzg::refzg_impl_t &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~refzg_impl_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::refzg::refzg_impl_t & operator=(tchecker::refzg::refzg_impl_t const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::refzg::refzg_impl_t & operator=(tchecker::refzg::refzg_impl_t &&) = delete;

  using ts_impl_t::state;
  using ts_impl_t::status;
  using ts_impl_t::transition;

  /*!
   \brief Accessor
   \return range of initial edges
   */
  virtual tchecker::refzg::initial_range_t initial_edges();

  /*!
   \brief Initial state and transition
   \param init_edge : initial state valuation
   \param v : container
   \post triples (status, s, t) have been added to v, for each initial state s
   and initial transition t that are initialized from init_edge.
   */
  virtual void initial(tchecker::refzg::initial_value_t const & init_edge, std::vector<sst_t> & v);

  using ts_impl_t::initial;

  /*!
   \brief Accessor
   \param s : state
   \return outgoing edges from state s
   */
  virtual tchecker::refzg::outgoing_edges_range_t outgoing_edges(tchecker::refzg::const_state_sptr_t const & s);

  /*!
   \brief Next state and transition
   \param s : state
   \param out_edge : outgoing edge value
   \param v : container
   \post triples (status, s', t') have been added to v, for each successor state
   s' and transition t from s to s' along outgoing edge out_edge
   */
  virtual void next(tchecker::refzg::const_state_sptr_t const & s, tchecker::refzg::outgoing_edges_value_t const & out_edge,
                    std::vector<sst_t> & v);

  using ts_impl_t::next;

  /*!
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  virtual boost::dynamic_bitset<> labels(tchecker::refzg::const_state_sptr_t const & s) const;

  /*!
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true is s has a non-empty zone that contains a synchronized
   valuation, false otherwise
  */
  virtual bool is_valid_final(tchecker::refzg::const_state_sptr_t const & s) const;

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
  std::shared_ptr<tchecker::ta::system_t const> const & system_ptr() const;

  /*!
   \brief Accessor
   \return Underlying system of timed processes
   */
  tchecker::ta::system_t const & system() const;

  /*!
   \brief Accessor
   \return Spread
  */
  tchecker::integer_t spread() const;

private:
  std::shared_ptr<tchecker::ta::system_t const> _system;              /*!< System of timed processes */
  std::shared_ptr<tchecker::reference_clock_variables_t const> _r;    /*!< Reference clock variables */
  std::shared_ptr<tchecker::refzg::semantics_t> _semantics;           /*!< Zone semantics */
  tchecker::integer_t _spread;                                        /*!< Spread bound over reference clocks */
  tchecker::refzg::state_pool_allocator_t _state_allocator;           /*!< Pool allocator of states */
  tchecker::refzg::transition_pool_allocator_t _transition_allocator; /*! Pool allocator of transitions */
};

/*!
 \class refzg_t
 \brief Zone graph with reference clocks of a timed automaton with allocation of
 states and transitions, as well as states and transitions sharing
 \note all returned states and transitions deallocated automatically
 */
class refzg_t final : public tchecker::ts::make_ts_from_impl_t<tchecker::refzg::refzg_impl_t> {
public:
  using tchecker::ts::make_ts_from_impl_t<tchecker::refzg::refzg_impl_t>::make_ts_from_impl_t;

  /*!
   \brief Destructor
  */
  virtual ~refzg_t() = default;

  /*!
   \brief Accessor
   \return Shared pointer to underlying system of timed processes
  */
  std::shared_ptr<tchecker::ta::system_t const> const & system_ptr() const;

  /*!
   \brief Accessor
   \return Underlying system of timed processes
   */
  tchecker::ta::system_t const & system() const;

  /*!
   \brief Accessor
   \return Spread
  */
  tchecker::integer_t spread() const;
};

/*!
 \class sharing_refzg_t
 \brief Zone graph with reference clocks of a timed automaton with allocation of
 states and transitions, as well as states and transitions sharing
 \note all returned states and transitions deallocated automatically
 */
class sharing_refzg_t final : public tchecker::ts::make_sharing_ts_from_impl_t<tchecker::refzg::refzg_impl_t> {
public:
  using tchecker::ts::make_sharing_ts_from_impl_t<tchecker::refzg::refzg_impl_t>::make_sharing_ts_from_impl_t;

  /*!
   \brief Destructor
  */
  virtual ~sharing_refzg_t() = default;

  /*!
   \brief Accessor
   \return Shared pointer to underlying system of timed processes
  */
  std::shared_ptr<tchecker::ta::system_t const> const & system_ptr() const;

  /*!
   \brief Accessor
   \return Underlying system of timed processes
   */
  tchecker::ta::system_t const & system() const;

  /*!
   \brief Accessor
   \return Spread
  */
  tchecker::integer_t spread() const;
};

/*!
 \brief Compute initial state of a zone graph with reference clocks from a tuple of locations
 \param refzg : zone graph with reference clocks
 \param vloc : tuple of locations
 \param mask : state status mask
 \return the initial state of refzg with tuple of locations vloc and status
 compatible with mask if any, nullptr otherwise
 */
template <class REFZG>
tchecker::refzg::state_sptr_t initial(REFZG & refzg, tchecker::vloc_t const & vloc,
                                      tchecker::state_status_t mask = tchecker::STATE_OK)
{
  std::vector<typename REFZG::sst_t> v;
  refzg.initial(v, mask);
  for (auto && [status, s, t] : v) {
    if (s->vloc() == vloc)
      return s;
  }
  return nullptr;
}

/*!
 \brief Compute next state and transition from a tuple of edges
 \param refzg : zone graph with reference clocks
 \param s : state
 \param vedge : tuple of edges
 \param mask : next state status mask
 \return the pair (nexts, nextt) of successor state and transition of s
 along tuple of edges vedge if any, (nullptr, nullptr) otherwise
 */
template <class REFZG>
std::tuple<tchecker::refzg::state_sptr_t, tchecker::refzg::transition_sptr_t>
next(REFZG & refzg, tchecker::refzg::const_state_sptr_t const & s, tchecker::vedge_t const & vedge,
     tchecker::state_status_t mask = tchecker::STATE_OK)
{
  std::vector<typename REFZG::sst_t> v;
  refzg.next(s, v, mask);
  for (auto && [status, nexts, nextt] : v)
    if (nextt->vedge() == vedge)
      return std::make_tuple(nexts, nextt);
  return std::make_tuple(nullptr, nullptr);
}

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
                                   enum tchecker::refzg::reference_clock_variables_type_t refclocks_type,
                                   enum tchecker::refzg::semantics_type_t semantics_type, tchecker::integer_t spread,
                                   std::size_t block_size, std::size_t table_size);

/*!
 \brief Factory of zone graphs with reference clocks and states/transitions sharing
 \param system : system of timed processes
 \param refclocks_type : type of reference clocks
 \param semantics_type : type of semantics over zones with reference clocks
 \param spread : spread bound over reference clocks
 \param block_size : number of objects allocated in a block
 \param table_size : size of hash tables
 \return a zone graph over system with zone semantics and spread bound
 defined from semantics_type and spread, reference clocks defined from
 refclocks_type, and allocation of block_size objects at a time
 \note set spread to tchecker::refdbm::UNBOUNDED_SPREAD for unbounded spread
 \note the states and transitions computed by the returned zone graph share
 internal components
 */
tchecker::refzg::sharing_refzg_t * factory_sharing(std::shared_ptr<tchecker::ta::system_t const> const & system,
                                                   enum tchecker::refzg::reference_clock_variables_type_t refclocks_type,
                                                   enum tchecker::refzg::semantics_type_t semantics_type,
                                                   tchecker::integer_t spread, std::size_t block_size, std::size_t hash_table);

} // end of namespace refzg

} // end of namespace tchecker

#endif // TCHECKER_REFZG_HH
