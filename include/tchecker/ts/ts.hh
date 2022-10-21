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
 \brief Transition systems: interfaces and functors
 */

namespace tchecker {

namespace ts {

/*!
 \class ts_impl_t
 \brief Interface of low-level implementation of a transition system
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
class ts_impl_t {
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
  virtual ~ts_impl_t() = default;

  /*!
   \brief Accessor
   \return Initial edges
   */
  virtual initial_range_t initial_edges() = 0;

  /*!
   \brief Initial state and transition
   \param init_edge : initial state valuation
   \param v : container
   \post triples (status, s, t) have been added to v, for each initial state s
   and initial transition t that are initialized from init_edge.
   */
  virtual void initial(initial_value_t const & init_edge, std::vector<sst_t> & v) = 0;

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
   \post triples (status, s', t') have been added to v, for each successor state
   s' and transition t from s to s' along outgoing edge out_edge
   */
  virtual void next(const_state_t const & s, outgoing_edges_value_t const & out_edge, std::vector<sst_t> & v) = 0;

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
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  virtual boost::dynamic_bitset<> labels(const_state_t const & s) const = 0;

  /*!
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true if a run ending in s is a valid run, false otherwise
  */
  virtual bool is_valid_final(const_state_t const & s) const = 0;

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post attributes of state s have been added to map m
   */
  virtual void attributes(const_state_t const & s, std::map<std::string, std::string> & m) const = 0;

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post attributes of transition t have been added to map m
   */
  virtual void attributes(const_transition_t const & t, std::map<std::string, std::string> & m) const = 0;

  /*!
   \brief Share state components
   \param s : a state
   \post internal components in s have been shared
  */
  virtual void share(state_t & s) = 0;

  /*!
   \brief Share transition components
   \param t : a transition
   \post internal components in t have been shared
  */
  virtual void share(transition_t & t) = 0;
};

/*!
  \brief Initial states and transitions with selected status
  \tparam TS_IMPL : type of transition system implementation, should implement
  tchecker::ts::ts_impl_t
  \param ts_impl : a transition system implementation
  \param v : container
  \param mask : mask on initial states
  \post all tuples (status, s, t) of status, initial states and transitions such
  that status matches mask (i.e. status & mask != 0) have been pushed back into v
  */
template <class TS_IMPL>
void initial(TS_IMPL & ts_impl, std::vector<typename TS_IMPL::sst_t> & v, tchecker::state_status_t mask)
{
  std::vector<typename TS_IMPL::sst_t> sst;
  typename TS_IMPL::initial_range_t init_edges = ts_impl.initial_edges();
  for (typename TS_IMPL::initial_value_t && init_edge : init_edges) {
    ts_impl.initial(init_edge, sst);
    for (auto && [status, s, t] : sst) {
      if (status & mask)
        v.push_back(std::make_tuple(status, s, t));
    }
    sst.clear();
  }
}

/*!
  \brief Next states and transitions with selected status
  \tparam TS_IMPL : type of  transition system implementation, should implement
  tchecker::ts::ts_impl_t
  \param ts_impl : a transition system implementation
  \param s : state
  \param v : container
  \param mask : mask on next states
  \post all tuples (status, s', t) such that s -t-> s' is a transition and the
  status of s' matches mask (i.e. status & mask != 0) have been pushed to v
  */
template <class TS_IMPL>
void next(TS_IMPL & ts_impl, typename TS_IMPL::const_state_t const & s, std::vector<typename TS_IMPL::sst_t> & v,
          tchecker::state_status_t mask)
{
  std::vector<typename TS_IMPL::sst_t> sst;
  typename TS_IMPL::outgoing_edges_range_t out_edges = ts_impl.outgoing_edges(s);
  for (typename TS_IMPL::outgoing_edges_value_t && out_edge : out_edges) {
    ts_impl.next(s, out_edge, sst);
    for (auto && [status, next_s, next_t] : sst) {
      if (status & mask)
        v.push_back(std::make_tuple(status, next_s, next_t));
    }
    sst.clear();
  }
}

/*!
 \class ts_t
 \brief Transition system interface
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
  using sst_t = std::tuple<tchecker::state_status_t, state_t, transition_t>;

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

  /*!
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  virtual boost::dynamic_bitset<> labels(const_state_t const & s) const = 0;

  /*!
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true if a run ending in s is a valid run, false otherwise
  */
  virtual bool is_valid_final(const_state_t const & s) const = 0;

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post attributes of state s have been added to map m
   */
  virtual void attributes(const_state_t const & s, std::map<std::string, std::string> & m) const = 0;

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post attributes of transition t have been added to map m
   */
  virtual void attributes(const_transition_t const & t, std::map<std::string, std::string> & m) const = 0;
};

/*!
 \class make_ts_from_impl_t
 \brief Functor that make a transition system from an implementation
 \tparam TS_IMPL : transition system implementation, should implement tchecker::ts::ts_impl_t
*/
template <class TS_IMPL>
class make_ts_from_impl_t : public ts_t<typename TS_IMPL::state_t, typename TS_IMPL::const_state_t,
                                        typename TS_IMPL::transition_t, typename TS_IMPL::const_transition_t> {
public:
  // Inherited tyoes
  using ts_t = ts_t<typename TS_IMPL::state_t, typename TS_IMPL::const_state_t, typename TS_IMPL::transition_t,
                    typename TS_IMPL::const_transition_t>;
  using const_state_t = typename ts_t::const_state_t;
  using const_transition_t = typename ts_t::const_transition_t;
  using sst_t = typename ts_t::sst_t;
  using state_t = typename ts_t::state_t;
  using transition_t = typename ts_t::transition_t;

  /*!
   \brief Constructor
   \param args : arguments to a constructor of TS_IMPL
  */
  template <class... ARGS> make_ts_from_impl_t(ARGS &&... args) : _ts_impl(args...) {}

  /*!
   \brief Copy constructor (deleted)
  */
  make_ts_from_impl_t(tchecker::ts::make_ts_from_impl_t<TS_IMPL> const &) = delete;

  /*!
   \brief Move constructor (deleted)
  */
  make_ts_from_impl_t(tchecker::ts::make_ts_from_impl_t<TS_IMPL> &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~make_ts_from_impl_t() = default;

  /*!
   \brief Assignment operator (deleted)
  */
  tchecker::ts::make_ts_from_impl_t<TS_IMPL> & operator=(tchecker::ts::make_ts_from_impl_t<TS_IMPL> const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
  */
  tchecker::ts::make_ts_from_impl_t<TS_IMPL> & operator=(tchecker::ts::make_ts_from_impl_t<TS_IMPL> &&) = delete;

  /*!
  \brief Initial states and transitions with selected status
  \param v : container
  \param mask : mask on initial states
  \post all tuples (status, s, t) of status, initial states and transitions such
  that status matches mask (i.e. status & mask != 0) have been pushed back into v
   */
  inline virtual void initial(std::vector<sst_t> & v, tchecker::state_status_t mask = tchecker::STATE_OK)
  {
    tchecker::ts::initial(_ts_impl, v, mask);
  }

  /*!
  \brief Next states and transitions with selected status
  \param s : state
  \param v : container
  \param mask : mask on next states
  \post all tuples (status, s', t) such that s -t-> s' is a transition and the
  status of s' matches mask (i.e. status & mask != 0) have been pushed to v
  */
  inline virtual void next(const_state_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask = tchecker::STATE_OK)
  {
    tchecker::ts::next(_ts_impl, s, v, mask);
  }

  /*!
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  inline virtual boost::dynamic_bitset<> labels(const_state_t const & s) const { return _ts_impl.labels(s); }

  /*!
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true if a run ending in s is a valid run, false otherwise
  */
  inline virtual bool is_valid_final(const_state_t const & s) const { return _ts_impl.is_valid_final(s); }

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post attributes of state s have been added to map m
   */
  inline virtual void attributes(const_state_t const & s, std::map<std::string, std::string> & m) const
  {
    _ts_impl.attributes(s, m);
  }

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post attributes of transition t have been added to map m
   */
  inline virtual void attributes(const_transition_t const & t, std::map<std::string, std::string> & m) const
  {
    _ts_impl.attributes(t, m);
  }

protected:
  /*!
   \brief Accessor
   \return Underlying transition system implementation
  */
  TS_IMPL const & ts_impl() const { return _ts_impl; }

private:
  TS_IMPL _ts_impl; /*!< Transition system implementation */
};

/*!
 \class make_sharing_ts_from_impl_t
 \brief Functor that make a sharing transition system from an implementation
 \tparam TS_IMPL : transition system implementation, should implement tchecker::ts::ts_impl_t
*/
template <class TS_IMPL>
class make_sharing_ts_from_impl_t : public ts_t<typename TS_IMPL::const_state_t, typename TS_IMPL::const_state_t,
                                                typename TS_IMPL::const_transition_t, typename TS_IMPL::const_transition_t> {
public:
  // Inherited types
  using ts_t = ts_t<typename TS_IMPL::const_state_t, typename TS_IMPL::const_state_t, typename TS_IMPL::const_transition_t,
                    typename TS_IMPL::const_transition_t>;
  using const_state_t = typename ts_t::const_state_t;
  using const_transition_t = typename ts_t::const_transition_t;
  using sst_t = typename ts_t::sst_t;
  using state_t = typename ts_t::state_t;
  using transition_t = typename ts_t::transition_t;

  /*!
   \brief Constructor
   \param args : arguments to a constructor of TS_IMPL
  */
  template <class... ARGS> make_sharing_ts_from_impl_t(ARGS &&... args) : _ts_impl(args...) {}

  /*!
   \brief Copy constructor (deleted)
  */
  make_sharing_ts_from_impl_t(tchecker::ts::make_sharing_ts_from_impl_t<TS_IMPL> const &) = delete;

  /*!
   \brief Move constructor (deleted)
  */
  make_sharing_ts_from_impl_t(tchecker::ts::make_sharing_ts_from_impl_t<TS_IMPL> &&) = delete;

  /*!
   \brief Destructor
   */
  virtual ~make_sharing_ts_from_impl_t() = default;

  /*!
   \brief Assignment operator (deleted)
  */
  tchecker::ts::make_sharing_ts_from_impl_t<TS_IMPL> &
  operator=(tchecker::ts::make_sharing_ts_from_impl_t<TS_IMPL> const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
  */
  tchecker::ts::make_sharing_ts_from_impl_t<TS_IMPL> &
  operator=(tchecker::ts::make_sharing_ts_from_impl_t<TS_IMPL> &&) = delete;

  /*!
  \brief Initial states and transitions with selected status
  \param v : container
  \param mask : mask on initial states
  \post all tuples (status, s, t) of status, initial states and transitions such
  that status matches mask (i.e. status & mask != 0) have been pushed back into v
   */
  inline virtual void initial(std::vector<sst_t> & v, tchecker::state_status_t mask = tchecker::STATE_OK)
  {
    std::vector<typename TS_IMPL::sst_t> vv;
    tchecker::ts::initial(_ts_impl, vv, mask);
    share(vv, v);
    vv.clear();
  }

  /*!
  \brief Next states and transitions with selected status
  \param s : state
  \param v : container
  \param mask : mask on next states
  \post all tuples (status, s', t) such that s -t-> s' is a transition and the
  status of s' matches mask (i.e. status & mask != 0) have been pushed to v
  */
  inline virtual void next(const_state_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask = tchecker::STATE_OK)
  {
    std::vector<typename TS_IMPL::sst_t> vv;
    tchecker::ts::next(_ts_impl, s, vv, mask);
    share(vv, v);
    vv.clear();
  }

  /*!
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  inline virtual boost::dynamic_bitset<> labels(const_state_t const & s) const { return _ts_impl.labels(s); }

  /*!
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true if a run ending in s is a valid run, false otherwise
  */
  inline virtual bool is_valid_final(const_state_t const & s) const { return _ts_impl.is_valid_final(s); }

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post attributes of state s have been added to map m
   */
  inline virtual void attributes(const_state_t const & s, std::map<std::string, std::string> & m) const
  {
    _ts_impl.attributes(s, m);
  }

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post attributes of transition t have been added to map m
   */
  inline virtual void attributes(const_transition_t const & t, std::map<std::string, std::string> & m) const
  {
    _ts_impl.attributes(t, m);
  }

protected:
  /*!
   \brief Accessor
   \return Underlying transition system implementation
  */
  TS_IMPL const & ts_impl() const { return _ts_impl; }

private:
  /*!
  \brief Share state and transition components
  \param v : container of implementation sst
  \param vv : a container of sst
  \post states and transitions in vv may share internal components with other
  states and transitions
  */
  void share(std::vector<typename TS_IMPL::sst_t> & v, std::vector<sst_t> & vv)
  {
    for (auto && [status, s, t] : v) {
      _ts_impl.share(s);
      _ts_impl.share(t);
      vv.push_back(std::make_tuple(status, static_cast<const_state_t>(s), static_cast<const_transition_t>(t)));
    }
  }

  TS_IMPL _ts_impl; /*!< Transition system implementation */
};

} // end of namespace ts

} // end of namespace tchecker

#endif // TCHECKER_TS_HH
