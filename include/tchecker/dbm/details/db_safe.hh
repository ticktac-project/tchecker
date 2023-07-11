/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_DBM_DB_SAFE_HH
#define TCHECKER_DBM_DB_SAFE_HH

#include "tchecker/basictypes.hh"

/*!
 \file db_safe.hh
 \brief Difference bounds <=c and <c for DBMs
 \note This implementation is slightly slower than db_unsafe.hh, but it checks for
 overflow/underflow, it is portable, and conforms to standards
 */

namespace tchecker {

namespace dbm {

static_assert(tchecker::LT < tchecker::LE, "tchecker::LT need to be smaller than tchecker::LE");

/*!
 \brief Strengthen
 \param cmp1 : comparator
 \param cmp2 : comparator
 \return stronger comparator between cmp1 and cmp2
*/
constexpr enum tchecker::ineq_cmp_t comparator_stronger(enum tchecker::ineq_cmp_t cmp1, enum tchecker::ineq_cmp_t cmp2)
{
  return (cmp1 < cmp2 ? cmp1 : cmp2);
}

/*!
 \brief Type of difference bounds
 */
struct db_t {
  enum tchecker::ineq_cmp_t cmp : 1;
#if (INTEGER_T_SIZE == 64)
  tchecker::integer_t value : 63;
#elif (INTEGER_T_SIZE == 32)
  tchecker::integer_t value : 31;
#elif (INTEGER_T_SIZE == 16)
  tchecker::integer_t value : 15;
#else
#error Only 64/32/16 int bit supported
#endif
};

/*!
 \brief Equality operator
 \param db1 : difference bound
 \param db2 : difference bound
 \return true if db1 and db2 are equal (same comparator, same value), false otherwise
*/
constexpr bool operator==(struct db_t const & db1, struct db_t const & db2)
{
  return (db1.cmp == db2.cmp) && (db1.value == db2.value);
}

/*!
 \brief Disequality operator
 \param db1 : difference bound
 \param db2 : difference bound
 \return false if db1 and db2 are equal (same comparator, same value), true otherwise
*/
constexpr bool operator!=(struct db_t const & db1, struct db_t const & db2) { return !(db1 == db2); }

tchecker::integer_t const INF_VALUE = tchecker::int_maxval >> 1; /*!< Infinity value */
tchecker::integer_t const MAX_VALUE = INF_VALUE - 1;             /*!< Maximum value */
tchecker::integer_t const MIN_VALUE = tchecker::int_minval / 2;  /*!< Minimum value (negative: >> 1 is unspecified) */

static_assert(tchecker::dbm::INF_VALUE != tchecker::dbm::MAX_VALUE, "");
static_assert(tchecker::dbm::INF_VALUE != tchecker::dbm::MIN_VALUE, "");
static_assert(tchecker::dbm::MAX_VALUE != tchecker::dbm::MIN_VALUE, "");

constexpr tchecker::dbm::db_t const LE_ZERO = {tchecker::LE, 0};             /*!< <=0 */
constexpr tchecker::dbm::db_t const LT_ZERO = {tchecker::LT, 0};             /*!< <0 */
constexpr tchecker::dbm::db_t const LT_INFINITY = {tchecker::LT, INF_VALUE}; /*!< <inf */

static_assert(tchecker::dbm::LE_ZERO != tchecker::dbm::LT_ZERO, "");
static_assert(tchecker::dbm::LT_ZERO != tchecker::dbm::LT_INFINITY, "");
static_assert(tchecker::dbm::LE_ZERO != tchecker::dbm::LT_INFINITY, "");

/*!
 \brief Build a difference bound
 \param cmp : a comparator
 \param value : a value
 \pre MIN_VALUE <= value <= MAX_VALUE
 \return <value if cmp is LT and <=value if cmp is LE
 \throw std::invalid_argument : if value is not between MIN_VALUE and MAX_VALUE
 */
inline tchecker::dbm::db_t db(enum tchecker::ineq_cmp_t cmp, tchecker::integer_t value)
{
  if ((value < MIN_VALUE) || (value > MAX_VALUE))
    throw std::invalid_argument("value out of bounds");
  return db_t{cmp, value};
}

/*!
 \brief Sum of difference bounds
 \param db1 : a difference bound
 \param db2 : a difference bound
 \pre db1 + db2 can be represented
 \return db1 + db2, i.e. the difference bound #c where c is the sum of values in db1 and db2, and # is LT is at least one of
 db1 and db2 is LT, and # is LE otherwise
 \throw std::invalid_argument : if the sum cannot be represented as a tchecker::dbm::db_t
 \throw std::overflow_error : if the sum produces an integer overflow
 \throw std::underfow_error : if the sum produces an integer underflow
 */
inline tchecker::dbm::db_t sum(tchecker::dbm::db_t const & db1, tchecker::dbm::db_t const & db2)
{
  if ((db1 == tchecker::dbm::LT_INFINITY) || (db2 == tchecker::dbm::LT_INFINITY))
    return tchecker::dbm::LT_INFINITY;
  tchecker::integer_t sum = db1.value + db2.value;
  if (db1.value > 0 && db2.value > 0 && sum < db1.value)
    throw std::overflow_error("Overflow in sum of difference bounds");
  if (db1.value < 0 && db2.value < 0 && sum > db1.value)
    throw std::underflow_error("Underflow in sum of difference bounds");
  return tchecker::dbm::db(tchecker::dbm::comparator_stronger(db1.cmp, db2.cmp), sum);
}

/*!
 \brief Add an integer to a difference bound
 \param db : a difference bound
 \param value : a value
 \pre `db + value` can be represented
 \return #c where # is the comparator in db, and c is value plus the value in db
 \throw std::invalid_argument : see tchecker::dbm::sum
 \throw std::overflow_error : see tchecker::dbm::sum
 \throw std::underflow_error : see tchecker::dbm::sum
 */
inline tchecker::dbm::db_t add(tchecker::dbm::db_t const & db, tchecker::integer_t value)
{
  return tchecker::dbm::sum(db, db_t{tchecker::LE, value});
}

/*!
 \brief Comparison of difference bounds according to the relative strength of db1 and db2
 \param db1 : a difference bound
 \param db2 : a difference bound
 \return 0 if db1 and db2 are equal, a negative value if db1 is smaller than db2, a positive value otherwise
 */
inline int db_cmp(tchecker::dbm::db_t const & db1, tchecker::dbm::db_t const & db2)
{
  if (db1.value != db2.value)
    return (db1.value < db2.value) ? -1 : 1;
  return db1.cmp - db2.cmp;
}

/*!
 \brief Less-than operator on difference bounds
 \param db1 : a difference bound
 \param db2 : a difference bound
 \return true if db1 is stronger than db2, false otherwise
*/
inline bool operator<(tchecker::dbm::db_t const & db1, tchecker::dbm::db_t const & db2)
{
  return tchecker::dbm::db_cmp(db1, db2) < 0;
}

/*!
 \brief Less-than-or-equal operator on difference bounds
 \param db1 : a difference bound
 \param db2 : a difference bound
 \return true if db1 is stronger or equal to db2, false otherwise
*/
inline bool operator<=(tchecker::dbm::db_t const & db1, tchecker::dbm::db_t const & db2)
{
  return tchecker::dbm::db_cmp(db1, db2) <= 0;
}

/*!
 \brief Greater-than-or-equal operator on difference bounds
 \param db1 : a difference bound
 \param db2 : a difference bound
 \return true if db1 is weaker or equal to db2, false otherwise
*/
inline bool operator>=(tchecker::dbm::db_t const & db1, tchecker::dbm::db_t const & db2)
{
  return tchecker::dbm::db_cmp(db1, db2) >= 0;
}

/*!
 \brief Greater-than operator on difference bounds
 \param db1 : a difference bound
 \param db2 : a difference bound
 \return true if db1 is weaker than db2, false otherwise
*/
inline bool operator>(tchecker::dbm::db_t const & db1, tchecker::dbm::db_t const & db2)
{
  return tchecker::dbm::db_cmp(db1, db2) > 0;
}

/*!
 \brief Minimum of difference bounds
 \param db1 : a difference bound
 \param db2 : a difference bound
 \return db1 if db1 < db2, db2 otherwise
 */
inline tchecker::dbm::db_t min(tchecker::dbm::db_t const & db1, tchecker::dbm::db_t const & db2)
{
  return tchecker::dbm::db_cmp(db1, db2) <= 0 ? db1 : db2;
}

/*!
 \brief Maximum of difference bounds
 \param db1 : a difference bound
 \param db2 : a difference bound
 \return db1 if db1 > db2, db2 otherwise
 */
inline tchecker::dbm::db_t max(tchecker::dbm::db_t const & db1, tchecker::dbm::db_t const & db2)
{
  return tchecker::dbm::db_cmp(db1, db2) <= 0 ? db2 : db1;
}

/*!
 \brief Accessor
 \param db : a difference bound
 \return the comparator in db
 */
inline enum tchecker::ineq_cmp_t comparator(tchecker::dbm::db_t const & db) { return db.cmp; }

/*!
 \brief Accessor
 \param db : a difference bound
 \return value of db
 */
inline tchecker::integer_t value(tchecker::dbm::db_t const & db) { return db.value; }

/*!
 \brief Accessor
 \param db : a difference bound
 \return hash value for db
 */
inline std::size_t hash(tchecker::dbm::db_t const & db) { return 2 * static_cast<std::size_t>(db.value) + db.cmp; }

} // end of namespace dbm

} // end of namespace tchecker

#endif // TCHECKER_DBM_DB_SAFE_HH
