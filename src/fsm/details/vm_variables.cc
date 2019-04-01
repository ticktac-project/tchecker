/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/fsm/details/vm_variables.hh"

namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /* vm_variables_t */
      
      tchecker::integer_variables_t const & vm_variables_t::intvars(tchecker::fsm::system_t const & system) const
      {
        return system.intvars();
      }
      
      tchecker::clock_variables_t const & vm_variables_t::clocks(tchecker::fsm::system_t const & system) const
      {
        return _empty_clocks;
      }
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker
