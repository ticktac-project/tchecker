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
#if (INTEGER_T_SIZE==64)
using integer_t = int64_t;
const integer_t int_maxval = INT64_MAX;
const integer_t int_minval = INT64_MIN;
#elif (INTEGER_T_SIZE==32)
using integer_t = int32_t;
const integer_t int_maxval = INT32_MAX;
const integer_t int_minval = INT32_MIN;
#elif (INTEGER_T_SIZE==16)
using integer_t = int16_t;
const integer_t int_maxval = INT16_MAX;
const integer_t int_minval = INT16_MIN;
#else
#error Only 64/32/16 int bit supported
#endif

  
  /*!
   \brief Type of event identifiers
   */
  using event_id_t = uint32_t;
  
  /*!
   \brief Type of process identifiers
   */
  using process_id_t = uint32_t;
  
  /*!
   \brief Type of variable indentifiers
   */
  using variable_id_t = uint32_t;
  
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
   \brief Definition of static variable zero clock name
   */
  std::string const zero_clock_name("0");
  
  /*!
   \brief Definition of static variable zero clock ID
   */
  tchecker::clock_id_t const zero_clock_id(0);
  
  
  /*!
   \brief Type of label identifiers
   */
  using label_id_t = uint32_t;
  
  /*!
   \brief Type of location identifiers
   */
  using loc_id_t = uint32_t;
  
  /*!
   \brief Type of edge identifiers
   */
  using edge_id_t = uint32_t;
  
  /*!
   \brief Type of synchronization identifiers
   */
  using sync_id_t = uint32_t;
  
  /*!
   \brief Type of node identifiers
   */
  using node_id_t = uint32_t;
  
  /*!
   \brief Strength of synchronization constraint
   */
  enum sync_strength_t {
    SYNC_WEAK,     /*!< Broadcast constraint */
    SYNC_STRONG    /*!< Handshaking constraint */
  };
  
  /*!
   \brief Output operator
   \param os : output stream
   \param s : synchronization strength
   \return os after s has been output
   */
  std::ostream & operator<< (std::ostream & os, enum tchecker::sync_strength_t s);
  
  /*!
   \brief Status of states
   */
  enum state_status_t {
    STATE_OK,                               /*!< state computation OK */
    STATE_INCOMPATIBLE_EDGE,                /*!< state computation failed due to unmatching locations */
    STATE_INTVARS_GUARD_VIOLATED,           /*!< state computation failed due to intvars guard violation */
    STATE_INTVARS_SRC_INVARIANT_VIOLATED,   /*!< state computation failed due to intvars source invariant violation */
    STATE_INTVARS_TGT_INVARIANT_VIOLATED,   /*!< state computation failed due to intvars target invariant violation */
    STATE_INTVARS_STATEMENT_FAILED,         /*!< state computation failed due to intvars edge statement */
    STATE_CLOCKS_GUARD_VIOLATED,            /*!< state computation failed due to clocks guard violation */
    STATE_CLOCKS_SRC_INVARIANT_VIOLATED,    /*!< state computation failed due to clocks source invariant violation */
    STATE_CLOCKS_TGT_INVARIANT_VIOLATED,    /*!< state computation failed due to clocks target invariant violation */
    STATE_EMPTY_ZONE,                       /*!< state computation resulted in an empty zone (no details provided) */
  };
  
} // end of namespace tchecker

#endif // TCHECKER_BASICTYPES_HH
