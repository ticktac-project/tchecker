/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COUVREUR_SCC_ALGORITHM_HH
#define TCHECKER_ALGORITHMS_COUVREUR_SCC_ALGORITHM_HH

#include <cassert>
#include <stack>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/algorithms/couvreur_scc/graph.hh"
#include "tchecker/algorithms/couvreur_scc/stats.hh"

/*!
 \file algorithm.hh
 \brief Couvreur's SCC-decomposition-based liveness algorithm
 */

namespace tchecker {

namespace algorithms {

namespace couvscc {

/*!
 \class generalized_algorithm_t
 \brief Couvreur's SCC-decomposition-based liveness algorithm for generalized Büchi conditions
 \tparam TS : type of transition system, should implement tchecker::ts::fwd_t
 and tchecker::ts::inspector_t
 \tparam GRAPH : type of graph, should derive from
 tchecker::graph::reachability_graph_t, and nodes of type GRAPH::shared_node_t
 should derive from tchecker::algorithms::couvscc::node_t and have a method
 state_ptr() that yields a pointer to the corresponding state in TS
 \note Implementation based on the variant of Couvreur's algorithm in:
 "Comparison of Algorithms for Checking Emptiness on Büchi Automata",
 Andreas Gaiser and Stefan Schwoon
 MEMICS 2009

 We have implemented the iterative translation of the recursive algorithm in
 Gaiser&Schwoon's paper. The algorithm below works for generalised Büchi conditions.

 procedure couvscc(A)
   count := 0
   Roots := {}
   Active := {}
   Todo := {}
   for each initial state s0 of A
     if s0.dfsnum = 0 then
       couv_dfs(s0)

 procedure push(s)
   count := count + 1
   s.dfsnum := count
   s.current := true
   push(Roots, <s, labels(s)>)
   push(Active, <s>)
   push(Todo, <s, post(s)>)

 procedure couv_dfs(s)
   push(s)
   while (Todo is not empty) do
     <n, succ> := top(Todo)
     if (succ is empty) then
       if (top(Roots) = <n, ?>) then
         close_scc(n)
       pop(Todo)
     else
       t := pick(succ)
       if (t.dfsnum = 0) then
         push(t)
       else if t.current then
         merge_scc(t)

 procedure close_scc(n)
   pop(Roots)
   repeat
     u := pop(Active)
     u.current := false
   until u = n

 procedure merge_scc(t)
   L := {}
   repeat
     <u, C> := pop(Roots)
     L := L + C
     if (L is accepting) then
       report cycle
   until u.dfsnum <= t.dfsnum
   push(Roots, <u, L>)
 */
template <class TS, class GRAPH> class generalized_algorithm_t {
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
   Initial nodes have been marked in graph
   \return statistics on the run
   \note if labels is empty, graph is the full state-space of ts
   */
  tchecker::algorithms::couvscc::stats_t run(TS & ts, GRAPH & graph, boost::dynamic_bitset<> const & labels)
  {
    tchecker::algorithms::couvscc::stats_t stats;

    stats.set_start_time();

    _count = 0;

    std::vector<typename TS::sst_t> sst;
    ts.initial(sst);
    for (auto && [status, s, t] : sst) {
      auto && [is_new_node, initial_node] = graph.add_node(s);
      initial_node->initial(true);
      couv_dfs(initial_node, ts, graph, labels, stats);
      if (stats.cycle())
        break;
    }

    stats.stored_states() = graph.nodes_count();

    empty_stacks();

    stats.set_end_time();

    return stats;
  }

private:
  /*!
   \brief DFS loop of Couvreur's algorithm
   \param ts : a transition system
   \param graph : a graph
   \param labels : accepting labels
   \param stats : statistics on the run
   \param n : a node
   \post the DFS search in Couvreur's algorithm has been performed from n.
   stats.cycle() is true if an accepting cycle w.r.t labels has been found in
   ts, and false otherwise
   graph contains the part of ts that has been explored
  */
  void couv_dfs(node_sptr_t & n, TS & ts, GRAPH & graph, boost::dynamic_bitset<> const & labels,
                tchecker::algorithms::couvscc::stats_t & stats)
  {
    push(n, ts, graph, stats);
    while (!_todo.empty()) {
      auto && [n, succ] = _todo.top();
      if (succ.empty()) {
        if (_roots.top().n == n)
          close_scc(n);
        _todo.pop();
      }
      else {
        node_sptr_t t = _todo.top().pick_successor();
        if (t->dfsnum() == 0)
          push(t, ts, graph, stats);
        else if (t->current()) {
          merge_scc(t, stats, labels);
          if (stats.cycle())
            break;
        }
      }
    }
  }

  /*!
   \brief Close the SCC of node n
   \param n : a node
   \pre n is a root node
   \post the current flag of all nodes in the SCC of n has been set to false
   (hence closing the SCC)
  */
  void close_scc(node_sptr_t & n)
  {
    _roots.pop();

    node_sptr_t u{nullptr};
    do {
      u = _active.top();
      _active.pop();
      u->current() = false;
    } while (u != n);
  }

  /*!
   \brief Merge the SCCs on a loop (generalised Büchi conditions)
   \param n : a node
   \param labels : accepting labels
   \post all SCCs on a loop on n have been merged
  */
  void merge_scc(node_sptr_t & n, tchecker::algorithms::couvscc::stats_t & stats, boost::dynamic_bitset<> const & labels)
  {
    boost::dynamic_bitset<> scc_labels{labels.size(), 0};
    node_sptr_t u{nullptr};
    do {
      u = _roots.top().n;
      scc_labels |= _roots.top().labels;
      if (!labels.none() && labels.is_subset_of(scc_labels)) {
        stats.cycle() = true;
        return;
      }
      _roots.pop();
    } while (u->dfsnum() > n->dfsnum());
    _roots.push(roots_stack_entry_t{u, scc_labels});
  }

  /*!
   \brief Push node and set its flags
   \param n : a node
   \param ts : a transition system
   \param graph : a graph
   \param stats : statistics
   \post n has been expanded into graph, its flags (current and dfsnum) have
   been set, and n has been pushed to the stacks todo, roots and active
   a visited node has been added to stats
   */
  void push(node_sptr_t & n, TS & ts, GRAPH & graph, tchecker::algorithms::couvscc::stats_t & stats)
  {
    ++_count;
    n->dfsnum() = _count;
    n->current() = true;
    _todo.push(todo_stack_entry_t{n, expand_node(n, ts, graph, stats)});
    _roots.push(roots_stack_entry_t{n, ts.labels(n->state_ptr())});
    _active.push(n);
    ++stats.visited_states();
  }

  /*!
   \brief Adds successor nodes to the graph
   \param n : a node
   \param ts : a transition system
   \param graph : a graph
   \param stats : statistics
   \post all successor nodes of n in ts have been added to graph (if not yet in)
   with corresponding edges
   visited transitions have been added to stats
   \return all successor nodes of n
  */
  std::deque<node_sptr_t> expand_node(node_sptr_t & n, TS & ts, GRAPH & graph, tchecker::algorithms::couvscc::stats_t & stats)
  {
    std::deque<node_sptr_t> next_nodes;
    std::vector<typename TS::sst_t> v;
    ts.next(n->state_ptr(), v);
    for (auto && [status, s, t] : v) {
      ++stats.visited_transitions();
      auto && [new_node, nextn] = graph.add_node(s);
      graph.add_edge(n, nextn, *t);
      next_nodes.push_back(nextn);
    }
    return next_nodes;
  }

  /*!
   \class todo_stack_entry_t
   \brief Entry of the todo stack
   */
  struct todo_stack_entry_t {
    node_sptr_t n;                /*!< Current node */
    std::deque<node_sptr_t> succ; /*!< Successor nodes */

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
   \class roots_stack_entry_t
   \brief Entry of the roots stack
  */
  struct roots_stack_entry_t {
    node_sptr_t n;                  /*!< Root node */
    boost::dynamic_bitset<> labels; /*!< Labels in the explored SCC of n */
  };

  /*!
   \brief Empty all stacks
   \post All stacks are empty
   */
  void empty_stacks()
  {
    while (!_todo.empty())
      _todo.pop();
    while (!_roots.empty())
      _roots.pop();
    while (!_active.empty())
      _active.pop();
  }

private:
  unsigned int _count;                    /*!< DFS number counter */
  std::stack<todo_stack_entry_t> _todo;   /*!< todo stack */
  std::stack<roots_stack_entry_t> _roots; /*!< roots stack */
  std::stack<node_sptr_t> _active;        /*!< active stack */
};

/*!
 \class single_algorithm_t
 \brief Couvreur's SCC-decomposition-based liveness algorithm for single Büchi conditions
 \tparam TS : type of transition system, should implement tchecker::ts::fwd_t
 and tchecker::ts::inspector_t
 \tparam GRAPH : type of graph, should derive from
 tchecker::graph::reachability_graph_t, and nodes of type GRAPH::shared_node_t
 should derive from tchecker::algorithms::couvscc::node_t and have a method
 state_ptr() that yields a pointer to the corresponding state in TS.
 \note Implementation based on the variant of Couvreur's algorithm in:
 "Comparison of Algorithms for Checking Emptiness on Büchi Automata",
 Andreas Gaiser and Stefan Schwoon
 MEMICS 2009

 We have implemented the iterative translation of the recursive algorithm in
 Gaiser&Schwoon's paper. The algorithm below works for single Büchi condition.

 procedure couvscc(A)
   count := 0
   Roots := {}
   Active := {}
   Todo := {}
   for each initial state s0 of A
     if s0.dfsnum = 0 then
       couv_dfs(s0)

 procedure push(s)
   count := count + 1
   s.dfsnum := count
   s.current := true
   push(Roots, <s, accepting(s)>)
   push(Active, <s>)
   push(Todo, <s, post(s)>)

 procedure couv_dfs(s)
   push(s)
   while (Todo is not empty) do
     <n, succ> := top(Todo)
     if (succ is empty) then
       if (top(Roots) = <n, ?>) then
         close_scc(n)
       pop(Todo)
     else
       t := pick(succ)
       if (t.dfsnum = 0) then
         push(t)
       else if t.current then
         merge_scc(t)

 procedure close_scc(n)
   pop(Roots)
   repeat
     u := pop(Active)
     u.current := false
   until u = n

 procedure merge_scc(t)
   accepting := false
   repeat
     <u, A> := pop(Roots)
     accepting := accepting || A
     if (accepting) then
       report cycle
   until u.dfsnum <= t.dfsnum
   push(Roots, <u, L>)
 */
template <class TS, class GRAPH> class single_algorithm_t {
public:
  using node_sptr_t = typename GRAPH::node_sptr_t;

  /*!
   \brief Check if a transition has an infinite run that satisfies a given set
   of labels and build the corresponding graph
   \param ts : a transition system
   \param graph : a graph
   \param labels : accepting labels
   \pre labels is a single accepting condition
   \post graph is built from a traversal of ts starting from its initial states,
   until a cycle that satisfies labels is reached (if any).
   A node is created for each reached state in ts, and an edge is created for
   each transition in ts.
   Initial and final nodes have been marked in graph
   \return statistics on the run
   \throw std::invalid_argument : if labels is not a single accepting label
   \note if labels is empty, graph is the full state-space of ts
   */
  tchecker::algorithms::couvscc::stats_t run(TS & ts, GRAPH & graph, boost::dynamic_bitset<> const & labels)
  {
    if (labels.count() > 1)
      throw std::invalid_argument("*** tchecker::algorithms::couvscc::single_algorithm_t: single accepting condition required");

    tchecker::algorithms::couvscc::stats_t stats;

    stats.set_start_time();

    _count = 0;

    std::vector<typename TS::sst_t> sst;
    ts.initial(sst);
    for (auto && [status, s, t] : sst) {
      auto && [is_new_node, initial_node] = graph.add_node(s);
      initial_node->initial(true);
      initial_node->final(accepting(initial_node, ts, labels));
      couv_dfs(initial_node, ts, graph, labels, stats);
      if (stats.cycle())
        break;
    }

    stats.stored_states() = graph.nodes_count();

    empty_stacks();

    stats.set_end_time();

    return stats;
  }

private:
  /*!
   \brief DFS loop of Couvreur's algorithm
   \param ts : a transition system
   \param graph : a graph
   \param labels : accepting labels
   \param stats : statistics on the run
   \param n : a node
   \post the DFS search in Couvreur's algorithm has been performed from n.
   stats.cycle() is true if an accepting cycle w.r.t labels has been found in
   ts, and false otherwise
   graph contains the part of ts that has been explored
  */
  void couv_dfs(node_sptr_t & n, TS & ts, GRAPH & graph, boost::dynamic_bitset<> const & labels,
                tchecker::algorithms::couvscc::stats_t & stats)
  {
    push(n, ts, graph, labels, stats);
    while (!_todo.empty()) {
      auto && [n, succ] = _todo.top();
      if (succ.empty()) {
        if (_roots.top().n == n)
          close_scc(n);
        _todo.pop();
      }
      else {
        node_sptr_t t = _todo.top().pick_successor();
        if (t->dfsnum() == 0)
          push(t, ts, graph, labels, stats);
        else if (t->current()) {
          merge_scc(t, stats);
          if (stats.cycle())
            break;
        }
      }
    }
  }

  /*!
   \brief Close the SCC of node n
   \param n : a node
   \pre n is a root node
   \post the current flag of all nodes in the SCC of n has been set to false
   (hence closing the SCC)
  */
  void close_scc(node_sptr_t & n)
  {
    _roots.pop();

    node_sptr_t u{nullptr};
    do {
      u = _active.top();
      _active.pop();
      u->current() = false;
    } while (u != n);
  }

  /*!
   \brief Merge the SCCs on a loop (generalised Büchi conditions)
   \param n : a node
   \param stats : statstics
   \post all SCCs on a loop on n have been merged
  */
  void merge_scc(node_sptr_t & n, tchecker::algorithms::couvscc::stats_t & stats)
  {
    node_sptr_t u{nullptr};
    do {
      u = _roots.top().n;
      if (u->final()) {
        stats.cycle() = true;
        return;
      }
      _roots.pop();
    } while (u->dfsnum() > n->dfsnum());
    _roots.push(roots_stack_entry_t{u});
  }

  /*!
   \brief Push node and set its flags
   \param n : a node
   \param ts : a transition system
   \param graph : a graph
   \param labels : accepting labels
   \param stats : statistics
   \post n has been expanded into graph, its flags (current and dfsnum) have
   been set, and n has been pushed to the stacks todo, roots and active
   a visited node has been added to stats
   */
  void push(node_sptr_t & n, TS & ts, GRAPH & graph, boost::dynamic_bitset<> const & labels,
            tchecker::algorithms::couvscc::stats_t & stats)
  {
    ++_count;
    n->dfsnum() = _count;
    n->current() = true;
    _todo.push(todo_stack_entry_t{n, expand_node(n, ts, graph, labels, stats)});
    _roots.push(roots_stack_entry_t{n});
    _active.push(n);
    ++stats.visited_states();
  }

  /*!
   \brief Adds successor nodes to the graph
   \param n : a node
   \param ts : a transition system
   \param graph : a graph
   \param labels : accepting labels
   \param stats : statistics
   \post all successor nodes of n in ts have been added to graph (if not yet in)
   with corresponding edges
   visited transitions have been added to stats
   final flga has been set to true for all acepting successor nodes of n
   \return all successor nodes of n
  */
  std::deque<node_sptr_t> expand_node(node_sptr_t & n, TS & ts, GRAPH & graph, boost::dynamic_bitset<> const & labels,
                                      tchecker::algorithms::couvscc::stats_t & stats)
  {
    std::deque<node_sptr_t> next_nodes;
    std::vector<typename TS::sst_t> v;
    ts.next(n->state_ptr(), v);
    for (auto && [status, s, t] : v) {
      ++stats.visited_transitions();
      auto && [new_node, nextn] = graph.add_node(s);
      graph.add_edge(n, nextn, *t);
      nextn->final(accepting(nextn, ts, labels));
      next_nodes.push_back(nextn);
    }
    return next_nodes;
  }

  /*!
   \brief Accepting node predicate
   \param n : a node
   \param ts : transition system
   \param labels : accepting labels
   \return true if n is accepting w.r.t. labels, false otherwise
   */
  bool accepting(node_sptr_t & n, TS & ts, boost::dynamic_bitset<> const & labels) const
  {
    return !labels.none() && labels.is_subset_of(ts.labels(n->state_ptr()));
  }

  /*!
   \class todo_stack_entry_t
   \brief Entry of the todo stack
   */
  struct todo_stack_entry_t {
    node_sptr_t n;                /*!< Current node */
    std::deque<node_sptr_t> succ; /*!< Successor nodes */

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
   \class roots_stack_entry_t
   \brief Entry of the roots stack
  */
  struct roots_stack_entry_t {
    node_sptr_t n; /*!< Root node */
  };

  /*!
   \brief Empty all stacks
   \post All stacks are empty
   */
  void empty_stacks()
  {
    while (!_todo.empty())
      _todo.pop();
    while (!_roots.empty())
      _roots.pop();
    while (!_active.empty())
      _active.pop();
  }

private:
  unsigned int _count;                    /*!< DFS number counter */
  std::stack<todo_stack_entry_t> _todo;   /*!< todo stack */
  std::stack<roots_stack_entry_t> _roots; /*!< roots stack */
  std::stack<node_sptr_t> _active;        /*!< active stack */
};

} // namespace couvscc

} // namespace algorithms

} // namespace tchecker

#endif // TCHECKER_ALGORITHMS_COUVREUR_SCC_ALGORITHM_HH