/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_NDFS_ALGORITHM_HH
#define TCHECKER_ALGORITHMS_NDFS_ALGORITHM_HH

#include <cassert>
#include <deque>
#include <iterator>
#include <stack>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/algorithms/ndfs/graph.hh"
#include "tchecker/algorithms/ndfs/stats.hh"

/*!
 \file algorithm.hh
 \brief Nested DFS algorithm
 */

namespace tchecker {

namespace algorithms {

namespace ndfs {

/*!
 \class algorithm_t
 \brief Nested DFS algorithm
 \tparam TS : type of transition system, should implement tchecker::ts::fwd_t
 and tchecker::ts::inspector_t
 \tparam GRAPH : type of graph, should derive from
 tchecker::graph::reachability_graph_t, and nodes of type GRAPH::shared_node_t
 should derive from tchecker::algorithms::ndfs::node_t and have a method
 state_ptr() that yields a pointer to the corresponding state in TS
 \note Our implementation is based on the nested DFS algorithm in:
 "Comparison of Algorithms for Checking Emptiness on Büchi Automata",
 Andreas Gaiser and Stefan Schwoon
 MEMICS 2009

 We have implemented the iterative translation of the recursive algorithm in
 Gaiser&Schwoon's paper.

 procedure ndfs(A)
   for each initial state s0 of A
     if s0 is white then
       dfs_blue(s0)

 procedure dfs_blue(s)
   push <s, post(s), true> on blue_stack
   while blue_stack is not empty
     <s, succ, allred> := top(blue_stack)
     if succ is empty then
       pop(blue_stack)
       if allred then
         s.color := red
       else if s is accepting
         call dfs_red(s)
         s.color := red
       else
         s.color := blue
         if blue_stack is not empty then
           top(blue_stack).allred := false
     else // still a successor to explore
       t := pick from succ
       if t.color = cyan and (s or t is accepting) then
         report cycle
       else if t.color = white then
         push <t, post(t), true> on blue_stack
       else if t.color != red then
         allred := false

 procedure dfs_red(s)
   push <s, post(s)> on red_stack
   while red_stack is not empty
     <s, succ> := top(red_stack)
     if succ is empty then
       pop(red_stack)
     else
       t := pick from succ
       if t.color = cyan then
         report cycle
       else if t.color = blue then
         t.color := red
         push <t, post(t)> on red_stack
 */
template <class TS, class GRAPH> class algorithm_t {
public:
  using node_sptr_t = typename GRAPH::node_sptr_t;

  /*!
   \brief Check if a transition has an infinite run that satisfies a given set
   of labels and build the corresponding graph
   \param ts : a transition system
   \param graph : a graph
   \param labels : accepting labels
   \post graph is built from a traversal of ts starting from its initial states,
   until a cycle that satisfies labels is reached (if any).
   A node is created for each reached state in ts, and an edge is created for
   each transition in ts.
   \return statistics on the run
   \note if labels is empty, graph is the full state-space of ts
   */
  tchecker::algorithms::ndfs::stats_t run(TS & ts, GRAPH & graph, boost::dynamic_bitset<> const & labels)
  {
    tchecker::algorithms::ndfs::stats_t stats;

    stats.set_start_time();

    std::vector<typename TS::sst_t> sst;
    ts.initial(sst);
    for (auto && [status, s, t] : sst) {
      auto && [is_new_node, initial_node] = graph.add_node(s);
      initial_node->initial(true);
      if (initial_node->color() == tchecker::algorithms::ndfs::WHITE)
        dfs_blue(ts, graph, labels, stats, initial_node);
      if (stats.cycle())
        break;
    }

    stats.stored_states() = graph.nodes_count();

    stats.set_end_time();

    return stats;
  }

private:
  /*!
   \brief Adds successor nodes to the graph
   \param ts : a transition system
   \param graph : a graph
   \param n : a node
   \post all successor nodes of n in ts have been added to graph (if not yet in)
   with corresponding edges
   \return all successor nodes of n
  */
  std::deque<node_sptr_t> expand_node(TS & ts, GRAPH & graph, node_sptr_t & n)
  {
    std::deque<node_sptr_t> next_nodes;
    std::vector<typename TS::sst_t> v;
    ts.next(n->state_ptr(), v);
    for (auto && [status, s, t] : v) {
      auto && [new_node, nextn] = graph.add_node(s);
      graph.add_edge(n, nextn, *t);
      next_nodes.push_back(nextn);
    }
    return next_nodes;
  }

  /*!
   \brief Type of entries of the blue DFS stack
   */
  struct blue_stack_entry_t {
    node_sptr_t n;                /*!< Node */
    std::deque<node_sptr_t> succ; /*!< Successors of node n that have not been visited yet */
    bool allred;                  /*!< True if all explored successors of n are red */

    /*!
     \brief Remove and return the first successor node
     \pre succ is not empty (checked by assertion)
     \return the first successor of node n
     \post the first successors of node n has been removed from succ
    */
    node_sptr_t pick_successor()
    {
      assert(!succ.empty());
      node_sptr_t n = succ.front();
      succ.pop_front();
      return n;
    }
  };

  /*!
   \brief Blue DFS from a node
   \param ts : a transition system
   \param graph : a graph
   \param labels : accepting labels
   \param stats : statistics
   \param n : node
  */
  void dfs_blue(TS & ts, GRAPH & graph, boost::dynamic_bitset<> const & labels, tchecker::algorithms::ndfs::stats_t & stats,
                node_sptr_t & n)
  {
    std::stack<blue_stack_entry_t> stack;

    n->color() = tchecker::algorithms::ndfs::CYAN;
    stack.push(blue_stack_entry_t{n, expand_node(ts, graph, n), true});
    ++stats.visited_states_blue();

    while (!stack.empty()) {
      auto && [s, succ, allred] = stack.top();
      if (succ.empty()) {
        if (allred)
          s->color() = tchecker::algorithms::ndfs::RED;
        else if (accepting(s, ts, labels)) {
          dfs_red(ts, graph, labels, stats, s);
          s->color() = tchecker::algorithms::ndfs::RED;
        }
        else
          s->color() = tchecker::algorithms::ndfs::BLUE;
        bool s_is_red = (s->color() == tchecker::algorithms::ndfs::RED);
        stack.pop();
        if (!s_is_red && !stack.empty())
          stack.top().allred = false;
      }
      else {
        node_sptr_t t = stack.top().pick_successor();
        ++stats.visited_transitions_blue();
        if (t->color() == tchecker::algorithms::ndfs::CYAN && (accepting(s, ts, labels) || accepting(t, ts, labels))) {
          stats.cycle() = true;
          break;
        }
        else if (t->color() == tchecker::algorithms::ndfs::WHITE) {
          t->color() = tchecker::algorithms::ndfs::CYAN;
          stack.push(blue_stack_entry_t{t, expand_node(ts, graph, t), true});
          ++stats.visited_states_blue();
        }
        else if (t->color() != tchecker::algorithms::ndfs::RED)
          allred = false;
      }
    }
  }

  /*!
   \brief Check if a node is accepting
   \param n : a node
   \param ts : a transition system
   \param labels : a set of labels
   \return true if labels is not empty, and labels is a subset of the labels of
   node n in ts, false otherwise
   */
  bool accepting(node_sptr_t const & n, TS & ts, boost::dynamic_bitset<> const & labels) const
  {
    return !labels.none() && labels.is_subset_of(ts.labels(n->state_ptr()));
  }

  /*!
   \brief Type of entries in the red DFS stack
  */
  struct red_stack_entry_t {
    node_sptr_t n;                                     /*!< Node */
    typename GRAPH::outgoing_edges_iterator_t current; /*!< Iterator on current successor node */
    typename GRAPH::outgoing_edges_iterator_t end;     /*!< Path-the-end iterator on successor nodes */

    /*!
     \brief Constructor
     \param n : a node
     \param r : range of outgoing edges of node n
    */
    red_stack_entry_t(node_sptr_t & n, tchecker::range_t<typename GRAPH::outgoing_edges_iterator_t> const & r)
        : n(n), current(r.begin()), end(r.end())
    {
    }

    /*!
     \brief Check emptiness of successor range
     \return true if the range [current; end) of successor nodes is empty, false
     otherwise
     */
    bool has_successor() const { return (current != end); }

    /*!
     \brief Remove and return the first successor node
     \param graph : a graph
     \pre the range [current,end) is not empty (checked by assertion)
     \return the first successor node of node n
     \post the first successor of node n has been removed from the range of successors
    */
    node_sptr_t pick_successor(GRAPH & graph)
    {
      assert(current != end);
      node_sptr_t next = graph.edge_tgt(*current);
      ++current;
      return next;
    }
  };

  /*!
   \brief Red DFS from a node
   \param ts : a transition system
   \param graph : a graph
   \param labels : accepting labels
   \param stats : statistics
   \param n : node
  */
  void dfs_red(TS & ts, GRAPH & graph, boost::dynamic_bitset<> const & labels, tchecker::algorithms::ndfs::stats_t & stats,
               node_sptr_t & n)
  {
    std::stack<red_stack_entry_t> stack;

    stack.push(red_stack_entry_t{n, graph.outgoing_edges(n)});
    ++stats.visited_states_red();

    while (!stack.empty()) {
      red_stack_entry_t & top = stack.top();
      if (!top.has_successor())
        stack.pop();
      else {
        node_sptr_t t = top.pick_successor(graph);
        ++stats.visited_transitions_red();
        if (t->color() == tchecker::algorithms::ndfs::CYAN) {
          stats.cycle() = true;
          break;
        }
        else if (t->color() == tchecker::algorithms::ndfs::BLUE) {
          t->color() = tchecker::algorithms::ndfs::RED;
          stack.push(red_stack_entry_t{t, graph.outgoing_edges(t)});
          ++stats.visited_states_red();
        }
      }
    }
  }
};

} // namespace ndfs

} // namespace algorithms

} // namespace tchecker

#endif // TCHECKER_ALGORITHMS_NDFS_ALGORITHM_HH