/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TCK_SIMULATE_SIMULATE_HH
#define TCHECKER_TCK_SIMULATE_SIMULATE_HH

/*!
 \file simulate.hh
 \brief Simulation of timed automata
*/

#include <memory>

#include "graph.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/zg/zg.hh"

namespace tchecker {

namespace tck_simulate {

/*!
\brief Randomized simulation of timed automata
\param sysdecl : system declaration
\param nsteps : number of simulation steps
\return the simulation tree built from nsteps randomized simulation steps of the
system of timed processes sysdecl
*/
std::shared_ptr<tchecker::tck_simulate::graph_t> randomized_simulation(tchecker::parsing::system_declaration_t const & sysdecl,
                                                                       std::size_t nsteps);

/*!
\brief Interactive simulation of timed automata
\param sysdecl : system declaration
\return the simulation tree built from interactive of the system of timed processes sysdecl
*/
std::shared_ptr<tchecker::tck_simulate::graph_t>
interactive_simulation(tchecker::parsing::system_declaration_t const & sysdecl);

} // namespace tck_simulate

} // namespace tchecker

#endif // TCHECKER_TCK_SIMULATE_SIMULATE_HH
