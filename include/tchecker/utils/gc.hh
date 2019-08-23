/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GC_HH
#define TCHECKER_GC_HH

#include <functional>
#include <thread>
#include <vector>

/*!
 \file gc.hh
 \brief Garbage collector
 */

namespace tchecker {
  
  /*!
   \class gc_t
   \brief Garbarge collector for pool allocated objects
   \note Repeatedly runs a list of functions. Functions are added using method
   enroll(). A specialized version of enroll() for tchecker::pool_t adds the
   pool_t::collect() method to the list of functions. Enrolled functions should
   terminate. The GC is run in a separate thread. Hence, enrolled functions should
   be thread safe.
   \note Use enroll() to register functions and pools. Then, use start() to start
   garbage collecting, and use stop() to stop garbarge collecting.
   \note Garbage collection must be stopped before any enrolled pool is destructed
   */
  class gc_t {
  public:
    /*!
     \brief Constructor
     */
    gc_t();
    
    /*!
     \brief Destructor
     \post The garbage collection thread has been stopped
     */
    ~gc_t();
    
    /*!
     \brief Enroll a function
     \param f : function
     \pre f terminates and garbage collection is stopped
     \post f has been added to the list of GC functions
     \throw std::runtime_error : if garbage collection is running
     */
    void enroll(std::function<void(void)> && f);
    
    /*!
     \brief Garbarge collection
     \post Repeatedly runs enrolled functions in a round-robin
     \note The function terminates if stop() is called. As a result, execution
     of enrolled functions is suspended
     */
    void collect() const;
    
    /*!
     \brief Start garbage collection
     \post A garbage collection thread has been started
     \throw std::runtime_error : if garbage collection is already running
     */
    void start();
    
    /*!
     \brief Terminates the garbage collection thread
     \post The garbarge collection thread has been stopped, assuming that
     all enrolled functions terminate
     \note This method is blocking until the thread terminates
     */
    void stop();
  private:
    std::vector< std::function<void(void)> > _functions;    /*!< GC functions */
    bool _stop;                                             /*!< Stop flag */
    std::thread * _thread;                                  /*!< GC thread */
  };
  
  
} // end of namespace tchecker

#endif // TCHECKER_GC_HH
