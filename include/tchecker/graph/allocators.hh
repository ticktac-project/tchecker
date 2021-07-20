/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GRAPH_ALLOCATORS_HH
#define TCHECKER_GRAPH_ALLOCATORS_HH

#include <tuple>

#include "tchecker/utils/pool.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file allocators.hh
 \brief Allocators for graphs
 */

namespace tchecker {

namespace graph {

/*!
 \class node_pool_allocator_t
 \brief Pool allocator of nodes
 \tparam NODE : type of nodes, should inherit from tchecker::make_shared_t<N>
 for some N, and tchecker::allocation_size_t<N> should be defined
*/
template <class NODE> class node_pool_allocator_t {
public:
  /*!
   \brief Type of allocated nodes
   */
  using node_t = NODE;

  /*!
   \brief Type of shared pointer to nodes
   */
  using node_sptr_t = typename tchecker::intrusive_shared_ptr_t<NODE>;

  /*!
   \brief Constructor
   \param alloc_nb : number of nodes allocated in one block
   */
  node_pool_allocator_t(std::size_t alloc_nb) : _node_pool(alloc_nb, tchecker::allocation_size_t<NODE>::alloc_size()) {}

  /*!
   \brief Copy constructor (deleted)
   */
  node_pool_allocator_t(tchecker::graph::node_pool_allocator_t<NODE> const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  node_pool_allocator_t(tchecker::graph::node_pool_allocator_t<NODE> &&) = delete;

  /*!
   \brief Destructor
   */
  ~node_pool_allocator_t() { destruct_all(); }

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::graph::node_pool_allocator_t<NODE> & operator=(tchecker::graph::node_pool_allocator_t<NODE> const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::graph::node_pool_allocator_t<NODE> & operator=(tchecker::graph::node_pool_allocator_t<NODE> &&) = delete;

  /*!
   \brief Construct node
   \param args : arguments to a constructor of NODE
   \return a new instance of NODE constructed from args
   */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<NODE> construct(ARGS &&... args)
  {
    return _node_pool.construct(std::forward<ARGS>(args)...);
  }

  /*!
   \brief Destruct node
   \param p : pointer to a node
   \pre p has been constructed by this allocator
   p is not nullptr
   \post the object pointed by p has been destructed if its reference counter is 1
   (i.e. p is the only pointer to the object), and p points to nullptr. Does nothing
   otherwise
   \return true if the node has been destructed, false otherwise
   */
  bool destruct(tchecker::intrusive_shared_ptr_t<NODE> & p) { return _node_pool.destruct(p); }

  /*!
   \brief Collect unused nodes
   \post Unused nodes have been deleted
   */
  void collect() { _node_pool.collect(); }

  /*!
   \brief Destruct all allocated nodes
   \post All allocated nodes have been destructed
   */
  void destruct_all() { _node_pool.destruct_all(); }

  /*!
   \brief Accessor
   \return Memory used by this node allocator
   */
  std::size_t memsize() const { return _node_pool.memsize(); }

protected:
  tchecker::pool_t<NODE> _node_pool; /*!< Pool of nodes */
};

/*!
 \class edge_pool_allocator_t
 \brief Pool allocator of edges
 \tparam EDGE : type of edges, should inherit from tchecker::make_shared_t<E>
 for some E, and tchecker::allocation_size_t<E> should be defined
 */
template <class EDGE> class edge_pool_allocator_t {
public:
  /*!
   \brief Type of allocated edges
   */
  using edge_t = EDGE;

  /*!
   \brief Type of shard pointer to edge
   */
  using edge_sptr_t = typename tchecker::intrusive_shared_ptr_t<EDGE>;

  /*!
   \brief Constructor
   \param alloc_nb : number of edges allocated in one block
   */
  edge_pool_allocator_t(std::size_t alloc_nb) : _edge_pool(alloc_nb, tchecker::allocation_size_t<EDGE>::alloc_size()) {}

  /*!
   \brief Copy constructor (deleted)
   */
  edge_pool_allocator_t(tchecker::graph::edge_pool_allocator_t<EDGE> const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  edge_pool_allocator_t(tchecker::graph::edge_pool_allocator_t<EDGE> &&) = delete;

  /*!
   \brief Destructor
   */
  ~edge_pool_allocator_t() { destruct_all(); }

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::graph::edge_pool_allocator_t<EDGE> & operator=(tchecker::graph::edge_pool_allocator_t<EDGE> const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::graph::edge_pool_allocator_t<EDGE> & operator=(tchecker::graph::edge_pool_allocator_t<EDGE> &&) = delete;

  /*!
   \brief Construct edge
   \param args : arguments to a constructor of EDGE
   \return a new instance of EDGE constructed from args
   */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<EDGE> construct(ARGS &&... args)
  {
    return _edge_pool.construct(std::forward<ARGS>(args)...);
  }

  /*!
   \brief Destruct edge
   \param p : pointer to an edge
   \pre p has been constructed by this allocator
   p is not nullptr
   \post the object pointed by p has been destructed if its reference counter is 1
   (i.e. p is the only pointer to the object), and p points to nullptr. Does nothing otherwise
   \return true if the edge has been destructed, false otherwise
   */
  bool destruct(tchecker::intrusive_shared_ptr_t<EDGE> & p) { return _edge_pool.destruct(p); }

  /*!
   \brief Collect unused edges
   \post Unused edges have been deleted
   */
  void collect() { _edge_pool.collect(); }

  /*!
   \brief Destruct all allocated edges
   \post All allocated edges have been destructed
   */
  void destruct_all() { _edge_pool.destruct_all(); }

  /*!
   \brief Accessor
   \return Memory used by this edge allocator
   */
  std::size_t memsize() const { return _edge_pool.memsize(); }

protected:
  tchecker::pool_t<EDGE> _edge_pool; /*!< Pool of edges */
};

} // end of namespace graph

} // end of namespace tchecker

#endif // TCHECKER_GRAPH_ALLOCATORS_HH
