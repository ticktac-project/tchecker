/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COVREACH_GRAPH_HH
#define TCHECKER_ALGORITHMS_COVREACH_GRAPH_HH

#include <cassert>
#include <tuple>
#include <vector>

#include "tchecker/algorithms/covreach/waiting.hh"
#include "tchecker/graph/directed_graph.hh"
#include "tchecker/graph/cover_graph.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/gc.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/utils/pool.hh"

/*!
 \file graph.hh
 \brief Graph for covreach algorithm
 */

namespace tchecker {
  
  namespace covreach {
    
    /*!
     \class node_t
     \brief Node for covreach graph
     \tparam STATE : type of state in transition system
     \tparam EDGE_PTR : type of pointers to edge, *EDGE_PTR shoud derive from tchecker::covreach::edge_t
     \note nodes are states of a transition system extended with the capacity to store edges of a directed
     graph, a protected/unprotected flag (protected nodes should not be removed from the graph) and
     an active/inactive flag (that allows to remove nodes lazily)
     */
    template <class STATE, class EDGE_PTR>
    class node_t
    : public STATE,
    public tchecker::graph::cover::node_t,
    public tchecker::graph::directed::node_t<EDGE_PTR>,
    public tchecker::covreach::details::active_node_t
    {
    public:
      /*!
       \brief Constructor
       \param sargs : parameters to a constructor of class STATE
       \post this node has been built from sargs and is active and unprotected
       */
      template <class ... SARGS>
      explicit node_t(SARGS && ... sargs) : STATE(std::forward<SARGS>(sargs)...), _protected(0)
      {}
      
      /*!
       \brief Copy constructor
       */
      node_t(tchecker::covreach::node_t<STATE, EDGE_PTR> const &) = default;
      
      /*!
       \brief Move constructor
       */
      node_t(tchecker::covreach::node_t<STATE, EDGE_PTR> &&) = default;
      
      /*!
       \brief Destructor
       */
      ~node_t() = default;
      
      /*!
       \brief Assignment operator
       */
      tchecker::covreach::node_t<STATE, EDGE_PTR> & operator= (tchecker::covreach::node_t<STATE, EDGE_PTR> const &) = default;
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::covreach::node_t<STATE, EDGE_PTR> & operator= (tchecker::covreach::node_t<STATE, EDGE_PTR> &&) = default;
      
      /*!
       \brief Accessor
       \return true if this node is protected, false
       */
      bool is_protected() const
      {
        return (_protected == 1);
      }
      
      /*!
       \brief Make protected
       \post this node is protected
       */
      void make_protected()
      {
        _protected = 1;
      }
      
      /*!
       \brief Accessor
       \return node identifier
       */
      std::size_t identifier() const
      {
        return reinterpret_cast<std::size_t>(this);
      }

    protected:
      unsigned char _protected : 1;   /*!< Protected node flag */
    };
    
  } // end of namespace covreach
  
  
  
  
  /*!
   \brief Specialization of class allocation_size_t for tchecker::covreach::node_t
   */
  template <class STATE, class EDGE_PTR>
  class allocation_size_t<tchecker::covreach::node_t<STATE, EDGE_PTR>> {
  public:
    template <class ... ARGS>
    static constexpr std::size_t alloc_size(ARGS && ... args)
    {
      return sizeof(tchecker::covreach::node_t<STATE, EDGE_PTR>);
    }
  };
  
  
  
  
  namespace covreach {
    
    /*!
     \brief Type of edges
     */
    enum edge_type_t {
      ACTUAL_EDGE,
      ABSTRACT_EDGE,
    };
    
    
    
    
    /*!
     \class edge_t
     \brief Edge for covreach graph
     \tparam NODE_PTR : type of pointers to node, *NODE_PTR should derive from tchecker::covreach::node_t
     \tparam EDGE_PTR : type of pointers to edge, *EDGE_PTR should derive from tchecker::covreach::edge_t
     */
    template <class NODE_PTR, class EDGE_PTR>
    class edge_t : public tchecker::graph::directed::edge_t<NODE_PTR, EDGE_PTR> {
    public:
      /*!
       \brief Constructor
       \param edge_type : type of edge
       \post this edge has type edge_type and does not point any node
       */
      explicit edge_t(enum tchecker::covreach::edge_type_t edge_type = tchecker::covreach::ACTUAL_EDGE)
      : _edge_type(edge_type)
      {}
      
      /*!
       \brief Copy constructor
       */
      edge_t(tchecker::covreach::edge_t<NODE_PTR, EDGE_PTR> const &) = default;
      
      /*!
       \brief Move constructor
       */
      edge_t(tchecker::covreach::edge_t<NODE_PTR, EDGE_PTR> &&) = default;
      
      /*!
       \brief Destructor
       */
      ~edge_t() = default;
      
      /*!
       \brief Assignment operator
       */
      tchecker::covreach::edge_t<NODE_PTR, EDGE_PTR> &
      operator= (tchecker::covreach::edge_t<NODE_PTR, EDGE_PTR> const &) = default;
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::covreach::edge_t<NODE_PTR, EDGE_PTR> & operator= (tchecker::covreach::edge_t<NODE_PTR, EDGE_PTR> &&) = default;
      
      /*!
       \brief Accessor
       \return edge type
       */
      inline enum tchecker::covreach::edge_type_t edge_type() const
      {
        return _edge_type;
      }
      
      /*!
       \brief Setter
       \pre edge_type : an edge type
       \post the type of this edge has been set to edge_type
       */
      inline void set_type(enum tchecker::covreach::edge_type_t edge_type)
      {
        _edge_type = edge_type;
      }
    private:
      edge_type_t _edge_type;  /*!< Type of edge */
    };
    
  } // end of namespace covreach
  
  
  
  
  /*!
   \brief Specialization of class allocation_size_t for tchecker::covreach::edge_t
   */
  template <class NODE_PTR, class EDGE_PTR>
  class allocation_size_t<tchecker::covreach::edge_t<NODE_PTR, EDGE_PTR>> {
  public:
    template <class ... ARGS>
    static constexpr std::size_t alloc_size(ARGS && ... args)
    {
      return sizeof(tchecker::covreach::edge_t<NODE_PTR, EDGE_PTR>);
    }
  };
  
  
  
  
  namespace covreach {
    
    namespace details {
      
      // Forward declaration
      template <class STATE> class node_impl_t;
      template <class STATE> class edge_impl_t;
      
      /*!
       \brief Type of (shared) nodes
       */
      template <class STATE> using node_t = tchecker::make_shared_t<node_impl_t<STATE>>;
      
      /*!
       \brief Type of pointers to node
       */
      template <class STATE> using node_ptr_t = tchecker::intrusive_shared_ptr_t<node_t<STATE>>;
      
      /*!
       \brief Type of (shared) edge
       */
      template <class STATE>
      using edge_t = tchecker::make_shared_t<edge_impl_t<STATE>>;
      
      /*!
       \brief Type of pointers to edge
       */
      template <class STATE>
      using edge_ptr_t = tchecker::intrusive_shared_ptr_t<edge_t<STATE>>;
      
      /*!
       \brief Type of nodes
       */
      template <class STATE>
      class node_impl_t : public tchecker::covreach::node_t<STATE, edge_ptr_t<STATE>> {
      public:
        using tchecker::covreach::node_t<STATE, edge_ptr_t<STATE>>::node_t;
      };
      
      /*!
       \brief Type of edges
       */
      template <class STATE>
      class edge_impl_t : public tchecker::covreach::edge_t<node_ptr_t<STATE>, edge_ptr_t<STATE>> {
      public:
        using tchecker::covreach::edge_t<node_ptr_t<STATE>, edge_ptr_t<STATE>>::edge_t;
      };
      
      
      
      
      /*!
       \class graph_types_t
       \brief Define types of nodes and edges from TS
       \tparam TS : type of transition system (see tchecker::ts::ts_t)
       */
      template <class TS>
      class graph_types_t {
      public:
        /*!
         \brief Type of (shared) nodes
         */
        using node_t = tchecker::covreach::details::node_t<typename TS::state_t>;
        
        /*!
         \brief Type of pointers to node
         */
        using node_ptr_t = tchecker::covreach::details::node_ptr_t<typename TS::state_t>;
        
        /*!
         \brief Type of (shared) edge
         */
        using edge_t = tchecker::covreach::details::edge_t<typename TS::state_t>;
        
        /*!
         \brief Type of pointers to edge
         */
        using edge_ptr_t = tchecker::covreach::details::edge_ptr_t<typename TS::state_t>;
      };
      
    } // end of namespace details
    
  } // end of namespace covreach
  
  
  
  
  /*!
   \brief Specialization of class allocation_size_t for tchecker::covreach::details::graph_types_t::node_impl_t
   */
  template <class STATE>
  class allocation_size_t<tchecker::covreach::details::node_impl_t<STATE>> {
  public:
    template <class ... ARGS>
    static constexpr std::size_t alloc_size(ARGS && ... args)
    {
      return sizeof(typename tchecker::covreach::details::node_impl_t<STATE>);
    }
  };
  
  
  
  
  /*!
   \brief Specialization of class allocation_size_t for tchecker::covreach::details::graph_types_t::edge_impl_t
   */
  template <class STATE>
  class allocation_size_t<tchecker::covreach::details::edge_impl_t<STATE>> {
  public:
    template <class ... ARGS>
    static constexpr std::size_t alloc_size(ARGS && ... args)
    {
      return sizeof(tchecker::covreach::details::edge_impl_t<STATE>);
    }
  };
  
  
  
  
  namespace covreach {
    
    /*!
     \class graph_t
     \brief Graph for covreach algorithm
     \tparam KEY : type of node key (see tchecker::graph::cover::graph_t)
     \tparam TS : type of transition system (see tchecker::ts::ts_t)
     \param TS_ALLOCATOR : type of allocator of transition system (see tchecker::ts::allocator_t), should be garbage collected,
     should allocates nodes
     */
    template <class KEY, class TS, class TS_ALLOCATOR>
    class graph_t
    : public tchecker::covreach::details::graph_types_t<TS>,
    private tchecker::graph::cover::graph_t<typename tchecker::covreach::details::graph_types_t<TS>::node_ptr_t, KEY>,
    private tchecker::graph::directed::graph_t
    <typename tchecker::covreach::details::graph_types_t<TS>::node_ptr_t,
    typename tchecker::covreach::details::graph_types_t<TS>::edge_ptr_t>
    {
    public:
      /*!
       \brief Type of key
       */
      using key_t = KEY;
      
      /*!
       \brief Type of transition system
       */
      using ts_t = TS;
      
      /*!
       \brief Type of transition system allocator
       */
      using ts_allocator_t = TS_ALLOCATOR;
      
      /*!
       \brief Type of nodes
       */
      using node_t = typename tchecker::covreach::details::graph_types_t<TS>::node_t;
      
      /*!
       \brief Type of pointers to node
       */
      using node_ptr_t = typename tchecker::covreach::details::graph_types_t<TS>::node_ptr_t;
      
      /*!
       \brief Type of edges
       */
      using edge_t = typename tchecker::covreach::details::graph_types_t<TS>::edge_t;
      
      /*!
       \brief Type of pointers to edge
       */
      using edge_ptr_t = typename tchecker::covreach::details::graph_types_t<TS>::edge_ptr_t;
      
      /*!
       \brief Type of map : node -> key
       */
      using node_to_key_t = typename tchecker::graph::cover::node_to_key_t<key_t, node_ptr_t>;
      
      /*!
       \brief Type of less-or-equal predicate on nodes
       */
      using node_binary_predicate_t = typename tchecker::graph::cover::node_binary_predicate_t<node_ptr_t>;
      
      // TS_ALLOCATOR should allocate nodes
      static_assert( std::is_same<typename TS_ALLOCATOR::state_t, node_t>::value, "" );
      
      /*!
       \brief Constructor
       \param gc : a garbage collector
       \param ta_alloc_args : parameters to a constructor of TS_ALLOCATOR
       \param block_size : size of allocation blocks (see tchecker::pool_t)
       \param table_size : size of the nodes table (see tchecker::graph::cover_graph_t)
       \param node_to_key : a map that associates keys to nodes (see tchecker::graph::cover_graph_t)
       \param le_node : less-than-or-equal-to predicate on nodes
       \post this owns a transition system allocator of type TS_ALLOCATOR built from args and an edge allocator built
       from block_size. The nodes of the graph are built directly using TS_ALLOCATOR. The allocators have been enrolled
       to gc. In order to answer node covering queries, nodes are stored in a table with table_size entries. Each entry
       is a container of all nodes with same key according to node_to_key. Nodes are compared for covering w.r.t. le_node.
       */
      template <class ... ARGS>
      graph_t(tchecker::gc_t & gc,
              std::tuple<ARGS...> && ts_alloc_args,
              std::size_t block_size,
              std::size_t table_size,
              node_to_key_t node_to_key,
              node_binary_predicate_t le_node)
      : tchecker::graph::cover::graph_t<node_ptr_t, key_t>(table_size,
                                                           std::forward<node_to_key_t>(node_to_key),
                                                           std::forward<node_binary_predicate_t>(le_node)),
      _ts_allocator(std::forward<std::tuple<ARGS...>>(ts_alloc_args)),
      _edge_allocator(block_size, tchecker::allocation_size_t<edge_t>::alloc_size())
      {
        _ts_allocator.enroll(gc);
        _edge_allocator.enroll(gc);
      }
      
      /*!
       \brief Copy constructor (deleted)
       */
      graph_t(tchecker::covreach::graph_t<KEY, TS, TS_ALLOCATOR> const &) = delete;
      
      /*!
       \brief Move constructor (deleted)
       */
      graph_t(tchecker::covreach::graph_t<KEY, TS, TS_ALLOCATOR> &&) = delete;
      
      /*!
       \brief Destructor
       \post All the memory allocated by the graph (states, transitions, nodes and edges) have been destructed
       */
      ~graph_t()
      {
        destruct_all();
      }
      
      /*!
       \brief Assignment operator (deleted)
       */
      tchecker::covreach::graph_t<KEY, TS, TS_ALLOCATOR> &
      operator= (tchecker::covreach::graph_t<KEY, TS, TS_ALLOCATOR> const &) = delete;
      
      /*!
       \brief Move-assignment operator (deleted)
       */
      tchecker::covreach::graph_t<KEY, TS, TS_ALLOCATOR> &
      operator= (tchecker::covreach::graph_t<KEY, TS, TS_ALLOCATOR> &&) = delete;
      
      /*!
       \brief Clear
       \pre the garbage collector is not running
       \post this graph is empty
       \note nodes and edges have not been destructed (they may be collected using their allocators)
       */
      void clear()
      {
        using cover_graph_t
        = tchecker::graph::cover::graph_t<typename tchecker::covreach::details::graph_types_t<TS>::node_ptr_t, KEY>;
        
        tchecker::graph::directed::graph_t<node_ptr_t, edge_ptr_t>::clear(cover_graph_t::begin(), cover_graph_t::end());
        _root_nodes.clear();
        tchecker::graph::cover::graph_t<node_ptr_t, key_t>::clear();
      }
      
      /*!
       \brief Destruct all the nodes and edges allocated by the graph
       \pre the garbage collector is not running
       \post all the state, transitions, nodes and edges allocated by the graph have been destructed. The graph is empty
       and its allocators are empty.
       */
      void destruct_all()
      {
        clear();
        _edge_allocator.destruct_all();
        _ts_allocator.destruct_all();
      }
      
      /*!
       \brief Free all the nodes and edges allocated by the graph. No destructor called.
       \pre the garbage collector is not running
       \post all the states, transitions, nodes and edges allocated by the graph have been freed. No destructor called.
       The graph is empty and its allocators are empty.
       */
      void free_all()
      {
        clear();
        _edge_allocator.free_all();
        _ts_allocator.free_all();
      }
      
      static bool const ROOT_NODE = true; /*!< Name for root nodes */
      
      /*!
       \brief Add node
       \param n : a node
       \param root_node : root node flag
       \post n has been added to the graph. n is a root node of the graph iff root_node is true
       \throw std::invalid_argument : if n is already stored in a graph
       */
      void add_node(node_ptr_t const & n, bool root_node = false)
      {
        tchecker::graph::cover::graph_t<node_ptr_t, key_t>::add_node(n);
        if (root_node) {
          n->make_protected();
          _root_nodes.push_back(n);
        }
      }
      
      /*!
       \brief Add edge
       \param src : source node
       \param tgt : target node
       \param edge_type : type of edge
       \post an edge src -> tgt with type edge_type has been allocated and added to the graph
       */
      void add_edge(node_ptr_t const & src, node_ptr_t const & tgt, enum tchecker::covreach::edge_type_t edge_type)
      {
        edge_ptr_t edge = _edge_allocator.construct(edge_type);
        tchecker::graph::directed::graph_t<node_ptr_t, edge_ptr_t>::add_edge(src, tgt, edge);
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
      void remove_node(node_ptr_t const & n)
      {
        assert( ! connected(n) );
        tchecker::graph::cover::graph_t<node_ptr_t, key_t>::remove_node(n);
      }
      
      /*!
       \brief Remove all the edges of a node
       \param n : a node
       \post all incoming edges and outgoing edges of n have been removed from the graph
       */
      void remove_edges(node_ptr_t const & n)
      {
        tchecker::graph::directed::graph_t<node_ptr_t, edge_ptr_t>::remove_edges(n);
        assert( ! connected(n) );
      }
      
      /*!
       \brief Move incoming edges
       \param n1 : a node
       \param n2 : a node
       \param edge_type : a type of edge
       \post all incoming edges of node n1 have been moved into incoming edges of node n2
       and their type has been changed to edge_type
       */
      void move_incoming_edges(node_ptr_t const & n1,
                               node_ptr_t const & n2,
                               enum tchecker::covreach::edge_type_t edge_type)
      {
        auto in_edges = incoming_edges(n1);
        for (edge_ptr_t const & edge : in_edges)
          edge->set_type(edge_type);
        tchecker::graph::directed::graph_t<node_ptr_t, edge_ptr_t>::move_incoming_edges(n1, n2);
      }
      
      /*!
       \brief Cover a node
       \param n : a node
       \param covering_node : a node
       \pre n is stored in this graph
       \post covering_node points to a node bigger-than-or-equal-to n is any, nullptr otherwise
       \return true if n is less-than-or-equal-to some node with same key as n in this graph, false otherwise
       */
      bool is_covered(node_ptr_t const & n, node_ptr_t & covering_node) const
      {
        return tchecker::graph::cover::graph_t<node_ptr_t, key_t>::is_covered(n, covering_node);
      }
      
      /*!
       \brief Accessor
       \param n : a node
       \param ins : a node inserter
       \pre n is stored in this graph
       \post all the nodes with same key as n that are less-than-or-equal-to n have been inserted using ins
       */
      template <class INSERTER>
      void covered_nodes(node_ptr_t const & n, INSERTER & ins) const
      {
        return tchecker::graph::cover::graph_t<node_ptr_t, key_t>::covered_nodes(n, ins);
      }
      
      /*!
       \brief Type of iterator over root nodes
       */
      using root_nodes_const_iterator_t = typename std::vector<node_ptr_t>::const_iterator;
      
      /*!
       \brief Accessor
       \return range of root nodes
       */
      tchecker::range_t<root_nodes_const_iterator_t> root_nodes() const
      {
        return tchecker::make_range(_root_nodes.begin(), _root_nodes.end());
      }
      
      /*!
       \brief Accessor
       \param n : node
       \return range of incoming edges of node n
       */
      inline
      tchecker::range_t<typename tchecker::graph::directed::graph_t<node_ptr_t, edge_ptr_t>::incoming_edges_iterator_t>
      incoming_edges(node_ptr_t const & n) const
      {
        return tchecker::graph::directed::graph_t<node_ptr_t, edge_ptr_t>::incoming_edges(n);
      }
      
      /*!
       \brief Accessor
       \param n : node
       \return range of outgoing edges of node n
       */
      inline
      tchecker::range_t<typename tchecker::graph::directed::graph_t<node_ptr_t, edge_ptr_t>::outgoing_edges_iterator_t>
      outgoing_edges(node_ptr_t const & n) const
      {
        return tchecker::graph::directed::graph_t<node_ptr_t, edge_ptr_t>::outgoing_edges(n);
      }
      
      /*!
       \brief Accessor
       \param edge : an edge
       \return the source node of edge
       */
      inline node_ptr_t const & edge_src(edge_ptr_t const & edge) const
      {
        return tchecker::graph::directed::graph_t<node_ptr_t, edge_ptr_t>::edge_src(edge);
      }
      
      /*!
       \brief Accessor
       \param edge : an edge
       \return the target node of edge
       */
      inline node_ptr_t const & edge_tgt(edge_ptr_t const & edge) const
      {
        return tchecker::graph::directed::graph_t<node_ptr_t, edge_ptr_t>::edge_tgt(edge);
      }
      
      /*!
       \brief Accessor
       \param edge : an edge
       \return the type of edge
       */
      inline enum tchecker::covreach::edge_type_t edge_type(edge_ptr_t const & edge) const
      {
        return edge->edge_type();
      }
      
      /*!
       \brief Accessor
       \return the TS allocator of this graph
       */
      inline TS_ALLOCATOR & ts_allocator()
      {
        return _ts_allocator;
      }
      
      /*!
       \brief Accessor
       \return Number of nodes in this graph
       */
      std::size_t nodes_count() const
      {
        return tchecker::graph::cover::graph_t<node_ptr_t, key_t>::nodes_count();
      }

      using const_iterator_t = typename tchecker::graph::cover::graph_t<node_ptr_t, key_t>::const_iterator_t;

      const_iterator_t begin () const {
        return tchecker::graph::cover::graph_t<node_ptr_t, key_t>::begin ();
      }

      const_iterator_t end () const {
        return tchecker::graph::cover::graph_t<node_ptr_t, key_t>::end ();
      }

    private:
      /*!
       \brief Connection predicate
       \param n : a node
       \return true if n is connected (has at least an incoming/outgoing edge), false otherwise
       */
      bool connected(node_ptr_t const & n)
      {
        auto in_edges = incoming_edges(n);
        auto out_edges = outgoing_edges(n);
        return ((in_edges.begin() != in_edges.end()) || (out_edges.begin() != out_edges.end()));
      }

      std::vector<node_ptr_t> _root_nodes;         /*!< Root nodes */
      TS_ALLOCATOR _ts_allocator;                  /*!< Transition system allocator */
      tchecker::pool_t<edge_t> _edge_allocator;    /*!< Allocator of edges */
    };
    
  } // end of namespace covreach
  
} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COVREACH_GRAPH_HH
