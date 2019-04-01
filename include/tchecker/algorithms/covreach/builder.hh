/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COVREACH_BUILDER_HH
#define TCHECKER_ALGORITHMS_COVREACH_BUILDER_HH

#include <tuple>

#include "tchecker/basictypes.hh"
#include "tchecker/ts/builder.hh"

/*!
 \file builder.hh
 \brief Transition system builder for covering reachability algorithm
 */

namespace tchecker {
  
  namespace covreach {
    
    /*!
     \class builder_t
     \brief Transition system builder for covering reachability algorithm
     \tparam TS : type of transition system (see tchecker::ts::builder_ok_t)
     \tparam ALLOCATOR : type of allocator (see tchecker::ts::builder_ok_t)
     */
    template <class TS, class ALLOCATOR>
    class builder_t : public tchecker::ts::builder_ok_t<TS, ALLOCATOR> {
    public:
      /*!
       \brief Type of pointers to state
       */
      using state_ptr_t = typename tchecker::ts::builder_ok_t<TS, ALLOCATOR>::state_ptr_t;
      
      /*!
       \brief Type of pointers to transition
       */
      using transition_ptr_t = typename tchecker::ts::builder_ok_t<TS, ALLOCATOR>::transition_ptr_t;
      
      /*!
       \brief Constructors
       */
      using tchecker::ts::builder_ok_t<TS, ALLOCATOR>::builder_ok_t;
    };
    
  } // end of namespace covreach
  
} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COVREACH_BUILDER_HH
