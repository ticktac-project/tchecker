/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COVREACH_ALGORITHM_HH
#define TCHECKER_ALGORITHMS_COVREACH_ALGORITHM_HH

/*!
 \file algorithm.hh
 \brief Reachability algorithm with covering
 */

#include <iterator>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/algorithms/covreach/stats.hh"
#include "tchecker/graph/subsumption_graph.hh"
#include "tchecker/waiting/factory.hh"

namespace tchecker {

namespace algorithms {

namespace covreach {

/*!
 \class algorithm_t
 \brief Covering reachability algorithm
 \tparam TS : type of transition system, should derive from tchecker::ts::ts_t
 \tparam GRAPH : type of graph, should derive from
 tchecker::graph::subsumption::graph_t, and nodes of type GRAPH::shared_node_t
 should have a method state)ptr() that yields a pointer to the corresponding
 state in TS.
 For correctness of the algorithm, the covering relation over nodes in GRAPH
 should be a trace inclusion, and it should be irreflexive: a node should not
 cover itself
*/
template <class TS, class GRAPH> class algorithm_t {
public:
  /*!
   \brief Build a covering reachability graph of a transition system from its
   initial states
   \param ts : a transition system
   \param graph : a graph
   \param labels : accepting labels
   \param policy : waiting list policy
   \post graph is a covering reachability graph of ts built from its initial
   states, until a state that satisfies labels is reached if any, or until the
   entire state-space has been exhausted.
   A node is created for each maximal state in ts, and an edge is created for
   each transition in ts. Actual edges correspond to transitions in ts. A
   subsumption edge from node n1 to node n2 means that the actual successor of
   n1 in ts is subsumed by n2.
   The order in which the nodes of ts are visited depends on policy.
   \return Statistics on the run
   \note if labels is empty, the algorithm explores the entire state-space
  */
  tchecker::algorithms::covreach::stats_t run(TS & ts, GRAPH & graph, boost::dynamic_bitset<> const & labels,
                                              enum tchecker::waiting::policy_t policy)
  {
    using node_sptr_t = typename GRAPH::node_sptr_t;

    std::unique_ptr<tchecker::waiting::waiting_t<node_sptr_t>> waiting{tchecker::waiting::factory<node_sptr_t>(policy)};
    tchecker::algorithms::covreach::stats_t stats;
    std::vector<node_sptr_t> nodes, covered_nodes;

    stats.set_start_time();

    expand_initial_nodes(ts, graph, nodes, stats);
    for (node_sptr_t const & n : nodes)
      waiting->insert(n);
    nodes.clear();

    while (!waiting->empty()) {
      node_sptr_t node = waiting->first();
      waiting->remove_first();

      ++stats.visited_states();

      if (ts.satisfies(node->state_ptr(), labels)) {
        stats.reachable() = true;
        break;
      }

      expand_next_nodes(node, ts, graph, nodes, stats);

      for (node_sptr_t const & next_node : nodes) {
        waiting->insert(next_node);
        remove_covered_nodes(graph, next_node, covered_nodes, stats);
        for (node_sptr_t const & covered_node : covered_nodes)
          waiting->remove(covered_node);
        covered_nodes.clear();
      }
      nodes.clear();
    }

    waiting->clear();

    stats.stored_states() = graph.nodes_count();

    stats.set_end_time();

    return stats;
  }

  /*!
   \brief Create nodes for initial states
   \param ts : transition system
   \param graph : a subsumption graph
   \param initial_nodes : nodes container
   \param stats : statistics
   \post A node has been created in graph for each initial state of ts which is
   maximal w.r.t. the node covering in graph. All these maximal nodes have been
   added to initial_nodes
   All covered initial nodes have been counted in stats
   */
  void expand_initial_nodes(TS & ts, GRAPH & graph, std::vector<typename GRAPH::node_sptr_t> & initial_nodes,
                            tchecker::algorithms::covreach::stats_t & stats)
  {
    std::vector<typename TS::sst_t> sst;
    typename GRAPH::node_sptr_t covering_node;

    ts.initial(sst);
    for (auto && [status, s, t] : sst) {
      typename GRAPH::node_sptr_t n = graph.add_node(s);
      if (graph.is_covered(n, covering_node)) {
        graph.remove_node(n);
        ++stats.covered_states();
      }
      else
        initial_nodes.push_back(n);
    }
  }

  /*!
   \brief Create successor nodes of a node
   \param node : a node
   \param ts : a transition system
   \param graph : a subsumption graph
   \param next_nodes : nodes container
   \param stats : statistics
   \post A node has been created in the graph for each successor of node that
   is maximal in graph. An actual edge has been created from node to each
   maximal successor. All maximal successors have been added to next_nodes.
   For each successor node that is not maximal, a subsumption edge has been
   created from node to a covering node.
   All covered successor nodes have been counted in stats.
   */
  void expand_next_nodes(typename GRAPH::node_sptr_t const & node, TS & ts, GRAPH & graph,
                         std::vector<typename GRAPH::node_sptr_t> & next_nodes, tchecker::algorithms::covreach::stats_t & stats)
  {
    std::vector<typename TS::sst_t> sst;
    typename GRAPH::node_sptr_t covering_node;

    ts.next(node->state_ptr(), sst);
    for (auto && [status, s, t] : sst) {
      typename GRAPH::node_sptr_t next_node = graph.add_node(s);
      if (graph.is_covered(next_node, covering_node)) {
        graph.add_edge(node, covering_node, tchecker::graph::subsumption::EDGE_SUBSUMPTION, *t);
        graph.remove_node(next_node);
        ++stats.covered_states();
      }
      else {
        graph.add_edge(node, next_node, tchecker::graph::subsumption::EDGE_ACTUAL, *t);
        next_nodes.push_back(next_node);
      }
    }
  }

  /*!
   \brief Remove non-maximal nodes
   \param graph : a subsumption graph
   \param node : a node
   \param covered_nodes : a container of nodes
   \param stats : statistics
   \post All the nodes in graph that are covered by node have been removed from
   graph and added to covered_nodes.
   All incoming edges to covered nodes have been transformed into incoming
   subsumption edges of node.
   Removed nodes have been counted in stats
  */
  void remove_covered_nodes(GRAPH & graph, typename GRAPH::node_sptr_t const & node,
                            std::vector<typename GRAPH::node_sptr_t> & covered_nodes,
                            tchecker::algorithms::covreach::stats_t & stats)
  {
    auto covered_nodes_inserter = std::back_inserter(covered_nodes);

    covered_nodes.clear();
    graph.covered_nodes(node, covered_nodes_inserter);
    for (typename GRAPH::node_sptr_t const & covered_node : covered_nodes) {
      graph.move_incoming_edges(covered_node, node, tchecker::graph::subsumption::EDGE_SUBSUMPTION);
      graph.remove_edges(covered_node);
      graph.remove_node(covered_node);
      ++stats.covered_states();
    }
  }
};

} // end of namespace covreach

} // end of namespace algorithms

} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COVREACH_ALGORITHM_HH
