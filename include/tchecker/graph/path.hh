/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GRAPH_PATH_HH
#define TCHECKER_GRAPH_PATH_HH

/*!
 \file path.hh
 \brief Representation of paths
 */

#include <functional>
#include <tuple>

#include "tchecker/graph/output.hh"
#include "tchecker/graph/reachability_graph.hh"
#include "tchecker/utils/iterator.hh"

namespace tchecker {

namespace graph {

/*!
 \class finite_path_t
 \brief Finite path
 \tparam NODE : type of nodes
 \tparam EDGE : type of edges
 \note this graph allocates nodes of type tchecker::graph::reachability::node_t<NODE, EDGE> and
 edges of type tchecker::graph::reachability::edge_t<NODE, EDGE>
*/
template <class NODE, class EDGE> class finite_path_t : private tchecker::graph::reachability::multigraph_t<NODE, EDGE> {
public:
  /*!
   \brief Type of nodes
   */
  using node_t = NODE;

  /*!
  \brief Type of pointer to shared nodes
  */
  using node_sptr_t = typename tchecker::graph::reachability::multigraph_t<NODE, EDGE>::node_sptr_t;

  /*!
  \brief Type of pointer to const shared nodes
  */
  using const_node_sptr_t = typename tchecker::graph::reachability::multigraph_t<NODE, EDGE>::const_node_sptr_t;

  /*!
  \brief Type of edges
  */
  using edge_t = EDGE;

  /*!
  \brief Type of pointer to shared edge
  */
  using edge_sptr_t = typename tchecker::graph::reachability::multigraph_t<NODE, EDGE>::edge_sptr_t;

  /*!
  \brief Type of pointer to const shared edge
  */
  using const_edge_sptr_t = typename tchecker::graph::reachability::multigraph_t<NODE, EDGE>::const_edge_sptr_t;

  /*!
   \brief Constructor
   \param args : arguments to a constructor of NODE
   \post this path consists in a single node built from args
  */
  template <class... ARGS>
  finite_path_t(ARGS &&... args) : tchecker::graph::reachability::multigraph_t<NODE, EDGE>(128), _first(nullptr), _last(nullptr)
  {
    _first = _last = tchecker::graph::reachability::multigraph_t<NODE, EDGE>::add_node(args...);
  }

  /*!
   \brief Copy constructor
  */
  finite_path_t(tchecker::graph::finite_path_t<NODE, EDGE> const &) = delete;

  /*!
   \brief Move constructor
  */
  finite_path_t(tchecker::graph::finite_path_t<NODE, EDGE> &&) = delete;

  /*!
   \brief Destructor
  */
  ~finite_path_t() = default;

  /*!
   \brief Assignment operator
  */
  tchecker::graph::finite_path_t<NODE, EDGE> & operator=(tchecker::graph::finite_path_t<NODE, EDGE> const &) = delete;

  /*!
   \brief Move-assignment operator
  */
  tchecker::graph::finite_path_t<NODE, EDGE> & operator=(tchecker::graph::finite_path_t<NODE, EDGE> &&) = delete;

  /*!
   \brief Extend path at end
   \param edge_arg : argument to a constructor of EDGE
   \param node_arg : argument to a constructor of NODE
   \post this path has been extended into first -> ... -> last -e-> n where e has been built from edge_arg and
   n has been built from node_arg
   */
  template <typename EDGE_ARG, typename NODE_ARG> void extend_back(EDGE_ARG const & edge_arg, NODE_ARG const & node_arg)
  {
    node_sptr_t n = tchecker::graph::reachability::multigraph_t<NODE, EDGE>::add_node(node_arg);
    tchecker::graph::reachability::multigraph_t<NODE, EDGE>::add_edge(_last, n, edge_arg);
    _last = n;
  }

  /*!
   \brief Extend path at front
   \param edge_arg : argument to a constructor of EDGE
   \param node_arg : argument to a constructor of NODE
   \post this path has been extended into n -e-> first -> ... -> last where e has been built from edge_arg and
   n has been built from node_arg
   */
  template <typename EDGE_ARG, typename NODE_ARG> void extend_front(EDGE_ARG const & edge_arg, NODE_ARG const & node_arg)
  {
    node_sptr_t n = tchecker::graph::reachability::multigraph_t<NODE, EDGE>::add_node(node_arg);
    tchecker::graph::reachability::multigraph_t<NODE, EDGE>::add_edge(n, _first, edge_arg);
    _first = n;
  }

  /*!
   \brief Accessor
   \return first node in path
  */
  inline node_sptr_t first() const { return _first; }

  /*!
   \brief Accessor
   \return last node in path
  */
  inline node_sptr_t last() const { return _last; }

  /*!
  \brief Type of node iterator
  */
  using const_node_iterator_t = typename tchecker::graph::reachability::multigraph_t<NODE, EDGE>::const_node_iterator_t;

  /*!
   \brief Accessor
   \return the range of nodes in this path
  */
  using tchecker::graph::reachability::multigraph_t<NODE, EDGE>::nodes;

  /*!
   \brief Accessor
   \return number of nodes in this path
   */
  using tchecker::graph::reachability::multigraph_t<NODE, EDGE>::nodes_count;

  /*!
  \brief Type of incoming edges iterator
  */
  using incoming_edges_iterator_t = typename tchecker::graph::reachability::multigraph_t<NODE, EDGE>::incoming_edges_iterator_t;

  /*!
  \brief Type of outgoing edges iterator
  */
  using outgoing_edges_iterator_t = typename tchecker::graph::reachability::multigraph_t<NODE, EDGE>::outgoing_edges_iterator_t;

  /*!
   \brief Accessor
   \param n : node
   \return range of incoming edges of node n, which is empty if n is the first node, or it contains a single edge otherwise
   */
  using tchecker::graph::reachability::multigraph_t<NODE, EDGE>::incoming_edges;

  /*!
   \brief Accessor
   \param n : node
   \pre n is not the first node in this path
   \return the incoming edge to node n
   \throw std::invalid_argument : if n is the first node in this path
  */
  edge_sptr_t incoming_edge(node_sptr_t const & n) const
  {
    if (n == _first)
      throw std::invalid_argument("finite path first node has no incoming edge");
    auto r = incoming_edges(n);
    assert(r.begin() != r.end());
    return *(r.begin());
  }

  /*!
   \brief Accessor
   \param n : node
   \return range of outgoing edges of node n, which is empty if n is the last node, or it contains a single edge otherwise
   */
  using tchecker::graph::reachability::multigraph_t<NODE, EDGE>::outgoing_edges;

  /*!
   \brief Accessor
   \param n : node
   \pre n is not the last node in this path
   \return the outgoing edge of node n
   \throw std::invalid_argument : if n is the last node in this path
  */
  edge_sptr_t outgoing_edge(node_sptr_t const & n) const
  {
    if (n == _last)
      throw std::invalid_argument("finite path last node has no outgoing edge");
    auto r = outgoing_edges(n);
    assert(r.begin() != r.end());
    return *(r.begin());
  }

  /*!
   \brief Accessor
   \param edge : an edge
   \return the source node of edge
   */
  using tchecker::graph::reachability::multigraph_t<NODE, EDGE>::edge_src;

  /*!
   \brief Accessor
   \param edge : an edge
   \return the target node of edge
   */
  using tchecker::graph::reachability::multigraph_t<NODE, EDGE>::edge_tgt;

  using tchecker::graph::reachability::multigraph_t<NODE, EDGE>::attributes;

  /*!
   \class iterator_t
   \brief Bidirectional iterator over the nodes in this path
  */
  class iterator_t {
  public:
    /*!
     \brief Constructor
     \param path : a finite path
     \param node : a node
     \post this iterator points to node
     \note this keeps a pointer to path but does not delete it
    */
    iterator_t(tchecker::graph::finite_path_t<NODE, EDGE> const * path, node_sptr_t const & node)
        : _path(path), _current_node(node)
    {
    }

    /*!
     \brief Copy constructor
    */
    iterator_t(tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t const & it)
        : _path(it._path), _current_node(it._current_node)
    {
    }

    /*!
     \brief Move constructor
    */
    iterator_t(tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t && it)
        : _path(std::move(it._path)), _current_node(std::move(it._current_node))
    {
    }

    /*!
     \brief Destructor
    */
    ~iterator_t() = default; // DO NOT DELETE _path (reference as a pointer to allow assignment)

    /*!
     \brief Assignment operator
    */
    tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t &
    operator=(tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t const & it)
    {
      if (&it != this) {
        _path = it._path;
        _current_node = it._current_node;
      }
      return *this;
    }

    /*!
     \brief Move-assignment operator
    */
    tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t &
    operator=(tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t && it)
    {
      if (&it != this) {
        _path = std::move(it._path);
        _current_node = std::move(it._current_node);
      }
      return *this;
    }

    /*!
     \brief Equality predicate
     \param it : an iterator
     \return true if if and this iterator point to the same node, false otherwise
    */
    bool operator==(tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t const & it) const
    {
      return (_path == it._path) && (_current_node == it._current_node);
    }

    /*!
     \brief Disequality predicate
     \param it : an iterator
     \return true if if and this iterator do not point to the same node, false otherwise
    */
    inline bool operator!=(tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t const & it) const { return !(it == *this); }

    /*!
     \brief Accessor
     \return the node pointed by this iterator
    */
    inline node_sptr_t const & operator*() { return _current_node; }

    /*!
     \brief Increment (next)
     \pre this iterator does not point to nullptr and it does not point to the last node in the path
     \post this iterator points to the next node in the path (if any)
     \return this iterator after increment
     \throw std::runtime_error : if the preconditions are not met
    */
    tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t & operator++()
    {
      if (_current_node.ptr() == nullptr)
        throw std::runtime_error("finite path iterator is out of range");
      auto r = _path->outgoing_edges(_current_node);
      if (r.empty())
        _current_node = nullptr;
      else {
        auto edge = *(r.begin());
        _current_node = _path->edge_tgt(edge);
      }
      return *this;
    }

    /*!
     \brief Decrement (previous)
     \pre this iterator does not point to nullptr and it does not point to the first node in the path
     \post this iterator points to the previous node in the path (if any)
     \return this iterator after decrement
     \throw std::runtime_error : if the preconditions are not met
    */
    tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t & operator--()
    {
      if (_current_node.ptr() == nullptr)
        throw std::runtime_error("finite path iterator is out of range");
      auto r = _path->incoming_edges(_current_node);
      if (r.empty())
        _current_node = nullptr;
      else {
        auto edge = *(r.begin());
        _current_node = _path->edge_src(edge);
      }
      return *this;
    }

  private:
    tchecker::graph::finite_path_t<NODE, EDGE> const * _path; /*!< Path */
    node_sptr_t _current_node;                                /*!< Pointer to current node in path */
  };

  /*!
   \brief Accessor
   \return Iterator to first node in this path
  */
  tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t begin()
  {
    return tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t(this, _first);
  }

  /*!
   \brief Accessor
   \return Past-the-end iterator to first node in this path
  */
  tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t end()
  {
    return tchecker::graph::finite_path_t<NODE, EDGE>::iterator_t(this, nullptr);
  }

  /*!
   \class reverse_iterator_t
   \brief Bidirectional reverse iterator over the nodes in this path
  */
  class reverse_iterator_t {
  public:
    /*!
     \brief Constructor
     \param path : a path
     \param node : a node
     \post this iterator points to node
     \note this keeps a pointer to path but does not delete it
    */
    reverse_iterator_t(tchecker::graph::finite_path_t<NODE, EDGE> const * path, node_sptr_t const & node)
        : _path(path), _current_node(node)
    {
    }

    /*!
     \brief Copy constructor
    */
    reverse_iterator_t(tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t const & it)
        : _path(it._path), _current_node(it._current_node)
    {
    }

    /*!
     \brief Move constructor
    */
    reverse_iterator_t(tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t && it)
        : _path(std::move(it._path)), _current_node(std::move(it._current_node))
    {
    }

    /*!
     \brief Destructor
    */
    ~reverse_iterator_t() = default; // DO NOT DELETE _path (reference as a pointer to allow assignment)

    /*!
     \brief Assignment operator
    */
    tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t &
    operator=(tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t const & it)
    {
      if (&it != this) {
        _path = it._path;
        _current_node = it._current_node;
      }
      return *this;
    }

    /*!
     \brief Move-assignment operator
    */
    tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t &
    operator=(tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t && it)
    {
      if (&it != this) {
        _path = std::move(it._path);
        _current_node = std::move(it._current_node);
      }
      return *this;
    }

    /*!
     \brief Equality predicate
     \param it : an iterator
     \return true if if and this iterator point to the same node, false otherwise
    */
    bool operator==(tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t const & it) const
    {
      return (_path == it._path) && (_current_node == it._current_node);
    }

    /*!
     \brief Disequality predicate
     \param it : an iterator
     \return true if if and this iterator do not point to the same node, false otherwise
    */
    inline bool operator!=(tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t const & it) const
    {
      return !(it == *this);
    }

    /*!
     \brief Accessor
     \return the node pointed by this iterator
    */
    inline node_sptr_t const & operator*() { return _current_node; }

    /*!
     \brief Increment (next in reverse = previous node in path)
     \pre this iterator does not point to nullptr
     \post this iterator points to the previous node in the path (if any)
     \return this iterator after decrement
     \throw std::runtime_error : if the preconditions are not met
    */
    tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t & operator++()
    {
      if (_current_node.ptr() == nullptr)
        throw std::runtime_error("finite path iterator is out of range");
      auto r = _path->incoming_edges(_current_node);
      if (r.empty())
        _current_node = nullptr;
      else {
        auto edge = *(r.begin());
        _current_node = _path->edge_src(edge);
      }
      return *this;
    }

    /*!
     \brief Decrement (previous node in reverse = next node in path)
     \pre this iterator does not point to nullptr
     \post this iterator points to the next node in the path (if any)
     \return this iterator after increment
     \throw std::runtime_error : if the preconditions are not met
    */
    tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t & operator--()
    {
      if (_current_node.ptr() == nullptr)
        throw std::runtime_error("finite path iterator is out of range");
      auto r = _path->outgoing_edges(_current_node);
      if (r.empty())
        _current_node = nullptr;
      else {
        auto edge = *(r.begin());
        _current_node = _path->edge_tgt(edge);
      }
      return *this;
    }

  private:
    tchecker::graph::finite_path_t<NODE, EDGE> const * _path; /*!< Path */
    node_sptr_t _current_node;                                /*!< Pointer to current node in path */
  };

  /*!
   \brief Accessor
   \return Iterator to first node in this path
  */
  tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t rbegin()
  {
    return tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t(this, _last);
  }

  /*!
   \brief Accessor
   \return Past-the-end iterator to first node in this path
  */
  tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t rend()
  {
    return tchecker::graph::finite_path_t<NODE, EDGE>::reverse_iterator_t(this, nullptr);
  }

private:
  // tchecker::graph::reachability::multigraph_t<NODE, EDGE> _graph; /*!< Graph representation of path */
  node_sptr_t _first; /*!< First node */
  node_sptr_t _last;  /*!< Last node */
};

} // namespace graph

} // namespace tchecker

#endif // TCHECKER_GRAPH_PATH_HH