/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VLOC_HH
#define TCHECKER_VLOC_HH

#include <cassert>

#include "tchecker/basictypes.hh"
#include "tchecker/system/system.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/array.hh"
#include "tchecker/utils/cache.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file vloc.hh
 \brief Vectors of locations
 */

namespace tchecker {

/*!
 \class vloc_base_t
 \brief Base class for tuple of locations that extends array capacity with
 cache object
*/
class vloc_base_t : public tchecker::array_capacity_t<unsigned int>, public tchecker::cached_object_t {
public:
  using tchecker::array_capacity_t<unsigned int>::array_capacity_t;
};

/*!
 \brief Type of fixed capacity array of locations
 */
using loc_array_t = tchecker::make_array_t<tchecker::loc_id_t, sizeof(tchecker::loc_id_t), tchecker::vloc_base_t>;

/*!
 \class vloc_t
 \brief Vector of locations
 \note NO FIELD SHOULD BE ADDED TO THIS CLASS (either by definition or
 inheritance). See tchecker::make_array_t for details
 */
class vloc_t : public tchecker::loc_array_t {
public:
  /*!
   \brief Assignment operator
   */
  tchecker::vloc_t & operator=(tchecker::vloc_t const & vloc) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::vloc_t & operator=(tchecker::vloc_t && vloc) = default;

  /*!
   \brief Accessor
   \return Size
   \note Size coincides with capacity
   */
  inline tchecker::loc_array_t::capacity_t size() const { return tchecker::loc_array_t::capacity(); }

  /*!
   \brief Construction
   \param args : arguments to a constructor of tchecker::vloc_t
   \pre ptr points to an allocated zone of size at least
   tchecker::allocations_size_t<tchecker::vloc_t>::alloc_size(args)
   \post vloc_t(args) has been called on ptr
   */
  template <class... ARGS> static inline void construct(void * ptr, ARGS &&... args) { new (ptr) tchecker::vloc_t(args...); }

  /*!
   \brief Destruction
   \param vloc : vector of locations
   \post ~vloc_t() has been called on vloc
   */
  static inline void destruct(tchecker::vloc_t * vloc)
  {
    assert(vloc != nullptr);
    vloc->~vloc_t();
  }

protected:
  /*!
   \brief Constructor
   \param size : size of vector of locations
   \post this is a vector of size location identifiers
   */
  vloc_t(unsigned int size);

  /*!
   \brief Copy constructor
   */
  vloc_t(tchecker::vloc_t const &) = default;

  /*!
   \brief Move constructor
   */
  vloc_t(tchecker::vloc_t &&) = default;

  /*!
   \brief Destructor
   */
  ~vloc_t() = default;
};

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class tchecker::vloc_t
 */
template <> class allocation_size_t<tchecker::vloc_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::vloc_t
   \return allocation size for objects of class tchecker::vloc_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... args)
  {
    return tchecker::allocation_size_t<tchecker::loc_array_t>::alloc_size(args...);
  }
};

/*!
 \brief Allocate and construct a vector of locations
 \param size : vector size
 \param args : arguments to a constructor of tchecker::vloc_t
 \return an instance of tchecker::vloc_t with size location identifiers initialized from args
 */
template <class... ARGS> tchecker::vloc_t * vloc_allocate_and_construct(unsigned int size, ARGS &&... args)
{
  char * ptr = new char[tchecker::allocation_size_t<tchecker::vloc_t>::alloc_size(size)];
  tchecker::vloc_t::construct(ptr, args...);
  return reinterpret_cast<tchecker::vloc_t *>(ptr);
}

/*!
 \brief Destruct and deallocate a vector of locations
 \param vloc : vector of locations
 \pre vloc has been allocated by tchecker::vloc_allocate_and_construct
 \post vloc has been destructed and deallocated
 */
void vloc_destruct_and_deallocate(tchecker::vloc_t * vloc);

/*!
 \brief Output vector of locations
 \param os : output stream
 \param vloc : vector of locations
 \param system : a system
 \pre vloc contains identifiers of locations from system
 \post vloc has been output to os mapping location identifiers to names from system
 \return os after output
 */
std::ostream & output(std::ostream & os, tchecker::vloc_t const & vloc, tchecker::system::system_t const & system);

/*!
 \brief Output vector of locations to string
 \param vloc : vector of locations
 \param system : a system
 \return a string corresponding to output of vloc (see tchecker::output)
 */
std::string to_string(tchecker::vloc_t const & vloc, tchecker::system::system_t const & system);

/*!
 \brief Initialize a vector of locations from a string
 \param vloc : vector of locations
 \param system : a system
 \param str : a string
 \pre str is a comma-separated list of location names, starting with < and ending with >
 str contains exactly one location for each process in system
 \post the locations in vloc have been updated according to str
 \throw std::invalid_argument : if str is not syntactically correct,
 or if str does not contain exactly one location per process,
 or if one of the location does not belong to the corresponding process,
 */
void from_string(tchecker::vloc_t & vloc, tchecker::system::system_t const & system, std::string const & str);

/*!
 \brief Lexical ordering
 \param vloc1 : first tuple of locations
 \param vloc2 : second tuple of locations
 \return 0 if vloc1 and vloc2 are equal, a negative value if vloc1 is smaller than vloc2 w.r.t. lexical ordering, and a positive
 value otherwise
 */
int lexical_cmp(tchecker::vloc_t const & vloc1, tchecker::vloc_t const & vloc2);

/*!
 \brief Type of shared tuple of locations
 */
using shared_vloc_t = tchecker::make_shared_t<tchecker::vloc_t>;

/*!
 \brief Type of shared pointer to a tuple of locations
*/
using vloc_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t>;

/*!
 \brief Type of shared pointer to a const tuple of locations
*/
using const_vloc_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const>;

} // end of namespace tchecker

#endif // TCHECKER_VLOC_HH
