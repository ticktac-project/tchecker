/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_DETAILS_TS_HH
#define TCHECKER_ZG_DETAILS_TS_HH

#include "tchecker/ts/ts.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file ts.hh
 \brief Transition system interface to zone graphs
 */

namespace tchecker {
  
  namespace zg {
    
    namespace details {
      
      /*!
       \class ts_t
       \brief Transition system for zone graphs
       \tparam STATE : type of states, should inherit from tchecker::zg::details::state_t
       \tparam TRANSITION : type of transitions, should inherit from tchecker::zg::details::transition_t
       \tparam ZG : type of zone graph, should instantiate tchecker::zg::details::zg_t
       \note STATE::zone_t and ZG::zone_t should coincide but not necessarily be the same. For instance,
       STATE::zone_t can be tchecker::make_shared_t<Z> for some zone implementation Z whereas ZG::zone_t
       is Z itself as STATE::zone_t needs to take into account memory management contingencies.
       Nevertheless, tchecker::make_shared_t<Z> and Z are compatible types for the operations done by
       ZG on the zones in STATE
       */
      template <class STATE, class TRANSITION, class ZG>
      class ts_t : public tchecker::ts::ts_t<
      STATE,
      TRANSITION,
      typename ZG::initial_iterator_t,
      typename ZG::outgoing_edges_iterator_t,
      typename ZG::initial_iterator_value_t,
      typename ZG::outgoing_edges_iterator_value_t>
      {
      public:
        /*!
         \brief Constructor
         \param model : a model to build ZG
         */
        template <class MODEL>
        explicit ts_t(MODEL & model)
        : _zg(model)
        {}
        
        /*!
         \brief Copy constructor
         \param ts : ZG transition system
         \post this is a copy of ts
         */
        ts_t(tchecker::zg::details::ts_t<STATE, TRANSITION, ZG> const & ts) = default;
        
        /*!
         \brief Move constructor
         \param ts : ZG transition system
         \post ts has been moved to this
         */
        ts_t(tchecker::zg::details::ts_t<STATE, TRANSITION, ZG> && ts) = default;
        
        /*!
         \brief Destructor
         */
        virtual ~ts_t() = default;
        
        /*!
         \brief Assignment operator
         \param ts : ZG transition system
         \post this is a copy of zg
         \return this after assignment
         */
        tchecker::zg::details::ts_t<STATE, TRANSITION, ZG> &
        operator= (tchecker::zg::details::ts_t<STATE, TRANSITION, ZG> const & ts) = default;
        
        /*!
         \brief Move-assignment operator
         \param ts : ZG transition system
         \post ts has been moved to this
         \return this after assignment
         */
        tchecker::zg::details::ts_t<STATE, TRANSITION, ZG> &
        operator= (tchecker::zg::details::ts_t<STATE, TRANSITION, ZG> && ts) = default;
        
        /*!
         \brief Accessor (see tchecker::ts::ts_t::initial)
         */
        virtual tchecker::range_t<typename ZG::initial_iterator_t> initial()
        {
          return _zg.initial();
        }
        
        /*!
         \brief Initialize state (see tchecker::ts::ts_t::initialize)
         */
        virtual enum tchecker::state_status_t initialize(STATE & s,
                                                         TRANSITION & t,
                                                         typename ZG::initial_iterator_value_t const & v)
        {
          return _zg.initialize(*s.vloc_ptr(), *s.intvars_valuation_ptr(), *s.zone_ptr(), v,
                                t.src_invariant_container());
        }
        
        /*!
         \brief Accessor (see tchecker::ts::ts_t::outgoing_edges)
         */
        virtual tchecker::range_t<typename ZG::outgoing_edges_iterator_t> outgoing_edges(STATE const & s)
        {
          return _zg.outgoing_edges(s.vloc());
        }
        
        /*!
         \brief Next state computation (see tchecker::ts::ts_t::next)
         */
        virtual enum tchecker::state_status_t next(STATE & s,
                                                   TRANSITION & t,
                                                   typename ZG::outgoing_edges_iterator_value_t const & v)
        {
          return _zg.next(*s.vloc_ptr(), *s.intvars_valuation_ptr(), *s.zone_ptr(), v,
                          t.src_invariant_container(), t.guard_container(), t.reset_container(),
                          t.tgt_invariant_container());
        }
        
        /*!
         \brief Accessor
         \return Underlying zone graph
         */
        inline constexpr ZG const & zg() const
        {
          return _zg;
        }
        
        /*!
         \brief Type of model
         */
        using model_t = typename ZG::model_t;
        
        /*!
         \brief Accessor
         \return Underlying model
         */
        inline constexpr model_t const & model() const
        {
          return _zg.model();
        }
      protected:
        ZG _zg;   /*!< Zone graph */
      };
      
    } // end of namespace details
    
  } // end of namespace zg
  
} // end of namespace tchecker

#endif // TCHECKER_ZG_DETAILS_TS_HH

