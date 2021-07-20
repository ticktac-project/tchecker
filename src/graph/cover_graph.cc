/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <limits>

#include "tchecker/graph/cover_graph.hh"

namespace tchecker {

namespace graph {

namespace cover {

/* Not-stored node position */

tchecker::graph::cover::node_position_t const NOT_STORED = std::numeric_limits<tchecker::graph::cover::node_position_t>::max();

/* node_t */

node_t::node_t() { clear_position(); }

node_t::node_t(tchecker::graph::cover::node_t const & node)
    : _position_in_table(node._position_in_table), _position_in_container(node._position_in_container)
{
  if (node.is_stored())
    throw std::invalid_argument("Cannot copy a stored node");
}

tchecker::graph::cover::node_t & node_t::operator=(tchecker::graph::cover::node_t const & node)
{
  if (node.is_stored())
    throw std::invalid_argument("Cannot copy a stored node");
  _position_in_table = node._position_in_table;
  _position_in_container = node._position_in_container;
  return *this;
}

void node_t::set_position(tchecker::graph::cover::node_position_t position_in_table,
                          tchecker::graph::cover::node_position_t position_in_container)
{
  if (position_in_table == tchecker::graph::cover::NOT_STORED)
    throw std::invalid_argument("Invalid node position in the table of containers");
  if (position_in_container == tchecker::graph::cover::NOT_STORED)
    throw std::invalid_argument("Invalid node position in the node container");
  _position_in_table = position_in_table;
  _position_in_container = position_in_container;
}

void node_t::clear_position()
{
  _position_in_table = tchecker::graph::cover::NOT_STORED;
  _position_in_container = tchecker::graph::cover::NOT_STORED;
}

bool node_t::is_stored() const
{
  return _position_in_table != tchecker::graph::cover::NOT_STORED &&
         _position_in_container != tchecker::graph::cover::NOT_STORED;
}

} // end of namespace cover

} // end of namespace graph

} // end of namespace tchecker
