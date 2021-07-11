/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_REFZG_ALLOCATORS_HH
#define TCHECKER_REFZG_ALLOCATORS_HH

#include <type_traits>

#include "tchecker/refzg/state.hh"
#include "tchecker/refzg/transition.hh"
#include "tchecker/ta/allocators.hh"

/*!
 \file allocators.hh
 \brief Allocators of states and transitions for zone graphs with reference clocks
 */

namespace tchecker {

namespace refzg {

namespace details {

/*!
 \class state_pool_allocator_t
 \tparam STATE : type of states, should inherit from tchecker::refzg::state_t
 and should be a tchecker::make_shared object
 \brief Pool allocator for states of zone graphs with reference clocks that can
 be extended to allocate more complex states
 */
template <class STATE> class state_pool_allocator_t : private tchecker::ta::details::state_pool_allocator_t<STATE> {
  static_assert(std::is_base_of<tchecker::refzg::state_t, STATE>::value, "");

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

  /*!
   \brief Constructor
   \param state_alloc_nb : number of states allocated in one block
   \param vloc_alloc_nb : number of tuple of locations allocated in one block
   \param vloc_capacity : capacity of allocated tuples of locations
   \param intval_alloc_nb : number of valuations of bounded integer variables
   allocated in one block
   \param intval_capacity : capacity of allocated valuations of bounded integer
   variables
   \param zone_alloc_nb : number of zones allocated in one block
   \param ref_clocks : reference clocks
   */
  state_pool_allocator_t(std::size_t state_alloc_nb, std::size_t vloc_alloc_nb, std::size_t vloc_capacity,
                         std::size_t intval_alloc_nb, std::size_t intval_capacity, std::size_t zone_alloc_nb,
                         std::shared_ptr<tchecker::reference_clock_variables_t const> const & ref_clocks)
      : tchecker::ta::details::state_pool_allocator_t<STATE>(state_alloc_nb, vloc_alloc_nb, vloc_capacity, intval_alloc_nb,
                                                             intval_capacity),
        _ref_clocks(ref_clocks),
        _zone_pool(zone_alloc_nb, tchecker::allocation_size_t<tchecker::refzg::shared_zone_t>::alloc_size(_ref_clocks))
  {
  }

  /*!
   \brief Copy constructor (deleted)
   */
  state_pool_allocator_t(tchecker::refzg::details::state_pool_allocator_t<STATE> const &) = delete;

  /*!
   \brief Move constructor (deleted)
   */
  state_pool_allocator_t(tchecker::refzg::details::state_pool_allocator_t<STATE> &&) = delete;

  /*!
   \brief Destructor
   */
  ~state_pool_allocator_t() { destruct_all(); }

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::refzg::details::state_pool_allocator_t<STATE> &
  operator=(tchecker::refzg::details::state_pool_allocator_t<STATE> const &) = delete;

  /*!
   \brief Move-assignment operator (deleted)
   */
  tchecker::refzg::details::state_pool_allocator_t<STATE> &
  operator=(tchecker::refzg::details::state_pool_allocator_t<STATE> &&) = delete;

  /*!
   \brief Construct state
   \param args : arguments to a constructor of STATE beyond the zone
   \return a new instance of STATE constructed from a newly allocated zone, and args
   */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<STATE> construct(ARGS &&... args)
  {
    return tchecker::ta::details::state_pool_allocator_t<STATE>::construct(_zone_pool.construct(_ref_clocks), args...);
  }

  /*!
   \brief Clone state
   \param s : a state
   \return a new instance of STATE that is a clone of s
  */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<STATE> clone(STATE const & s)
  {
    return tchecker::refzg::details::state_pool_allocator_t<STATE>::construct_from_state(s);
  }

  /*!
   \brief Destruct state
   \param p : pointer to state
   \pre p has been constructed by this allocator
   \pre p is not nullptr
   \post the state pointed by p has been destructed if its reference counter is 1 (i.e. p is the
   only pointer to the state), does nothing otherwise
   p points to nullptr if the state has been destructed
   \return true if the state has been destructed, false otherwise
   */
  bool destruct(tchecker::intrusive_shared_ptr_t<STATE> & p)
  {
    if (p.ptr() == nullptr)
      return false;

    auto zone_ptr = p->zone_ptr();

    if (!tchecker::ta::details::state_pool_allocator_t<STATE>::destruct(p))
      return false;

    _zone_pool.destruct(zone_ptr);

    return true;
  }

  /*!
   \brief Collect unused states
   \post Unused states, unused tuples of locations, and unused valuations of
   bounded integer variables have been collected
   */
  void collect()
  {
    tchecker::ta::details::state_pool_allocator_t<STATE>::collect();
    _zone_pool.collect();
  }

  /*!
   \brief Destruct all allocated states
   \post All allocated states, zones, tuples of locations and valuations of
   bounded integer variables have been destructed
   \note invalidates all pointers to states, zones, tuple of locations and
   valuations of bounded integer variables allocated by this allocator
   */
  void destruct_all()
  {
    tchecker::ta::details::state_pool_allocator_t<STATE>::destruct_all();
    _zone_pool.destruct_all();
  }

  /*!
   \brief Accessor
   \return Memory used by this state allocator
   */
  std::size_t memsize() const { return tchecker::ta::details::state_pool_allocator_t<STATE>::memsize() + _zone_pool.memsize(); }

protected:
  /*!
   \brief Construct state from a state
   \param s : a state
   \param args : arguments to a constructor of STATE beyond the zone
   \return a new instance of STATE constructed from a copy of the zone in s, and args
   */
  template <class... ARGS> tchecker::intrusive_shared_ptr_t<STATE> construct_from_state(STATE const & s, ARGS &&... args)
  {
    return tchecker::ta::details::state_pool_allocator_t<STATE>::construct_from_state(s, _zone_pool.construct(s.zone()),
                                                                                      args...);
  }

  std::shared_ptr<tchecker::reference_clock_variables_t const> _ref_clocks; /*!< Reference clocks */
  tchecker::pool_t<tchecker::refzg::shared_zone_t> _zone_pool;              /*!< Pool of zones */
};

/*!
 \class transition_pool_allocator_t
 \tparam TRANSITION : type of transitions, should inherit from
 tchecker::refzg::transition_t and should be a tchecker::make_shared object
 \brief Pool allocator for transitions of zone graphs with reference clocks that
 can be extended to allocate more complex transitions
 */
template <class TRANSITION>
class transition_pool_allocator_t : private tchecker::ta::details::transition_pool_allocator_t<TRANSITION> {
  static_assert(std::is_base_of<tchecker::refzg::transition_t, TRANSITION>::value, "");

public:
  /*!
   \brief Type of allocated transitions
   */
  using transition_t = typename tchecker::ta::details::transition_pool_allocator_t<TRANSITION>::transition_t;

  /*!
   \brief Type of allocated objects (transitions)
   */
  using t = transition_t;

  /*!
   \brief Type of pointer to allocated objects (pointer to transitions)
   */
  using ptr_t = tchecker::intrusive_shared_ptr_t<transition_t>;

  using tchecker::ta::details::transition_pool_allocator_t<TRANSITION>::transition_pool_allocator_t;
  using tchecker::ta::details::transition_pool_allocator_t<TRANSITION>::collect;
  using tchecker::ta::details::transition_pool_allocator_t<TRANSITION>::construct;
  using tchecker::ta::details::transition_pool_allocator_t<TRANSITION>::clone;
  using tchecker::ta::details::transition_pool_allocator_t<TRANSITION>::destruct;
  using tchecker::ta::details::transition_pool_allocator_t<TRANSITION>::destruct_all;
  using tchecker::ta::details::transition_pool_allocator_t<TRANSITION>::memsize;

protected:
  using tchecker::ta::details::transition_pool_allocator_t<TRANSITION>::construct_from_transition;
};

} // end of namespace details

/*!
 \class state_pool_allocator_t
 \brief Pool allocator for states of zone graphs with reference clocks
 */
class state_pool_allocator_t final : public tchecker::refzg::details::state_pool_allocator_t<tchecker::refzg::shared_state_t> {
public:
  using tchecker::refzg::details::state_pool_allocator_t<tchecker::refzg::shared_state_t>::state_pool_allocator_t;
};

/*!
 \class transition_pool_allocator_t
 \brief Pool allocator for transitions of zone graphs with reference clocks
 */
class transition_pool_allocator_t final
    : public tchecker::refzg::details::transition_pool_allocator_t<tchecker::refzg::shared_transition_t> {
public:
  using tchecker::refzg::details::transition_pool_allocator_t<
      tchecker::refzg::shared_transition_t>::transition_pool_allocator_t;
};

} // namespace refzg

} // end of namespace tchecker

#endif // TCHECKER_REFZG_ALLOCATORS_HH
