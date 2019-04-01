/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GRAPH_HH
#define TCHECKER_GRAPH_HH

/*!
 \file graph.hh
 \brief Graphs
 */

namespace tchecker {
  
  namespace graph {
    
    /*!
     \class graph_t
     \brief Empty graph (default implementation)
     \tparam NODE_PTR : type of pointer to node
     \tparam EDGE_PTR : type of pointer to edge
     */
    template <class NODE_PTR, class EDGE_PTR>
    class graph_t {
    public:
      /*!
       \brief Type of pointer to node
       */
      using node_ptr_t = NODE_PTR;
      
      /*!
       \brief Type of pointer to edges
       */
      using edge_ptr_t = EDGE_PTR;
      
      /*!
       \brief Add node
       \param node : a node
       \post Does nothing
       \return true
       */
      bool add_node(NODE_PTR const & node)
      {
        return true;
      }
      
      /*!
       \brief Remove node
       \param node : a node
       \post Does nothing
       \return true
       */
      bool remove_node(NODE_PTR const & node)
      {
        return true;
      }
      
      /*!
       \brief Add edge
       \param src : source node
       \param tgt : target node
       \param edge : an edge
       \post Does nothing
       \return true
       */
      bool add_edge(NODE_PTR const & src, NODE_PTR const & tgt, EDGE_PTR const & edge)
      {
        return true;
      }
    };
    
  } // end of namespace graph
  
} // end of namespace tchecker

#endif // TCHECKER_GRAPH_HH
