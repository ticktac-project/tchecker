/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_SEARCH_ORDER_HH
#define TCHECKER_ALGORITHMS_SEARCH_ORDER_HH

#include <string>

#include "tchecker/waiting/factory.hh"

/*!
 \file search_order.hh
 \brief Conversion from search order to waiting policy
 */

namespace tchecker {

namespace algorithms {

/*!
 \brief Conversion from search order to waiting policy
 \param search_order : search order
 \pre search_order is either "dfs" or "bfs"
 \return tchecker::waiting::STACK if search_order is "dfs",
 tchecker::waiting::QUEUE if search_order is "bfs"
 \throw std::invalid_argument if the precondition is not satisfied
*/
enum tchecker::waiting::policy_t waiting_policy(std::string const & search_order);

/*!
 \brief Conversion from search order to waiting policy for fast remove waiting containers
 \param search_order : search order
 \pre search_order is either "dfs" or "bfs"
 \return tchecker::waiting::FAST_REMOVE_STACK if search_order is "dfs",
 tchecker::waiting::FAST_REMOVE_QUEUE if search_order is "bfs"
 \throw std::invalid_argument if the precondition is not satisfied
*/
enum tchecker::waiting::policy_t fast_remove_waiting_policy(std::string const & search_order);

} // end of namespace algorithms

} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_SEARCH_ORDER_HH