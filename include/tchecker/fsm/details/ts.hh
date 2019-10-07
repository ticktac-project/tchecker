/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_TS_HH
#define TCHECKER_FSM_TS_HH

#include "tchecker/basictypes.hh"
#include "tchecker/ts/ts.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file ts.hh
 \brief Transition system interface to finite state machine
 */

namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \class ts_t
       \brief Transition system for finite state machines
       \tparam STATE : type of state, should inherit from tchecker::fsm::details::state_t
       \tparam TRANSITION : type of transition, should inherit from tchecker::fsm::details::transition_t
       \tparam FSM : type of finite state machine, should instantiate tchecker::fsm::details::fsm_t
       */
      template <class STATE, class TRANSITION, class FSM>
      class ts_t : public tchecker::ts::ts_t<
      STATE,
      TRANSITION,
      typename FSM::initial_iterator_t,
      typename FSM::outgoing_edges_iterator_t,
      typename FSM::initial_iterator_value_t,
      typename FSM::outgoing_edges_iterator_value_t
      >
      {
      public:
        /*!
         \brief Constructor
         \param fsm_args : arguments to a constructor of FSM
         */
        template <class ... FSM_ARGS>
        explicit ts_t(FSM_ARGS && ... fsm_args)
        : _fsm(std::forward<FSM_ARGS>(fsm_args)...)
        {}
        
        /*!
         \brief Copy constructor
         \param ts : FSM transition system
         \post this is a copy of ts
         */
        ts_t(tchecker::fsm::details::ts_t<STATE, TRANSITION, FSM> const & ts)
        : _fsm(ts._fsm)
        {}
        
        /*!
         \brief Move constructor
         \param ts : FSM transition system
         \post ts has been moved to this
         */
        ts_t(tchecker::fsm::details::ts_t<STATE, TRANSITION, FSM> && ts)
        : _fsm(std::move(ts._fsm))
        {}
        
        /*!
         \brief Destructor
         */
        virtual ~ts_t() = default;
        
        /*!
         \brief Assignment operator (deleted)
         */
        tchecker::fsm::details::ts_t<STATE, TRANSITION, FSM> &
        operator= (tchecker::fsm::details::ts_t<STATE, TRANSITION, FSM> const &) = delete;
        
        /*!
         \brief Move-assignment operator (deleted)
         */
        tchecker::fsm::details::ts_t<STATE, TRANSITION, FSM> &
        operator= (tchecker::fsm::details::ts_t<STATE, TRANSITION, FSM> &&) = delete;
        
        /*!
         \brief Accessor (see tchecker::ts::ts_t::initial)
         */
        virtual tchecker::range_t<typename FSM::initial_iterator_t> initial()
        {
          return _fsm.initial();
        }
        
        /*!
         \brief Initialize state (see tchecker::ts::ts_t::initialize)
         */
        virtual enum tchecker::state_status_t initialize(STATE & s,
                                                         TRANSITION & t,
                                                         typename FSM::initial_iterator_value_t const & v)
        {
          return _fsm.initialize(*s.vloc_ptr(), *s.intvars_val_ptr(), v);
        }
        
        /*!
         \brief Accessor (see tchecker::ts::ts_t::outgoing_edges)
         */
        virtual tchecker::range_t<typename FSM::outgoing_edges_iterator_t> outgoing_edges(STATE const & s)
        {
          return _fsm.outgoing_edges(s.vloc());
        }
        
        /*!
         \brief Next state computation (see tchecker::ts::ts_t::next)
         */
        virtual enum tchecker::state_status_t next(STATE & s,
                                                   TRANSITION & t,
                                                   typename FSM::outgoing_edges_iterator_value_t const & v)
        {
          return _fsm.next(*s.vloc_ptr(), *s.intvars_val_ptr(), v);
        }
        
        /*!
         \brief Accessor
         \return Underlying finite state machine
         */
        inline constexpr FSM const & fsm() const
        {
          return _fsm;
        }
        
        /*!
         \brief Type of model
         */
        using model_t = typename FSM::model_t;
        
        /*!
         \brief Accessor
         \return Underlying model
         */
        inline constexpr model_t const & model() const
        {
          return _fsm.model();
        }
      private:
        FSM _fsm;  /*!< FSM */
      };
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_TS_HH
