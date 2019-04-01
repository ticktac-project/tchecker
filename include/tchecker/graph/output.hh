/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GRAPH_OUTPUT_HH
#define TCHECKER_GRAPH_OUTPUT_HH

#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>

#include "tchecker/basictypes.hh"
#include "tchecker/utils/allocation_size.hh"

/*!
 \file output.hh
 \brief Outputters for graphs
 */

namespace tchecker {
  
  namespace graph {
    
    /*!
     \class node_with_id_t
     \brief Node with an ID
     */
    class node_with_id_t {
    public:
      /*!
       \brief Constructor
       \param id : node identifier
       \note id should uniquely identify this node
       \post this node has idenitifer id
       */
      node_with_id_t(tchecker::node_id_t id) : _id(id)
      {}
      
      /*!
       \brief Accessor
       \return this node ID
       */
      inline tchecker::node_id_t id() const
      {
        return _id;
      }
    private:
      tchecker::node_id_t _id;  /*!< Node ID */
    };
    
    
    
    
    /*!
     \class outputter_t
     \brief Interface to graph outputter
     \tparam NODE : type of nodes
     \tparam EDGE : type of edges
     */
    template <class NODE, class EDGE>
    class outputter_t {
    public:
      virtual ~outputter_t() = default;
      
      /*!
       \brief Initialize output
       \param name : graph name
       \note should be called once, before outputting any node or edge
       */
      virtual void initialize(std::string const & name) = 0;
      
      /*!
       \brief Finalize output
       \note should be called once, afer outputting all nodes and edges
       */
      virtual void finalize() = 0;
      
      /*!
       \brief Output node
       \param node : a node
       */
      virtual void output_node(NODE const & node) = 0;
      
      /*!
       \brief Output edge
       \param src : source node
       \param tgt : target node
       \param edge : an edge
       */
      virtual void output_edge(NODE const & src, NODE const & tgt, EDGE const & edge) = 0;
    };
    
    
    
    namespace details {
      
      /*!
       \class outputter_impl_t
       \brief Partial implementation of tchecker::graph::outputter_t
       \tparam NODE : type of nodes, should derive from tchecker::graph::node_with_id_t
       \tparam EDGE : type of edges
       \tparam NODE_OUTPUTTER : outputter for nodes
       \tparam EDGE_OUTPUTTER : outputter for edges
       */
      template <class NODE, class EDGE, class NODE_OUTPUTTER, class EDGE_OUTPUTTER>
      class outputter_impl_t : public tchecker::graph::outputter_t<NODE, EDGE> {
      public:
        /*!
         \brief Type of nodes
         */
        using node_t = NODE;
        
        /*!
         \brief Type of edges
         */
        using edge_t = EDGE;
        
        /*!
         \brief Type of node outputter
         */
        using node_outputter_t = NODE_OUTPUTTER;
        
        /*!
         \brief Type of edge outputter
         */
        using edge_outputter_t = EDGE_OUTPUTTER;
        
        /*!
         \brief Constructor
         \param os : output stream
         \param no_args : arguments to a constructor of NODE_OUTPUTTER
         \param eo_args : arguments to a constructor of EDGE_OUTPUTTER
         \note this keeps a reference on os
         */
        template <class ... NO_ARGS, class ... EO_ARGS>
        outputter_impl_t(std::ostream & os, std::tuple<NO_ARGS...> && no_args, std::tuple<EO_ARGS...> && eo_args)
        : _os(os),
        _node_outputter(std::make_from_tuple<NODE_OUTPUTTER>(no_args)),
        _edge_outputter(std::make_from_tuple<EDGE_OUTPUTTER>(eo_args))
        {}
        
        /*!
         \brief Copy constructor
         */
        outputter_impl_t(tchecker::graph::details::outputter_impl_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> const &) = default;
        
        /*!
         \brief Move constructor
         */
        outputter_impl_t(tchecker::graph::details::outputter_impl_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> &&) = default;
        
        /*!
         \brief Destructor
         */
        virtual ~outputter_impl_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::graph::details::outputter_impl_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> &
        operator= (tchecker::graph::details::outputter_impl_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> const &) = default;
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::graph::details::outputter_impl_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> &
        operator= (tchecker::graph::details::outputter_impl_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> &&) = default;
      protected:
        std::ostream & _os;               /*!< Output stream */
        NODE_OUTPUTTER _node_outputter;   /*!< Outputter for nodes */
        EDGE_OUTPUTTER _edge_outputter;   /*!< Outputter for edges */
      };
      
    } // end of namespace details
    
    
    
    
    /*!
     \class raw_outputter_t
     \brief Raw outputter for graphs
     \tparam NODE : type of nodes, should derive from tchecker::graph::node_with_id_t
     \tparam EDGE : type of edges
     \tparam NODE_OUTPUTTER : outputter for nodes
     \tparam EDGE_OUTPUTTER : outputter for edges
     */
    template <class NODE, class EDGE, class NODE_OUTPUTTER, class EDGE_OUTPUTTER>
    class raw_outputter_t : public tchecker::graph::details::outputter_impl_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> {
      
      static_assert(std::is_base_of<tchecker::graph::node_with_id_t, NODE>::value, "");
      
    public:
      /*!
       \brief Constructor
       \param os : output stream
       \param no_args : arguments to a constructor of NODE_OUTPUTTER
       \param eo_args : arguments to a constructor of EDGE_OUTPUTTER
       \note this keeps a reference on os
       */
      template <class ... NO_ARGS, class ... EO_ARGS>
      raw_outputter_t(std::ostream & os, std::tuple<NO_ARGS...> && no_args, std::tuple<EO_ARGS...> && eo_args)
      : tchecker::graph::details::outputter_impl_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER>
      (os, std::forward<std::tuple<NO_ARGS...>>(no_args), std::forward<std::tuple<EO_ARGS...>>(eo_args))
      {}
      
      /*!
       \brief Copy constructor
       */
      raw_outputter_t(tchecker::graph::raw_outputter_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> const &) = default;
      
      /*!
       \brief Move constructor
       */
      raw_outputter_t(tchecker::graph::raw_outputter_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> &&) = default;
      
      /*!
       \brief Destructor
       */
      virtual ~raw_outputter_t() = default;
      
      /*!
       \brief Assignment operator
       */
      tchecker::graph::raw_outputter_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> &
      operator= (tchecker::graph::raw_outputter_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> const &) = default;
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::graph::raw_outputter_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> &
      operator= (tchecker::graph::raw_outputter_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> &&) = default;
      
      /*!
       \brief Initialize output
       \param name : graph name
       \post name has been output
       \note should be called once, before outputting any node or edge
       */
      virtual void initialize(std::string const & name)
      {
        this->_os << name << std::endl;
      }
      
      /*!
       \brief Finalize output
       \post does nothing
       \note should be called once, afer outputting all nodes and edges
       */
      virtual void finalize()
      {}
      
      /*!
       \brief Output node
       \param node : a node
       \post node has been output following NODE_OUTPUTTER
       */
      virtual void output_node(NODE const & node)
      {
        this->_os << node.id() << ": ";
        this->_node_outputter.output(this->_os, node);
        this->_os << std::endl;
      }
      
      /*!
       \brief Output edge
       \param src : source node
       \param tgt : target node
       \param edge : an edge
       \post "src -> tgt edge" has been output following NODE_OUTPUTTER and EDGE_OUTPUTTER
       */
      virtual void output_edge(NODE const & src, NODE const & tgt, EDGE const & edge)
      {
        this->_os << src.id() << " -> " << tgt.id() << " ";
        this->_edge_outputter.output(this->_os, edge);
        this->_os << std::endl;
      }
    };
    
    
    
    
    /*!
     \class dot_outputter_t
     \brief graphviz dot outputter for graphs
     \tparam NODE : type of nodes, should derive from tchecker::graph::node_with_id_t
     \tparam EDGE : type of edges
     \tparam NODE_OUTPUTTER : outputter for nodes
     \tparam EDGE_OUTPUTTER : outputter for edges
     */
    template <class NODE, class EDGE, class NODE_OUTPUTTER, class EDGE_OUTPUTTER>
    class dot_outputter_t : public tchecker::graph::details::outputter_impl_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> {
      
      static_assert(std::is_base_of<tchecker::graph::node_with_id_t, NODE>::value, "");
      
    public:
      /*!
       \brief Constructor
       \param os : output stream
       \param no_args : arguments to a constructor of NODE_OUTPUTTER
       \param eo_args : arguments to a constructor of EDGE_OUTPUTTER
       \note this keeps a reference on os
       */
      template <class ... NO_ARGS, class ... EO_ARGS>
      dot_outputter_t(std::ostream & os, std::tuple<NO_ARGS...> && no_args, std::tuple<EO_ARGS...> && eo_args)
      : tchecker::graph::details::outputter_impl_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER>
      (os, std::forward<std::tuple<NO_ARGS...>>(no_args), std::forward<std::tuple<EO_ARGS...>>(eo_args))
      {}
      
      /*!
       \brief Copy constructor
       */
      dot_outputter_t(tchecker::graph::dot_outputter_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> const &) = default;
      
      /*!
       \brief Move constructor
       */
      dot_outputter_t(tchecker::graph::dot_outputter_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> &&) = default;
      
      /*!
       \brief Destructor
       */
      virtual ~dot_outputter_t() = default;
      
      /*!
       \brief Assignment operator
       */
      tchecker::graph::dot_outputter_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> &
      operator= (tchecker::graph::dot_outputter_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> const &) = default;
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::graph::dot_outputter_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> &
      operator= (tchecker::graph::dot_outputter_t<NODE, EDGE, NODE_OUTPUTTER, EDGE_OUTPUTTER> &&) = default;
      
      /*!
       \brief Initialize output
       \param name : graph name
       \post preample of dot graph and name have been output
       \note should be called once, before outputting any node or edge
       */
      virtual void initialize(std::string const & name)
      {
        this->_os << "digraph " << name << " {" << std::endl;
        this->_os << "node [shape=\"box\",style=\"rounded\"];" << std::endl;
      }
      
      /*!
       \brief Finalize output
       \post close of dot graph has been output
       \note should be called once, afer outputting all nodes and edges
       */
      virtual void finalize()
      {
        this->_os << "}" << std::endl;
      }
      
      /*!
       \brief Output node
       \param node : a node
       \post node has been output following NODE_OUTPUTTER
       */
      virtual void output_node(NODE const & node)
      {
        this->_os << "n" << node.id() << " [label=\"";
        this->_node_outputter.output(this->_os, node);
        this->_os << "\"]" << std::endl;
      }
      
      /*!
       \brief Output edge
       \param src : source node
       \param tgt : target node
       \param edge : an edge
       \post "src -> tgt" (edge is not output)
       */
      virtual void output_edge(NODE const & src, NODE const & tgt, EDGE const & edge)
      {
        this->_os << "n" << src.id() << " -> " << "n" << tgt.id() << std::endl;
      }
    };
    
  } // end of namespace graph
  
  
  
  
  /*!
   \brief Allocation size for tchecker::graph::node_with_id_t
   */
  template <>
  class allocation_size_t<tchecker::graph::node_with_id_t> {
  public:
    static constexpr std::size_t alloc_size()
    {
      return sizeof(tchecker::graph::node_with_id_t);
    }
  };
  
} // end of namespace tchecker

#endif // TCHECKER_GRAPH_OUTPUT_HH
