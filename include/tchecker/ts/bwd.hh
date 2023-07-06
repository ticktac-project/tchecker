/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_BWD_HH
#define TCHECKER_TS_BWD_HH

#include <tuple>
#include <type_traits>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"

/*!
 \file bwd.hh
 \brief Transition systems interfaces for backward state-space exploration
 */

namespace tchecker {

namespace ts {

/*!
 \class bwd_t
 \brief Transition system interface, backward state-space exploration
 \tparam STATE : type of state
 \tparam CONST_STATE : type of const state
 \tparam TRANSITION : type of transition
 \tparam CONST_TRANSITION : type of const transition
*/
template <class STATE, class CONST_STATE, class TRANSITION, class CONST_TRANSITION> class bwd_t {
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
  virtual ~bwd_t() = default;

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
  \brief Final states and transitions with selected status
  \param labels : set of final labels
  \param v : container
  \param mask : mask on initial states
  \post all tuples (status, s, t) of status, states and transitions that satisfy
  labels, and such that status matches mask (i.e. status & mask != 0), have been
  pushed back into v
  */
  virtual void final(boost::dynamic_bitset<> const & labels, std::vector<sst_t> & v, tchecker::state_status_t mask) = 0;

  /*!
  \brief Previous states and transitions with selected status
  \param s : state
  \param v : container
  \param mask : mask on next states
  \post all tuples (status, s', t) such that s' -t-> s is a transition and the
  status of s' matches mask (i.e. status & mask != 0) have been pushed to v
  */
  virtual void prev(const_state_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask) = 0;
};

/*!
 \class bwd_impl_t
 \brief Low-level interface of backward transition systems in TChecker (used to implement tchecker::ts::bwd_t)
 \tparam STATE : type of state
 \tparam CONST_STATE : type of const state
 \tparam TRANSITION : type of transition
 \tparam CONST_TRANSITION : type of const transition
 \tparam FINAL_RANGE : type of range of final edges, should be tchecker::make_range_t<...>
 \tparam INCOMING_EDGES_RANGE : type of range of incoming edges, should be tchecker::make_range_t<...>
 \tparam FINAL_VALUE : type of value in FINAL_RANGE
 \tparam INCOMING_EDGES_VALUE : type of value in INCOMING_EDGES_RANGE
*/
template <class STATE, class CONST_STATE, class TRANSITION, class CONST_TRANSITION, class FINAL_RANGE,
          class INCOMING_EDGES_RANGE,
          class FINAL_VALUE = typename std::iterator_traits<typename FINAL_RANGE::begin_iterator_t>::reference_type const,
          class INCOMING_EDGES_VALUE =
              typename std::iterator_traits<typename INCOMING_EDGES_RANGE::begin_iterator_t>::reference_type const>
class bwd_impl_t {
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
   \brief Type of range of final edges
   */
  using final_range_t = FINAL_RANGE;

  /*!
   \brief Value type for range over final states
   */
  using final_value_t = FINAL_VALUE;

  /*!
   \brief Type of range over incoming edges
   */
  using incoming_edges_range_t = INCOMING_EDGES_RANGE;

  /*!
   \brief Value type for range of incoming edges
   */
  using incoming_edges_value_t = INCOMING_EDGES_VALUE;

  /*!
  \brief Type of tuples (status, state, transition)
  */
  using sst_t = std::tuple<tchecker::state_status_t, state_t, transition_t>;

  /*!
   \brief Destructor
   */
  virtual ~bwd_impl_t() = default;

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
   \param labels : final states labels
   \return Final edges
   */
  virtual final_range_t final_edges(boost::dynamic_bitset<> const & labels) = 0;

  /*!
   \brief Final states and transitions
   \param final_edge : final edge valuation
   \param v : container
   \param mask : mask on initial states
   \post triples (status, s, t) have been added to v, for each final state s and transition t
   such that status matches mask (i.e. status & mask != 0)
   */
  virtual void final(final_value_t const & final_edge, std::vector<sst_t> & v, tchecker::state_status_t mask) = 0;

  /*!
   \brief Accessor
   \param s : state
   \return incoming edges to state s
   */
  virtual incoming_edges_range_t incoming_edges(const_state_t const & s) = 0;

  /*!
   \brief Previous state and transition
   \param s : state
   \param in_edge : incoming edge value
   \param v : container
   \param mask : mask on initial states
   \post triples (status, s', t') have been added to v, for each incoming transition s'-t'->s
   along in_edge such that status matches mask (i.e. status & mask != 0)
   */
  virtual void prev(const_state_t const & s, incoming_edges_value_t const & in_edge, std::vector<sst_t> & v,
                    tchecker::state_status_t mask) = 0;
};

/*!
 \brief Computation of final states and transitions with selected status
 \tparam BWD_IMPL : type of low-level backward transition system (should derive
 from tchecker::ts::bwd_impl_t<...>)
 \param bwd_impl : low-level backward transition system
 \param labels : labels of final states
 \param v : container
 \param mask : mask on initial states
 \post all tuples (status, s, t) of status, final states and transitions such
 that status matches mask (i.e. status & mask != 0) have been pushed back into v
 \note this is a possible implementation of method tchecker::ts::bwd_t::final()
 using tchecker::ts::bwd_impl_t<...>
*/
template <class BWD_IMPL>
void final(BWD_IMPL & bwd_impl, boost::dynamic_bitset<> const & labels, std::vector<typename BWD_IMPL::sst_t> & v,
           tchecker::state_status_t mask)
{
  typename BWD_IMPL::final_range_t final_edges = bwd_impl.final_edges(labels);
  for (typename BWD_IMPL::final_value_t && final_edge : final_edges)
    bwd_impl.final(final_edge, v, mask);
}

/*!
 \brief Computation of previous states and transitions with selected status
 \tparam BWD_IMPL : type of low-level backward transition system (should derive
 from tchecker::ts::bwd_impl_t<...>)
 \param bwd_impl : low-level backward transition system
 \param s : state
 \param v : container
 \param mask : mask on next states
 \post all tuples (status, s', t) such that s' -t-> s is a transition and the
 status of s' matches mask (i.e. status & mask != 0) have been pushed to v
 \note this is a possible implementation of method tchecker::ts::bwd_t::next()
 using tchecker::ts::bwd_impl_t<...>
*/
template <class BWD_IMPL>
void prev(BWD_IMPL & bwd_impl, typename BWD_IMPL::const_state_t const & s, std::vector<typename BWD_IMPL::sst_t> & v,
          tchecker::state_status_t mask)
{
  typename BWD_IMPL::incoming_edges_range_t in_edges = bwd_impl.incoming_edges(s);
  for (typename BWD_IMPL::incoming_edges_value_t && in_edge : in_edges)
    bwd_impl.prev(s, in_edge, v, mask);
}

} // end of namespace ts

} // end of namespace tchecker

#endif // TCHECKER_TS_BWD_HH
