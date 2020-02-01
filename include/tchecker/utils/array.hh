/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ARRAY_HH
#define TCHECKER_ARRAY_HH

#include <cassert>
#include <cstring>
#include <iostream>
#include <tuple>
#include <type_traits>

#if BOOST_VERSION <= 106600
# include <boost/functional/hash.hpp>
#else
# include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/utils/allocation_size.hh"

/*!
 \file array.hh
 \brief Fixed-capacity array with single allocation
 */

namespace tchecker {
  
  
  /*!
   \class array_capacity_t
   \tparam CAPACITY : storage type for array capacity, must be unsigned
   \brief Default base class for single allocation array, provides array
   capacity
   */
  template <class CAPACITY>
  class array_capacity_t {
    
    static_assert(std::is_unsigned<CAPACITY>::value,
                  "CAPACITY must be unsigned");
    
  public:
    /*!
     \brief Type of array capacity
     */
    using capacity_t = CAPACITY;
    
    /*!
     \brief Constructor
     \param capacity : array capacity
     */
    array_capacity_t(CAPACITY capacity) : _capacity(capacity)
    {}
    
    /*!
     \brief Copy constructor
     \param b : array base
     \post this is a copy of b
     */
    array_capacity_t(tchecker::array_capacity_t<CAPACITY> const & b) = default;
    
    /*!
     \brief Move constructor
     \param b : array base
     \post b has been moved to this
     */
    array_capacity_t(tchecker::array_capacity_t<CAPACITY> && b) = default;
    
    /*!
     \brief Destructor
     */
    ~array_capacity_t() = default;
    
    /*!
     \brief Assignment operator
     \param b : array base
     \post this is a copy of b
     \return this after assignment
     */
    tchecker::array_capacity_t<CAPACITY> & operator= (tchecker::array_capacity_t<CAPACITY> const & b) = default;
    
    /*!
     \brief Move assignment operator
     \param b : array base
     \post b has been moved to this
     \return this after assignment
     */
    tchecker::array_capacity_t<CAPACITY> & operator= (tchecker::array_capacity_t<CAPACITY> && b) = default;
    
    /*!
     \brief Accessor
     \return Array capacity
     */
    inline constexpr  CAPACITY capacity() const
    {
      return _capacity;
    }
  protected:
    CAPACITY _capacity;  /*!< Array capacity */
  };
  
  
  /*!
   \brief Equality check
   \param a1 : array base
   \param a2 : array base
   \return true if a1 and a2 have same capacity, false otherwise
   */
  template <class CAPACITY>
  bool
  operator== (tchecker::array_capacity_t<CAPACITY> const & a1, tchecker::array_capacity_t<CAPACITY> const & a2)
  {
    return (a1.capacity() == a2.capacity());
  }
  
  /*!
   \brief Disequality check
   \param a1 : array base
   \param a2 : array base
   \return false if a1 and a2 have same capacity, true otherwise
   */
  template <class CAPACITY>
  bool
  operator!= (tchecker::array_capacity_t<CAPACITY> const & a1, tchecker::array_capacity_t<CAPACITY> const & a2)
  {
    return (! (a1 == a2));
  }
  
  /*!
   \brief Hash
   \aram a : array base
   \return Hash value for array base a
   */
  template <class CAPACITY>
  std::size_t hash_value(tchecker::array_capacity_t<CAPACITY> const & a)
  {
    return a.capacity();
  }
  
  
  
  
  /*!
   \class allocaction_size_t
   \brief Specialization of tchecker::allocation_size_t for class
   tchecker::array_capacity_t
   */
  template <class CAPACITY>
  class allocation_size_t<tchecker::array_capacity_t<CAPACITY>> {
  public:
    static constexpr std::size_t alloc_size()
    {
      return sizeof(tchecker::array_capacity_t<CAPACITY>);
    }
  };
  
  
  
  
  /*!
   \class make_array_t
   \brief Fixed-capacity array with single allocation
   \tparam T : type of value
   \tparam T_ALLOCSIZE : allocation size for T, should be at least sizeof(T)
   \tparam BASE : array base class, should derive from
   tchecker::array_capacity_t
   \note Implements an array with fixed capacity, but which is unknown at
   compilation time (contrasting with std::array).
   
   In order to achieve good performances, a single allocation is used for both
   the make_array_t instance AND the array of T. In details, we
   allocate:
   
   allocation_size_t<make_array_t<T, T_ALLOCSIZE, BASE>>::aloc_size()
   + capacity * T_ALLOCSIZE
   
   bytes of memory. The array starts after the first
   allocation_size_t<make_array_t<T, T_ALLOCSIZE, BASE>>::alloc_size() bytes.
   
   This has several consequences:
   - single allocation arrays cannot be allocated automatically (on the
   stack). Static methods allocate_and_construct() and destruct_and_deallocate()
   are provided for allocation and deallocation.
   - inheriting from single_allocation_array_t is in general not safe as it
   may overwrite the memory allocated for the array since the compiler is not
   aware of it. Extending single_allocation_array_t can be done in two ways.
   First, specify a BASE class that provides extra variables and methods.
   Second, inherit from single_allocation_array_t which is safe when adding new
   methods only (i.e. no variables)
   */
  template<class T, std::size_t T_ALLOCSIZE = sizeof(T), class BASE = tchecker::array_capacity_t<std::size_t>>
  class make_array_t : public BASE {
    
    static_assert(T_ALLOCSIZE >= sizeof(T),
                  "T_ALLOCSIZE should be at least sizeof(T)");
    
    static_assert(std::is_base_of<tchecker::array_capacity_t<typename BASE::capacity_t>, BASE>::value,
                  "BASE should derive from tchecker::array_capacity_t");
    
  public:
    /*!
     \brief Type of values
     */
    using value_t = T;
    
    /*!
     \brief Type of base class
     */
    using base_t = BASE;
    
    
    // Accessors
    
    /*!
     \brief Accessor
     \param i : index
     \pre i < size (chcked by assertion)
     \return reference to the value at index i in this array
     */
    inline T & operator[] (typename BASE::capacity_t i)
    {
      assert( i < BASE::_capacity );
      return *(array_begin() + i);
    }
    
    /*!
     \brief Accessor
     \param i : index
     \pre i < size (checked by assertion)
     \return const reference to the value at index i in this array
     */
    inline T const & operator[] (typename BASE::capacity_t i) const
    {
      assert( i < BASE::_capacity );
      return *(array_begin() + i);
    }
    
    /*!
     \bief Assignment operator
     \param a : array
     \pre capacity == a.capacity
     \post this is a copy of a
     \throw std::invalid_argument : if precondition is violated
     */
    tchecker::make_array_t<T, T_ALLOCSIZE, BASE> &
    operator= (tchecker::make_array_t<T, T_ALLOCSIZE, BASE> const & a)
    {
      if (BASE::_capacity != a.BASE::_capacity)
        throw std::invalid_argument("invalid array capacity");
      
      BASE::operator=(a);
      
      auto it = array_begin(), end = array_end();
      auto a_it = a.array_begin(), a_end = a.array_end();
      for ( ; (it != end) && (a_it != a_end); ++it, ++a_it)
        *it = *a_it;
      
      return *this;
    }
    
    /*!
     \bief Move assignment operator
     \param a : array
     \pre capacity == a.capacity
     \post a has been moved to this
     \throw std::invalid_argument : if precondition is violated
     */
    tchecker::make_array_t<T, T_ALLOCSIZE, BASE> & operator= (tchecker::make_array_t<T, T_ALLOCSIZE, BASE> && a)
    {
      if (BASE::_capacity != a.BASE::_capacity)
        throw std::invalid_argument("invalid array capacity");
      
      BASE::operator=(std::move(a));
      
      auto it = array_begin(), end = array_end();
      auto a_it = a.array_begin(), a_end = a.array_end();
      for ( ; (it != end) && (a_it != a_end); ++it, ++a_it)
        *it = std::move(*a_it);
      
      return *this;
    }
    
    
    // Base pointer access
    
    /*!
     \brief Accessor
     \return pointer to first element
     */
    T * ptr()
    {
      return array_begin();
    }
    
    /*!
     \brief Accessor
     \return pointer to first element
     */
    T const * ptr() const
    {
      return array_begin();
    }
    
    
    // Iterators
    
    /*!
     \brief Iterator
     */
    using iterator_t = T *;
    
    /*!
     \brief Const iterator
     */
    using const_iterator_t = T const *;
    
    /*!
     \brief Accessor
     \return iterator to first value
     */
    iterator_t begin()
    {
      return array_begin();
    }
    
    /*!
     \brief Accessor
     \return const iterator to first value
     */
    const_iterator_t begin() const
    {
      return array_begin();
    }
    
    /*!
     \brief Accessor
     \return past-the-end iterator
     */
    iterator_t end()
    {
      return array_end();
    }
    
    /*!
     \brief Accessor
     \return past-the-end const iterator
     */
    const_iterator_t end() const
    {
      return array_end();
    }
    
    
    // Allocation/destruction
    
    /*!
     \brief Construction
     \tparam BASE_ARGS : type of arguments to a constructor of BASE
     \tparam T_ARGS : type of arguments to a constructor of T
     \param ptr : pointer to an allocated zone
     \param base_args : tuple of arguments to a constructor of BASE
     \param t_args : tuple of arguments to a constructor of T
     \pre ptr points to an allocated zone of sufficient capacity, i.e. at least
     allocation_size_t<tchecker::make_array_t<T, T_ALLOCSIZE, BASE>>
     ::alloc_size(capacity)
     \note see tchecker::make_array_t::make_array_t
     */
    template <class... BASE_ARGS, class... T_ARGS>
    static inline void construct(void * ptr, std::tuple<BASE_ARGS...> base_args, std::tuple<T_ARGS...> t_args)
    {
      new (ptr) tchecker::make_array_t<T, T_ALLOCSIZE, BASE>(base_args, t_args);
    }
    
    /*!
     \brief Destruction
     \param ptr : single allocation array
     \post the destructor of ptr has been called
     */
    static inline void destruct(tchecker::make_array_t<T, T_ALLOCSIZE, BASE> * ptr)
    {
      if (ptr != nullptr)
        ptr->~make_array_t();
    }
  protected:
    /*!
     \brief Constructor
     \tparam BASE_ARGS : type of arguments to a constructor of BASE
     \tparam T_ARGS : type of arguments to a constructor of T
     \param base_args : tuple of arguments to a constructor of BASE
     \param t_args : tuple of arguments to a constructor of T
     \post BASE has been built with base_args, and every instance of T in
     array has been built with t_args
     */
    template <class... BASE_ARGS, class... T_ARGS>
    make_array_t(std::tuple<BASE_ARGS...> base_args, std::tuple<T_ARGS...> t_args)
    : BASE(std::make_from_tuple<BASE_ARGS...>(base_args))
    {
      T * const begin = array_begin();
      T * const end = array_end();
      for (T * t = begin; t != end; ++t)
        new (t) T(std::make_from_tuple<T_ARGS...>(t_args));
    }
    
    /*!
     \brief Copy constructor
     \param array : an array
     \post this is a copy of array
     */
    make_array_t(tchecker::make_array_t<T, T_ALLOCSIZE, BASE> const & array)
    : BASE(array)
    {
      T * it = array.array_begin();
      T * const end = array.array_end();
      T * t = array_begin();
      for ( ; it != end; ++it, ++t)
        new (t) T(*it);
    }
    
    /*!
     \brief Move constructor
     \param array : an array
     \post array has been moved to this
     */
    make_array_t(tchecker::make_array_t<T, T_ALLOCSIZE, BASE> && array)
    : BASE(array)
    {
      T * it = array.array_begin();
      T * const end = array.array_end();
      T * t = array_begin();
      for ( ; it != end; ++it, ++t)
        new (t) T(std::move(*it));
    }
    
    /*!
     \brief Destructor
     \post values have been destructed
     */
    ~make_array_t()
    {
      T * const begin = array_begin();
      T * const end = array_end();
      for (T * t = begin; t != end; ++t)
        t->~T();
    }
    
    /*!
     \brief Accessor
     \return first address in array
     */
    constexpr T * array_begin() const
    {
      return
      reinterpret_cast<T *>
      (reinterpret_cast<char *>
       (const_cast<tchecker::make_array_t<T, T_ALLOCSIZE, BASE> *>(this)) + tchecker::allocation_size_t<BASE>::alloc_size());
    }
    
    /*!
     \brief Accessor
     \return past-the-end address
     */
    constexpr T * array_end() const
    {
      return (array_begin() + BASE::_capacity);
    }
  };
  
  
  
  
  /*!
   \brief Equality check
   \param a1 : array
   \param a2 : array
   \return true if a1 and a2 have equal BASE and equal elements, false otherwise
   */
  template <class T, std::size_t T_ALLOCSIZE, class BASE>
  bool operator== (tchecker::make_array_t<T, T_ALLOCSIZE, BASE> const & a1,
                   tchecker::make_array_t<T, T_ALLOCSIZE, BASE> const & a2)
  {
    if (static_cast<BASE const &>(a1) != static_cast<BASE const &>(a2))
      return false;
    std::size_t capacity = a1.capacity();
    for (std::size_t i = 0; i < capacity; ++i)
      if (a1[i] != a2[i])
        return false;
    return true;
  }
  
  /*!
   \brief Disequality check
   \param a1 : array
   \param a2 : array
   \return false if a1 and a2 have equal BASE and equal elements, true otherwise
   */
  template <class T, std::size_t T_ALLOCSIZE, class BASE>
  bool operator!= (tchecker::make_array_t<T, T_ALLOCSIZE, BASE> const & a1,
                   tchecker::make_array_t<T, T_ALLOCSIZE, BASE> const & a2)
  {
    return (! (a1 == a2));
  }
  
  
  /*!
   \brief Hash
   \param a : array
   \return hash value for array a
   */
  template <class T, std::size_t T_ALLOCSIZE, class BASE>
  std::size_t hash_value(tchecker::make_array_t<T, T_ALLOCSIZE, BASE> const & a)
  {
    std::size_t h = hash_value(static_cast<BASE>(a));
    boost::hash_range(h, a.begin(), a.end());
    return h;
  }
  
  
  
  
  /*!
   \brief Output operator for arrays
   \param os : output stream
   \param array : array
   \post array has been output to os
   \return os after array has been output
   */
  template<class T, std::size_t T_ALLOCSIZE, class BASE>
  std::ostream & operator<< (std::ostream & os, tchecker::make_array_t<T, T_ALLOCSIZE, BASE> const & array)
  {
    for (auto it = array.begin(); it != array.end(); ++it) {
      if (it != array.begin())
        os << ",";
      os << *it;
    }
    return os;
  }
  
  
  
  
  /*!
   \class allocation_size_t
   \brief Specialization of class tchecker::allocation_size_t for type
   tchecker::make_array_t
   */
  template <class T, std::size_t T_ALLOCSIZE, class BASE>
  class allocation_size_t< tchecker::make_array_t<T, T_ALLOCSIZE, BASE> > {
    
    using capacity_t =
    typename tchecker::make_array_t<T, T_ALLOCSIZE, BASE>::capacity_t;
    
  public:
    /*!
     \brief Accessor
     \param capacity : array capacity
     \return Allocation size for objects of type
     tchecker::make_array_t<T, T_SIZE, BASE> with storage capacity
     */
    static constexpr std::size_t alloc_size(capacity_t capacity)
    {
      return (allocation_size_t<BASE>::alloc_size() + capacity * T_ALLOCSIZE);
    }
    
    /*!
     \brief Accessor
     \param capacity : array capacity
     \param args : arguments to a constructor of class tchecker::make_array_t
     \return Allocation size for objects of type
     tchecker::make_array_t<T, T_SIZE, BASE> with storage capacity
     */
    template <class ... ARGS>
    static constexpr std::size_t alloc_size(capacity_t capacity, ARGS && ... args)
    {
      return allocation_size_t<tchecker::make_array_t<T, T_ALLOCSIZE, BASE>>::alloc_size(capacity);
    }
  };
  
  
  
  
  /*!
   \brief Array allocation and construction
   \tparam T : type of value
   \tparam T_ALLOCSIZE : allocation size for T, should be at least sizeof(T)
   \tparam BASE : array base class, should derive from
   tchecker::array_capacity_t
   \tparam BASE_ARGS : types of arguments to a constructor of BASE
   \tparam T_ARGS : typed of arguments to a constructor of T
   \param capacity : array capacity
   \param base_args : tuple of arguments to a constructor of BASE
   \param t_args : tuple of argument to a constructor of T
   \return An instance of tchecker::make_array_t<T, T_ALLOCSIZE, BASE>
   constructed with base_args, with capacity values constructed with t_args
   */
  template<class T, std::size_t T_ALLOCSIZE, class BASE, class... BASE_ARGS, class... T_ARGS>
  tchecker::make_array_t<T, T_ALLOCSIZE, BASE> *
  make_array_allocate_and_construct(typename BASE::capacity_t capacity,
                                    std::tuple<BASE_ARGS...> base_args,
                                    std::tuple<T_ARGS...> t_args)
  {
    using array_t = tchecker::make_array_t<T, T_ALLOCSIZE, BASE>;
    
    void * ptr = new char[tchecker::allocation_size_t<array_t>::alloc_size(capacity)];
    
    array_t::construct(ptr, base_args, t_args);
    
    return reinterpret_cast<array_t *>(ptr);
  }
  
  
  /*!
   \brief Array destruction and deallocation
   \param a : array
   \pre pre has been allocated by tchecker::make_array_allocate_and_construct
   \post the destructor of a has been called, and a has been deleted
   */
  template <class T, std::size_t T_ALLOCSIZE, class BASE>
  void make_array_destruct_and_deallocate(tchecker::make_array_t<T, T_ALLOCSIZE, BASE> * a)
  {
    tchecker::make_array_t<T, T_ALLOCSIZE, BASE>::destruct(a);
    
    delete[] reinterpret_cast<char *>(a);
  }
  
} // end of namespace tchecker

#endif // TCHECKER_ARRAY_HH
