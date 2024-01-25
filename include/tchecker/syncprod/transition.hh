/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYNCPROD_TRANSITION_HH
#define TCHECKER_SYNCPROD_TRANSITION_HH

#include "tchecker/syncprod/vedge.hh"
#include "tchecker/ts/transition.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file transition.hh
 \brief Transitions for synchronized product of timed processes
 */

namespace tchecker {

namespace syncprod {

/*!
 \class transition_t
 \brief Transition for synchronized product of timed processes
 */
class transition_t : public tchecker::ts::transition_t {
public:
  /*!
   \brief Constructor
   \param vedge : tuple of edges
   \pre vedge must not point to nullptr (checked by assertion)
   */
  explicit transition_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge);

  /*!
   \brief Partial copy constructor
   \param t : a transition
   \param vedge : tuple of edges
   \pre vedge must not point to nullptr (checked by assertion)
   \note the transition is copied from t, except the tuple of edges which is initialized from vedge
   */
  transition_t(tchecker::syncprod::transition_t const & t,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge);

  /*!
   \brief Copy constructor (deleted)
   */
  transition_t(tchecker::syncprod::transition_t const &) = delete;

  /*!
   \brief Move constructor
   */
  transition_t(tchecker::syncprod::transition_t &&) = default;

  /*!
   \brief Destructor
   */
  ~transition_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::syncprod::transition_t & operator=(tchecker::syncprod::transition_t const &) = delete;

  /*!
   \brief Move-assignment operator
   */
  tchecker::syncprod::transition_t & operator=(tchecker::syncprod::transition_t &&) = default;

  /*!
   \brief Accessor
   \return tuple of edges
   */
  constexpr inline tchecker::vedge_t const & vedge() const { return *_vedge; }

  /*!
   \brief Accessor
   \return reference to pointer to tuple of edges
   */
  inline tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> & vedge_ptr() { return _vedge; }

  /*!
   \brief Accessor
   \return pointer to const tuple of edges
   */
  inline tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t const> vedge_ptr() const
  {
    return tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t const>(_vedge.ptr());
  }

protected:
  tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> _vedge; /*!< Tuple of edges */
};

/*!
 \brief Equality check
 \param t1 : transition
 \param t2 : transition
 \return true if t1 and t2 have same tuple of edges, false otherwise
 */
bool operator==(tchecker::syncprod::transition_t const & t1, tchecker::syncprod::transition_t const & t2);

/*!
 \brief Disequality check
 \param t1 : transition
 \param t2 : transition
 \return true if t1 and t2 have different tuples of edges, false otherwise
 */
bool operator!=(tchecker::syncprod::transition_t const & t1, tchecker::syncprod::transition_t const & t2);

/*!
 \brief Equality check for shared transitions
 \param t1 : transition
 \param t2 : transition
 \return true if t1 and t2 have same tuple of edges, false otherwise
 \note note this should only be used on transitions that have shared internal components: this
 function checks pointer equality (not values equality)
*/
bool shared_equal_to(tchecker::syncprod::transition_t const & t1, tchecker::syncprod::transition_t const & t2);

/*!
 \brief Hash
 \param t : transition
 \return Hash value for transition t
 */
std::size_t hash_value(tchecker::syncprod::transition_t const & t);

/*!
 \brief Hash for shared transitions
 \param t : transition
 \return Hash value for transition t
 \note note this should only be used on transitions that have shared internal components: this function
 hashes the pointers (not the values)
 */
std::size_t shared_hash_value(tchecker::syncprod::transition_t const & t);

/*!
 \brief Lexical ordering on transitions of synchronized product of timed processes
 \param t1 : first transition
 \param t2 : second transition
 \return 0 if t1 and t2 are equal, a negative value if t1 is smaller than t2 w.r.t. lexical ordering on tuple of edges, a
 positive value otherwise
 */
int lexical_cmp(tchecker::syncprod::transition_t const & t1, tchecker::syncprod::transition_t const & t2);

/*!
 \brief Type of shared transition
 */
using shared_transition_t = tchecker::make_shared_t<tchecker::syncprod::transition_t>;

/*!
\brief Type of pointer to shared transition
*/
using transition_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::syncprod::shared_transition_t>;

/*!
\brief Type of pointer to shared const transition
*/
using const_transition_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::syncprod::shared_transition_t const>;

} // end of namespace syncprod

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class tchecker::syncprod::transition_t
 */
template <> class allocation_size_t<tchecker::syncprod::transition_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::syncprod::transition_t
   \return allocation size for objects of class tchecker::syncprod::transition_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... /*args*/)
  {
    return sizeof(tchecker::syncprod::transition_t);
  }
};

} // end of namespace tchecker

#endif // TCHECKER_SYNCPROD_TRANSITION_HH
