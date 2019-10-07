/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ASYNC_ZG_DETAILS_TS_HH
#define TCHECKER_ASYNC_ZG_DETAILS_TS_HH

#include "tchecker/ts/ts.hh"

/*!
 \file ts.hh
 \brief Transition system interface to asynchronous zone graphs
 */

namespace tchecker {
  
  namespace async_zg {
    
    namespace details {
      
      /*!
       \class ts_t
       \brief Transition system for asynchronous zone graphs
       \tparam STATE : type of states, should inherit from tchecker::async_zg::details::state_t
       \tparam TRANSITION : type of transitions, should inherit from tchecker::async_zg::details::transition_t
       \tparam ASYNC_ZG : type of asynchronous zone graph, should instantiate tchecker::async_zg::details::zg_t
       \note STATE::offset_zone_t and ASYNC_ZG::offset_zone_t should coincide but not necessarily be the same.
       For instance, STATE::offset_zone_t can be tchecker::make_shared_t<Z> for some zone implementation Z
       whereas ASYNC_ZG::offset_zone_t is Z itself as STATE::offset_zone_t needs to take into account memory
       management contingencies.
       Similarly for STATE::sync_zone_t and ASYNC_ZG::sync_zone_t.
       tchecker::make_shared_t<Z> and Z are compatible types for the operations done by
       ASYNC_ZG on the zones in STATE
       */
      template <class STATE, class TRANSITION, class ASYNC_ZG>
      class ts_t : public tchecker::ts::ts_t<
      STATE,
      TRANSITION,
      typename ASYNC_ZG::initial_iterator_t,
      typename ASYNC_ZG::outgoing_edges_iterator_t,
      typename ASYNC_ZG::initial_iterator_value_t,
      typename ASYNC_ZG::outgoing_edges_iterator_value_t>
      {
      public:
        /*!
         \brief Constructor
         \param model : a model to build ASYNC_ZG, should inherit from tchecker::async_zg::details::model_t
         */
        template <class MODEL>
        explicit ts_t(MODEL & model)
        : _async_zg(model)
        {}
        
        /*!
         \brief Copy constructor
         \param ts : a transition system
         \post this is a copy of ts
         */
        ts_t(tchecker::async_zg::details::ts_t<STATE, TRANSITION, ASYNC_ZG> const & ts) = default;
        
        /*!
         \brief Move constructor
         \param ts : a transition system
         \post ts has been moved to this
         */
        ts_t(tchecker::async_zg::details::ts_t<STATE, TRANSITION, ASYNC_ZG> && ts) = default;
        
        /*!
         \brief Destructor
         */
        virtual ~ts_t() = default;
        
        /*!
         \brief Assignment operator
         \param ts : a transition system
         \post this is a copy of ts
         \return this after assignment
         */
        tchecker::async_zg::details::ts_t<STATE, TRANSITION, ASYNC_ZG> &
        operator= (tchecker::async_zg::details::ts_t<STATE, TRANSITION, ASYNC_ZG> const & ts) = default;
        
        /*!
         \brief Move-assignment operator
         \param ts : a transition system
         \post ts has been moved to this
         \return this after assignment
         */
        tchecker::async_zg::details::ts_t<STATE, TRANSITION, ASYNC_ZG> &
        operator= (tchecker::async_zg::details::ts_t<STATE, TRANSITION, ASYNC_ZG> && ts) = default;
        
        /*!
         \brief Accessor (see tchecker::ts::ts_t::initial)
         */
        virtual tchecker::range_t<typename ASYNC_ZG::initial_iterator_t> initial()
        {
          return _async_zg.initial();
        }
        
        /*!
         \brief Initialize state (see tchecker::ts::ts_t::initialize)
         */
        virtual enum tchecker::state_status_t initialize(STATE & s,
                                                         TRANSITION & t,
                                                         typename ASYNC_ZG::initial_iterator_value_t const & v)
        {
          return _async_zg.initialize(*s.vloc_ptr(), *s.intvars_val_ptr(), *s.offset_zone_ptr(), *s.sync_zone_ptr(),
                                      v, t.src_invariant_container());
        }
        
        /*!
         \brief Accessor (see tchecker::ts::ts_t::outgoing_edges)
         */
        virtual tchecker::range_t<typename ASYNC_ZG::outgoing_edges_iterator_t> outgoing_edges(STATE const & s)
        {
          return _async_zg.outgoing_edges(s.vloc());
        }
        
        /*!
         \brief Next state computation (see tchecker::ts::ts_t::next)
         */
        virtual enum tchecker::state_status_t next(STATE & s,
                                                   TRANSITION & t,
                                                   typename ASYNC_ZG::outgoing_edges_iterator_value_t const & v)
        {
          return _async_zg.next(*s.vloc_ptr(), *s.intvars_val_ptr(), *s.offset_zone_ptr(), *s.sync_zone_ptr(), v,
                                t.src_invariant_container(), t.guard_container(), t.reset_container(),
                                t.tgt_invariant_container());
        }
        
        /*!
         \brief Accessor
         \return Underlying asynchronous zone graph
         */
        inline constexpr ASYNC_ZG const & async_zg() const
        {
          return _async_zg;
        }
        
        /*!
         \brief Type of model
         */
        using model_t = typename ASYNC_ZG::model_t;
        
        /*!
         \brief Accessor
         \return Underlying model
         */
        inline constexpr model_t const & model() const
        {
          return _async_zg.model();
        }
      protected:
        ASYNC_ZG _async_zg;   /*!< Asynchronous zone graph */
      };
      
    } // end of namespace details
    
  } // end of namespace async_zg
  
} // end of namespace tchecker

#endif // TCHECKER_ASYNC_ZG_DETAILS_TS_HH




