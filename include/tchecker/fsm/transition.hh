/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_TRANSITION_HH
#define TCHECKER_FSM_TRANSITION_HH

#include "tchecker/syncprod/transition.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file transition.hh
 \brief Transitions for finite state machines
 */

namespace tchecker {

namespace fsm {

/*!
 \class transition_t
 \brief Transition for finite state machines
 */
class transition_t : public tchecker::syncprod::transition_t {
public:
  using tchecker::syncprod::transition_t::transition_t;
};

/*!
 \brief Equality check
 \param t1 : transition
 \param t2 : transition
 \return true if t1 and t2 have same tuple of edges, false otherwise
 */
bool operator==(tchecker::fsm::transition_t const & t1, tchecker::fsm::transition_t const & t2);

/*!
 \brief Disequality check
 \param t1 : transition
 \param t2 : transition
 \return true if t1 and t2 have different tuples of edges, false otherwise
 */
bool operator!=(tchecker::fsm::transition_t const & t1, tchecker::fsm::transition_t const & t2);

/*!
 \brief Equality check for shared transitions
 \param t1 : transition
 \param t2 : transition
 \return true if t1 and t2 have same tuple of edges, false otherwise
 \note note this should only be used on transitions that have shared internal components: this
 function checks pointer equality (not values equality)
*/
bool shared_equal_to(tchecker::fsm::transition_t const & t1, tchecker::fsm::transition_t const & t2);

/*!
 \brief Hash
 \param t : transition
 \return Hash value for transition t
 */
std::size_t hash_value(tchecker::fsm::transition_t const & t);

/*!
 \brief Hash for shared transitions
 \param t : transition
 \return Hash value for transition t
 \note note this should only be used on transitions that have shared internal components: this function
 hashes the pointers (not the values)
 */
std::size_t shared_hash_value(tchecker::fsm::transition_t const & t);

/*!
 \brief Lexical ordering on transitions of finite state machines
 \param t1 : first transition
 \param t2 : second transition
 \return 0 if t1 and t2 are equal, a negative value if t1 is smaller than t2 w.r.t. lexical ordering on tuple of edges, a
 positive value otherwise
 */
int lexical_cmp(tchecker::fsm::transition_t const & t1, tchecker::fsm::transition_t const & t2);

/*!
 \brief Type of shared transition
 */
using shared_transition_t = tchecker::make_shared_t<tchecker::fsm::transition_t>;

/*!
\brief Type of pointer to shared transition
*/
using transition_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::fsm::shared_transition_t>;

/*!
\brief Type of pointer to shared const transition
*/
using const_transition_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::fsm::shared_transition_t const>;

} // end of namespace fsm

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class tchecker::fsm::transition_t
 */
template <> class allocation_size_t<tchecker::fsm::transition_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::fsm::transition_t
   \return allocation size for objects of class tchecker::fsm::transition_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... /*args*/)
  {
    return sizeof(tchecker::fsm::transition_t);
  }
};

} // end of namespace tchecker

#endif // TCHECKER_FSM_TRANSITION_HH
