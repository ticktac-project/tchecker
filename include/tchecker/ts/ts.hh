/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_HH
#define TCHECKER_TS_HH

#include <type_traits>

#include "tchecker/basictypes.hh"
#include "tchecker/ts/state.hh"
#include "tchecker/ts/transition.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file ts.hh
 \brief Interface to transition system
 */

namespace tchecker {
  
  namespace ts {
    
    /*!
     \class ts_t
     \brief Transition system
     \tparam STATE : type of state, should derive from tchecker::ts::state_t
     \tparam TRANSITION : type of transition, should derive from tchecker::transition_t
     \tparam INITIAL_ITERATOR : type of iterator over initial states
     \tparam OUTGOING_EDGES_ITERATOR : type of iterator over outgoing edges
     \tparam INITIAL_ITERATOR_VALUE : type of value for INITIAL_ITERATOR
     \tparam OUTGING_EDGES_ITERATOR_VALUE : type of value for OUTGOING_EDGES_ITERATOR
     */
    template <
    class STATE,
    class TRANSITION,
    class INITIAL_ITERATOR,
    class OUTGOING_EDGES_ITERATOR,
    class INITIAL_ITERATOR_VALUE=typename std::iterator_traits<INITIAL_ITERATOR>::reference_type const,
    class OUTGOING_EDGES_ITERATOR_VALUE=typename std::iterator_traits<OUTGOING_EDGES_ITERATOR>::reference_type const
    >
    class ts_t {
      
      static_assert(std::is_base_of<tchecker::ts::state_t, STATE>::value, "");
      static_assert(std::is_base_of<tchecker::ts::transition_t, TRANSITION>::value, "");
      
    public:
      /*!
       \brief Type of state
       */
      using state_t = STATE;
      
      /*!
       \brief Type of transition
       */
      using transition_t = TRANSITION;
      
      /*!
       \brief Type of iterator over initial states
       */
      using initial_iterator_t = INITIAL_ITERATOR;
      
      /*!
       \brief Value type for iterator over initial states
       */
      using initial_iterator_value_t = INITIAL_ITERATOR_VALUE;
      
      /*!
       \brief Type of iterator over outgoing edges
       */
      using outgoing_edges_iterator_t = OUTGOING_EDGES_ITERATOR;
      
      /*!
       \brief Value type for iterator over outgoing edges
       */
      using outgoing_edges_iterator_value_t = OUTGOING_EDGES_ITERATOR_VALUE;
      
      /*!
       \brief Destructor
       */
      virtual ~ts_t() = default;
      
      /*!
       \brief Accessor
       \return Initial state valuations
       */
      virtual tchecker::range_t<INITIAL_ITERATOR> initial() = 0;
      
      /*!
       \brief Initialize state
       \param s : state
       \param t : transition
       \param v : initial state valuation
       \post state s and transtion t have been initialized from v
       \return status of state s after initialization
       \note t represents an initial transition to s
       */
      virtual enum tchecker::state_status_t initialize(STATE & s,
                                                       TRANSITION & t,
                                                       INITIAL_ITERATOR_VALUE const & v) = 0;
      
      /*!
       \brief Accessor
       \param s : state
       \return outgoing edges from state s
       */
      virtual tchecker::range_t<OUTGOING_EDGES_ITERATOR> outgoing_edges(STATE const & s) = 0;
      
      /*!
       \brief Next state computation
       \param s : state
       \param t : transition
       \param v : outgoing edge value
       \post s and t have been updated from v
       \return status of state s after update
       */
      virtual enum tchecker::state_status_t next(STATE & s, TRANSITION & t, OUTGOING_EDGES_ITERATOR_VALUE const & v) = 0;
    };
    
  } // end of namespace ts
  
} // end of namespace tchecker

#endif // TCHECKER_TS_HH
