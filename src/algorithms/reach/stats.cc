/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>

#include "tchecker/algorithms/reach/stats.hh"

namespace tchecker {

namespace algorithms {

namespace reach {

stats_t::stats_t() : _visited_states(0), _reachable(false) {}

unsigned long & stats_t::visited_states() { return _visited_states; }

unsigned long stats_t::visited_states() const { return _visited_states; }

bool & stats_t::reachable() { return _reachable; }

bool stats_t::reachable() const { return _reachable; }

void stats_t::attributes(std::map<std::string, std::string> & m) const
{
  tchecker::algorithms::stats_t::attributes(m);

  std::stringstream sstream;

  sstream << _visited_states;
  m["VISITED_STATES"] = sstream.str();

  sstream.str("");
  sstream << std::boolalpha << _reachable;
  m["REACHABLE"] = sstream.str();
}

} // end of namespace reach

} // end of namespace algorithms

} // end of namespace tchecker
