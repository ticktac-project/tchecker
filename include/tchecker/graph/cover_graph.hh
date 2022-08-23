/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_COVER_GRAPH_HH
#define TCHECKER_COVER_GRAPH_HH

#include "tchecker/utils/hashtable.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file cover_graph.hh
 \brief Graph with node covering
 */

namespace tchecker {

namespace graph {

namespace cover {

/*!
 \brief Type of node in a cover graph
*/
using node_t = tchecker::collision_table_object_t;

/*!
 \class graph_t
 \brief Graph with node covering
 \tparam NODE_SPTR : type of shared pointer to node
 \tparam NODE_SPTR_HASH : type of hash function on node pointers. Should be callable
 with a NODE_SPTR argument and return a hash code for the pointed node
 \tparam NODE_SPTR_LE : less-than-or-equal predicate on nodes. Should be callable
 with two NODE_SPTR argument and return true if the first one is smaller than the
 second one, and false otherwise. Usually, two nodes that are comparable w.r.t.
 NODE_SPTR_LE should have the same hash code returned by NODE_SPTR_HASH
 \note This graph allows to check if there is a node in the graph that covers
 some given node. Nodes are compared using NODE_SPTR_LE. Only the nodes with the same
 hash value w.r.t. NODE_SPTR_HASH are compared
 */
template <class NODE_SPTR, class NODE_SPTR_HASH, class NODE_SPTR_LE> class graph_t {
public:
  /*!
   \brief Type of node shared pointer
  */
  using node_sptr_t = NODE_SPTR;

  /*!
   \brief Constructor
   \param table_size : size of the collision table of nodes
   \param node_hash : hash function
   \param node_le : covering predicate on nodes
   \pre table_size != tchecker::COLLISION_TABLE_NOT_STORED
   \throw std::invalid_argument : if the precondition is violated
   */
  graph_t(std::size_t table_size, NODE_SPTR_HASH const & node_hash, NODE_SPTR_LE const & node_le)
      : _nodes(table_size, node_hash), _node_le(node_le)
  {
  }

  /*!
   \brief Copy constructor (deleted)
   */
  graph_t(tchecker::graph::cover::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_LE> const &) = delete;

  /*!
   \brief Move constructor
   */
  graph_t(tchecker::graph::cover::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_LE> &&) = default;

  /*!
   \brief Destructor
   */
  ~graph_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::graph::cover::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_LE> &
  operator=(tchecker::graph::cover::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_LE> const &) = delete;

  /*!
   \brief Move-assignment operator
   */
  tchecker::graph::cover::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_LE> &
  operator=(tchecker::graph::cover::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_LE> &&) = default;

  /*!
   \brief Clear
   \post The graph is empty
   \note No destructor call on nodes
   \note Invalidates iterators
   */
  void clear() { _nodes.clear(); }

  /*!
   \brief Add node to the graph
   \param n : a node
   \pre n is not stored in a graph
   \post n has been added to the graph
   \throw std::invalid_argument : if n is already stored in a graph
   \note Complexity : computation of the hash value of node n
   \note Invalidates iterators
   */
  void add_node(NODE_SPTR const & n) { _nodes.add(n); }

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
   \brief Check if a node is covered in the graph
   \param n : a node
   \param covering_node : a node
   \post covering_node is such that NODE_SPTR_LE(n, covering_node) is true if such
   node exists in the graph, nullptr otherwise
   \return true if if a covering node has been found for n, false otherwise
   \note Only the nodes which have the same hash value than n w.r.t. NODE_SPTR_HASH will
   be considered as potential covering nodes
   \note this function ensures that n is never covered by itself (i.e. it
   returns false if n is the only node in this graph for which NODE_SPTR_LE is true)
   */
  bool is_covered(NODE_SPTR const & n, NODE_SPTR & covering_node) const
  {
    auto && range = _nodes.collision_range(n);
    for (NODE_SPTR const & node : range) {
      if ((n != node) && _node_le(n, node)) {
        covering_node = node;
        return true;
      }
    }
    covering_node = nullptr;
    return false;
  }

  /*!
   \brief Accessor to the nodes in the graph that are covered by a given node
   \param n : a node
   \param ins : an inserter iterator that accepts NODE_SPTR
   \post All the nodes in this graph with the same hash value as n, and that are
   smaller-than-or-equal-to n w.r.t. NODE_SPTR_LE have been inserted using ins
   \note this function ensures that n is never covered by itself (i.e. n is not
   added to ins if it belongs to this graph)
   */
  template <class INSERTER> void covered_nodes(NODE_SPTR const & n, INSERTER & ins) const
  {
    auto && range = _nodes.collision_range(n);
    for (NODE_SPTR const & node : range)
      if ((node != n) && _node_le(node, n))
        ins = node;
  }

  /*!
   \brief Accessor
   \return Number of nodes in this graph
   */
  inline std::size_t size() const { return _nodes.size(); }

  /*!
   \brief Type of iterator over the nodes in the graph
   */
  using const_iterator_t = typename tchecker::collision_table_t<NODE_SPTR, NODE_SPTR_HASH>::const_iterator_t;

  /*!
   \brief Accessor
   \return Iterator pointing to the first node in the graph, or past-the-end if the graph is empty
   */
  tchecker::graph::cover::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_LE>::const_iterator_t begin() const
  {
    return _nodes.begin();
  }

  /*!
   \brief Accessor
   \return Past-the-end iterator
   */
  tchecker::graph::cover::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_LE>::const_iterator_t end() const
  {
    return _nodes.end();
  }

  /*!
   \brief Accessor
   \return Range of nodes
  */
  tchecker::range_t<tchecker::graph::cover::graph_t<NODE_SPTR, NODE_SPTR_HASH, NODE_SPTR_LE>::const_iterator_t> nodes() const
  {
    return tchecker::make_range(begin(), end());
  }

private:
  tchecker::collision_table_t<NODE_SPTR, NODE_SPTR_HASH> _nodes; /*!< Set of nodes */
  NODE_SPTR_LE _node_le;                                         /*!< Covering predicate on node pointers */
};

} // end of namespace cover

} // end of namespace graph

} // end of namespace tchecker

#endif // TCHECKER_COVER_GRAPH_HH
