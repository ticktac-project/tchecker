/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_HH
#define TCHECKER_TS_HH

#include <tuple>
#include <type_traits>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/ts/state.hh"
#include "tchecker/ts/transition.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file ts.hh
 \brief Interface to transition system
 */

namespace tchecker {

namespace ts {

/*!
 \class ts_t
 \brief Transition system
 \tparam STATE : type of state
 \tparam CONST_STATE : type of const state
 \tparam TRANSITION : type of transition
 \tparam INITIAL_ITERATOR : type of iterator over initial states
 \tparam OUTGOING_EDGES_ITERATOR : type of iterator over outgoing edges
 \tparam INITIAL_ITERATOR_VALUE : type of value for INITIAL_ITERATOR
 \tparam OUTGOING_EDGES_ITERATOR_VALUE : type of value for OUTGOING_EDGES_ITERATOR
 */
template <class STATE, class CONST_STATE, class TRANSITION, class INITIAL_ITERATOR, class OUTGOING_EDGES_ITERATOR,
          class INITIAL_ITERATOR_VALUE = typename std::iterator_traits<INITIAL_ITERATOR>::reference_type const,
          class OUTGOING_EDGES_ITERATOR_VALUE = typename std::iterator_traits<OUTGOING_EDGES_ITERATOR>::reference_type const>
class ts_t {
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
   \brief Type of iterator over initial states
   */
  using initial_iterator_t = INITIAL_ITERATOR;

  /*!
   \brief Value type for iterator over initial states
   */
  using initial_iterator_value_t = INITIAL_ITERATOR_VALUE;

  /*!
   \brief Type of iterator over outgoing edges
   */
  using outgoing_edges_iterator_t = OUTGOING_EDGES_ITERATOR;

  /*!
   \brief Value type for iterator over outgoing edges
   */
  using outgoing_edges_iterator_value_t = OUTGOING_EDGES_ITERATOR_VALUE;

  /*!
   \brief Destructor
   */
  virtual ~ts_t() = default;

  /*!
   \brief Accessor
   \return Initial state valuations
   */
  virtual tchecker::range_t<INITIAL_ITERATOR> initial_states() = 0;

  /*!
   \brief Initial state and transition
   \param v : initial state valuation
   \post state s and transition t have been initialized from v
   \return (status, s, t) where initial state s and initial transition t have
   been computed from v, and status is the status of state s after initialization
   \note t represents an initial transition to s
   */
  virtual std::tuple<enum tchecker::state_status_t, STATE, TRANSITION> initial(INITIAL_ITERATOR_VALUE const & v) = 0;

  /*!
   \brief Accessor
   \param s : state
   \return outgoing edges from state s
   */
  virtual tchecker::range_t<OUTGOING_EDGES_ITERATOR> outgoing_edges(CONST_STATE const & s) = 0;

  /*!
   \brief Next state and transition
   \param s : state
   \param v : outgoing edge value
   \return (status, s', t) where next state s' and transition t have been
   computed from v, and status is the status of state s'
   */
  virtual std::tuple<enum tchecker::state_status_t, STATE, TRANSITION> next(CONST_STATE const & s,
                                                                            OUTGOING_EDGES_ITERATOR_VALUE const & v) = 0;

  /*!
  \brief Initial states and transitions with status tchecker::STATE_OK
  \param v : container
  \post all pairs (s, t) of initial states and transitions such that s has
  status tchecker::STATE_OK have been pushed back into v
   */
  virtual void initial_ok(std::vector<std::tuple<STATE, TRANSITION>> & v)
  {
    enum tchecker::state_status_t status;
    STATE state;
    TRANSITION transition;
    tchecker::range_t<INITIAL_ITERATOR> states = initial_states();
    for (auto it = states.begin(); !it.at_end(); ++it) {
      std::tie(status, state, transition) = initial(*it);
      if (status == tchecker::STATE_OK)
        v.push_back(std::make_tuple(state, transition));
    }
  }

  /*!
  \brief Next states and transitions with status tchecker::STATE_OK
  \param s : state
  \param v : container
  \post all pairs (s', t) such that s -t-> s' is a transition such that s' has
  status tchecker::STATE_OK have been pushed back into v
  */
  virtual void next_ok(CONST_STATE const & s, std::vector<std::tuple<STATE, TRANSITION>> & v)
  {
    enum tchecker::state_status_t status;
    STATE next_state;
    TRANSITION transition;
    tchecker::range_t<OUTGOING_EDGES_ITERATOR> edges = outgoing_edges(s);
    for (auto it = edges.begin(); !it.at_end(); ++it) {
      std::tie(status, next_state, transition) = next(s, *it);
      if (status == tchecker::STATE_OK)
        v.push_back(std::make_tuple(next_state, transition));
    }
  }
};

} // end of namespace ts

} // end of namespace tchecker

#endif // TCHECKER_TS_HH
