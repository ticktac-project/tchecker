/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_ALLOCATORS_HH
#define TCHECKER_FSM_ALLOCATORS_HH

#include <memory>
#include <type_traits>

#include "tchecker/fsm/state.hh"
#include "tchecker/fsm/transition.hh"
#include "tchecker/syncprod/allocators.hh"
#include "tchecker/ta/allocators.hh"
#include "tchecker/utils/cache.hh"

/*!
 \file allocators.hh
 \brief Allocators of states and transitions for finite state machines
 */

namespace tchecker {

namespace fsm {

namespace details {

/*!
 \class state_pool_allocator_t
 \tparam STATE : type of states, should inherit from tchecker::fsm::state_t and should be a tchecker::make_shared object
 \brief Pool allocator for states of finite state machines that can be extended to allocate more complex states
 */
template <class STATE> class state_pool_allocator_t : private tchecker::ta::details::state_pool_allocator_t<STATE> {
  static_assert(std::is_base_of<tchecker::fsm::state_t, STATE>::value, "");

public:
  /*!
   \brief Type of allocated states
   */
  using state_t = typename tchecker::ta::details::state_pool_allocator_t<STATE>::state_t;

  /*!
   \brief Type of allocated objects (states)
   */
  using t = state_t;

  /*!
   \brief Type of pointer to allocated objects (pointer to states)
   */
  using ptr_t = tchecker::intrusive_shared_ptr_t<state_t>;

  using tchecker::ta::details::state_pool_allocator_t<STATE>::state_pool_allocator_t;
  using tchecker::ta::details::state_pool_allocator_t<STATE>::construct;
  using tchecker::ta::details::state_pool_allocator_t<STATE>::clone;
  using tchecker::ta::details::state_pool_allocator_t<STATE>::destruct;
  using tchecker::ta::details::state_pool_allocator_t<STATE>::share;
  using tchecker::ta::details::state_pool_allocator_t<STATE>::collect;
  using tchecker::ta::details::state_pool_allocator_t<STATE>::destruct_all;
  using tchecker::ta::details::state_pool_allocator_t<STATE>::memsize;

protected:
  using tchecker::ta::details::state_pool_allocator_t<STATE>::construct_from_state;
};

/*!
 \class transition_pool_allocator_t
 \tparam TRANSITION : type of transitions, should inherit from tchecker::fsm::transition_t and should be a tchecker::make_shared
 object
 \brief Pool allocator for transitions of finite state machines that can be extended to allocate more complex transitions
 */
template <class TRANSITION>
class transition_pool_allocator_t : private tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION> {
  static_assert(std::is_base_of<tchecker::fsm::transition_t, TRANSITION>::value, "");

public:
  /*!
   \brief Type of allocated transitions
   */
  using transition_t = typename tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION>::transition_t;

  /*!
   \brief Type of allocated objects (transitions)
   */
  using t = transition_t;

  /*!
   \brief Type of pointer to allocated objects (pointer to transitions)
   */
  using ptr_t = tchecker::intrusive_shared_ptr_t<transition_t>;

  using tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION>::transition_pool_allocator_t;
  using tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION>::collect;
  using tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION>::construct;
  using tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION>::clone;
  using tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION>::destruct;
  using tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION>::share;
  using tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION>::destruct_all;
  using tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION>::memsize;

protected:
  using tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION>::construct_from_transition;
};

} // end of namespace details

/*!
 \class state_pool_allocator_t
 \brief Pool allocator for states of finite state machines
 */
class state_pool_allocator_t final : public tchecker::fsm::details::state_pool_allocator_t<tchecker::fsm::shared_state_t> {
public:
  using tchecker::fsm::details::state_pool_allocator_t<tchecker::fsm::shared_state_t>::state_pool_allocator_t;
};

/*!
 \class transition_pool_allocator_t
 \brief Pool allocator for transitions of finite state machines
 */
class transition_pool_allocator_t final
    : public tchecker::fsm::details::transition_pool_allocator_t<tchecker::fsm::shared_transition_t> {
public:
  using tchecker::fsm::details::transition_pool_allocator_t<tchecker::fsm::shared_transition_t>::transition_pool_allocator_t;
};

} // end of namespace fsm

} // end of namespace tchecker

#endif // TCHECKER_FSM_ALLOCATORS_HH
