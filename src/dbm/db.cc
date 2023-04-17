/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <ostream>

#include "tchecker/dbm/db.hh"

namespace tchecker {

namespace dbm {

std::ostream & output(std::ostream & os, tchecker::dbm::db_t const & db)
{
  if (db == tchecker::dbm::LT_INFINITY)
    os << "<inf";
  else
    os << tchecker::dbm::comparator_str(db) << tchecker::dbm::value(db);
  return os;
}

std::string comparator_str(tchecker::dbm::db_t const & db)
{
  return (tchecker::dbm::comparator(db) == tchecker::dbm::LE ? "<=" : "<");
}

} // namespace dbm

} // end of namespace tchecker
