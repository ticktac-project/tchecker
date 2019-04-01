/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_TRANSITION_HH
#define TCHECKER_TA_DETAILS_TRANSITION_HH

#include "tchecker/fsm/details/transition.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file transition.hh
 \brief Transitions for transition systems over timed automata
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \class transition_t
       \brief Transition in TA transition system
       */
      class transition_t : public tchecker::fsm::details::transition_t {
      public:
        /*!
         \brief Constructor
         */
        transition_t();
        
        /*!
         \brief Copy constructor
         */
        transition_t(tchecker::ta::details::transition_t const &) = default;
        
        /*!
         \brief Move constructor
         */
        transition_t(tchecker::ta::details::transition_t &&) = default;
        
        /*!
         \brief Destructor
         */
        ~transition_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::ta::details::transition_t & operator= (tchecker::ta::details::transition_t const &) = default;
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::ta::details::transition_t & operator= (tchecker::ta::details::transition_t &&) = default;
        
        /*!
         \brief Clear transition
         \post The transition source invariant, target invariant, giard and reset are empty
         */
        void clear();
        
        
        // Container accessors
        
        /*!
         \brief Accessor
         \return this transition's source invariant container
         */
        inline tchecker::clock_constraint_container_t & src_invariant_container()
        {
          return _src_invariant;
        }
        
        /*!
         \brief Accessor
         \return this transition's guard container
         */
        inline tchecker::clock_constraint_container_t & guard_container()
        {
          return _guard;
        }
        
        /*!
         \brief Accessor
         \return this transition's reset container
         */
        inline tchecker::clock_reset_container_t & reset_container()
        {
          return _reset;
        }
        
        /*!
         \brief Accessor
         \return this transition's target invariant container
         */
        inline tchecker::clock_constraint_container_t & tgt_invariant_container()
        {
          return _tgt_invariant;
        }
        
        
        // Range accessors
        
        /*!
         \brief Accessor
         \return this transitions's source invariant
         */
        tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> src_invariant() const;
        
        /*!
         \brief Accessor
         \return this transition's guard
         */
        tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> guard() const;
        
        /*!
         \brief Accessor
         \return this transition's reset
         */
        tchecker::range_t<tchecker::clock_reset_container_const_iterator_t> reset() const;
        
        /*!
         \brief Accessor
         \return this transition's target invariant
         */
        tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> tgt_invariant() const;
      protected:
        tchecker::clock_constraint_container_t _src_invariant;              /*!< Source invariant */
        tchecker::clock_constraint_container_t _guard;                      /*!< Guard */
        tchecker::clock_reset_container_t _reset;                           /*!< Reset */
        tchecker::clock_constraint_container_t _tgt_invariant;              /*!< Target invariant */
      };
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_TRANSITION_HH

