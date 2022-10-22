/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <boost/dynamic_bitset.hpp>

#include "tchecker/system/static_analysis.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/utils/log.hh"
#include "zg-couvscc.hh"

namespace tchecker {

namespace tck_liveness {

namespace zg_couvscc {

/* node_t */

node_t::node_t(tchecker::zg::state_sptr_t const & s) : _state(s) {}

node_t::node_t(tchecker::zg::const_state_sptr_t const & s) : _state(s) {}

/* node_hash_t */

std::size_t node_hash_t::operator()(tchecker::tck_liveness::zg_couvscc::node_t const & n) const
{
  return tchecker::zg::shared_hash_value(n.state());
}

/* node_equal_to_t */

bool node_equal_to_t::operator()(tchecker::tck_liveness::zg_couvscc::node_t const & n1,
                                 tchecker::tck_liveness::zg_couvscc::node_t const & n2) const
{
  return tchecker::zg::shared_equal_to(n1.state(), n2.state());
}

/* edge_t */

edge_t::edge_t(tchecker::zg::transition_t const & t) : _vedge(t.vedge_ptr()) {}

/* graph_t */

graph_t::graph_t(std::shared_ptr<tchecker::zg::sharing_zg_t> const & zg, std::size_t block_size, std::size_t table_size)
    : tchecker::graph::reachability::graph_t<
          tchecker::tck_liveness::zg_couvscc::node_t, tchecker::tck_liveness::zg_couvscc::edge_t,
          tchecker::tck_liveness::zg_couvscc::node_hash_t, tchecker::tck_liveness::zg_couvscc::node_equal_to_t>(
          block_size, table_size, tchecker::tck_liveness::zg_couvscc::node_hash_t(),
          tchecker::tck_liveness::zg_couvscc::node_equal_to_t()),
      _zg(zg)
{
}

graph_t::~graph_t()
{
  tchecker::graph::reachability::graph_t<tchecker::tck_liveness::zg_couvscc::node_t, tchecker::tck_liveness::zg_couvscc::edge_t,
                                         tchecker::tck_liveness::zg_couvscc::node_hash_t,
                                         tchecker::tck_liveness::zg_couvscc::node_equal_to_t>::clear();
}

void graph_t::attributes(tchecker::tck_liveness::zg_couvscc::node_t const & n, std::map<std::string, std::string> & m) const
{
  _zg->attributes(n.state_ptr(), m);
}

void graph_t::attributes(tchecker::tck_liveness::zg_couvscc::edge_t const & e, std::map<std::string, std::string> & m) const
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
  bool operator()(tchecker::tck_liveness::zg_couvscc::graph_t::node_sptr_t const & n1,
                  tchecker::tck_liveness::zg_couvscc::graph_t::node_sptr_t const & n2) const
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
   \return true if e1 is less-than e2 w.r.t. the tuple of edges in e1 and e2
  */
  bool operator()(tchecker::tck_liveness::zg_couvscc::graph_t::edge_sptr_t const & e1,
                  tchecker::tck_liveness::zg_couvscc::graph_t::edge_sptr_t const & e2) const
  {
    return tchecker::lexical_cmp(e1->vedge(), e2->vedge()) < 0;
  }
};

std::ostream & dot_output(std::ostream & os, tchecker::tck_liveness::zg_couvscc::graph_t const & g, std::string const & name)
{
  return tchecker::graph::reachability::dot_output<tchecker::tck_liveness::zg_couvscc::graph_t,
                                                   tchecker::tck_liveness::zg_couvscc::node_lexical_less_t,
                                                   tchecker::tck_liveness::zg_couvscc::edge_lexical_less_t>(os, g, name);
}

/* run */

std::tuple<tchecker::algorithms::couvscc::stats_t, std::shared_ptr<tchecker::tck_liveness::zg_couvscc::graph_t>>
run(std::shared_ptr<tchecker::parsing::system_declaration_t> const & sysdecl, std::string const & labels,
    std::size_t block_size, std::size_t table_size)
{
  std::shared_ptr<tchecker::ta::system_t const> system{new tchecker::ta::system_t{*sysdecl}};
  if (!tchecker::system::every_process_has_initial_location(system->as_system_system()))
    std::cerr << tchecker::log_warning << "system has no initial state" << std::endl;

  std::shared_ptr<tchecker::zg::sharing_zg_t> zg{tchecker::zg::factory_sharing(
      system, tchecker::zg::ELAPSED_SEMANTICS, tchecker::zg::EXTRA_LU_PLUS_LOCAL, block_size, table_size)};

  std::shared_ptr<tchecker::tck_liveness::zg_couvscc::graph_t> graph{
      new tchecker::tck_liveness::zg_couvscc::graph_t{zg, block_size, table_size}};

  boost::dynamic_bitset<> accepting_labels = system->as_syncprod_system().labels(labels);

  tchecker::tck_liveness::zg_couvscc::algorithm_t algorithm;

  tchecker::algorithms::couvscc::stats_t stats = algorithm.run(*zg, *graph, accepting_labels);

  return std::make_tuple(stats, graph);
}

} // namespace zg_couvscc

} // namespace tck_liveness

} // end of namespace tchecker