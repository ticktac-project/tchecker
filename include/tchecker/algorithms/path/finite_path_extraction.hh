/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_FINITE_PATH_EXTRACTION_HH
#define TCHECKER_ALGORITHMS_FINITE_PATH_EXTRACTION_HH

#include <cassert>
#include <functional>
#include <stack>
#include <tuple>
#include <unordered_set>
#include <vector>

#include "tchecker/utils/iterator.hh"

/*!
 \file finite_path_extraction.hh
 \brief Finite path extraction algorithm
 */

namespace tchecker {

namespace algorithms {

/*!
 \class finite_path_extraction_algorithm_t
 \brief Finite path extraction algorithm
 \tparam GRAPH : type of graph. Should provide:
 - a type of shared nodes GRAPH::node_sptr_t
 - a type of shared edges GRAPH::edge_sptr_t
 - a type of outgoing edges iterator GRAPH::outgoing_edges_iterator_t
 - a method nodes() that returns the range of nodes in the graph
 - a method outgoing_edges(n) that returns the range of outgoing edges of node n
 See tchecker::graph::reachability::graph_t, tchecker::graph::reachability::multigraph_t
 and tchecker::graph::subsumption::graph_t as examples of such graphs
 */
template <class GRAPH> class finite_path_extraction_algorithm_t {
public:
  /*!
   \brief Type of pointer to node
  */
  using node_sptr_t = typename GRAPH::node_sptr_t;

  /*!
   \brief Typoe of pointer to edge
  */
  using edge_sptr_t = typename GRAPH::edge_sptr_t;

  /*!
   \brief Extract a finite sequence of edges from a graph
   \param g : a graph
   \param filter_first : predicate on nodes
   \param filter_last : predicate on nodes
   \param filter_edge : predicate on edges
   \return a triple (true, n, seq) if a (possibly empty) sequence of edges has been found from n, that satisfies filter_first,
   to a node that satisfies filter_last, where all edges on seq satisfy filter_edge,
   (false, nullptr, seq) if no such path exist where seq is empty
  */
  std::tuple<bool, node_sptr_t, std::vector<edge_sptr_t>> run(GRAPH const & g, std::function<bool(node_sptr_t)> && filter_first,
                                                              std::function<bool(node_sptr_t)> && filter_last,
                                                              std::function<bool(edge_sptr_t)> && filter_edge)
  {
    std::unordered_set<node_sptr_t> visited;
    auto r = g.nodes();
    for (node_sptr_t n : r) {
      if (!filter_first(n))
        continue;
      auto && [found, seq] = find_sequence(g, n, std::move(filter_last), std::move(filter_edge), visited);
      if (found) {
        assert(seq.empty() || g.edge_src(seq[0]) == n);
        return std::make_tuple(true, n, seq);
      }
    }
    return std::make_tuple(false, nullptr, std::vector<edge_sptr_t>{});
  }

private:
  /*!
   \brief Waiting stack entry for depth-first search
  */
  class dfs_entry_t {
  public:
    /*!
     \brief Constructor
     \param n : a node
    */
    dfs_entry_t(node_sptr_t const & n, tchecker::range_t<typename GRAPH::outgoing_edges_iterator_t> const & r)
        : _n(n), _current_edge(r.begin()), _end_edge(r.end())
    {
    }

    /*!
     \brief Checks if node has been fully explored
    */
    bool fully_explored() const { return _current_edge == _end_edge; }

    /*!
     \brief Move current edge to next edge
     \pre not fully explored (checked by assertion)
    */
    void next()
    {
      assert(!fully_explored());
      ++_current_edge;
    }

    /*!
     \brief Accessor
     \return node
    */
    inline node_sptr_t node() const { return _n; }

    /*!
     \brief Accessor
     \pre not fully explored (checked by assertion)
     \return current node
    */
    inline edge_sptr_t current_edge() const { return *_current_edge; }

  private:
    node_sptr_t _n;                                          /*!< Current node */
    typename GRAPH::outgoing_edges_iterator_t _current_edge; /*!< Current outgoing edge of n */
    typename GRAPH::outgoing_edges_iterator_t _end_edge;     /*!< Past-the-end outgoing edge of n */
  };

  /*!
   \brief Depth-first search from n to a node satisfying last
   \param g : a graph
   \param n : a node
   \param filter_last : predicate on nodes
   \param filter_edge : predicate on edges
   \param visited : set of nodes that have already been visited
   \return a pair (true, seq) if a (possibly empty) sequence of edges seq has been found from n
   to a node satisfying filter_last where all edges satisfy filter_edge,
   (false, seq) whwre seq is empty otherwise
   \return a finite sequence of edges from g, all satisfying filter_edge, and
   that leads from n to a node satisfying filter_last if any, an empty sequence
   otherwise
   \post all nodes visited during the search have been added to visited
   \note nodes in visited are not explored
  */
  std::tuple<bool, std::vector<edge_sptr_t>> find_sequence(GRAPH const & g, node_sptr_t const & n,
                                                           std::function<bool(node_sptr_t)> && filter_last,
                                                           std::function<bool(edge_sptr_t)> && filter_edge,
                                                           std::unordered_set<node_sptr_t> & visited)
  {
    std::vector<edge_sptr_t> seq;

    if (filter_last(n))
      return std::make_tuple(true, seq);

    std::stack<dfs_entry_t> waiting;

    waiting.push(dfs_entry_t{n, g.outgoing_edges(n)});
    visited.insert(n);

    while (!waiting.empty()) {
      if (waiting.top().fully_explored()) {
        waiting.pop();
        seq.pop_back();
        continue;
      }

      edge_sptr_t e = waiting.top().current_edge();
      waiting.top().next();

      if (!filter_edge(e))
        continue;

      node_sptr_t nextn = g.edge_tgt(e);

      if (visited.find(nextn) != visited.end())
        continue;

      seq.push_back(e);

      if (filter_last(nextn))
        return std::make_tuple(true, seq);

      waiting.push(dfs_entry_t{nextn, g.outgoing_edges(nextn)});
      visited.insert(nextn);
    }

    assert(seq.empty());

    return std::make_tuple(false, seq);
  }
};

} // namespace algorithms

} // namespace tchecker

#endif // TCHECKER_ALGORITHMS_FINITE_PATH_EXTRACTION_HH