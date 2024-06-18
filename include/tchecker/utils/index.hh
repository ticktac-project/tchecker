/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_INDEX_HH
#define TCHECKER_INDEX_HH

#include <limits>
#include <stdexcept>
#include <type_traits>

#include <boost/container/flat_map.hpp>

#include "tchecker/utils/iterator.hh"

/*!
 \file index.hh
 \brief Indexes
 */

namespace tchecker {

/*!
 \class index_t
 \tparam KEY : type of keys
 \tparam T : type of values
 \brief Stores an bijective bimap KEY <-> T
 \note see boost::container::flat_map for requirements on KEY and T
 */
template <class KEY, class T> class index_t {
protected:
  /*!
   \brief KEY -> T map type
   */
  using key_map_t = typename boost::container::flat_map<KEY, T>;

  /*!
   \brief T -> KEY map type
   */
  using value_map_t = typename boost::container::flat_map<T, KEY>;

public:
  /*!
   \brief Constructor
   */
  index_t() = default;

  /*!
   \brief Copy constructor
   \param idx : index
   \post this is a copy of idx
   */
  index_t(index_t<KEY, T> const & idx) = default;

  /*!
   \brief Move constructor
   \param ids : index
   \post idx has been moved to this
   */
  index_t(index_t<KEY, T> && idx) = default;

  /*!
   \brief Assignment operator
   \param idx : index
   \post this is a copy of idx
   */
  index_t<KEY, T> & operator=(index_t<KEY, T> const & idx) = default;

  /*!
   \brief Move assignment operator
   \param idx : index
   \post ids has been moved to this
   */
  index_t<KEY, T> & operator=(index_t<KEY, T> && idx) = default;

  /*!
   \brief Adds a pair (key, value)
   \param t : value
   \param k : key
   \post the pair (k, t) has been added
   \throw std::invalid_argument : if t or k is already indexed
   */
  void add(KEY const & k, T const & t)
  {
    assert(_key_map.size() == _value_map.size());

    auto && [it, ok] = _key_map.insert(typename key_map_t::value_type(k, t));
    if (!ok)
      throw std::invalid_argument("key is already indexed");

    auto && ok2 = _value_map.insert(typename value_map_t::value_type(t, k)).second;
    if (!ok2) {
      _key_map.erase(it);
      assert(_key_map.size() == _value_map.size());
      throw std::invalid_argument("value is already indexed");
    }

    assert(_key_map.size() == _value_map.size());
  }

  /*!
   \brief Erase
   \param k : key
   \post any pair with key k as been removed
   \throw std::invalid_argument : if k is not a key in this index
   */
  void erase(KEY const & k)
  {
    assert(_key_map.size() == _value_map.size());

    auto key_it = _key_map.find(k);
    if (key_it == _key_map.end())
      throw std::invalid_argument("Unknown key");

    auto value_it = _value_map.find(key_it->second);
    if (value_it == _value_map.end())
      throw std::invalid_argument("Unknown value, inconsistent index");

    _key_map.erase(key_it);
    _value_map.erase(value_it);
  }

  /*!
   \brief Clears index
   \post this is empty
   */
  void clear()
  {
    _key_map.clear();
    _value_map.clear();

    assert(_key_map.size() == _value_map.size());
  }

  /*!
   \brief Accessor
   \return Size of index
   */
  inline size_t size() const
  {
    assert(_key_map.size() == _value_map.size());
    return _key_map.size();
  }

  /*!
   \brief Accessor
   \param t : value
   \return key for value t
   \throw std::invalid_argument : if t is not indexed
   */
  inline KEY const & key(T const & t) const
  {
    auto it = _value_map.find(t);
    if (it == _value_map.end())
      throw std::invalid_argument("value is not indexed");
    return it->second;
  }

  /*!
   \brief Accessor
   \param k : key
   \return value for key k
   \throw std::invalid_argument : if k is not indexed
   */
  inline T const & value(KEY const & k) const
  {
    auto it = _key_map.find(k);
    if (it == _key_map.end())
      throw std::invalid_argument("key is not indexed");
    return it->second;
  }

  /*!
   \brief Iterator on key indexed map (i.e. KEY -> T)
   */
  using const_key_map_iterator_t = typename key_map_t::const_iterator;

  /*!
   \brief Accessor
   \param e : key map element
   \return key in key map element e
   */
  inline constexpr KEY const & key(typename const_key_map_iterator_t::value_type const & e) const { return e.first; }

  /*!
   \brief Accessor
   \param e : key map element
   \return value in key map element e
   */
  inline constexpr T const & value(typename const_key_map_iterator_t::value_type const & e) const { return e.second; }

  /*!
   \brief Accessor
   \return Reference on first element in key indexed map
   */
  inline const_key_map_iterator_t begin_key_map() const { return const_key_map_iterator_t(_key_map.begin()); }

  /*!
   \brief Accessor
   \return Reference on passed-the-end in key indexed map
   */
  inline const_key_map_iterator_t end_key_map() const { return const_key_map_iterator_t(_key_map.end()); }

  /*!
   \brief Iterator on value indexed map (i.e. T -> KEY)
   */
  using const_value_map_iterator_t = typename value_map_t::const_iterator;

  /*!
   \brief Accessor
   \param e : element in value map
   \return key in value map element e
   */
  inline constexpr KEY const & key(typename const_value_map_iterator_t::value_type const & e) const { return e.second; }

  /*!
   \brief Accessor
   \param e : element in value map
   \return value in value map element e
   */
  inline constexpr T const & value(typename const_value_map_iterator_t::value_type const & e) const { return e.first; }

  /*!
   \brief Accessor
   \return Reference on first element in value indexed map
   */
  inline const_value_map_iterator_t begin_value_map() const { return const_value_map_iterator_t(_value_map.begin()); }

  /*!
   \brief Accessor
   \return Reference on passed-the-end in value indexed map
   */
  inline const_value_map_iterator_t end_value_map() const { return const_value_map_iterator_t(_value_map.end()); }

  /*!
   \brief Accessor
   \return Reference on first element (key map iterator)
   */
  inline const_key_map_iterator_t begin() const { return begin_key_map(); }

  /*!
   \brief Accessor
   \return Reference on passed-the-end element (key map iterator)
   */
  inline const_key_map_iterator_t end() const { return end_key_map(); }

  /*!
   \brief Finds a value
   \param t : value
   \return iterator to t if found, value_map_end() otherwise
   */
  inline const_value_map_iterator_t find_value(T const & t) const { return _value_map.find(t); }

  /*!
   \brief Finds a key
   \param key : key
   \return iterator to key if found, key_map_end() otherwise
   */
  inline const_key_map_iterator_t find_key(KEY const & key) const { return _key_map.find(key); }

  /*!
   \brief Iterator over keys
   */
  class keys_iterator_t {
  public:
    /*!
     \brief Constructor
     \param key_map_it : key map iterator
     \post this points to it
     */
    keys_iterator_t(const_key_map_iterator_t const & key_map_it) : _key_map_it(key_map_it) {}

    /*!
     \brief Equality check
     \param it : keys iterator
     \return true if this and it point to the same element, false otherwise
     */
    bool operator==(keys_iterator_t const & it) const { return _key_map_it == it._key_map_it; }

    /*!
     \brief Disequality check
     \param it : keys iterators
     \return true if this and it point to distinct elements, false otherwise
     */
    bool operator!=(keys_iterator_t const & it) const { return !(*this == it); }

    /*!
     \brief Step forward
     \pre this is a valid iterator
     \post this has been moved to next element
     */
    keys_iterator_t & operator++()
    {
      ++_key_map_it;
      return *this;
    }

    /*!
     \brief Dereference
     \pre this is a valid iterator
     \return key pointed by this iterator
     */
    KEY const & operator*() const { return (*_key_map_it).first; }

  private:
    const_key_map_iterator_t _key_map_it; /*!< Iterator over key map */
  };

  /*!
   \brief Type of range of keys
   */
  using keys_range_t = tchecker::range_t<keys_iterator_t>;

  /*!
   \brief Accessor
   \return range of keys in this index
   */
  keys_range_t keys() const { return tchecker::make_range(keys_iterator_t{_key_map.begin()}, keys_iterator_t{_key_map.end()}); }

protected:
  key_map_t _key_map;     /*!< KEY -> T map */
  value_map_t _value_map; /*!< T -> KEY map */
};

/*!
 \class autokey_index_t
 \tparam KEY : type of keys, must be an unsigned integer type
 \tparam T : type of values
 \brief Stores an bijective bimap KEY <-> T where keys are generated
 automatically
 \note see boost::container::flat_map for requirements on T
 */
template <class KEY, class T> class autokey_index_t : public index_t<KEY, T> {

  static_assert(std::is_unsigned<KEY>::value, "integer keys required");

public:
  /*!
   \brief Constructor
   */
  autokey_index_t()
  {
    clear(); // Resets key generator
  }

  /*!
   \brief Copy constructor
   \param idx : index
   \post this is a copy of idx
   */
  autokey_index_t(autokey_index_t<KEY, T> const & idx) = default;

  /*!
   \brief Move constructor
   \param idx : index
   \post ids has been moved to this
   */
  autokey_index_t(autokey_index_t<KEY, T> && idx) = default;

  /*!
   \brief Assignment operator
   \param idx : index
   \post this is a copy of idx
   */
  autokey_index_t<KEY, T> & operator=(autokey_index_t<KEY, T> const & idx) = default;

  /*!
   \brief Move assignment operator
   \param idx : index
   \post idx has been moved to this
   */
  autokey_index_t<KEY, T> & operator=(autokey_index_t<KEY, T> && idx) = default;

  /*!
   \brief Adds a value with auto-generated key
   \param t : value
   \post t is indexed is this
   \return automatic key associated to t
   \throw std::length_error : if all keys are in use
   \throw std::invalid_argument : if t is already indexed
   */
  KEY add(T const & t)
  {
    try {
      KEY k = next_key();
      this->index_t<KEY, T>::add(k, t);
      return k;
    }
    catch (...) {
      throw;
    }
  }

  /*!
   \brief Clears index
   */
  void clear() { this->index_t<KEY, T>::clear(); }

protected:
  /*!
   \brief Next key generator
   \return Next key
   \throw std::length_error : if there no key left
   */
  inline KEY next_key()
  {
    std::size_t size = this->index_t<KEY, T>::size();
    if (size == std::numeric_limits<KEY>::max())
      throw std::length_error("all keys in use");
    return static_cast<KEY>(size);
  }
};

} // end of namespace tchecker

#endif // TCHECKER_INDEX_HH
