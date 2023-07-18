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

#include "display.hh"
#include "simulate.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/zg/zg.hh"

namespace tchecker {

namespace tck_simulate {

/*!
 \brief Type of display
*/

/* Simulation functions */

static std::size_t const NO_SELECTION = std::numeric_limits<std::size_t>::max();

// Randomized simulation */

/*!
 \brief Random selection
 \param v : a vector of triples (status, state, transition)
 \pre the size of v is less than NO_SELECTION (checked by assertion)
 \return the index of the chosen element in v if v is not empty,
 tchecker::tck_simulate::NO_SELECTION otherwise
*/
static std::size_t randomized_select(std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  assert(v.size() < tchecker::tck_simulate::NO_SELECTION);
  if (v.size() == 0)
    return tchecker::tck_simulate::NO_SELECTION;
  return std::rand() % v.size();
}

std::shared_ptr<tchecker::tck_simulate::graph_t>
randomized_simulation(tchecker::parsing::system_declaration_t const & sysdecl, std::size_t nsteps,
                      std::map<std::string, std::string> const & starting_state_attributes)
{
  std::size_t const block_size = 1000;
  std::size_t const table_size = 65536;

  std::shared_ptr<tchecker::ta::system_t const> system{new tchecker::ta::system_t{sysdecl}};
  std::shared_ptr<tchecker::zg::zg_t> zg{tchecker::zg::factory(system, tchecker::ts::NO_SHARING,
                                                               tchecker::zg::STANDARD_SEMANTICS, tchecker::zg::NO_EXTRAPOLATION,
                                                               block_size, table_size)};
  std::shared_ptr<tchecker::tck_simulate::graph_t> g{new tchecker::tck_simulate::graph_t{zg, block_size}};
  std::vector<tchecker::zg::zg_t::sst_t> v;

  srand(time(NULL));

  if (starting_state_attributes.empty()) // start simulation from initial state
    zg->initial(v);
  else // start simulation from specified starting state
    zg->build(starting_state_attributes, v);

  std::size_t k = tchecker::tck_simulate::randomized_select(v);
  if (k == tchecker::tck_simulate::NO_SELECTION)
    return g;
  tchecker::tck_simulate::graph_t::node_sptr_t previous_node = g->add_node(zg->state(v[k]));
  previous_node->initial(true);
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

// Interactive simulation

/*!
 \brief Interactive selection
 \param display : display
 \param s : current state
 \param v : a vector of successors (status, state, transition)
 \pre the size of v is less than NO_SELECTION (checked by assertion)
 \return the index of the chosen element in v if v is not empty,
 tchecker::tck_simulate::NO_SELECTION otherwise
*/
static std::size_t interactive_select(tchecker::tck_simulate::display_t & display, tchecker::zg::const_state_sptr_t const & s,
                                      std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  assert(v.size() < tchecker::tck_simulate::NO_SELECTION);
  if (v.size() == 0)
    return tchecker::tck_simulate::NO_SELECTION;

  if (s.ptr() == nullptr) // initial simulation step
    display.output_initial(v);
  else
    display.output_next(s, v);

  do {
    std::cout << "Select 0-" << v.size() - 1 << " (q: quit, r: random)? ";
    std::string s;
    std::cin >> s;

    if (std::cin.eof() || s == "q")
      return tchecker::tck_simulate::NO_SELECTION;
    else if (s == "r")
      return tchecker::tck_simulate::randomized_select(v);
    else {
      char * end = nullptr;
      errno = 0;
      unsigned long k = std::strtoul(s.c_str(), &end, 10);
      if (*end != 0) {
        std::cerr << "Invalid input" << std::endl;
        continue;
      }
      if (errno == ERANGE || k > std::numeric_limits<std::size_t>::max() || k >= v.size()) {
        std::cerr << "Out-of-range input" << std::endl;
        continue;
      }
      return static_cast<std::size_t>(k);
    }
  } while (1);
}

std::shared_ptr<tchecker::tck_simulate::graph_t>
interactive_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                       enum tchecker::tck_simulate::display_type_t display_type,
                       std::map<std::string, std::string> const & starting_state_attributes)
{
  std::size_t const block_size = 1000;
  std::size_t const table_size = 65536;

  std::shared_ptr<tchecker::ta::system_t const> system{new tchecker::ta::system_t{sysdecl}};
  std::shared_ptr<tchecker::zg::zg_t> zg{tchecker::zg::factory(system, tchecker::ts::NO_SHARING,
                                                               tchecker::zg::STANDARD_SEMANTICS, tchecker::zg::NO_EXTRAPOLATION,
                                                               block_size, table_size)};
  std::shared_ptr<tchecker::tck_simulate::graph_t> g{new tchecker::tck_simulate::graph_t{zg, block_size}};
  std::vector<tchecker::zg::zg_t::sst_t> v;

  std::unique_ptr<tchecker::tck_simulate::display_t> display{
      tchecker::tck_simulate::display_factory(display_type, std::cout, zg)};

  srand(time(NULL)); // needed if user chooses randomize selection

  tchecker::tck_simulate::graph_t::node_sptr_t previous_node{nullptr};
  std::size_t k = tchecker::tck_simulate::NO_SELECTION;

  if (starting_state_attributes.empty()) {
    // start simulation from initial states (interactive selection)
    zg->initial(v);
    k = tchecker::tck_simulate::interactive_select(*display, tchecker::zg::const_state_sptr_t{nullptr}, v);
  }
  else {
    // start simulation from specified states
    zg->build(starting_state_attributes, v);
    assert(v.size() <= 1);
    k = (v.size() == 0 ? tchecker::tck_simulate::NO_SELECTION : 0); // select state if any
  }
  if (k == tchecker::tck_simulate::NO_SELECTION)
    return g;
  previous_node = g->add_node(zg->state(v[k]));
  previous_node->initial(true);
  v.clear();

  do {
    zg->next(previous_node->state_ptr(), v);

    std::size_t k = tchecker::tck_simulate::interactive_select(*display, previous_node->state_ptr(), v);
    if (k == tchecker::tck_simulate::NO_SELECTION)
      break;
    tchecker::tck_simulate::graph_t::node_sptr_t node = g->add_node(zg->state(v[k]));
    g->add_edge(previous_node, node, *zg->transition(v[k]));
    v.clear();

    previous_node = node;
  } while (1);

  return g;
}

} // namespace tck_simulate

} // namespace tchecker