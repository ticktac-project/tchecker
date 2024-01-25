/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>

#include "tchecker/algorithms/covreach/stats.hh"

namespace tchecker {

namespace algorithms {
namespace covreach {

stats_t::stats_t() : _visited_states(0), _visited_transitions(0), _covered_states(0), _stored_states(0), _reachable(false) {}

unsigned long & stats_t::visited_states() { return _visited_states; }

unsigned long stats_t::visited_states() const { return _visited_states; }

unsigned long & stats_t::visited_transitions() { return _visited_transitions; }

unsigned long stats_t::visited_transitions() const { return _visited_transitions; }

unsigned long & stats_t::covered_states() { return _covered_states; }

unsigned long stats_t::covered_states() const { return _covered_states; }

unsigned long & stats_t::stored_states() { return _stored_states; }

unsigned long stats_t::stored_states() const { return _stored_states; }

bool & stats_t::reachable() { return _reachable; }

bool stats_t::reachable() const { return _reachable; }

void stats_t::attributes(std::map<std::string, std::string> & m) const
{
  tchecker::algorithms::stats_t::attributes(m);

  std::stringstream sstream;

  sstream << _visited_states;
  m["VISITED_STATES"] = sstream.str();

  sstream.str("");
  sstream << _visited_transitions;
  m["VISITED_TRANSITIONS"] = sstream.str();

  sstream.str("");
  sstream << _covered_states;
  m["COVERED_STATES"] = sstream.str();

  sstream.str("");
  sstream << _stored_states;
  m["STORED_STATES"] = sstream.str();

  sstream.str("");
  sstream << std::boolalpha << _reachable;
  m["REACHABLE"] = sstream.str();
}

} // end of namespace covreach

} // namespace algorithms

} // end of namespace tchecker
