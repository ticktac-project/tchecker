/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_DETAILS_VARIABLES_HH
#define TCHECKER_FSM_DETAILS_VARIABLES_HH

#include "tchecker/vm/variables.hh"

/*!
 \file variables.hh
 \brief Variables for finite state machine model
 */

namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \class variables_t
       \brief Model variables for FSM
       */
      class variables_t : public tchecker::vm_variables_t {
      public:
        /*!
         \brief Constructor
         \param SYSTEM : type of system, should inherit from tchecker::fsm::details::system_t
         \param system : a system
         \post the model variables have been built from system
         */
        template <class SYSTEM>
        variables_t(SYSTEM const & system)
        {
          tchecker::vm_variables_declare_bounded_integers(system, *this);
        }
      };
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_DETAILS_VARIABLES_HH
