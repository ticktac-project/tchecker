/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_TA_HH
#define TCHECKER_ZG_TA_HH

#include "tchecker/parsing/declaration.hh"
#include "tchecker/ta/details/ta.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/ta/ta.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/zg/details/allocators.hh"
#include "tchecker/zg/details/model.hh"
#include "tchecker/zg/details/output.hh"
#include "tchecker/zg/details/state.hh"
#include "tchecker/zg/details/transition.hh"
#include "tchecker/zg/details/ts.hh"
#include "tchecker/zg/details/variables.hh"
#include "tchecker/zg/details/zg.hh"
#include "tchecker/zone/dbm/semantics.hh"

/*!
 \file zg_ta.hh
 \brief Zone graph for timed automata
 */

namespace tchecker {
  
  namespace zg {
    
    namespace ta {
      
      /*!
       \brief Model instantiation
       */
      using model_instantiation_t = tchecker::zg::details::model_t<tchecker::ta::system_t, tchecker::zg::details::variables_t>;
      
      /*!
       \class model_t
       \brief Model for zone graphs over timed automata (see tchecker::zg::details::model_t)
       */
      class model_t final : public tchecker::zg::ta::model_instantiation_t {
      public:
        /*!
         \brief Constructor
         \param system_declaration : system declaration
         \param log : logging facility
         \note see tchecker::fsm::model_t
         */
        model_t(tchecker::parsing::system_declaration_t const & system_declaration, tchecker::log_t & log);
        
        using tchecker::zg::ta::model_instantiation_t::model_t;
      };
      
      
      
      /*!
       \brief Short name for timed automata for zone graphs
       */
      using ta_instantiation_t
      = tchecker::ta::details::ta_t<tchecker::zg::ta::model_t, tchecker::ta::vloc_t, tchecker::ta::intvars_valuation_t>;
      
      
      
      /*!
       \class ta_t
       \brief Timed automaton for zone graph
       */
      class ta_t final : public tchecker::zg::ta::ta_instantiation_t {
      public:
        using tchecker::zg::ta::ta_instantiation_t::ta_t;
      };
      
      
      
      
      /*!
       \brief Zone graph over timed automaton
       \tparam ZONE_SEMANTICS : type of zone semantics
       */
      template <class ZONE_SEMANTICS>
      using zg_t = tchecker::zg::details::zg_t<tchecker::zg::ta::ta_t, ZONE_SEMANTICS>;
      
      
      
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
       \brief Zone graph pool allocated state
       \tparam ZONE : type of zone
       */
      template <class ZONE>
      using state_t = tchecker::zg::details::state_t<
      tchecker::zg::ta::shared_vloc_t,
      tchecker::zg::ta::shared_intvars_valuation_t,
      tchecker::zg::ta::shared_zone_t<ZONE>,
      tchecker::intrusive_shared_ptr_t<tchecker::zg::ta::shared_vloc_t>,
      tchecker::intrusive_shared_ptr_t<tchecker::zg::ta::shared_intvars_valuation_t>,
      tchecker::intrusive_shared_ptr_t<tchecker::zg::ta::shared_zone_t<ZONE>>
      >;
      
      
      /*!
       \brief Zone graph shared state
       \tparam ZONE : type of zone
       */
      template <class ZONE>
      using shared_state_t = tchecker::make_shared_t<tchecker::zg::ta::state_t<ZONE>>;
      
      
      /*!
       \class state_pool_allocalor_t
       \brief Zone graph shared state allocator
       \tparam STATE : type of states, should derive from tchecker::zg::ta::state_t
       */
      template <class STATE>
      class state_pool_allocator_t : public tchecker::zg::details::state_pool_allocator_t<STATE> {
      public:
        /*!
         \brief Constructor
         \param model : a model
         \param alloc_nb : number of objects in an allocation block
         */
        state_pool_allocator_t(tchecker::zg::ta::model_t & model, std::size_t alloc_nb)
        : tchecker::zg::details::state_pool_allocator_t<STATE>
        (alloc_nb,
         alloc_nb, model.system().processes_count(),
         alloc_nb, model.flattened_integer_variables().flattened_size(),
         alloc_nb, model.flattened_clock_variables().flattened_size())
        {}
      };
      
      
      /*!
       \brief Zone graph state outputter
       */
      using state_outputter_t = tchecker::zg::details::state_outputter_t;
      
      
      /*!
       \brief Zone graph transition
       */
      using transition_t = tchecker::zg::details::transition_t;
      
      
      /*!
       \brief Zone graph transition outputter
       */
      using transition_outputter_t = tchecker::zg::details::transition_outputter_t;
      
      
      /*!
       \brief Zone graph transition singleton allocator
       \tparam TRANSITION : type of transition, should derive from tchecker::zg::ta::transiton_t
       */
      template <class TRANSITION>
      class transition_singleton_allocator_t : public tchecker::zg::details::transition_singleton_allocator_t<TRANSITION> {
        static_assert(std::is_base_of<tchecker::zg::ta::transition_t, TRANSITION>::value, "");
      public:
        using tchecker::zg::details::transition_singleton_allocator_t<TRANSITION>::transition_singleton_allocator_t;
      };
      
      
      /*!
       \brief Zone graph transition system
       \tparam ZONE_SEMANTICS : type of zone semantics
       */
      template <class ZONE_SEMANTICS>
      using ts_t
      = tchecker::zg::details::ts_t
      <tchecker::zg::ta::state_t<typename ZONE_SEMANTICS::zone_t>,
      tchecker::zg::ta::transition_t,
      tchecker::zg::ta::zg_t<ZONE_SEMANTICS>>;
      
      
      
      
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
          using zg_t = tchecker::zg::ta::zg_t<ZONE_SEMANTICS>;
          
          /*!
           \brief Type of state
           */
          using state_t = tchecker::zg::ta::state_t<typename ZONE_SEMANTICS::zone_t>;
          
          /*!
           \brief Type of shared state
           */
          using shared_state_t = tchecker::zg::ta::shared_state_t<typename ZONE_SEMANTICS::zone_t>;
          
          /*!
           \brief Type of pointer to shared state
           */
          using shared_state_ptr_t = tchecker::intrusive_shared_ptr_t<shared_state_t>;
          
          /*!
           \brief Type of pool allocator for states
           \tparam STATE : type of states, should derive from state_t
           */
          template <class STATE=shared_state_t>
          using state_pool_allocator_t = tchecker::zg::ta::state_pool_allocator_t<STATE>;
          
          /*!
           \brief Type of transition
           */
          using transition_t = tchecker::zg::ta::transition_t;
          
          /*!
           \brief Type of singleton allocator for transitions
           \tparam TRANSITION : type of transitons, should derive from transition_t
           */
          template <class TRANSITION=transition_t>
          using transition_singleton_allocator_t = tchecker::zg::ta::transition_singleton_allocator_t<TRANSITION>;
          
          /*!
           \brief Type of transition system
           */
          using ts_t = tchecker::zg::ta::ts_t<ZONE_SEMANTICS>;
        };
        
      } // end of namespace details
      
      
      using elapsed_no_extrapolation_t = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::elapsed_no_extrapolation_t>;
      using elapsed_extraLU_global_t = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::elapsed_extraLU_global_t>;
      using elapsed_extraLU_local_t = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::elapsed_extraLU_local_t>;
      using elapsed_extraLUplus_global_t = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::elapsed_extraLUplus_global_t>;
      using elapsed_extraLUplus_local_t = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::elapsed_extraLUplus_local_t>;
      using elapsed_extraM_global_t = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::elapsed_extraM_global_t>;
      using elapsed_extraM_local_t = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::elapsed_extraM_local_t>;
      using elapsed_extraMplus_global_t = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::elapsed_extraMplus_global_t>;
      using elapsed_extraMplus_local_t = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::elapsed_extraMplus_local_t>;
      using non_elapsed_no_extrapolation_t
      = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::non_elapsed_no_extrapolation_t>;
      using non_elapsed_extraLU_global_t = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::non_elapsed_extraLU_global_t>;
      using non_elapsed_extraLU_local_t = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::non_elapsed_extraLU_local_t>;
      using non_elapsed_extraLUplus_global_t
      = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::non_elapsed_extraLUplus_global_t>;
      using non_elapsed_extraLUplus_local_t
      = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::non_elapsed_extraLUplus_local_t>;
      using non_elapsed_extraM_global_t = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::non_elapsed_extraM_global_t>;
      using non_elapsed_extraM_local_t = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::non_elapsed_extraM_local_t>;
      using non_elapsed_extraMplus_global_t
      = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::non_elapsed_extraMplus_global_t>;
      using non_elapsed_extraMplus_local_t
      = tchecker::zg::ta::details::instance_types_t<tchecker::dbm::non_elapsed_extraMplus_local_t>;
      
    } // end of namespace ta
    
  } // end of namespace zg
  
  
  /*!
   \brief Allocation size for states with tchecker::dbm::zone_t zones
   */
  template <class ZONE>
  class allocation_size_t<tchecker::zg::ta::state_t<ZONE>> {
  public:
    static constexpr std::size_t alloc_size()
    {
      return sizeof(tchecker::zg::ta::state_t<ZONE>);
    }
  };
  
} // end of namespace tchecker

#endif // TCHECKER_ZG_TA_HH

