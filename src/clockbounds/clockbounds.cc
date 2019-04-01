/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/utils/iterator.hh"

namespace tchecker {
  
  namespace clockbounds {
    
    /* map_t */
    
    tchecker::clockbounds::map_t * allocate_map(tchecker::clock_id_t clock_nb)
    {
      tchecker::clockbounds::map_t * map = tchecker::make_array_allocate_and_construct
      <tchecker::clockbounds::bound_t, sizeof(tchecker::clockbounds::bound_t), tchecker::array_capacity_t<clock_id_t>>
      (clock_nb, std::make_tuple(clock_nb), std::tuple(0));
      
      // Initialize to 0,-inf,...,-inf
      clear(*map);
      
      return map;
    }
    
    
    tchecker::clockbounds::map_t * clone_map(tchecker::clockbounds::map_t const & m)
    {
      tchecker::clockbounds::map_t * clone = tchecker::clockbounds::allocate_map(m.capacity());
      for (tchecker::clock_id_t id = 0; id < m.capacity(); ++id)
        (*clone)[id] = m[id];
      return clone;
    }
    
    
    void deallocate_map(tchecker::clockbounds::map_t * m)
    {
      tchecker::make_array_destruct_and_deallocate(m);
    }
    
    
    void clear(tchecker::clockbounds::map_t & map)
    {
      map[0] = 0;
      for (tchecker::clock_id_t id = 1; id < map.capacity(); ++id)
        map[id] = tchecker::clockbounds::NO_BOUND;
    }
    
    
    bool update(tchecker::clockbounds::map_t & map, tchecker::clock_id_t id, tchecker::clockbounds::bound_t bound)
    {
      assert( id < map.capacity() );
      if (bound <= map[id])
        return false;
      map[id] = bound;
      return true;
    }
    
    
    bool update(tchecker::clockbounds::map_t & map, tchecker::clockbounds::map_t const & upd)
    {
      bool modified = false;
      for (tchecker::clock_id_t id = 0; id < map.capacity(); ++id)
        if (update(map, id, upd[id]))
          modified = true;
      return modified;
    }
    
    
    std::ostream & operator<< (std::ostream & os, tchecker::clockbounds::map_t const & map)
    {
      for (std::size_t i = 0; i < map.capacity(); ++i) {
        if (i != 0)
          os << ",";
        
        if (map[i] == tchecker::clockbounds::NO_BOUND)
          os << ".";
        else
          os << map[i];
      }
      return os;
    }
    
    
    
    
    /* local_lu_map_t */
    
    local_lu_map_t::local_lu_map_t(tchecker::loc_id_t loc_nb, tchecker::clock_id_t clock_nb)
    : _loc_nb(loc_nb), _clock_nb(clock_nb), _L(_loc_nb, nullptr), _U(_loc_nb, nullptr)
    {
      for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id) {
        _L[id] = tchecker::clockbounds::allocate_map(_clock_nb);
        _U[id] = tchecker::clockbounds::allocate_map(_clock_nb);
      }
    }
    
    
    local_lu_map_t::local_lu_map_t(tchecker::clockbounds::local_lu_map_t const & m)
    : _loc_nb(m._loc_nb), _clock_nb(m._clock_nb), _L(_loc_nb, nullptr), _U(_loc_nb, nullptr)
    {
      for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id) {
        _L[id] = tchecker::clockbounds::clone_map(* m._L[id]);
        _U[id] = tchecker::clockbounds::clone_map(* m._U[id]);
      }
    }
    
    
    local_lu_map_t::local_lu_map_t(tchecker::clockbounds::local_lu_map_t && m)
    : _loc_nb(m._loc_nb), _clock_nb(m._clock_nb), _L(std::move(m._L)), _U(std::move(m._U))
    {
      m._L.clear();
      m._U.clear();
      m._loc_nb = 0;
      m._clock_nb = 0;
    }
    
    
    local_lu_map_t::~local_lu_map_t()
    {
      for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id) {
        tchecker::clockbounds::deallocate_map(_L[id]);
        tchecker::clockbounds::deallocate_map(_U[id]);
      }
    }
    
    
    tchecker::clockbounds::local_lu_map_t & local_lu_map_t::operator= (tchecker::clockbounds::local_lu_map_t const & m)
    {
      if (this != &m) {
        for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id) {
          tchecker::clockbounds::deallocate_map(_L[id]);
          tchecker::clockbounds::deallocate_map(_U[id]);
        }
        
        _loc_nb = m._loc_nb;
        _clock_nb = m._clock_nb;
        _L.resize(_loc_nb, nullptr);
        _U.resize(_loc_nb, nullptr);
        for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id) {
          _L[id] = tchecker::clockbounds::clone_map(* m._L[id]);
          _U[id] = tchecker::clockbounds::clone_map(* m._U[id]);
        }
      }
      return *this;
    }
    
    
    tchecker::clockbounds::local_lu_map_t & local_lu_map_t::operator= (tchecker::clockbounds::local_lu_map_t && m)
    {
      if (this != &m) {
        for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id) {
          tchecker::clockbounds::deallocate_map(_L[id]);
          tchecker::clockbounds::deallocate_map(_U[id]);
        }
        
        _loc_nb = std::move(m._loc_nb);
        _clock_nb = std::move(m._clock_nb);
        _L = std::move(m._L);
        _U = std::move(m._U);
        
        m._L.clear();
        m._U.clear();
        m._loc_nb = 0;
        m._clock_nb = 0;
      }
      return *this;
    }
    
    
    tchecker::loc_id_t local_lu_map_t::loc_number() const
    {
      return _loc_nb;
    }
    
    
    tchecker::clock_id_t local_lu_map_t::clock_number() const
    {
      return _clock_nb;
    }
    
    
    tchecker::clockbounds::map_t & local_lu_map_t::L(tchecker::loc_id_t id)
    {
      assert(id < _loc_nb);
      return * _L[id];
    }
    
    
    tchecker::clockbounds::map_t const & local_lu_map_t::L(tchecker::loc_id_t id) const
    {
      assert(id < _loc_nb);
      return * _L[id];
    }
    
    
    tchecker::clockbounds::map_t & local_lu_map_t::U(tchecker::loc_id_t id)
    {
      assert(id < _loc_nb);
      return * _U[id];
    }
    
    
    tchecker::clockbounds::map_t const & local_lu_map_t::U(tchecker::loc_id_t id) const
    {
      assert(id < _loc_nb);
      return * _U[id];
    }
    
    
    std::ostream & operator<< (std::ostream & os, tchecker::clockbounds::local_lu_map_t const & map)
    {
      tchecker::loc_id_t loc_nb = map.loc_number();
      for (tchecker::loc_id_t l = 0; l < loc_nb; ++l)
        os << l << ": L=" << map.L(l) << " U=" << map.U(l) << std::endl;
      return os;
    }
    
    
    
    
    /* global_lu_map_t */
    
    global_lu_map_t::global_lu_map_t(tchecker::clock_id_t clock_nb)
    : _clock_nb(clock_nb), _L(nullptr), _U(nullptr)
    {
      _L = tchecker::clockbounds::allocate_map(_clock_nb);
      _U = tchecker::clockbounds::allocate_map(_clock_nb);
    }
    
    
    global_lu_map_t::global_lu_map_t(tchecker::clockbounds::global_lu_map_t const & m)
    : _clock_nb(m._clock_nb), _L(nullptr), _U(nullptr)
    {
      _L = tchecker::clockbounds::clone_map(* m._L);
      _U = tchecker::clockbounds::clone_map(* m._U);
    }
    
    
    global_lu_map_t::global_lu_map_t(tchecker::clockbounds::global_lu_map_t && m)
    : _clock_nb(m._clock_nb), _L(std::move(m._L)), _U(std::move(m._U))
    {
      m._L = nullptr;
      m._U = nullptr;
      m._clock_nb = 0;
    }
    
    
    global_lu_map_t::~global_lu_map_t()
    {
      tchecker::clockbounds::deallocate_map(_L);
      tchecker::clockbounds::deallocate_map(_U);
    }
    
    
    tchecker::clockbounds::global_lu_map_t & global_lu_map_t::operator= (tchecker::clockbounds::global_lu_map_t const & m)
    {
      if (this != &m) {
        tchecker::clockbounds::deallocate_map(_L);
        tchecker::clockbounds::deallocate_map(_U);
        
        _clock_nb = m._clock_nb;
        _L = tchecker::clockbounds::clone_map(* m._L);
        _U = tchecker::clockbounds::clone_map(* m._U);
      }
      return *this;
    }
    
    
    tchecker::clockbounds::global_lu_map_t & global_lu_map_t::operator= (tchecker::clockbounds::global_lu_map_t && m)
    {
      if (this != &m) {
        tchecker::clockbounds::deallocate_map(_L);
        tchecker::clockbounds::deallocate_map(_U);
        
        _clock_nb = std::move(m._clock_nb);
        _L = std::move(m._L);
        _U = std::move(m._U);
        
        m._L = nullptr;
        m._U = nullptr;
        m._clock_nb = 0;
      }
      return *this;
    }
    
    
    tchecker::clock_id_t global_lu_map_t::clock_number() const
    {
      return _clock_nb;
    }
    
    
    tchecker::clockbounds::map_t & global_lu_map_t::L(void)
    {
      return * _L;
    }
    
    
    tchecker::clockbounds::map_t const & global_lu_map_t::L(void) const
    {
      return * _L;
    }
    
    
    tchecker::clockbounds::map_t & global_lu_map_t::U(void)
    {
      return * _U;
    }
    
    
    tchecker::clockbounds::map_t const & global_lu_map_t::U(void) const
    {
      return * _U;
    }
    
    
    std::ostream & operator<< (std::ostream & os, tchecker::clockbounds::global_lu_map_t const & map)
    {
      return os << "L=" << map.L() << " U=" << map.U() << std::endl;
    }
    
    
    
    
    /* local_m_map_t */
    
    local_m_map_t::local_m_map_t(tchecker::loc_id_t loc_nb, tchecker::clock_id_t clock_nb)
    : _loc_nb(loc_nb), _clock_nb(clock_nb), _M(_loc_nb, nullptr)
    {
      for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id)
        _M[id] = tchecker::clockbounds::allocate_map(_clock_nb);
    }
    
    
    local_m_map_t::local_m_map_t(tchecker::clockbounds::local_m_map_t const & m)
    : _loc_nb(m._loc_nb), _clock_nb(m._clock_nb), _M(_loc_nb, nullptr)
    {
      for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id)
        _M[id] = tchecker::clockbounds::clone_map(* m._M[id]);
    }
    
    
    local_m_map_t::local_m_map_t(tchecker::clockbounds::local_m_map_t && m)
    : _loc_nb(m._loc_nb), _clock_nb(m._clock_nb), _M(std::move(m._M))
    {
      m._M.clear();
      m._loc_nb = 0;
      m._clock_nb = 0;
    }
    
    
    local_m_map_t::~local_m_map_t()
    {
      for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id)
        tchecker::clockbounds::deallocate_map(_M[id]);
    }
    
    
    tchecker::clockbounds::local_m_map_t & local_m_map_t::operator= (tchecker::clockbounds::local_m_map_t const & m)
    {
      if (this != &m) {
        for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id)
          tchecker::clockbounds::deallocate_map(_M[id]);
        
        _loc_nb = m._loc_nb;
        _clock_nb = m._clock_nb;
        _M.resize(_loc_nb, nullptr);
        for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id)
          _M[id] = tchecker::clockbounds::clone_map(* m._M[id]);
      }
      return *this;
    }
    
    
    tchecker::clockbounds::local_m_map_t & local_m_map_t::operator= (tchecker::clockbounds::local_m_map_t && m)
    {
      if (this != &m) {
        for (tchecker::loc_id_t id = 0; id < _loc_nb; ++id)
          tchecker::clockbounds::deallocate_map(_M[id]);
        
        _loc_nb = std::move(m._loc_nb);
        _clock_nb = std::move(m._clock_nb);
        _M = std::move(m._M);
        
        m._M.clear();
        m._loc_nb = 0;
        m._clock_nb = 0;
      }
      return *this;
    }
    
    
    tchecker::loc_id_t local_m_map_t::loc_number() const
    {
      return _loc_nb;
    }
    
    
    tchecker::clock_id_t local_m_map_t::clock_number() const
    {
      return _clock_nb;
    }
    
    
    tchecker::clockbounds::map_t & local_m_map_t::M(tchecker::loc_id_t id)
    {
      assert(id < _loc_nb);
      return * _M[id];
    }
    
    
    tchecker::clockbounds::map_t const & local_m_map_t::M(tchecker::loc_id_t id) const
    {
      assert(id < _loc_nb);
      return * _M[id];
    }
    
    
    std::ostream & operator<< (std::ostream & os, tchecker::clockbounds::local_m_map_t const & map)
    {
      tchecker::loc_id_t loc_nb = map.loc_number();
      for (tchecker::loc_id_t l = 0; l < loc_nb; ++l)
        os << l << ": M=" << map.M(l) << std::endl;
      return os;
    }
    
    
    
    
    /* global_m_map_t */
    
    global_m_map_t::global_m_map_t(tchecker::clock_id_t clock_nb)
    : _clock_nb(clock_nb), _M(nullptr)
    {
      _M = tchecker::clockbounds::allocate_map(_clock_nb);
    }
    
    
    global_m_map_t::global_m_map_t(tchecker::clockbounds::global_m_map_t const & m)
    : _clock_nb(m._clock_nb), _M(nullptr)
    {
      _M = tchecker::clockbounds::clone_map(* m._M);
    }
    
    
    global_m_map_t::global_m_map_t(tchecker::clockbounds::global_m_map_t && m)
    : _clock_nb(m._clock_nb), _M(std::move(m._M))
    {
      m._M = nullptr;
      m._clock_nb = 0;
    }
    
    
    global_m_map_t::~global_m_map_t()
    {
      tchecker::clockbounds::deallocate_map(_M);
    }
    
    
    tchecker::clockbounds::global_m_map_t & global_m_map_t::operator= (tchecker::clockbounds::global_m_map_t const & m)
    {
      if (this != &m) {
        tchecker::clockbounds::deallocate_map(_M);
        
        _clock_nb = m._clock_nb;
        _M = tchecker::clockbounds::clone_map(* m._M);
      }
      return *this;
    }
    
    
    tchecker::clockbounds::global_m_map_t & global_m_map_t::operator= (tchecker::clockbounds::global_m_map_t && m)
    {
      if (this != &m) {
        tchecker::clockbounds::deallocate_map(_M);
        
        _clock_nb = std::move(m._clock_nb);
        _M = std::move(m._M);
        
        m._M = nullptr;
        m._clock_nb = 0;
      }
      return *this;
    }
    
    
    tchecker::clock_id_t global_m_map_t::clock_number() const
    {
      return _clock_nb;
    }
    
    
    tchecker::clockbounds::map_t & global_m_map_t::M(void)
    {
      return * _M;
    }
    
    
    tchecker::clockbounds::map_t const & global_m_map_t::M(void) const
    {
      return * _M;
    }
    
    
    std::ostream & operator<< (std::ostream & os, tchecker::clockbounds::global_m_map_t const & map)
    {
      return os << "M=" << map.M() << std::endl;
    }
    
  } // end of namespace clockbound
  
} // end of namespace tchecker
