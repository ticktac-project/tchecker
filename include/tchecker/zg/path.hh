/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_PATH_HH
#define TCHECKER_ZG_PATH_HH

#include <map>
#include <memory>
#include <string>

#include "tchecker/graph/node.hh"
#include "tchecker/ts/path.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/zg/state.hh"
#include "tchecker/zg/transition.hh"
#include "tchecker/zg/zg.hh"

/*!
 \file path.hh
 \brief Graph representation of paths in zone graphs
*/

namespace tchecker {

namespace zg {

namespace path {

namespace symbolic {

/*!
 \class node_t
 \brief Type of node on a path in a zone graph
*/
class node_t : public tchecker::graph::node_flags_t, public tchecker::graph::node_zg_state_t {
public:
  /*!
  \brief Constructor
  \param s : a zone graph state
  \param initial : initial node flag
  \param final : final node flag
  \post this node keeps a shared pointer to s, and has initial/final node flags as specified
  */
  node_t(tchecker::zg::state_sptr_t const & s, bool initial = false, bool final = false);

  /*!
   \brief Constructor
   \param s : a zone graph state
   \param initial : initial node flag
   \param final : final node flag
   \post this node keeps a shared pointer to s, and has initial/final node flags as specified
   */
  node_t(tchecker::zg::const_state_sptr_t const & s, bool initial = false, bool final = false);
};

/*!
 \brief Lexical ordering on path nodes
 \param n1 : node
 \param n2 : node
 \return < 0 if n1 is less than n2, 0 if n1 is equal to n2, > 0 otherwise
*/
int lexical_cmp(tchecker::zg::path::symbolic::node_t const & n1, tchecker::zg::path::symbolic::node_t const & n2);

/*!
 \class edge_t
 \brief Type of edge on a path in a zone graph
*/
class edge_t {
public:
  /*!
   \brief Constructor
   \post this keeps a pointer on t
  */
  edge_t(tchecker::zg::const_transition_sptr_t const & t);

  /*!
   \brief Constructor
   \post this keeps a pointer on t
  */
  edge_t(tchecker::zg::transition_sptr_t const & t);

  /*!
   \brief Copy constructor
  */
  edge_t(tchecker::zg::path::symbolic::edge_t const & e);

  /*!
   \brief Move constructor
  */
  edge_t(tchecker::zg::path::symbolic::edge_t && e);

  /*!
   \brief Destructor
  */
  ~edge_t() = default;

  /*!
   \brief Assignment operator
   \post this point to the same transition as e
  */
  tchecker::zg::path::symbolic::edge_t & operator=(tchecker::zg::path::symbolic::edge_t const & e);

  /*!
   \brief Move assignment operator
  */
  tchecker::zg::path::symbolic::edge_t & operator=(tchecker::zg::path::symbolic::edge_t && e);

  /*!
   \brief Accessor
   \return a pointer to the transition in this edge
  */
  inline tchecker::zg::const_transition_sptr_t const & transition_ptr() const { return _t; }

  /*!
   \brief Accessor
   \return the transition in this edge
  */
  inline tchecker::zg::transition_t const & transition() const { return *_t; }

private:
  tchecker::zg::const_transition_sptr_t _t; /*!< Zone graph transition */
};

/*!
 \brief Lexical ordering on path edges
 \param e1 : edge
 \param e2 : edge
 \return < 0 if e1 is less than e2, 0 if e1 is equal to e2, > 0 otherwise
*/
int lexical_cmp(tchecker::zg::path::symbolic::edge_t const & e1, tchecker::zg::path::symbolic::edge_t const & e2);

/*!
 \class finite_path_t
 \brief Finite path in a zone graph
*/
class finite_path_t : public tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::symbolic::node_t,
                                                         tchecker::zg::path::symbolic::edge_t> {
public:
  /*!
   \brief Constructor
   \param zg : a zone graph
   \post this is an empty path
   \note this keeps a pointer to zg
   \note all nodes and edges added to this path shall be built from states and transitions in zg
  */
  using tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::symbolic::node_t,
                                    tchecker::zg::path::symbolic::edge_t>::finite_path_t;

  using tchecker::graph::finite_path_t<tchecker::zg::path::symbolic::node_t, tchecker::zg::path::symbolic::edge_t>::attributes;

  /*!
   \brief Accessor
   \return Underlying zone graph
  */
  inline tchecker::zg::zg_t & zg() const
  {
    return tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::symbolic::node_t,
                                       tchecker::zg::path::symbolic::edge_t>::ts();
  }

  /*!
   \brief Accessor
   \return Pointer to nderlying zone graph
  */
  inline std::shared_ptr<tchecker::zg::zg_t> zg_ptr() const
  {
    return tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::symbolic::node_t,
                                       tchecker::zg::path::symbolic::edge_t>::ts_ptr();
  }

protected:
  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(tchecker::zg::path::symbolic::node_t const & n, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(tchecker::zg::path::symbolic::edge_t const & e, std::map<std::string, std::string> & m) const;
};

/* output */

/*!
 \brief Dot output of zone graph finite paths
 \param os : output stream
 \param path : a path
 \param name : path name
 \post path has been output to os following the graphviz DOT file format
 \return os after output
*/
std::ostream & dot_output(std::ostream & os, tchecker::zg::path::symbolic::finite_path_t const & path,
                          std::string const & name);

/* path computation */

/*!
 \brief Compute a finite symbolic run in a zone graph following a specified sequence of tuples of edges
 \param zg : zone graph
 \param initial_vloc : tuple of initial locations
 \param seq : sequence of tuple of edges as a range
 \param last_node_final : final flag for last node
 \pre initial_vloc is a tuple of initial locations in zg
 \pre seq is a feasible sequence in zg from initial_vloc
 \return a finite path in zg, starting from the initial state with tuple of locations initial_vloc,
 and that follows the sequence seq if possible. If the path is not empty, its first node has flag initial set to true,
 and its last node has flag final set to last_node_final
 \throw std::invalid_argument : if there is not initial state in zg with tuple of locations initial_vloc
 or if seq is not feasible from the initial state
 \note the returned path keeps a shared pointer on zg
 */
tchecker::zg::path::symbolic::finite_path_t * compute_finite_path(std::shared_ptr<tchecker::zg::zg_t> const & zg,
                                                                  tchecker::vloc_t const & initial_vloc,
                                                                  std::vector<tchecker::const_vedge_sptr_t> const & seq,
                                                                  bool last_node_final = false);

/*!
 \class lasso_path_t
 \brief Lasso path in a zone graph
*/
class lasso_path_t : public tchecker::ts::lasso_path_t<tchecker::zg::zg_t, tchecker::zg::path::symbolic::node_t,
                                                       tchecker::zg::path::symbolic::edge_t> {
public:
  /*!
   \brief Constructor
   \param zg : a zone graph
   \post this is an empty lasso path
   \note this keeps a pointer to zg
   \note all nodes and edges added to this lasso path shall be built from states and transitions in zg
  */
  using tchecker::ts::lasso_path_t<tchecker::zg::zg_t, tchecker::zg::path::symbolic::node_t,
                                   tchecker::zg::path::symbolic::edge_t>::lasso_path_t;

  using tchecker::graph::lasso_path_t<tchecker::zg::path::symbolic::node_t, tchecker::zg::path::symbolic::edge_t>::attributes;

  /*!
   \brief Accessor
   \return Underlying zone graph
  */
  inline tchecker::zg::zg_t & zg() const
  {
    return tchecker::ts::lasso_path_t<tchecker::zg::zg_t, tchecker::zg::path::symbolic::node_t,
                                      tchecker::zg::path::symbolic::edge_t>::ts();
  }

  /*!
   \brief Accessor
   \return Pointer to underlying zone graph
  */
  inline std::shared_ptr<tchecker::zg::zg_t> zg_ptr() const
  {
    return tchecker::ts::lasso_path_t<tchecker::zg::zg_t, tchecker::zg::path::symbolic::node_t,
                                      tchecker::zg::path::symbolic::edge_t>::ts_ptr();
  }

protected:
  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(tchecker::zg::path::symbolic::node_t const & n, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(tchecker::zg::path::symbolic::edge_t const & e, std::map<std::string, std::string> & m) const;
};

/* output */

/*!
 \brief Dot output of zone graph lasso paths
 \param os : output stream
 \param path : a lasso path
 \param name : path name
 \post path has been output to os following the graphviz DOT file format
 \return os after output
*/
std::ostream & dot_output(std::ostream & os, tchecker::zg::path::symbolic::lasso_path_t const & path, std::string const & name);

/* path computation */

/*!
 \brief Compute a symbolic lasso path in a zone graph following a specified sequence of tuples of edges
 \param zg : zone graph
 \param initial_vloc : tuple of initial locations
 \param prefix : sequence of tuple of edges, prefix of the lasso path
 \param cycle : sequence of tuple of edges, cycle of the lasso path
 \param final_state : predicate that identifies final states
 \pre initial_vloc is a tuple of initial locations in zg
 \pre prefix is a feasible sequence in zg from initial_vloc
 \pre cycle is not empty
 \pre cycle is a feasible sequence in zg after prefix from initial_vloc
 \pre the state reached after prefix+cycle is the same as the state reached after prefix
 \return a lasso path in zg, starting from the initial state with tuple of locations initial_vloc,
 and that follows the sequence prefix+cycle if possible.
 If the path is not empty, its first node has flag initial set to true, and its nodes that match final_state
 have flag final set to true
 \throw std::invalid_argument : if cycle is empty
 \throw std::invalid_argument : if there is not initial state in zg with tuple of locations initial_vloc
 or if prefix+cycle is not feasible from the initial state
 \note the returned path keeps a shared pointer on zg
 */
tchecker::zg::path::symbolic::lasso_path_t *
compute_lasso_path(std::shared_ptr<tchecker::zg::zg_t> const & zg, tchecker::vloc_t const & initial_vloc,
                   std::vector<tchecker::const_vedge_sptr_t> const & prefix,
                   std::vector<tchecker::const_vedge_sptr_t> const & cycle,
                   std::function<bool(tchecker::zg::state_t const &)> && final_state);

} // namespace symbolic

namespace concrete {

/*!
\class node_t
\brief Type of node on a concrete path in a zone graph
*/
class node_t : public tchecker::zg::path::symbolic::node_t {
public:
  /*!
  \brief Constructor
  \param s : a zone graph state
  \param clockval : a clock valuation
  \param initial : initial node flag
  \param final : final node flag
  \post this node keeps a shared pointer to s, a shared pointer to clockval, as well as
  initial/final node flags as specified
  \throw std::invalid_argument : if the size of clockval is different from the dimension of the
  zone in s, or if clockval does not belong to the zone in s
  */
  node_t(tchecker::zg::state_sptr_t const & s, tchecker::clockval_sptr_t const & clockval, bool initial = false,
         bool final = false);

  /*!
   \brief Constructor
   \param s : a zone graph state
   \param clockval : clock valuation
   \param initial : initial node flag
   \param final : final node flag
   \post this node keeps a shared pointer to s, a shared pointer to clockval, as
   well as initial/final node flags as specified
   \throw std::invalid_argument : if the size of clockval is different from the dimension of the
   zone in s, or if clockval does not belong to the zone in s
   */
  node_t(tchecker::zg::const_state_sptr_t const & s, tchecker::const_clockval_sptr_t const & clockval, bool initial = false,
         bool final = false);

  /*!
  \brief Constructor
  \param t : tuple of <zone graph state, clock valuation, initial node flag, final node flag>
  \post this node as been initialized from t (see other constructors)
  \throw std::invalid_argument : if the size of the clock valuation is different from the dimension
  of the zone in s, or if clock valuation does not belong to the zone in s
  */
  node_t(std::tuple<tchecker::zg::state_sptr_t, tchecker::clockval_sptr_t, bool, bool> const & t);

  /*!
  \brief Constructor
  \param t : tuple of <zone graph state, clock valuation, initial node flag, final node flag>
  \post this node as been initialized from t (see other constructors)
  \throw std::invalid_argument : if the size of the clock valuation is different from the dimension
  of the zone in s, or if clock valuation does not belong to the zone in s
  */
  node_t(std::tuple<tchecker::zg::const_state_sptr_t, tchecker::const_clockval_sptr_t, bool, bool> const & t);

  /*!
   \brief Accessor
   \return the clock valuation in this node
  */
  inline tchecker::clockval_t const & clockval() const { return *_clockval; }

  /*!
   \brief Accessor
   \return pointer to clock valuation in this node
  */
  inline tchecker::const_clockval_sptr_t const & clockval_ptr() const { return _clockval; }

private:
  tchecker::const_clockval_sptr_t _clockval; /*!< Clock valuation */
};

/*!
 \brief Lexical ordering on concrete path nodes
 \param n1 : node
 \param n2 : node
 \return < 0 if n1 is less than n2, 0 if n1 is equal to n2, > 0 otherwise
*/
int lexical_cmp(tchecker::zg::path::concrete::node_t const & n1, tchecker::zg::path::concrete::node_t const & n2);

/*!
 \class edge_t
 \brief Type of edge on a path in a zone graph
*/
class edge_t : public tchecker::zg::path::symbolic::edge_t {
public:
  /*!
   \brief Constructor
   \param t : a zone graph transition
   \param delay : a delay
   \post this keeps a pointer on t
   \throw std::invalid_argument : if delay < 0
  */
  edge_t(tchecker::zg::const_transition_sptr_t const & t, tchecker::clock_rational_value_t delay);

  /*!
   \brief Constructor
   \param t : a zone graph transition
   \param delay : a delay
   \post this keeps a pointer on t
   \throw std::invalid_argument : if delay < 0
  */
  edge_t(tchecker::zg::transition_sptr_t const & t, tchecker::clock_rational_value_t delay);

  /*!
   \brief Constructor
   \param t : a tuple <transition, delay>
   \post this has been initialized from t (see other constructors)
   \throw std::invalid_argument : if delay < 0
  */
  edge_t(std::tuple<tchecker::zg::const_transition_sptr_t, tchecker::clock_rational_value_t> const & t);

  /*!
   \brief Constructor
   \param t : a tuple <transition, delay>
   \post this has been initialized from t (see other constructors)
   \throw std::invalid_argument : if delay < 0
  */
  edge_t(std::tuple<tchecker::zg::transition_sptr_t, tchecker::clock_rational_value_t> const & t);

  /*!
   \brief Copy constructor
  */
  edge_t(tchecker::zg::path::concrete::edge_t const & e) = default;

  /*!
   \brief Move constructor
  */
  edge_t(tchecker::zg::path::concrete::edge_t && e) = default;

  /*!
   \brief Destructor
  */
  ~edge_t() = default;

  /*!
   \brief Assignment operator
   \post this point to the same transition as e
  */
  tchecker::zg::path::concrete::edge_t & operator=(tchecker::zg::path::concrete::edge_t const & e) = default;

  /*!
   \brief Move assignment operator
  */
  tchecker::zg::path::concrete::edge_t & operator=(tchecker::zg::path::concrete::edge_t && e) = default;

  using tchecker::zg::path::symbolic::edge_t::transition;
  using tchecker::zg::path::symbolic::edge_t::transition_ptr;

  /*!
   \brief Accessor
   \return delay in this transition
  */
  tchecker::clock_rational_value_t delay() const { return _delay; }

private:
  tchecker::clock_rational_value_t _delay; /*!< delay on this transition */
};

/*!
 \brief Lexical ordering on concrete path edges
 \param e1 : edge
 \param e2 : edge
 \return < 0 if e1 is less than e2, 0 if e1 is equal to e2, > 0 otherwise
*/
int lexical_cmp(tchecker::zg::path::concrete::edge_t const & e1, tchecker::zg::path::concrete::edge_t const & e2);

/*!
 \class finite_path_t
 \brief Concrete finite path in a zone graph
*/
class finite_path_t : public tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::concrete::node_t,
                                                         tchecker::zg::path::concrete::edge_t> {
public:
  /*!
   \brief Constructor
   \param zg : a zone graph
   \param clockval_alloc_nb : number of clock valuations allocated in one block
   \post this is an empty path
   \note this keeps a pointer to zg
   \note all nodes and edges added to this path shall be built from states and transitions in zg
  */
  finite_path_t(std::shared_ptr<tchecker::zg::zg_t> const & zg, std::size_t clockval_alloc_nb);

  /*!
   \brief Copy constructor
  */
  finite_path_t(tchecker::zg::path::concrete::finite_path_t const &) = delete;

  /*!
   \brief Move constructor
  */
  finite_path_t(tchecker::zg::path::concrete::finite_path_t &&) = delete;

  /*!
   \brief Destructor
  */
  ~finite_path_t();

  /*!
   \brief Assignment operator
  */
  tchecker::zg::path::concrete::finite_path_t & operator=(tchecker::zg::path::concrete::finite_path_t const &) = delete;

  /*!
   \brief Move-assignment operator
  */
  tchecker::zg::path::concrete::finite_path_t & operator=(tchecker::zg::path::concrete::finite_path_t &&) = delete;

  /*!
   \brief Add first node to the path
   \param s : a zone graph state
   \param clockval : clock valuation
   \param initial : initial node flag
   \param final : final node flag
   \post this node keeps a shared pointer to s, a copy of clockval, as
   well as initial/final node flags as specified
   \throw std::runtime_error : if this path is not empty
   \throw see tchecker::zg::path::concrete::node_t
  */
  void add_first_node(tchecker::zg::const_state_sptr_t const & s, tchecker::clockval_t const & clockval, bool initial = false,
                      bool final = false);

  /*!
   \brief Extend path at end
   \param t : a zone graph transition
   \param delay : delay taken before the transition
   \param s : a zone graph target state
   \param clockval : target clock valuation
   \param initial : initial target node flag
   \param final : final target node flag
   \pre this path is not empty
   \post this path has been extended into first -> ... -> last -e-> n where e has been built from t, delay and
   n has been built from s, clockval, initial, final
   \throw std::runtime_error : if this path is empty
   \throw see tchecker::zg::path::concrete::node_t
   \throw see tchecker::zg::path::concrete::edge_t
   */
  void extend_back(tchecker::zg::const_transition_sptr_t const & t, tchecker::clock_rational_value_t delay,
                   tchecker::zg::const_state_sptr_t const & s, tchecker::clockval_t const & clockval, bool initial = false,
                   bool final = false);

  /*!
   \brief Extend path at front
   \param t : a zone graph transition
   \param delay : delay taken before the transition
   \param s : a zone graph target state
   \param clockval : target clock valuation
   \param initial : initial target node flag
   \param final : final target node flag
   \pre this path is not empty
   \post this path has been extended into n -e-> first -> ... -> last where e has been built from t, delay and
   n has been built from s, clockval, initial, final
   \throw std::runtime_error : if this path is empty
   \throw see tchecker::zg::path::concrete::node_t
   \throw see tchecker::zg::path::concrete::edge_t
   */
  void extend_front(tchecker::zg::const_transition_sptr_t const & t, tchecker::clock_rational_value_t delay,
                    tchecker::zg::const_state_sptr_t const & s, tchecker::clockval_t const & clockval, bool initial = false,
                    bool final = false);

  using tchecker::graph::finite_path_t<tchecker::zg::path::concrete::node_t, tchecker::zg::path::concrete::edge_t>::attributes;

protected:
  /*!
   \brief Accessor to concrete node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(tchecker::zg::path::concrete::node_t const & n, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to concrete edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(tchecker::zg::path::concrete::edge_t const & e, std::map<std::string, std::string> & m) const;

private:
  tchecker::pool_t<tchecker::shared_clockval_t> _clockval_pool; /*!< Pool allocator of clock valuations */
};

/* output */

/*!
 \brief Dot output of zone graph finite concrete paths
 \param os : output stream
 \param path : a concrete path
 \param name : path name
 \post path has been output to os following the graphviz DOT file format
 \return os after output
*/
std::ostream & dot_output(std::ostream & os, tchecker::zg::path::concrete::finite_path_t const & path,
                          std::string const & name);

/* concrete path computation */

/*!
 \brief Compute a finite concrete run in a zone graph from a finite symnbolic run
 \param symbolic_run : finite symbolic run
 \return a finite concrete run in zg, following the sequence of states and transitions in symbolic_run,
 where each clock valuation belongs to the zone in the symbolic path, each successive clock valuations
 are reachable from each others. Delays in transitions correspond to the delay between the clock valuation
 in the source state, and the clock valuation in the target state.
 The initial and final flags on nodes in the returned concrete path are set accordingly to symbolic_run
 \note the returned path shares pointers to states, transitions and zone graph with symbolic_run
 \throw std::runtime_error : if concretization of symbolic_run fails
 */
tchecker::zg::path::concrete::finite_path_t *
compute_finite_path(tchecker::zg::path::symbolic::finite_path_t const & symbolic_run);

} // namespace concrete

} // namespace path

} // namespace zg

} // namespace tchecker

#endif // TCHECKER_ZG_PATH_HH