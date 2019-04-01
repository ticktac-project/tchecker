/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GRAPH_ALLOCATORS_HH
#define TCHECKER_GRAPH_ALLOCATORS_HH

#include <tuple>

#include "tchecker/utils/gc.hh"

/*!
 \file allocators.hh
 \brief Allocators for graphs
 */

namespace tchecker {
  
  namespace graph {
    
    /*!
     \class graph_allocator_t
     \brief Garbage-collected allocator of graph
     \tparam NODE_ALLOCATOR : type of node allocator, should provide destruct_all() method
     \tparam EDGE_ALLOCATOR : type of edge allocator, should provide destruct_all() method
     */
    template <class NODE_ALLOCATOR, class EDGE_ALLOCATOR>
    class graph_allocator_t {
    public:
      /*!
       \brief Type of nodes
       */
      using node_t = typename NODE_ALLOCATOR::t;
      
      /*!
       \brief Type of pointer to node
       */
      using node_ptr_t = typename NODE_ALLOCATOR::ptr_t;
      
      /*!
       \brief Type of node allocator
       */
      using node_allocator_t = NODE_ALLOCATOR;
      
      /*!
       \brief Type of edges
       */
      using edge_t = typename EDGE_ALLOCATOR::t;
      
      /*!
       \brief Type of pointer to edge
       */
      using edge_ptr_t = typename EDGE_ALLOCATOR::ptr_t;
      
      /*!
       \brief Type of edge allocator
       */
      using edge_allocator_t = EDGE_ALLOCATOR;
      
      /*!
       \brief Constructor
       \param gc : garbage collector
       \param na_args : parameters to a constructor of NODE_ALLOCATOR
       \param ea_args : parameters to a constructor of EDGE_ALLOCATOR
       \post this has built a node allocator and an edge allocator, and have
       enrolled to gc
       */
      template <class ... NA_ARGS, class ... EA_ARGS>
      graph_allocator_t(tchecker::gc_t & gc,
                        std::tuple<NA_ARGS...> && na_args,
                        std::tuple<EA_ARGS...> && ea_args)
      : _node_allocator(std::make_from_tuple<NODE_ALLOCATOR>(na_args)),
      _edge_allocator(std::make_from_tuple<EDGE_ALLOCATOR>(ea_args))
      {
        _node_allocator.enroll(gc);
        _edge_allocator.enroll(gc);
      }
      
      /*!
       \brief Constructor
       \param args : tuple of arguments to a constructor of tchecker::graph::graph_allocator_t
       \post see other constructors
       */
      template <class ... NA_ARGS, class ... EA_ARGS>
      graph_allocator_t(std::tuple<tchecker::gc_t &, std::tuple<NA_ARGS...>, std::tuple<EA_ARGS...>> && args)
      : graph_allocator_t(std::get<0>(args),
      std::forward<std::tuple<NA_ARGS...>>(std::get<1>(args)),
                          std::forward<std::tuple<EA_ARGS...>>(std::get<2>(args)))
      {}
      
      /*!
       \brief Copy constructor (deleted)
       */
      graph_allocator_t(tchecker::graph::graph_allocator_t<NODE_ALLOCATOR, EDGE_ALLOCATOR> const &) = delete;
      
      /*!
       \brief Move constructor (deleted)
       */
      graph_allocator_t(tchecker::graph::graph_allocator_t<NODE_ALLOCATOR, EDGE_ALLOCATOR> &&) = delete;
      
      /*!
       \brief Destructor
       \note see destruct_all()
       */
      ~graph_allocator_t() = default;
      
      /*!
       \brief Assignment operator (deleted)
       */
      tchecker::graph::graph_allocator_t<NODE_ALLOCATOR, EDGE_ALLOCATOR> &
      operator= (tchecker::graph::graph_allocator_t<NODE_ALLOCATOR, EDGE_ALLOCATOR> const &) = delete;
      
      /*!
       \brief Move-assignment operator (deleted)
       */
      tchecker::graph::graph_allocator_t<NODE_ALLOCATOR, EDGE_ALLOCATOR> &
      operator= (tchecker::graph::graph_allocator_t<NODE_ALLOCATOR, EDGE_ALLOCATOR> &&) = delete;
      
      /*!
       \brief Nodes and edges destruction
       \post all allocated nodes and edges have been deleted. All pointers
       returned by methods allocate_node(), allocate_from_node() and
       allocate_edge() have been invalidated
       */
      void destruct_all()
      {
        _node_allocator.destruct_all();
        _edge_allocator.destruct_all();
      }
      
      /*!
       \brief Fast memory deallocation
       \post all allocated nodes and edges have been freed. No destructor
       has been called. All pointers returned by methods allocate_node(),
       allocate_from_node() and allocate_edge() have been invalidated
       */
      void free_all()
      {
        _node_allocator.free_all();
        _edge_allocator.free_all();
      }
      
      /*!
       \brief Node allocation
       \param nargs : parameters to a constructor of node_t
       \return pointer to a newly allocated node constructed from nargs
       */
      template <class ... NARGS>
      inline node_ptr_t allocate_node(NARGS && ... nargs)
      {
        return _allocator_node(nargs...);
      }
      
      /*!
       \brief Node allocation
       \param nargs : tuple of parameters to a constructor of node_t
       \return pointer to a newly allocated node constructed from nargs
       */
      template <class ... NARGS>
      inline node_ptr_t allocate_node(std::tuple<NARGS...> && nargs)
      {
        return std::apply(&tchecker::graph::graph_allocator_t<NODE_ALLOCATOR, EDGE_ALLOCATOR>::_allocate_node<NARGS...>,
                          std::tuple_cat(std::make_tuple(this), nargs));
      }
      
      /*!
       \brief Node allocation
       \param node : a node
       \param nargs : parameters to a constructor of node_t beyond node
       \return pointer to a newly allocated node constructed from node and nargs
       \note nodes are usually made of a state part and a node part. The state part is built
       from cloning node, while the node part is built from nargs
       */
      template <class ... NARGS>
      inline node_ptr_t allocate_from_node(node_ptr_t const & node, NARGS && ... nargs)
      {
        return _allocate_from_node(node, nargs...);
      }
      
      /*!
       \brief Node allocation
       \param node : a node
       \param nargs : tuple of parameters to a constructor of node_t beyond node
       \return pointer to a newly allocated node constructed from node and nargs
       \note nodes are usually made of a state part and a node part. The state part is built
       from cloning node, while the node part is built from nargs
       */
      template <class ... NARGS>
      inline node_ptr_t allocate_from_node(node_ptr_t const & node, std::tuple<NARGS...> && nargs)
      {
        return std::apply(&tchecker::graph::graph_allocator_t<NODE_ALLOCATOR, EDGE_ALLOCATOR>::_allocate_from_node<NARGS...>,
                          std::tuple_cat(std::tuple<decltype(this), node_ptr_t const &>(this, node), nargs));
      }
      
      /*!
       \brief Edge allocation
       \param eargs : parameters to a constructor of edge_t
       \return pointer to a newly allocated edge constructed from eargs
       */
      template <class ... EARGS>
      inline edge_ptr_t allocate_edge(EARGS && ... eargs)
      {
        return _allocate_edge(eargs...);
      }
      
      /*!
       \brief Edge allocation
       \param eargs : tuple of parameters to a constructor of edge_t
       \return pointer to a newly allocated edge constructed from eargs
       */
      template <class ... EARGS>
      inline edge_ptr_t allocate_edge(std::tuple<EARGS...> && eargs)
      {
        return std::apply(&tchecker::graph::graph_allocator_t<NODE_ALLOCATOR, EDGE_ALLOCATOR>::_allocate_edge<EARGS...>,
                          std::tuple_cat(std::make_tuple(this), eargs));
      }
    protected:
      /*!
       \brief Node allocation
       \param nargs : parameters to a constructor of node_t
       \return pointer to a newly allocated node constructed from nargs
       */
      template <class ... NARGS>
      inline node_ptr_t _allocate_node(NARGS && ... nargs)
      {
        return _node_allocator.construct(nargs...);
      }
      
      /*!
       \brief Node allocation
       \param node : a node
       \param nargs : parameters to a constructor of node_t beyond node
       \return pointer to a newly allocated node constructed from node and nargs
       \note nodes are usually made of a state part and a node part. The state part is built
       from cloning node, while the node part is built from nargs
       */
      template <class ... NARGS>
      inline node_ptr_t _allocate_from_node(node_ptr_t const & node, NARGS && ... nargs)
      {
        return _node_allocator.construct_from_state(*node, nargs...);
      }
      
      /*!
       \brief Edge allocation
       \param eargs : parameters to a constructor of edge_t
       \return pointer to a newly allocated edge constructed from eargs
       */
      template <class ... EARGS>
      inline edge_ptr_t _allocate_edge(EARGS && ... eargs)
      {
        return _edge_allocator.construct(eargs...);
      }
      
      NODE_ALLOCATOR _node_allocator;   /*!< Nodes allocator */
      EDGE_ALLOCATOR _edge_allocator;   /*!< Edges allocator */
    };
    
  } // end of namespace graph
  
} // end of namespace tchecker

#endif // TCHECKER_GRAPH_ALLOCATORS_HH
