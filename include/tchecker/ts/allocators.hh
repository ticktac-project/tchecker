/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_ALLOCATORS_HH
#define TCHECKER_TS_ALLOCATORS_HH

#include "tchecker/ts/state.hh"
#include "tchecker/ts/transition.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/gc.hh"
#include "tchecker/utils/pool.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/utils/singleton_pool.hh"

/*!
 \file allocators.hh
 \brief Allocators for transition systems
 */

namespace tchecker {
  
  namespace ts {
    
    /*!
     \class state_pool_allocator_t
     \brief Pool allocator of states
     \tparam STATE : type of states, should inherit from tchecker::ts::state_t
     */
    template <class STATE>
    class state_pool_allocator_t {
      
      static_assert(std::is_base_of<tchecker::ts::state_t, STATE>::value, "");
      
    public:
      /*!
       \brief Type of allocated states
       */
      using state_t = STATE;
      
      /*!
       \brief Type of pointer to states
       */
      using state_ptr_t = typename tchecker::intrusive_shared_ptr_t<STATE>;
      
      /*!
       \brief Type of allocated objects
       */
      using t = state_t;
      
      /*!
       \brief Type of pointer to allocated objects
       */
      using ptr_t = state_ptr_t;
      
      /*!
       \brief Constructor
       \param alloc_nb : number of states allocated in one block
       */
      state_pool_allocator_t(std::size_t alloc_nb)
      : _state_pool(alloc_nb, tchecker::allocation_size_t<STATE>::alloc_size())
      {}
      
      /*!
       \brief Copy constructor (deleted)
       */
      state_pool_allocator_t(tchecker::ts::state_pool_allocator_t<STATE> const &) = delete;
      
      /*!
       \brief Move constructor (deleted)
       */
      state_pool_allocator_t(tchecker::ts::state_pool_allocator_t<STATE> &&) = delete;
      
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
      tchecker::ts::state_pool_allocator_t<STATE> &
      operator= (tchecker::ts::state_pool_allocator_t<STATE> const &) = delete;
      
      /*!
       \brief Move-assignment operator (deleted)
       */
      tchecker::ts::state_pool_allocator_t<STATE> &
      operator= (tchecker::ts::state_pool_allocator_t<STATE> &&) = delete;
      
      /*!
       \brief Construct state
       \param args : arguments to a constructor of STATE
       \return a new instance of STATE constructed from args
       */
      template <class ... ARGS>
      tchecker::intrusive_shared_ptr_t<STATE> construct(ARGS && ... args)
      {
        return _state_pool.construct(std::forward<ARGS>(args)...);
      }
      
      /*!
       \brief Destruct state
       \param p : pointer to a state
       \pre p has been constructed by this allocator
       p is not nullptr
       \post the object pointed by p has been destructed if its reference counter is 1
       (i.e. p is the only pointer to the object), and p points to nullptr. Does nothing
       otherwise
       \return true if the state has been destructed, false otherwise
       */
      bool destruct(tchecker::intrusive_shared_ptr_t<STATE> & p)
      {
        return _state_pool.destruct(p);
      }
      
      /*!
       \brief Collect unused states
       \post Unused states have been deleted
       \note Use method enroll() to enroll on a tchecker::gc_t garbage collector
       */
      void collect()
      {
        _state_pool.collect();
      }
      
      /*!
       \brief Destruct all allocated states
       \post All allocated states have been destructed
       */
      void destruct_all()
      {
        _state_pool.destruct_all();
      }
      
      /*!
       \brief Free all allocated memory
       \post All allocated memory have been deleted, but not destructor has been called
       */
      void free_all()
      {
        _state_pool.free_all();
      }
      
      /*!
       \brief Accessor
       \return Memory used by this state allocator
       */
      std::size_t memsize() const
      {
        return _state_pool.memsize();
      }
      
      /*!
       \brief Enroll on garbage collector
       \param gc : garbage collector
       \post The state pool allocator has been enrolled on gc
       \note this should be enrolled on at most one GC
       */
      void enroll(tchecker::gc_t & gc)
      {
        _state_pool.enroll(gc);
      }
    protected:
      tchecker::pool_t<STATE> _state_pool;   /*!< Pool of states */
    };
    
    
    
    
    /*!
     \brief Singleton allocator for transitions
     \tparam TRANSITION : type of transition, should derive from tchecker::ts::transition_t
     */
    template <class TRANSITION>
    class transition_singleton_allocator_t : public tchecker::singleton_pool_t<TRANSITION> {
      static_assert(std::is_base_of<tchecker::ts::transition_t, TRANSITION>::value, "");
    public:
      using tchecker::singleton_pool_t<TRANSITION>::singleton_pool_t;
    };
    
    
    
    
    /*!
     \class allocator_t
     \brief Garbage-collected allocator of transition system
     \tparam STATE_ALLOCATOR : type of state allocator
     \tparam TRANSITION_ALLOCATOR : type of transition allocator
     */
    template <class STATE_ALLOCATOR, class TRANSITION_ALLOCATOR>
    class allocator_t {
    public:
      /*!
       \brief Type of states
       */
      using state_t = typename STATE_ALLOCATOR::t;
      
      /*!
       \brief Type of pointers to state
       */
      using state_ptr_t = typename STATE_ALLOCATOR::ptr_t;
      
      /*!
       \brief Type of state allocator
       */
      using state_allocator_t = STATE_ALLOCATOR;
      
      /*!
       \brief Type of transitions
       */
      using transition_t = typename TRANSITION_ALLOCATOR::t;
      
      /*!
       \brief Type of pointers to transition
       */
      using transition_ptr_t = typename TRANSITION_ALLOCATOR::ptr_t;
      
      /*!
       \brief Type of transition allocator
       */
      using transition_allocator_t = TRANSITION_ALLOCATOR;
      
      /*!
       \brief Constructor
       \param gc : garbage collector
       \param sa_args : parameters to a constructor of STATE_ALLOCATOR
       \param ta_args : parameters to a constructor of TRANSITION_ALLOCATOR
       \post this owns a state allocator built from sa_args, and a transition allocator built from ta_args.
       Both allocators have been enrolled to gc
       */
      template <class ... SA_ARGS, class ... TA_ARGS>
      allocator_t(tchecker::gc_t & gc, std::tuple<SA_ARGS...> && sa_args, std::tuple<TA_ARGS...> && ta_args)
      : _state_allocator(std::make_from_tuple<STATE_ALLOCATOR>(sa_args)),
      _transition_allocator(std::make_from_tuple<TRANSITION_ALLOCATOR>(ta_args))
      {
        _state_allocator.enroll(gc);
        _transition_allocator.enroll(gc);
      }
      
      /*!
       \brief Constructor
       \param t : tuple of arguments to a constructor of tchecker::ts::allocator_t
       \post see other constructors
       */
      template <class ... SA_ARGS, class ... TA_ARGS>
      allocator_t(std::tuple<tchecker::gc_t &, std::tuple<SA_ARGS...>, std::tuple<TA_ARGS...>> && t)
      : allocator_t(std::get<0>(t),
      std::forward<std::tuple<SA_ARGS...>>(std::get<1>(t)),
                    std::forward<std::tuple<TA_ARGS...>>(std::get<2>(t)))
      {}
      
      /*!
       \brief Copy constructor (deleted)
       */
      allocator_t(tchecker::ts::allocator_t<STATE_ALLOCATOR, TRANSITION_ALLOCATOR> const &) = delete;
      
      /*!
       \brief Move constructor (deleted)
       */
      allocator_t(tchecker::ts::allocator_t<STATE_ALLOCATOR, TRANSITION_ALLOCATOR> &&) = delete;
      
      /*!
       \brief Destructor
       \note see destruct_all()
       */
      ~allocator_t() = default;
      
      /*!
       \brief Assignment operator (deleted)
       */
      tchecker::ts::allocator_t<STATE_ALLOCATOR, TRANSITION_ALLOCATOR> &
      operator= (tchecker::ts::allocator_t<STATE_ALLOCATOR, TRANSITION_ALLOCATOR> const &) = delete;
      
      /*!
       \brief Move-assignment operator (deleted)
       */
      tchecker::ts::allocator_t<STATE_ALLOCATOR, TRANSITION_ALLOCATOR> &
      operator= (tchecker::ts::allocator_t<STATE_ALLOCATOR, TRANSITION_ALLOCATOR> &&) = delete;
      
      /*!
       \brief States and transitions destruction
       \post all allocated states and transitions have been deleted. All pointers
       returned by methods allocate_state(), allocate_from_state() and
       allocate_transition() have been invalidated
       */
      void destruct_all()
      {
        _state_allocator.destruct_all();
        _transition_allocator.destruct_all();
      }
      
      /*!
       \brief Fast memory deallocation
       \post all allocated states and transitins have been freed. No destructor
       has been called. All pointers returned by methods allocate_state(),
       allocate_from_state() and allocate_transition() have been invalidated
       */
      void free_all()
      {
        _state_allocator.free_all();
        _transition_allocator.free_all();
      }
      
      /*!
       \brief State construction
       \param args : parameters to a constructor of state_t
       \return pointer to a newly allocated state constructed from args
       */
      template <class ... ARGS>
      inline state_ptr_t construct_state(ARGS && ... args)
      {
        return _construct_state(std::forward<ARGS>(args)...);
      }
      
      /*!
       \brief State construction
       \param args : tuple of parameters to a constructor of state_t
       \return pointer to a newly allocated state constructed from args
       */
      template <class ... ARGS>
      inline state_ptr_t construct_state(std::tuple<ARGS...> && args)
      {
        return std::apply(&tchecker::ts::allocator_t<STATE_ALLOCATOR, TRANSITION_ALLOCATOR>::_construct_state<ARGS...>,
                          std::tuple_cat(std::make_tuple(this), args));
      }
      
      /*!
       \brief State construction
       \param state : a state
       \param args : extra parameters to a constructor of state_t
       \return pointer to a newly allocated state constructed from state and args
       */
      template <class ... ARGS>
      inline state_ptr_t construct_from_state(state_ptr_t const & state, ARGS && ... args)
      {
        return _construct_from_state(state, std::forward<ARGS>(args)...);
      }
      
      /*!
       \brief State construction
       \param state : a state
       \param args : tuple of extra parameters to a constructor of state_t
       \return pointer to a newly allocated state constructed from state and args
       */
      template <class ... ARGS>
      inline state_ptr_t construct_from_state(state_ptr_t const & state, std::tuple<ARGS...> && args)
      {
        return
        std::apply(&tchecker::ts::allocator_t<STATE_ALLOCATOR, TRANSITION_ALLOCATOR>::_construct_from_state<ARGS...>,
                   std::tuple_cat(std::tuple<decltype(this), state_ptr_t const &>(this, state), args));
      }
      
      /*!
       \brief Destruct state
       \param p : pointer to state
       \pre p has been allocated by this allocator
       p is not nullptr
       \post the state pointed by p has been destructed and set to nullptr if its reference
       counter is 1 (i.e. p is the only pointer to this state)
       */
      bool destruct_state(state_ptr_t & p)
      {
        return _state_allocator.destruct(p);
      }
      
      /*!
       \brief Transition contruction
       \param args : parameters to a constructor of transition_t
       \return pointer to a newly allocated transition constructed from args
       */
      template <class ... ARGS>
      inline transition_ptr_t construct_transition(ARGS && ... args)
      {
        return _allocate_transition(std::forward<ARGS>(args)...);
      }
      
      /*!
       \brief Transition construction
       \param args : tuple of parameters to a constructor of transition_t
       \return pointer to a newly allocated transition constructed from args
       */
      template <class ... ARGS>
      inline transition_ptr_t construct_transition(std::tuple<ARGS...> && args)
      {
        return
        std::apply(&tchecker::ts::allocator_t<STATE_ALLOCATOR, TRANSITION_ALLOCATOR>::_construct_transition<ARGS...>,
                   std::tuple_cat(std::make_tuple(this), args));
      }
      
      /*!
       \brief Destruct transition
       \param p : pointer to transition
       \pre p has been allocated by this allocator
       p is not nullptr
       \post the transition pointed by p has been destructed and set to nullptr if its reference
       counter is 1 (i.e. p is the only pointer to this transition)
       */
      bool destruct_transition(transition_ptr_t & p)
      {
        return _transition_allocator.destruct(p);
      }
      
      /*!
       \brief Enroll on garbage collector
       \param gc : garbage collector
       \pre this is not enrolled to a garbage collector yet
       \post this is enrolled to gc
       */
      void enroll(tchecker::gc_t & gc)
      {
        _state_allocator.enroll(gc);
        _transition_allocator.enroll(gc);
      }
    protected:
      /*!
       \brief State construction
       \param args : parameters to a constructor of state_t
       \return pointer to a newly allocated state constructed from args
       */
      template <class ... ARGS>
      inline state_ptr_t _construct_state(ARGS && ... args)
      {
        return _state_allocator.construct(args...);
      }
      
      /*!
       \brief State construction
       \param state : a state
       \param args : extra parameters to a constructor of state_t
       \return pointer to a newly allocated state constructed from state and args
       */
      template <class ... ARGS>
      inline state_ptr_t _construct_from_state(state_ptr_t const & state, ARGS && ... args)
      {
        return _state_allocator.construct_from_state(*state, std::forward<ARGS>(args)...);
      }
      
      /*!
       \brief Transition construction
       \param args : parameters to a constructor of transition_t
       \return pointer to a newly allocated transiton constructed from args
       */
      template <class ... ARGS>
      inline transition_ptr_t _construct_transition(ARGS && ... args)
      {
        return _transition_allocator.construct(args...);
      }
      
      STATE_ALLOCATOR _state_allocator;             /*!< States allocator */
      TRANSITION_ALLOCATOR _transition_allocator;   /*!< Transitions allocator */
    };
    
  } // end of namespace ts
  
} // end of namespace tchecker

#endif // TCHECKER_TS_ALLOCATORS_HH

