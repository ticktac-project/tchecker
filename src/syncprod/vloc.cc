/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <ostream>
#include <regex>
#include <sstream>

#include "tchecker/syncprod/vloc.hh"
#include "tchecker/utils/ordering.hh"
#include "tchecker/utils/string.hh"

namespace tchecker {

vloc_t::vloc_t(unsigned int size) : tchecker::loc_array_t(std::make_tuple(size), std::make_tuple(tchecker::NO_LOC)) {}

void vloc_destruct_and_deallocate(tchecker::vloc_t * vloc)
{
  tchecker::vloc_t::destruct(vloc);
  delete[] reinterpret_cast<char *>(vloc);
}

std::ostream & output(std::ostream & os, tchecker::vloc_t const & vloc, tchecker::system::system_t const & system)
{
  auto begin = vloc.begin(), end = vloc.end();
  os << "<";
  for (auto it = begin; it != end; ++it) {
    if (it != begin)
      os << ",";
    os << system.location(*it)->name();
  }
  os << ">";
  return os;
}

std::string to_string(tchecker::vloc_t const & vloc, tchecker::system::system_t const & system)
{
  std::stringstream sstream;
  tchecker::output(sstream, vloc, system);
  return sstream.str();
}

void from_string(tchecker::vloc_t & vloc, tchecker::system::system_t const & system, std::string const & str)
{
  // Matches text of the form: <ID,ID,...,ID> where ID is a alphanum
  const std::regex vloc_regex("<[a-zA-Z_][a-zA-Z0-9_.]*(,[a-zA-Z_][a-zA-Z0-9_.]*)*>");
  if (!std::regex_match(str, vloc_regex))
    throw std::invalid_argument("tchecker::from_string: syntax error, str should be of the form <l0,l1,...,l2>");

  // Get the list of location names in str
  std::vector<std::string> location_names = tchecker::split(str.substr(1, str.size() - 2), ',');

  // Initialize vloc from the list of location names
  if (location_names.size() != vloc.capacity())
    throw std::invalid_argument("tchecker::from_string: expecting " + std::to_string(vloc.capacity()) +
                                " location(s) but got " + std::to_string(location_names.size()) + " in " + str);

  for (tchecker::process_id_t pid = 0; pid < vloc.capacity(); ++pid) {
    try {
      tchecker::system::loc_const_shared_ptr_t loc = system.location(pid, location_names[pid]);
      vloc[pid] = loc->id();
    }
    catch (std::invalid_argument & e) {
      throw std::invalid_argument("tchecker::from_string: unknown location " + location_names[pid] + " in process " +
                                  system.process_name(pid));
    }
  }
}

int lexical_cmp(tchecker::vloc_t const & vloc1, tchecker::vloc_t const & vloc2)
{
  return tchecker::lexical_cmp(
      vloc1.begin(), vloc1.end(), vloc2.begin(), vloc2.end(),
      [](tchecker::loc_id_t id1, tchecker::loc_id_t id2) -> int { return (id1 < id2 ? -1 : (id1 == id2 ? 0 : 1)); });
}

} // end of namespace tchecker
