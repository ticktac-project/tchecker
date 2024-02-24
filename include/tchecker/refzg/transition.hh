/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_REFZG_TRANSITION_HH
#define TCHECKER_REFZG_TRANSITION_HH

#include "tchecker/ta/transition.hh"
#include "tchecker/utils/allocation_size.hh"

/*!
 \file transition.hh
 \brief Transitions for zone graphs with reference clocks
 */

namespace tchecker {

namespace refzg {

/*!
 \class transition_t
 \brief Transition for zone graphs with reference clocks
 */
class transition_t : public tchecker::ta::transition_t {
public:
  /*!
   \brief Constructor
   \param sync_id synchronization identifier
   \param vedge : tuple of edges
   \pre vedge must not point to nullptr (checked by assertion)
   \pre if sync_id is tchecker::NO_SYNC then vedge is a single edge (checked by assertion)
   \pre if sync_id is not tchecker::NO_SYNC, then vedge is an instance of sync_id
   */
  explicit transition_t(tchecker::sync_id_t sync_id, tchecker::vedge_sptr_t const & vedge);

  /*!
   \brief Partial copy constructor
   \param t : a transition
   \param sync_id : synchronization identifier
   \param vedge : tuple of edges
   \pre vedge must not point to nullptr (checked by assertion)
   \pre if sync_id is tchecker::NO_SYNC then vedge is a single edge (checked by assertion)
   \pre if sync_id is not tchecker::NO_SYNC, then vedge is an instance of sync_id
   \note the transition is copied from t, except the synchronization identifier which is set to sync_id,
   and the tuple of edges which is initialized from vedge
   */
  transition_t(tchecker::refzg::transition_t const & t, tchecker::sync_id_t sync_id, tchecker::vedge_sptr_t const & vedge);

  /*!
   \brief Copy constructor (deleted)
   */
  transition_t(tchecker::refzg::transition_t const &) = delete;

  /*!
   \brief Move constructor
   */
  transition_t(tchecker::refzg::transition_t &&) = default;

  /*!
   \brief Destructor
   */
  ~transition_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::refzg::transition_t & operator=(tchecker::refzg::transition_t const &) = delete;

  /*!
   \brief Move-assignment operator
   */
  tchecker::refzg::transition_t & operator=(tchecker::refzg::transition_t &&) = default;
};

/*!
 \brief Equality check
 \param t1 : transition
 \param t2 : transition
 \return see tchecker::ta::operator== on transitions
 */
bool operator==(tchecker::refzg::transition_t const & t1, tchecker::refzg::transition_t const & t2);

/*!
 \brief Disequality check
 \param t1 : transition
 \param t2 : transition
 \return see tchecker::ta::operator!= on transitions
 */
inline bool operator!=(tchecker::refzg::transition_t const & t1, tchecker::refzg::transition_t const & t2)
{
  return !(t1 == t2);
}

/*!
 \brief Equality check for shared transitions
 \param t1 : transition
 \param t2 : transition
 \return see tchecker::ta::operator== on transitions
 \note note this should only be used on transitions that have shared internal components: this
 function checks pointer equality (not values equality)
 */
bool shared_equal_to(tchecker::refzg::transition_t const & t1, tchecker::refzg::transition_t const & t2);

/*!
 \brief Hash
 \param t : transition
 \return Hash value for transition t
 */
std::size_t hash_value(tchecker::refzg::transition_t const & t);

/*!
 \brief Hash for shared transitions
 \param t : transition
 \return Hash value for transition t
 \note note this should only be used on transitions that have shared internal components: this function
 hashes the pointers (not the values)
 */
std::size_t shared_hash_value(tchecker::refzg::transition_t const & t);

/*!
 \brief Lexical ordering on transitions of the local-time zone graph
 \param t1 : first transition
 \param t2 : second transition
 \return 0 if t1 and t2 are equal, a negative value if t1 is smaller than t2
 w.r.t. lexical ordering on tuple of edges, source invariant, guard, reset,
 target invariant, a positive value otherwise
 */
int lexical_cmp(tchecker::refzg::transition_t const & t1, tchecker::refzg::transition_t const & t2);

/*!
 \brief Type of shared transition
 */
using shared_transition_t = tchecker::make_shared_t<tchecker::refzg::transition_t>;

/*!
\brief Type of pointer to shared transition
*/
using transition_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_transition_t>;

/*!
\brief Type of pointer to shared const transition
*/
using const_transition_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_transition_t const>;

} // end of namespace refzg

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class tchecker::refzg::transition_t
 */
template <> class allocation_size_t<tchecker::refzg::transition_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::refzg::transition_t
   \return allocation size for objects of class tchecker::refzg::transition_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... args)
  {
    return sizeof(tchecker::refzg::transition_t);
  }
};

} // end of namespace tchecker

#endif // TCHECKER_REFZG_TRANSITION_HH
