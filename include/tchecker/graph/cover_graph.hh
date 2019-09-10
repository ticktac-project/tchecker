/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_COVER_GRAPH_HH
#define TCHECKER_COVER_GRAPH_HH

#include <functional>
#include <type_traits>
#include <vector>

#include "tchecker/utils/iterator.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file cover_graph.hh
 \brief Graph with node covering
 */

namespace tchecker {
  
  namespace graph {
    
    namespace cover {
      
      // Forward declaration
      template <class NPTR, class K> class graph_t;
      
      
      
      
      /*!
       \brief Node-to-key map
       \tparam KEY : type of keys
       \tparam NODE_PTR : type of pointers to node
       */
      template <class KEY, class NODE_PTR>
      using node_to_key_t = std::function<KEY(NODE_PTR const &)>;
      
      /*!
       \brief Type of binary predicate on nodes
       \tparam NODE_PTR : type of pointers to node
       */
      template <class NODE_PTR>
      using node_binary_predicate_t = std::function<bool(NODE_PTR const &, NODE_PTR const &)>;
      
      
      
      
      namespace details {
        
        /*!
         \class nodes_container_t
         \brief Type of container of nodes
         \tparam NODE_PTR : type of pointers to node, *NODE_PTR should inherit from tchecker::graph::cover::node_t
         */
        template <class NODE_PTR>
        class nodes_container_t {
        public:
          /*!
           \brief Clear
           \post This container is empty
           \note No destructor call on nodes
           \note invalidates iterators on this container
           */
          void clear()
          {
            for (NODE_PTR & n : _vector)
              n->clear_position();
            _vector.clear();
          }
          
          /*!
           \brief Add node
           \param n : a node
           \pre n is not stored (not checked)
           \post n has been added to this container
           \note invalidates iterators on this container
           */
          void add(NODE_PTR const & n)
          {
            n->position_in_container(_vector.size());
            _vector.push_back(n);
          }
          
          /*!
           \brief Remove node
           \param n : a node
           \pre n is stored in this container (checked by assertion)
           \post n has been removed from this container
           \throw std::invalid_argument : if n is not stored in this container
           \note constant-time complexity
           \note invalidates iterators on this container
           */
          void remove(NODE_PTR const & n)
          {
            assert(n->is_stored());
            auto position = n->position_in_container();
            if ((position >= _vector.size()) || (_vector[position] != n))
              throw std::invalid_argument("Removing a node that is not stored in this containter is not allowed");
            _vector.back()->position_in_container(position);
            n->clear_position();
            _vector[position] = _vector.back();
            _vector.pop_back();
          }
          
          /*!
           \brief Type of constant iterator
           */
          using const_iterator_t = typename std::vector<NODE_PTR>::const_iterator;
          
          /*!
           \brief Accessor
           \return constant iterator on first node in container if any, past-the-end iterator otherwise
           */
          tchecker::graph::cover::details::nodes_container_t<NODE_PTR>::const_iterator_t begin() const
          {
            return _vector.begin();
          }
          
          /*!
           \brief Accessor
           \return constant past-the-end iterator
           */
          tchecker::graph::cover::details::nodes_container_t<NODE_PTR>::const_iterator_t end() const
          {
            return _vector.end();
          }
          
          /*!
           \brief Accessor
           \return size of this container
           \note constant-time complexity
           */
          std::size_t size() const
          {
            return _vector.size();
          }
        private:
          std::vector<NODE_PTR> _vector;  /*!< Container of nodes */
        };
        
      } // end of namespace details
      
      
      
      
      /*!
       \brief Type of position of a node in container
       */
      using node_position_t = unsigned int;
      
      
      /*!
       \class node_t
       \brief Node in a cover graph
       \note stores the position of the node in the container for fast removal
       */
      class node_t {
      public:
        /*!
         \brief Constructor
         \post node is not stored in the graph
         */
        node_t()
        {
          clear_position();
        }
        
        /*!
         \brief Copy constructor
         */
        node_t(tchecker::graph::cover::node_t const &) = default;
        
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
         */
        tchecker::graph::cover::node_t & operator= (tchecker::graph::cover::node_t const &) = default;
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::graph::cover::node_t & operator= (tchecker::graph::cover::node_t &) = default;
      private:
        template <class NPTR> friend class tchecker::graph::cover::details::nodes_container_t;
        template <class NPTR, class K> friend class tchecker::graph::cover::graph_t;
        
        /*!
         \brief Accessor
         \return position of this node in the table of node containers
         */
        inline tchecker::graph::cover::node_position_t position_in_table() const
        {
          return _position_in_table;
        }
        
        /*!
         \brief Accessor
         \return position of this node in the node container
         */
        inline tchecker::graph::cover::node_position_t position_in_container() const
        {
          return _position_in_container;
        }
        
        /*!
         \brief Setter
         \param position : a position
         \post this node's position-in-table has been set to position
         */
        inline void position_in_table(tchecker::graph::cover::node_position_t position)
        {
          _position_in_table = position;
        }
        
        /*!
         \brief Setter
         \param position : a position
         \pre position != _NOT_A_POSITION
         \post this node's position-in-container has been set to position
         \throw std::invalid_argument : if precondition is violated
         */
        inline void position_in_container(tchecker::graph::cover::node_position_t position)
        {
          if (position == _NOT_A_POSITION)
            throw std::invalid_argument("cover node position out of bounds");
          _position_in_container = position;
        }
        
        /*!
         \brief Clear
         \post this node's position has been cleared
         */
        inline void clear_position()
        {
          _position_in_table = 0;
          _position_in_container = _NOT_A_POSITION;
        }
        
        /*!
         \brief Accessor
         \return true if this node is stored in a graph, false otherwise
         */
        inline bool is_stored() const
        {
          return ((_position_in_table != 0) || (_position_in_container != _NOT_A_POSITION));
        }
        
        node_position_t _position_in_table;      /*!< Position in the table of node containers */
        node_position_t _position_in_container;  /*!< Position in the node container */
        static node_position_t const _NOT_A_POSITION = std::numeric_limits<node_position_t>::max();
      };
      
      
      
      
      /*!
       \class graph_t
       \brief Graph with node covering
       \tparam NODE_PTR : type of pointers to node, *NODE_PTR should inherit from tchecker::graph::cover::node_t
       \tparam KEY : type of keys
       \note this graph stores nodes in a map : keys -> containers of nodes. All nodes with the same key
       are stored in the same container. Keys are used to restrict the set of nodes that need to be examined
       for covering queries: only the nodes with the same key are considered
       */
      template <class NODE_PTR, class KEY=std::size_t>
      class graph_t {
        static_assert(std::is_unsigned<KEY>::value, "");
      private:
        /*!
         \brief Type of map : index -> container of nodes
         */
        using nodes_map_t = std::vector<tchecker::graph::cover::details::nodes_container_t<NODE_PTR>>;
        
        /*!
         \brief Type of nodes container
         */
        using nodes_container_t = tchecker::graph::cover::details::nodes_container_t<NODE_PTR>;
      public:
        /*!
         \brief Type of pointers to node
         */
        using node_ptr_t = NODE_PTR;
        
        /*!
         \brief Type of key
         */
        using key_t = KEY;
        
        /*!
         \brief Constructor
         \param table_size : size of the table of node containers
         \param node_to_key : a map from NODE_PTR to KEY
         \param le_node : s less-or-equal predicate on nodes
         \throw std::invalid_argument : if table_size is too big w.r.t. type tchecker::graph::cover::node_position_t
         */
        graph_t(std::size_t table_size,
                tchecker::graph::cover::node_to_key_t<KEY, NODE_PTR> node_to_key,
                tchecker::graph::cover::node_binary_predicate_t<NODE_PTR> le_node)
        : _node_to_key(std::move(node_to_key)), _le_node(std::move(le_node)), _nodes(table_size)
        {
          if (table_size > std::numeric_limits<tchecker::graph::cover::node_position_t>::max())
            throw std::invalid_argument("table size exceeds node positions");
        }
        
        /*!
         \brief Copy constructor
         */
        graph_t(tchecker::graph::cover::graph_t<NODE_PTR, KEY> const &) = default;
        
        /*!
         \brief Move constructor
         */
        graph_t(tchecker::graph::cover::graph_t<NODE_PTR, KEY> &&) = default;
        
        /*!
         \brief Destructor
         */
        ~graph_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::graph::cover::graph_t<NODE_PTR, KEY> &
        operator= (tchecker::graph::cover::graph_t<NODE_PTR, KEY> const &) = default;
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::graph::cover::graph_t<NODE_PTR, KEY> & operator= (tchecker::graph::cover::graph_t<NODE_PTR, KEY> &&) = default;
        
        /*!
         \brief Clear
         \post The graph is empty
         \note No destructor call on nodes
         */
        void clear()
        {
          for (auto & container : _nodes)
            container.clear();
          _nodes.clear();
        }
        
        /*!
         \brief Add node
         \param n : a node
         \pre n is not stored in a graph
         \post n has been added to the graph
         \throw std::invalid_argument : if n is already stored in a graph
         \note complexity : computation of the key of node n
         \note invalidates iterators
         */
        void add_node(NODE_PTR const & n)
        {
          if (n->is_stored())
            throw std::invalid_argument("adding a stored node is not allowed");
          tchecker::graph::cover::node_position_t position =_node_to_key(n) % _nodes.size();
          n->position_in_table(position);
          _nodes[position].add(n);
        }
        
        /*!
         \brief Remove node
         \param n : a node
         \pre n is stored in this graph
         \post n has been removed from this graph
         \throw std::invalid_argument : if n is not stored in this graph
         \throw as tchecker::graph::cover::details::nodes_container_t::remove
         \note contant-time complexity
         \note invalidates iterators
         */
        void remove_node(NODE_PTR const & n)
        {
          if (! n->is_stored())
            throw std::invalid_argument("removing an unstored node is not allowed");
          auto position = n->position_in_table();
          if (position >= _nodes.size())
            throw std::invalid_argument("removing a node which is not stored in this graph is not allowed");
          _nodes[position].remove(n);
        }
        
        /*!
         \brief Cover a node
         \param n : a node
         \param covering_node : a node
         \pre n is stored in this graph (partially checked by assertion)
         \post covering_node points to a node bigger-than-or-equal-to n if any, nullptr otherwise
         \return true if n is less-than-or-equal-to some node with same key in this graph, false otherwise
         */
        bool is_covered(NODE_PTR const & n, NODE_PTR & covering_node) const
        {
          assert(n->position_in_table() < _nodes.size());
          auto const & container = _nodes[n->position_in_table()];
          for (NODE_PTR const & node : container)
            if (_le_node(n, node)) {
              covering_node = node;
              return true;
            }
          
          covering_node = nullptr;
          return false;
        }
        
        /*!
         \brief Accessor
         \param n : a node
         \param ins : a node inserter
         \pre n is stored in this graph (partially checked by assertion)
         \post all nodes in graph with same key as n which are less-or-equal-to n have been inserted in ins
         */
        template <class INSERTER>
        void covered_nodes(NODE_PTR const & n, INSERTER & ins) const
        {
          assert(n->position_in_table() < _nodes.size());
          auto const & container = _nodes[n->position_in_table()];
          for (NODE_PTR const & node : container)
            if (_le_node(node, n))
              ins = node;
        }
        
        /*!
         \brief Accessor
         \return Number of nodes in this graph
         \note linear-time complexity in the size of table of nodes
         */
        std::size_t nodes_count() const
        {
          std::size_t count = 0;
          for (nodes_container_t const & c : _nodes)
            count += c.size();
          return count;
        }
        
        /*!
         \brief Type of iterator over nodes in the graph
         */
        using const_iterator_t =
        tchecker::join_iterator_t<typename nodes_map_t::const_iterator, typename nodes_container_t::const_iterator_t>;

        /*!
         \brief Accessor
         \return Iterator pointing to the first node in the graph, or past-the-end if the graph is empty
         */
        tchecker::graph::cover::graph_t<NODE_PTR, KEY>::const_iterator_t begin() const
        {
          return tchecker::graph::cover::graph_t<NODE_PTR, KEY>::const_iterator_t
          (_nodes.begin(),
           _nodes.end(),
           [] (typename nodes_map_t::const_iterator const & it) {
             return std::make_tuple(it->begin(), it->end());
           });
        }
        
        /*!
         \brief Accessor
         \return Past-the-end iterator
         */
        tchecker::graph::cover::graph_t<NODE_PTR, KEY>::const_iterator_t end() const
        {
          return tchecker::graph::cover::graph_t<NODE_PTR, KEY>::const_iterator_t
          (_nodes.end(),
           _nodes.end(),
           [] (typename nodes_map_t::const_iterator const & it) {
             return std::make_tuple(it->begin(), it->end());
           });
        }
      protected:
        tchecker::graph::cover::node_to_key_t<KEY, NODE_PTR> _node_to_key;  /*!< a node-to-key map */
        tchecker::graph::cover::node_binary_predicate_t<NODE_PTR> _le_node; /*!< less-or-equal relation on node pointers */
        nodes_map_t _nodes;                                                 /*!< map : key -> nodes with that key */
      };
      
    } // end of namespace cover
    
  } // end of namespace graph
  
} // end of namespace tchecker

#endif // TCHECKER_COVER_GRAPH_HH

