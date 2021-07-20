/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VEDGE_HH
#define TCHECKER_VEDGE_HH

#include <functional>
#include <iostream>

#include <boost/iterator/filter_iterator.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/system/system.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/array.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file vedge.h
 \brief Vectors of edges
 */

namespace tchecker {

/*!
 \brief Type of fixed capacity array of edges
 */
using edge_array_t =
    tchecker::make_array_t<tchecker::edge_id_t, sizeof(tchecker::edge_id_t), tchecker::array_capacity_t<unsigned int>>;

/*!
 \class vedge_t
 \brief Vector of edges
 \note The vector of edges is implemented as an array of edge identifiers. For each process P, the array either contains
 the identifier of the edge taken by P, or tchecker::NO_EDGE if P is not involved in this vector of edges. Hence, direct
 accesses to the array may give value tchecker::NO_EDGE which is not a valid edge identifier. On the other hand, iterators
 returned by methods begin() and end() range over valid edge identifiers.
 */
class vedge_t : public tchecker::edge_array_t {
public:
  /*!
   \brief Assignment operator
   */
  tchecker::vedge_t & operator=(tchecker::vedge_t const &) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::vedge_t & operator=(tchecker::vedge_t &&) = default;

  /*!
   \brief Accessor
   \return Size
   \note Size coincides with capacity
   */
  inline tchecker::edge_array_t::capacity_t size() const { return tchecker::edge_array_t::capacity(); }

  /*!
   \brief Construction
   \param args : arguments to a constructor of tchecker::vedge_t
   \pre ptr points to an allocated zone of size at least
   tchecker::allocations_size_t<tchecker::vedge_t>::alloc_size(args)
   \post vedge_t(args) has been called on ptr
   */
  template <class... ARGS> static inline void construct(void * ptr, ARGS &&... args) { new (ptr) tchecker::vedge_t(args...); }

  /*!
   \brief Destruction
   \param vedge : vector of edges
   \post ~vedge_t() has been called on vedge
   */
  static inline void destruct(tchecker::vedge_t * vedge)
  {
    assert(vedge != nullptr);
    vedge->~vedge_t();
  }

  /*!
   \brief Type of iterator over actual edge identifiers
   */
  using iterator_t = boost::filter_iterator<bool (*)(tchecker::edge_id_t), tchecker::edge_array_t::iterator_t>;

  /*!
   \brief Accessor
   \return iterator on first actual edge identifier
   */
  iterator_t begin();

  /*!
   \brief Accessor
   \return Past-the-end iterator on actual edge identifiers
   */
  iterator_t end();

  /*!
   \brief Type of const iterator over actual edge identifiers
   */
  using const_iterator_t = boost::filter_iterator<bool (*)(tchecker::edge_id_t), tchecker::edge_array_t::const_iterator_t>;

  /*!
   \brief Accessor
   \return const iterator on first actual edge identifier
   */
  const_iterator_t begin() const;

  /*!
   \brief Accessor
   \return Past-the-end const iterator on actual edge identifiers
   */
  const_iterator_t end() const;

  /*!
   \brief Type of iterator on the array of edge identifiers
   \note array iterators may dereference to actual edge identifier or to tchecker::NO_EDGE
   */
  using array_iterator_t = tchecker::edge_array_t::iterator_t;

  /*!
   \brief Accessor
   \return iterator on first element in the array of edge identifiers
   \note see method begin() for iterator on actual edge identifiers
  */
  array_iterator_t begin_array();

  /*!
   \brief Accessor
   \return Past-the-end iterator on the array of edge identifiers
   */
  array_iterator_t end_array();

  /*!
   \brief Type of const iterator on the array of edge identifiers
   \note array iterators may dereference to actual edge identifier or to tchecker::NO_EDGE
   */
  using const_array_iterator_t = tchecker::edge_array_t::const_iterator_t;

  /*!
   \brief Accessor
   \return const iterator on first element in the array of edge identifiers
   \note see method begin() for iterator on actual edge identifiers
  */
  const_array_iterator_t begin_array() const;

  /*!
   \brief Accessor
   \return Past-the-end const iterator on the array of edge identifiers
   */
  const_array_iterator_t end_array() const;

protected:
  /*!
   \brief Constructor
   \param size : size of vector of edges
   \post this is a vector of size edge identifiers
   */
  vedge_t(unsigned int size);

  /*!
   \brief Copy constructor
   */
  vedge_t(tchecker::vedge_t const &) = default;

  /*!
   \brief Move constructor
   */
  vedge_t(tchecker::vedge_t &&) = default;

  /*!
   \brief Destructor
   */
  ~vedge_t() = default;
};

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class tchecker::vedge_t
 */
template <> class allocation_size_t<tchecker::vedge_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::vedge_t
   \return allocation size for objects of class tchecker::vedge_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... args)
  {
    return tchecker::allocation_size_t<tchecker::edge_array_t>::alloc_size(args...);
  }
};

/*!
 \brief Allocate and construct a vector of edges
 \param size : vector size
 \param args : arguments to a constuctor of tchecker::vedge_t
 \return an instance of tchecker::vedge_t with size edge identifiers initialized from args
 */
template <class... ARGS> tchecker::vedge_t * vedge_allocate_and_construct(unsigned int size, ARGS &&... args)
{
  char * ptr = new char[tchecker::allocation_size_t<tchecker::vedge_t>::alloc_size(size)];
  tchecker::vedge_t::construct(ptr, args...);
  return reinterpret_cast<tchecker::vedge_t *>(ptr);
}

/*!
 \brief Destruct and deallocate a vector of edges
 \param vedge : vector of edges
 \pre vedge has been allocated by tchecker::vedge_allocate_and_construct
 \post vedge has been destructed and deallocated
 */
void vedge_destruct_and_deallocate(tchecker::vedge_t * vedge);

/*!
 \brief Output vector of edges
 \param os : output stream
 \param vedge : vector of edges
 \param system : a system
 \pre vedge contains identifiers of edges from system
 \post vedge has been output to os mapping edge identifiers to edges from system
 \return os after output
 */
std::ostream & output(std::ostream & os, tchecker::vedge_t const & vedge, tchecker::system::system_t const & system);

/*!
 \brief Output vector of edges to string
 \param vedge : vector of edges
 \param system : a system
 \return a string corresponding to output of vedge (see tchecker::output)
 */
std::string to_string(tchecker::vedge_t const & vedge, tchecker::system::system_t const & system);

/*!
 \brief Lexical ordering
 \param vedge1 : first tuple of edges
 \param vedge2 : second tuple of edges
 \return 0 if vedge1 and vedge2 are equal, a negative value if vedge1 is smaller than vedge2 w.r.t. lexical ordering, and a
 positive value otherwise
 */
int lexical_cmp(tchecker::vedge_t const & vedge1, tchecker::vedge_t const & vedge2);

/*!
 \brief Type of shared tuple of edges
 */
using shared_vedge_t = tchecker::make_shared_t<tchecker::vedge_t>;

} // end of namespace tchecker

#endif // TCHECKER_VEDGE_HH
