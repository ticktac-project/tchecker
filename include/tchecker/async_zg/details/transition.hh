/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ASYNC_ZG_DETAILS_TRANSITION_HH
#define TCHECKER_ASYNC_ZG_DETAILS_TRANSITION_HH

#include "tchecker/ta/details/transition.hh"

/*!
 \file transition.hh
 \brief Transition of asynchronous zone graphs (details)
 */

namespace tchecker {
  
  namespace async_zg {
    
    namespace details {
      
      /*!
       \class transition_t
       \brief Transition of asynchronous zone graphs
       */
      class transition_t : public tchecker::ta::details::transition_t {
      public:
        using tchecker::ta::details::transition_t::transition_t;
      };
      
    } // end of namespace details
    
  } // end of namespace async_zg
  
} // end of namespace tchecker

#endif // TCHECKER_ASYNC_ZG_DETAILS_TRANSITION_HH


