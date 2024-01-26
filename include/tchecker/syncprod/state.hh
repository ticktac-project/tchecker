/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYNCPROD_STATE_HH
#define TCHECKER_SYNCPROD_STATE_HH

#include "tchecker/syncprod/vloc.hh"
#include "tchecker/ts/state.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file state.hh
 \brief States for synchronized product of timed processes
 */

namespace tchecker {

namespace syncprod {

/*!
 \class state_t
 \brief State for synchronized product of timed processes
 */
class state_t : public tchecker::ts::state_t {
public:
  /*!
   \brief Constructor
   \param vloc : tuple of locations
   \pre vloc must not point to nullptr (checked by assertion)
   \note this keeps a pointer to vloc
   */
  explicit state_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc);

  /*!
   \brief Partial copy constructor
   \param state : a state
   \param vloc : tuple of locations
   \pre vloc must not point to nullptr (checked by assertion)
   \note this keeps a pointer to vloc
   \note the state is copied from s, except the tuple of locations which is initialized from vloc
   */
  state_t(tchecker::syncprod::state_t const & s, tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc);

  /*!
   \brief Copy constructor (deleted)
   */
  state_t(tchecker::syncprod::state_t const &) = delete;

  /*!
   \brief Move constructor
   */
  state_t(tchecker::syncprod::state_t &&) = default;

  /*!
   \brief Destructor
   */
  ~state_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::syncprod::state_t & operator=(tchecker::syncprod::state_t const &) = delete;

  /*!
   \brief Move-assignment operator
   */
  tchecker::syncprod::state_t & operator=(tchecker::syncprod::state_t &&) = default;

  /*!
   \brief Accessor
   \return tuple of locations
   */
  constexpr inline tchecker::vloc_t const & vloc() const { return *_vloc; }

  /*!
   \brief Accessor
   \return reference to pointer to tuple of locations
   */
  inline tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> & vloc_ptr() { return _vloc; }

  /*!
   \brief Accessor
   \return pointer to const tuple of locations
   */
  inline tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> vloc_ptr() const
  {
    return tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const>(_vloc.ptr());
  }

protected:
  tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> _vloc; /*!< Tuple of locations */
};

/*!
 \brief Equality check
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have same tuple of locations, false otherwise
 */
bool operator==(tchecker::syncprod::state_t const & s1, tchecker::syncprod::state_t const & s2);

/*!
 \brief Disequality check
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have different tuples of locations, false otherwise
 */
bool operator!=(tchecker::syncprod::state_t const & s1, tchecker::syncprod::state_t const & s2);

/*!
 \brief Equality check for shared states
 \param s1 : state
 \param s2 : state
 \return true if s1 and s2 have same tuple of locations, false otherwise
 \note this should only be used on states that have shared internal components: this
 function checks pointer equality (not values equality)
*/
bool shared_equal_to(tchecker::syncprod::state_t const & s1, tchecker::syncprod::state_t const & s2);

/*!
 \brief Hash
 \param s : state
 \return Hash value for state s
 */
std::size_t hash_value(tchecker::syncprod::state_t const & s);

/*!
 \brief Hash for shared states
 \param s : state
 \return Hash value for state s
 \note this should only be used on states that have shared internal components: this function
 hashes the pointers (not the values)
 */
std::size_t shared_hash_value(tchecker::syncprod::state_t const & s);

/*!
 \brief Lexical ordering on states of synchronized product of timed processes
 \param s1 : a state
 \param s2 : a state
 \return 0 if s1 and s2 are equal, a negative value if s1 is smaller than s2 w.r.t. lexical ordering on tuple of locations, a
 positive value otherwise
 */
int lexical_cmp(tchecker::syncprod::state_t const & s1, tchecker::syncprod::state_t const & s2);

/*!
 \brief Type of shared state
 */
using shared_state_t = tchecker::make_shared_t<tchecker::syncprod::state_t>;

/*!
\brief Type of pointer to shared state
*/
using state_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::syncprod::shared_state_t>;

/*!
\brief Type of pointer to shared const state
*/
using const_state_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::syncprod::shared_state_t const>;

} // end of namespace syncprod

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class tchecker::syncprod::state_t
 */
template <> class allocation_size_t<tchecker::syncprod::state_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::syncprod::state_t
   \return allocation size for objects of class tchecker::syncprod::state_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... /*args*/)
  {
    return sizeof(tchecker::syncprod::state_t);
  }
};

} // end of namespace tchecker

#endif // TCHECKER_SYNCPROD_STATE_HH
