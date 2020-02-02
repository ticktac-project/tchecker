/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_HH
#define TCHECKER_TA_HH

#include <cassert>

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

#include "tchecker/flat_system/vloc.hh"
#include "tchecker/fsm/fsm.hh"
#include "tchecker/ta/details/allocators.hh"
#include "tchecker/ta/details/model.hh"
#include "tchecker/ta/details/output.hh"
#include "tchecker/ta/details/state.hh"
#include "tchecker/ta/details/ta.hh"
#include "tchecker/ta/details/ts.hh"
#include "tchecker/ta/details/transition.hh"
#include "tchecker/ta/details/variables.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/variables/intvars.hh"

/*!
 \file ta.hh
 \brief Timed automaton
 */

namespace tchecker {
  
  namespace ta {
    
    /*!
     \class variables_t
     \brief Type of model variables
     */
    class variables_t : public tchecker::ta::details::variables_t {
    public:
      using tchecker::ta::details::variables_t::variables_t;
    };
    
    
    
    /*!
     \brief Type of model instantiated with timed automata system and variables
     */
    using model_instantiation_t = tchecker::ta::details::model_t<tchecker::ta::system_t, tchecker::ta::variables_t>;
    
    /*!
     \class model_t
     \brief Model for timed automata (see tchecker::ta::details::model_t)
     */
    class model_t final : public tchecker::ta::model_instantiation_t {
    public:
      /*!
       \brief Constructor
       \param system_declaration : system declaration
       \param log : logging facility
       \note see tchecker::fsm::model_t
       */
      model_t(tchecker::parsing::system_declaration_t const & system_declaration, tchecker::log_t & log);
      
      using tchecker::ta::model_instantiation_t::model_t;
    };
    
    
    
    /*!
     \brief Type of tuple of locations
     */
    using vloc_t = tchecker::vloc_t<tchecker::ta::loc_t const>;
    
    
    /*!
     \brief Checks if time can elapse in a tuple of locations
     \tparam LOC : type of locations, should derive from tchecker::ta::details::loc_t
     \param vloc : tuple of locations
     \return true if time delay is allowed in vloc, false otherwise
     */
    template <class LOC>
    bool delay_allowed(tchecker::vloc_t<LOC> const & vloc)
    {
      for (LOC const * loc : vloc)
        if (! loc->delay_allowed())
          return false;
      return true;
    }
    
    
    /*!
     \brief Checks if time can elapse in a tuple of locations
     \tparam LOC : type of locations, should derive from tchecker::ta::details::loc_t
     \param vloc : tuple of locations
     \param allowed : bit vector
     \pre allowed and vloc have same size (checked by assertion)
     \post allowed[i] indicates whether process i can delay (value 1) or not (value 0)
     */
    template <class LOC>
    void delay_allowed(tchecker::vloc_t<LOC> const & vloc, boost::dynamic_bitset<> & allowed)
    {
      assert(vloc.size() == allowed.size());
      std::size_t size = vloc.size();
      for (std::size_t i = 0; i < size; ++i)
        allowed[i] = vloc[i]->delay_allowed();
    }
    
    
    /*!
     \brief Type of integer variables valuation
     */
    using intvars_valuation_t = tchecker::intvars_valuation_t;
    
    
    
    
    /*!
     \brief Short-name type for instantiated timed automata
     */
    using ta_instantiation_t
    = tchecker::ta::details::ta_t<tchecker::ta::model_instantiation_t, tchecker::ta::vloc_t, tchecker::ta::intvars_valuation_t>;
    
    
    
    
    /*!
     \class ta_t
     \brief Timed automaton
     */
    class ta_t final : public tchecker::ta::ta_instantiation_t {
    public:
      using tchecker::ta::ta_instantiation_t::ta_t;
    };
    
    
    
    
    // Pool allocated transition system
    
    /*!
     \brief Type of shared tuple of locations
     */
    using shared_vloc_t = tchecker::make_shared_t<tchecker::ta::vloc_t>;
    
    
    /*!
     \brief Type of shared integer variables valuation
     */
    using shared_intvars_valuation_t = tchecker::make_shared_t<tchecker::ta::intvars_valuation_t>;
    
    
    /*!
     \brief TA state
     */
    using state_t = tchecker::ta::details::state_t<
    tchecker::ta::shared_vloc_t,
    tchecker::ta::shared_intvars_valuation_t,
    tchecker::intrusive_shared_ptr_t<tchecker::ta::shared_vloc_t>,
    tchecker::intrusive_shared_ptr_t<tchecker::ta::shared_intvars_valuation_t>
    >;
    
    
    /*!
     \brief TA shared state
     */
    using shared_state_t = tchecker::make_shared_t<tchecker::ta::state_t>;
    
    
    /*!
     \class state_pool_allocalor_t
     \brief TA shared state allocator
     \tparam STATE : type of states, should derive from tchecker::ta::state_t
     */
    template <class STATE=tchecker::ta::shared_state_t>
    class state_pool_allocator_t : public tchecker::ta::details::state_pool_allocator_t<STATE> {
      static_assert(std::is_base_of<tchecker::ta::state_t, STATE>::value,
                    "STATE should derive from tchecker::ta::state_t");
    public:
      /*!
       \brief Constructor
       \param model : a model
       \param alloc_nb : number of objects in an allocation block
       */
      state_pool_allocator_t(tchecker::ta::model_t & model, std::size_t alloc_nb)
      : tchecker::ta::details::state_pool_allocator_t<STATE>
      (alloc_nb,
       alloc_nb, model.system().processes_count(),
       alloc_nb, model.flattened_integer_variables().flattened_size())
      {}
    };
    
    
    /*!
     \brief TA state ouputter
     */
    using state_outputter_t = tchecker::ta::details::state_outputter_t;
    
    
    /*!
     \brief TA transition
     */
    using transition_t = tchecker::ta::details::transition_t;
    
    
    /*!
     \brief TA transition ouputter
     */
    using transition_outputter_t = tchecker::ta::details::transition_outputter_t;
    
    
    /*!
     \brief TA transition singleton allocator
     \tparam TRANSITION : type of transition, should derive from tchecker::ta::transition_t
     */
    template <class TRANSITION=tchecker::ta::transition_t>
    using transition_singleton_allocator_t = tchecker::ta::details::transition_singleton_allocator_t<TRANSITION>;
    
    
    /*!
     \class ts_t
     \brief TA transition system
     */
    class ts_t final : public tchecker::ta::details::ts_t<tchecker::ta::state_t, tchecker::ta::transition_t, tchecker::ta::ta_t> {
    public:
      using tchecker::ta::details::ts_t<tchecker::ta::state_t, tchecker::ta::transition_t, tchecker::ta::ta_t>::ts_t;
    };
    
    
  } // end of namespace ta
  
  
  /*!
   \brief Allocation size for tchecker::ta::state_t
   */
  template <>
  class allocation_size_t<tchecker::ta::state_t> {
  public:
    static constexpr std::size_t alloc_size()
    {
      return sizeof(tchecker::ta::state_t);
    }
  };
  
} // end of namespace tchecker

#endif // TCHECKER_TA_HH
