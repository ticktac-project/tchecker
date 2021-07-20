/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_REACH_ALGORITHM_HH
#define TCHECKER_ALGORITHMS_REACH_ALGORITHM_HH

#include <memory>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/algorithms/reach/stats.hh"
#include "tchecker/basictypes.hh"
#include "tchecker/waiting/factory.hh"

/*!
 \file algorithm.hh
 \brief Reachability algorithm
 */

namespace tchecker {

namespace algorithms {

namespace reach {

/*!
 \class algorithm_t
 \brief Reachability algorithm
 \tparam TS : type of transition system, should derive from tchecker::ts::ts_t
 \tparam GRAPH : type of graph, should derive from
 tchecker::graph::reachability_graph_t, and nodes of type GRAPH::shared_node_t
 should have a method state_ptr() that yields a pointer to the corresponding
 state in TS
 */
template <class TS, class GRAPH> class algorithm_t {
public:
  /*!
   \brief Build a reachability graph of a transition system from its initial
   states
   \param ts : a transition system
   \param graph : a graph
   \param labels : accepting labels
   \param policy : waiting list policy
   \post graph is built from a traversal of ts starting from its initial states,
   until a state that satisfies labels is reached (if any).
   A node is created for each reachable state in ts, and an edge is created for
   each transition in ts. The order in which the nodes of ts are visited depends
   on policy.
   \return statistics on the run
   \note if labels is empty, graph is the full reachability graph of ts
   */
  tchecker::algorithms::reach::stats_t run(TS & ts, GRAPH & graph, boost::dynamic_bitset<> const & labels,
                                           enum tchecker::waiting::policy_t policy)
  {
    using node_sptr_t = typename GRAPH::node_sptr_t;

    std::unique_ptr<tchecker::waiting::waiting_t<node_sptr_t>> waiting{tchecker::waiting::factory<node_sptr_t>(policy)};

    tchecker::algorithms::reach::stats_t stats;

    stats.set_start_time();

    std::vector<typename TS::sst_t> sst;
    ts.initial(sst, tchecker::STATE_OK);
    for (auto && [status, s, t] : sst) {
      auto && [is_new_node, initial_node] = graph.add_node(s);
      if (is_new_node)
        waiting->insert(initial_node);
    }

    run_from_waiting(ts, graph, labels, *waiting, stats);

    stats.set_end_time();

    return stats;
  }

  /*!
  \brief Build a reachability graph of a transition system from a waiting
  container
  \param ts : a transition system
  \param graph : a graph
  \param labels : accepting labels
  \param waiting : a waiting container
  \post graph is built from a traversal of ts starting from the nodes in
  waiting, until a state that satisfies labels is reached (if any).
  A node is created for each reachable state in ts, and an edge is
  created for each transition is ts. The order in which the nodes of ts are
  visited depends on the policy implemented by waiting
  \return statistics on the run
  \note if labels is empty, graph is the full reachability graph of ts from the
  nodes in waiting
  */
  tchecker::algorithms::reach::stats_t run(TS & ts, GRAPH & graph, boost::dynamic_bitset<> const & labels,
                                           tchecker::waiting::waiting_t<typename GRAPH::node_sptr_t> & waiting)
  {
    tchecker::algorithms::reach::stats_t stats;

    stats.set_start_time();
    run_from_waiting(ts, graph, labels, waiting, stats);
    stats.set_end_time();

    return stats;
  }

private:
  /*!
  \brief Build a reachability graph of a transition system from a waiting
  container
  \param ts : a transition system
  \param graph : a graph
  \param labels : accepting labels
  \param waiting : a waiting container
  \param stats : statistics
  \post graph is built from a traversal of ts starting from the nodes in
  waiting, until a state that satisfies labels is reached (if any).
  A node is created for each reachable state in ts, and an edge is
  created for each transition is ts. The order in which the nodes of ts are
  visited depends on the policy implemented by waiting.
  The number of visited nodes and reachability of a satisfying node have been
  set in stats.
  */
  void run_from_waiting(TS & ts, GRAPH & graph, boost::dynamic_bitset<> const & labels,
                        tchecker::waiting::waiting_t<typename GRAPH::node_sptr_t> & waiting,
                        tchecker::algorithms::reach::stats_t & stats)
  {
    using node_sptr_t = typename GRAPH::node_sptr_t;

    std::vector<typename TS::sst_t> sst;

    while (!waiting.empty()) {
      node_sptr_t node = waiting.first();
      waiting.remove_first();

      ++stats.visited_states();

      if (ts.satisfies(node->state_ptr(), labels)) {
        stats.reachable() = true;
        break;
      }

      ts.next(node->state_ptr(), sst, tchecker::STATE_OK);
      for (auto && [status, s, t] : sst) {
        auto && [is_new_node, next_node] = graph.add_node(s);
        if (is_new_node)
          waiting.insert(next_node);
        graph.add_edge(node, next_node, *t);
      }
      sst.clear();
    }

    waiting.clear();
  }
};

} // end of namespace reach

} // end of namespace algorithms

} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_REACH_ALGORITHM_HH
