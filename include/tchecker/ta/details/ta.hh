/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_TA_HH
#define TCHECKER_TA_DETAILS_TA_HH

#include "tchecker/basictypes.hh"
#include "tchecker/fsm/details/fsm.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file ta.hh
 \brief Timed automaton (details)
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \class ta_t
       \brief Timed automaton details
       \tparam MODEL : type of model, should inherit from tchecker::ta::details::model_t
       \tparam VLOC : type of tuple of locations, locations should inherit from tchecker::ta::loc_t
       \tparam INTVARS_VAL : type of integer variables valuation
       */
      template <class MODEL, class VLOC, class INTVARS_VAL>
      class ta_t : protected tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL> {
      public:
        /*!
         \brief Type of model
         */
        using model_t = typename tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL>::model_t;
        
        /*!
         \brief Type of tuple of locations
         */
        using vloc_t = typename tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL>::vloc_t;
        
        /*!
         \brief Type of valuation of bounded integer variables
         */
        using intvars_valuation_t = typename tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL>::intvars_valuation_t;
        
        /*!
         \brief Constructor
         \param model : a model
         */
        explicit ta_t(MODEL & model)
        : tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL>(model)
        {}
        
        /*!
         \brief Copy constructor
         */
        ta_t(tchecker::ta::details::ta_t<MODEL, VLOC, INTVARS_VAL> const &) = default;
        
        /*!
         \brief Move constructor
         */
        ta_t(tchecker::ta::details::ta_t<MODEL, VLOC, INTVARS_VAL> &&) = default;
        
        /*!
         \brief Destructor
         */
        ~ta_t() = default;
        
        /*!
         \brief Assignment operator (deleted)
         */
        tchecker::ta::details::ta_t<MODEL, VLOC, INTVARS_VAL> &
        operator= (tchecker::ta::details::ta_t<MODEL, VLOC, INTVARS_VAL> const &) = delete;
        
        /*!
         \brief Move-assignment operator (deleted)
         */
        tchecker::ta::details::ta_t<MODEL, VLOC, INTVARS_VAL> &
        operator= (tchecker::ta::details::ta_t<MODEL, VLOC, INTVARS_VAL> &&) = delete;
        
        /*!
         \brief Type of iterator over initial states
         */
        using initial_iterator_t = typename tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL>::initial_iterator_t;
        
        /*!
         \brief Accessor
         \return iterator over initial states
         \note see tchecker::fsm::details::fsm_t::initial
         */
        using tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL>::initial;
        
        /*!
         \brief Dereference type for iterator over initial states
         */
        using initial_iterator_value_t = typename tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL>::initial_iterator_value_t;
        
        /*!
         \brief Initialize state
         \note see tchecker::fsm::details::fsm_t::initialize
         */
        enum tchecker::state_status_t initialize(VLOC & vloc,
                                                 INTVARS_VAL & intvars_val,
                                                 initial_iterator_value_t const & initial_range,
                                                 tchecker::clock_constraint_container_t & invariant)
        {
          return tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL>::initialize(vloc, intvars_val, initial_range, invariant);
        }
        
        /*!
         \brief Type of iterator over outgoing edges
         */
        using outgoing_edges_iterator_t
        = typename tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL>::outgoing_edges_iterator_t;
        
        /*!
         \brief Accessor
         \param vloc : tuple of locations
         \return range of outgoing synchronized and asynchronous edges from vloc
         \note see tchecker::fsm::details::fsm_t::outgoing_edges
         */
        using tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL>::outgoing_edges;
        
        /*!
         \brief Dereference type for iterator over outgoing edges
         */
        using outgoing_edges_iterator_value_t
        = typename tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL>::outgoing_edges_iterator_value_t;
        
        /*!
         \brief Compute next state
         \note see tchecker::fsm::details::fsm_t::next
         */
        enum tchecker::state_status_t next(VLOC & vloc,
                                           INTVARS_VAL & intvars_val,
                                           outgoing_edges_iterator_value_t const & vedge,
                                           tchecker::clock_constraint_container_t & src_invariant,
                                           tchecker::clock_constraint_container_t & guard,
                                           tchecker::clock_reset_container_t & clkreset,
                                           tchecker::clock_constraint_container_t & tgt_invariant)
        {
          return tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL>::next(vloc,
                                                                               intvars_val,
                                                                               vedge,
                                                                               src_invariant,
                                                                               guard,
                                                                               clkreset,
                                                                               tgt_invariant);
        }
        
        /*!
         \brief Accessor
         \return Underlying model
         */
        inline constexpr model_t const & model() const
        {
          return tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL>::model();
        }
      };
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_TA_HH

