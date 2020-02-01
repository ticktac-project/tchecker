/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SHARED_OBJECTS_HH
#define TCHECKER_SHARED_OBJECTS_HH

#include <exception>
#include <limits>

#if BOOST_VERSION <= 106600
# include <boost/functional/hash.hpp>
#else
# include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/utils/allocation_size.hh"

/*!
 \file shared_objects.hh
 \brief shared objects with support for pool allocation and garbage collection
 */

namespace tchecker {
  
  // shared objects
  
  /*!
   \class make_shared_t
   \brief Functor to create a shared class from a given class. Adds a reference
   counter to the given class.
   \tparam T : type to share
   \tparam REFCOUNT : type of the reference counter. Must be an unsigned type.
   \tparam RESERVED : number of reserved values of the reference counter
   \note The reference counter is stored by allocating sizeof(REFCOUNT) extra
   bytes of memory. These bytes are stored at the beginning of the allocated
   chunk of memory. More precisely, we allocate p of requested size and return
   p+sizeof(REFCOUNT) as the allocated address. Hence the reference counter is
   stored in the bytes before the address of this object
   \note The values in 0..std::numeric_limits<REFCOUNT>-RESERVED are used for
   reference counting. The values above REFCOUNT_MAX can be used by allocators
   to represent other states of this object. The default value, 2, is the number
   of states needed by tchecker::pool_t
   */
  template <class T, class REFCOUNT = unsigned int, std::size_t RESERVED = 2>
  class make_shared_t final : public T {
    
    static_assert(std::is_unsigned<REFCOUNT>::value,
                  "REFCOUNT must be an unsigned type");
    
  public:
    /*!
     \brief Type of object
     */
    using object_t = T;
    
    /*!
     \brief Type of reference counter
     */
    using refcount_t = REFCOUNT;
    
    /*!
     \brief Maximal value of the reference counter
     \note REFCOUNT_MAX is used to identify allocated objects that are not
     referenced by any pointer them. This protects allocated objects from
     garbage collection
     \note All the values beyond REFCOUNT_MAX can be used to represent other
     states of this object (see tchecker::gc_pool_t)
     */
    constexpr static refcount_t REFCOUNT_MAX = std::numeric_limits<refcount_t>::max() - RESERVED;
    
    static_assert( REFCOUNT_MAX > 0, "REFCOUNT_MAX should be > 0" );
    
    /*!
     \brief Object construction
     \param ptr : pointer to an allocated zone
     \param args : parameters to a constructor of type T
     \pre ptr points sizeof(refcount_t) bytes after the beginning of an
     allocated zone of size at least
     tchecker::allocation_size_t<shared_ptr_t<T>>
     \see tchecker::make_shared_t::make_shared_t
     */
    template <class ... ARGS>
    static inline void construct(void * ptr, ARGS && ... args)
    {
      new (ptr) make_shared_t<T, REFCOUNT, RESERVED>(args...);
    }
    
    /*!
     \brief Object allocation and construction
     \param args : parameters to a constructor of type T
     \note see tchecker::make_shared_t::construct
     */
    template <class ... ARGS>
    static tchecker::make_shared_t<T, REFCOUNT, RESERVED> * allocate_and_construct(ARGS && ... args)
    {
      std::size_t const alloc_size =
      tchecker::allocation_size_t<tchecker::make_shared_t<T, REFCOUNT, RESERVED>>().alloc_size(args...);
      
      char * ptr = new char[alloc_size];
      ptr += sizeof(refcount_t);      // shared object starts after refcount
      
      construct(ptr, args...);
      
      return reinterpret_cast<tchecker::make_shared_t<T, REFCOUNT, RESERVED> *>(ptr);
    }
    
    /*!
     \brief Object destruction
     \param ptr : shared object
     \post the destructor of ptr has been called
     */
    static void destruct(make_shared_t<T, REFCOUNT, RESERVED> * ptr)
    {
      ptr->~make_shared_t<T, REFCOUNT, RESERVED>();
    }
    
    /*!
     \brief Object destruction and deallocation
     \param ptr : shared object
     \pre ptr has been returned by
     maked_shared_t<T, REFCOUNT, RESERVED>::allocate_and_construct()
     \post the destructor of ptr has been called, and ptr has been deleted
     */
    static void destruct_and_deallocate(make_shared_t<T, REFCOUNT, RESERVED> * ptr)
    {
      make_shared_t<T, REFCOUNT, RESERVED>::destruct(ptr);
      
      char * p = reinterpret_cast<char *>(ptr) - sizeof(refcount_t);
      delete[] p;
    }
    
    /*!
     \brief Assignment operator
     \param t : shared object
     \post t has been assign to this
     \note the reference counter is not touched
     */
    make_shared_t<T, REFCOUNT, RESERVED> & operator= (tchecker::make_shared_t<T, REFCOUNT, RESERVED> const & t)
    {
      this->T::operator=(t);
      return *this;
    }
    
    /*!
     \brief Move assignment operator
     \param t : shared object
     \post t has been moved to this
     \note the reference counter is not touched
     */
    make_shared_t<T, REFCOUNT, RESERVED> & operator= (tchecker::make_shared_t<T, REFCOUNT, RESERVED> && t)
    {
      this->T::operator=(std::move(t));
      return *this;
    }
    
    /*!
     \brief Take a reference on this object
     \post The reference counter has been incremented
     \thow std::overflow_error : if the value of the reference counter exceeds
     the maximal value
     */
    inline void take_reference(void) const
    {
      refcount_t * refcount = refcount_addr();
      if (*refcount == REFCOUNT_MAX)  // allocated object with no reference yet
        *refcount = 1;                // first reference to this object
      else
        *refcount += 1;
      if (*refcount == REFCOUNT_MAX)  // overflow
        throw std::overflow_error("reference counter overflow");
    }
    
    /*!
     \brief Release a reference on this object
     \post The reference counter has been decremented
     \throw std::underflow_error : if the value of the reference counter
     becomes smaller than 0
     */
    inline void release_reference(void) const
    {
      refcount_t * refcount = refcount_addr();
      if (*refcount == 0)
        throw std::underflow_error("reference counter underflow");
      *refcount -= 1;
    }
    
    /*!
     \brief Accessor
     \return The value of the reference counter
     */
    inline constexpr std::size_t refcount(void) const
    {
      return *( refcount_addr() );
    }
  private:
    /*!
     \brief Constructor
     \param args : parameters to a constructor of type T
     \post The reference counter has value REFCOUNT_MAX (i.e. object
     initialized but not referenced yet)
     \note will also act as a copy/move constructor if T has one
     */
    template <class ... ARGS>
    make_shared_t(ARGS && ... args) : T(std::forward<ARGS>(args)...)
    {
      // initialize reference counter to REFCOUNT_MAX (allocated object)
      refcount_t * const refcount = refcount_addr();
      *refcount = REFCOUNT_MAX;   // yet no reference to this (GC protection)
    }
    
    /*!
     \brief Copy-constructor
     \param shared : shared object
     \post this is a copy of shared
     */
    make_shared_t(make_shared_t<T, REFCOUNT, RESERVED> const & shared) : T(shared)
    {
      // initialize reference counter to REFCOUNT_MAX (allocated object)
      refcount_t * const refcount = refcount_addr();
      *refcount = REFCOUNT_MAX;   // yet no reference to this (GC protection)
    }
    
    /*!
     \brief Destructor
     */
    ~make_shared_t() = default;
    
    /*!
     \brief Accessor
     \return The address of the reference counter
     */
    constexpr refcount_t * refcount_addr() const
    {
      return (reinterpret_cast<refcount_t *>(const_cast<tchecker::make_shared_t<T> *>(this)) - 1);
    }
  };
  
  
  
  
  // allocation size for shared objects
  
  /*!
   \class allocation_size_t<make_shared_t<T>>
   \brief Specialization of class tchecker::allocation_size_t for type
   tchecker::make_shared_t
   \note A specialization of tchecker::allocation_size_t should be defined for
   type T in namespace tchecker
   */
  template <class T>
  class allocation_size_t< tchecker::make_shared_t<T> > {
  public:
    /*!
     \brief Accessor
     \param args : parameters needed to determine the allocation size of T
     \return Allocation size for objects of type tchecker::make_shared_t<T>,
     which is the size needed by T: tchecker::allocation_size_t<T>().size(args)
     plus the bytes for the reference counter
     */
    template <class ... ARGS>
    static constexpr std::size_t alloc_size(ARGS && ... args)
    {
      // allocation size for T + size of reference counter
      return (tchecker::allocation_size_t<T>().alloc_size(args...) + sizeof(typename tchecker::make_shared_t<T>::refcount_t));
    }
  };
  
  
  
  
  // equality and hash for shared objects
  
  /*!
   \brief Equality check
   \param shared1 : shared object
   \param shared2 : shared object
   \return true if shared1 and shared2 are equal w.r.t. equality for type T, false otherwise
   */
  template <class T, class REFCOUNT, std::size_t RESERVED>
  bool operator== (tchecker::make_shared_t<T, REFCOUNT, RESERVED> const & shared1,
                   tchecker::make_shared_t<T, REFCOUNT, RESERVED> const & shared2)
  {
    return (static_cast<T const &>(shared1) == static_cast<T const &>(shared2));
  }
  
  /*!
   \brief Disequality check
   \param shared1 : shared object
   \param shared2 : shared object
   \return false if shared1 and shared2 are equal w.r.t. equality for type T, true otherwise
   */
  template <class T, class REFCOUNT, std::size_t RESERVED>
  bool operator!= (tchecker::make_shared_t<T, REFCOUNT, RESERVED> const & shared1,
                   tchecker::make_shared_t<T, REFCOUNT, RESERVED> const & shared2)
  {
    return (! (shared1 == shared2));
  }
  
  /*!
   \brief Hash
   \param shared : shared object
   \return hash value for shared
   */
  template <class T, class REFCOUNT, std::size_t RESERVED>
  std::size_t hash_value(tchecker::make_shared_t<T, REFCOUNT, RESERVED> const & shared)
  {
    return hash_value(static_cast<T const &>(shared));
  }
  
  
  
  
  // pointers to shared objects
  
  /*!
   \class intrusive_shared_ptr_t
   \brief Shared pointers with intrusive reference counter
   \tparam T : type of internal pointer. Must be make_shared_t<Y> for some type
   Y.
   */
  template <class T>
  class intrusive_shared_ptr_t {
  public:
    /*!
     \brief Type of shared object
     */
    using shared_object_t = T;
    
    /*!
     \brief Constructor
     \param t : pointer to store
     \post this object holds a reference on t
     */
    intrusive_shared_ptr_t(T * t = nullptr) : _t(nullptr)
    {
      reset(t);
    }
    
    /*!
     \brief Copy constructor
     \param p : shared pointer to copy from
     \post this object holds a reference to the object pointed by p
     */
    intrusive_shared_ptr_t(tchecker::intrusive_shared_ptr_t<T> const & p)
    : _t(nullptr)
    {
      reset(p._t);
    }
    
    /*!
     \brief Constructor
     \param y : shared pointer to copy from
     \tparam Y : type of object pointed to by y
     \pre dynamic cast of Y* into T* should be allowed
     \post this object holds a reference to the object pointed by y
     */
    template <class Y>
    explicit intrusive_shared_ptr_t(tchecker::intrusive_shared_ptr_t<Y> const & y)
    : _t(nullptr)
    {
      reset(dynamic_cast<T*>(y._t));
    }
    
    /*!
     \brief Move constructor
     \param p : shared pointer to move from
     \post this object holds a reference on the object pointed by p, and p
     has been reset to nullptr
     */
    intrusive_shared_ptr_t(tchecker::intrusive_shared_ptr_t<T> && p)
    : _t(nullptr)
    {
      reset(p._t);
      p.reset(nullptr);
    }
    
    /*!
     \brief Move constructor
     \param y : shared pointer to move from
     \tparam Y : type of object pointed to by y
     \pre dynamic cast of Y* into T* should be allowed
     \post this object holds a reference to the object pointed by y, and y
     has been reset to nullptr
     */
    template <class Y>
    explicit intrusive_shared_ptr_t(tchecker::intrusive_shared_ptr_t<Y> && y)
    : _t(nullptr)
    {
      reset(dynamic_cast<T *>(y._t));
      y.reset(nullptr);
    }
    
    /*!
     \brief Destructor
     \post the reference hold by this object has been released
     */
    ~intrusive_shared_ptr_t()
    {
      reset(nullptr);
    }
    
    /*!
     \brief Assignment operator
     \param p : shared pointer to assign from
     \post this object holds a reference to the object pointed by p
     */
    tchecker::intrusive_shared_ptr_t<T> & operator= (tchecker::intrusive_shared_ptr_t<T> const & p)
    {
      if (this != &p)
        reset(p._t);
      return *this;
    }
    
    /*!
     \brief Assignment operator
     \param y : shared pointer to assign from
     \tparam Y : type of object pointed to by y
     \pre dynamic cast of Y* into T* should be allowed
     \post this object holds a reference to the object pointed by y
     */
    template <class Y>
    tchecker::intrusive_shared_ptr_t<T> & operator= (tchecker::intrusive_shared_ptr_t<Y> const & y)
    {
      if (this != &y)
        reset(dynamic_cast<T *>(y._t));
      return *this;
    }
    
    /*!
     \brief Move assignment operator
     \param p : shared pointer to assign from
     \post this object holds a reference to the object pointed by p, and p
     has been reset to nullptr
     */
    tchecker::intrusive_shared_ptr_t<T> & operator= (tchecker::intrusive_shared_ptr_t<T> && p)
    {
      if (this != &p) {
        reset(p._t);
        p.reset(nullptr);
      }
      return *this;
    }
    
    /*!
     \brief Move assignment operator
     \param y : shared pointer to assign from
     \tparam Y : type of object pointed to by y
     \pre dynamic cast of Y* into T* should be allowed
     \post this object holds a reference to the object pointed by y, and y
     has been reset to nullptr
     */
    template <class Y>
    tchecker::intrusive_shared_ptr_t<T> & operator= (tchecker::intrusive_shared_ptr_t<Y> && y)
    {
      if (this != &y) {
        reset(dynamic_cast<T *>(y._t));
        y.reset(nullptr);
      }
      return *this;
    }
    
    /*!
     \brief Reset the shared pointer
     \param t : new address
     \post this object holds a reference to t and the reference it was holding
     previously has been released
     */
    inline void reset(T * t = nullptr)
    {
      if (t != nullptr)
        t->take_reference();
      if (_t != nullptr)
        _t->release_reference();
      _t = t;
    }
    
    /*!
     \brief Accessor
     \return reference counter of the object that is pointed to
     \pre this object hold a non nullptr pointer (checked by assertion)
     \throw std::runtime_error : when this object holds a nullptr pointer
     */
    inline unsigned refcount() const
    {
      assert( _t != nullptr );
      return _t->refcount();
    }
    
    /*!
     \brief Accessor
     \return internal pointer
     */
    inline constexpr T * ptr() const
    {
      return _t;
    }
    
    /*!
     \brief Accessor
     \return object pointed to
     \pre this object hold a non nullptr pointer (checked by assertion)
     \throw std::runtime_error : when this object holds a nullptr pointer
     */
    inline constexpr T & operator* () const
    {
      assert( _t != nullptr );
      return *_t;
    }
    
    /*!
     \brief Accessor
     \return internal pointer
     \pre this object hold a non nullptr pointer (checked by assertion)
     */
    inline constexpr T * operator-> () const
    {
      assert( _t != nullptr );
      return _t;
    }
  protected:
    template <class Y> friend class intrusive_shared_ptr_t;
    
    T * _t;   /*!< Internal pointer */
  };
  
  
  
  
  /*!
   \brief Static cast
   \param p : a shared pointer on type U
   \return A pointer on type T that points to the same object as p
   */
  template <class T, class U>
  tchecker::intrusive_shared_ptr_t<T> static_pointer_cast(tchecker::intrusive_shared_ptr_t<U> const & p)
  {
    return tchecker::intrusive_shared_ptr_t<T>(static_cast<T*>(p.ptr()));
  }
  
  
  
  
  /*!
   \brief Equality check
   \param p1 : shared pointer
   \param p2 : shared pointer
   \return true if p1 and p2 point the same object, false otherwise
   */
  template <class T>
  bool operator== (tchecker::intrusive_shared_ptr_t<T> const & p1, tchecker::intrusive_shared_ptr_t<T> const & p2)
  {
    return (p1.ptr() == p2.ptr());
  }
  
  
  /*!
   \brief Disequality check
   \param p1 : shared pointer
   \param p2 : shared pointer
   \return false if p1 and p2 point the same object, true otherwise
   */
  template <class T>
  bool operator!= (tchecker::intrusive_shared_ptr_t<T> const & p1, tchecker::intrusive_shared_ptr_t<T> const & p2)
  {
    return (! (p1 == p2));
  }
  
  
  /*!
   \brief Hash
   \param p : shared pointer
   \return hash value for p
   */
  template <class T>
  std::size_t hash_value(tchecker::intrusive_shared_ptr_t<T> const & p)
  {
    boost::hash<T *> hasher;
    return hasher(p.ptr());
  }
  
  
  /*!
   \class intrusive_shared_ptr_hash_t
   \brief Hash
   */
  class instrusive_shared_ptr_hash_t {
  public:
    /*!
     \brief Hash
     \param p : intrusive shared pointer
     \return Hash value for p
     */
    template <class T>
    inline std::size_t operator() (tchecker::intrusive_shared_ptr_t<T> const & p) const
    {
      return hash_value(p);
    }
  };
  
  
  
  
  /*!
   \class intrusive_shared_ptr_delegate_hash_t
   \brief Hash delegated to pointed object
   */
  class intrusive_shared_ptr_delegate_hash_t {
  public:
    /*!
     \brief Hash
     \param p : intrusive shared pointer
     \return Hash value for pointed object
     */
    template <class T>
    std::size_t operator() (tchecker::intrusive_shared_ptr_t<T> const & p) const
    {
      return hash_value(*p);
    }
  };
  
  
  
  
  /*!
   \class intrusive_shared_ptr_delegate_equal_to_t
   \brief Equality check delegated to pointed object
   */
  class intrusive_shared_ptr_delegate_equal_to_t {
  public:
    template <class T>
    bool operator() (tchecker::intrusive_shared_ptr_t<T> const & p1, tchecker::intrusive_shared_ptr_t<T> const & p2) const
    {
      return (*p1 == *p2);
    }
  };
  
} // end of namespace tchecker



namespace std {
  
  /*!
   \class hash
   \brief Specialisation of std::hash<> for tchecker::intrusive_shared_ptr_t
   \tparam T : type of internal pointer (see tchecker::intrusive_shared_ptr_t)
   */
  template <class T>
  class hash<tchecker::intrusive_shared_ptr_t<T>> {
  public:
    /*!
     \brief Hash function
     \param p : intrusive shared pointer
     \return hash value for the internal pointer in p
     */
    std::size_t operator() (tchecker::intrusive_shared_ptr_t<T> const & p) const
    {
      return tchecker::hash_value(p);
    }
  };
  
} // end of namespace std

#endif // TCHECKER_SHARED_OBJECTS_HH

