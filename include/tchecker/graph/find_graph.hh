/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FIND_GRAPH_HH
#define TCHECKER_FIND_GRAPH_HH

#include <unordered_set>

/*!
 \file find_graph.hh
 \brief Graph with node finding
 */

namespace tchecker {
  
  namespace graph {
    
    /*!
     \class find_graph_t
     \brief Graph with node finding
     \tparam NODE_PTR : type of pointer to node
     \tparam HASH : hash function on NODE_PTR (see std::hash)
     \tparam EQUAL : equality function on NODE_PTR (see std::equal_to)
     \note this graph implementation stores nodes and answers find queries.
     It does not store edges
     \note each node has a unique instance in this graph w.r.t. EQUAL
     */
    template <class NODE_PTR, class HASH, class EQUAL>
    class find_graph_t {
    public:
      /*!
       \brief Type of pointers of node
       */
      using node_ptr_t = NODE_PTR;
      
      /*!
       \brief Type of hash function
       */
      using hash_t = HASH;
      
      /*!
       \brief Type of equality predicate
       */
      using equal_t = EQUAL;
      
      /*!
       \brief Clear
       \post The graph is empty
       \note No destructor call on nodes
       */
      inline void clear()
      {
        _nodes.clear();
      }
      
      /*!
       \brief Accessor
       \param n : a node
       \return the node in the graph equivalent to n w.r.t. HASH and EQUAL if any,
       n otherwise
       */
      NODE_PTR const & find(NODE_PTR const & n)
      {
        auto it = _nodes.find(n);
        if (it != _nodes.end())
          return *it;
        return n;
      }
      
      /*!
       \brief Add node
       \param n : a node
       \post n has been added to the graph unless it already contains an equivalent
       node w.r.t. HASH and EQUAL
       \return true if n has been added to the graph, false otherwise
       */
      bool add_node(NODE_PTR const & n)
      {
        try {
          _nodes.insert(n);
          return true;
        }
        catch (...)
        {}
        return false;
      }
    protected:
      std::unordered_set<NODE_PTR, HASH, EQUAL> _nodes;   /*!< Set of nodes */
    };
    
  } // end of namespace graph
  
} // end of namespace tchecker

#endif // TCHECKER_FIND_GRAPH_HH

