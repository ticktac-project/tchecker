/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_HH
#define TCHECKER_TS_HH

#include <map>
#include <tuple>
#include <type_traits>
#include <vector>

#include <boost/dynamic_bitset.hpp>

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
\tparam CONST_TRANSITION : type of const transition
*/
template <class STATE, class CONST_STATE, class TRANSITION, class CONST_TRANSITION> class ts_t {
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
  using sst_t = std::tuple<tchecker::state_status_t, STATE, TRANSITION>;

  /*!
   \brief Destructor
   */
  virtual ~ts_t() = default;

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
  inline STATE state(sst_t const & sst) const { return std::get<1>(sst); }

  /*!
   \brief Accessor
   \param sst : a tuple (status, state, transition)
   \return the transition in sst
  */
  inline TRANSITION transition(sst_t const & sst) const { return std::get<2>(sst); }

  /*!
  \brief Initial states and transitions
  \param v : container
  \post all tuples (status, s, t) of status, initial states and transitions have
  been pushed back into v
   */
  virtual void initial(std::vector<sst_t> & v) = 0;

  /*!
  \brief Next states and transitions
  \param s : state
  \param v : container
  \post all tuples (status, s', t) such that s -t-> s' is a transition have been
  pushed to v
  */
  virtual void next(CONST_STATE const & s, std::vector<sst_t> & v) = 0;

  /*!
  \brief Checks if a state satisfies a set of labels
  \param s : a state
  \param labels : a set of labels
  \return true if labels is not empty and labels is included in the set of
  labels of state s, false otherwise
   */
  virtual bool satisfies(CONST_STATE const & s, boost::dynamic_bitset<> const & labels) const = 0;

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post attributes of state s have been added to map m
   */
  virtual void attributes(CONST_STATE const & s, std::map<std::string, std::string> & m) const = 0;

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post attributes of transition t have been added to map m
   */
  virtual void attributes(CONST_TRANSITION const & t, std::map<std::string, std::string> & m) const = 0;
};

/*!
 \class full_ts_t
 \brief Transition system with full access to initial/outgoing edges
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
class full_ts_t : public tchecker::ts::ts_t<STATE, CONST_STATE, TRANSITION, CONST_TRANSITION> {
public:
  /*!
   \brief Type of state
   */
  using state_t = typename tchecker::ts::ts_t<STATE, CONST_STATE, TRANSITION, CONST_TRANSITION>::state_t;

  /*!
  \brief Type of const state
  */
  using const_state_t = typename tchecker::ts::ts_t<STATE, CONST_STATE, TRANSITION, CONST_TRANSITION>::const_state_t;

  /*!
   \brief Type of transition
   */
  using transition_t = typename tchecker::ts::ts_t<STATE, CONST_STATE, TRANSITION, CONST_TRANSITION>::transition_t;

  /*!
   \brief Type of const transition
  */
  using const_transition_t = typename tchecker::ts::ts_t<STATE, CONST_STATE, TRANSITION, CONST_TRANSITION>::const_transition_t;

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
  using sst_t = typename tchecker::ts::ts_t<STATE, CONST_STATE, TRANSITION, CONST_TRANSITION>::sst_t;

  /*!
   \brief Destructor
   */
  virtual ~full_ts_t() = default;

  /*!
   \brief Accessor
   \return Initial edges
   */
  virtual INITIAL_RANGE initial_edges() = 0;

  /*!
   \brief Initial state and transition
   \param init_edge : initial state valuation
   \param v : container
   \post triples (status, s, t) have been added to v, for each initial state s
   and initial transition t that are initialized from init_edge.
   */
  virtual void initial(INITIAL_VALUE const & init_edge, std::vector<sst_t> & v) = 0;

  /*!
   \brief Accessor
   \param s : state
   \return outgoing edges from state s
   */
  virtual OUTGOING_EDGES_RANGE outgoing_edges(CONST_STATE const & s) = 0;

  /*!
   \brief Next state and transition
   \param s : state
   \param out_edge : outgoing edge value
   \param v : container
   \post triples (status, s', t') have been added to v, for each successor state
   s' and transition t from s to s' along outgoing edge out_edge
   */
  virtual void next(CONST_STATE const & s, OUTGOING_EDGES_VALUE const & out_edge, std::vector<sst_t> & v) = 0;

  /*!
  \brief Initial states and transitions with selected status
  \param v : container
  \param mask : mask on initial states
  \post all tuples (status, s, t) of status, initial states and transitions such
  that status matches mask (i.e. status & mask != 0) have been pushed back into v
   */
  void initial(std::vector<sst_t> & v, tchecker::state_status_t mask)
  {
    std::vector<sst_t> sst;
    INITIAL_RANGE init_edges = initial_edges();
    for (INITIAL_VALUE && init_edge : init_edges) {
      initial(init_edge, sst);
      for (auto && [status, s, t] : sst) {
        if (status & mask)
          v.push_back(std::make_tuple(status, s, t));
      }
      sst.clear();
    }
  }

  /*!
  \brief Next states and transitions with selected status
  \param s : state
  \param v : container
  \param mask : mask on next states
  \post all tuples (status, s', t) such that s -t-> s' is a transition and the
  status of s' matches mask (i.e. status & mask != 0) have been pushed to v
  */
  void next(CONST_STATE const & s, std::vector<sst_t> & v, tchecker::state_status_t mask)
  {
    std::vector<sst_t> sst;
    OUTGOING_EDGES_RANGE out_edges = outgoing_edges(s);
    for (OUTGOING_EDGES_VALUE && out_edge : out_edges) {
      next(s, out_edge, sst);
      for (auto && [status, next_s, next_t] : sst) {
        if (status & mask)
          v.push_back(std::make_tuple(status, next_s, next_t));
      }
      sst.clear();
    }
  }

  /*!
  \brief Initial states and transitions
  \param v : container
  \post all tuples (status, s, t) of status, initial states and transitions such
  that s has status tchecker::STATE_OK have been pushed back into v
   */
  virtual void initial(std::vector<sst_t> & v) { initial(v, tchecker::STATE_OK); }

  /*!
  \brief Next states and transitions
  \param s : state
  \param v : container
  \post all tuples (status, s', t) such that s -t-> s' is a transition and s'
  has status tchecker::STATE_OK have been pushed to v
  */
  virtual void next(CONST_STATE const & s, std::vector<sst_t> & v) { next(s, v, tchecker::STATE_OK); }

  using tchecker::ts::ts_t<STATE, CONST_STATE, TRANSITION, CONST_TRANSITION>::status;
  using tchecker::ts::ts_t<STATE, CONST_STATE, TRANSITION, CONST_TRANSITION>::state;
  using tchecker::ts::ts_t<STATE, CONST_STATE, TRANSITION, CONST_TRANSITION>::transition;
  using tchecker::ts::ts_t<STATE, CONST_STATE, TRANSITION, CONST_TRANSITION>::satisfies;
  using tchecker::ts::ts_t<STATE, CONST_STATE, TRANSITION, CONST_TRANSITION>::attributes;
};

} // end of namespace ts

} // end of namespace tchecker

#endif // TCHECKER_TS_HH
