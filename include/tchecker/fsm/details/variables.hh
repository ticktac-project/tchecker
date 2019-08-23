/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_DETAILS_VARIABLES_HH
#define TCHECKER_FSM_DETAILS_VARIABLES_HH

#include "tchecker/variables/intvars.hh"

/*!
 \file variables.hh
 \brief Variables for finite state machine model
 */

namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \class variables_t
       \brief Variables proxy for finite state machines
       */
      class variables_t {
      public:
        /*!
         \brief Accessor
         \tparam SYSTEM : type of system, should derive from tchecker::fsm::details::system_t
         \param system : a system
         \return bounded integer variables in system
         */
        template <class SYSTEM>
        inline constexpr tchecker::integer_variables_t const & system_integer_variables(SYSTEM const & system) const
        {
          return system.intvars();
        }
        
        /*!
         \brief Accessor
         \tparam SYSTEM : type of system, should derive from tchecker::fsm::details::system_t
         \param system : a system
         \return flattened bounded integer variables in system
         */
        template <class SYSTEM>
        inline constexpr tchecker::flat_integer_variables_t const & flattened_integer_variables(SYSTEM const & system) const
        {
          return system.intvars().flattened();
        }
      };
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_DETAILS_VARIABLES_HH
