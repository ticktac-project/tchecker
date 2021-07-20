/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <iostream>
#include <limits>

#include "tchecker/basictypes.hh"

namespace tchecker {

/* loc_id_t */

tchecker::loc_id_t const NO_LOC = std::numeric_limits<tchecker::loc_id_t>::max();

bool valid_loc_id(tchecker::loc_id_t id) { return (id != tchecker::NO_LOC); }

/* edge_id_t */

tchecker::edge_id_t const NO_EDGE = std::numeric_limits<tchecker::edge_id_t>::max();

bool valid_edge_id(tchecker::edge_id_t id) { return (id != tchecker::NO_EDGE); }

/* sync_strength_t */

std::ostream & operator<<(std::ostream & os, enum tchecker::sync_strength_t s)
{
  if (s == tchecker::SYNC_WEAK)
    os << "?";
  return os;
}

} // end of namespace tchecker
