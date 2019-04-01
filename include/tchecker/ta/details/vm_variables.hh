/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_VM_VARIABLES_HH
#define TCHECKER_TA_DETAILS_VM_VARIABLES_HH

#include "tchecker/ta/system.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"

/*!
 \file vm_variables.hh
 \brief Timed automaton variables accessor for virtual machine
 */

namespace tchecker {
  
  namespace ta {
    
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
         \return system layout integer variables
         */
        tchecker::integer_variables_t const & intvars(tchecker::ta::system_t const & system) const;
        
        /*!
         \brief Accessor
         \param system : a system
         \return system layout clock variables
         */
        tchecker::clock_variables_t const & clocks(tchecker::ta::system_t const & system) const;
      };
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_VM_VARIABLES_HH

