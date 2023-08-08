/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/fsm/state.hh"

namespace tchecker {

namespace fsm {

bool operator==(tchecker::fsm::state_t const & s1, tchecker::fsm::state_t const & s2)
{
  return tchecker::ta::operator==(s1, s2);
}

bool operator!=(tchecker::fsm::state_t const & s1, tchecker::fsm::state_t const & s2) { return !(s1 == s2); }

bool shared_equal_to(tchecker::fsm::state_t const & s1, tchecker::fsm::state_t const & s2)
{
  return tchecker::ta::shared_equal_to(s1, s2);
}

std::size_t hash_value(tchecker::fsm::state_t const & s) { return tchecker::ta::hash_value(s); }

std::size_t shared_hash_value(tchecker::fsm::state_t const & s) { return tchecker::ta::shared_hash_value(s); }

int lexical_cmp(tchecker::fsm::state_t const & s1, tchecker::fsm::state_t const & s2)
{
  return tchecker::ta::lexical_cmp(s1, s2);
}

} // end of namespace fsm

} // end of namespace tchecker
