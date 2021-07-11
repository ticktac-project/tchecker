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
template <class STATE> class state_pool_allocator_t {

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
  state_pool_allocator_t(std::size_t alloc_nb) : _state_pool(alloc_nb, tchecker::allocation_size_t<STATE>::alloc_size()) {}

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
  ~state_pool_allocator_t() { destruct_all(); }

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::ts::state_pool_allocator_t<STATE> & operator=(tchecker::ts::state_pool_allocator_t<STATE> const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::ts::state_pool_allocator_t<STATE> & operator=(tchecker::ts::state_pool_allocator_t<STATE> &&) = delete;

  /*!
   \brief Construct state
   \param args : arguments to a constructor of STATE
   \return a new instance of STATE constructed from args
   */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<STATE> construct(ARGS &&... args)
  {
    return _state_pool.construct(std::forward<ARGS>(args)...);
  }

  /*!
   \brief Clone state
   \param s : s state
   \return a new instance of STATE that is a clone of s
  */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<STATE> clone(STATE const & s)
  {
    return tchecker::ts::state_pool_allocator_t<STATE>::construct_from_state(s);
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
  bool destruct(tchecker::intrusive_shared_ptr_t<STATE> & p) { return _state_pool.destruct(p); }

  /*!
   \brief Collect unused states
   \post Unused states have been deleted
   */
  void collect() { _state_pool.collect(); }

  /*!
   \brief Destruct all allocated states
   \post All allocated states have been destructed
   */
  void destruct_all() { _state_pool.destruct_all(); }

  /*!
   \brief Accessor
   \return Memory used by this state allocator
   */
  std::size_t memsize() const { return _state_pool.memsize(); }

protected:
  /*!
   \brief Construct a state from a state
   \param s : a state
   \param args : arguments to a constructor of STATE beyond s
   \return a new instance of STATE constructed from s and args
   */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<STATE> construct_from_state(STATE const & s, ARGS &&... args)
  {
    return _state_pool.construct(s, std::forward<ARGS>(args)...);
  }

  tchecker::pool_t<STATE> _state_pool; /*!< Pool of states */
};

/*!
 \class transition_pool_allocator_t
 \brief Pool allocator of transitions
 \tparam TRANSITION : type of transitions, should inherit from tchecker::ts::transition_t
 */
template <class TRANSITION> class transition_pool_allocator_t {

  static_assert(std::is_base_of<tchecker::ts::transition_t, TRANSITION>::value, "");

public:
  /*!
   \brief Type of allocated transitions
   */
  using transition_t = TRANSITION;

  /*!
   \brief Type of pointer to transitions
   */
  using transition_ptr_t = typename tchecker::intrusive_shared_ptr_t<TRANSITION>;

  /*!
   \brief Type of allocated objects
   */
  using t = transition_t;

  /*!
   \brief Type of pointer to allocated objects
   */
  using ptr_t = transition_ptr_t;

  /*!
   \brief Constructor
   \param alloc_nb : number of transitions allocated in one block
   */
  transition_pool_allocator_t(std::size_t alloc_nb)
      : _transition_pool(alloc_nb, tchecker::allocation_size_t<TRANSITION>::alloc_size())
  {
  }

  /*!
   \brief Copy constructor (deleted)
   */
  transition_pool_allocator_t(tchecker::ts::transition_pool_allocator_t<TRANSITION> const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  transition_pool_allocator_t(tchecker::ts::transition_pool_allocator_t<TRANSITION> &&) = delete;

  /*!
   \brief Destructor
   */
  ~transition_pool_allocator_t() { destruct_all(); }

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::ts::transition_pool_allocator_t<TRANSITION> &
  operator=(tchecker::ts::transition_pool_allocator_t<TRANSITION> const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::ts::transition_pool_allocator_t<TRANSITION> &
  operator=(tchecker::ts::transition_pool_allocator_t<TRANSITION> &&) = delete;

  /*!
   \brief Construct transition
   \param args : arguments to a constructor of TRANSITION
   \return a new instance of TRANSITION constructed from args
   */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<TRANSITION> construct(ARGS &&... args)
  {
    return _transition_pool.construct(std::forward<ARGS>(args)...);
  }

  /*!
   \brief Clone a transition
   \param t : a transition
   \return a new instance of TRANSITION that is a clone of t
  */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<TRANSITION> clone(TRANSITION const & t)
  {
    return tchecker::ts::transition_pool_allocator_t<TRANSITION>::construct_from_transition(t);
  }

  /*!
   \brief Destruct transition
   \param p : pointer to a transition
   \pre p has been constructed by this allocator
   p is not nullptr
   \post the object pointed by p has been destructed if its reference counter is 1
   (i.e. p is the only pointer to the object), and p points to nullptr. Does nothing otherwise
   \return true if the transition has been destructed, false otherwise
   */
  bool destruct(tchecker::intrusive_shared_ptr_t<TRANSITION> & p) { return _transition_pool.destruct(p); }

  /*!
   \brief Collect unused transitions
   \post Unused transitions have been deleted
   */
  void collect() { _transition_pool.collect(); }

  /*!
   \brief Destruct all allocated transitions
   \post All allocated transitions have been destructed
   */
  void destruct_all() { _transition_pool.destruct_all(); }

  /*!
   \brief Accessor
   \return Memory used by this transition allocator
   */
  std::size_t memsize() const { return _transition_pool.memsize(); }

protected:
  /*!
   \brief Construct a transition from a transition
   \param t : a transition
   \param args : arguments to a constructor of TRANSITION beyond t
   \return a new instance of TRANSITION constructed from t and args
   */
  template <class... ARGS>
  tchecker::intrusive_shared_ptr_t<TRANSITION> construct_from_transition(TRANSITION const & t, ARGS &&... args)
  {
    return _transition_pool.construct(t, std::forward<ARGS>(args)...);
  }

  tchecker::pool_t<TRANSITION> _transition_pool; /*!< Pool of transitions */
};

} // end of namespace ts

} // end of namespace tchecker

#endif // TCHECKER_TS_ALLOCATORS_HH
