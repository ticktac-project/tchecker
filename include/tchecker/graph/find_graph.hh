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
 \tparam NODE_SPTR_HASH : hash function on nodes pointed by NODE_SPTR, should
 return the same hash code for nodes which are equal w.r.t. EQUAL
 \tparam NODE_SPTR_EQUAL : equality function on nodes pointed by NODE_SPTR
 \note this graph implementation stores nodes and answers find queries.
 It does not store edges
 \note each node has a unique instance in this graph w.r.t. NODE_SPTR_EQUAL
 */
template <class NODE_SPTR, class NODE_SPTR_HASH, class NODE_SPTR_EQUAL> class graph_t {
public:
  /*!
   \brief Type of shared pointers to node
   */
  using node_sptr_t = NODE_SPTR;

  /*!
   \brief Type of hash function
   */
  using hash_t = NODE_SPTR_HASH;

  /*!
   \brief Type of equality predicate
   */
  using equal_t = NODE_SPTR_EQUAL;

  /*!
   \brief Constructor
   \param table_size : size of hash table
   \param hash : hash function
   \param equal : equality predicate
  */
  graph_t(std::size_t table_size, NODE_SPTR_HASH const & hash, NODE_SPTR_EQUAL const & equal) : _nodes(table_size, hash, equal)
  {
  }

  /*!
   \brief Constructor
   \param table_size : size of hash table
   \param hash : hash function
   \param equal : equality predicate
  */
  graph_t(std::size_t table_size, NODE_SPTR_HASH && hash, NODE_SPTR_EQUAL && equal)
      : _nodes(table_size, std::move(hash), std::move(equal))
  {
  }

  /*!
   \brief Copy constructor
  */
  graph_t(tchecker::graph::find::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_EQUAL> const &) = default;

  /*!
   \brief Move constructor
  */
  graph_t(tchecker::graph::find::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_EQUAL> &&) = default;

  /*!
   \brief Destructor
  */
  ~graph_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::graph::find::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_EQUAL> &
  operator=(tchecker::graph::find::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_EQUAL> const &) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::graph::find::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_EQUAL> &
  operator=(tchecker::graph::find::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_EQUAL> &&) = default;

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
   \pre n is not stored in a graph
   \post n has been added to the graph unless it already contains an equivalent
   node w.r.t. NODE_SPTR_HASH and NODE_SPTR_EQUAL
   \return true if n has been added to the graph, false otherwise
   \throw std::invalid_argument : if n is already stored in a hashtable
   \note Invalidates iterators
   */
  bool add_node(NODE_SPTR const & n) { return _nodes.add(n); }

  /*!
   \brief Remove node from the graph
   \param n : a node
   \pre n is stored in this graph
   \post n has been removed from this graph
   \throw std::invalid_argument : if n is not stored in this graph
   \note Contant-time complexity
   \note Invalidates iterators
   */
  void remove_node(NODE_SPTR const & n) { _nodes.remove(n); }

  /*!
   \brief Type of iterator on nodes
   */
  using const_iterator_t = typename tchecker::hashtable_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_EQUAL>::const_iterator_t;

  /*!
   \brief Accessor
   \return iterator on first node if any, past-the-end iterator otherwise
   */
  inline tchecker::graph::find::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_EQUAL>::const_iterator_t begin() const
  {
    return _nodes.begin();
  }

  /*!
   \brief Accessor
   \return past-the-end iterator
   */
  inline tchecker::graph::find::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_EQUAL>::const_iterator_t end() const
  {
    return _nodes.end();
  }

  /*!
   \brief Accessor
   \return Number of nodes in this graph
   */
  inline std::size_t size() const { return _nodes.size(); }

protected:
  tchecker::hashtable_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_EQUAL> _nodes; /*!< Set of nodes */
};

} // end of namespace find

} // end of namespace graph

} // end of namespace tchecker

#endif // TCHECKER_FIND_GRAPH_HH
