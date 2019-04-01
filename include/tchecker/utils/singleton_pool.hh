/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SINGLETON_POOL_HH
#define TCHECKER_SINGLETON_POOL_HH

/*!
 \file singleton_pool.hh
 \brief Singleton pool allocator
 */

namespace tchecker {
  
  /*!
   \class singleton_pool_t
   \brief Singleton pool allocator
   \tparam T : type of allocated object
   \note A singleton pool allocates a unique object. Each call to construct()
   returns the same address. The object at this address is reset at each
   construction.
   */
  template <class T>
  class singleton_pool_t {
  public:
    /*!
     \brief Type of allocated object
     */
    using t = T;
    
    /*!
     \brief Type of pointer to allocated object
     */
    using ptr_t = T *;
    
    /*!
     \brief Constructor
     */
    singleton_pool_t()
    : _t(nullptr)
    {}
    
    /*!
     \brief Copy constructor (deleted)
     */
    singleton_pool_t(tchecker::singleton_pool_t<T> const &) = delete;
    
    /*!
     \brief Move constructor (deleted)
     */
    singleton_pool_t(tchecker::singleton_pool_t<T> &&) = delete;
    
    /*!
     \brief Destructor
     \post Allocated objects have been destructed
     */
    ~singleton_pool_t()
    {
      destruct_all();
    }
    
    /*!
     \brief Assignment operator (deleted)
     */
    tchecker::singleton_pool_t<T> & operator= (tchecker::singleton_pool_t<T> const &) = delete;
    
    /*!
     \brief Move-assignment operator (deleted)
     */
    tchecker::singleton_pool_t<T> & operator= (tchecker::singleton_pool_t<T> &&) = delete;
    
    /*!
     \brief Construct an object
     \param args : parameters to a constructor of type T
     \return A new instance of T built with args and allocated by the pool
     */
    template <class ... TARGS>
    T * construct(TARGS && ... targs)
    {
      if (_t == nullptr)
        return _t = new T(targs...);
      
      _t->clear(std::forward<TARGS>(targs)...);
      return _t;
    }
    
    /*!
     \brief Collection
     \post Does nothing
     */
    void collect()
    {}
    
    /*!
     \brief Destruct all allocated objects
     \note All returned addresses have been invalidated
     */
    void destruct_all()
    {
      delete _t;
      _t = nullptr;
    }
    
    /*!
     \brief Free allocated memory
     \note Similar to delete_all()
     */
    void free_all()
    {
      destruct_all();
    }
    
    /*!
     \brief Enroll to garbarge collector
     \param gc : a garbage collector
     \pre this is not enrolled to a garbage collector yet
     \post this is enrolled to gc
     */
    void enroll(tchecker::gc_t & gc)
    {
      gc.enroll( [&] () { this->collect(); } );
    }
  private:
    T * _t;   /*!< Singleton allocated object */
  };
  
} // end of namespace tchecker

#endif // TCHECKER_SINGLETON_POOL_HH
