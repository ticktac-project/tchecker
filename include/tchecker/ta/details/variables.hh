/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_VARIABLES_HH
#define TCHECKER_TA_DETAILS_VARIABLES_HH

#include "tchecker/fsm/details/variables.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file variables.hh
 \brief Variables for timed automata model
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \class variables_t
       \brief Variables proxy for timed automata
       */
      class variables_t : public tchecker::fsm::details::variables_t {
      public:
        /*!
         \brief Accessor
         \tparam SYSTEM : type of system, should derive from tchecker::ta::details::system_t
         \param system : a system
         \return clock variables in system
         */
        template <class SYSTEM>
        inline constexpr tchecker::clock_variables_t const & system_clock_variables(SYSTEM const & system) const
        {
          return system.clocks();
        }
        
        /*!
         \brief Accessor
         \tparam SYSTEM : type of system, should derive from tchecker::ta::details::system_t
         \param system : a system
         \return flattened clock variables in system
         */
        template <class SYSTEM>
        inline constexpr tchecker::flat_clock_variables_t const & flattened_clock_variables(SYSTEM const & system) const
        {
          return system.clocks().flattened();
        }
      };
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_VARIABLES_HH
