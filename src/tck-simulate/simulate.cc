/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cstdlib>
#include <ctime>
#include <limits>
#include <vector>

#include "simulate.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/zg/zg.hh"

namespace tchecker {

namespace tck_simulate {

static std::size_t const NO_SELECTION = std::numeric_limits<std::size_t>::max();

// Randomized simulation

/*!
 \brief Random selection
 \param v : a vector of triples (status, state, transition)
 \pre the size of v is less than NO_SELECTION (checked by assertion)
 \return the index of the chosen element in v if v is not empty,
*/
static std::size_t randomized_select(std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  assert(v.size() < tchecker::tck_simulate::NO_SELECTION);
  if (v.size() == 0)
    return tchecker::tck_simulate::NO_SELECTION;
  return std::rand() % v.size();
}

std::shared_ptr<tchecker::tck_simulate::graph_t> randomized_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                                                                       std::size_t nsteps)
{
  std::shared_ptr<tchecker::ta::system_t const> system{new tchecker::ta::system_t{sysdecl}};
  std::shared_ptr<tchecker::zg::zg_t> zg{
      tchecker::zg::factory(system, tchecker::zg::STANDARD_SEMANTICS, tchecker::zg::NO_EXTRAPOLATION, 100)};
  std::shared_ptr<tchecker::tck_simulate::graph_t> g{new tchecker::tck_simulate::graph_t{zg, 100}};
  std::vector<tchecker::zg::zg_t::sst_t> v;

  srand(time(NULL));

  zg->initial(v);
  std::size_t k = tchecker::tck_simulate::randomized_select(v);
  if (k == tchecker::tck_simulate::NO_SELECTION)
    return g;
  tchecker::tck_simulate::graph_t::node_sptr_t previous_node = g->add_node(zg->state(v[k]));
  v.clear();

  for (std::size_t i = 0; i < nsteps; ++i) {
    zg->next(previous_node->state_ptr(), v);

    std::size_t k = tchecker::tck_simulate::randomized_select(v);
    if (k == tchecker::tck_simulate::NO_SELECTION)
      break;
    tchecker::tck_simulate::graph_t::node_sptr_t node = g->add_node(zg->state(v[k]));
    g->add_edge(previous_node, node, *zg->transition(v[k]));
    v.clear();

    previous_node = node;
  }

  return g;
}

} // namespace tck_simulate

} // namespace tchecker