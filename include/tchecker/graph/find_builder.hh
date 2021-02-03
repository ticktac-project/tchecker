/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GRAPH_FIND_BUILDER_HH
#define TCHECKER_GRAPH_FIND_BUILDER_HH

#include <tuple>

#include "tchecker/basictypes.hh"
#include "tchecker/graph/builder.hh"

/*!
 \file find_builder.hh
 \brief Find graph builder
 */

namespace tchecker {

namespace graph {

/*!
 \class ts_find_builder_t
 \brief Build a find graph from a transition system
 \tparam TS : type of transition system (see tchecker::ts::ts_t)
 \tparam FIND_GRAPH : type of find graph, should derive from tchecker::graph::find_graph_t, and
 should be a graph allocator (see tchecker::graph::graph_allocator_t)
 */
template <class TS, class FIND_GRAPH> class ts_find_builder_t : protected tchecker::graph::ts_builder_t<TS, FIND_GRAPH> {
public:
  /*!
   \brief Type of pointer to node
   */
  using node_ptr_t = typename tchecker::graph::ts_builder_t<TS, FIND_GRAPH>::node_ptr_t;

  /*!
   \brief Type of pointer to edge
   */
  using edge_ptr_t = typename tchecker::graph::ts_builder_t<TS, FIND_GRAPH>::edge_ptr_t;

  /*!
   \brief Constructor
   \param ts : a transition system
   \param find_graph : a find graph with allocation
   \note see tchecker::graph::ts_builder_t::ts_builder_t
   */
  ts_find_builder_t(TS & ts, FIND_GRAPH & find_graph) : tchecker::graph::ts_builder_t<TS, FIND_GRAPH>(ts, find_graph) {}

  /*!
   \brief Copy constructor
   */
  ts_find_builder_t(tchecker::graph::ts_find_builder_t<TS, FIND_GRAPH> const &) = default;

  /*!
   \brief Move constructor
   */
  ts_find_builder_t(tchecker::graph::ts_find_builder_t<TS, FIND_GRAPH> &&) = default;

  /*!
   \brief Destructor
   */
  ~ts_find_builder_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::graph::ts_find_builder_t<TS, FIND_GRAPH> &
  operator=(tchecker::graph::ts_find_builder_t<TS, FIND_GRAPH> const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::graph::ts_find_builder_t<TS, FIND_GRAPH> &
  operator=(tchecker::graph::ts_find_builder_t<TS, FIND_GRAPH> &&) = default;

  /*!
   \brief Compute initial node
   \param v : initial iterator value from the transition system
   \param nargs : parameters to FIND_GRAPH::allocate_node()
   \param eargs : parameters to FIND_GRAPH::allocate_edge()
   \return a quadruple <node, edge, status, is_new_node> where
   status is the tchecker::state_status_t status of node,
   node and edge are the initial nodes and edges computed from v,
   and is_new_node tells if node is a new node of an existing node.
   node points to nullptr if status != tchecker::STATE_OK
   */
  template <class... NARGS, class... EARGS>
  std::tuple<node_ptr_t, edge_ptr_t, tchecker::state_status_t, bool>
  initial_node(typename TS::initial_iterator_value_t const & v, std::tuple<NARGS...> && nargs, std::tuple<EARGS...> && eargs)
  {
    node_ptr_t node;
    edge_ptr_t edge;
    tchecker::state_status_t status;

    std::tie(node, edge, status) = tchecker::graph::ts_builder_t<TS, FIND_GRAPH>::initial_node(
        v, std::forward<std::tuple<NARGS...>>(nargs), std::forward<std::tuple<EARGS...>>(eargs));

    if (status != tchecker::STATE_OK)
      return std::make_tuple(node, edge, status, false);

    node_ptr_t actual_node = this->_graph.find(node);
    bool is_new_node = (actual_node == node);
    return std::make_tuple(actual_node, edge, status, is_new_node);
  }

  /*!
   \brief Compute next node
   \param node : source node
   \param v : outgoing edges iterator value from the transition system
   \param nargs : parameters to FIND_GRAPH::allocate_node()
   \param eargs : parameters to FIND_GRAPH::allocate_edge()
   \return a quadruple <next_node, edge, status, is_new_node> where status
   is the tchecker::state_status_t status of next_node,
   next_node and edges are the next node and outgoing edge of node
   corresponding to v, and is_new_node tells if next_node is a new node
   or an existing node. next_node points to nullptr if
   status != tchecker::STATE_OK
   */
  template <class... NARGS, class... EARGS>
  std::tuple<node_ptr_t, edge_ptr_t, tchecker::state_status_t, bool>
  next_node(node_ptr_t & node, typename TS::outgoing_edges_iterator_value_t const & v, std::tuple<NARGS...> && nargs,
            std::tuple<EARGS...> && eargs)
  {
    node_ptr_t next_node;
    edge_ptr_t edge;
    tchecker::state_status_t status;

    std::tie(next_node, edge, status) = tchecker::graph::ts_builder_t<TS, FIND_GRAPH>::next_node(
        node, v, std::forward<std::tuple<NARGS...>>(nargs), std::forward<std::tuple<EARGS...>>(eargs));

    if (status != tchecker::STATE_OK)
      return std::make_tuple(next_node, edge, status, false);

    node_ptr_t actual_next_node = this->_graph.find(next_node);
    bool is_new_node = (actual_next_node == next_node);
    return std::make_tuple(actual_next_node, edge, status, is_new_node);
  }
};

} // end of namespace graph

} // end of namespace tchecker

#endif // TCHECKER_GRAPH_FIND_BUILDER_HH
