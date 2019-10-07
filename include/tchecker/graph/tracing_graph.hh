/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TRACING_GRAPH_HH
#define TCHECKER_TRACING_GRAPH_HH

#include <string>

#include "tchecker/graph/graph.hh"

/*!
 \file tracing_graph.hh
 \brief Graph that traces operations (add, remove, etc)
 */

namespace tchecker {
  
  namespace graph {
    
    /*!
     \class tracing_graph_t
     \brief Graph that traces graph operations (adding node/edge, etc)
     \tparam NODE_PTR : type of pointer to node
     \tparam EDGE_PTR : type of pointer to edge
     \tparam GRAPH_OUTPUTTER : type of graph outputter (see tchecker::graph::outputter_t)
     \note This graph does not store nodes and edges
     */
    template <class NODE_PTR, class EDGE_PTR, class GRAPH_OUTPUTTER>
    class tracing_graph_t : public tchecker::graph::graph_t<NODE_PTR, EDGE_PTR> {
    public:
      /*!
       \brief Type of graph outputter (see tchecker::graph::outputter_t)
       */
      using graph_outputter_t = GRAPH_OUTPUTTER;
      
      /*!
       \brief Constructor
       \param name : graph name
       \param go_args : parameters to a constructor of GRAPH_OUTPUTTER
       \post output has been initialized with name
       */
      template <class ... GO_ARGS>
      explicit tracing_graph_t(std::string const & name, GO_ARGS && ... go_args)
      : _graph_outputter(std::forward<GO_ARGS>(go_args)...)
      {
        _graph_outputter.initialize(name);
      }
      
      /*!
       \brief Copy constructor
       */
      tracing_graph_t(tchecker::graph::tracing_graph_t<NODE_PTR, EDGE_PTR, GRAPH_OUTPUTTER> const &) = default;
      
      /*!
       \brief Move constructor
       */
      tracing_graph_t(tchecker::graph::tracing_graph_t<NODE_PTR, EDGE_PTR, GRAPH_OUTPUTTER> &&) = default;
      
      /*!
       \brief Destructor
       \post output has been finalized
       */
      ~tracing_graph_t()
      {
        _graph_outputter.finalize();
      }
      
      /*!
       \brief Assignment operator
       */
      tchecker::graph::tracing_graph_t<NODE_PTR, EDGE_PTR, GRAPH_OUTPUTTER> &
      operator= (tchecker::graph::tracing_graph_t<NODE_PTR, EDGE_PTR, GRAPH_OUTPUTTER> const &) = default;
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::graph::tracing_graph_t<NODE_PTR, EDGE_PTR, GRAPH_OUTPUTTER> &
      operator= (tchecker::graph::tracing_graph_t<NODE_PTR, EDGE_PTR, GRAPH_OUTPUTTER> &&) = default;
      
      /*!
       \brief Add node
       \param node : a node
       \post node has been output
       \return true
       */
      bool add_node(NODE_PTR const & node)
      {
        _graph_outputter.output_node(*node);
        return true;
      }
      
      /*!
       \brief Add edge
       \param src : source node
       \param tgt : target node
       \param edge : an edge
       \post src -- edge -> tgt has been output
       \return true
       */
      bool add_edge(NODE_PTR const & src, NODE_PTR const & tgt, EDGE_PTR const & edge)
      {
        _graph_outputter.output_edge(*src, *tgt, *edge);
        return true;
      }
    protected:
      GRAPH_OUTPUTTER _graph_outputter;   /*!< Graph outputter */
    };
    
  } // end of namespace graph
  
} // end of namespace tchecker

#endif // TCHECKER_TRACING_GRAPH_HH

