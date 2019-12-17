/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_EXPLORE_GRAPH_HH
#define TCHECKER_ALGORITHMS_EXPLORE_GRAPH_HH

#include <string>
#include <tuple>

#include "tchecker/graph/find_graph.hh"
#include "tchecker/graph/output.hh"
#include "tchecker/graph/tracing_graph.hh"
#include "tchecker/utils/allocation_size.hh"

/*!
 \file graph.hh
 \brief Graph for explore algorithm
 */

namespace tchecker {
  
  namespace explore {
    
    /*!
     \class node_t
     \brief Node for explore algorithm
     \tparam STATE : type of state in underlying transition system
     */
    template <class STATE>
    class node_t : public tchecker::graph::node_with_id_t, public STATE {
    public:
      /*!
       \brief Constructor
       \param id : node identifier
       \param sargs : parameters to a constructor of class STATE
       \pre id should uniquely identify this node
       \post this node has been built from sargs, with identifier id
       */
      template <class ... SARGS>
      node_t(tchecker::node_id_t id, SARGS && ... sargs)
      : tchecker::graph::node_with_id_t(id), STATE(std::forward<SARGS>(sargs)...)
      {}

      /*!
	\brief Constructor
	\param node : a node
	\param id : node identifier
	\param sargs : parameters to a constructor of class STATE
	\pre id should uniquely identify this node
	\post this node has been built from sargs, with identifier id
      */
      template <class ... SARGS>
      node_t(tchecker::explore::node_t<STATE> const & node, tchecker::node_id_t id,
	     SARGS && ... sargs)
	: tchecker::graph::node_with_id_t(id), STATE(node, std::forward<SARGS>(sargs)...)
      {}
      
      /*!
       \brief Copy constructor
       \param node : a node
       \post this is a copy of node
       \note this gets same identifier as node
       */
      node_t(tchecker::explore::node_t<STATE> const & node) = default;
      
      /*!
       \brief Move constructor
       \param node : a node
       \post node has been moved to this
       \note this gets same identifier as node
       */
      node_t(tchecker::explore::node_t<STATE> && node) = default;
      
      /*!
       \brief Destructor
       */
      ~node_t() = default;
      
      /*!
       \brief Assignment operator
       \param node : a node
       \post this is a copy of node
       \return this after assignment
       \note this has same identifier as node
       */
      tchecker::explore::node_t<STATE> & operator= (tchecker::explore::node_t<STATE> const & node) = default;
      
      /*!
       \brief Move-assignment operator
       \param node : a node
       \post this is a copy of node
       \return this after assignment
       \note this has same identifier as node
       */
      tchecker::explore::node_t<STATE> & operator= (tchecker::explore::node_t<STATE> && node) = default;
    };
    
    
    
    
    /*!
     \brief Edge for explore algorithm
     */
    template <class TRANSITION>
    using edge_t = TRANSITION;
    
    
    
    
    /*!
     \class graph_t
     \brief Graph for explore algorithm
     \tparam ALLOCATOR : type of graph allocator. Should derive from tchecker::graph::graph_allocator_t.
     Should allocate nodes that derive from tchecker::explore::node_t. The node allocator should not be a
     singleton allocator since this graph stores nodes. Should allocate edges that derive from
     tchecker::explore::edge_t
     \tparam HASH : type of hash function on nodes (see tchecker::graph::find_graph_t)
     \tparam EQUAL_TO : type of equality predicate on nodes (see tchecker::graph::find_graph_t)
     \tparam OUTPUTTER : type of graph outputter
     \note This graph stores nodes, but not edges. Nodes and edges are output when added to the graph.
     */
    template <class ALLOCATOR, class HASH, class EQUAL_TO, class OUTPUTTER>
    class graph_t
    : public ALLOCATOR,
    public tchecker::graph::find_graph_t<typename ALLOCATOR::node_ptr_t, HASH, EQUAL_TO>,
    public tchecker::graph::tracing_graph_t<typename ALLOCATOR::node_ptr_t, typename ALLOCATOR::edge_ptr_t, OUTPUTTER>
    {
    public:
      /*!
       \brief Type of nodes
       */
      using node_t = typename ALLOCATOR::node_t;
      
      /*!
       \brief Type of pointer to node
       */
      using node_ptr_t = typename ALLOCATOR::node_ptr_t;
      
      /*!
       \brief Type of edges
       */
      using edge_t = typename ALLOCATOR::edge_t;
      
      /*!
       \brief Type of pointer to edge
       */
      using edge_ptr_t = typename ALLOCATOR::edge_ptr_t;
      
      /*!
       \brief Constructor
       \param name : graph name
       \param a_args : tuple of parameters to a constructor of class ALLOCATOR
       \param go_args : parameters to a constructor of class GRAPH_OUPUTTER
       */
      template <class ... A_ARGS, class ... O_ARGS>
      graph_t(std::string const & name, std::tuple<A_ARGS...> && a_args, O_ARGS && ... o_args)
      : ALLOCATOR(std::forward<std::tuple<A_ARGS...>>(a_args)),
      tchecker::graph::tracing_graph_t<typename ALLOCATOR::node_ptr_t, typename ALLOCATOR::edge_ptr_t, OUTPUTTER>
      (name, std::forward<O_ARGS>(o_args)...)
      {}
      
      /*!
       \brief Copy constructor (deleted)
       */
      graph_t(tchecker::explore::graph_t<ALLOCATOR, HASH, EQUAL_TO, OUTPUTTER> const &) = delete;
      
      /*!
       \brief Move constructor (deleted)
       */
      graph_t(tchecker::explore::graph_t<ALLOCATOR, HASH, EQUAL_TO, OUTPUTTER> &&) = delete;
      
      /*!
       \brief Destructor
       \post All the nodes and edges allocated by the graph have been freed and invaidated
       */
      ~graph_t()
      {
        destruct_all();
      }
      
      /*!
       \brief Assignment operator (deleted)
       */
      tchecker::explore::graph_t<ALLOCATOR, HASH, EQUAL_TO, OUTPUTTER> &
      operator= (tchecker::explore::graph_t<ALLOCATOR, HASH, EQUAL_TO, OUTPUTTER> const &) = delete;
      
      /*!
       \brief Move-assignment operator (deleted)
       */
      tchecker::explore::graph_t<ALLOCATOR, HASH, EQUAL_TO, OUTPUTTER> &
      operator= (tchecker::explore::graph_t<ALLOCATOR, HASH, EQUAL_TO, OUTPUTTER> &&) = delete;
      
      /*!
       \brief Destruct all the nodes and edges allocated by the graph
       */
      void destruct_all()
      {
        tchecker::graph::find_graph_t<typename ALLOCATOR::node_ptr_t, HASH, EQUAL_TO>::clear();
        ALLOCATOR::destruct_all();
      }
      
      /*!
       \brief Free all the nodes and edges allocated by the graph. No destructor called.
       */
      void free_all()
      {
        tchecker::graph::find_graph_t<typename ALLOCATOR::node_ptr_t, HASH, EQUAL_TO>::clear();
        ALLOCATOR::free_all();
      }
      
      /*!
       \brief Add a node
       \param node_ptr : a node
       \return true if the node has been added to the graph, false otherwise (if a node similar w.r.t. HASH
       and EQUAL_TO already exists)
       \post node has been output using GRAPH_OUTPUTTER if it has been added to the graph
       */
      bool add_node(node_ptr_t & node_ptr)
      {
        if (! tchecker::graph::find_graph_t<typename ALLOCATOR::node_ptr_t, HASH, EQUAL_TO>::add_node(node_ptr))
          return false;
        return tchecker::graph::tracing_graph_t<node_ptr_t, edge_ptr_t, OUTPUTTER>::add_node(node_ptr);
      }
      
      /*!
       \brief Add an edge
       \param src_ptr : source node
       \param tgt_ptr : target node
       \param edge_ptr : an edge
       \post the edge has been output using GRAPH_OUPUTTER
       */
      void add_edge(node_ptr_t & src_ptr, node_ptr_t & tgt_ptr, edge_ptr_t & edge_ptr)
      {
        tchecker::graph::tracing_graph_t<node_ptr_t, edge_ptr_t, OUTPUTTER>::add_edge(src_ptr, tgt_ptr, edge_ptr);
      }
    };
    
  } // end of namespace explore
  
  
  
  
  /*!
   \brief Specialization of class allocation_size_t for tchecker::explore::node_t
   */
  template <class STATE>
  class allocation_size_t<tchecker::explore::node_t<STATE>> {
  public:
    template <class ... ARGS>
    static constexpr std::size_t alloc_size(ARGS && ... args)
    {
      return sizeof(tchecker::explore::node_t<STATE>);
    }
  };
  
} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_EXPLORE_GRAPH_HH
