/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/ta/details/output.hh"
#include "tchecker/variables/clocks.hh"

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /* output */
      
      std::ostream & output(std::ostream & os,
                            tchecker::ta::details::transition_t const & t,
                            tchecker::clock_index_t const & clock_index)
      {
        auto src_invariant_range = t.src_invariant();
        if (! src_invariant_range.empty()) {
          os << "src inv: ";
          tchecker::output_clock_constraints(os, src_invariant_range, clock_index);
        }
        
        auto guard_range = t.guard();
        if (! guard_range.empty()) {
          os << " guard: ";
          tchecker::output_clock_constraints(os, guard_range, clock_index);
        }
        
        auto reset_range = t.reset();
        if (! reset_range.empty()) {
          os << " reset: ";
          tchecker::output_clock_resets(os, reset_range, clock_index);
        }
        
        auto tgt_invariant_range = t.tgt_invariant();
        if (! tgt_invariant_range.empty()) {
          os << " tgt inv: ";
          tchecker::output_clock_constraints(os, tgt_invariant_range, clock_index);
        }
        
        return os;
      }
      
      
      
      
      /* transition_outputter_t */
      
      transition_outputter_t::transition_outputter_t(tchecker::clock_index_t const & clock_index) : _clock_index(clock_index)
      {}
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

