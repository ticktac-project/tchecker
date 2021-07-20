/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/algorithms/search_order.hh"

namespace tchecker {

namespace algorithms {

enum tchecker::waiting::policy_t waiting_policy(std::string const & search_order)
{
  if (search_order == "dfs")
    return tchecker::waiting::STACK;
  else if (search_order == "bfs")
    return tchecker::waiting::QUEUE;
  throw std::invalid_argument("Unknown search order: " + search_order);
}

enum tchecker::waiting::policy_t fast_remove_waiting_policy(std::string const & search_order)
{
  if (search_order == "dfs")
    return tchecker::waiting::FAST_REMOVE_STACK;
  else if (search_order == "bfs")
    return tchecker::waiting::FAST_REMOVE_QUEUE;
  throw std::invalid_argument("Unknown search order: " + search_order);
}

} // end of namespace algorithms

} // end of namespace tchecker
