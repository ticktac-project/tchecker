/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_LASSO_PATH_EXTRACTION_HH
#define TCHECKER_ALGORITHMS_LASSO_PATH_EXTRACTION_HH

#include <cassert>
#include <functional>
#include <stack>
#include <unordered_set>
#include <vector>

#include "tchecker/utils/iterator.hh"

/*!
 \file lasso_path_extraction.hh
 \brief Lasso path extraction algorithm
 */

namespace tchecker {

namespace algorithms {

/*!
 \class lasso_path_extraction_algorithm_t
 \brief Lasso path extraction algorithm
 \tparam GRAPH : type of graph. Should provide:
 - a type of shared nodes GRAPH::node_sptr_t
 - a type of shared edges GRAPH::edge_sptr_t
 - a type of outgoing edges iterator GRAPH::outgoing_edges_iterator_t
 - a method nodes() that returns the range of nodes in the graph
 - a method outgoing_edges(n) that returns the range of outgoing edges of node n
 See tchecker::graph::reachability::graph_t, tchecker::graph::reachability::multigraph_t
 and tchecker::graph::subsumption::graph_t as examples of such graphs
*/
template <class GRAPH> class lasso_path_extraction_algorithm_t {
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
   \brief Type of lasso path edges
  */
  struct lasso_edges_t {
    /*!
     \brief Checks if the lasso path is not empty
     \return true if cycle is not empty, false otherwise
    */
    inline bool empty() const { return cycle.empty(); }

    std::vector<edge_sptr_t> prefix; /*!< Prefix of the lasso path */
    std::vector<edge_sptr_t> cycle;  /*!< Cycle of the lasso path */
  };

  /*!
   \brief Extract lasso path edges from a graph
   \param g : a graph
   \param filter_first : predicate on nodes
   \param filter_final : predicate on nodes
   \param filter_edge : predicate on edges
   \return a lasso path (prefix, cycle) of edges from g, all satisfying filter_edge,
   starting from a node that satisfies filter_first,
   and such that cycle contains a node that satisfies filter_final,
   the empty lasso path if no such sequences of edges exist
  */
  lasso_edges_t run(GRAPH const & g, std::function<bool(GRAPH const &, node_sptr_t)> && filter_first,
                    std::function<bool(GRAPH const &, node_sptr_t)> && filter_final,
                    std::function<bool(GRAPH const &, edge_sptr_t)> && filter_edge)
  {
    std::unordered_set<node_sptr_t> cyan, blue, red;

    auto r = g.nodes();
    for (node_sptr_t n : r) {
      if (!filter_first(g, n))
        continue;
      lasso_edges_t lasso_edges = find_lasso_edges(g, n, std::move(filter_final), std::move(filter_edge), cyan, blue, red);
      if (!lasso_edges.empty())
        return lasso_edges;
    }

    return lasso_edges_t{};
  }

private:
  /*!
     \brief Waiting stack entry for nested depth-first search
    */
  class ndfs_entry_t {
  public:
    /*!
     \brief Constructor
     \param n : a node
    */
    ndfs_entry_t(node_sptr_t const & n, tchecker::range_t<typename GRAPH::outgoing_edges_iterator_t> const & r)
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
   \brief Nested depth-first search from n to a node satisfying filter_final
   \param g : a graph
   \param n : a node
   \param filter_final : predicate on nodes
   \param filter_edge : predicate on edges
   \param cyan : set of cyan nodes in the nested DFS algorithm
   \param blue : set of blue nodes in the nested DFS algorithm
   \param red : set of red nodes in the nested DFS algorithm
   \return a lasso (prefix, cycle) of edges from g, all satisfying filter_edge, and
   that leads from n to a node satisfying filter_final if any,
   an empty lasso otherwise
   \post all nodes visited during the search have been added to cyan, blue or red depending on
   the nested DFS algorithm
  */
  lasso_edges_t find_lasso_edges(GRAPH const & g, node_sptr_t const & n,
                                 std::function<bool(GRAPH const &, node_sptr_t)> && filter_final,
                                 std::function<bool(GRAPH const &, edge_sptr_t)> && filter_edge,
                                 std::unordered_set<node_sptr_t> & cyan, std::unordered_set<node_sptr_t> & blue,
                                 std::unordered_set<node_sptr_t> & red)
  {
    lasso_edges_t lasso_edges;

    std::vector<edge_sptr_t> edges = blue_dfs(g, n, std::move(filter_final), std::move(filter_edge), cyan, blue, red);

    if (edges.empty())
      return lasso_edges;

    node_sptr_t loop_root = g.edge_tgt(edges.back());
    node_sptr_t first = g.edge_src(edges.front());

    auto it = edges.begin();
    edge_sptr_t edge = nullptr;

    // prefix, if non-empty
    if (loop_root != first) {
      do {
        assert(it != edges.end());
        edge = *it;
        lasso_edges.prefix.push_back(edge);
        ++it;
      } while (g.edge_tgt(edge) != loop_root);
    }

    // cycle
    for (; it != edges.end(); ++it)
      lasso_edges.cycle.push_back(*it);

    return lasso_edges;
  }

  /*!
   \brief Blue DFS search
   \param g : a graph
   \param n : a node
   \param filter_final : predicate on nodes
   \param filter_edge : predicate on edges
   \param cyan : set of cyan nodes in the nested DFS algorithm
   \param blue : set of blue nodes in the nested DFS algorithm
   \param red : set of red nodes in the nested DFS algorithm
   \return a sequence of edges, all satisfying filter_edge, that form a lasso path if a lasso path
   from n to a node that satisfies filter_final exists in g, an empty sequence of edges otherwise.
   The source node of the first edge is n, and the target node of the last edge is the loop root node of the
   lasso path
   */
  std::vector<edge_sptr_t> blue_dfs(GRAPH const & g, node_sptr_t const & n,
                                    std::function<bool(GRAPH const &, node_sptr_t)> && filter_final,
                                    std::function<bool(GRAPH const &, edge_sptr_t)> && filter_edge,
                                    std::unordered_set<node_sptr_t> & cyan, std::unordered_set<node_sptr_t> & blue,
                                    std::unordered_set<node_sptr_t> & red)
  {
    std::stack<ndfs_entry_t> stack;
    std::vector<edge_sptr_t> edges;

    stack.push(ndfs_entry_t{n, g.outgoing_edges(n)});
    cyan.insert(n);

    while (!stack.empty()) {
      ndfs_entry_t & entry = stack.top();

      if (!entry.fully_explored()) {
        edge_sptr_t edge = entry.current_edge();
        entry.next();

        if (!filter_edge(g, edge))
          continue;

        node_sptr_t next = g.edge_tgt(edge);
        if ((cyan.find(next) == cyan.end()) && (blue.find(next) == blue.end())) {
          stack.push(ndfs_entry_t{next, g.outgoing_edges(next)});
          cyan.insert(next);
          edges.push_back(edge);
        }
      }
      else {
        if (filter_final(g, entry.node())) {
          std::vector<edge_sptr_t> red_edges = red_dfs(g, entry.node(), std::move(filter_edge), cyan, blue, red);
          if (!red_edges.empty()) { // lasso path found
            for (edge_sptr_t e : red_edges)
              edges.push_back(e);
            return edges;
          }
        }
        blue.insert(entry.node());
        cyan.erase(entry.node());
        stack.pop();
        if (!edges.empty())
          edges.pop_back();
      }
    }

    assert(edges.empty());
    return edges;
  }

  /*!
   \brief Red DFS search
   \param g : a graph
   \param n : a node
   \param filter_final : predicate on nodes
   \param filter_edge : predicate on edges
   \param cyan : set of cyan nodes in the nested DFS algorithm
   \param blue : set of blue nodes in the nested DFS algorithm
   \param red : set of red nodes in the nested DFS algorithm
   \return a sequence of edges, all satisfying filter_edge, from n to a cyan n ode ifsuch a path exists,
   the empty sequence of edges otherwise
   */
  std::vector<edge_sptr_t> red_dfs(GRAPH const & g, node_sptr_t const & n,
                                   std::function<bool(GRAPH const &, edge_sptr_t)> && filter_edge,
                                   std::unordered_set<node_sptr_t> & cyan, std::unordered_set<node_sptr_t> & blue,
                                   std::unordered_set<node_sptr_t> & red)
  {
    std::stack<ndfs_entry_t> stack;
    std::vector<edge_sptr_t> red_edges;

    stack.push(ndfs_entry_t{n, g.outgoing_edges(n)});
    red.insert(n);

    while (!stack.empty()) {
      ndfs_entry_t & entry = stack.top();

      if (entry.fully_explored()) {
        stack.pop();
        if (!red_edges.empty())
          red_edges.pop_back();
      }
      else {
        edge_sptr_t edge = entry.current_edge();
        entry.next();

        if (!filter_edge(g, edge))
          continue;

        node_sptr_t next = g.edge_tgt(edge);
        if (cyan.find(next) != cyan.end()) { // cycle found
          red_edges.push_back(edge);
          return red_edges;
        }
        else if (red.find(next) == red.end()) {
          stack.push(ndfs_entry_t{next, g.outgoing_edges(next)});
          red.insert(next);
          red_edges.push_back(edge);
        }
      }
    }

    assert(red_edges.empty());
    return red_edges;
  }
};

} // namespace algorithms

} // namespace tchecker

#endif // TCHECKER_ALGORITHMS_LASSO_PATH_EXTRACTION_HH
