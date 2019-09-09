/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_BUILDER_HH
#define TCHECKER_TS_BUILDER_HH

#include <tuple>
#include <type_traits>

#include "tchecker/basictypes.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file builder.hh
 \brief Transition system builder
 */

namespace tchecker {
  
  namespace ts {
    
    /*!
     \class builder_t
     \brief Build states and transitons of a transition system
     \tparam TS : type of transition system (see tchecker::ts::ts_t)
     \tparam ALLOCATOR : type of allocator (see tchecker::ts::allocator_t), should be garbage collected
     */
    template <class TS, class ALLOCATOR>
    class builder_t {
      static_assert(std::is_base_of<typename TS::state_t, typename ALLOCATOR::state_t>::value, "");
      static_assert(std::is_base_of<typename TS::transition_t, typename ALLOCATOR::transition_t>::value, "");
    public:
      /*!
       \brief Type of pointer to state
       */
      using state_ptr_t = typename ALLOCATOR::state_ptr_t;
      
      /*!
       \brief Type of pointer to transition
       */
      using transition_ptr_t = typename ALLOCATOR::transition_ptr_t;
      
      /*!
       \brief Constructor
       \param ts : a transition system
       \param allocator : an allocator
       \note this keeps a reference on ts and a reference on allocator
       */
      builder_t(TS & ts, ALLOCATOR & allocator) : _ts(ts), _allocator(allocator)
      {}
      
      /*!
       \brief Copy constructor
       */
      builder_t(tchecker::ts::builder_t<TS, ALLOCATOR> const &) = default;
      
      /*!
       \brief Move constructor
       */
      builder_t(tchecker::ts::builder_t<TS, ALLOCATOR> &&) = default;
      
      /*!
       \brief Destructor
       */
      ~builder_t() = default;
      
      /*!
       \brief Assignment operator
       */
      tchecker::ts::builder_t<TS, ALLOCATOR> & operator= (tchecker::ts::builder_t<TS, ALLOCATOR> const &) = default;
      
      /*!
       \brief Move assignment operator
       */
      tchecker::ts::builder_t<TS, ALLOCATOR> & operator= (tchecker::ts::builder_t<TS, ALLOCATOR> &&) = default;
      
      /*!
       \brief Compute initial state
       \param v : initial iterator value from the transition system
       \param sargs : parameters to ALLOCATOR::allocate_state()
       \param targs : parameters to ALLOCATOR::allocate_transition()
       \return a triple <state, transition, status> where state and transition are the initial state and transition
       computed from v, and status is the tchecker::state_status_t status of state.
       \note state points to nullptr if status != tchecker::STATE_OK
       */
      template <class ... SARGS, class ... TARGS>
      std::tuple<state_ptr_t, transition_ptr_t, tchecker::state_status_t>
      initial_state(typename TS::initial_iterator_value_t const & v, std::tuple<SARGS...> && sargs, std::tuple<TARGS...> && targs)
      {
        state_ptr_t state = _allocator.construct_state(std::forward<std::tuple<SARGS...>>(sargs));
        transition_ptr_t transition = _allocator.construct_transition(std::forward<std::tuple<TARGS...>>(targs));
        
        tchecker::state_status_t status = _ts.initialize(*state, *transition, v);
        
        return std::make_tuple((status == tchecker::STATE_OK ? state : state_ptr_t(nullptr)), transition, status);
      }
      
      /*!
       \brief Compute next state
       \param state : source state
       \param v : outgoing iterator value from the transition system
       \param sargs : parameters to ALLOCATOR::allocate_state()
       \param targs : parameters to ALLOCATOR::allocate_transition()
       \return a triple <next_state, transition, status> where next_state and transition are the next state
       and the outgoing transition of state corresponding to v, and status is the tchecker::state_status_t
       status of next_state.
       \note next_state points to nullptr if status != tchecker::STATE_OK
       */
      template <class ... SARGS, class ... TARGS>
      std::tuple<state_ptr_t, transition_ptr_t, tchecker::state_status_t>
      next_state(state_ptr_t & state,
                 typename TS::outgoing_edges_iterator_value_t const & v,
                 std::tuple<SARGS...> && sargs,
                 std::tuple<TARGS...> && targs)
      {
        state_ptr_t next_state = _allocator.construct_from_state(state, std::forward<std::tuple<SARGS...>>(sargs));
        transition_ptr_t transition = _allocator.construct_transition(std::forward<std::tuple<TARGS...>>(targs));
        
        tchecker::state_status_t status = _ts.next(*next_state, *transition, v);
        
        return std::make_tuple((status == tchecker::STATE_OK ? next_state : state_ptr_t(nullptr)), transition, status);
      }
    protected:
      TS & _ts;                 /*!< Transition system */
      ALLOCATOR & _allocator;   /*!< Allocator */
    };
    
    
    
    
    /*!
     \class builder_ok_t
     \brief Build states and transitions from a transition system skiping states which do not have status tchecker::STATE_OK
     \param TS : type of transition system (see tchecker::ts::ts_t)
     \tparam ALLOCATOR : type of allocator (see tchecker::ts::allocator_t), should be garbage collected
     */
    template <class TS, class ALLOCATOR>
    class builder_ok_t : private tchecker::ts::builder_t<TS, ALLOCATOR> {
    public:
      /*!
       \brief Type of pointer to state
       */
      using state_ptr_t = typename tchecker::ts::builder_t<TS, ALLOCATOR>::state_ptr_t;
      
      /*!
       \brief Type of pointer to transition
       */
      using transition_ptr_t = typename tchecker::ts::builder_t<TS, ALLOCATOR>::transition_ptr_t;
      
      /*!
       \brief Constructors
       */
      using tchecker::ts::builder_t<TS, ALLOCATOR>::builder_t;
      
      /*!
       \class iterator_t
       \brief Iterator on pairs (state, transition)
       \tparam TS_ITERATOR : iterator in TS
       */
      template <class TS_ITERATOR>
      class iterator_t {
      public:
        /*!
         \brief Type of TS iterator
         */
        using ts_iterator_t = TS_ITERATOR;
        
        /*!
         \brief Dereference type of TS iterator
         */
        using ts_iterator_value_t = typename std::iterator_traits<ts_iterator_t>::value_type;
        
        /*!
         \brief Type of state/transition computing function
         */
        using state_transition_computer_t
        = std::function<std::tuple<state_ptr_t, transition_ptr_t, tchecker::state_status_t>(ts_iterator_value_t const &)>;
        
        /*!
         \brief Constructor
         \param it : TS iterator, should provide method at_end()
         \param computer : state/transition computing function
         */
        iterator_t(TS_ITERATOR const & it, state_transition_computer_t computer)
        : _it(it), _computer(std::move(computer))
        {
          skip_bad();
        }
        
        /*!
         \brief Constructor
         \param it : TS iterator, should provide nethod at_end()
         \param computer : state/transition computing function
         */
        iterator_t(TS_ITERATOR && it, state_transition_computer_t computer)
        : _it(std::move(it)), _computer(std::move(computer))
        {
          skip_bad();
        }
        
        /*!
         \brief Copy constructor
         */
        iterator_t(tchecker::ts::builder_ok_t<TS, ALLOCATOR>::iterator_t<TS_ITERATOR> const &) = default;
        
        /*!
         \brief Move constructor
         */
        iterator_t(tchecker::ts::builder_ok_t<TS, ALLOCATOR>::iterator_t<TS_ITERATOR> &&) = default;
        
        /*!
         \brief Destructor
         */
        ~iterator_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::ts::builder_ok_t<TS, ALLOCATOR>::iterator_t<TS_ITERATOR> &
        operator= (tchecker::ts::builder_ok_t<TS, ALLOCATOR>::iterator_t<TS_ITERATOR> const &) = default;
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::ts::builder_ok_t<TS, ALLOCATOR>::iterator_t<TS_ITERATOR> &
        operator= (tchecker::ts::builder_ok_t<TS, ALLOCATOR>::iterator_t<TS_ITERATOR> &&) = default;
        
        /*!
         \brief Equality predicate
         \param it : an iterator
         \return true if this and it are equal, false otherwise
         */
        bool operator== (tchecker::ts::builder_ok_t<TS, ALLOCATOR>::iterator_t<TS_ITERATOR> const & it) const
        {
          return ((_it == it._it) && (_computer == it._computer));
        }
        
        /*!
         \brief Disequality predicate
         \param it : an iterator
         \return false if this and it are equal, true otherwise
         */
        inline bool operator!= (tchecker::ts::builder_ok_t<TS, ALLOCATOR>::iterator_t<TS_ITERATOR> const & it) const
        {
          return ! (*this == it);
        }
        
        /*!
         \brief Accessor
         \return true if this iterator is past-the-end, false otherwise
         \note calling at_end() is more efficient than iterator comparison (i.e. == and !=)
         */
        inline bool at_end() const
        {
          return _it.at_end();
        }
        
        /*!
         \brief Increment operator
         \pre not at_end() (checked by assertion)
         \post either at_end() or the iterator points to the next valid (state, transition)
         \return this after increment
         */
        tchecker::ts::builder_ok_t<TS, ALLOCATOR>::iterator_t<TS_ITERATOR> & operator++ ()
        {
          assert( ! at_end() );
          _current_state = nullptr;
          _current_transition = nullptr;
          ++_it;
          skip_bad();
          return *this;
        }
        
        /*!
         \brief Dereference operator
         \pre not at_end() (checked by assertion)
         \return pair (state, transition) pointed by this iterator
         */
        std::tuple<state_ptr_t, transition_ptr_t> operator* () const
        {
          assert( ! at_end() );
          return std::make_tuple(_current_state, _current_transition);
        }
      private:
        /*!
         \brief Skip states with bad status
         \post either at_end() or _current_state is the next state with status tchecker::STATE_OK, and
         _current_transition is the corresponding transition. _it has been moved to point to
         (_current_state, _current_transition)
         */
        void skip_bad()
        {
          tchecker::state_status_t status;
          while (! _it.at_end()) {
            std::tie(_current_state, _current_transition, status) = _computer(*_it);
            if (status == tchecker::STATE_OK)
              break;
            ++_it;
          }
        }
        
        state_ptr_t _current_state;             /*!< State pointed by iterator */
        transition_ptr_t _current_transition;   /*!< Transition pointed by iterator */
        TS_ITERATOR _it;                        /*!< Iterator to curent position */
        state_transition_computer_t _computer;  /*!< State/transition computing function */
      };
      
      
      
      
      /*!
       \brief Type of iterator over initial (state, transition)
       */
      using initial_iterator_t = tchecker::ts::builder_ok_t<TS, ALLOCATOR>::iterator_t<typename TS::initial_iterator_t>;
      
      /*!
       \brief Accessor
       \return range over initial (state, transition)
       */
      tchecker::range_t<tchecker::ts::builder_ok_t<TS, ALLOCATOR>::initial_iterator_t> initial()
      {
        auto initial_range = tchecker::ts::builder_t<TS, ALLOCATOR>::_ts.initial();
        
        auto computer = [&] (typename TS::initial_iterator_value_t const & v) {
          return tchecker::ts::builder_t<TS, ALLOCATOR>::initial_state(v, std::make_tuple(), std::make_tuple());
        };
        
        return tchecker::make_range
        (tchecker::ts::builder_ok_t<TS, ALLOCATOR>::initial_iterator_t(initial_range.begin(), computer),
         tchecker::ts::builder_ok_t<TS, ALLOCATOR>::initial_iterator_t(initial_range.end(), computer));
      }
      
      
      
      
      /*!
       \brief Type of iterator over outgoing (state, transition)
       */
      using outgoing_iterator_t
      = tchecker::ts::builder_ok_t<TS, ALLOCATOR>::iterator_t<typename TS::outgoing_edges_iterator_t>;
      
      /*!
       \brief Accessor
       \param s : a state
       \return range over outgoing (state, transition) of s
       */
      tchecker::range_t<tchecker::ts::builder_ok_t<TS, ALLOCATOR>::outgoing_iterator_t> outgoing(state_ptr_t & s)
      {
        auto outgoing_range = tchecker::ts::builder_t<TS, ALLOCATOR>::_ts.outgoing_edges(*s);
        
        auto computer = [&] (typename TS::outgoing_edges_iterator_value_t const & v) {
          return tchecker::ts::builder_t<TS, ALLOCATOR>::next_state(s, v, std::make_tuple(), std::make_tuple());
        };
        
        return tchecker::make_range
        (tchecker::ts::builder_ok_t<TS, ALLOCATOR>::outgoing_iterator_t(outgoing_range.begin(), computer),
         tchecker::ts::builder_ok_t<TS, ALLOCATOR>::outgoing_iterator_t(outgoing_range.end(), computer));
      }
    };
    
  } // end of namespace ts
  
} // end of namespace tchecker

#endif // TCHECKER_TS_BUILDER_HH
