/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_TS_HH
#define TCHECKER_TA_TS_HH

#include "tchecker/ts/ts.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file ts.hh
 \brief Transition system interface to timed automaton
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \class ts_t
       \brief Transition system for timed automata
       \tparam STATE : type of states, should inherit from tchecker::ta::details::state_t
       \tparam TRANSITION : type of transitions, should inherit from tchecker::ta::details::transition_t
       \tparam TA : type of timed automaton, should instantiate tchecker::ta::details::ta_t
       */
      template <class STATE, class TRANSITION, class TA>
      class ts_t : public tchecker::ts::ts_t<
      STATE,
      TRANSITION,
      typename TA::initial_iterator_t,
      typename TA::outgoing_edges_iterator_t,
      typename TA::initial_iterator_value_t,
      typename TA::outgoing_edges_iterator_value_t>
      {
      public:
        /*!
         \brief Constructor
         \param ta_args : arguments to a constructor of TA
         */
        template <class ... TA_ARGS>
        explicit ts_t(TA_ARGS && ... ta_args)
        : _ta(std::forward<TA_ARGS>(ta_args)...)
        {}
        
        /*!
         \brief Copy constructor
         \param ts : TA transition system
         \post this is a copy of ts
         */
        ts_t(tchecker::ta::details::ts_t<STATE, TRANSITION, TA> const & ts)
        : _ta(ts._ta)
        {}
        
        /*!
         \brief Move constructor
         \param ts : TA transition system
         \post ts has been moved to this
         */
        ts_t(tchecker::ta::details::ts_t<STATE, TRANSITION, TA> && ts)
        : _ta(std::move(ts._ta))
        {}
        
        /*!
         \brief Destructor
         */
        virtual ~ts_t() = default;
        
        /*!
         \brief Assignment operator (deleted)
         */
        tchecker::ta::details::ts_t<STATE, TRANSITION, TA> &
        operator= (tchecker::ta::details::ts_t<STATE, TRANSITION, TA> const &) = delete;
        
        /*!
         \brief Move-assignment operator (deleted)
         */
        tchecker::ta::details::ts_t<STATE, TRANSITION, TA> &
        operator= (tchecker::ta::details::ts_t<STATE, TRANSITION, TA> &&) = delete;
        
        /*!
         \brief Accessor (see tchecker::ts::ts_t::initial)
         */
        virtual tchecker::range_t<typename TA::initial_iterator_t> initial()
        {
          return _ta.initial();
        }
        
        /*!
         \brief Initialize state (see tchecker::ts::ts_t::initialize)
         */
        virtual enum tchecker::state_status_t initialize(STATE & s,
                                                         TRANSITION & t,
                                                         typename TA::initial_iterator_value_t const & v)
        {
          return _ta.initialize(*s.vloc_ptr(), *s.intvars_val_ptr(), v, t.src_invariant_container());
        }
        
        /*!
         \brief Accessor (see tchecker::ts::ts_t::outgoing_edges)
         */
        virtual tchecker::range_t<typename TA::outgoing_edges_iterator_t> outgoing_edges(STATE const & s)
        {
          return _ta.outgoing_edges(s.vloc());
        }
        
        /*!
         \brief Next state computation (see tchecker::ts::ts_t::next)
         */
        virtual enum tchecker::state_status_t next(STATE & s,
                                                   TRANSITION & t,
                                                   typename TA::outgoing_edges_iterator_value_t const & v)
        {
          return _ta.next(*s.vloc_ptr(), *s.intvars_val_ptr(), v, t.src_invariant_container(), t.guard_container(),
                          t.reset_container(), t.tgt_invariant_container());
        }
        
        /*!
         \brief Accessor
         \return Underlying timed automaton
         */
        inline constexpr TA const & ta() const
        {
          return _ta;
        }
        
        /*!
         \brief Type of model
         */
        using model_t = typename TA::model_t;
        
        /*!
         \brief Accessor
         \return Underlying model
         */
        inline constexpr model_t const & model() const
        {
          return _ta.model();
        }
      protected:
        TA _ta;  /*!< Timed automaton */
      };
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_TS_HH
