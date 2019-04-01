/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_ALLOCATORS_HH
#define TCHECKER_TA_DETAILS_ALLOCATORS_HH

#include "tchecker/fsm/details/allocators.hh"
#include "tchecker/ta/details/transition.hh"

/*!
 \file allocators.hh
 \brief Allocators for transition systems over timed automata
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \class state_pool_allocator_t
       \brief Pool allocator for states of timed automata
       \tparam STATE : type of states, should derive from tchecker::ta::details::state_t
       \tparam VLOC : type of tuple of locations
       \tparam INTVARS_VAL : type of integer variables valuations
       */
      template <class STATE, class VLOC=typename STATE::vloc_t, class INTVARS_VAL=typename STATE::intvars_valuation_t>
      class state_pool_allocator_t : public tchecker::fsm::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL> {
      public:
        using tchecker::fsm::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL>::state_pool_allocator_t;
      };
      
      
      
      
      /*!
       \class transition_singleton_allocator_t
       \brief Singleton allocator for transitions of timed automata
       \tparam TRANSITION : type of transition, should derive from tchecker::ta::details::transition_t
       */
      template <class TRANSITION>
      class transition_singleton_allocator_t : public tchecker::fsm::details::transition_singleton_allocator_t<TRANSITION> {
        static_assert(std::is_base_of<tchecker::ta::details::transition_t, TRANSITION>::value, "");
      public:
        using tchecker::fsm::details::transition_singleton_allocator_t<TRANSITION>::transition_singleton_allocator_t;
      };
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_ALLOCATORS_HH

