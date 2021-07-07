/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GRAPH_OUTPUT_HH
#define TCHECKER_GRAPH_OUTPUT_HH

#include <iostream>
#include <map>
#include <ostream>
#include <set>
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
 \brief Output graph header in graphviz DOT language
 \param os : output stream
 \param name : graph name
 \post the graph header has been output to os
 \return os after output
*/
std::ostream & dot_output_header(std::ostream & os, std::string const & name);

/*!
 \brief Output graph footer in graphviz DOT language
 \param os : output stream
 \post the graph footer has been output to os
 \return os after output
 */
std::ostream & dot_output_footer(std::ostream & os);

/*!
 \brief Output node in graphviz DOT language
 \param os : output stream
 \param name : node name
 \param attr : node attributes
 \post the node has been output to os
 \return os after output
*/
std::ostream & dot_output_node(std::ostream & os, std::string const & name, std::map<std::string, std::string> const & attr);

/*!
 \brief Output edge in graphviz DOT language
 \param os : output stream
 \param src : source node name
 \param tgt : target node name
 \param attr : node attributes
 \post the edge has been output to os
 \return os after output
*/
std::ostream & dot_output_edge(std::ostream & os, std::string const & src, std::string const & tgt,
                               std::map<std::string, std::string> const & attr);

/*!
 \brief Output a graph in graphviz DOT language
 \tparam GRAPH : type of graph, should provide types GRAPH::node_sptr_t,
 GRAPH::edge_sptr_t, and method GRAPH::nodes() that returns the range of nodes,
 and a method GRAPH::outgoing_edges(n) that returns the range of outgoing edges
 of node n
 \tparam NODE_LE : total order on type GRAPH::node_sptr_t
 \tparam EDGE_LE : total order on type GRAPH::edge_sptr_t
 \param os : output stream
 \param g : a graph
 \param name : graph name
 \post the graph g has been output to os in the graphviz DOT language. The nodes
 and edges are output following the order given by NODE_LE and EDGE_LE
 */
template <class GRAPH, class NODE_LE, class EDGE_LE>
std::ostream & dot_output(std::ostream & os, GRAPH const & g, std::string const & name)
{
  // sort nodes and edges
  unsigned int count = 0;
  std::map<typename GRAPH::node_sptr_t, unsigned int, NODE_LE> node_map;
  std::multiset<typename GRAPH::edge_sptr_t, EDGE_LE> edge_set;
  for (typename GRAPH::node_sptr_t const & n : g.nodes()) {
    node_map.insert(std::make_pair(n, count));
    ++count;
    for (typename GRAPH::edge_sptr_t const & e : g.outgoing_edges(n))
      edge_set.insert(e);
  }

  // output graph
  std::map<std::string, std::string> attr;

  tchecker::graph::dot_output_header(os, name);

  for (auto && [node, id] : node_map) {
    attr.clear();
    g.attributes(node, attr);
    tchecker::graph::dot_output_node(os, std::to_string(id), attr);
  }

  for (typename GRAPH::edge_sptr_t const & edge : edge_set) {
    unsigned int src = node_map[g.edge_src(edge)];
    unsigned int tgt = node_map[g.edge_tgt(edge)];
    attr.clear();
    g.attributes(edge, attr);
    tchecker::graph::dot_output_edge(os, std::to_string(src), std::to_string(tgt), attr);
  }

  tchecker::graph::dot_output_footer(os);

  return os;
}

} // end of namespace graph

} // end of namespace tchecker

#endif // TCHECKER_GRAPH_OUTPUT_HH
