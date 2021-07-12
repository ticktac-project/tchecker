/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_REACHABILITY_GRAPH_HH
#define TCHECKER_REACHABILITY_GRAPH_HH

/*!
 \file reachability_graph.hh
 \brief Reachability graph
 */

#include <map>
#include <set>
#include <string>

#include "tchecker/graph/allocators.hh"
#include "tchecker/graph/directed_graph.hh"
#include "tchecker/graph/find_graph.hh"
#include "tchecker/graph/output.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/shared_objects.hh"

namespace tchecker {

namespace graph {

namespace reachability {

// Forward declarations
template <class NODE, class EDGE> class node_t;
template <class NODE, class EDGE> class edge_t;

/*!
 \brief Type of shared node
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
using shared_node_t = tchecker::make_shared_t<tchecker::graph::reachability::node_t<NODE, EDGE>>;

/*!
 \brief Type of pointer to shared node
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
using node_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::graph::reachability::shared_node_t<NODE, EDGE>>;

/*!
 \brief Type of pointer to const shared node
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
using const_node_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::graph::reachability::shared_node_t<NODE, EDGE> const>;

/*!
 \brief Type of shared edge
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
using shared_edge_t = tchecker::make_shared_t<tchecker::graph::reachability::edge_t<NODE, EDGE>>;

/*!
 \brief Type of pointer to shared edge
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
using edge_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::graph::reachability::shared_edge_t<NODE, EDGE>>;

/*!
 \brief Type of pointer to const shared edge
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
using const_edge_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::graph::reachability::shared_edge_t<NODE, EDGE> const>;

/*!
 \brief Type of reachability graph node that inherits from NODE
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
class node_t : public NODE, public tchecker::graph::directed::node_t<tchecker::graph::reachability::edge_sptr_t<NODE, EDGE>> {
public:
  using NODE::NODE;
};

/*!
 \brief Type of reachability graph edge that inherits from EDGE
 \tparam NODE : type of user node
 \tparam EDGE : type of user edge
 */
template <class NODE, class EDGE>
class edge_t : public EDGE,
               public tchecker::graph::directed::edge_t<tchecker::graph::reachability::node_sptr_t<NODE, EDGE>,
                                                        tchecker::graph::reachability::edge_sptr_t<NODE, EDGE>> {
public:
  using EDGE::EDGE;
};

} // end of namespace reachability

} // end of namespace graph

/*!
 \class allocation_size_t
 \brief Specialisation of class allocation_size_t for type tchecker::graph::reachability::node_t
 */
template <class NODE, class EDGE> class allocation_size_t<tchecker::graph::reachability::node_t<NODE, EDGE>> {
public:
  /*!
     \brief Allocation size for objects of type tchecker::graph::reachability::node_t
     \param args : parameters needed to determine the allocation size of
     tchecker::graph::reachability::node_t
     */
  template <class... ARGS> static std::size_t alloc_size(ARGS &&... args)
  {
    return sizeof(tchecker::graph::reachability::node_t<NODE, EDGE>);
  }
};

/*!
 \class allocation_size_t
 \brief Specialisation of class allocation_size_t for type tchecker::graph::reachability::edge_t
 */
template <class NODE, class EDGE> class allocation_size_t<tchecker::graph::reachability::edge_t<NODE, EDGE>> {
public:
  /*!
     \brief Allocation size for objects of type tchecker::graph::reachability::edge_t
     \param args : parameters needed to determine the allocation size of
     tchecker::graph::reachability::edge_t
     */
  template <class... ARGS> static std::size_t alloc_size(ARGS &&... args)
  {
    return sizeof(tchecker::graph::reachability::edge_t<NODE, EDGE>);
  }
};

namespace graph {

namespace reachability {

/*!
 \class graph_t
 \brief Graph that allocates and stores nodes and edges in a reachability graph
 \tparam NODE : type of nodes
 \tparam EDGE : type of edges
 \tparam NODE_HASH : hash function on nodes
 \tparam NODE_EQUAL : equality predicate on nodes
 \note this graph allocates nodes of type
 tchecker::graph::reachability::node_t<NODE, EDGE> and edges of type
 tchecker::graph::reachability::edge_t<NODE, EDGE>
*/
template <class NODE, class EDGE, class NODE_HASH, class NODE_EQUAL> class graph_t {
private:
  // Forward declarations
  class node_sptr_hash_t;
  class node_sptr_equal_to_t;

public:
  /*!
   \brief Type of nodes
   */
  using node_t = NODE;

  /*!
   \brief Type of shared nodes
  */
  using shared_node_t = tchecker::graph::reachability::shared_node_t<NODE, EDGE>;

  /*!
  \brief Type of pointer to shared nodes
  */
  using node_sptr_t = tchecker::graph::reachability::node_sptr_t<NODE, EDGE>;

  /*!
  \brief Type of pointer to const shared nodes
  */
  using const_node_sptr_t = tchecker::graph::reachability::const_node_sptr_t<NODE, EDGE>;

  /*!
  \brief Type of edges
  */
  using edge_t = EDGE;

  /*!
   \brief Type of shared edge
  */
  using shared_edge_t = tchecker::graph::reachability::shared_edge_t<NODE, EDGE>;

  /*!
  \brief Type of pointer to shared edge
  */
  using edge_sptr_t = tchecker::graph::reachability::edge_sptr_t<NODE, EDGE>;

  /*!
  \brief Type of pointer to const shared edge
  */
  using const_edge_sptr_t = tchecker::graph::reachability::const_edge_sptr_t<NODE, EDGE>;

  /*!
  \brief Constructor
  \param block_size : number of objects allocated in a block
  \param table_size : size of hash table
  */
  graph_t(std::size_t block_size, std::size_t table_size)
      : _find_graph(table_size), _node_pool(block_size), _edge_pool(block_size)
  {
  }

  /*!
  \brief Copy constructor (deleted)
  */
  graph_t(tchecker::graph::reachability::graph_t<NODE, EDGE, NODE_HASH, NODE_EQUAL> const &) = delete;

  /*!
  \brief Move constructor (deleted)
  */
  graph_t(tchecker::graph::reachability::graph_t<NODE, EDGE, NODE_HASH, NODE_EQUAL> &&) = delete;

  /*!
  \brief Destructor
  */
  virtual ~graph_t() { clear(); }

  /*!
  \brief Assignment operator (deleted)
  */
  tchecker::graph::reachability::graph_t<NODE, EDGE, NODE_HASH, NODE_EQUAL> &
  operator=(tchecker::graph::reachability::graph_t<NODE, EDGE, NODE_HASH, NODE_EQUAL> const &) = delete;

  /*!
  \brief Move-assignment operator (deleted)
  */
  tchecker::graph::reachability::graph_t<NODE, EDGE, NODE_HASH, NODE_EQUAL> &
  operator=(tchecker::graph::reachability::graph_t<NODE, EDGE, NODE_HASH, NODE_EQUAL> &&) = delete;

  /*!
  \brief Clear the graph
  \post the graph is empty
  */
  void clear()
  {
    _directed_graph.clear(_find_graph.begin(), _find_graph.end());
    _find_graph.clear();
    _node_pool.destruct_all();
    _edge_pool.destruct_all();
  }

  /*!
  \brief Add a node
  \param args : arguments to a constructor of type NODE
  \post an instance of NODE(args) has been added to the graph if it does not
  already contain an instance that is similar w.r.t. NODE_EQUAL
  \return a pair (status, n) where status is true if n is a new node that has
  been created and added to the graph, and status is false if the graph already
  contains node n that is equivalent w.r.t NODE_HASH and NODE_EQUAL
   */
  template <class... ARGS> std::tuple<bool, node_sptr_t> add_node(ARGS &&... args)
  {
    node_sptr_t node = _node_pool.construct(args...);
    node_sptr_t found_node = _find_graph.find(node);
    if (found_node != node)
      return std::make_tuple(false, found_node);
    _find_graph.add_node(node);
    return std::make_tuple(true, node);
  }

  /*!
   \brief Add an edge
   \param n1 : source node
   \param n2 : target node
   \param args : arguments to a constructor of EDGE
   \pre n1 and n2 should be nodes of the graph
   \post an instance of EDGE(args) from node n1 to node n2 has been added to the
   graph
   */
  template <class... ARGS> void add_edge(node_sptr_t const & n1, node_sptr_t const & n2, ARGS &&... args)
  {
    edge_sptr_t edge = _edge_pool.construct(args...);
    _directed_graph.add_edge(n1, n2, edge);
  }

  /*!
  \brief Type of node iterator
  */
  using const_node_iterator_t =
      typename tchecker::graph::find::graph_t<node_sptr_t, node_sptr_hash_t, node_sptr_equal_to_t>::const_iterator_t;

  /*!
  \brief Accessor
  \return range of nodes in the graph
  */
  inline tchecker::range_t<tchecker::graph::reachability::graph_t<NODE, EDGE, NODE_HASH, NODE_EQUAL>::const_node_iterator_t>
  nodes() const
  {
    return tchecker::make_range(_find_graph.begin(), _find_graph.end());
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
  inline tchecker::range_t<tchecker::graph::reachability::graph_t<NODE, EDGE, NODE_HASH, NODE_EQUAL>::incoming_edges_iterator_t>
  incoming_edges(node_sptr_t const & n) const
  {
    return _directed_graph.incoming_edges(n);
  }

  /*!
   \brief Accessor
   \param n : node
   \return range of outgoing edges of node n
   */
  inline tchecker::range_t<tchecker::graph::reachability::graph_t<NODE, EDGE, NODE_HASH, NODE_EQUAL>::outgoing_edges_iterator_t>
  outgoing_edges(node_sptr_t const & n) const
  {
    return _directed_graph.outgoing_edges(n);
  }

  /*!
   \brief Accessor
   \param edge : an edge
   \return the source node of edge
   */
  inline node_sptr_t const & edge_src(edge_sptr_t const & edge) const { return _directed_graph.edge_src(edge); }

  /*!
   \brief Accessor
   \param edge : an edge
   \return the target node of edge
   */
  inline node_sptr_t const & edge_tgt(edge_sptr_t const & edge) const { return _directed_graph.edge_tgt(edge); }

  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
   \post attributes of node n have been added to map m
  */
  void attributes(node_sptr_t const & n, std::map<std::string, std::string> & m) const { attributes(*n, m); }

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
   \post attributes of edge e have been added to map m
  */
  void attributes(edge_sptr_t const & e, std::map<std::string, std::string> & m) const { attributes(*e, m); }

protected:
  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(NODE const & n, std::map<std::string, std::string> & m) const = 0;

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
  */
  virtual void attributes(EDGE const & e, std::map<std::string, std::string> & m) const = 0;

private:
  /*!
   \class node_sptr_hash_t
   \brief Hash functor for node pointers
   */
  class node_sptr_hash_t : public NODE_HASH {
  public:
    using NODE_HASH::NODE_HASH;

    /*!
     \brief Hash function on shared pointers to nodes
     \param n : a shared pointer to node
     \return hash value for *n w.r.t. NODE_HASH
     */
    inline std::size_t operator()(node_sptr_t const & n) const { return NODE_HASH::operator()(*n); }
  };

  /*!
   \class node_sptr_equal_to_t
   \brief Equality functor for node pointers
   */
  class node_sptr_equal_to_t : public NODE_EQUAL {
  public:
    using NODE_EQUAL::NODE_EQUAL;

    /*!
     \brief Equality predicate on shared pointers to nodes
     \param n1 : a node
     \param n2 : a node
     \return true if *n1 and *n2 are equal w.r.t. NODE_EQUAL, false otherwise
     */
    inline bool operator()(node_sptr_t const & n1, node_sptr_t const & n2) const { return NODE_EQUAL::operator()(*n1, *n2); }
  };

  tchecker::graph::find::graph_t<node_sptr_t, node_sptr_hash_t, node_sptr_equal_to_t> _find_graph; /*!< Node store */
  tchecker::graph::directed::graph_t<node_sptr_t, edge_sptr_t> _directed_graph;                    /*!< Edge store */
  tchecker::graph::node_pool_allocator_t<shared_node_t> _node_pool;                                /*!< Node pool allocator */
  tchecker::graph::edge_pool_allocator_t<shared_edge_t> _edge_pool;                                /*!< Edge pool allocator */
};

/* output */

/*!
 \brief Output a graph in graphviz DOT language
 \tparam GRAPH : type of graph, should inherit from
 tchecker::graph::reachability::graph_t
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

} // end of namespace reachability

} // end of namespace graph

} // end of namespace tchecker

#endif // TCHECKER_REACHABILITY_GRAPH_HH