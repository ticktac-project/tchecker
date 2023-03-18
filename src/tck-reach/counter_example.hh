/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TCK_REACH_COUNTER_EXAMPLE_HH
#define TCHECKER_TCK_REACH_COUNTER_EXAMPLE_HH

#include <memory>
#include <vector>

#include "tchecker/algorithms/path/algorithm.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/zg/path.hh"
#include "tchecker/zg/zg.hh"

/*!
 \file counter_example.hh
 \brief Generation of counter examples
*/

namespace tchecker {

namespace tck_reach {

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
 \brief Compute a counter example over the zone graph
 \tparam GRAPH : type of graph, see tchecker::algorithms::path::finite::algorithm_t for requirements
 \tparam CEX : type of counter example, should inherit from tchecker::zg::finite_ath_t
 \param g : a graph over the zone graph (reachability graph, subsumption graph, etc)
 \return a finite path from an initial node of g to a final node of g
 */
template <class GRAPH, class CEX> CEX * counter_example_zg(GRAPH const & g)
{
  std::shared_ptr<tchecker::zg::zg_t> zg{
      tchecker::zg::factory(g.zg().system_ptr(), tchecker::zg::STANDARD_SEMANTICS, tchecker::zg::NO_EXTRAPOLATION, 128, 128)};

  // compute sequence of edges from initial to final node in g
  tchecker::algorithms::path::finite::algorithm_t<GRAPH> algorithm;

  std::vector<typename GRAPH::edge_sptr_t> seq =
      algorithm.run(g, &tchecker::tck_reach::initial_node<GRAPH>, &tchecker::tck_reach::final_node<GRAPH>,
                    &tchecker::tck_reach::true_edge<GRAPH>);

  if (seq.empty())
    return new CEX{zg};

  // Extract sequence of vedges
  std::vector<tchecker::const_vedge_sptr_t> vedge_seq;
  for (typename GRAPH::edge_sptr_t const & e : seq)
    vedge_seq.push_back(e->vedge_ptr());

  // Get the corresponding run in a zone graph with standard semantics and no extrapolation
  tchecker::vloc_t const & initial_vloc = g.edge_src(seq[0])->state().vloc();
  CEX * cex = tchecker::zg::compute_run(zg, initial_vloc, tchecker::make_range(vedge_seq));
  if (!cex->empty()) {
    cex->first()->initial(true);
    cex->last()->final(true);
  }

  return cex;
}

} // namespace tck_reach

} // namespace tchecker

#endif // TCHECKER_TCK_REACH_COUNTER_EXAMPLE_HH
