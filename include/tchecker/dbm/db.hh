/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

/*!
 \file db.hh
 \brief Difference bounds <=c and <c for DBMs
 \note We provide two implementations: safe DBMs which are portable and check for
 integer overflow/underflow, and unsafe DBMs which are slightly faster but are not
 portable and rely on unspecified compiler implementation. Default implementation
 is the safe one (highly recommended), set TCHECKER_DBM_UNSAFE to use the
 (historical) unsafe implementation instead
 */

#ifdef TCHECKER_DBM_UNSAFE
#include "tchecker/dbm/details/db_unsafe.hh"
#else
#include "tchecker/dbm/details/db_safe.hh"
#endif // TCHECKER_DBM_UNSAFE

#ifndef TCHECKER_DBM_DB_HH
#define TCHECKER_DBM_DB_HH

#include <iostream>

namespace tchecker {

namespace dbm {

/*!
 \brief Output a difference bound
 \param os : output stream
 \param db : a difference bound
 \post db has been output to os
 \return os after output
 */
std::ostream & output(std::ostream & os, tchecker::dbm::db_t const & db);

/*!
 \brief Accessor
 \param db : a difference bound
 \return a string translation of the comparator in db
 */
std::string comparator_str(tchecker::dbm::db_t const & db);

} // end of namespace dbm

} // end of namespace tchecker

#endif // TCHECKER_DBM_DB_HH
