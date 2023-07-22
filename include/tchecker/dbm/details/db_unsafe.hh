/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_DBM_DB_UNSAFE_HH
#define TCHECKER_DBM_DB_UNSAFE_HH

#include "tchecker/basictypes.hh"

/*!
 \file db_unsafe.hh
 \brief Difference bounds <=c and <c for DBMs
 \note This implementation is fast but not safe: overflow/underflow may occur,
 it relies on implementation-dependent behaviors of compilers, and it does not compile
 on architectures that do not use two-complement representation of signed ints
 */

namespace tchecker {

namespace dbm {

/* IMPLEMENTATION NOTES:
 Difference bounds (<=c) and (<c) are encoded using integers with the least-significant
 bit used to store the comparator: LE (<=) or LT (<), and the other bits used to store c.
 */

static_assert(-1 == (~1 + 1), "Unsafe DBMs: expecting two-complement's representation of signed integers");

/*!
 \brief Type of difference bounds
 */
using db_t = tchecker::integer_t;

static_assert(std::is_same<tchecker::integer_t, tchecker::dbm::db_t>::value, "");

static_assert(tchecker::LT == 0, "tchecker::LT must be 0");
static_assert(tchecker::LE == 1, "tchecker::LT must be 1");

tchecker::dbm::db_t const INF_VALUE = tchecker::int_maxval >> 1; /*!< Infinity value */
tchecker::dbm::db_t const MAX_VALUE = INF_VALUE - 1;             /*!< Maximum value */
tchecker::dbm::db_t const MIN_VALUE = tchecker::int_minval / 2;  /*!< Minimum value (negative: >> 1 is unspecified) */

static_assert(tchecker::dbm::INF_VALUE != tchecker::dbm::MAX_VALUE, "");
static_assert(tchecker::dbm::INF_VALUE != tchecker::dbm::MIN_VALUE, "");
static_assert(tchecker::dbm::MAX_VALUE != tchecker::dbm::MIN_VALUE, "");

tchecker::dbm::db_t const LE_ZERO = (0 << 1) | tchecker::LE;                  /*!< <=0 */
tchecker::dbm::db_t const LT_ZERO = (0 << 1) | tchecker::LT;                  /*!< <0 */
tchecker::dbm::db_t const LT_INFINITY = (INF_VALUE << 1) | tchecker::dbm::LT; /*!< <inf */

static_assert(tchecker::dbm::LE_ZERO != tchecker::dbm::LT_ZERO, "");
static_assert(tchecker::dbm::LT_ZERO != tchecker::dbm::LT_INFINITY, "");
static_assert(tchecker::dbm::LE_ZERO != tchecker::dbm::LT_INFINITY, "");

/*!
 \brief Build a difference bound
 \param cmp : a comparator
 \param value : a value
 \pre tchecker::dbm::MIN_VALUE <= value <= tchecker::dbm::MAX_VALUE
 \return <value if cmp is LT and <=value if cmp is LE
 \note it is not checked whether value is between tchecker::dbm::MIN_VALUE and
 tchecker::dbm::MAX_VALUE, hence overflow/underflow may occur
 \note the implementation relies on the left-shift operator which is unspecified
 when value is negative. Hence, semantics is implementation-dependent. In particular,
 this is incorrect on architectures that do not use two-complement representation
 of signed integers (checked by static_assert)
 */
inline tchecker::dbm::db_t db(enum tchecker::ineq_cmp_t cmp, tchecker::integer_t value) { return ((value << 1) | cmp); }

/*!
 \brief Sum of difference bounds
 \param db1 : a difference bound
 \param db2 : a difference bound
 \pre db1 + db2 can be represented
 \return db1 + db2, i.e. the difference bound #c where c is the sum of values in db1 and db2, and # is LT is at least one of
 db1 and db2 is LT, and # is LE otherwise
 */
inline tchecker::dbm::db_t sum(tchecker::dbm::db_t db1, tchecker::dbm::db_t db2)
{
  if ((db1 == tchecker::dbm::LT_INFINITY) || (db2 == tchecker::dbm::LT_INFINITY))
    return tchecker::dbm::LT_INFINITY;
  return ((db1 + db2) - ((db1 | db2) & 1));
}

/*!
 \brief Add an integer to a difference bound
 \param db : a difference bound
 \param value : a value
 \pre `db + value` can be represented
 \return #c where # is the comparator in db, and c is value plus the value in db
 */
inline tchecker::dbm::db_t add(tchecker::dbm::db_t db, tchecker::integer_t value)
{
  if (db == tchecker::dbm::LT_INFINITY)
    return tchecker::dbm::LT_INFINITY;
  return (db + (value << 1));
}

/*!
 \note Standard comparison operators <, <=, ==, !=, >= and > on integers carry on difference bounds
 */

/*!
 \brief Minimum of difference bounds
 \param db1 : a difference bound
 \param db2 : a difference bound
 \return db1 if db1 < db2, db2 otherwise
 */
inline tchecker::dbm::db_t min(tchecker::dbm::db_t db1, tchecker::dbm::db_t db2) { return (db1 < db2 ? db1 : db2); }

/*!
 \brief Maximum of difference bounds
 \param db1 : a difference bound
 \param db2 : a difference bound
 \return db1 if db1 > db2, db2 otherwise
 */
inline tchecker::dbm::db_t max(tchecker::dbm::db_t db1, tchecker::dbm::db_t db2) { return (db1 > db2 ? db1 : db2); }

/*!
 \brief Comparison of difference bounds
 \param db1 : a difference bound
 \param db2 : a difference bound
 \return 0 if db1 and db2 are equal, a negative value if db1 is smaller than db2, a positive value otherwise
 */
inline int db_cmp(tchecker::dbm::db_t db1, tchecker::dbm::db_t db2) { return (db1 < db2 ? -1 : (db1 == db2 ? 0 : 1)); }

/*!
 \brief Accessor
 \param db : a difference bound
 \return the comparator in db
 */
inline enum tchecker::ineq_cmp_t comparator(tchecker::dbm::db_t db)
{
  return ((db & tchecker::LE) ? tchecker::LE : tchecker::LT);
}

/*!
 \brief Accessor
 \param db : a difference bound
 \return value of db
 */
inline tchecker::integer_t value(tchecker::dbm::db_t db) { return (db >> 1); }

/*!
 \brief Accessor
 \param db : a difference bound
 \return hash value for db
 */
inline std::size_t hash(tchecker::dbm::db_t db) { return static_cast<std::size_t>(db); }

} // end of namespace dbm

} // end of namespace tchecker

#endif // TCHECKER_DBM_DB_UNSAFE_HH
