/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/fsm/transition.hh"

namespace tchecker {

namespace fsm {

bool operator==(tchecker::fsm::transition_t const & t1, tchecker::fsm::transition_t const & t2)
{
  return tchecker::syncprod::operator==(t1, t2);
}

bool operator!=(tchecker::fsm::transition_t const & t1, tchecker::fsm::transition_t const & t2) { return !(t1 == t2); }

bool shared_equal_to(tchecker::fsm::transition_t const & t1, tchecker::fsm::transition_t const & t2)
{
  return tchecker::syncprod::shared_equal_to(t1, t2);
}

std::size_t hash_value(tchecker::fsm::transition_t const & t) { return tchecker::syncprod::hash_value(t); }

std::size_t shared_hash_value(tchecker::fsm::transition_t const & t) { return tchecker::syncprod::shared_hash_value(t); }

int lexical_cmp(tchecker::fsm::transition_t const & t1, tchecker::fsm::transition_t const & t2)
{
  return tchecker::syncprod::lexical_cmp(t1, t2);
}

} // end of namespace fsm

} // end of namespace tchecker
