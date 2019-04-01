/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_DETAILS_TRANSITION_HH
#define TCHECKER_FSM_DETAILS_TRANSITION_HH

#include <iostream>

#include "tchecker/ts/ts.hh"

/*!
 \file transition.hh
 \brief Transitions for transition systems over finite-state machines
 */

namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \class transition_t
       \brief Transition in FSM transition system
       \note transitions do not carry any information
       */
      class transition_t : public tchecker::ts::transition_t {
      public:
        /*!
         \brief Clear transition
         \post Does not do anything
         */
        void clear();
      };
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_DETAILS_TRANSITION_HH

