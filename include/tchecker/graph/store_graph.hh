/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STORE_GRAPH_HH
#define TCHECKER_STORE_GRAPH_HH

#include <vector>

/*!
 \file store_graph.hh
 \brief Graph as an iterable collection of nodes
 */

namespace tchecker {

namespace graph {

namespace store {

/*!
 \class graph_t
 \brief Graph as an iterable collection of nodes
 \tparam NODE_PTR : type of pointer to node
 \note this graph implementation stores nodes and allows iteration over the set
 of nodes.
 It does not store edges
 */
template <class NODE_PTR> class graph_t {
public:
  /*!
   \brief Type of pointers of node
   */
  using node_ptr_t = NODE_PTR;

  /*!
   \brief Clear
   \post The graph is empty
   \note No destructor call on nodes
   */
  inline void clear() { _nodes.clear(); }

  /*!
   \brief Add node
   \param n : a node
   \post n has been added to the graph
   */
  inline void add_node(NODE_PTR const & n) { _nodes.push_back(n); }

  /*!
   \brief Type of iterator on nodes
   */
  using const_iterator_t = typename std::vector<NODE_PTR>::const_iterator;

  /*!
   \brief Accessor
   \return iterator on first node if any, past-the-end iterator otherwise
   */
  inline tchecker::graph::store::graph_t<NODE_PTR>::const_iterator_t begin() const { return _nodes.begin(); }

  /*!
   \brief Accessor
   \return past-the-end iterator
   */
  inline tchecker::graph::store::graph_t<NODE_PTR>::const_iterator_t end() const { return _nodes.end(); }

protected:
  std::vector<NODE_PTR> _nodes; /*!< Set of nodes */
};

} // end of namespace store

} // end of namespace graph

} // end of namespace tchecker

#endif // TCHECKER_STORE_GRAPH_HH
