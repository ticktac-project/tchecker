/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ASYNC_ZG_DETAILS_VARIABLES_HH
#define TCHECKER_ASYNC_ZG_DETAILS_VARIABLES_HH

#include "tchecker/clockbounds/variables.hh"

/*!
 \file variables.hh
 \brief Variables for asynchronous zone graph model
 */

namespace tchecker {
  
  namespace async_zg {
    
    namespace details {
      
      /*!
       \class variables_t
       \brief Model variables for asynchronous zone graphs
       */
      class variables_t : public tchecker::clockbounds::variables_t {
      public:
        /*!
         \brief Constructor
         \tparam SYSTEM : type of system, should inherit from tchecker::ta::details::system_t
         \param system : a system
         \post the model variables have been built from system
         */
        template <class SYSTEM>
        variables_t(SYSTEM const & system) : tchecker::clockbounds::variables_t(system)
        {}
      };
      
    } // end of namespace details
    
  } // end of namespace async_zg
  
} // end of namespace tchecker

#endif // TCHECKER_ASYNC_ZG_DETAILS_VARIABLES_HH




