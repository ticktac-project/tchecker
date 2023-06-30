/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_BASICTYPES_HH
#define TCHECKER_BASICTYPES_HH

#include <cstdint>
#include <iostream>
#include <limits>
#include <tchecker/config.hh>

/*!
 \file basictypes.hh
 \brief Definition of basic types for models
 */

namespace tchecker {

/*!
 \brief Type of integers
 */
#if (INTEGER_T_SIZE == 64)
using integer_t = int64_t;
const integer_t int_maxval = INT64_MAX;
const integer_t int_minval = INT64_MIN;
#elif (INTEGER_T_SIZE == 32)
using integer_t = int32_t;
const integer_t int_maxval = INT32_MAX;
const integer_t int_minval = INT32_MIN;
#elif (INTEGER_T_SIZE == 16)
using integer_t = int16_t;
const integer_t int_maxval = INT16_MAX;
const integer_t int_minval = INT16_MIN;
#else
#error Only 64/32/16 int bit supported
#endif

/*!
 \brief Type of identifiers
*/
using id_t = uint32_t;

/*!
 \brief Type of event identifiers
 */
using event_id_t = tchecker::id_t;

/*!
 \brief Type of process identifiers
 */
using process_id_t = tchecker::id_t;

/*!
 \brief Type of variable indentifiers
 */
using variable_id_t = tchecker::id_t;

/*!
 \brief Type of variable size
 */
using variable_size_t = uint32_t;

/*!
 \brief Type of integer variable identifiers
 */
using intvar_id_t = tchecker::variable_id_t;

static_assert((std::numeric_limits<tchecker::intvar_id_t>::min() >= std::numeric_limits<tchecker::variable_id_t>::min()) &&
                  (std::numeric_limits<tchecker::intvar_id_t>::max() <= std::numeric_limits<tchecker::variable_id_t>::max()),
              "variable_id_t should contain intvar_id_t");

/*!
 \brief Type of clock identifiers
 */
using clock_id_t = tchecker::variable_id_t;

static_assert((std::numeric_limits<tchecker::clock_id_t>::min() >= std::numeric_limits<tchecker::variable_id_t>::min()) &&
                  (std::numeric_limits<tchecker::clock_id_t>::max() <= std::numeric_limits<tchecker::variable_id_t>::max()),
              "variable_id_t should contain clock_id_t");

/*!
\brief Definition of reference clock ID
*/
#if (INTEGER_T_SIZE == 64)
tchecker::clock_id_t const REFCLOCK_ID(std::numeric_limits<tchecker::clock_id_t>::max());
#elif (INTEGER_T_SIZE == 32)
tchecker::clock_id_t const REFCLOCK_ID(std::numeric_limits<tchecker::integer_t>::max());
#elif (INTEGER_T_SIZE == 16)
tchecker::clock_id_t const REFCLOCK_ID(std::numeric_limits<tchecker::integer_t>::max());
#else
#error Only 64/32/16 int bit supported
#endif

// required by vm_t
static_assert(REFCLOCK_ID <= std::numeric_limits<tchecker::clock_id_t>::max());
static_assert(REFCLOCK_ID <= std::numeric_limits<tchecker::integer_t>::max());

/*!
\brief Definition of reference clock name
*/
std::string const REFCLOCK_NAME("0");

/*!
 \brief Type inequality comparator
 */
enum ineq_cmp_t : unsigned {
  LT = 0, /*!< less-than < */
  LE = 1, /*!< less-than-or-equal-to, i.e. <= */
};

static_assert(tchecker::LT == 0, "Less-than should be 0");
static_assert(tchecker::LE == 1, "Less-than should be 1");

/*!
 \brief Type of label identifiers
 */
using label_id_t = tchecker::id_t;

/*!
 \brief Type of location identifiers
 */
using loc_id_t = tchecker::id_t;

/*!
 \brief Location identifier representing absence of location
 */
extern tchecker::loc_id_t const NO_LOC;

/*!
 \brief Checs if a location identifier is valid
 \param id : a location identifier
 \return true if id != NO_LOC, false otherwise;
 */
bool valid_loc_id(tchecker::loc_id_t id);

/*!
 \brief Type of edge identifiers
 */
using edge_id_t = tchecker::id_t;

/*!
 \brief Edge identifier representing absence of edge
 */
extern tchecker::edge_id_t const NO_EDGE;

/*!
 \brief Checks if an edge identifier is valid
 \param id : an edge identifier
 \return true if id != NO_EDGE, false otherwise
 */
bool valid_edge_id(tchecker::edge_id_t id);

/*!
 \brief Type of synchronization identifiers
 */
using sync_id_t = tchecker::id_t;

/*!
 \brief Type of node identifiers
 */
using node_id_t = tchecker::id_t; // should be large enough

/*!
 \brief Strength of synchronization constraint
 */
enum sync_strength_t {
  SYNC_WEAK,  /*!< Broadcast constraint */
  SYNC_STRONG /*!< Handshaking constraint */
};

/*!
 \brief Output operator
 \param os : output stream
 \param s : synchronization strength
 \return os after s has been output
 */
std::ostream & operator<<(std::ostream & os, enum tchecker::sync_strength_t s);

/*!
 \brief Status of states
 */
using state_status_t = unsigned int;

state_status_t const STATE_OK = 1;                     /*!< state computation OK */
state_status_t const STATE_INCOMPATIBLE_EDGE = 2;      /*!< state computation failed due to unmatching locations */
state_status_t const STATE_INTVARS_GUARD_VIOLATED = 4; /*!< state computation failed due to intvars guard violation */
state_status_t const STATE_INTVARS_SRC_INVARIANT_VIOLATED =
    8; /*!< state computation failed due to intvars source invariant violation */
state_status_t const STATE_INTVARS_TGT_INVARIANT_VIOLATED =
    16; /*!< state computation failed due to intvars target invariant violation */
state_status_t const STATE_INTVARS_STATEMENT_FAILED = 32; /*!< state computation failed due to intvars edge statement */
state_status_t const STATE_CLOCKS_GUARD_VIOLATED = 64;    /*!< state computation failed due to clocks guard violation */
state_status_t const STATE_CLOCKS_SRC_INVARIANT_VIOLATED =
    128; /*!< state computation failed due to clocks source invariant violation */
state_status_t const STATE_CLOCKS_TGT_INVARIANT_VIOLATED =
    256;                                               /*!< state computation failed due to clocks target invariant violation */
state_status_t const STATE_CLOCKS_EMPTY_SYNC = 512;    /*!< state computation failed due to empty sync zone */
state_status_t const STATE_CLOCKS_EMPTY_SPREAD = 1024; /*!< state computation failed due to empty bounded-spread zone */
state_status_t const STATE_ZONE_EMPTY = 2048;          /*!< state computation result in an empty zone (no details provided) */
state_status_t const STATE_ZONE_EMPTY_SYNC = 4096;     /*!< state computation result in a ref zone that has no sync valuation */

} // end of namespace tchecker

#endif // TCHECKER_BASICTYPES_HH
