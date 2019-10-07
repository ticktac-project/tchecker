/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_DIRECTED_GRAPH_HH
#define TCHECKER_DIRECTED_GRAPH_HH

#include "tchecker/utils/iterator.hh"

/*!
 \file directed_graph.hh
 \brief Directed graph
 */

namespace tchecker {
  
  namespace graph {
    
    namespace directed {
      
      // Forward declarations
      template <class TAG, class EDGE_PTR> class edge_iterator_t;
      template <class NODE_PTR, class EDGE_PTR> class graph_t;
      
      
      
      
      namespace details {
        
        /*!
         \class next_edge_t
         \brief Cell of single list link of edges
         \tparam TAG : list tag (identifier)
         \tparam EDGE_PTR : type of pointers to edge
         */
        template <class TAG, class EDGE_PTR>
        struct next_edge_t : public TAG {
          /*!
           \brief Constructor
           \post next points to nullptr
           */
          next_edge_t() : _next{nullptr}
          {}
          
          EDGE_PTR _next;  /*!< Pointer to next edge */
        };
        
        
        
        
        /*!
         \class edge_list_t
         \brief Link list of egdes
         \tparam TAG : list tag (identifier)
         \tparam EDGE_PTR : type of pointers to edge
         */
        template <class TAG, class EDGE_PTR>
        struct edge_list_t : public TAG {
          /*!
           \brief Constructor
           \post head points to nullptr
           */
          edge_list_t() : _head{nullptr}
          {}
          
          EDGE_PTR _head;  /*!< Pointer to list head */
        };
        
        
        
        
        /*!
         \class edge_node_t
         \brief Node owning the edge in its TAG list
         \tparam TAG : list tag (identifier)
         \tparam NODE_PTR : type of pointers to node
         */
        template <class TAG, class NODE_PTR>
        struct edge_node_t : public TAG {
          /*!
           \brief Constructor
           \post node points to nullptr
           */
          edge_node_t() : _node{nullptr}
          {}
          
          NODE_PTR _node;
        };
        
        
        
        
        struct outgoing {}; /*!< Tag for list of outgoing edges */
        struct incoming {}; /*!< Tag for list of incoming edges */
        
      } // end of namespace details
      
      
      
      
      /*!
       \class edge_t
       \brief Base edge type providing access to source and target nodes as well as links to
       next incoming/outgoing edges
       \tparam NODE_PTR : type of pointers to node
       \tparam EDGE_PTR : type of pointers to edge
       */
      template <class NODE_PTR, class EDGE_PTR>
      class edge_t
      : private tchecker::graph::directed::details::next_edge_t<struct tchecker::graph::directed::details::incoming, EDGE_PTR>,
      private tchecker::graph::directed::details::next_edge_t<struct tchecker::graph::directed::details::outgoing, EDGE_PTR>,
      private tchecker::graph::directed::details::edge_node_t<struct tchecker::graph::directed::details::incoming, NODE_PTR>,
      private tchecker::graph::directed::details::edge_node_t<struct tchecker::graph::directed::details::outgoing, NODE_PTR>
      {
      private:
        friend class tchecker::graph::directed::graph_t<NODE_PTR, EDGE_PTR>;
        friend class tchecker::graph::directed::edge_iterator_t<struct tchecker::graph::directed::details::incoming, EDGE_PTR>;
        friend class tchecker::graph::directed::edge_iterator_t<struct tchecker::graph::directed::details::outgoing, EDGE_PTR>;
        
        /*!
         \brief Accessor
         \tparam TAG : list tag (identifier)
         \return reference to next edge with TAG
         */
        template <class TAG>
        inline EDGE_PTR & next()
        {
          return tchecker::graph::directed::details::next_edge_t<TAG, EDGE_PTR>::_next;
        }
        
        /*!
         \brief Accessor
         \tparam TAG : list tag (identifier)
         \return reference to node with TAG
         */
        template <class TAG>
        inline NODE_PTR & node()
        {
          return tchecker::graph::directed::details::edge_node_t<TAG, NODE_PTR>::_node;
        }
      };
      
      
      
      
      /*!
       \class node_t
       \brief Base node type providing access to incoming/outgoing edges
       \tparam EDGE_PTR : type of pointers to edge
       */
      template <class EDGE_PTR>
      class node_t
      : private tchecker::graph::directed::details::edge_list_t<struct tchecker::graph::directed::details::incoming, EDGE_PTR>,
      private tchecker::graph::directed::details::edge_list_t<struct tchecker::graph::directed::details::outgoing, EDGE_PTR>
      {
      private:
        template <class N, class E> friend class tchecker::graph::directed::graph_t;
        
        /*!
         \brief Accessor
         \tparam TAG : list tag (identifier)
         \return reference to head of list of edges with TAG
         */
        template <class TAG>
        inline EDGE_PTR & head()
        {
          return tchecker::graph::directed::details::edge_list_t<TAG, EDGE_PTR>::_head;
        }
      };
      
      
      
      
      /*!
       \class edge_iterator_t
       \brief Iterator on edges
       \tparam TAG : edge list tag (identifier)
       \tparam EDGE_PTR : type of pointers to edge
       */
      template <class TAG, class EDGE_PTR>
      class edge_iterator_t {
      public:
        /*!
         \brief Constructor
         \param edge : first edge
         \post this iterator points to edge
         */
        explicit edge_iterator_t(EDGE_PTR const & edge)
        : _edge(edge)
        {}
        
        /*!
         \brief Copy constructor
         \param it : an iterator
         \post this iterator points to the same edge as it
         */
        edge_iterator_t(tchecker::graph::directed::edge_iterator_t<TAG, EDGE_PTR> const & it)
        : _edge(it._edge)
        {}
        
        /*!
         \brief Move constructor
         \param it : an iterator
         \post this points to the edge that was pointed by it, and it points to nullptr
         */
        edge_iterator_t(tchecker::graph::directed::edge_iterator_t<TAG, EDGE_PTR> && it)
        : _edge(it._edge)
        {
          it._edge = EDGE_PTR{nullptr};
        }
        
        /*!
         \brief Destructor
         \note Does not delete pointed edge
         */
        ~edge_iterator_t() = default;
        
        /*!
         \brief Assignement operator
         \param it : an iterator
         \post this is a copy of it: they point to the same edge
         */
        tchecker::graph::directed::edge_iterator_t<TAG, EDGE_PTR> &
        operator= (tchecker::graph::directed::edge_iterator_t<TAG, EDGE_PTR> const & it)
        {
          if (this != &it)
            _edge = it._edge;
          return *this;
        }
        
        /*!
         \brief Move-assignement operator
         \param it : an iterator
         \post this points to the edge that was pointed by it, and it points to nullptr
         */
        tchecker::graph::directed::edge_iterator_t<TAG, EDGE_PTR> &
        operator= (tchecker::graph::directed::edge_iterator_t<TAG, EDGE_PTR> && it)
        {
          if (this != &it) {
            _edge = it._edge;
            it._edge = EDGE_PTR{nullptr};
          }
          return *this;
        }
        
        /*!
         \brief Equality check
         \param it : an iterator
         \return true if this iterator and it point to the same edge, false otherwise
         */
        bool operator== (tchecker::graph::directed::edge_iterator_t<TAG, EDGE_PTR> const & it) const
        {
          return (_edge == it._edge);
        }
        
        /*!
         \brief Disequality check
         \param it : an iterator
         \return true if this iterator and it point to distinct edges, false otherwise
         */
        bool operator!= (tchecker::graph::directed::edge_iterator_t<TAG, EDGE_PTR> const & it) const
        {
          return ! (*this == it);
        }
        
        /*!
         \brief Incrementation
         \pre this iterator does not point to nullptr (checked by assertion)
         \post this iterator points to the nest edge with same TAG
         */
        tchecker::graph::directed::edge_iterator_t<TAG, EDGE_PTR> & operator++ ()
        {
          assert( _edge != EDGE_PTR{nullptr} );
          _edge = _edge->template next<TAG>();
          return *this;
        }
        
        /*!
         \brief Accessor
         \return edge pointed by this iterator
         \pre this iterator does not point to nullptr (checked by assertion)
         \note returns nullptr if this iterator points to nullptr
         \note the returned edge should not be modified directly but only through the methods provided by
         tchecker::graph::directed::graph_t
         */
        EDGE_PTR operator* () const
        {
          return _edge;
        }
      protected:
        EDGE_PTR _edge;  /*!< Pointed edge */
      };
      
      
      
      
      /*!
       \class graph_t
       \brief Directed graph
       \tparam NODE_PTR : type of pointers to node, *NODE_PTR should derive from tchecker::graph::directed::node_t
       \tparam EDGE_PTR : type of pointers to edge, *EDGE_PTR should derive from tchecker::graph::directed::edge_t
       \note This graph does not allocate and does not store nodes and edges. It only links nodes and edges. Edges
       are stored in nodes.
       */
      template <class NODE_PTR, class EDGE_PTR>
      class graph_t {
      public:
        /*!
         \brief Type of pointers to node
         */
        using node_ptr_t = NODE_PTR;
        
        /*!
         \brief Type of pointers to edge
         */
        using edge_ptr_t = EDGE_PTR;
        
        /*!
         \brief Clear
         \tparam NODES_BEGIN : type of iterator over nodes
         \tparam NODES_END : type of past-the-end iterator over nodes
         \param begin : iterator to first node
         \param end : past-the-end iterator
         \post edges have been removed from all nodes in [begin,end)
         \note this function does not destruct edges or nodes
         */
        template <class NODES_BEGIN, class NODES_END>
        void clear(NODES_BEGIN const & begin, NODES_END const & end)
        {
          for (NODES_BEGIN it = begin; it != end; ++it)
            remove_edges(*it);
        }
        
        /*!
         \brief Type of iterator on incoming edges
         */
        using incoming_edges_iterator_t
        = tchecker::graph::directed::edge_iterator_t<struct tchecker::graph::directed::details::incoming, EDGE_PTR>;
        
        /*!
         \brief Type of iterator on outgoing edges
         */
        using outgoing_edges_iterator_t
        = tchecker::graph::directed::edge_iterator_t<struct tchecker::graph::directed::details::outgoing, EDGE_PTR>;
        
        // builder interface
        
        /*!
         \brief Add an edge
         \param src : source node
         \param tgt : target node
         \param edge : and edge
         \pre edge does not belong to a graph yet
         \post edge has source node src and target node tgt. edge is an outgoing edge of src and an incoming edge of tgt
         */
        void add_edge(NODE_PTR const & src, NODE_PTR const & tgt, EDGE_PTR const & edge)
        {
          add_edge<struct tchecker::graph::directed::details::incoming>(tgt, edge);
          add_edge<struct tchecker::graph::directed::details::outgoing>(src, edge);
        }
        
        
        /*!
         \brief Remove an edge
         \param e : an edge
         \post e has been removed from the graph
         */
        void remove_edge(EDGE_PTR const & e)
        {
          remove_edge<struct tchecker::graph::directed::details::incoming>(e);
          remove_edge<struct tchecker::graph::directed::details::outgoing>(e);
        }
        
        
        /*!
         \brief Remove all the edges of a node
         \param n : a node
         \post all incoming edges and outgoing edges of n have been removed from the graph
         */
        void remove_edges(NODE_PTR const & n)
        {
          auto out_edges = outgoing_edges(n);
          auto out_end = out_edges.end();
          for (auto it = out_edges.begin(); it != out_end; ) {
            EDGE_PTR edge = *it;
            ++it;
            remove_edge(edge);
          }
          
          auto in_edges = incoming_edges(n);
          auto in_end = in_edges.end();
          for (auto it = in_edges.begin(); it != in_end; ) {
            EDGE_PTR edge = *it;
            ++it;
            remove_edge(edge);
          }
        }
        
        
        /*!
         \brief Move incoming edges
         \param n1 : current target node
         \param n2 : new target node
         \post All incoming edges of n1 have been moved into incoming edges of n2
         */
        void move_incoming_edges(NODE_PTR const & n1, NODE_PTR const & n2)
        {
          if (n1 == n2)
            return;
          
          auto in_edges = incoming_edges(n1);
          for (auto it = in_edges.begin(); it != in_edges.end(); ) {
            EDGE_PTR e = *it;
            ++it;
            
            remove_edge<struct tchecker::graph::directed::details::incoming>(e);
            add_edge<struct tchecker::graph::directed::details::incoming>(n2, e);
          }
        }
        
        
        // explorer interface
        
        /*!
         \brief Accessor
         \param n : node
         \return range of incoming edges of node n
         */
        inline tchecker::range_t<tchecker::graph::directed::graph_t<NODE_PTR, EDGE_PTR>::incoming_edges_iterator_t>
        incoming_edges(NODE_PTR const & n) const
        {
          return edges<struct tchecker::graph::directed::details::incoming>(n);
        }
        
        /*!
         \brief Accessor
         \param n : node
         \return range of outgoing edges of node n
         */
        inline tchecker::range_t<tchecker::graph::directed::graph_t<NODE_PTR, EDGE_PTR>::outgoing_edges_iterator_t>
        outgoing_edges(NODE_PTR const & n) const
        {
          return edges<struct tchecker::graph::directed::details::outgoing>(n);
        }
        
        /*!
         \brief Accessor
         \param edge : an edge
         \return the source node of edge
         */
        inline NODE_PTR const & edge_src(EDGE_PTR const & edge) const
        {
          return edge->template node<struct tchecker::graph::directed::details::outgoing>();
        }
        
        /*!
         \brief Accessor
         \param edge : an edge
         \return the target node of edge
         */
        inline NODE_PTR const & edge_tgt(EDGE_PTR const & edge) const
        {
          return edge->template node<struct tchecker::graph::directed::details::incoming>();
        }
      protected:
        /*!
         \brief Accessor
         \tparam TAG : list tag (identifier)
         \param n : a node
         \return a range of edges in n with identifier TAG
         \note constant-time complexity
         */
        template <class TAG>
        tchecker::range_t< tchecker::graph::directed::edge_iterator_t<TAG, EDGE_PTR> > edges(NODE_PTR const & n) const
        {
          tchecker::graph::directed::edge_iterator_t<TAG, EDGE_PTR> begin(n->template head<TAG>()), end(nullptr);
          return tchecker::make_range(begin, end);
        }
        
        
        /*!
         \brief Add edge
         \tparam TAG : list tag (identifier)
         \param n : node
         \param e : edge
         \pre e does not belong to another list with identifier TAG (checked by assertion)
         \post e has been added to the list of edges in node n with identifier TAG
         \note e is added at the head of the list of identifier TAG in node n
         \note constant-time complexity
         */
        template <class TAG>
        void add_edge(NODE_PTR const & n, EDGE_PTR const & e)
        {
          assert(e->template next<TAG>() == EDGE_PTR{nullptr});
          assert(e->template node<TAG>() == NODE_PTR{nullptr});
          e->template next<TAG>() = n->template head<TAG>();
          n->template head<TAG>() = e;
          e->template node<TAG>() = n;
        }
        
        
        /*!
         \brief Remove an edge from an identifier
         \tparam TAG : list tag (identifier)
         \param e : an edge
         \pre e belongs to the list of edges with identifier TAG of its node with identifier TAG (checked by assertion)
         \post edge e has been removed from the list of edges with identifier TAG in its node with identifier TAG
         \note linear-time complexity in the size of the list of edges
         */
        template <class TAG>
        void remove_edge(EDGE_PTR const & e)
        {
          NODE_PTR n = e->template node<TAG>();
          auto range = edges<TAG>(n);
          auto end = range.end();
          EDGE_PTR prev{nullptr}, next{nullptr};
          
          for (auto it = range.begin(); it != end; ++it) {
            if (*it == e) {
              // get next edge in list (if any)
              ++it;
              if (it != end)
                next = *it;
              else // no next edge
                next = EDGE_PTR{nullptr};
              
              // remove edge
              if (prev == EDGE_PTR{nullptr}) // first edge in list
                n->template head<TAG>() = next;
              else
                prev->template next<TAG>() = next;
              
              e->template next<TAG>() = EDGE_PTR{nullptr};
              e->template node<TAG>() = NODE_PTR{nullptr};
              
              return;
            }
            prev = *it;
          }
          assert(false);  // e is not in the list of edges with identifier TAG from node n
        }
      };
      
    } // end of namespace directed
    
  } // end of namespace graph
  
} // end of namespace tchecker

#endif // TCHECKER_DIRECTED_GRAPH_HH
