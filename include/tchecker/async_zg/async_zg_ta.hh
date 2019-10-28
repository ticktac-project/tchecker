/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ASYNC_ZG_TA_HH
#define TCHECKER_ASYNC_ZG_TA_HH

#include "tchecker/parsing/declaration.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/ta/ta.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/async_zg/details/allocators.hh"
#include "tchecker/async_zg/details/model.hh"
#include "tchecker/async_zg/details/output.hh"
#include "tchecker/async_zg/details/state.hh"
#include "tchecker/async_zg/details/transition.hh"
#include "tchecker/async_zg/details/ts.hh"
#include "tchecker/async_zg/details/variables.hh"
#include "tchecker/async_zg/details/zg.hh"
#include "tchecker/zone/offset_dbm/semantics.hh"

/*!
 \file zg_ta.hh
 \brief Asynchronous zone graph for timed automata
 */

namespace tchecker {
  
  namespace async_zg {
    
    namespace ta {
      
      /*!
       \brief Model instantiation
       */
      using model_instantiation_t
      = tchecker::async_zg::details::model_t<tchecker::ta::system_t, tchecker::async_zg::details::variables_t>;
      
      /*!
       \class model_t
       \brief Model for asynchronous zone graphs over timed automata (see tchecker::async_zg::details::model_t)
       */
      class model_t final : public tchecker::async_zg::ta::model_instantiation_t {
      public:
        /*!
         \brief Constructor
         \param system_declaration : system declaration
         \param log : logging facility
         \note see tchecker::fsm::model_t
         */
        model_t(tchecker::parsing::system_declaration_t const & system_declaration, tchecker::log_t & log);
        
        using tchecker::async_zg::ta::model_instantiation_t::model_t;
      };
      
      
      
      
      /*!
       \brief Short name for timed automata for asynchronous zone graphs
       */
      using ta_instantiation_t
      = tchecker::ta::details::ta_t<tchecker::async_zg::ta::model_t, tchecker::ta::vloc_t, tchecker::ta::intvars_valuation_t>;
      
      
      
      /*!
       \class ta_t
       \brief Timed automaton for zone graph
       */
      class ta_t final : public tchecker::async_zg::ta::ta_instantiation_t {
      public:
        using tchecker::async_zg::ta::ta_instantiation_t::ta_t;
      };
      
      
      
      
      /*!
       \brief Asynchronous zone graph over timed automaton
       \tparam ZONE_SEMANTICS : type of zone semantics
       */
      template <class ZONE_SEMANTICS>
      using zg_t = tchecker::async_zg::details::zg_t<tchecker::async_zg::ta::ta_t, ZONE_SEMANTICS>;
      
      
      
      // Pool allocated transition system for zone graph over timed automaton
      
      /*!
       \brief Type of tuple of locations
       */
      using shared_vloc_t = tchecker::ta::shared_vloc_t;
      
      
      /*!
       \brief Type of integer variables valuation
       */
      using shared_intvars_valuation_t = tchecker::ta::shared_intvars_valuation_t;
      
      
      /*!
       \brief Type of pointer on zones
       \tparam ZONE : type of zone
       */
      template <class ZONE>
      using shared_zone_t = tchecker::make_shared_t<ZONE>;
      
      
      /*!
       \brief State of asynchronous zone graph over timed automaton
       \tparam OFFSET_ZONE : type of offset zone
       \tparam SYNC_ZONE : type of synchronized zone
       */
      template <class OFFSET_ZONE, class SYNC_ZONE>
      using state_t = tchecker::async_zg::details::state_t<
      tchecker::async_zg::ta::shared_vloc_t,
      tchecker::async_zg::ta::shared_intvars_valuation_t,
      tchecker::async_zg::ta::shared_zone_t<OFFSET_ZONE>,
      tchecker::async_zg::ta::shared_zone_t<SYNC_ZONE>,
      tchecker::intrusive_shared_ptr_t<tchecker::async_zg::ta::shared_vloc_t>,
      tchecker::intrusive_shared_ptr_t<tchecker::async_zg::ta::shared_intvars_valuation_t>,
      tchecker::intrusive_shared_ptr_t<tchecker::async_zg::ta::shared_zone_t<OFFSET_ZONE>>,
      tchecker::intrusive_shared_ptr_t<tchecker::async_zg::ta::shared_zone_t<SYNC_ZONE>>
      >;
      
      
      /*!
       \brief Asynchronous one graph shared state
       \tparam OFFSET_ZONE : type of offset zone
       \tparam SYNC_ZONE : type of synchronized zone
       */
      template <class OFFSET_ZONE, class SYNC_ZONE>
      using shared_state_t = tchecker::make_shared_t<tchecker::async_zg::ta::state_t<OFFSET_ZONE, SYNC_ZONE>>;
      
      
      /*!
       \class state_pool_allocalor_t
       \brief Asynchrnonous zone graph shared state allocator
       \tparam STATE : type of states, should derive from tchecker::async_zg::ta::state_t
       */
      template <class STATE>
      class state_pool_allocator_t : public tchecker::async_zg::details::state_pool_allocator_t<STATE> {
      public:
        /*!
         \brief Constructor
         \param model : a model
         \param alloc_nb : number of objects in an allocation block
         */
        state_pool_allocator_t(tchecker::async_zg::ta::model_t & model, std::size_t alloc_nb)
        : tchecker::async_zg::details::state_pool_allocator_t<STATE>
        (alloc_nb,
         alloc_nb, model.system().processes_count(),
         alloc_nb, model.flattened_integer_variables().flattened_size(),
         alloc_nb, model.flattened_offset_clock_variables().flattened_size(), model.flattened_clock_variables().flattened_size())
        {}
      };
      
      /*!
       \brief Asynchronous zone graph state outputter
       */
      using state_outputter_t = tchecker::async_zg::details::state_outputter_t;
      
      
      /*!
       \brief Asynchronous zone graph transition
       */
      using transition_t = tchecker::async_zg::details::transition_t;
      
      
      /*!
       \brief Asynchronous zone graph transition outputter
       */
      using transition_outputter_t = tchecker::async_zg::details::transition_outputter_t;
      
      
      /*!
       \brief Asynchronous zone graph transition singleton allocator
       \tparam TRANSITION : type of transition, should derive from tchecker::async_zg::ta::transiton_t
       */
      template <class TRANSITION>
      class transition_singleton_allocator_t : public tchecker::async_zg::details::transition_singleton_allocator_t<TRANSITION> {
        static_assert(std::is_base_of<tchecker::async_zg::ta::transition_t, TRANSITION>::value, "");
      public:
        using tchecker::async_zg::details::transition_singleton_allocator_t<TRANSITION>::transition_singleton_allocator_t;
      };
      
      
      /*!
       \brief Asynchronous zone graph transition system
       \tparam ZONE_SEMANTICS : type of zone semantics
       */
      template <class ZONE_SEMANTICS>
      using ts_t
      = tchecker::async_zg::details::ts_t
      <tchecker::async_zg::ta::state_t<typename ZONE_SEMANTICS::offset_zone_t, typename ZONE_SEMANTICS::sync_zone_t>,
      tchecker::async_zg::ta::transition_t,
      tchecker::async_zg::ta::zg_t<ZONE_SEMANTICS>>;
      
      
      
      
      // Instances
      
      namespace details {
        
        /*!
         \class instance_types_t
         \brief Final types for given zone semantics
         \tparam ZONE_SEMANTICS : zone semantics
         */
        template <class ZONE_SEMANTICS>
        class instance_types_t {
        public:
          /*!
           \brief Type of zone graph
           */
          using zg_t = tchecker::async_zg::ta::zg_t<ZONE_SEMANTICS>;
          
          /*!
           \brief Type of state
           */
          using state_t
          = tchecker::async_zg::ta::state_t<typename ZONE_SEMANTICS::offset_zone_t, typename ZONE_SEMANTICS::sync_zone_t>;
          
          /*!
           \brief Type of shared state
           */
          using shared_state_t
          = tchecker::async_zg::ta::shared_state_t<typename ZONE_SEMANTICS::offset_zone_t, typename ZONE_SEMANTICS::sync_zone_t>;
          
          /*!
           \brief Type of pointer to shared state
           */
          using shared_state_ptr_t = tchecker::intrusive_shared_ptr_t<shared_state_t>;
          
          /*!
           \brief Type of pool allocator for states
           \tparam STATE : type of states, should derive from state_t
           */
          template <class STATE=shared_state_t>
          using state_pool_allocator_t = tchecker::async_zg::ta::state_pool_allocator_t<STATE>;
          
          /*!
           \brief Type of transition
           */
          using transition_t = tchecker::async_zg::ta::transition_t;
          
          /*!
           \brief Type of singleton allocator for transitions
           \tparam TRANSITION : type of transitons, should derive from transition_t
           */
          template <class TRANSITION=transition_t>
          using transition_singleton_allocator_t = tchecker::async_zg::ta::transition_singleton_allocator_t<TRANSITION>;
          
          /*!
           \brief Type of transition system
           */
          using ts_t = tchecker::async_zg::ta::ts_t<ZONE_SEMANTICS>;
        };
        
      } // end of namespace details
      
      
      using elapsed_extraLUplus_local_t
      = tchecker::async_zg::ta::details::instance_types_t<tchecker::offset_dbm::elapsed_extraLUplus_local_t>;
      using non_elapsed_extraLUplus_local_t
      = tchecker::async_zg::ta::details::instance_types_t<tchecker::offset_dbm::non_elapsed_extraLUplus_local_t>;
      
    } // end of namespace ta
    
  } // end of namespace async_zg
  
  
  /*!
   \brief Allocation size for states
   */
  template <class OFFSET_ZONE, class SYNC_ZONE>
  class allocation_size_t<tchecker::async_zg::ta::state_t<OFFSET_ZONE, SYNC_ZONE>> {
  public:
    static constexpr std::size_t alloc_size()
    {
      return sizeof(tchecker::async_zg::ta::state_t<OFFSET_ZONE, SYNC_ZONE>);
    }
  };
  
} // end of namespace tchecker

#endif // TCHECKER_ASYNC_ZG_TA_HH


