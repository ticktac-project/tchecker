/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_DETAILS_OUTPUT_HH
#define TCHECKER_ZG_DETAILS_OUTPUT_HH

#include <iostream>

#include "tchecker/ta/details/output.hh"
#include "tchecker/variables/intvars.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/zg/details/state.hh"
#include "tchecker/zg/details/transition.hh"

/*!
 \file output.hh
 \brief Outputters for zone graphs
 */

namespace tchecker {
  
  namespace zg {
    
    namespace details {
      
      /*!
       \brief Output state
       \tparam VLOC : type of tuple of locations
       \tparam INTVARS_VAL : type of integer variables valuations
       \tparam ZONE : type of zone
       \tparam VLOC_PTR : type of pointer to tuple of locations
       \tparam INTVARS_VAL_PTR : type of pointer to integer variables valuation
       \tparam ZONE_PTR : type of pointer to zone
       \param os : output stream
       \param s : state
       \param intvar_index : index of integer variables
       \param clock_index : index of clocks
       \post s has been output to os using integer variables names from intvar_index, and clock names from
       clock_index
       \return os after output
       */
      template <class VLOC, class INTVARS_VAL, class ZONE, class VLOC_PTR, class INTVARS_VAL_PTR, class ZONE_PTR>
      inline std::ostream & output
      (std::ostream & os,
       tchecker::zg::details::state_t<VLOC, INTVARS_VAL, ZONE, VLOC_PTR, INTVARS_VAL_PTR, ZONE_PTR> const & s,
       tchecker::intvar_index_t const & intvar_index,
       tchecker::clock_index_t const & clock_index)
      {
        tchecker::ta::details::output(os, s, intvar_index);
        os << " ";
        s.zone().output(os, clock_index);
        return os;
      }
      
      
      
      
      /*!
       \class state_outputter_t
       \brief Outputter for states
       */
      class state_outputter_t : public tchecker::ta::details::state_outputter_t {
      public:
        /*!
         \brief Constructor
         \param intvar_index : index of integer variables
         \param clock_index : index of clocks
         \note this keeps a reference on intvar_index and a reference on clock_index
         */
        state_outputter_t(tchecker::intvar_index_t const & intvar_index, tchecker::clock_index_t const & clock_index)
        : tchecker::ta::details::state_outputter_t(intvar_index), _clock_index(clock_index)
        {}
        
        /*!
         \brief Copy constructor
         */
        state_outputter_t(tchecker::zg::details::state_outputter_t const &) = default;
        
        /*!
         \brief Move constructor
         */
        state_outputter_t(tchecker::zg::details::state_outputter_t &&) = default;
        
        /*!
         \brief Destructor
         */
        ~state_outputter_t() = default;
        
        /*!
         \brief Assignment operator (deleted)
         */
        tchecker::zg::details::state_outputter_t & operator= (tchecker::zg::details::state_outputter_t const &) = delete;
        
        /*!
         \brief Move-assignment operator (deleted)
         */
        tchecker::zg::details::state_outputter_t & operator= (tchecker::zg::details::state_outputter_t &&) = delete;
        
        /*!
         \brief Output state
         \tparam VLOC : type of tuple of locations
         \tparam INTVARS_VAL : type of integer variables valuations
         \tparam ZONE : type of zone
         \tparam VLOC_PTR : type of pointer to tuple of locations
         \tparam INTVARS_VAL_PTR : type of pointer to integer variables valuation
         \tparam ZONE_PTR : type of pointer to zone
         \param os : output stream
         \param s : state
         \post see tchecker::ta::details::output
         \return os after output
         */
        template <class VLOC, class INTVARS_VAL, class ZONE, class VLOC_PTR, class INTVARS_VAL_PTR, class ZONE_PTR>
        inline std::ostream & output
        (std::ostream & os, tchecker::zg::details::state_t<VLOC, INTVARS_VAL, ZONE, VLOC_PTR, INTVARS_VAL_PTR, ZONE_PTR> const & s)
        {
          return tchecker::zg::details::output(os, s, _intvar_index, _clock_index);
        }
      protected:
        tchecker::clock_index_t const & _clock_index;  /*!< Index of clocks */
      };
      
      
      
      
      /*!
       \brief Output transition
       \param os : output stream
       \param t : transition
       \param clock_index : a clock index
       \post t has been output to os using clock names from clock_index
       \return os after output
       */
      inline std::ostream & output(std::ostream & os,
                                   tchecker::zg::details::transition_t const & t,
                                   tchecker::clock_index_t const & clock_index)
      {
        return tchecker::ta::details::output(os, t, clock_index);
      }
      
      
      
      
      /*!
       \class transition_outputter_t
       \brief Transition outputter
       */
      class transition_outputter_t : public tchecker::ta::details::transition_outputter_t {
      public:
        using tchecker::ta::details::transition_outputter_t::transition_outputter_t;
        
        /*!
         \brief Output transition
         \param os : output stream
         \param t : transition
         \post t has been output to os
         \return os after output
         */
        inline std::ostream & output(std::ostream & os, tchecker::zg::details::transition_t const & t)
        {
          return tchecker::zg::details::output(os, t, _clock_index);
        }
      };
      
    } // end of namespace details
    
  } // end of namespace zg
  
} // end of namespace tchecker

#endif // TCHECKER_ZG_DETAILS_OUTPUT_HH

