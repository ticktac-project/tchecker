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
int lexical_cmp(tchecker::zg::path::node_t const & n1, tchecker::zg::path::node_t const & n2);

/*!
 \class path_edge_t
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
  edge_t(tchecker::zg::path::edge_t const & e);

  /*!
   \brief Move constructor
  */
  edge_t(tchecker::zg::path::edge_t && e);

  /*!
   \brief Destructor
  */
  ~edge_t() = default;

  /*!
   \brief Assignment operator
   \post this point to the same transition as e
  */
  tchecker::zg::path::edge_t & operator=(tchecker::zg::path::edge_t const & e);

  /*!
   \brief Move assignment operator
  */
  tchecker::zg::path::edge_t & operator=(tchecker::zg::path::edge_t && e);

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
int lexical_cmp(tchecker::zg::path::edge_t const & e1, tchecker::zg::path::edge_t const & e2);

/*!
 \class finite_path_t
 \brief Finite path in a zone graph
*/
class finite_path_t
    : public tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::node_t, tchecker::zg::path::edge_t> {
public:
  /*!
   \brief Constructor
   \param zg : a zone graph
   \post this is an empty path
   \note this keeps a pointer to zg
   \note all nodes and edges added to this path shall be built from states and transitions in zg
  */
  using tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::node_t, tchecker::zg::path::edge_t>::finite_path_t;

  using tchecker::graph::finite_path_t<tchecker::zg::path::node_t, tchecker::zg::path::edge_t>::attributes;

protected:
  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(tchecker::zg::path::node_t const & n, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(tchecker::zg::path::edge_t const & e, std::map<std::string, std::string> & m) const;
};

/* output */

/*!
 \brief Lexicographic ordering on path nodes
*/
class node_le_t {
public:
  /*!
   \brief Lexicographic ordering
   \param n1 : a path node
   \param n2 : a path node
   \return true if n1 is lexicographically smaller than n2, false otherwise
  */
  bool operator()(tchecker::zg::path::finite_path_t::node_sptr_t const & n1,
                  tchecker::zg::path::finite_path_t::node_sptr_t const & n2) const;
};

/*!
 \brief Lexicograohic ordering on edges
 \class path_edge_le_t
*/
class edge_le_t {
public:
  /*!
   \brief Lexicographic ordering on path edges
   \param e1 : a path edge
   \param e2 : a path edge
   \return true if e1 is lexicographically smaller than e2, false otherwise
  */
  bool operator()(tchecker::zg::path::finite_path_t::edge_sptr_t const & e1,
                  tchecker::zg::path::finite_path_t::edge_sptr_t const & e2) const;
};

/*!
 \brief Dot output of zone graph finite paths
 \param os : output stream
 \param path : a path
 \param name : path name
 \post path has been output to os following the graphviz DOT file format
 \return os after output
*/
std::ostream & dot_output(std::ostream & os, tchecker::zg::path::finite_path_t const & path, std::string const & name);

/*!
 \brief Compute a finite symbolic run in a zone graph following a specified sequence of tuples of edges
 \tparam VEDGE_RANGE : type of range of tuple of edges, shall dereference to tchecker::const_vedge_sptr_t
 \param zg : zone graph
 \param initial_vloc : tuple of initial locations
 \param seq : sequence of tuple of edges as a range
 \pre initial_vloc is a tuple of initial locations in zg
 \pre seq is a feasible sequence in zg from initial_vloc
 \return a finite path in refzg, starting from the initial state with tuple of locations initial_vloc,
 and that follows the sequence seq if possible. If the path is not empty, its first node has flag initial set to true
 \throw std::invalid_argument : if there is not initial state in refzg with tuple of locations initial_vloc
 or if seq is not feasible from the initial state
 \note the returned path keeps a shared pointer on zg
 */
template <typename VEDGE_RANGE>
tchecker::zg::path::finite_path_t * compute_symbolic_run(std::shared_ptr<tchecker::zg::zg_t> const & zg,
                                                         tchecker::vloc_t const & initial_vloc, VEDGE_RANGE const & seq)
{
  tchecker::zg::path::finite_path_t * path = new tchecker::zg::path::finite_path_t{zg};

  tchecker::zg::const_state_sptr_t s{tchecker::zg::initial(*zg, initial_vloc)};
  if (s.ptr() == nullptr) {
    delete path;
    throw std::invalid_argument("No initial state with given tuple of locations");
  }

  path->add_first_node(s);
  path->first()->initial(true);

  for (tchecker::const_vedge_sptr_t const & vedge_ptr : seq) {
    s = path->last()->state_ptr();
    auto && [nexts, nextt] = tchecker::zg::next(*zg, s, *vedge_ptr);
    if (nexts.ptr() == nullptr || nextt.ptr() == nullptr) {
      delete path;
      throw std::invalid_argument("Sequence is not feasible from given initial locations");
    }
    path->extend_back(nextt, nexts);
  }

  return path;
}

} // namespace path

} // namespace zg

} // namespace tchecker

#endif // TCHECKER_ZG_PATH_HH