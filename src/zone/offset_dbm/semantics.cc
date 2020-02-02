/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/zone/offset_dbm/semantics.hh"

namespace tchecker {
  
  namespace offset_dbm {
    
    namespace details {
      
      bool constrain(tchecker::dbm::db_t * dbm,
                     tchecker::clock_id_t dim,
                     tchecker::clock_constraint_container_t const & constraints)
      {
        assert(dbm != nullptr);
        assert(! tchecker::offset_dbm::is_empty_0(dbm, dim));
        assert(tchecker::offset_dbm::is_tight(dbm, dim));
        
        for (tchecker::clock_constraint_t const & c : constraints) {
          auto cmp = (c.comparator() == tchecker::clock_constraint_t::LT
                      ? tchecker::dbm::LT
                      : tchecker::dbm::LE);
          if (tchecker::dbm::constrain(dbm, dim, c.id1(), c.id2(), cmp, c.value()) == tchecker::dbm::EMPTY)
            return false;
        }
        return true;
      }
      
      
      void reset(tchecker::dbm::db_t * dbm,
                 tchecker::clock_id_t dim,
                 tchecker::clock_id_t refcount,
                 tchecker::clock_id_t const * refmap,
                 tchecker::clock_reset_container_t const & resets)
      {
        assert(dbm != nullptr);
        assert(! tchecker::offset_dbm::is_empty_0(dbm, dim));
        assert(tchecker::offset_dbm::is_tight(dbm, dim));
        assert(1 <= refcount);
        assert(refcount <= dim);
        assert(refmap != nullptr);
        
        for (tchecker::clock_reset_t const & r : resets) {
          assert(r.left_id() < dim);
          assert(r.right_id() < dim);
          
          if ((r.right_id() != refmap[r.left_id()]) || (r.value() != 0))
            throw std::invalid_argument("invalid reset: not a reset to reference clock");
          
          tchecker::offset_dbm::reset_to_refclock(dbm, dim, r.left_id(), refcount, refmap);
        }
      }
      
      
      // sync_zone_computer_t
      
      sync_zone_computer_t::sync_zone_computer_t(tchecker::clock_id_t offset_dim,
                                                 tchecker::clock_id_t refcount,
                                                 tchecker::clock_id_t const * refmap)
      : _offset_dim(offset_dim), _refcount(refcount), _refmap(nullptr)
      {
        _offset_dbm = new tchecker::dbm::db_t[_offset_dim * _offset_dim];
        
        _refmap = new tchecker::clock_id_t[_offset_dim];
        std::memcpy(_refmap, refmap, _offset_dim * sizeof(*_refmap));
      }
      
      
      sync_zone_computer_t::sync_zone_computer_t(tchecker::offset_dbm::details::sync_zone_computer_t const & c)
      : _offset_dim(c._offset_dim), _refcount(c._refcount)
      {
        _offset_dbm = new tchecker::dbm::db_t[_offset_dim * _offset_dim];
        std::memcpy(_offset_dbm, c._offset_dbm, _offset_dim * _offset_dim * sizeof(*_offset_dbm));
        
        _refmap = new tchecker::clock_id_t[_offset_dim];
        std::memcpy(_refmap, c._refmap, _offset_dim * sizeof(*_refmap));
      }
      
      
      sync_zone_computer_t::sync_zone_computer_t(tchecker::offset_dbm::details::sync_zone_computer_t && c)
      : _offset_dbm(c._offset_dbm),
      _offset_dim(std::move(c._offset_dim)),
      _refcount(std::move(c._refcount)),
      _refmap(c._refmap)
      {
        c._offset_dbm = nullptr;
        c._refmap = nullptr;
      }
      
      
      sync_zone_computer_t::~sync_zone_computer_t()
      {
        delete[] _offset_dbm;
        delete[] _refmap;
      }
      
      
      tchecker::offset_dbm::details::sync_zone_computer_t &
      sync_zone_computer_t::operator= (tchecker::offset_dbm::details::sync_zone_computer_t const & c)
      {
        if (this != std::addressof(c)) {
          _offset_dim = c._offset_dim;
          
          delete[] _offset_dbm;
          _offset_dbm = new tchecker::dbm::db_t[_offset_dim * _offset_dim];
          std::memcpy(_offset_dbm, c._offset_dbm, _offset_dim * _offset_dim * sizeof(*_offset_dbm));
          
          _refcount = c._refcount;
          
          delete[] _refmap;
          _refmap = new tchecker::clock_id_t[_offset_dim];
          std::memcpy(_refmap, c._refmap, _offset_dim * sizeof(*_refmap));
        }
        return *this;
      }
      
      
      tchecker::offset_dbm::details::sync_zone_computer_t &
      sync_zone_computer_t::operator= (tchecker::offset_dbm::details::sync_zone_computer_t && c)
      {
        if (this != std::addressof(c)) {
          _offset_dim = std::move(c._offset_dim);
          
          delete[] _offset_dbm;
          _offset_dbm = c._offset_dbm;
          c._offset_dbm = nullptr;
          
          _refcount = std::move(c._refcount);
          
          delete[] _refmap;
          _refmap = c._refmap;
          c._refmap = nullptr;
        }
        return *this;
      }
      
      
      enum tchecker::dbm::status_t
      sync_zone_computer_t::sync_zone(tchecker::dbm::db_t const * const offset_dbm,
                                      tchecker::clock_id_t offset_dim,
                                      tchecker::dbm::db_t * dbm,
                                      tchecker::clock_id_t dim)
      {
        assert(offset_dbm != nullptr);
        assert(offset_dim == _offset_dim);
        assert(dbm != nullptr);
        assert(dim == _offset_dim - _refcount + 1);
        
        std::memcpy(_offset_dbm, offset_dbm, _offset_dim * _offset_dim * sizeof(*_offset_dbm));
        
        auto res = tchecker::offset_dbm::synchronize(_offset_dbm, _offset_dim, _refcount);
        
        if (res == tchecker::dbm::NON_EMPTY)
          tchecker::offset_dbm::to_dbm(_offset_dbm, _offset_dim, _refcount, _refmap, dbm, dim);
        
        return res;
      }
      
    } // end of namespace details
    
  } // end of namespace offset_dbm
  
} // end of namespace tchecker
