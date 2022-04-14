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

/* Display functions */

/*!
 \brief Display state
 \param os : output stream
 \param zg : zone graph
 \param s : state
 \post Attributes of state s have been displayed on os
 */
static void display(std::ostream & os, tchecker::zg::zg_t const & zg, tchecker::zg::const_state_sptr_t const & s)
{
  std::map<std::string, std::string> attr;
  zg.attributes(s, attr);
  for (auto && [key, value] : attr)
    os << "\t" << key << ": " << value << std::endl;
}

/*!
 \brief Display transition
 \param os : output stream
 \param zg : zone graph
 \param t : transition
 \post Attributes of transition t have been displayed on os
 */
static void display(std::ostream & os, tchecker::zg::zg_t const & zg, tchecker::zg::const_transition_sptr_t const & t)
{
  std::map<std::string, std::string> attr;
  zg.attributes(t, attr);
  for (auto && [key, value] : attr)
    os << "\t" << key << ": " << value << " ";
}

/*!
 \brief Display simulation step
 \param os : output stream
 \param zg : zone graph
 \param s : current state
 \param v : successor triples (status, state, transition)
 \pre point s to nullptr if no current state
 \post s and its successor states and transitions have been displayed to os
 */
static void display(std::ostream & os, tchecker::zg::zg_t const & zg, tchecker::zg::const_state_sptr_t const & s,
                    std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  if (s.ptr() != nullptr) {
    os << "--- Current state: " << std::endl;
    tchecker::tck_simulate::display(os, zg, s);
    os << "--- Successors: " << std::endl;
  }
  else
    os << "--- Initial states: " << std::endl;

  for (std::size_t i = 0; i < v.size(); ++i) {
    tchecker::zg::const_state_sptr_t nexts{zg.state(v[i])};
    tchecker::zg::const_transition_sptr_t nextt{zg.transition(v[i])};
    os << i << ") ";
    if (s.ptr() != nullptr)
      tchecker::tck_simulate::display(os, zg, nextt);
    os << std::endl;
    tchecker::tck_simulate::display(os, zg, nexts);
  }
}

/* Simulation functins */

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

// Interactive simulation

/*!
 \brief Interactive selection
 \param zg : zone graph
 \param s : current state
 \param v : a vector of successors (status, state, transition)
 \pre the size of v is less than NO_SELECTION (checked by assertion)
 \return the index of the chosen element in v if v is not empty,
 tchecker::tck_simulate::NO_SELECTION otherwise
*/
static std::size_t interactive_select(tchecker::zg::zg_t const & zg, tchecker::zg::const_state_sptr_t const & s,
                                      std::vector<tchecker::zg::zg_t::sst_t> const & v)
{
  assert(v.size() < tchecker::tck_simulate::NO_SELECTION);
  if (v.size() == 0)
    return tchecker::tck_simulate::NO_SELECTION;

  display(std::cout, zg, s, v);

  do {
    std::cout << "Select 0-" << v.size() - 1 << " (q: quit, r: random)? ";
    std::string s;
    std::cin >> s;

    if (s == "q")
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

std::shared_ptr<tchecker::tck_simulate::graph_t> interactive_simulation(tchecker::parsing::system_declaration_t const & sysdecl)
{
  std::shared_ptr<tchecker::ta::system_t const> system{new tchecker::ta::system_t{sysdecl}};
  std::shared_ptr<tchecker::zg::zg_t> zg{
      tchecker::zg::factory(system, tchecker::zg::STANDARD_SEMANTICS, tchecker::zg::NO_EXTRAPOLATION, 100)};
  std::shared_ptr<tchecker::tck_simulate::graph_t> g{new tchecker::tck_simulate::graph_t{zg, 100}};
  std::vector<tchecker::zg::zg_t::sst_t> v;

  srand(time(NULL)); // needed if user chooses randomize selection

  zg->initial(v);
  std::size_t k = tchecker::tck_simulate::interactive_select(*zg, tchecker::zg::const_state_sptr_t{nullptr}, v);
  if (k == tchecker::tck_simulate::NO_SELECTION)
    return g;
  tchecker::tck_simulate::graph_t::node_sptr_t previous_node = g->add_node(zg->state(v[k]));
  v.clear();

  do {
    zg->next(previous_node->state_ptr(), v);

    std::size_t k = tchecker::tck_simulate::interactive_select(*zg, previous_node->state_ptr(), v);
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