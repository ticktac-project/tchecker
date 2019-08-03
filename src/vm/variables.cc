/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/vm/variables.hh"

namespace tchecker {
  
  void vm_variables_t::declare_clock(std::string const & name, tchecker::clock_id_t dim)
  {
    declare<tchecker::clock_variables_t, tchecker::clock_info_t>(name, dim);
  }
  

  void vm_variables_t::declare_bounded_integer(std::string const & name,
                                               tchecker::intvar_id_t dim,
                                               tchecker::integer_t min,
                                               tchecker::integer_t max,
                                               tchecker::integer_t initial)
  {
    declare<tchecker::integer_variables_t, tchecker::intvar_info_t>(name, dim, min, max, initial);
  }

} // end of namespace tchecker
