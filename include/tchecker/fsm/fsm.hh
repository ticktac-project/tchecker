/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_HH
#define TCHECKER_FSM_HH

#include <type_traits>

#include "tchecker/basictypes.hh"
#include "tchecker/fsm/details/allocators.hh"
#include "tchecker/fsm/details/fsm.hh"
#include "tchecker/fsm/details/model.hh"
#include "tchecker/fsm/details/output.hh"
#include "tchecker/fsm/details/state.hh"
#include "tchecker/fsm/details/transition.hh"
#include "tchecker/fsm/details/ts.hh"
#include "tchecker/fsm/details/variables.hh"
#include "tchecker/fsm/system.hh"
#include "tchecker/flat_system/vloc.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"

/*!
 \file fsm.hh
 \brief Finite state machine
 */

namespace tchecker {
  
  namespace fsm {
    
    /*!
     \class variables_t
     \brief Variables for finite state systems with empty set of clocks
     */
    class variables_t : public tchecker::fsm::details::variables_t {
    public:
      using tchecker::fsm::details::variables_t::variables_t;
      
      /*!
       \brief Accessor
       \tparam SYSTEM : type of system, should derive from tchecker::fsm::details::system_t
       \param system : a system
       \return clock variables in system
       */
      template <class SYSTEM>
      inline constexpr tchecker::clock_variables_t const & system_clock_variables(SYSTEM const & system) const
      {
        return _empty_clocks;
      }
      
      /*!
       \brief Accessor
       \tparam SYSTEM : type of system, should derive from tchecker::fsm::details::system_t
       \param system : a system
       \return flattened clock variables in system
       */
      template <class SYSTEM>
      inline constexpr tchecker::flat_clock_variables_t const & flattened_clock_variables(SYSTEM const & system) const
      {
        return _empty_clocks.flattened();
      }
    private:
      tchecker::clock_variables_t _empty_clocks;  /*!< Empty set of clocks */
    };
    
    
    
    
    /*!
     \brief Type of model instantiation
     */
    using model_instantiation_t = tchecker::fsm::details::model_t<tchecker::fsm::system_t, tchecker::fsm::variables_t>;
    
    
    /*!
     \class model_t
     \brief Model for finite-state systems (see tchecker::fsm::details::model_t)
     */
    class model_t final : public tchecker::fsm::model_instantiation_t {
    public:
      /*!
       \brief Constructor
       \param system_declaration : system declaration
       \param log : logging facility
       \post this is a model that consists in an FSM system, a synchronizer and
       bytecode for the locations and edges in the system. All warnings and
       errors have been reported to log
       */
      model_t(tchecker::parsing::system_declaration_t const & system_declaration, tchecker::log_t & log);
      
      using tchecker::fsm::model_instantiation_t::model_t;
    };
    
    
    
    /*!
     \brief Type of tuple of locations for FSM
     */
    using vloc_t = tchecker::vloc_t<tchecker::fsm::loc_t const>;
    
    
    
    /*!
     \brief Type of intgere variables valuation for FSM
     */
    using intvars_valuation_t = tchecker::intvars_valuation_t;
    
    
    
    /*!
     \brief Short name for type of instantiated FSM
     */
    using fsm_instantiation_t =
    tchecker::fsm::details::fsm_t<tchecker::fsm::model_instantiation_t, tchecker::fsm::vloc_t, tchecker::fsm::intvars_valuation_t>;
    
    
    
    
    /*!
     \class fsm_t
     \brief Finite state machine
     */
    class fsm_t final : public tchecker::fsm::fsm_instantiation_t {
    public:
      using tchecker::fsm::fsm_instantiation_t::fsm_t;
      
      /*!
       \brief Compute initial state
       \note see tchecker::fsm::details::fsm_instantiation_t::initialize
       */
      enum tchecker::state_status_t initialize(tchecker::fsm::vloc_t & vloc,
                                               tchecker::fsm::intvars_valuation_t & intvars_val,
                                               initial_iterator_value_t const & initial_range);
      
      /*!
       \brief Compute next state
       \note see tchecker::fsm::details::fsm_instantiation_t::next
       */
      enum tchecker::state_status_t next(tchecker::fsm::vloc_t & vloc,
                                         tchecker::fsm::intvars_valuation_t & intvars_val,
                                         outgoing_edges_iterator_value_t const & vedge);
    };
    
    
    
    // Pool allocated transition system over finite state machines
    
    /*!
     \brief Type of shared tuple of locations
     */
    using shared_vloc_t = tchecker::make_shared_t<tchecker::fsm::vloc_t>;
    
    
    /*!
     \brief Type of shared integer variables valuation
     */
    using shared_intvars_valuation_t = tchecker::make_shared_t<tchecker::fsm::intvars_valuation_t>;
    
    
    /*!
     \brief FSM state
     */
    using state_t = tchecker::fsm::details::state_t<
    tchecker::fsm::shared_vloc_t,
    tchecker::fsm::shared_intvars_valuation_t,
    tchecker::intrusive_shared_ptr_t<tchecker::fsm::shared_vloc_t>,
    tchecker::intrusive_shared_ptr_t<tchecker::fsm::shared_intvars_valuation_t>
    >;
    
    
    /*!
     \brief FSM shared state
     */
    using shared_state_t = tchecker::make_shared_t<tchecker::fsm::state_t>;
    
    
    /*!
     \class state_pool_allocator_t
     \brief FSM shared state allocator
     \tparam STATE : type of states, should derive from tchecker::fsm::state_t
     */
    template <class STATE=tchecker::fsm::shared_state_t>
    class state_pool_allocator_t : public tchecker::fsm::details::state_pool_allocator_t<STATE> {
      static_assert(std::is_base_of<tchecker::fsm::state_t, STATE>::value,
                    "STATE should derive from tchecker::fsm::state_t");
    public:
      /*!
       \brief Constructor
       \param model : a model
       \param alloc_nb : number of objects in an allocation block
       */
      state_pool_allocator_t(tchecker::fsm::model_t & model, std::size_t alloc_nb)
      : tchecker::fsm::details::state_pool_allocator_t<STATE>
      (alloc_nb,
       alloc_nb, model.system().processes_count(),
       alloc_nb, model.flattened_integer_variables().flattened_size())
      {}
    };
    
    
    /*!
     \brief FSM state outputter
     */
    using state_outputter_t = tchecker::fsm::details::state_outputter_t;
    
    
    /*!
     \brief FSM transition
     */
    using transition_t = tchecker::fsm::details::transition_t;
    
    
    /*!
     \brief FSM transition outputter
     */
    using transition_outputter_t = tchecker::fsm::details::transition_outputter_t;
    
    
    /*!
     \brief FSM transition singleton allocator
     \tparam TRANSITION : type of transition, should derive from tchecker::fsm::transition_t
     */
    template <class TRANSITION=tchecker::fsm::transition_t>
    using transition_singleton_allocator_t = tchecker::fsm::details::transition_singleton_allocator_t<TRANSITION>;
    
    
    /*!
     \class ts_t
     \brief FSM transition system
     */
    class ts_t final
    : public tchecker::fsm::details::ts_t<tchecker::fsm::state_t, tchecker::fsm::transition_t, tchecker::fsm::fsm_t> {
    public:
      using tchecker::fsm::details::ts_t<tchecker::fsm::state_t, tchecker::fsm::transition_t, tchecker::fsm::fsm_t>::ts_t;
    };
    
  } // end of namespace fsm
  
  
  
  
  /*!
   \brief Allocation size for tchecker::fsm::state_t
   */
  template <>
  class allocation_size_t<tchecker::fsm::state_t> {
  public:
    static constexpr std::size_t alloc_size()
    {
      return sizeof(tchecker::fsm::state_t);
    }
  };
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_HH
