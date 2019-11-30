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

#include <vector>

#include "tchecker/utils/gc.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/utils/spinlock.hh"

namespace tchecker {
  
  namespace details {
    
    /*!
     \class thread_safe_collection_t
     \brief Collection of shared objects with thread-safe access
     \param T : type of object, should be tchecker::make_shared_t<>
     \param EQUAL : equality predicate on T values, should be default constructible
     */
    template <class T, class EQUAL>
    class thread_safe_collection_t {
    public:
      /*!
       \brief Constructor
       */
      thread_safe_collection_t() = default;
      
      /*!
       \brief Copy-construction (deleted)
       */
      thread_safe_collection_t(tchecker::details::thread_safe_collection_t<T, EQUAL> const &) = delete;
      
      /*!
       \brief Move-construction (deleted)
       */
      thread_safe_collection_t(tchecker::details::thread_safe_collection_t<T, EQUAL> &&) = delete;
      
      /*!
       \brief Destructor
       */
      ~thread_safe_collection_t() = default;
      
      /*!
       \brief Assignment operator (deleted)
       */
      tchecker::details::thread_safe_collection_t<T, EQUAL> &
      operator= (tchecker::details::thread_safe_collection_t<T, EQUAL> const &) = delete;
      
      /*!
       \brief Move-assignment operator (deleted)
       */
      tchecker::details::thread_safe_collection_t<T, EQUAL> &
      operator= (tchecker::details::thread_safe_collection_t<T, EQUAL> &&) = delete;
      
      /*!
       \brief Object caching
       \param t : object
       \return The object equal to t in this collection, which is t itself if no objec equal to t is already in the collection
       \post If no object equal to t was in the collection, t has been inserted into the collection
       \note Compelxity is linear in the size of the collection
       */
      tchecker::intrusive_shared_ptr_t<T> find_else_insert(tchecker::intrusive_shared_ptr_t<T> const & t)
      {
        _lock.lock();
        for (tchecker::intrusive_shared_ptr_t<T> const & u : _values)
          if (_equal(*t, *u)) {
            tchecker::intrusive_shared_ptr_t<T> uu(u);  // ensures at least 2 references to the object
            _lock.unlock();
            return uu;
          }
        _values.push_back(t);
        _lock.unlock();
        return t;
      }
      
      /*!
       \brief Membership predicate
       \param t : object
       \return true if the collection contains an object equal to t, false otherwise
       \note Complexity is linear in the size of the collection
       */
      bool find(tchecker::intrusive_shared_ptr_t<T> const & t)
      {
        _lock.lock();
        for (tchecker::intrusive_shared_ptr_t<T> const & u : _values)
          if (_equal(*t, *u)) {
            _lock.unlock();
            return true;
          }
        _lock.unlock();
        return false;
      }
      
      /*!
       \brief Clear
       \post The collection is empty
       */
      void clear()
      {
        _lock.lock();
        _values.clear();
        _lock.unlock();
      }
      
      /*!
       \brief Garbage collection
       \post All  objects with reference counter 1 (i.e. objects that are only referenced by this collection) have been
       removed from this collection
       */
      void collect()
      {
        _lock.lock();
        for (std::size_t i = 0; i < _values.size(); ) {
          if (_values[i]->refcount() == 1) {
            _values[i] = _values.back();
            _values.pop_back();
          }
          else
            ++i;
        }
        _lock.unlock();
      }
      
      /*!
       \brief Accessor
       \return Number of objects in the collection
       */
      std::size_t size() const
      {
        _lock.lock();
        std::size_t size = _values.size();
        _lock.unlock();
        return size;
      }
    private:
      std::vector<tchecker::intrusive_shared_ptr_t<T>> _values;  /*!< Collection */
      mutable tchecker::spinlock_t _lock;                        /*!< Lock for thread-safe access */
      EQUAL _equal;                                              /*!< Equality predicate on T values */
    };
    
  } // end of namespace details
  
  
  
  
  /*!
   \class cache_t
   \brief Cache of shared objects
   \param T : type of objects, should be tchecker::make_shared_t<>
   \param HASH : type of hash function on T, should be default constructible
   \param EQUAL : type of equality predicate on T, should be default constructible
   */
  template <class T, class HASH, class EQUAL>
  class cache_t {
  public:
    /*!
     \brief Constructor
     \param table_size : size of the hash table
     \note The size of the hash table is fixed. The hash table maps hash codes to collection of objects.
     */
    cache_t(std::size_t table_size = 65536)
    : _table_size(table_size)
    {
      _table = new tchecker::details::thread_safe_collection_t<T, EQUAL>[_table_size];
    }
    
    /*!
     \brief Copy-construction (deleted)
     */
    cache_t(tchecker::cache_t<T, HASH, EQUAL> const &) = delete;
    
    /*!
     \brief Move-construction (deleted)
     */
    cache_t(tchecker::cache_t<T, HASH, EQUAL> &&) = delete;
    
    /*!
     \brief Destructor
     */
    ~cache_t()
    {
      clear();
      delete[] _table;
    }
    
    /*!
     \brief Assignment operator (deleted)
     */
    tchecker::cache_t<T, HASH, EQUAL> & operator= (tchecker::cache_t<T, HASH, EQUAL> const &) = delete;
    
    /*!
     \brief Move-assignment operator (deleted)
     */
    tchecker::cache_t<T, HASH, EQUAL> & operator= (tchecker::cache_t<T, HASH, EQUAL> &&) = delete;
    
    /*!
     \brief Object caching
     \param t : object
     \return object equivalent to t (w.r.t. HASH and EQUAL) in this cache, t itself if no equivalent object was in the cache before
     \post t has been inserted in the cache if no equivalent object (w.r.t. HASH and EQUAL) was in the cache before
     */
    tchecker::intrusive_shared_ptr_t<T> find_else_insert(tchecker::intrusive_shared_ptr_t<T> const & t)
    {
      std::size_t i = _hash(*t) % _table_size;
      return _table[i].find_else_insert(t);
    }
    
    /*!
     \brief Membership predicate
     \param t : object
     \return true if this cache constains an object equivalent to t (w.r.t. HASH and EQUAL), false otherwise
     */
    bool find(tchecker::intrusive_shared_ptr_t<T> const & t)
    {
      std::size_t i = _hash(*t) % _table_size;
      return _table[i].find(t);
    }
    
    /*!
     \brief Clear
     \post The cache is empty
     */
    void clear()
    {
      for (std::size_t i = 0; i < _table_size; ++i)
        _table[i].clear();
    }
    
    /*!
     \brief Garbage collection
     \post All objects with reference counter 1 (i.e. objects with no reference outisde of the cache) have been removed from the cache
     */
    void collect()
    {
      for (std::size_t i = 0; i < _table_size; ++i)
        _table[i].collect();
    }
    
    /*!
     \brief Enroll to garbarge collector
     \param gc : a garbage collector
     \pre this is not enrolled to a garbage collector yet
     \post this is enrolled to gc
     */
    void enroll(gc_t & gc)
    {
      gc.enroll([&] () { this->collect(); });
    }
    
    /*!
     \brief Accessor
     \return Number of objects in the cache
     \note Complexity linear in the size of the hash table
     */
    std::size_t size() const
    {
      std::size_t size = 0;
      for (std::size_t i = 0; i < _table_size; ++i)
        size += _table[i].size();
      return size;
    }
  private:
    std::size_t _table_size;                                        /*!< Number of collections in the map */
    tchecker::details::thread_safe_collection_t<T, EQUAL> * _table; /*!< Map : hash code -> objects collection */
    HASH _hash;                                                     /*!< Hash function on T values */
  };
  
} // end of namespace tchecker

#endif // TCHECKER_CACHE_HH
