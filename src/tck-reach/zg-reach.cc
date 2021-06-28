/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <boost/dynamic_bitset.hpp>

#include "tchecker/algorithms/search_order.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/utils/gc.hh"
#include "zg-reach.hh"

namespace tchecker {

namespace algorithms {

namespace reach {

namespace zg {

/* node_t */

node_t::node_t(tchecker::zg::state_sptr_t const & s) : _state(s) {}

node_t::node_t(tchecker::zg::const_state_sptr_t const & s) : _state(s) {}

/* node_hash_t */

std::size_t node_hash_t::operator()(tchecker::algorithms::reach::zg::node_t const & n) const { return hash_value(n.state()); }

/* node_equal_to_t */

bool node_equal_to_t::operator()(tchecker::algorithms::reach::zg::node_t const & n1,
                                 tchecker::algorithms::reach::zg::node_t const & n2) const
{
  return n1.state() == n2.state();
}

/* edge_t */

edge_t::edge_t(tchecker::zg::transition_t const & t) : _vedge(t.vedge_ptr()) {}

/* graph_t */

graph_t::graph_t(std::shared_ptr<tchecker::zg::zg_t> const & zg, std::size_t block_size, std::size_t table_size,
                 tchecker::gc_t & gc)
    : tchecker::graph::reachability::graph_t<tchecker::algorithms::reach::zg::node_t, tchecker::algorithms::reach::zg::edge_t,
                                             tchecker::algorithms::reach::zg::node_hash_t,
                                             tchecker::algorithms::reach::zg::node_equal_to_t>(block_size, table_size, gc),
      _zg(zg)
{
}

void graph_t::attributes(tchecker::algorithms::reach::zg::node_t const & n, std::map<std::string, std::string> & m) const
{
  _zg->attributes(n.state_ptr(), m);
}

void graph_t::attributes(tchecker::algorithms::reach::zg::edge_t const & e, std::map<std::string, std::string> & m) const
{
  m["vedge"] = tchecker::to_string(e.vedge(), _zg->system().as_system_system());
}

/* dot_output */

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
  bool operator()(tchecker::algorithms::reach::zg::graph_t::node_sptr_t const & n1,
                  tchecker::algorithms::reach::zg::graph_t::node_sptr_t const & n2) const
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
  bool operator()(tchecker::algorithms::reach::zg::graph_t::edge_sptr_t const & e1,
                  tchecker::algorithms::reach::zg::graph_t::edge_sptr_t const & e2) const
  {
    return tchecker::lexical_cmp(e1->vedge(), e2->vedge()) < 0;
  }
};

std::ostream & dot_output(std::ostream & os, tchecker::algorithms::reach::zg::graph_t const & g, std::string const & name)
{
  return tchecker::graph::reachability::dot_output<tchecker::algorithms::reach::zg::graph_t,
                                                   tchecker::algorithms::reach::zg::node_lexical_less_t,
                                                   tchecker::algorithms::reach::zg::edge_lexical_less_t>(os, g, name);
}

/* run */

std::tuple<tchecker::algorithms::reach::stats_t, std::shared_ptr<tchecker::algorithms::reach::zg::graph_t>>
run(std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl, std::string const & labels,
    std::string const & search_order, std::size_t block_size, std::size_t table_size)
{
  tchecker::gc_t gc;

  std::shared_ptr<tchecker::ta::system_t const> system{new tchecker::ta::system_t{*sysdecl}};

  std::shared_ptr<tchecker::zg::zg_t> zg{
      tchecker::zg::factory(system, tchecker::zg::ELAPSED_SEMANTICS, tchecker::zg::EXTRA_LU_PLUS_LOCAL, block_size, gc)};

  std::shared_ptr<tchecker::algorithms::reach::zg::graph_t> graph{
      new tchecker::algorithms::reach::zg::graph_t{zg, block_size, table_size, gc}};

  boost::dynamic_bitset<> accepting_labels = system->as_syncprod_system().labels(labels);

  tchecker::algorithms::reach::zg::algorithm_t algorithm;

  enum tchecker::waiting::policy_t policy = tchecker::algorithms::waiting_policy(search_order);

  gc.start();

  tchecker::algorithms::reach::stats_t stats = algorithm.run(*zg, *graph, accepting_labels, policy);

  gc.stop();

  return std::make_tuple(stats, graph);
}

} // end of namespace zg

} // end of namespace reach

} // end of namespace algorithms

} // end of namespace tchecker