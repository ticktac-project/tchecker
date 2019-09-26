/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_OUTPUT_HH
#define TCHECKER_TA_DETAILS_OUTPUT_HH

#include <iostream>
#include <string>

#include "tchecker/basictypes.hh"
#include "tchecker/fsm/details/output.hh"
#include "tchecker/ta/details/state.hh"
#include "tchecker/ta/details/transition.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"

/*!
 \file output.hh
 \brief Outputters for timed automata
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \brief Output clock variables following tchecker syntax
       \tparam SYSTEM : type of system, should inherit from
       tchecker::ta::details::system_t
       \param os : output stream
       \param s : system
       \post s clock variables have been output to os
       */
      template <class SYSTEM>
      void tchecker_output_system_clocks(std::ostream & os, SYSTEM const & s)
      {
        auto const & decl_clocks = s.clocks().index();
        for (auto c : decl_clocks) {
          std::string const & name = decl_clocks.value(c);
          tchecker::clock_id_t id = decl_clocks.key(c);
          tchecker::clock_info_t const & info = s.clocks().info(id);
          os << "clock:" << info.size() << ":" << name << std::endl;
        }
      }
      
      
      /*!
       \brief Output location committed flag following tchecker syntax
       \tparam SYSTEM : type of system, should inherit from
       tchecker::ta::details::system_t
       \param os : output stream
       \param s : system
       \param loc : location
       \post loc committed flag has been output to os
       */
      template <class SYSTEM>
      void tchecker_output_location_committed_flag(std::ostream & os, SYSTEM const & s, typename SYSTEM::loc_t const & loc)
      {
        if (loc.committed())
          os << "committed:";
      }
      
      
      /*!
       \brief Output location urgent flag following tchecker syntax
       \tparam SYSTEM : type of system, should inherit from
       tchecker::ta::details::system_t
       \param os : output stream
       \param s : system
       \param loc : location
       \post loc urgent flag has been output to os
       */
      template <class SYSTEM>
      void tchecker_output_location_urgent_flag(std::ostream & os, SYSTEM const & s, typename SYSTEM::loc_t const & loc)
      {
        if (loc.urgent())
          os << "urgent:";
      }
      
      
      
      
      /*!
       \brief Output state
       \tparam VLOC : type of tuple of locations
       \tparam INTVARS_VAL : type of integer variables valuations
       \tparam VLOC_PTR : type of pointer to tuple of locations
       \tparam INTVARS_VAL_PTR : type of pointer to integer variables valuation
       \param os : output stream
       \param s : state
       \param intvar_index : index of integer variables
       \post see tchecker::fsm::details::output
       \return os after output
       */
      template <class VLOC, class INTVARS_VAL, class VLOC_PTR, class INTVARS_VAL_PTR>
      inline std::ostream & output(std::ostream & os,
                                   tchecker::ta::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s,
                                   tchecker::intvar_index_t const & intvar_index)
      {
        return tchecker::fsm::details::output(os, s, intvar_index);
      }
      
      
      
      
      /*!
       \class state_outputter_t
       \brief Outputter for states
       */
      class state_outputter_t : public tchecker::fsm::details::state_outputter_t {
      public:
        using tchecker::fsm::details::state_outputter_t::state_outputter_t;
        
        /*!
         \brief Output state
         \tparam VLOC : type of tuple of locations
         \tparam INTVARS_VAL : type of integer variables valuations
         \tparam VLOC_PTR : type of pointer to tuple of locations
         \tparam INTVARS_VAL_PTR : type of pointer to integer variables valuation
         \param os : output stream
         \param s : state
         \post see tchecker::ta::details::output
         \return os after output
         */
        template <class VLOC, class INTVARS_VAL, class VLOC_PTR, class INTVARS_VAL_PTR>
        inline std::ostream & output(std::ostream & os,
                                     tchecker::ta::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s)
        {
          return tchecker::ta::details::output(os, s, _intvar_index);
        }
      };
      
      
      
      
      /*!
       \brief Output transition
       \param os : output stream
       \param t : transition
       \param clock_index : a clock index
       \post t has been output to os using clock names from clock_index
       \return os after output
       */
      std::ostream & output(std::ostream & os,
                            tchecker::ta::details::transition_t const & t,
                            tchecker::clock_index_t const & clock_index);
      
      
      
      
      /*!
       \class transition_outputter_t
       \brief Transition outputter
       */
      class transition_outputter_t : public tchecker::fsm::details::transition_outputter_t {
      public:
        /*!
         \brief Constructor
         \param clock_index : clocks index
         \note this keeps a reference on clock_index
         */
        transition_outputter_t(tchecker::clock_index_t const & clock_index);
        
        /*!
         \brief Copy constructor
         */
        transition_outputter_t(tchecker::ta::details::transition_outputter_t const &) = default;
        
        /*!
         \brief Move constructor
         */
        transition_outputter_t(tchecker::ta::details::transition_outputter_t &&) = default;
        
        /*!
         \brief Destructor
         */
        ~transition_outputter_t() = default;
        
        /*!
         \brief Assignment operator (deleted)
         */
        tchecker::ta::details::transition_outputter_t &
        operator= (tchecker::ta::details::transition_outputter_t const &) = delete;
        
        /*!
         \brief Move-assignment operator (deleted)
         */
        tchecker::ta::details::transition_outputter_t & operator= (tchecker::ta::details::transition_outputter_t &&) = delete;
        
        /*!
         \brief Output transition
         \param os : output stream
         \param t : transition
         \post t has been output to os
         \return os after output
         */
        inline std::ostream & output(std::ostream & os, tchecker::ta::details::transition_t const & t)
        {
          return tchecker::ta::details::output(os, t, _clock_index);
        }
      protected:
        tchecker::clock_index_t const & _clock_index;  /*!< Clock index */
      };
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_OUTPUT_HH

