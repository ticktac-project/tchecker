/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_STATE_HH
#define TCHECKER_TA_DETAILS_STATE_HH

#include "tchecker/fsm/details/state.hh"

/*!
 \file state.hh
 \brief State for transition systems over timed automata
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \class state_t
       \brief TA's state (details)
       \tparam VLOC : type of tuple of locations
       \tparam INTVARS_VAL : type of integer variables valuations
       \tparam VLOC_PTR : type of pointer to tuple of locations
       \tparam INTVARS_VAL_PTR : type of pointer to integer variables valuation
       */
      template <class VLOC, class INTVARS_VAL, class VLOC_PTR=VLOC *, class INTVARS_VAL_PTR=INTVARS_VAL *>
      class state_t : public tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> {
      public:
        using tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR>::state_t;
      };
      
      
      
      
      /*!
       \brief Equality check
       \param s1 : state
       \param s2 : state
       \return true if s1 and s2 have equal tuple of locations, and equal integer variables
       valuation, false otherwise
       */
      template <class VLOC, class INTVARS_VAL, class VLOC_PTR, class INTVARS_VAL_PTR>
      inline bool operator== (tchecker::ta::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s1,
                              tchecker::ta::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s2)
      {
        return fsm::details::operator==(s1, s2);
      }
      
      
      /*!
       \brief Disequality check
       \param s1 : state
       \param s2 : state
       \return false if s1 and s2 have equal tuple of locations, and equal integer variables
       valuation, true otherwise
       */
      template <class VLOC, class INTVARS_VAL, class VLOC_PTR, class INTVARS_VAL_PTR>
      inline bool operator!= (tchecker::ta::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s1,
                              tchecker::ta::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s2)
      {
        return (! (s1 == s2));
      }
      
      
      /*!
       \brief Hash
       \param s : state
       \return Hash value for state s
       */
      template <class VLOC, class INTVARS_VAL, class VLOC_PTR, class INTVARS_VAL_PTR>
      inline std::size_t hash_value(tchecker::ta::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s)
      {
        return tchecker::fsm::details::hash_value(s);
      }
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_STATE_HH

