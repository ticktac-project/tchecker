/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_DBM_DB_HH
#define TCHECKER_DBM_DB_HH

#include <cstdint>
#include <iostream>

#include "tchecker/basictypes.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file db.hh
 \brief Difference bounds <=c and <c for DBMs
 \note Compile with DBM_UNSAFE to disable overflow/underflow checking (not recommended)
 */

namespace tchecker {
  
  namespace dbm {
    
    /* IMPLEMENTATION NOTES:
     Difference bound #c is encoded in a 32-bits integer with the least-significant bit used to store the
     comparator #: LE (<=) or LT (<), and the other bits used to store c.
     
     Comparison with "cleaner" implementations, e.g.:
     struct db_t {                            struct db_t {
       unsigned cmp : 1;                        int32_t db;  // managed as db_t below
       int value : 31;                        };
     };
     have shown that current implementation is the fastest. This is due to using a primitive non-composite
     type (no constructor), and to arithmetic and bit-level operations.
     */
    
    /*!
     \brief Type of difference bounds
     */
    using db_t = tchecker::integer_t;
    
    static_assert(std::is_same<tchecker::integer_t, tchecker::dbm::db_t>::value, "");
    
    /*!
     \brief Type of difference bound comparator: < or <=
     */
    enum comparator_t {
      LT = 0,  /*!< less-than < */
      LE = 1,  /*!< less-than-or-equal-to, i.e. <= */
    };
    
    static_assert(static_cast<int>(tchecker::clock_constraint_t::LE) == static_cast<int>(tchecker::dbm::LE), "");
    static_assert(static_cast<int>(tchecker::clock_constraint_t::LT) == static_cast<int>(tchecker::dbm::LT), "");


    tchecker::dbm::db_t const INF_VALUE = tchecker::int_maxval >> 1;   /*!< Infinity value */
    tchecker::dbm::db_t const MAX_VALUE = INF_VALUE - 1;   /*!< Maximum value */
    tchecker::dbm::db_t const MIN_VALUE = tchecker::int_minval >> 1;  /*!< Minimum value */
    
    
    tchecker::dbm::db_t const LE_ZERO = (0 << 1) | tchecker::dbm::LE;             /*!< <=0 */
    tchecker::dbm::db_t const LT_ZERO = (0 << 1) | tchecker::dbm::LT;             /*!< <0 */
    tchecker::dbm::db_t const LT_INFINITY = (INF_VALUE << 1) | tchecker::dbm::LT; /*!< <inf */
    
    /*!
     \brief Build a difference bound
     \param cmp : a comparator
     \param value : a value
     \pre MIN_VALUE <= value <= MAX_VALUE
     \return <value if cmp is LT and <=value if cmp is LE
     \throw std::invalid_argument : if value is not between MIN_VALUE and MAX_VALUE (only if compilation flag DBM_UNSAFE is not set)
     */
    inline tchecker::dbm::db_t db(enum tchecker::dbm::comparator_t cmp, tchecker::integer_t value)
    {
#if !defined(DBM_UNSAFE)
      if ((value < MIN_VALUE) || (value > MAX_VALUE))
        throw std::invalid_argument("value out of bounds");
#endif // DBM_UNSAFE
      return ((value << 1) | cmp);
    }
    
    /*!
     \brief Sum of difference bounds
     \param db1 : a difference bound
     \param db2 : a difference bound
     \pre db1 + db2 can be represented
     \return db1 + db2, i.e. the difference bound #c where c is the sum of values in db1 and db2, and # is LT is at least one of
     db1 and db2 is LT, and # is LE otherwise
     \throw std::invalid_argument : if the sum cannot be represented as a tchecker::dbm::db_t (only if compilation flag DBM_UNSAFE
     is not set)
     */
    inline tchecker::dbm::db_t sum(tchecker::dbm::db_t db1, tchecker::dbm::db_t db2)
    {
      if ((db1 == tchecker::dbm::LT_INFINITY) || (db2 == tchecker::dbm::LT_INFINITY))
        return tchecker::dbm::LT_INFINITY;
#if defined(DBM_UNSAFE)
      return ((db1 + db2) - ((db1 | db2) & 1));
#else
      return tchecker::dbm::db(((db1 & tchecker::dbm::LE) && (db2 & tchecker::dbm::LE))
                               ? tchecker::dbm::LE
                               : tchecker::dbm::LT,
                               (db1 >> 1) + (db2 >> 1));
#endif // DBM_UNSAFE
    }
    
    /*!
     \brief Add an integer to a difference bound
     \param db : a difference bound
     \param value : a value
     \pre `db + value` can be represented
     \return #c where # is the comparator in db, and c is value plus the value in db
     \throw std::invalid_argument : if the new value cannot be represented as a tchecker::dbm::db_t (only if compilation flag
     DBM_UNSAFE is not set)
     */
    inline tchecker::dbm::db_t add(tchecker::dbm::db_t db, tchecker::integer_t value)
    {
      if (db == tchecker::dbm::LT_INFINITY)
        return tchecker::dbm::LT_INFINITY;
#if defined(DBM_UNSAFE)
      return (db + (value << 1));
#else
      return tchecker::dbm::db(((db & tchecker::dbm::LE) ? tchecker::dbm::LE : tchecker::dbm::LT),
                               (db >> 1) + value);
#endif // DBM_UNSAFE
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
    inline tchecker::dbm::db_t min(tchecker::dbm::db_t db1, tchecker::dbm::db_t db2)
    {
      return (db1 < db2 ? db1 : db2);
    }
    
    /*!
     \brief Maximum of difference bounds
     \param db1 : a difference bound
     \param db2 : a difference bound
     \return db1 if db1 > db2, db2 otherwise
     */
    inline tchecker::dbm::db_t max(tchecker::dbm::db_t db1, tchecker::dbm::db_t db2)
    {
      return (db1 > db2 ? db1 : db2);
    }

    /*!
     \brief Comparison of difference bounds
     \param db1 : a difference bound
     \param db2 : a difference bound
     \return 0 if db1 and db2 are equal, a negative value if db1 is smaller than db2, a positive value otherwise
     */
    inline int db_cmp(tchecker::dbm::db_t db1, tchecker::dbm::db_t db2)
    {
      return (db1 < db2 ? -1 : (db1 == db2 ? 0 : 1));
    }
    
    /*!
     \brief Accessor
     \param db : a difference bound
     \return the comparator in db
     */
    inline enum tchecker::dbm::comparator_t comparator(tchecker::dbm::db_t db)
    {
      return ((db & tchecker::dbm::LE) ? tchecker::dbm::LE : tchecker::dbm::LT);
    }
    
    /*!
     \brief Accessor
     \param db : a difference bound
     \return value of db
     */
    inline tchecker::integer_t value(tchecker::dbm::db_t db)
    {
      return (db >> 1);
    }
    
    /*!
     \brief Accessor
     \param db : a difference bound
     \return hash value for db
     */
    inline std::size_t hash(tchecker::dbm::db_t db)
    {
      return db;
    }
    
    /*!
     \brief Output a difference bound
     \param os : output stream
     \param db : a difference bound
     \post db has been output to os
     \return os after output
     */
    std::ostream & output(std::ostream & os, tchecker::dbm::db_t db);
    
    /*!
     \brief Accessor
     \param db : a difference bound
     \return a string translation of the comparator in db
     */
    std::string comparator_str(tchecker::dbm::db_t db);
    
  } // end of namespace dbm
  
} // end of namespace tchecker

#endif // TCHECKER_DBM_DB_HH
