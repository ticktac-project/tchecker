/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_STATE_HH
#define TCHECKER_TS_STATE_HH

#include <cstdlib>

/*!
 \file state.hh
 \brief Basic states for transition systems
 */

namespace tchecker {
  
  namespace ts {
    
    /*!
     \class state_t
     \brief Empty state for transition systems
     */
    class state_t {
    };
    
    
    
    
    /*!
     \brief Equality check
     \param s1 : state
     \param s2 : state
     \return true
     */
    inline bool operator== (tchecker::ts::state_t const & s1, tchecker::ts::state_t const & s2)
    {
      return true;
    }
    
    
    /*!
     \brief Disequality check
     \param s1 : state
     \param s2 : state
     \return false
     */
    inline bool operator!= (tchecker::ts::state_t const & s1, tchecker::ts::state_t const & s2)
    {
      return !(s1 == s2);
    }
    
    
    /*!
     \brief Hash
     \param s : state
     \return hash value for state s
     */
    inline std::size_t hash_value(tchecker::ts::state_t const & s)
    {
      return 0;
    }
    
    
    /*!
     \brief Lexical ordering on transition system states
     \param s1 : first state
     \param s2 : second state
     \return 0 (all states are equal)
     */
    constexpr inline int lexical_cmp(tchecker::ts::state_t const & s1, tchecker::ts::state_t const & s2)
    {
      return 0;
    }
    
  } // end of namespace ts
  
} // end of namespace tchecker

#endif // TCHECKER_TS_STATE_HH
