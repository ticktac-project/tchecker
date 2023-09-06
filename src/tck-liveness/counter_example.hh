/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TCK_LIVENESS_COUNTER_EXAMPLE_HH
#define TCHECKER_TCK_LIVENESS_COUNTER_EXAMPLE_HH

#include <memory>
#include <vector>

#include "tchecker/algorithms/path/lasso_path_extraction.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/zg/path.hh"
#include "tchecker/zg/zg.hh"

/*!
 \file counter_example.hh
 \brief Generation of counter examples
*/

namespace tchecker {

namespace tck_liveness {

/*!
 \brief Check if a node is initial
 \tparam GRAPH : type of graph
 \param n : a node
 \return true if n is an initial node, false otherwise
*/
template <class GRAPH> bool initial_node(typename GRAPH::node_sptr_t const & n) { return n->initial(); }

/*!
 \brief Check is a node is final
 \tparam GRAPH : type of graph
 \param n : a node
 \return true if n is a final node, false otherwise
*/
template <class GRAPH> bool final_node(typename GRAPH::node_sptr_t const & n) { return n->final(); }

/*!
 \brief True predicate over edges
 \tparam GRAPH : type of graph
 \param e : an edge
 \return true
*/
template <class GRAPH> bool true_edge(typename GRAPH::edge_sptr_t const & e) { return true; }

/*!
 \brief Compute a symbolic counter example of a zone graph
 \tparam GRAPH : type of graph, see tchecker::algorithms::path::finite::algorithm_t for requirements
 \param g : a graph over the zone graph (reachability graph, subsumption graph, etc)
 \return a lasso path from an initial node of g that visits infinitely often a final node of g
 */
template <class GRAPH> tchecker::zg::path::symbolic::lasso_path_t * symbolic_counter_example_zg(GRAPH const & g)
{
  // compute lasso sequence of edges from initial node in g with a cycle over a final node
  tchecker::algorithms::lasso_path_extraction_algorithm_t<GRAPH> algorithm;

  typename tchecker::algorithms::lasso_path_extraction_algorithm_t<GRAPH>::lasso_edges_t lasso_edges =
      algorithm.run(g, &tchecker::tck_liveness::initial_node<GRAPH>, &tchecker::tck_liveness::final_node<GRAPH>,
                    &tchecker::tck_liveness::true_edge<GRAPH>);

  if (lasso_edges.empty())
    return new tchecker::zg::path::symbolic::lasso_path_t{g.zg_ptr()};

  // Extract prefix and cycle sequences of vedges
  std::vector<tchecker::const_vedge_sptr_t> prefix_vedges, cycle_vedges;
  for (typename GRAPH::edge_sptr_t const & e : lasso_edges.prefix)
    prefix_vedges.push_back(e->vedge_ptr());
  for (typename GRAPH::edge_sptr_t const & e : lasso_edges.cycle)
    cycle_vedges.push_back(e->vedge_ptr());

  // Get the corresponding run in a zone graph
  tchecker::vloc_t const & initial_vloc = g.edge_src(lasso_edges.prefix[0])->state().vloc();

  tchecker::zg::path::symbolic::lasso_path_t * lasso_path = tchecker::zg::path::symbolic::compute_lasso_path(
      g.zg_ptr(), initial_vloc, prefix_vedges, cycle_vedges, [](tchecker::zg::state_t const & s) { return false; });

  // Mark final nodes according to extracted prefix and cycle
  tchecker::zg::path::symbolic::lasso_path_t::node_sptr_t node = lasso_path->first();

  for (typename GRAPH::edge_sptr_t const & e : lasso_edges.prefix) {
    node->final(g.edge_src(e)->final());
    auto node_outgoing_edges = lasso_path->outgoing_edges(node);
    assert(!node_outgoing_edges.empty());
    node = lasso_path->edge_tgt(*node_outgoing_edges.begin());
  }

  assert(node == lasso_path->loop_root());

  for (typename GRAPH::edge_sptr_t const & e : lasso_edges.cycle) {
    node->final(g.edge_src(e)->final());
    auto node_outgoing_edges = lasso_path->outgoing_edges(node);
    assert(!node_outgoing_edges.empty());
    node = lasso_path->edge_tgt(*node_outgoing_edges.begin());
  }

  return lasso_path;
}

} // namespace tck_liveness

} // namespace tchecker

#endif // TCHECKER_TCK_LIVENESS_COUNTER_EXAMPLE_HH
