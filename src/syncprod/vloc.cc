/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>

#include "tchecker/syncprod/vloc.hh"
#include "tchecker/utils/ordering.hh"

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

int lexical_cmp(tchecker::vloc_t const & vloc1, tchecker::vloc_t const & vloc2)
{
  return tchecker::lexical_cmp(
      vloc1.begin(), vloc1.end(), vloc2.begin(), vloc2.end(),
      [](tchecker::loc_id_t id1, tchecker::loc_id_t id2) -> int { return (id1 < id2 ? -1 : (id1 == id2 ? 0 : 1)); });
}

} // end of namespace tchecker
