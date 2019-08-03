/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_VARIABLES_HH
#define TCHECKER_TA_DETAILS_VARIABLES_HH

#include "tchecker/fsm/details/variables.hh"
#include "tchecker/vm/variables.hh"

/*!
 \file variables.hh
 \brief Variables for timed automata model
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \class variables_t
       \brief Model variables for timed automata
       */
      class variables_t : public tchecker::fsm::details::variables_t {
      public:
        /*!
         \brief Constructor
         \tparam SYSTEM : type of system, should inherit from tchecker::ta::details::system_t
         \param system : a system
         \post the model variables have been built from system
         */
        template <class SYSTEM>
        variables_t(SYSTEM const & system) : tchecker::fsm::details::variables_t(system)
        {
          tchecker::vm_variables_declare_clocks(system, *this);
        }
      };
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_VARIABLES_HH
