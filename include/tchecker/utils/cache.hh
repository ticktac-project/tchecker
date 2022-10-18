/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_CACHE_HH
#define TCHECKER_CACHE_HH

/*!
 \file cache.hh
 \brief Cache of shared objects
 */

#include <limits>
#include <vector>

#include "tchecker/utils/hashtable.hh"
#include "tchecker/utils/pool.hh"

namespace tchecker {

/*!
 \brief Type of cached objects
 */
using cached_object_t = tchecker::hashtable_object_t;

/*!
 \class cache_t
 \brief Cache of shared objects with collection of unused objects
 \tparam SPTR : type of pointer to stored objects. Must be a shared
 pointer tchecker::intrusive_shared_ptr_t<...> to an object that derives from
 tchecker::cached_object_t
 \tparam HASH : hash function over shared pointers of type SPTR, must be default
 constructible
 \tparam EQUAL : equality predicate over shared pointers of type SPTR, must be
 default constructible
 \note stored objects should derive from tchecker::cached_object_t
 */
template <class SPTR, class HASH, class EQUAL> class cache_t : public tchecker::collectable_t {
public:
  /*!
   \brief Constructor
   \param table_size : size of the hash table
   */
  cache_t(std::size_t table_size = 65536) : _hashtable(table_size, _hash, _equal) {}

  /*!
   \brief Copy-construction
   */
  cache_t(tchecker::cache_t<SPTR, HASH, EQUAL> const &) = default;

  /*!
   \brief Move-construction
   */
  cache_t(tchecker::cache_t<SPTR, HASH, EQUAL> &&) = default;

  /*!
   \brief Destructor
   */
  virtual ~cache_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::cache_t<SPTR, HASH, EQUAL> & operator=(tchecker::cache_t<SPTR, HASH, EQUAL> const &) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::cache_t<SPTR, HASH, EQUAL> & operator=(tchecker::cache_t<SPTR, HASH, EQUAL> &&) = default;

  /*!
   \brief Object caching
   \param o : object
   \return object equivalent to o (w.r.t. HASH and EQUAL) in this cache if any,
   o itself if no equivalent object was in the cache before
   \post o has been inserted in this cache if no equivalent object (w.r.t. HASH
   and EQUAL) was in the cache before
   */
  inline SPTR find_else_add(SPTR const & o) { return _hashtable.find_else_add(o); }

  /*!
   \brief Membership predicate
   \param o : object
   \return true if this cache constains an object equivalent to o (w.r.t. HASH
   and EQUAL), false otherwise
   */
  inline bool find(SPTR const & o)
  {
    auto && [found, p] = _hashtable.find(o);
    return found;
  }

  /*!
   \brief Clear the cache
   \post This cache is empty
   */
  inline void clear() { _hashtable.clear(); }

  /*!
   \brief Garbage collection
   \post All objects with reference counter 1 (i.e. objects with no reference
   outside of this cache) have been removed from this cache
   \return number of collected objects
   */
  virtual std::size_t collect()
  {
    std::size_t const previous_size = _hashtable.size();
    typename tchecker::hashtable_t<SPTR, HASH, EQUAL>::iterator_t it = _hashtable.begin();
    while (it != _hashtable.end()) {
      if ((*it)->refcount() == 1)
        it = _hashtable.remove(it);
      else
        ++it;
    }
    return previous_size - _hashtable.size();
  }

  /*!
   \brief Accessor
   \return Number of objects in the cache
   */
  inline std::size_t size() const { return _hashtable.size(); }

private:
  HASH _hash;                                          /*! Hash function */
  EQUAL _equal;                                        /*!< Equality predicate */
  tchecker::hashtable_t<SPTR, HASH, EQUAL> _hashtable; /*!< Table of stored objects */
};

/*!
 \class periodic_collectable_cache_t
 \brief Cache of shared objects with collection of unused objects. When
 collection is run and no object is collected, the period between two
 collections grows exponentially
 \tparam SPTR : type of pointer to stored objects. Must be a shared
 pointer tchecker::intrusive_shared_ptr_t<...> to an object that derives from
 tchecker::cached_object_t
 \tparam HASH : hash function over shared pointers of type SPTR, must be default
 constructible
 \tparam EQUAL : equality predicate over shared pointers of type SPTR, must be
 default constructible
 \note stored objects should derive from tchecker::cached_object_t
 */
template <class SPTR, class HASH, class EQUAL>
class periodic_collectable_cache_t : public tchecker::cache_t<SPTR, HASH, EQUAL> {
public:
  /*!
   \brief Constructor
   \param table_size : size of the hash table
   */
  periodic_collectable_cache_t(std::size_t table_size = 65536)
      : tchecker::cache_t<SPTR, HASH, EQUAL>(table_size), _period(1), _count(1)
  {
  }

  /*!
   \brief Copy-construction
   */
  periodic_collectable_cache_t(tchecker::periodic_collectable_cache_t<SPTR, HASH, EQUAL> const &) = default;

  /*!
   \brief Move-construction
   */
  periodic_collectable_cache_t(tchecker::periodic_collectable_cache_t<SPTR, HASH, EQUAL> &&) = default;

  /*!
   \brief Destructor
   */
  virtual ~periodic_collectable_cache_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::periodic_collectable_cache_t<SPTR, HASH, EQUAL> &
  operator=(tchecker::periodic_collectable_cache_t<SPTR, HASH, EQUAL> const &) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::periodic_collectable_cache_t<SPTR, HASH, EQUAL> &
  operator=(tchecker::periodic_collectable_cache_t<SPTR, HASH, EQUAL> &&) = default;

  /*!
   \brief Garbage collection
   \post If period between last collection has been reached, then
   all objects with reference counter 1 (i.e. objects with no reference
   outside of this cache) have been removed from this cache
   Otherwise no collection occured
   \post If collection occurred and collected no object, then period has
   been doubled
   \return number of collected objects
   */
  virtual std::size_t collect()
  {
    if (_count >= _period) {
      std::size_t n = tchecker::cache_t<SPTR, HASH, EQUAL>::collect();
      if (n > 0)
        _period = 1;
      else if (_period < std::numeric_limits<std::size_t>::max() / 2)
        _period *= 2;
      _count = 1;
      return n;
    }

    ++_count;
    return 0;
  }

private:
  std::size_t _period; /*!< Period between two collections */
  std::size_t _count;  /*!< Time from last collection */
};

} // end of namespace tchecker

#endif // TCHECKER_CACHE_HH
