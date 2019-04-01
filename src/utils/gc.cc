/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/utils/gc.hh"

namespace tchecker {
  
  gc_t::gc_t() : _stop(true), _thread(nullptr)
  {}
  
  
  gc_t::~gc_t()
  {
    if ((_thread != nullptr) && _thread->joinable())
      stop();
  }
  
  
  void gc_t::enroll(std::function<void(void)> && f)
  {
    if (_thread != nullptr)
      throw std::runtime_error("cannot enroll when garbage collection is running");
    _functions.push_back(std::forward<std::function<void(void)>>(f));
  }
  
  
  void gc_t::collect() const
  {
    while (true) {
      if (_stop)    // ensures stopping (empty list of functions)
        return;
      
      for (auto & f : _functions) {
        if (_stop)  // earlier termination (optimization)
          return;
        f();
      }
    }
  }
  
  
  void gc_t::start()
  {
    if (_thread != nullptr)
      throw std::runtime_error("garbage collection is already running");
    _stop = false;
    _thread = new std::thread(&tchecker::gc_t::collect, this);
  }
  
  
  void gc_t::stop()
  {
    _stop = true;
    _thread->join();
    delete _thread;
    _thread = nullptr;
  }
  
} // end of namespace tchecker
