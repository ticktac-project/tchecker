/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_FWD_HH
#define TCHECKER_TS_FWD_HH

#include <tuple>
#include <type_traits>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"

/*!
 \file fwd.hh
 \brief Transition systems interfaces for forward state-space exploration
 */

namespace tchecker {

namespace ts {

/*!
 \class fwd_t
 \brief Transition system interface, forward state-space exploration
 \tparam STATE : type of state
 \tparam CONST_STATE : type of const state
 \tparam TRANSITION : type of transition
 \tparam CONST_TRANSITION : type of const transition
*/
template <class STATE, class CONST_STATE, class TRANSITION, class CONST_TRANSITION> class fwd_t {
public:
  /*!
   \brief Type of state
   */
  using state_t = STATE;

  /*!
  \brief Type of const state
  */
  using const_state_t = CONST_STATE;

  /*!
   \brief Type of transition
   */
  using transition_t = TRANSITION;

  /*!
   \brief Type of const transition
  */
  using const_transition_t = CONST_TRANSITION;

  /*!
  \brief Type of tuples (status, state, transition)
  */
  using sst_t = std::tuple<tchecker::state_status_t, state_t, transition_t>;

  /*!
   \brief Destructor
   */
  virtual ~fwd_t() = default;

  /*!
   \brief Accessor
   \param sst : a tuple (status, state, transition)
   \return the status in sst
  */
  inline tchecker::state_status_t status(sst_t const & sst) const { return std::get<0>(sst); }

  /*!
   \brief Accessor
   \param sst : a tuple (status, state, transition)
   \return the state in sst
  */
  inline state_t state(sst_t const & sst) const { return std::get<1>(sst); }

  /*!
   \brief Accessor
   \param sst : a tuple (status, state, transition)
   \return the transition in sst
  */
  inline transition_t transition(sst_t const & sst) const { return std::get<2>(sst); }

  /*!
  \brief Initial states and transitions with selected status
  \param v : container
  \param mask : mask on initial states
  \post all tuples (status, s, t) of status, initial states and transitions such
  that status matches mask (i.e. status & mask != 0) have been pushed back into v
   */
  virtual void initial(std::vector<sst_t> & v, tchecker::state_status_t mask) = 0;

  /*!
  \brief Next states and transitions with selected status
  \param s : state
  \param v : container
  \param mask : mask on next states
  \post all tuples (status, s', t) such that s -t-> s' is a transition and the
  status of s' matches mask (i.e. status & mask != 0) have been pushed to v
  */
  virtual void next(const_state_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask) = 0;
};

/*!
 \class fwd_impl_t
 \brief Low-level interface of forward transition systems in TChecker (used to implement tchecker::ts::fwd_t)
 \tparam STATE : type of state
 \tparam CONST_STATE : type of const state
 \tparam TRANSITION : type of transition
 \tparam CONST_TRANSITION : type of const transition
 \tparam INITIAL_RANGE : type of range of initial states, should be tchecker::make_range_t<...>
 \tparam OUTGOING_EDGES_RANGE : type of range of outgoing edges, should be tchecker::make_range_t<...>
 \tparam INITIAL_VALUE : type of value in INITIAL_RANGE
 \tparam OUTGOING_EDGES_VALUE : type of value in OUTGOING_EDGES_RANGE
*/
template <class STATE, class CONST_STATE, class TRANSITION, class CONST_TRANSITION, class INITIAL_RANGE,
          class OUTGOING_EDGES_RANGE,
          class INITIAL_VALUE = typename std::iterator_traits<typename INITIAL_RANGE::begin_iterator_t>::reference_type const,
          class OUTGOING_EDGES_VALUE =
              typename std::iterator_traits<typename OUTGOING_EDGES_RANGE::begin_iterator_t>::reference_type const>
class fwd_impl_t {
public:
  /*!
   \brief Type of state
   */
  using state_t = STATE;

  /*!
  \brief Type of const state
  */
  using const_state_t = CONST_STATE;

  /*!
   \brief Type of transition
   */
  using transition_t = TRANSITION;

  /*!
   \brief Type of const transition
  */
  using const_transition_t = CONST_TRANSITION;

  /*!
   \brief Type of range of initial states
   */
  using initial_range_t = INITIAL_RANGE;

  /*!
   \brief Value type for range over initial states
   */
  using initial_value_t = INITIAL_VALUE;

  /*!
   \brief Type of range over outgoing edges
   */
  using outgoing_edges_range_t = OUTGOING_EDGES_RANGE;

  /*!
   \brief Value type for range of outgoing edges
   */
  using outgoing_edges_value_t = OUTGOING_EDGES_VALUE;

  /*!
  \brief Type of tuples (status, state, transition)
  */
  using sst_t = std::tuple<tchecker::state_status_t, state_t, transition_t>;

  /*!
   \brief Destructor
   */
  virtual ~fwd_impl_t() = default;

  /*!
   \brief Accessor
   \param sst : a tuple (status, state, transition)
   \return the status in sst
  */
  inline tchecker::state_status_t status(sst_t const & sst) const { return std::get<0>(sst); }

  /*!
   \brief Accessor
   \param sst : a tuple (status, state, transition)
   \return the state in sst
  */
  inline state_t state(sst_t const & sst) const { return std::get<1>(sst); }

  /*!
   \brief Accessor
   \param sst : a tuple (status, state, transition)
   \return the transition in sst
  */
  inline transition_t transition(sst_t const & sst) const { return std::get<2>(sst); }

  /*!
   \brief Accessor
   \return Initial edges
   */
  virtual initial_range_t initial_edges() = 0;

  /*!
   \brief Initial state and transition
   \param init_edge : initial state valuation
   \param v : container
   \param mask : mask on next states
   \post triples (status, s, t) have been added to v, for each initial state s
   and initial transition t from init_edge, such that status matches mask (i.e. status & mask != 0)
   */
  virtual void initial(initial_value_t const & init_edge, std::vector<sst_t> & v, tchecker::state_status_t mask) = 0;

  /*!
   \brief Accessor
   \param s : state
   \return outgoing edges from state s
   */
  virtual outgoing_edges_range_t outgoing_edges(const_state_t const & s) = 0;

  /*!
   \brief Next state and transition
   \param s : state
   \param out_edge : outgoing edge value
   \param v : container
   \param mask : mask on next states
   \post triples (status, s', t') have been added to v, for each transition s -t'-> s' along outgoing
   edge out_edge such that status matches mask (i.e. status & mask != 0)
   */
  virtual void next(const_state_t const & s, outgoing_edges_value_t const & out_edge, std::vector<sst_t> & v,
                    tchecker::state_status_t mask) = 0;
};

/*!
 \brief Computation of initial states and transitions with selected status
 \tparam FWD_IMPL : type of low-level forward transition system (should derive
 from tchecker::ts::fwd_impl_t<...>)
 \param fwd_impl : low-level forward transition system
 \param v : container
 \param mask : mask on initial states
 \post all tuples (status, s, t) of status, initial states and transitions such
 that status matches mask (i.e. status & mask != 0) have been pushed back into v
 \note this is a possible implementation of method tchecker::ts::fwd_t::initial()
 using tchecker::ts::fwd_impl_t<...>
*/
template <class FWD_IMPL>
void initial(FWD_IMPL & fwd_impl, std::vector<typename FWD_IMPL::sst_t> & v, tchecker::state_status_t mask)
{
  typename FWD_IMPL::initial_range_t init_edges = fwd_impl.initial_edges();
  for (typename FWD_IMPL::initial_value_t && init_edge : init_edges)
    fwd_impl.initial(init_edge, v, mask);
}

/*!
 \brief Computation of next states and transitions with selected status
 \tparam FWD_IMPL : type of low-level forward transition system (should derive
 from tchecker::ts::fwd_impl_t<...>)
 \param fwd_impl : low-level forward transition system
 \param s : state
 \param v : container
 \param mask : mask on next states
 \post all tuples (status, s', t) such that s -t-> s' is a transition and the
 status of s' matches mask (i.e. status & mask != 0) have been pushed to v
 \note this is a possible implementation of method tchecker::ts::fwd_t::next()
 using tchecker::ts::fwd_impl_t<...>
*/
template <class FWD_IMPL>
void next(FWD_IMPL & fwd_impl, typename FWD_IMPL::const_state_t const & s, std::vector<typename FWD_IMPL::sst_t> & v,
          tchecker::state_status_t mask)
{
  typename FWD_IMPL::outgoing_edges_range_t out_edges = fwd_impl.outgoing_edges(s);
  for (typename FWD_IMPL::outgoing_edges_value_t && out_edge : out_edges)
    fwd_impl.next(s, out_edge, v, mask);
}

} // end of namespace ts

} // end of namespace tchecker

#endif // TCHECKER_TS_FWD_HH
