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
  using node_id_t = std::size_t;
  using extended_edge_t = std::tuple<node_id_t, node_id_t, typename GRAPH::edge_sptr_t>; // <src, tgt, edge_sptr>

  // Extend EDGE_LE on triples (src, tgt, edge)
  class extended_edge_le_t : private EDGE_LE {
  public:
    bool operator()(extended_edge_t const & e1, extended_edge_t const & e2) const
    {
      auto && [src1, tgt1, edge_sptr1] = e1;
      auto && [src2, tgt2, edge_sptr2] = e2;
      if (src1 < src2)
        return true;
      else if (src1 > src2)
        return false;
      else if (tgt1 < tgt2)
        return true;
      else if (tgt1 > tgt2)
        return false;
      else
        return EDGE_LE::operator()(edge_sptr1, edge_sptr2);
    }
  };

  // Sort nodes and given them an ID
  std::map<typename GRAPH::node_sptr_t, node_id_t, NODE_LE> nodes_map;
  for (typename GRAPH::node_sptr_t const & n : g.nodes())
    nodes_map.insert(std::make_pair(n, 0));

  node_id_t count = 0;
  for (auto & [node, id] : nodes_map) {
    id = count;
    ++count;
  }

  // Sort (extended) edges
  std::multiset<extended_edge_t, extended_edge_le_t> edges_set;
  for (typename GRAPH::node_sptr_t const & n : g.nodes())
    for (typename GRAPH::edge_sptr_t const & e : g.outgoing_edges(n)) {
      node_id_t src = nodes_map[g.edge_src(e)];
      node_id_t tgt = nodes_map[g.edge_tgt(e)];
      edges_set.insert(std::make_tuple(src, tgt, e));
    }

  // output graph
  std::map<std::string, std::string> attr;

  tchecker::graph::dot_output_header(os, name);

  for (auto && [node, id] : nodes_map) {
    attr.clear();
    g.attributes(node, attr);
    tchecker::graph::dot_output_node(os, std::to_string(id), attr);
  }

  for (auto && [src, tgt, edge] : edges_set) {
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
