/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>

#include "tchecker/syncprod/vedge.hh"
#include "tchecker/utils/ordering.hh"

namespace tchecker {

/* vedge_t */

vedge_t::vedge_t(unsigned int size) : tchecker::edge_array_t(std::make_tuple(size), std::make_tuple(tchecker::NO_EDGE)) {}

tchecker::vedge_t::iterator_t vedge_t::begin()
{
  return tchecker::vedge_t::iterator_t(tchecker::valid_edge_id, tchecker::edge_array_t::begin(), tchecker::edge_array_t::end());
}

tchecker::vedge_t::iterator_t vedge_t::end()
{
  return tchecker::vedge_t::iterator_t(tchecker::valid_edge_id, tchecker::edge_array_t::end(), tchecker::edge_array_t::end());
}

tchecker::vedge_t::const_iterator_t vedge_t::begin() const
{
  return tchecker::vedge_t::const_iterator_t(tchecker::valid_edge_id, tchecker::edge_array_t::begin(),
                                             tchecker::edge_array_t::end());
}

tchecker::vedge_t::const_iterator_t vedge_t::end() const
{
  return tchecker::vedge_t::const_iterator_t(tchecker::valid_edge_id, tchecker::edge_array_t::end(),
                                             tchecker::edge_array_t::end());
}

tchecker::vedge_t::array_iterator_t vedge_t::begin_array() { return tchecker::edge_array_t::begin(); }

tchecker::vedge_t::array_iterator_t vedge_t::end_array() { return tchecker::edge_array_t::end(); }

tchecker::vedge_t::const_array_iterator_t vedge_t::begin_array() const { return tchecker::edge_array_t::begin(); }

tchecker::vedge_t::const_array_iterator_t vedge_t::end_array() const { return tchecker::edge_array_t::end(); }

void vedge_destruct_and_deallocate(tchecker::vedge_t * vedge)
{
  tchecker::vedge_t::destruct(vedge);
  delete[] reinterpret_cast<char *>(vedge);
}

std::ostream & output(std::ostream & os, tchecker::vedge_t const & vedge, tchecker::system::system_t const & system)
{
  auto begin = vedge.begin(), end = vedge.end();
  os << "<";
  for (auto it = begin; it != end; ++it) {
    if (it != begin)
      os << ",";
    auto edge = system.edge(*it);
    os << system.process_name(edge->pid()) << "@" << system.event_name(edge->event_id());
  }
  os << ">";
  return os;
}

std::string to_string(tchecker::vedge_t const & vedge, tchecker::system::system_t const & system)
{
  std::stringstream sstream;
  tchecker::output(sstream, vedge, system);
  return sstream.str();
}

int lexical_cmp(tchecker::vedge_t const & vedge1, tchecker::vedge_t const & vedge2)
{
  return tchecker::lexical_cmp(
      vedge1.begin(), vedge1.end(), vedge2.begin(), vedge2.end(),
      [](tchecker::edge_id_t id1, tchecker::edge_id_t id2) -> int { return (id1 < id2 ? -1 : (id1 == id2 ? 0 : 1)); });
}

} // end of namespace tchecker
