/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cstdlib>
#include <limits>
#include <stdexcept>

#include "tchecker/basictypes.hh"
#include "tchecker/variables/offset_clocks.hh"

namespace tchecker {
  
  /* offset_clock_variables_t */
  
  offset_clock_variables_t::offset_clock_variables_t() : _refcount{0}, _refmap{nullptr}
  {}
  
  
  offset_clock_variables_t::offset_clock_variables_t(tchecker::offset_clock_variables_t const & v)
  : tchecker::clock_variables_t(v)
  {
    _refcount = v._refcount;
    
    tchecker::clock_id_t size = tchecker::clock_variables_t::size();
    _refmap = static_cast<tchecker::clock_id_t *>(std::malloc(size * sizeof(*_refmap)));
    std::memcpy(_refmap, v._refmap, size * sizeof(*_refmap));
  }
  

  offset_clock_variables_t::offset_clock_variables_t(tchecker::offset_clock_variables_t && v)
  : tchecker::clock_variables_t(std::move(v))
  {
    _refcount = v._refcount;
    _refmap = v._refmap;
    
    v._refcount = 0;
    v._refmap = nullptr;
  }
  

  offset_clock_variables_t::~offset_clock_variables_t()
  {
    if (_refmap != nullptr)
      std::free(_refmap);
  }
  

  tchecker::offset_clock_variables_t & offset_clock_variables_t::operator= (tchecker::offset_clock_variables_t const & v)
  {
    if (this != &v) {
      tchecker::clock_variables_t::operator=(v);

      _refcount = v._refcount;
      
      tchecker::clock_id_t size = tchecker::clock_variables_t::size();
      _refmap = static_cast<tchecker::clock_id_t *>(std::malloc(size * sizeof(*_refmap)));
      std::memcpy(_refmap, v._refmap, size * sizeof(*_refmap));
    }
    return *this;
  }
  

  tchecker::offset_clock_variables_t & offset_clock_variables_t::operator= (tchecker::offset_clock_variables_t && v)
  {
    if (this != &v) {
      tchecker::clock_variables_t::operator=(std::move(v));
      _refcount = v._refcount;
      _refmap = v._refmap;
      
      v._refcount = 0;
      v._refmap = nullptr;
    }
    return *this;
  }
  
  
  void offset_clock_variables_t::declare_reference_clock(std::string const & name)
  {
    if (_refcount != tchecker::clock_variables_t::size())
      throw std::runtime_error("Reference clocks must be declared before offset variables");
    
    declare(name, 1, _refcount);
    ++ _refcount;
  }
  
  
  void offset_clock_variables_t::declare_offset_variable(std::string const & name,
                                                         tchecker::clock_id_t dim,
                                                         tchecker::clock_id_t refclock)
  {
    if (refclock >= _refcount)
      throw std::invalid_argument("Invalid reference clock identifier");
    
    declare(name, dim, refclock);
  }
  
  
  void offset_clock_variables_t::declare(std::string const & name, tchecker::clock_id_t dim, tchecker::clock_id_t refclock)
  {
    tchecker::clock_id_t size = tchecker::clock_variables_t::size();
    
    tchecker::clock_id_t id = size;
    tchecker::clock_info_t info{dim};
    tchecker::clock_variables_t::declare(id, name, info);
    
    _refmap = static_cast<tchecker::clock_id_t *>(std::realloc(_refmap, (size + 1) * sizeof(*_refmap)));
    _refmap[id] = refclock;
  }
  
  
  
  
  tchecker::offset_clock_variables_t build_from_variable_access(tchecker::variable_access_map_t const & vaccess_map,
                                                                tchecker::process_id_t proc_count,
                                                                tchecker::flat_clock_variables_t const & flat_clocks)
  {
    tchecker::offset_clock_variables_t offset_clocks;
    
    tchecker::clock_id_t clocks_count = flat_clocks.flattened_size();
    tchecker::clock_id_t offset_clocks_count = clocks_count - 1;  // ignore zero clock
    
    if (proc_count > std::numeric_limits<tchecker::clock_id_t>::max())
      throw std::invalid_argument("too many processes");
    
    unsigned long long offset_dim = proc_count + offset_clocks_count;
    if (offset_dim > std::numeric_limits<tchecker::clock_id_t>::max())
      throw std::invalid_argument("too many offset variables");
    
    // declare reference clocks (one per process)
    for (tchecker::clock_id_t pid = 0; pid < proc_count; ++pid) {
      offset_clocks.declare_reference_clock("$" + std::to_string(pid));
      assert( offset_clocks.id("$" + std::to_string(pid)) == pid );
    }
    
    // declare offset variables (skip the 0 clock)
    for (tchecker::clock_id_t clock_id = 1; clock_id < clocks_count; ++clock_id) {
      auto accessing_processes = vaccess_map.accessing_processes(clock_id, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);
      
      if (std::distance(accessing_processes.begin(), accessing_processes.end()) != 1)
        throw std::invalid_argument("bad clock access, cannot build offset clocks");
      
      tchecker::process_id_t pid = *accessing_processes.begin();
      tchecker::clock_id_t ref_id = offset_clocks.refmap()[pid];
      std::string const & clock_name = flat_clocks.index().value(clock_id);
      
      offset_clocks.declare_offset_variable("$" + clock_name, 1, ref_id);
    }
    
    return offset_clocks;
  }

} // end of namespace tchecker
