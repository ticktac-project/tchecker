/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/ta/details/vm_variables.hh"

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /* vm_variables_t */
      
      tchecker::integer_variables_t const & vm_variables_t::intvars(tchecker::ta::system_t const & system) const
      {
        return system.intvars();
      }
      
      tchecker::clock_variables_t const & vm_variables_t::clocks(tchecker::ta::system_t const & system) const
      {
        return system.clocks();
      }
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

