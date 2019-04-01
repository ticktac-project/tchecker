/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_DETAILS_VM_VARIABLES_HH
#define TCHECKER_FSM_DETAILS_VM_VARIABLES_HH

#include "tchecker/fsm/system.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"

/*!
 \file vm_variables.hh
 \brief Finite state machine variables accessor for virtual machine
 */

namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \class vm_variables_t
       \brief System variables accessor for VM
       */
      class vm_variables_t {
      public:
        /*!
         \brief Accessor
         \param system : a system
         \return system integer variables
         */
        tchecker::integer_variables_t const & intvars(tchecker::fsm::system_t const & system) const;
        
        /*!
         \brief Accessor
         \param system : a system
         \return Empty set of clock variables (FSM systems have no clock variables)
         */
        tchecker::clock_variables_t const & clocks(tchecker::fsm::system_t const & system) const;
      protected:
        tchecker::clock_variables_t _empty_clocks;  /*!< Empty set of clock variables */
      };
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_DETAILS_VM_VARIABLES_HH
