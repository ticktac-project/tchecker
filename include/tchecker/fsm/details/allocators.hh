/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_DETAILS_ALLOCATORS_HH
#define TCHECKER_FSM_DETAILS_ALLOCATORS_HH

#include "tchecker/fsm/details/transition.hh"
#include "tchecker/ts/allocators.hh"
#include "tchecker/utils/gc.hh"
#include "tchecker/utils/pool.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file allocators.hh
 \brief Allocators for transition systems over finite state machines
 */

namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \class state_pool_allocator_t
       \brief Pool allocator for FSM states
       \tparam STATE : type of state, should inherit from tchecker::fsm::details::state_t
       \tparam VLOC : type of tuple of locations
       \tparam INTVARS_VAL : type of integer variables valuations
       */
      template <class STATE, class VLOC=typename STATE::vloc_t, class INTVARS_VAL=typename STATE::intvars_valuation_t>
      class state_pool_allocator_t : protected tchecker::ts::state_pool_allocator_t<STATE> {
      public:
        /*!
         \brief Type of allocated states
         */
        using state_t = typename tchecker::ts::state_pool_allocator_t<STATE>::state_t;
        
        /*!
         \brief Type of allocated tuples of locations
         */
        using vloc_t = VLOC;
        
        /*!
         \brief Type of allocated integer variables valuations
         */
        using intvars_valuation_t = INTVARS_VAL;
        
        /*!
         \brief Type of allocated objects (states)
         */
        using t = state_t;
        
        /*!
         \brief Type of pointer to allocated objects (pointer to states)
         */
        using ptr_t = tchecker::intrusive_shared_ptr_t<state_t>;
        
        /*!
         \brief Constructor
         \param state_alloc_nb : number of states allocated in one block
         \param vloc_alloc_nb : number of tuple of locations allocated in one block
         \param vloc_capacity : capacity of allocated tuples of locations
         \param intvars_val_alloc_nb : number of integer variables valuations allocated in one block
         \param intvars_val_capacity : capacity of allocated integer variables valuations
         */
        state_pool_allocator_t(std::size_t state_alloc_nb, std::size_t vloc_alloc_nb, std::size_t vloc_capacity,
                               std::size_t intvars_val_alloc_nb, std::size_t intvars_val_capacity)
        : tchecker::ts::state_pool_allocator_t<STATE>(state_alloc_nb),
        _vloc_capacity(vloc_capacity),
        _intvars_val_capacity(intvars_val_capacity),
        _vloc_pool(vloc_alloc_nb, tchecker::allocation_size_t<VLOC>::alloc_size(_vloc_capacity)),
        _intvars_val_pool(intvars_val_alloc_nb, tchecker::allocation_size_t<INTVARS_VAL>::alloc_size(_intvars_val_capacity))
        {}
        
        /*!
         \brief Copy constructor (deleted)
         */
        state_pool_allocator_t(tchecker::fsm::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL> const &) = delete;
        
        /*!
         \brief Move constructor (deleted)
         */
        state_pool_allocator_t(tchecker::fsm::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL> &&) = delete;
        
        /*!
         \brief Destructor
         */
        ~state_pool_allocator_t()
        {
          destruct_all();
        }
        
        /*!
         \brief Assignment operator (deleted)
         */
        tchecker::fsm::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL> &
        operator= (tchecker::fsm::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL> const &) = delete;
        
        /*!
         \brief Move-assignment operator (deleted)
         */
        tchecker::fsm::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL> &
        operator= (tchecker::fsm::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL> &&) = delete;
        
        /*!
         \brief Construct state
         \param args : arguments to a constructor of STATE beyond tuple of locations and integer variables valuation
         \return a new instance of STATE constructed from a newly allocated vloc, a newly allocated valuation of integer
         variables, and args
         */
        template <class ... ARGS>
        tchecker::intrusive_shared_ptr_t<STATE> construct(ARGS && ... args)
        {
          return
          tchecker::ts::state_pool_allocator_t<STATE>::construct(args...,
                                                                 _vloc_pool.construct(_vloc_capacity),
                                                                 _intvars_val_pool.construct(_intvars_val_capacity));
        }
        
        /*!
         \brief Construct state
         \param state : a state
         \param args : arguments to a constructor of STATE beyond tuple of locations and integer variables valuation
         \return a new instance of STATE constructed from a copy of the tuple of locations in state, a copy of the
         integer variable valuation in state, and args
         */
        template <class ... ARGS>
        tchecker::intrusive_shared_ptr_t<STATE> construct_from_state(STATE const & state, ARGS && ... args)
        {
          return
          tchecker::ts::state_pool_allocator_t<STATE>::construct
	    (state,
	     args...,
	     _vloc_pool.construct(state.vloc()),
	     _intvars_val_pool.construct(state.intvars_valuation()));
        }
        
        /*!
         \brief Destruct state
         \param p : pointer to state
         \pre p has been constructed by this allocator
         \pre p is not nullptr
         \post the state pointed by p has been destructed if its reference counter is 1 (i.e. p is the
         only pointer to the state), and p points to nullptr. Does nothing otherwise. The tuple of
         locations and integer variables valuation in the state have been destructed (if the state
         was the only one pointing to them)
         \return true if the state has been destructed, false otherwise
         */
        bool destruct(tchecker::intrusive_shared_ptr_t<STATE> & p)
        {
          if (p.ptr() == nullptr)
            return false;
          
          auto vloc_ptr = p->vloc_ptr();
          auto intvars_val_ptr = p->intvars_val_ptr();
          
          if ( ! tchecker::ts::state_pool_allocator_t<STATE>::destruct(p) )
            return false;

          _vloc_pool.destruct(vloc_ptr);
          _intvars_val_pool.destruct(intvars_val_ptr);

          return true;
        }
        
        /*!
         \brief Collect unused states
         \post Unused states, unused tuples of locations and unused integer variables valuations have been collected
         */
        void collect()
        {
          tchecker::ts::state_pool_allocator_t<STATE>::collect();
          _vloc_pool.collect();
          _intvars_val_pool.collect();
        }
        
        /*!
         \brief Destruct all allocated states
         \post All allocated states, tuples of locations and integer variables valuations have been destructed
         */
        void destruct_all()
        {
          tchecker::ts::state_pool_allocator_t<STATE>::destruct_all();
          _vloc_pool.destruct_all();
          _intvars_val_pool.destruct_all();
        }
        
        /*!
         \brief Free all allocated memory
         \post All allocated states, tuples of locations and integer variables valuations have been freed.
         No destructor has been called
         */
        void free_all()
        {
          tchecker::ts::state_pool_allocator_t<STATE>::free_all();
          _vloc_pool.free_all();
          _intvars_val_pool.free_all();
        }
        
        /*!
         \brief Accessor
         \return Memory used by this state allocator
         */
        std::size_t memsize() const
        {
          return tchecker::ts::state_pool_allocator_t<STATE>::memsize()
          + _vloc_pool.memsize()
          + _intvars_val_pool.memsize();
        }
        
        /*!
         \brief Enroll on garbage collector
         \param gc : garbage collector
         \post The state, vloc and integer variables valuations pool allocators have been enrolled on gc
         \note this should be enrolled on at most one GC
         */
        void enroll(tchecker::gc_t & gc)
        {
          tchecker::ts::state_pool_allocator_t<STATE>::enroll(gc);
          _vloc_pool.enroll(gc);
          _intvars_val_pool.enroll(gc);
        }
      protected:
        std::size_t _vloc_capacity;                        /*!< Capacity of tuples of locations */
        std::size_t _intvars_val_capacity;                 /*!< Capacity of integer variables valuations */
        tchecker::pool_t<VLOC> _vloc_pool;                 /*!< Pool of tuples of locations */
        tchecker::pool_t<INTVARS_VAL> _intvars_val_pool;   /*!< Pool of integer variables valuations */
      };
      
      
      
      
      /*!
       \class transition_singleton_allocator_t
       \brief Singleton allocator for transitions
       \tparam TRANSITION : type of transition, should derive from tchecker::fsm::details::transition_t
       */
      template <class TRANSITION>
      class transition_singleton_allocator_t : public tchecker::ts::transition_singleton_allocator_t<TRANSITION> {
        static_assert(std::is_base_of<tchecker::fsm::details::transition_t, TRANSITION>::value, "");
      public:
        using tchecker::ts::transition_singleton_allocator_t<TRANSITION>::transition_singleton_allocator_t;
      };
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_DETAILS_ALLOCATOR_HH

