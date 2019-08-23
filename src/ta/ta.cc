/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/ta/ta.hh"

namespace tchecker {
  
  namespace ta {
    
    /* model_t */
    
    model_t::model_t(tchecker::parsing::system_declaration_t const & system_declaration, tchecker::log_t & log)
    : tchecker::ta::details::model_t<tchecker::ta::system_t, tchecker::ta::variables_t>
    (tchecker::ta::build_system(system_declaration, log), log)
    {}
    
  } // end of namespace ta
  
} // end of namespace tchecker
