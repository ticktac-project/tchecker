/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYNCPROD_ALLOCATORS_HH
#define TCHECKER_SYNCPROD_ALLOCATORS_HH

#include <type_traits>

#include "tchecker/syncprod/state.hh"
#include "tchecker/syncprod/transition.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/ts/allocators.hh"

/*!
 \file allocators.hh
 \brief Allocators of states and transitions for synchroned product of timed processes
 */

namespace tchecker {

namespace syncprod {

namespace details {

/*!
 \class state_pool_allocator_t
 \tparam STATE : type of states, should inherit from tchecker::syncprod::state_t and should be a tchecker::make_shared object
 \brief Pool allocator for states of synchronized product of transition systems that can be extended to allocate more complex
 states
 */
template <class STATE> class state_pool_allocator_t : private tchecker::ts::state_pool_allocator_t<STATE> {
  static_assert(std::is_base_of<tchecker::syncprod::state_t, STATE>::value, "");

public:
  /*!
   \brief Type of allocated states
   */
  using state_t = typename tchecker::ts::state_pool_allocator_t<STATE>::state_t;

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
   */
  state_pool_allocator_t(std::size_t state_alloc_nb, std::size_t vloc_alloc_nb, std::size_t vloc_capacity)
      : tchecker::ts::state_pool_allocator_t<STATE>(state_alloc_nb), _vloc_capacity(vloc_capacity),
        _vloc_pool(vloc_alloc_nb, tchecker::allocation_size_t<tchecker::shared_vloc_t>::alloc_size(_vloc_capacity))
  {
  }

  /*!
   \brief Copy constructor (deleted)
   */
  state_pool_allocator_t(tchecker::syncprod::details::state_pool_allocator_t<STATE> const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  state_pool_allocator_t(tchecker::syncprod::details::state_pool_allocator_t<STATE> &&) = delete;

  /*!
   \brief Destructor
   */
  ~state_pool_allocator_t() { destruct_all(); }

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::syncprod::details::state_pool_allocator_t<STATE> &
  operator=(tchecker::syncprod::details::state_pool_allocator_t<STATE> const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::syncprod::details::state_pool_allocator_t<STATE> &
  operator=(tchecker::syncprod::details::state_pool_allocator_t<STATE> &&) = delete;

  /*!
   \brief Construct state
   \param args : arguments to a constructor of STATE beyond tuple of locations
   \return a new instance of STATE constructed from a newly allocated tuple of locations and args
   */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<STATE> construct(ARGS &&... args)
  {
    return tchecker::ts::state_pool_allocator_t<STATE>::construct(_vloc_pool.construct(_vloc_capacity), args...);
  }

  /*!
   \brief Clone state
   \param s : state to clone
   \return a new instance of STATE that is a clone of s
  */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<STATE> clone(STATE const & s)
  {
    return tchecker::syncprod::details::state_pool_allocator_t<STATE>::construct_from_state(s);
  }

  /*!
   \brief Destruct state
   \param p : pointer to state
   \pre p has been constructed by this allocator
   \pre p is not nullptr
   \post the state pointed by p has been destructed if its reference counter is 1 (i.e. p is the
   only pointer to the state), does nothing otherwise
   p points to nullptr if the state has been dstructed
   \return true if the state has been destructed, false otherwise
   */
  bool destruct(tchecker::intrusive_shared_ptr_t<STATE> & p)
  {
    if (p.ptr() == nullptr)
      return false;

    auto vloc_ptr = p->vloc_ptr();

    if (!tchecker::ts::state_pool_allocator_t<STATE>::destruct(p))
      return false;

    _vloc_pool.destruct(vloc_ptr);

    return true;
  }

  /*!
   \brief Collect unused states
   \post Unused states and unused tuples of locations have been collected
   */
  void collect()
  {
    tchecker::ts::state_pool_allocator_t<STATE>::collect();
    _vloc_pool.collect();
  }

  /*!
   \brief Destruct all allocated states
   \post All allocated states and tuples of locations have been destructed
   \note invalidates all pointers to states and tuple of locations allocated by this allocator
   */
  void destruct_all()
  {
    tchecker::ts::state_pool_allocator_t<STATE>::destruct_all();
    _vloc_pool.destruct_all();
  }

  /*!
   \brief Accessor
   \return Memory used by this state allocator
   */
  std::size_t memsize() const { return tchecker::ts::state_pool_allocator_t<STATE>::memsize() + _vloc_pool.memsize(); }

protected:
  /*!
   \brief Construct state
   \param s : a state
   \param args : arguments to a constructor of STATE beyond tuple of locations
   \return a new instance of STATE constructed from a copy of the tuple of locations in s and args
   */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<STATE> construct_from_state(STATE const & s, ARGS &&... args)
  {
    return tchecker::ts::state_pool_allocator_t<STATE>::construct_from_state(s, _vloc_pool.construct(s.vloc()), args...);
  }

  std::size_t _vloc_capacity;                           /*!< Capacity of tuples of locations */
  tchecker::pool_t<tchecker::shared_vloc_t> _vloc_pool; /*!< Pool of tuples of locations */
};

/*!
 \class transition_pool_allocator_t
 \tparam TRANSITION : type of transitions, should inherit from tchecker::syncprod::transition_t and should be a
 tchecker::make_shared object
 \brief Pool allocator for transitions of synchronized product of transition systems that can be
 extended to allocate more complex transitions
 */
template <class TRANSITION> class transition_pool_allocator_t : private tchecker::ts::transition_pool_allocator_t<TRANSITION> {
  static_assert(std::is_base_of<tchecker::syncprod::transition_t, TRANSITION>::value, "");

public:
  /*!
   \brief Type of allocated transitions
   */
  using transition_t = typename tchecker::ts::transition_pool_allocator_t<TRANSITION>::transition_t;

  /*!
   \brief Type of allocated objects (transitions)
   */
  using t = transition_t;

  /*!
   \brief Type of pointer to allocated objects (pointer to transitions)
   */
  using ptr_t = tchecker::intrusive_shared_ptr_t<transition_t>;

  /*!
   \brief Constructor
   \param transition_alloc_nb : number of transitions allocated in one block
   \param vedge_alloc_nb : number of tuple of edges allocated in one block
   \param vedge_capacity : capacity of allocated tuples of edges
   */
  transition_pool_allocator_t(std::size_t transition_alloc_nb, std::size_t vedge_alloc_nb, std::size_t vedge_capacity)
      : tchecker::ts::transition_pool_allocator_t<TRANSITION>(transition_alloc_nb), _vedge_capacity(vedge_capacity),
        _vedge_pool(vedge_alloc_nb, tchecker::allocation_size_t<tchecker::shared_vedge_t>::alloc_size(_vedge_capacity))
  {
  }

  /*!
   \brief Copy constructor (deleted)
   */
  transition_pool_allocator_t(tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION> const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  transition_pool_allocator_t(tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION> &&) = delete;

  /*!
   \brief Destructor
   */
  ~transition_pool_allocator_t() { destruct_all(); }

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION> &
  operator=(tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION> const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION> &
  operator=(tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION> &&) = delete;

  /*!
   \brief Construct transition
   \param args : arguments to a constructor of TRANSITION beyond tuple of edges
   \return a new instance of TRANSITION constructed from a newly allocated tuple of edges and args
   */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<TRANSITION> construct(ARGS &&... args)
  {
    return tchecker::ts::transition_pool_allocator_t<TRANSITION>::construct(args..., _vedge_pool.construct(_vedge_capacity));
  }

  /*!
   \brief Clone a transition
   \param t : transition
   \return a new instance of TRANSITION that is a clone of t
  */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<TRANSITION> clone(TRANSITION const & t)
  {
    return tchecker::syncprod::details::transition_pool_allocator_t<TRANSITION>::construct_from_transition(t);
  }

  /*!
   \brief Destruct transition
   \param p : pointer to transition
   \pre p has been constructed by this allocator
   \pre p is not nullptr
   \post the transition pointed by p has been destructed if its reference counter is 1 (i.e. p is the
   only pointer to the transition), does nothing otherwise
   p points to nullptr if the transition has been dstructed
   \return true if the transition has been destructed, false otherwise
   */
  bool destruct(tchecker::intrusive_shared_ptr_t<TRANSITION> & p)
  {
    if (p.ptr() == nullptr)
      return false;

    auto vedge_ptr = p->vedge_ptr();

    if (!tchecker::ts::transition_pool_allocator_t<TRANSITION>::destruct(p))
      return false;

    _vedge_pool.destruct(vedge_ptr);

    return true;
  }

  /*!
   \brief Collect unused transitions
   \post Unused transitions and unused tuples of edges have been collected
   */
  void collect()
  {
    tchecker::ts::transition_pool_allocator_t<TRANSITION>::collect();
    _vedge_pool.collect();
  }

  /*!
   \brief Destruct all allocated transitions
   \post All allocated transitions and tuples of edges have been destructed
   \note invalidates all pointers to transitions and tuple of edges allocated by this allocator
   */
  void destruct_all()
  {
    tchecker::ts::transition_pool_allocator_t<TRANSITION>::destruct_all();
    _vedge_pool.destruct_all();
  }

  /*!
   \brief Accessor
   \return Memory used by this transitions allocator
   */
  std::size_t memsize() const
  {
    return tchecker::ts::transition_pool_allocator_t<TRANSITION>::memsize() + _vedge_pool.memsize();
  }

protected:
  /*!
   \brief Construct a transition from a transition
   \param t : a transition
   \param args : arguments to a constructor of TRANSITION beyond tuple of edges
   \return a new instance of TRANSITION constructed from a copy of the tuple of edges in t and args
   */
  template <class... ARGS>
  tchecker::intrusive_shared_ptr_t<TRANSITION> construct_from_transition(TRANSITION const & t, ARGS &&... args)

  {
    return tchecker::ts::transition_pool_allocator_t<TRANSITION>::construct_from_transition(t, args...,
                                                                                            _vedge_pool.construct(t.vedge()));
  }

  std::size_t _vedge_capacity;                            /*!< Capacity of tuples of edges */
  tchecker::pool_t<tchecker::shared_vedge_t> _vedge_pool; /*!< Pool of tuples of edges */
};

} // end of namespace details

/*!
 \class state_pool_allocator_t
 \brief Pool allocator for states of synchronized product of timed processes
 */
class state_pool_allocator_t final
    : public tchecker::syncprod::details::state_pool_allocator_t<tchecker::syncprod::shared_state_t> {
public:
  using tchecker::syncprod::details::state_pool_allocator_t<tchecker::syncprod::shared_state_t>::state_pool_allocator_t;
};

/*!
 \class transition_pool_allocator_t
 \brief Pool allocator for transitions of synchronized product of timed processes
 */
class transition_pool_allocator_t final
    : public tchecker::syncprod::details::transition_pool_allocator_t<tchecker::syncprod::shared_transition_t> {
public:
  using tchecker::syncprod::details::transition_pool_allocator_t<
      tchecker::syncprod::shared_transition_t>::transition_pool_allocator_t;
};

} // end of namespace syncprod

} // end of namespace tchecker

#endif // TCHECKER_SYNCPROD_ALLOCATORS_HH
