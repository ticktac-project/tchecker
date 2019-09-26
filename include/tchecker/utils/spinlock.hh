/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SPINLOCK_HH
#define TCHECKER_SPINLOCK_HH

#include <atomic>
#include <thread>

/*!
 \file spinlock.hh
 \brief Spin lock
 */

namespace tchecker {
  
  /*!
   \class spinlock_t
   \brief Spin lock
   */
  class spinlock_t {
  public:
    /*!
     \brief Constructor
     \post this lock is unlocked
     */
    spinlock_t()
    {
      _flag.clear(std::memory_order_release);
    }
    
    /*!
     \brief Copy constructor
     */
    spinlock_t(tchecker::spinlock_t const &) = delete;
    
    /*!
     \brief Move constructor
     */
    spinlock_t(tchecker::spinlock_t &&) = delete;
    
    /*!
     \brief Destructor
     */
    ~spinlock_t() = default;
    
    /*!
     \brief Assignment operator
     */
    tchecker::spinlock_t & operator= (tchecker::spinlock_t const &) = delete;
    
    /*!
     \brief Move assignment oeprator
     */
    tchecker::spinlock_t & operator= (tchecker::spinlock_t &&) = delete;
    
    /*!
     \brief Acquire the lock
     \post locked
     \note call to this method is blocking
     */
    inline void lock()
    {
      while ( _flag.test_and_set(std::memory_order_acquire) ) ;
    }
    
    /*!
     \brief Release the lock
     \post unlocked
     */
    inline void unlock()
    {
      _flag.clear(std::memory_order_release);
    }
  protected:
    std::atomic_flag _flag;               /*!< Atomic flag (lock) */
  };
  
  
} // end of namespace tchecker

#endif // TCHECKER_SPINLOCK_HH
