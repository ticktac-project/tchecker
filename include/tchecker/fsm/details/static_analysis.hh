/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_DETAILS_STATIC_ANALYSIS_HH
#define TCHECKER_FSM_DETAILS_STATIC_ANALYSIS_HH

#include "tchecker/expression/static_analysis.hh"
#include "tchecker/fsm/details/edge.hh"
#include "tchecker/fsm/details/system.hh"
#include "tchecker/system/static_analysis.hh"

/*!
 \file static_analysis.hh
 \brief Static analysis on system of finite state machines with bounded integer variables (details)
 */
namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \brief Checks if a system has non-trivial guard on a weakly synchronized event
       \tparam LOC : type of locations
       \tparam EDGE : type of edges
       \param system : a system of processes
       \return true if system has an edge labeled with an event that is weakly synchronized
       */
      template <class LOC, class EDGE>
      bool has_guarded_weakly_synchronized_event(tchecker::fsm::details::system_t<LOC, EDGE> const & system)
      {
        static_assert(std::is_base_of<tchecker::fsm::details::edge_t<LOC>, EDGE>::value, "");
        
        tchecker::process_events_map_t weakly_sync_map = tchecker::weakly_synchronized_events(system);
        for (EDGE const * edge : system.edges()) {
          if (weakly_sync_map.contains(edge->pid(), edge->event_id())) {
            try {
              tchecker::const_evaluate(edge->guard());
            }
            catch (...) {
              return true;
            }
          }
        }
        return false;
      }
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_DETAILS_STATIC_ANALYSIS_HH

