/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_DETAILS_ALLOCATORS_HH
#define TCHECKER_ZG_DETAILS_ALLOCATORS_HH

#include "tchecker/basictypes.hh"
#include "tchecker/ta/details/allocators.hh"
#include "tchecker/utils/pool.hh"
#include "tchecker/zg/details/transition.hh"

/*!
 \file state_pool_allocator.hh
 \brief Pool allocator for states of zone graphs (details)
 */

namespace tchecker {
  
  namespace zg {
    
    namespace details {
      
      /*!
       \class state_pool_allocator_t
       \brief Pool allocator for zone graph states
       \tparam STATE : type of state, should inherit from tchecker::zg::details::state_t
       \tparam VLOC : type of tuple of locations
       \tparam INTVARS_VAL : type of integer variables valuations
       \tparam ZONE : type of zones
       */
      template
      <class STATE,
      class VLOC=typename STATE::vloc_t,
      class INTVARS_VAL=typename STATE::intvars_valuation_t,
      class ZONE=typename STATE::zone_t
      >
      class state_pool_allocator_t : protected tchecker::ta::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL> {
      public:
        /*!
         \brief Type of allocated states
         */
        using state_t = typename tchecker::ta::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL>::state_t;
        
        /*!
         \brief Type of allocated tuples of locations
         */
        using vloc_t = typename tchecker::ta::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL>::vloc_t;
        
        /*!
         \brief Type of allocated integer variables valuations
         */
        using intvars_valuation_t
        = typename tchecker::ta::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL>::intvars_valuation_t;
        
        /*!
         \brief Type of allocated zones
         */
        using zone_t = ZONE;
        
        /*!
         \brief Type of allocated objects (i.e. states)
         */
        using t = state_t;
        
        /*!
         \brief Type of pointer to allocated objects (pointer to state)
         */
        using ptr_t = tchecker::intrusive_shared_ptr_t<state_t>;
        
        /*!
         \brief Constructor
         \param state_alloc_nb : number of states allocated in one block
         \param vloc_alloc_nb : number of tuple of locations allocated in one block
         \param vloc_capacity : capacity of allocated tuples of locations
         \param intvars_val_alloc_nb : number of integer variables valuations allocated in one block
         \param intvars_val_capacity : capacity of allocated integer variables valuations
         \param zone_alloc_nb : number of zones allocated in one block
         \param zone_dimension : dimension of allocated zones
         */
        state_pool_allocator_t(std::size_t state_alloc_nb,
                               std::size_t vloc_alloc_nb,
                               std::size_t vloc_capacity,
                               std::size_t intvars_val_alloc_nb,
                               std::size_t intvars_val_capacity,
                               std::size_t zone_alloc_nb,
                               tchecker::clock_id_t zone_dimension)
        : tchecker::ta::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL>(state_alloc_nb,
                                                                                  vloc_alloc_nb,
                                                                                  vloc_capacity,
                                                                                  intvars_val_alloc_nb,
                                                                                  intvars_val_capacity),
        _zone_dimension(zone_dimension),
        _zone_pool(zone_alloc_nb, tchecker::allocation_size_t<ZONE>::alloc_size(_zone_dimension))
        {}
        
        /*!
         \brief Copy constructor (deleted)
         */
        state_pool_allocator_t(tchecker::zg::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL, ZONE> const &) = delete;
        
        /*!
         \brief Move constructor (deleted)
         */
        state_pool_allocator_t(tchecker::zg::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL, ZONE> &&) = delete;
        
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
        tchecker::zg::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL, ZONE> &
        operator= (tchecker::zg::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL, ZONE> const &) = delete;
        
        /*!
         \brief Move-assignment operator (deleted)
         */
        tchecker::zg::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL, ZONE> &
        operator= (tchecker::zg::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL, ZONE> &&) = delete;
        
        /*!
         \brief Construct state
         \param args : arguments to a constructor of STATE beyond tuple of locations, integer variables valuation and zone
         \return a new instance of STATE constructed from a newly allocated vloc, a newly allocated valuation of integer
         variables, a newly allocated zone, and args
         */
        template <class ... ARGS>
        tchecker::intrusive_shared_ptr_t<STATE> construct(ARGS && ... args)
        {
          return tchecker::ta::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL>::construct
          (args..., _zone_pool.construct(_zone_dimension));
        }
        
        /*!
         \brief Construct state
         \param state : a state
         \param args : arguments to a constructor of STATE beyond tuple of locations, integer variables valuation and zone
         \return a new instance of STATE constructed from state, a copy of the zone in state, and args
         */
        template <class ... ARGS>
        tchecker::intrusive_shared_ptr_t<STATE> construct_from_state(STATE const & state, ARGS && ... args)
        {
          return tchecker::ta::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL>::construct_from_state
          (state, args..., _zone_pool.construct(state.zone()));
        }
        
        /*!
         \brief Destruct state
         \param p : pointer to state
         \pre p has been constructed by this allocator.
         p is not nullptr
         \post the state pointed by p has been destructed if its reference counter is 1 (i.e. p is the only
         reference to the state), and p points to nullptr. Does nothing otherwise.
         The tuple of locations, integer variables valuation and zone have been destructed (if the state
         was the only one pointing to them)
         \return true if the state has been destructed, false otherwise
         */
        bool destruct(tchecker::intrusive_shared_ptr_t<STATE> & p)
        {
          if (p.ptr() == nullptr)
            return false;
          
          auto zone_ptr = p->zone_ptr();
          
          if ( ! tchecker::ta::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL>::destruct(p) )
            return false;
          
          _zone_pool.destruct(zone_ptr);
          
          return true;
        }
        
        /*!
         \brief Collect unused states
         \post Unused states, unused tuples of locations, unused integer variables valuations and unused zones have been
         collected
         */
        void collect()
        {
          tchecker::ta::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL>::collect();
          _zone_pool.collect();
        }
        
        /*!
         \brief Destruct all allocated states
         \post All allocated states, tuples of locations, integer variables valuations, and zones have been destructed
         */
        void destruct_all()
        {
          tchecker::ta::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL>::destruct_all();
          _zone_pool.destruct_all();
        }
        
        /*!
         \brief Free all allocated memory
         \post All allocated states, tuples of locations, integer variables valuations, and zones have been freed.
         No destructor has been called
         */
        void free_all()
        {
          tchecker::ta::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL>::free_all();
          _zone_pool.free_all();
        }
        
        /*!
         \brief Accessor
         \return Memory used by this state allocator
         */
        std::size_t memsize() const
        {
          return tchecker::ta::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL>::memsize()
          + _zone_pool.memsize();
        }
        
        /*!
         \brief Enroll on garbage collector
         \param gc : garbage collector
         \post The state, vloc, integer variables valuations, and zone pool allocators have been enrolled on gc
         \note this should be enrolled on at most one GC
         */
        void enroll(tchecker::gc_t & gc)
        {
          tchecker::ta::details::state_pool_allocator_t<STATE, VLOC, INTVARS_VAL>::enroll(gc);
          _zone_pool.enroll(gc);
        }
      protected:
        tchecker::clock_id_t _zone_dimension;     /*!< Dimension of allocated zones */
        tchecker::pool_t<ZONE> _zone_pool;        /*!< Pool of zones */
      };
      
      
      
      
      /*!
       \class transition_singleton_allocator_t
       \brief Singleton allocator for transitions
       \tparam TRANSITION : type of transition, should derive from tchecker::zg::details::transition_t
       */
      template <class TRANSITION>
      class transition_singleton_allocator_t : public tchecker::ta::details::transition_singleton_allocator_t<TRANSITION> {
        static_assert(std::is_base_of<tchecker::zg::details::transition_t, TRANSITION>::value, "");
      public:
        using tchecker::ta::details::transition_singleton_allocator_t<TRANSITION>::transition_singleton_allocator_t;
      };
      
    } // end of namespace details
    
  } // end of namespace zg
  
} // end of namespace tchecker

#endif // TCHECKER_ZG_DETAILS_ALLOCATORS_HH
