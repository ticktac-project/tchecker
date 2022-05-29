/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "graph.hh"

namespace tchecker {

namespace tck_simulate {

/* node_t */

node_t::node_t(tchecker::zg::state_sptr_t const & s) : _state(s) {}

node_t::node_t(tchecker::zg::const_state_sptr_t const & s) : _state(s) {}

/* edge_t */

edge_t::edge_t(tchecker::zg::transition_t const & t) : _vedge(t.vedge_ptr()) {}

/* graph_t */

graph_t::graph_t(std::shared_ptr<tchecker::zg::zg_t> const & zg, std::size_t block_size)
    : tchecker::graph::reachability::multigraph_t<tchecker::tck_simulate::node_t, tchecker::tck_simulate::edge_t>::multigraph_t(
          block_size),
      _zg(zg)
{
}

graph_t::~graph_t()
{
  tchecker::graph::reachability::multigraph_t<tchecker::tck_simulate::node_t, tchecker::tck_simulate::edge_t>::clear();
}

void graph_t::attributes(tchecker::tck_simulate::node_t const & n, std::map<std::string, std::string> & m) const
{
  _zg->attributes(n.state_ptr(), m);
}

void graph_t::attributes(tchecker::tck_simulate::edge_t const & e, std::map<std::string, std::string> & m) const
{
  m["vedge"] = tchecker::to_string(e.vedge(), _zg->system().as_system_system());
}

/* DOT output */

/*!
 \class node_lexical_less_t
 \brief Less-than order on nodes based on lexical ordering
*/
class node_lexical_less_t {
public:
  /*!
   \brief Less-than order on nodes based on lexical ordering
   \param n1 : a node
   \param n2 : a node
   \return true if n1 is less-than n2 w.r.t. lexical ordering over the states in
   the nodes
  */
  bool operator()(tchecker::tck_simulate::graph_t::node_sptr_t const & n1,
                  tchecker::tck_simulate::graph_t::node_sptr_t const & n2) const
  {
    return tchecker::zg::lexical_cmp(n1->state(), n2->state()) < 0;
  }
};

/*!
 \class edge_lexical_less_t
 \brief Less-than ordering on edges based on lexical ordering
 */
class edge_lexical_less_t {
public:
  /*!
   \brief Less-than ordering on edges based on lexical ordering
   \param e1 : an edge
   \param e2 : an edge
   \return true if e1 is less-than  e2 w.r.t. the tuple of edges in e1 and e2
  */
  bool operator()(tchecker::tck_simulate::graph_t::edge_sptr_t const & e1,
                  tchecker::tck_simulate::graph_t::edge_sptr_t const & e2) const
  {
    return tchecker::lexical_cmp(e1->vedge(), e2->vedge()) < 0;
  }
};

std::ostream & dot_output(std::ostream & os, tchecker::tck_simulate::graph_t const & g, std::string const & name)
{
  return tchecker::graph::reachability::dot_output<tchecker::tck_simulate::graph_t, tchecker::tck_simulate::node_lexical_less_t,
                                                   tchecker::tck_simulate::edge_lexical_less_t>(os, g, name);
}

} // namespace tck_simulate

} // namespace tchecker