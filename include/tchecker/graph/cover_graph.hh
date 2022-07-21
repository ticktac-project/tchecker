/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_COVER_GRAPH_HH
#define TCHECKER_COVER_GRAPH_HH

#include <functional>
#include <vector>

#include "tchecker/utils/iterator.hh"

/*!
 \file cover_graph.hh
 \brief Graph with node covering
 */

namespace tchecker {

namespace graph {

namespace cover {

// Forward declarations
template <class NODE_PTR, class NODE_HASH, class NODE_LE> class graph_t;

/*!
 \brief Type of node position in the node container
 */
using node_position_t = unsigned int;

/*!
 \brief Not-stored node position
*/
extern tchecker::graph::cover::node_position_t const NOT_STORED;

/*!
 \class node_t
 \brief Node in a cover graph
 \note Stores the position of the node in the implementation of the graph for
 fast removal. The graph is implemented as a table of containers of nodes. The
 node stores two positions: its position in the table, and its position in the
 container of nodes
 */
class node_t {
public:
  /*!
   \brief Constructor
   \post this node is not stored in the graph
   */
  node_t();

  /*!
   \brief Copy constructor
   \throw std::invalid_argument if node is stored
   */
  node_t(tchecker::graph::cover::node_t const & node);

  /*!
   \brief Move constructor
   */
  node_t(tchecker::graph::cover::node_t &&) = default;

  /*!
   \brief Destructor
   */
  ~node_t() = default;

  /*!
   \brief Assignment operator
   \throw std::invalid_argument if node is stored
   */
  tchecker::graph::cover::node_t & operator=(tchecker::graph::cover::node_t const & node);

  /*!
   \brief Move-assignment operator
   */
  tchecker::graph::cover::node_t & operator=(tchecker::graph::cover::node_t &&) = default;

private:
  template <class NODE_PTR, class NODE_HASH, class NODE_LE> friend class tchecker::graph::cover::graph_t;

  /*!
   \brief Accessor
   \return position of this node in the table of node containers
   */
  inline tchecker::graph::cover::node_position_t position_in_table() const { return _position_in_table; }

  /*!
   \brief Accessor
   \return position of this node in the node container
   */
  inline tchecker::graph::cover::node_position_t position_in_container() const { return _position_in_container; }

  /*!
   \brief Setter
   \param position_in_table : a position
   \param position_in_container : a position
   \pre position_in_table != tchecker::graph::cover::NOT_STORED
   and position_in_container != tchecker::graph::cover::NOT_STORED
   \post the position of this node has been set to (position_in_table,
   position_in_container)
   \throw std::invalid_argument : if precondition is violated
   */
  void set_position(tchecker::graph::cover::node_position_t position_in_table,
                    tchecker::graph::cover::node_position_t position_in_container);

  /*!
   \brief Clear
   \post this node position has been cleared
   */
  void clear_position();

  /*!
   \brief Check if this node is stored in the graph based on the node positions
   \return true if this node is stored in a graph, false otherwise
   */
  bool is_stored() const;

  node_position_t _position_in_table;     /*!< Position in the table of node containers */
  node_position_t _position_in_container; /*!< Position in the node container */
};

/*!
 \class graph_t
 \brief Graph with node covering
 \tparam NODE_PTR : type of pointer to node, should be  shared pointer, either
 std::shared_ptr<> or  tchecker::intrusive_shared_ptr_t<> of some node type
 \tparam NODE_HASH : type of hash function on node pointers. Should be callable
 with a NODE_PTR argument and return a hash code for the pointed node
 \tparam NODE_LE : less-than-or-equal predicate on nodes. Should be callable
 with two NODE_PTR argument and return true if the first one is smaller than the
 second one, and false otherwise
 \note This graph allows to check if there is a node in the graph that covers
 some given node. Nodes are compared using NODE_LE. Only the nodes with the same
 hash value w.r.t. NODE_HASH are compared
 */
template <class NODE_PTR, class NODE_HASH, class NODE_LE> class graph_t {
private:
  /*!
   \brief Type of node container
   */
  using nodes_container_t = std::vector<NODE_PTR>;

public:
  /*!
   \brief Type of node pointer
  */
  using node_ptr_t = NODE_PTR;

  /*!
   \brief Constructor
   \param table_size : size of the hash table of node containers
   \param node_hash : hash function
   \param node_le : covering predicate on nodes
   \pre table_size should be less than tchecker::graph::cover::NOT_STORED
   \throw std::invalid_argument : if the precondition is violated
   */
  graph_t(std::size_t table_size, NODE_HASH node_hash, NODE_LE node_le)
      : _nodes(table_size), _node_hash(node_hash), _node_le(node_le), _size(0)
  {
    if (table_size >= tchecker::graph::cover::NOT_STORED)
      throw std::invalid_argument("Table size is too big");
  }

  /*!
   \brief Copy constructor (deleted)
   */
  graph_t(tchecker::graph::cover::graph_t<NODE_PTR, NODE_HASH, NODE_LE> const &) = delete;

  /*!
   \brief Move constructor
   */
  graph_t(tchecker::graph::cover::graph_t<NODE_PTR, NODE_HASH, NODE_LE> &&) = default;

  /*!
   \brief Destructor
   */
  ~graph_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::graph::cover::graph_t<NODE_PTR, NODE_HASH, NODE_LE> &
  operator=(tchecker::graph::cover::graph_t<NODE_PTR, NODE_HASH, NODE_LE> const &) = delete;

  /*!
   \brief Move-assignment operator
   */
  tchecker::graph::cover::graph_t<NODE_PTR, NODE_HASH, NODE_LE> &
  operator=(tchecker::graph::cover::graph_t<NODE_PTR, NODE_HASH, NODE_LE> &&) = default;

  /*!
   \brief Clear
   \post The graph is empty
   \note No destructor call on nodes
   \note Invalidates iterators
   */
  void clear()
  {
    for (auto & container : _nodes)
      clear(container);
    _nodes.clear();
    _size = 0;
  }

  /*!
   \brief Add node to the graph
   \param n : a node
   \pre n is not stored in a graph
   \post n has been added to the graph
   \throw std::invalid_argument : if n is already stored in a graph
   \note Complexity : computation of the hash value of node n
   \note Invalidates iterators
   */
  void add_node(NODE_PTR const & n)
  {
    if (n->is_stored())
      throw std::invalid_argument("Adding a node that is already stored is not allowed");
    tchecker::graph::cover::node_position_t position_in_table = compute_position_in_table(n);
    tchecker::graph::cover::node_position_t position_in_container = add_node(n, _nodes[position_in_table]);
    n->set_position(position_in_table, position_in_container);
    ++_size;
  }

  /*!
   \brief Remove node from the graph
   \param n : a node
   \pre n is stored in this graph
   \post n has been removed from this graph
   \throw std::invalid_argument : if n is not stored in this graph
   \note Contant-time complexity
   \note Invalidates iterators
   */
  void remove_node(NODE_PTR const & n)
  {
    if (!n->is_stored())
      throw std::invalid_argument("Removing a node that is not stored");
    tchecker::graph::cover::node_position_t position_in_table = n->position_in_table();
    if (position_in_table >= _nodes.size())
      throw std::invalid_argument("Removing a node which is not stored in this graph");
    remove_node(n, _nodes[position_in_table]);
    n->clear_position();
    --_size;
  }

  /*!
   \brief Check if a node is covered in the graph
   \param n : a node
   \param covering_node : a node
   \post covering_node is such that NODE_LE(n, covering_node) is true if such
   node exists in the graph, nullptr otherwise
   \return true if if a covering node has been found for n, false otherwise
   \note Only the nodes which have the same hash value than n w.r.t. HASH will
   be considered as potential covering nodes
   \note A node is never covered by itself
   */
  bool is_covered(NODE_PTR const & n, NODE_PTR & covering_node) const
  {
    tchecker::graph::cover::node_position_t position_in_table = compute_position_in_table(n);
    return is_covered(n, _nodes[position_in_table], covering_node);
  }

  /*!
   \brief Accessor to the nodes in the graph that are covered by a given node
   \param n : a node
   \param ins : an inserter iterator that accepts NODE_PTR
   \post All the nodes in this graph with the same hash value as n, and that are
   smaller-than-or-equal-to n w.r.t. NODE_LE have been inserted using ins
   \note A node is never covered by itself
   */
  template <class INSERTER> void covered_nodes(NODE_PTR const & n, INSERTER & ins) const
  {
    tchecker::graph::cover::node_position_t position_in_table = compute_position_in_table(n);
    covered_nodes(n, _nodes[position_in_table], ins);
  }

  /*!
   \brief Accessor
   \return Number of nodes in this graph
   */
  inline std::size_t size() const { return _size; }

  /*!
   \brief Type of iterator over the nodes in the graph
   */
  using const_iterator_t = tchecker::join_iterator_t<tchecker::range_t<typename std::vector<nodes_container_t>::const_iterator>,
                                                     tchecker::range_t<typename nodes_container_t::const_iterator>>;

  /*!
   \brief Accessor
   \return Iterator pointing to the first node in the graph, or past-the-end if the graph is empty
   */
  tchecker::graph::cover::graph_t<NODE_PTR, NODE_HASH, NODE_LE>::const_iterator_t begin() const
  {
    return tchecker::graph::cover::graph_t<NODE_PTR, NODE_HASH, NODE_LE>::const_iterator_t(
        _nodes.begin(), _nodes.end(), tchecker::graph::cover::graph_t<NODE_PTR, NODE_HASH, NODE_LE>::nodes_container_range);
  }

  /*!
   \brief Accessor
   \return Past-the-end iterator
   */
  tchecker::graph::cover::graph_t<NODE_PTR, NODE_HASH, NODE_LE>::const_iterator_t end() const
  {
    return tchecker::graph::cover::graph_t<NODE_PTR, NODE_HASH, NODE_LE>::const_iterator_t(
        _nodes.end(), _nodes.end(), tchecker::graph::cover::graph_t<NODE_PTR, NODE_HASH, NODE_LE>::nodes_container_range);
  }

  /*!
   \brief Accessor
   \return Range of nodes
  */
  tchecker::range_t<tchecker::graph::cover::graph_t<NODE_PTR, NODE_HASH, NODE_LE>::const_iterator_t> nodes() const
  {
    return tchecker::make_range(begin(), end());
  }

private:
  /*!
   \brief Computes node position in table
   \param n : a node
   \return The position than n should have in the table of nodes
  */
  inline tchecker::graph::cover::node_position_t compute_position_in_table(NODE_PTR const & n) const
  {
    return _node_hash(n) % _nodes.size();
  }

  /*!
   \brief Clear a node container
   \param c : a container
   \post c is empty
   \note No destructor call on nodes
   \note Invalidates iterators
  */
  void clear(nodes_container_t & c)
  {
    for (NODE_PTR const & n : c)
      n->clear_position();
    c.clear();
  }

  /*!
   \brief Add a node to a container
   \param n : a node
   \param c : a container
   \post n has been added to c
   \return The position of node n in container c
  */
  tchecker::graph::cover::node_position_t add_node(NODE_PTR const & n, nodes_container_t & c)
  {
    c.push_back(n);
    return c.size() - 1;
  }

  /*!
   \brief Remove a node from a container
   \param n : a node
   \param c : a container
   \post n has been removed from c
   \throw std::invalid_argument : if n is not stored in c
  */
  void remove_node(NODE_PTR const & n, nodes_container_t & c)
  {
    tchecker::graph::cover::node_position_t position_in_container = n->position_in_container();
    if (position_in_container >= c.size() || c[position_in_container] != n)
      throw std::invalid_argument("Removing a node that is not stored in this container");
    NODE_PTR back_node = c.back();
    back_node->set_position(back_node->position_in_table(), position_in_container);
    c[position_in_container] = back_node;
    c.pop_back();
  }

  /*!
   \brief Check if a node is covered in a node container
   \param n : a node
   \param c : a node container
   \param covering_node : a node
   \post covering_node is such that NODE_LE(n, covering_node) is true if such
   node exists in the graph, nullptr otherwise
   \return true if if a covering node has been found for n, false otherwise
   \note Only the nodes which have the same hash value than n w.r.t. HASH will
   be considered as potential covering nodes
   */
  bool is_covered(NODE_PTR const & n, nodes_container_t const & c, NODE_PTR & covering_node) const
  {
    for (NODE_PTR const & node : c) {
      if ((n != node) && _node_le(n, node)) {
        covering_node = node;
        return true;
      }
    }
    covering_node = nullptr;
    return false;
  }

  /*!
   \brief Accessor to the nodes in a container that are covered by a given node
   \param n : a node
   \param c : a node container
   \param ins : an inserter iterator that accepts NODE_PTR
   \post All the nodes in c that are smaller-than-or-equal-to n w.r.t. NODE_LE
   have been inserted using ins
   */
  template <class INSERTER> void covered_nodes(NODE_PTR const & n, nodes_container_t const & c, INSERTER & ins) const
  {
    for (NODE_PTR const & node : c)
      if ((node != n) && _node_le(node, n))
        ins = node;
  }

  /*!
   \brief Accessor to range of nodes in a container
   \param it : an iterator to a nodes container
   \pre it can be dereferenced
   \return A range (begin, end) of nodes in the container pointer by it
   */
  static tchecker::range_t<typename nodes_container_t::const_iterator>
  nodes_container_range(typename std::vector<nodes_container_t>::const_iterator const & it)
  {
    return tchecker::make_range(it->begin(), it->end());
  }

  std::vector<nodes_container_t> _nodes; /*!< Nodes */
  NODE_HASH _node_hash;                  /*!< Hash function on nodes */
  NODE_LE _node_le;                      /*!< Covering predicate on nodes */
  std::size_t _size;                     /*!< Number of nodes */
};

} // end of namespace cover

} // end of namespace graph

} // end of namespace tchecker

#endif // TCHECKER_COVER_GRAPH_HH
