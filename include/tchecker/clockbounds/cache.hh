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
 \class cache_local_lu_map_t
 \brief Access to local LU bounds of tuple of locations with cache
 \tparam HASH : type of hash functor of tchecker::const_vloc_sptr_t to std::size_t
 \tparam EQUAL : type of equality functor over tchecker::const_vloc_sptr_t
 */
template <class HASH, class EQUAL> class cache_local_lu_map_t {
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
  cache_local_lu_map_t(std::shared_ptr<tchecker::clockbounds::local_lu_map_t> local_lu, HASH const & hash, EQUAL const & equal)
      : _local_lu(local_lu), _hash(hash), _equal(equal)
  {
    if (_local_lu.get() == nullptr)
      throw std::invalid_argument("cache_local_lu_map_t: invalid nullptr pointer");
  }

  /*!
   \brief Copy constructor
   */
  cache_local_lu_map_t(tchecker::clockbounds::cache_local_lu_map_t<HASH, EQUAL> const & m)
      : _local_lu(m._local_lu), _hash(m._hash), _equal(m._equal)
  {
    insert(m._cache);
  }

  /*!
   \brief Move constructor
   */
  cache_local_lu_map_t(tchecker::clockbounds::cache_local_lu_map_t<HASH, EQUAL> &&) = default;

  /*!
   \brief Destructor
   */
  ~cache_local_lu_map_t() { clear(); }

  /*!
   \brief Assignment operator
   */
  tchecker::clockbounds::cache_local_lu_map_t<HASH, EQUAL> &
  operator=(tchecker::clockbounds::cache_local_lu_map_t<HASH, EQUAL> const & m)
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
  tchecker::clockbounds::cache_local_lu_map_t<HASH, EQUAL> &
  operator=(tchecker::clockbounds::cache_local_lu_map_t<HASH, EQUAL> &&) = default;

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

/*!
 \class bounded_cache_local_lu_map_t
 \brief Access to local LU bounds of tuple of locations with cache of bounded capacity
 \tparam HASH : type of hash functor of tchecker::const_vloc_sptr_t to std::size_t
 \tparam EQUAL : type of equality functor over tchecker::const_vloc_sptr_t
 */
template <class HASH, class EQUAL> class bounded_cache_local_lu_map_t {
  /*!< Type of LU clock bounds maps for storing */
  struct lu_maps_t {
    tchecker::clockbounds::map_t * L; /*!< L bounds map */
    tchecker::clockbounds::map_t * U; /*!< U bounds map */
  };

  /*!
   \brief Uniitialized maps
   */
  lu_maps_t const NO_MAPS = {.L = nullptr, .U = nullptr};

  /*!
   \brief Type of cache entry
   */
  struct entry_t {
    tchecker::const_vloc_sptr_t vloc;
    lu_maps_t lu_maps;
  };

  /*!
   \brief Uninitialized cache entry
   */
  entry_t const NO_ENTRY = {.vloc = nullptr, .lu_maps = NO_MAPS};

public:
  /*!
   \brief Constructor
   \param local_lu : local LU clock bounds map
   \param capacity : capacity of bounded cache
   \param hash : hash functor from tchecker::const_vloc_sptr_t to std::size_t
   \param equal : equality functor over tchecker::const_vloc_sptr_t
   \throw std::invalid_argument : if local_lu points to nullptr
   \note this keeps a shared pointer to local_lu, and creates a copy of hash and a copy of equal
   */
  bounded_cache_local_lu_map_t(std::shared_ptr<tchecker::clockbounds::local_lu_map_t> local_lu, std::size_t capacity,
                               HASH const & hash, EQUAL const & equal)
      : _local_lu(local_lu), _hash(hash), _equal(equal), _capacity(capacity), _table(_capacity, NO_ENTRY)
  {
    if (_local_lu.get() == nullptr)
      throw std::invalid_argument("bounded_cache_local_lu_map_t: invalid nullptr pointer");
  }

  /*!
   \brief Copy constructor
   */
  bounded_cache_local_lu_map_t(tchecker::clockbounds::bounded_cache_local_lu_map_t<HASH, EQUAL> const & m)
      : _local_lu(m._local_lu), _hash(m._hash), _equal(m._equal), _capacity(m._capacity), _table{_capacity, NO_ENTRY}
  {
    insert(m._table);
  }

  /*!
   \brief Move constructor
   */
  bounded_cache_local_lu_map_t(tchecker::clockbounds::bounded_cache_local_lu_map_t<HASH, EQUAL> &&) = default;

  /*!
   \brief Destructor
   */
  ~bounded_cache_local_lu_map_t() { clear(); }

  /*!
   \brief Assignment operator
   */
  tchecker::clockbounds::bounded_cache_local_lu_map_t<HASH, EQUAL> &
  operator=(tchecker::clockbounds::bounded_cache_local_lu_map_t<HASH, EQUAL> const & m)
  {
    if (this != &m) {
      clear();
      _local_lu = m._local_lu;
      _hash = m._hash;
      _equal = m._equal;
      _capacity = m._capacity;
      _table.resize(_capacity);
      insert(m._table);
    }
    return *this;
  }

  /*!
   \brief Move-assignment operator
   */
  tchecker::clockbounds::bounded_cache_local_lu_map_t<HASH, EQUAL> &
  operator=(tchecker::clockbounds::bounded_cache_local_lu_map_t<HASH, EQUAL> &&) = default;

  /*!
   \brief Clear the cache
   */
  void clear()
  {
    auto end = _table.end();
    for (auto it = _table.begin(); it != end; ++it)
      clear_entry(*it);
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
   cache is cleared, or if the corresponding bounds are removed from the cache later on
   */
  lu_maps_reference_t bounds(tchecker::const_vloc_sptr_t const & vloc)
  {
    auto i = _hash(vloc) % _capacity;
    entry_t & entry = _table[i];

    // vloc LU bound sare in the cache
    if (_equal(vloc, entry.vloc)) {
      assert(entry.lu_maps.L != nullptr);
      assert(entry.lu_maps.U != nullptr);
      return lu_maps_reference_t{.L = *entry.lu_maps.L, .U = *entry.lu_maps.U};
    }

    // otherwise, allocate new maps if entry is empty, then compute bounds
    if (entry.lu_maps.L == nullptr)
      entry.lu_maps.L = tchecker::clockbounds::allocate_map(_local_lu->clock_number());
    if (entry.lu_maps.U == nullptr)
      entry.lu_maps.U = tchecker::clockbounds::allocate_map(_local_lu->clock_number());

    entry.vloc = vloc;
    _local_lu->bounds(*entry.vloc, *entry.lu_maps.L, *entry.lu_maps.U);

    return lu_maps_reference_t{.L = *entry.lu_maps.L, .U = *entry.lu_maps.U};
  }

private:
  /*!
   \brief Clear cache entry
   \param entry : cache entry
   \post the bounds in entry have been deallocated, and entry has been set to
   NO_ENTRY
   */
  void clear_entry(entry_t & entry)
  {
    if (entry.lu_maps.L != nullptr)
      tchecker::clockbounds::deallocate_map(entry.lu_maps.L);
    if (entry.lu_maps.U != nullptr)
      tchecker::clockbounds::deallocate_map(entry.lu_maps.U);
    entry = NO_ENTRY;
  }

  /*!
   \brief Insert a table into this cache table
   \param table : a table
   \pre this and table have same capacity (checked by assertion)
   \post all elements from table have been inserted in this cache: shared pointers
   to tuple of locations are shared, whereas clock bounds are cloned
   */
  void insert(std::vector<entry_t> const & table)
  {
    assert(table.size() == _capacity);
    assert(_table.size() == _capacity);

    auto size = table.size();
    for (size_t i = 0; i < size; ++i) {
      clear_entry(_table[i]);
      _table[i].vloc = table[i].vloc;
      _table[i].lu_maps.L = (table[i].lu_maps.L == nullptr ? nullptr : tchecker::clockbounds::clone_map(*table[i].lu_maps.L));
      _table[i].lu_maps.U = (table[i].lu_maps.U == nullptr ? nullptr : tchecker::clockbounds::clone_map(*table[i].lu_maps.U));
    }
  }

  std::shared_ptr<tchecker::clockbounds::local_lu_map_t> _local_lu; /*!< Local LU map */
  HASH _hash;                                                       /*!< Hash functor over tuple of locations */
  EQUAL _equal;                                                     /*!< Equality functor over tuples of locations */
  std::size_t _capacity;                                            /*!< Cache capacity */
  std::vector<entry_t> _table;                                      /*!< Table of (vloc, LU bounds) */
};

} // namespace clockbounds

} // namespace tchecker

#endif // TCHECKER_CLOCKBOUNDS_CACHE_HH
