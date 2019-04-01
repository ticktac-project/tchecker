/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/ta/details/transition.hh"

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /* transition_t */
      
      transition_t::transition_t()
      {}
      
      
      void transition_t::clear()
      {
        _src_invariant.clear();
        _guard.clear();
        _reset.clear();
        _tgt_invariant.clear();
      }
      
      
      tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> transition_t::src_invariant() const
      {
        return tchecker::make_range(_src_invariant.begin(), _src_invariant.end());
      }
      
      
      tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> transition_t::guard() const
      {
        return tchecker::make_range(_guard.begin(), _guard.end());
      }
      
      
      tchecker::range_t<tchecker::clock_reset_container_const_iterator_t> transition_t::reset() const
      {
        return tchecker::make_range(_reset.begin(), _reset.end());
      }
      
      
      tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> transition_t::tgt_invariant() const
      {
        return tchecker::make_range(_tgt_invariant.begin(), _tgt_invariant.end());
      }
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker
