/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SUBSUMPTION_GRAPH_HH
#define TCHECKER_SUBSUMPTION_GRAPH_HH

/*!
 \file subsumption_graph.hh
 \brief Subsumption graph with node covering, and actual/subsumption edges
*/

#include <map>
#include <set>
#include <string>

#include "tchecker/graph/allocators.hh"
#include "tchecker/graph/cover_graph.hh"
#include "tchecker/graph/directed_graph.hh"
#include "tchecker/graph/output.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/utils/shared_objects.hh"

namespace tchecker {

namespace graph {

namespace subsumption {

// Forward declarations
template <class NODE, class EDGE> class node_t;
template <class NODE, class EDGE> class edge_t;
template <class NODE, class EDGE, class NODE_HASH, class NODE_LE> class graph_t;

/*!
 \brief Type of shared node
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
using shared_node_t = tchecker::make_shared_t<tchecker::graph::subsumption::node_t<NODE, EDGE>>;

/*!
 \brief Type of pointer to shared node
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
using node_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::graph::subsumption::shared_node_t<NODE, EDGE>>;

/*!
 \brief Type of pointer to const shared node
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
using const_node_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::graph::subsumption::shared_node_t<NODE, EDGE> const>;

/*!
 \brief Type of shared edge
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
using shared_edge_t = tchecker::make_shared_t<tchecker::graph::subsumption::edge_t<NODE, EDGE>>;

/*!
 \brief Type of pointer to shared edge
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
using edge_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::graph::subsumption::shared_edge_t<NODE, EDGE>>;

/*!
 \brief Type of pointer to const shared edge
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
using const_edge_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::graph::subsumption::shared_edge_t<NODE, EDGE> const>;

/*!
 \brief Type of subsumption graph node that inherits from NODE
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
class node_t : public NODE,
               public tchecker::graph::cover::node_t,
               public tchecker::graph::directed::node_t<tchecker::graph::subsumption::edge_sptr_t<NODE, EDGE>> {
public:
  using NODE::NODE;
};

/*!
 \brief Type of edge
*/
enum edge_type_t {
  EDGE_ACTUAL,      /*!< Actual edge */
  EDGE_SUBSUMPTION, /*!< Subsumption edge */
};

/*!
 \brief Type of subsumption graph edge that inherits from EDGE
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
class edge_t : public EDGE,
               public tchecker::graph::directed::edge_t<tchecker::graph::subsumption::node_sptr_t<NODE, EDGE>,
                                                        tchecker::graph::subsumption::edge_sptr_t<NODE, EDGE>> {
public:
  /*!
  \brief Constructor
  \param edge : type of edge
  \param args : arguments to a constructor of EDGE
  */
  template <class... ARGS>
  edge_t(enum tchecker::graph::subsumption::edge_type_t edge_type, ARGS &&... args) : EDGE(args...), _edge_type(edge_type)
  {
  }

private:
  template <class N, class E, class NODE_HASH, class NODE_LE> friend class tchecker::graph::subsumption::graph_t;

  /*!
   \brief Accessor
   \return This edge type
   */
  enum tchecker::graph::subsumption::edge_type_t edge_type() const { return _edge_type; }

  /*!
   \brief Setter
   \param edge_type : edge type
   \post this edge type has been set to edge_type
  */
  void set_edge_type(enum tchecker::graph::subsumption::edge_type_t edge_type) { _edge_type = edge_type; }

  enum tchecker::graph::subsumption::edge_type_t _edge_type; /*!< Edge type */
};

} // end of namespace subsumption

} // end of namespace graph

/*!
 \class allocation_size_t
 \brief Specialisation of class allocation_size_t for type tchecker::graph::subsumption::node_t
 */
template <class NODE, class EDGE> class allocation_size_t<tchecker::graph::subsumption::node_t<NODE, EDGE>> {
public:
  /*!
     \brief Allocation size for objects of type tchecker::graph::subsumption::node_t
     \param args : parameters needed to determine the allocation size of
     tchecker::graph::subsumption::node_t
     */
  template <class... ARGS> static std::size_t alloc_size(ARGS &&... args)
  {
    return sizeof(tchecker::graph::subsumption::node_t<NODE, EDGE>);
  }
};

/*!
 \class allocation_size_t
 \brief Specialisation of class allocation_size_t for type tchecker::graph::subsumption::edge_t
 */
template <class NODE, class EDGE> class allocation_size_t<tchecker::graph::subsumption::edge_t<NODE, EDGE>> {
public:
  /*!
     \brief Allocation size for objects of type tchecker::graph::subsumption::edge_t
     \param args : parameters needed to determine the allocation size of
     tchecker::graph::subsumption::edge_t
     */
  template <class... ARGS> static std::size_t alloc_size(ARGS &&... args)
  {
    return sizeof(tchecker::graph::subsumption::edge_t<NODE, EDGE>);
  }
};

namespace graph {

namespace subsumption {

/*!
 \class graph_t
 \brief Graph that allocates and stores nodes and edges in a subsumption graph.
 This graph allows to find covered nodes and covering nodes w.r.t. a covering
 predicate. It stores two kind of edges: actual edges and subsumption edges.
 \tparam NODE : type of nodes
 \tparam EDGE : type of edges
 \tparam NODE_HASH : hash function on nodes, should be callable with a parameter
 of type NODE const &, and return the hash value of the node
 \tparam NODE_LE : covering predicate on nodes, should be callable with two
 parameters of type NODE const &, and return true is the first node is covered
 by the second one, false otherwise
 \note this graph allocates nodes of type
 tchecker::graph::subsumption::node_t<NODE, EDGE> and edges of type
 tchecker::graph::subsumption::edge_t<NODE, EDGE>
*/
template <class NODE, class EDGE, class NODE_HASH, class NODE_LE> class graph_t {
private:
  // Forward declarations
  class node_sptr_hash_t;
  class node_sptr_le_t;

public:
  /*!
   \brief Type of nodes
   */
  using node_t = NODE;

  /*!
   \brief Type of shared nodes
  */
  using shared_node_t = tchecker::graph::subsumption::shared_node_t<NODE, EDGE>;

  /*!
  \brief Type of pointer to shared nodes
  */
  using node_sptr_t = tchecker::graph::subsumption::node_sptr_t<NODE, EDGE>;

  /*!
  \brief Type of pointer to const shared nodes
  */
  using const_node_sptr_t = tchecker::graph::subsumption::const_node_sptr_t<NODE, EDGE>;

  /*!
  \brief Type of edges
  */
  using edge_t = EDGE;

  /*!
   \brief Type of shared edge
  */
  using shared_edge_t = tchecker::graph::subsumption::shared_edge_t<NODE, EDGE>;

  /*!
  \brief Type of pointer to shared edge
  */
  using edge_sptr_t = tchecker::graph::subsumption::edge_sptr_t<NODE, EDGE>;

  /*!
  \brief Type of pointer to const shared edge
  */
  using const_edge_sptr_t = tchecker::graph::subsumption::const_edge_sptr_t<NODE, EDGE>;

  /*!
  \brief Constructor
  \param block_size : number of objects allocated in a block
  \param table_size : size of hash table
  \param node_hash : hash function on nodes
  \param node_le : covering predicate on nodes
  */
  graph_t(std::size_t block_size, std::size_t table_size, NODE_HASH const & node_hash, NODE_LE const & node_le)
      : _node_sptr_hash(node_hash), _node_sptr_le(node_le), _cover_graph(table_size, _node_sptr_hash, _node_sptr_le),
        _node_pool(block_size), _edge_pool(block_size)
  {
  }

  /*!
  \brief Copy constructor (deleted)
  */
  graph_t(tchecker::graph::subsumption::graph_t<NODE, EDGE, NODE_HASH, NODE_LE> const &) = delete;

  /*!
  \brief Move constructor (deleted)
  */
  graph_t(tchecker::graph::subsumption::graph_t<NODE, EDGE, NODE_HASH, NODE_LE> &&) = delete;

  /*!
  \brief Destructor
  */
  virtual ~graph_t() { clear(); }

  /*!
  \brief Assignment operator (deleted)
  */
  tchecker::graph::subsumption::graph_t<NODE, EDGE, NODE_HASH, NODE_LE> &
  operator=(tchecker::graph::subsumption::graph_t<NODE, EDGE, NODE_HASH, NODE_LE> const &) = delete;

  /*!
  \brief Move-assignment operator (deleted)
  */
  tchecker::graph::subsumption::graph_t<NODE, EDGE, NODE_HASH, NODE_LE> &
  operator=(tchecker::graph::subsumption::graph_t<NODE, EDGE, NODE_HASH, NODE_LE> &&) = delete;

  /*!
  \brief Clear the graph
  \post the graph is empty
  */
  void clear()
  {
    _directed_graph.clear(_cover_graph.begin(), _cover_graph.end());
    _cover_graph.clear();
    _node_pool.destruct_all();
    _edge_pool.destruct_all();
  }

  /*!
  \brief Add a node
  \param args : arguments to a constructor of type NODE
  \post an instance of NODE(args) has been added to the graph
   */
  template <class... ARGS> node_sptr_t add_node(ARGS &&... args)
  {
    node_sptr_t node = _node_pool.construct(args...);
    _cover_graph.add_node(node);
    return node;
  }

  /*!
   \brief Add an edge
   \param src : source node
   \param tgt : target node
   \param edge_type : edge type
   \param args : arguments to a constructor of type EDGE
   \pre src and tgt are nodes stored in this graph
   \post an instance of EDGE(args) has been added from src to tgt with type edge_type
  */
  template <class... ARGS>
  edge_sptr_t add_edge(node_sptr_t const & src, node_sptr_t const & tgt,
                       enum tchecker::graph::subsumption::edge_type_t edge_type, ARGS &&... args)
  {
    edge_sptr_t edge = _edge_pool.construct(edge_type, args...);
    _directed_graph.add_edge(src, tgt, edge);
    return edge;
  }

  /*!
   \brief Remove node
   \param n : a node
   \pre n is stored in this graph.
   n is disconnected (checked by assertion)
   \post n has been removed from this graph
   \note constant-time complexity
   \throw std::invalid_argument : if n is not stored in this graph
   */
  void remove_node(node_sptr_t const & n)
  {
    assert(!is_connected(n));
    _cover_graph.remove_node(n);
  }

  /*!
   \brief Remove all the edges of a node
   \param n : a node
   \post all incoming edges and outgoing edges of n have been removed from the graph
   */
  void remove_edges(node_sptr_t const & n)
  {
    _directed_graph.remove_edges(n);
    assert(!is_connected(n));
  }

  /*!
   \brief Move incoming edges
   \param n1 : a node
   \param n2 : a node
   \param edge_type : a type of edge
   \post all incoming edges of node n1 have been moved into incoming edges of node n2
   and their type has been changed to edge_type
   */
  void move_incoming_edges(node_sptr_t const & n1, node_sptr_t const & n2,
                           enum tchecker::graph::subsumption::edge_type_t edge_type)
  {
    auto in_edges = incoming_edges(n1);
    for (edge_sptr_t const & edge : in_edges)
      edge->set_edge_type(edge_type);
    _directed_graph.move_incoming_edges(n1, n2);
  }

  /*!
   \brief Check if a node is covered in this graph
   \param n : a node
   \param covering_node : a node
   \post covering_node points to a node bigger-than-or-equal-to n w.r.t. NODE_LE
   if any, nullptr otherwise
   \return true if n is NODE_LE to some node in this graph with same hash value
   than n w.r.t. NODE_HASH, false otherwise
   */
  bool is_covered(node_sptr_t const & n, node_sptr_t & covering_node) const
  {
    return _cover_graph.is_covered(n, covering_node);
  }

  /*!
   \brief Compute the nodes in the graph that are covered by a given node
   \param n : a node
   \param ins : a node inserter
   \post All the nodes in this graph that have the same hash value as n w.r.t.
   NODE_HASH, and that are covered by n w.r.t. NODE_LE, have been inserted in
   ins
   */
  template <class INSERTER> void covered_nodes(node_sptr_t const & n, INSERTER & ins) const
  {
    _cover_graph.covered_nodes(n, ins);
  }

  /*!
   \brief Type of incoming edges iterator
  */
  using incoming_edges_iterator_t =
      typename tchecker::graph::directed::graph_t<node_sptr_t, edge_sptr_t>::incoming_edges_iterator_t;

  /*!
   \brief Type of outgoing edges iterator
  */
  using outgoing_edges_iterator_t =
      typename tchecker::graph::directed::graph_t<node_sptr_t, edge_sptr_t>::outgoing_edges_iterator_t;

  /*!
   \brief Accessor
   \param n : node
   \return range of incoming edges of node n
   */
  tchecker::range_t<incoming_edges_iterator_t> incoming_edges(node_sptr_t const & n) const
  {
    return _directed_graph.incoming_edges(n);
  }

  /*!
   \brief Accessor
   \param n : node
   \return range of outgoing edges of node n
   */
  tchecker::range_t<outgoing_edges_iterator_t> outgoing_edges(node_sptr_t const & n) const
  {
    return _directed_graph.outgoing_edges(n);
  }

  /*!
   \brief Accessor
   \param edge : an edge
   \return the source node of edge
   */
  node_sptr_t const & edge_src(edge_sptr_t const & edge) const { return _directed_graph.edge_src(edge); }

  /*!
   \brief Accessor
   \param edge : an edge
   \return the target node of edge
   */
  node_sptr_t const & edge_tgt(edge_sptr_t const & edge) const { return _directed_graph.edge_tgt(edge); }

  /*!
   \brief Accessor
   \param edge : an edge
   \return the type of edge
   */
  enum tchecker::graph::subsumption::edge_type_t edge_type(edge_sptr_t const & edge) const { return edge->edge_type(); }

  /*!
   \brief Accessor
   \return Number of nodes in this graph
   */
  std::size_t nodes_count() const { return _cover_graph.size(); }

  /*!
   \brief Type of iterator on nodes
  */
  using nodes_const_iterator_t =
      typename tchecker::graph::cover::graph_t<node_sptr_t, node_sptr_hash_t, node_sptr_le_t>::const_iterator_t;

  /*!
   \brief Accessor
   \return Iterator on first node in this graph
  */
  nodes_const_iterator_t begin() const { return _cover_graph.begin(); }

  /*!
   \brief Accessor
   \return Past-the-end node iterator
  */
  nodes_const_iterator_t end() const { return _cover_graph.end(); }

  /*!
   \brief Accessor
   \return Range of nodes
  */
  tchecker::range_t<nodes_const_iterator_t> nodes() const { return _cover_graph.nodes(); }

  /*!
   \brief Accessor to node attributes
   \param n : a node
   \post Does nothing
  */
  void attributes(node_sptr_t const & n, std::map<std::string, std::string> & m) const { attributes(*n, m); }

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \post the type of edge e has been added to m
  */
  void attributes(edge_sptr_t const & e, std::map<std::string, std::string> & m) const
  {
    m["edge_type"] = (e->edge_type() == tchecker::graph::subsumption::EDGE_ACTUAL ? "actual" : "subsumption");
    attributes(*e, m);
  }

protected:
  /*!
   \brief Accessor to node attributes
   \param n : a node
  */
  virtual void attributes(NODE const & n, std::map<std::string, std::string> & m) const = 0;

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
  */
  virtual void attributes(EDGE const & e, std::map<std::string, std::string> & m) const = 0;

private:
  /*!
   \class node_sptr_hash_t
   \brief Hash functor for node pointers
   */
  class node_sptr_hash_t {
  public:
    /*!
     \brief Constructor
     \param node_hash : hash function on nodes
     \post this keeps of a copy of node_hash
    */
    node_sptr_hash_t(NODE_HASH const & node_hash) : _node_hash(node_hash) {}

    /*!
     \brief Hash function on shared pointers to nodes
     \param n : a shared pointer to node
     \return hash value for *n w.r.t. NODE_HASH
     */
    inline std::size_t operator()(node_sptr_t const & n) const { return _node_hash(*n); }

  private:
    NODE_HASH _node_hash; /*!< Hash function on nodes */
  };

  /*!
   \class node_sptr_le_t
   \brief Less-or-equal functor for node pointers
   */
  class node_sptr_le_t {
  public:
    /*!
     \brief Constructor
     \param node_le : covering predicate on nodes
     \post this keeps a copy of node_le
     */
    node_sptr_le_t(NODE_LE const & node_le) : _node_le(node_le) {}

    /*!
     \brief Covering predicate on shared pointers to nodes
     \param n1 : a node
     \param n2 : a node
     \return true if *n1 is less-than-or-equal-to *n2 w.r.t. NODE_LE, false otherwise
     */
    inline bool operator()(node_sptr_t const & n1, node_sptr_t const & n2) const { return _node_le(*n1, *n2); }

  private:
    NODE_LE _node_le; /*!< Covering predicate on nodes */
  };

  /*!
   \brief Check is a node is connected
   \param n : a node
   \return true if n has incoming or outgoing edges, false otherwise
  */
  bool is_connected(node_sptr_t const & n)
  {
    auto in_edges = incoming_edges(n);
    auto out_edges = outgoing_edges(n);
    return (in_edges.begin() != in_edges.end() || out_edges.begin() != out_edges.end());
  }

  node_sptr_hash_t _node_sptr_hash; /*!< Hash functor on shared pointers to nodes */
  node_sptr_le_t _node_sptr_le;     /*!< Covering functor on shared pointers to nodes */
  tchecker::graph::cover::graph_t<node_sptr_t, node_sptr_hash_t, node_sptr_le_t> _cover_graph; /*!< Node store with covering */
  tchecker::graph::directed::graph_t<node_sptr_t, edge_sptr_t> _directed_graph;                /*!< Edge store */
  tchecker::graph::node_pool_allocator_t<shared_node_t> _node_pool;                            /*!< Node pool allocator */
  tchecker::graph::edge_pool_allocator_t<shared_edge_t> _edge_pool;                            /*!< Edge pool allocator */
};

/* output */

/*!
 \brief Output a graph in graphviz DOT language
 \tparam GRAPH : type of graph, should inherit from
 tchecker::graph::subsumption::graph_t
 \tparam NODE_LE : total order on type GRAPH::const_node_sptr_t
 \tparam EDGE_LE : total order on type GRAPH::const_edge_sptr_t
 \param os : output stream
 \param g : a graph
 \param name : graph name
 \post the graph g has been output to os in the graphviz DOT language. The nodes
 and edges are output following the order given by NODE_LE and EDGE_LE
 */
template <class GRAPH, class NODE_LE, class EDGE_LE>
std::ostream & dot_output(std::ostream & os, GRAPH const & g, std::string const & name)
{
  return tchecker::graph::dot_output<GRAPH, NODE_LE, EDGE_LE>(os, g, name);
}

} // end of namespace subsumption

} // end of namespace graph

} // end of namespace tchecker

#endif // TCHECKER_SUBSUMPTION_GRAPH_HH