/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FIND_GRAPH_HH
#define TCHECKER_FIND_GRAPH_HH

#include "tchecker/utils/hashtable.hh"

/*!
 \file find_graph.hh
 \brief Graph with node finding
 */

namespace tchecker {

namespace graph {

namespace find {

/*!
 \class graph_t
 \brief Graph with node finding
 \tparam NODE_SPTR : type of shared  pointer to node, the pointed node should
 inherit from tchecker::hashtable_object_t
 \tparam HASH : hash function on NODE_SPTR (see std::hash), should return the
 same hash code for nodes which are equal w.r.t. EQUAL
 \tparam EQUAL : equality function on NODE_SPTR (see std::equal_to)
 \note this graph implementation stores nodes and answers find queries.
 It does not store edges
 \note each node has a unique instance in this graph w.r.t. EQUAL
 */
template <class NODE_SPTR, class HASH, class EQUAL> class graph_t {
public:
  /*!
   \brief Type of shared pointers to node
   */
  using node_sptr_t = NODE_SPTR;

  /*!
   \brief Type of hash function
   */
  using hash_t = HASH;

  /*!
   \brief Type of equality predicate
   */
  using equal_t = EQUAL;

  /*!
   \brief Constructor
   \param table_size : size of hash table
   \param hash : hash function
   \param equal : equality predicate
  */
  graph_t(std::size_t table_size, HASH const & hash, EQUAL const & equal) : _nodes(table_size, hash, equal) {}

  /*!
   \brief Copy constructor
  */
  graph_t(tchecker::graph::find::graph_t<NODE_SPTR, HASH, EQUAL> const &) = default;

  /*!
   \brief Move constructor
  */
  graph_t(tchecker::graph::find::graph_t<NODE_SPTR, HASH, EQUAL> &&) = default;

  /*!
   \brief Destructor
  */
  ~graph_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::graph::find::graph_t<NODE_SPTR, HASH, EQUAL> &
  operator=(tchecker::graph::find::graph_t<NODE_SPTR, HASH, EQUAL> const &) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::graph::find::graph_t<NODE_SPTR, HASH, EQUAL> &
  operator=(tchecker::graph::find::graph_t<NODE_SPTR, HASH, EQUAL> &&) = default;

  /*!
   \brief Clear
   \post The graph is empty
   \note No destructor call on nodes
   */
  inline void clear() { _nodes.clear(); }

  /*!
   \brief Accessor
   \param n : a node
   \return a pair (found, p) where p is true if a node p equal to n has been
   found in this hashtable, otherwise found is false and p == n
  */
  std::tuple<bool, NODE_SPTR const> find(NODE_SPTR const & n) { return _nodes.find(n); }

  /*!
   \brief Add node
   \param n : a node
   \pre n is not stored in a hashtable
   \post n has been added to the graph unless it already contains an equivalent
   node w.r.t. HASH and EQUAL
   \return true if n has been added to the graph, false otherwise
   \throw std::invalid_argument : if n is already stored in a hashtable
   */
  bool add_node(NODE_SPTR const & n) { return _nodes.add(n); }

  /*!
   \brief Type of iterator on nodes
   */
  using const_iterator_t = typename tchecker::hashtable_t<NODE_SPTR, HASH, EQUAL>::const_iterator_t;

  /*!
   \brief Accessor
   \return iterator on first node if any, past-the-end iterator otherwise
   */
  inline tchecker::graph::find::graph_t<NODE_SPTR, HASH, EQUAL>::const_iterator_t begin() const { return _nodes.begin(); }

  /*!
   \brief Accessor
   \return past-the-end iterator
   */
  inline tchecker::graph::find::graph_t<NODE_SPTR, HASH, EQUAL>::const_iterator_t end() const { return _nodes.end(); }

  /*!
   \brief Accessor
   \return Number of nodes in this graph
   */
  inline std::size_t size() const { return _nodes.size(); }

protected:
  tchecker::hashtable_t<NODE_SPTR, HASH, EQUAL> _nodes; /*!< Set of nodes */
};

} // end of namespace find

} // end of namespace graph

} // end of namespace tchecker

#endif // TCHECKER_FIND_GRAPH_HH
