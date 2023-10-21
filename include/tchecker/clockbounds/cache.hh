/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_CLOCKBOUNDS_CACHE_HH
#define TCHECKER_CLOCKBOUNDS_CACHE_HH

#include <memory>
#include <tuple>
#include <unordered_map>

#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/syncprod/vloc.hh"

/*!
 \file cache.hh
 \brief Cache for clock bounds
 */

namespace tchecker {

namespace clockbounds {

/*!
 \class cached_local_lu_map_t
 \brief Cached access to local LU bounds of tuple of locations
 \tparam HASH : type of hash functor of tchecker::const_vloc_sptr_t to std::size_t
 \tparam EQUAL : type of equality functor over tchecker::const_vloc_sptr_t
 */
template <class HASH, class EQUAL> class cached_local_lu_map_t {
  /*!< Type of LU clock bounds maps for storing */
  struct lu_maps_t {
    tchecker::clockbounds::map_t * L; /*!< L bounds map */
    tchecker::clockbounds::map_t * U; /*!< U bounds map */
  };

public:
  /*!
   \brief Constructor
   \param local_lu : local LU clock bounds map
   \param hash : hash functor from tchecker::const_vloc_sptr_t to std::size_t
   \param equal : equality functor over tchecker::const_vloc_sptr_t
   \throw std::invalid_argument : if local_lu points to nullptr
   \note this keeps a shared pointer to local_lu, and creates a copy of hash and a copy of equal
   */
  cached_local_lu_map_t(std::shared_ptr<tchecker::clockbounds::local_lu_map_t> local_lu, HASH const & hash, EQUAL const & equal)
      : _local_lu(local_lu), _hash(hash), _equal(equal)
  {
    if (_local_lu.get() == nullptr)
      throw std::invalid_argument("cached_local_lu_map_t: invalid nullptr pointer");
  }

  /*!
   \brief Copy constructor
   */
  cached_local_lu_map_t(tchecker::clockbounds::cached_local_lu_map_t<HASH, EQUAL> const & m)
      : _local_lu(m._local_lu), _hash(m._hash), _equal(m._equal)
  {
    insert(m._cache);
  }

  /*!
   \brief Move constructor
   */
  cached_local_lu_map_t(tchecker::clockbounds::cached_local_lu_map_t<HASH, EQUAL> &&) = default;

  /*!
   \brief Destructor
   */
  ~cached_local_lu_map_t() { clear(); }

  /*!
   \brief Assignment operator
   */
  tchecker::clockbounds::cached_local_lu_map_t<HASH, EQUAL> &
  operator=(tchecker::clockbounds::cached_local_lu_map_t<HASH, EQUAL> const & m)
  {
    if (this != &m) {
      clear();
      _local_lu = m._local_lu;
      _hash = m._hash;
      _equal = m._equal;
      insert(m._cache);
    }
    return *this;
  }

  /*!
   \brief Move-assignment operator
   */
  tchecker::clockbounds::cached_local_lu_map_t<HASH, EQUAL> &
  operator=(tchecker::clockbounds::cached_local_lu_map_t<HASH, EQUAL> &&) = default;

  /*!
   \brief Clear the cache
   */
  void clear()
  {
    auto end = _cache.end();
    for (auto it = _cache.begin(); it != end; ++it) {
      lu_maps_t lu_maps = (*it).second;
      tchecker::clockbounds::deallocate_map(lu_maps.L);
      tchecker::clockbounds::deallocate_map(lu_maps.U);
    }
    _cache.clear();
  }

  /*!
   \brief Type of reference to LU clock bounds maps
   */
  struct lu_maps_reference_t {
    tchecker::clockbounds::map_t const & L; /*!< L bounds */
    tchecker::clockbounds::map_t const & U; /*!< U bounds */
  };

  /*!
   \brief Accessor
   \param vloc : tuple of locations
   \return LU clock bounds maps for vloc
   \note the returned maps are references to the cache that will be invalidated if the
   cache is cleared
   */
  lu_maps_reference_t bounds(tchecker::const_vloc_sptr_t const & vloc)
  {
    auto it = _cache.find(vloc);

    // vloc LU bounds are already in the cache
    if (it != _cache.end()) {
      lu_maps_t lu = (*it).second;
      return lu_maps_reference_t{.L = *lu.L, .U = *lu.U};
    }

    // vloc LU bounds are not yet in the cache
    lu_maps_t lu_maps{.L = tchecker::clockbounds::allocate_map(_local_lu->clock_number()),
                      .U = tchecker::clockbounds::allocate_map(_local_lu->clock_number())};
    _local_lu->bounds(*vloc, *lu_maps.L, *lu_maps.U);
    _cache.insert(std::make_pair(vloc, lu_maps));

    return lu_maps_reference_t{.L = *lu_maps.L, .U = *lu_maps.U};
  }

private:
  /*!
   \brief Insert a cache into this cache
   \param cache : a cache
   \post all elements from cache have been inserted in this cache: shared pointers
   to tuple of locations are shared, whereas clock bounds are cloned
   */
  void insert(std::unordered_map<tchecker::const_vloc_sptr_t, lu_maps_t, HASH, EQUAL> const & cache)
  {
    auto end = cache.end();
    for (auto it = cache.begin(); it != end; ++it) {
      tchecker::const_vloc_sptr_t vloc = (*it).first;
      lu_maps_t lu = (*it).second;
      lu_maps_t lu_copy{.L = tchecker::clockbounds::clone_map(*lu.L), .U = tchecker::clockbounds::clone_map(*lu.U)};
      _cache.insert(std::make_pair(vloc, lu_copy));
    }
  }

  std::shared_ptr<tchecker::clockbounds::local_lu_map_t> _local_lu; /*!< Local LU map */
  HASH _hash;                                                       /*!< Hash functor over tuple of locations */
  EQUAL _equal;                                                     /*!< Equality functor over tuples of locations */
  std::unordered_map<tchecker::const_vloc_sptr_t, lu_maps_t, HASH, EQUAL> _cache; /*!< Cache of LU maps */
};

} // namespace clockbounds

} // namespace tchecker

#endif // TCHECKER_CLOCKBOUNDS_CACHE_HH
