/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FIND_GRAPH_HH
#define TCHECKER_FIND_GRAPH_HH

#include <unordered_set>

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
 \tparam NODE_PTR : type of pointer to node
 \tparam HASH : hash function on NODE_PTR (see std::hash)
 \tparam EQUAL : equality function on NODE_PTR (see std::equal_to)
 \note this graph implementation stores nodes and answers find queries.
 It does not store edges
 \note each node has a unique instance in this graph w.r.t. EQUAL
 */
template <class NODE_PTR, class HASH, class EQUAL> class graph_t {
public:
  /*!
   \brief Type of pointers of node
   */
  using node_ptr_t = NODE_PTR;

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
  */
  graph_t(std::size_t table_size = 65536) : _nodes(table_size) {}

  /*!
   \brief Copy constructor
  */
  graph_t(tchecker::graph::find::graph_t<NODE_PTR, HASH, EQUAL> const &) = default;

  /*!
   \brief Move constructor
  */
  graph_t(tchecker::graph::find::graph_t<NODE_PTR, HASH, EQUAL> &&) = default;

  /*!
   \brief Destructor
  */
  ~graph_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::graph::find::graph_t<NODE_PTR, HASH, EQUAL> &
  operator=(tchecker::graph::find::graph_t<NODE_PTR, HASH, EQUAL> const &) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::graph::find::graph_t<NODE_PTR, HASH, EQUAL> &
  operator=(tchecker::graph::find::graph_t<NODE_PTR, HASH, EQUAL> &&) = default;

  /*!
   \brief Clear
   \post The graph is empty
   \note No destructor call on nodes
   */
  inline void clear() { _nodes.clear(); }

  /*!
   \brief Accessor
   \param n : a node
   \return the node in the graph equivalent to n w.r.t. HASH and EQUAL if any,
   n otherwise
   */
  NODE_PTR const & find(NODE_PTR const & n)
  {
    auto it = _nodes.find(n);
    if (it != _nodes.end())
      return *it;
    return n;
  }

  /*!
   \brief Add node
   \param n : a node
   \post n has been added to the graph unless it already contains an equivalent
   node w.r.t. HASH and EQUAL
   \return true if n has been added to the graph, false otherwise
   */
  bool add_node(NODE_PTR const & n)
  {
    try {
      _nodes.insert(n);
      return true;
    }
    catch (...) {
    }
    return false;
  }

  /*!
   \brief Type of iterator on nodes
   */
  using const_iterator_t = typename std::unordered_set<NODE_PTR, HASH, EQUAL>::const_iterator;

  /*!
   \brief Accessor
   \return iterator on first node if any, past-the-end iterator otherwise
   */
  inline tchecker::graph::find::graph_t<NODE_PTR, HASH, EQUAL>::const_iterator_t begin() const { return _nodes.begin(); }

  /*!
   \brief Accessor
   \return past-the-end iterator
   */
  inline tchecker::graph::find::graph_t<NODE_PTR, HASH, EQUAL>::const_iterator_t end() const { return _nodes.end(); }

protected:
  std::unordered_set<NODE_PTR, HASH, EQUAL> _nodes; /*!< Set of nodes */
};

} // end of namespace find

} // end of namespace graph

} // end of namespace tchecker

#endif // TCHECKER_FIND_GRAPH_HH
