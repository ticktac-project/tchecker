/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <string>

#include "tchecker/dbm/offset_dbm.hh"
#include "tchecker/zone/offset_dbm/zone.hh"


namespace tchecker {
  
  namespace offset_dbm {
    
    tchecker::offset_dbm::zone_t & zone_t::operator= (tchecker::offset_dbm::zone_t const & zone)
    {
      if (_dim != zone._dim)
        throw std::invalid_argument("Zone dimension mismatch");
      
      if (this != &zone)
        memcpy(dbm_ptr(), zone.dbm_ptr(), _dim * _dim * sizeof(tchecker::dbm::db_t));
      
      return *this;
    }
    
    
    bool zone_t::empty() const
    {
      return tchecker::offset_dbm::is_empty_0(dbm_ptr(), _dim);
    }
    
    
    bool zone_t::operator==(tchecker::offset_dbm::zone_t const & zone) const
    {
      if (_dim != zone._dim)
        return false;
      return tchecker::offset_dbm::is_equal(dbm_ptr(), zone.dbm_ptr(), _dim);
    }
    
    
    bool zone_t::operator!=(tchecker::offset_dbm::zone_t const & zone) const
    {
      return ! (*this == zone);
    }
    
    
    bool zone_t::operator<=(tchecker::offset_dbm::zone_t const & zone) const
    {
      if (_dim != zone._dim)
        return false;
      return tchecker::offset_dbm::is_le(dbm_ptr(), zone.dbm_ptr(), _dim);
    }
    
    
    int zone_t::lexical_cmp(tchecker::offset_dbm::zone_t const & zone) const
    {
      return tchecker::offset_dbm::lexical_cmp(dbm_ptr(), _dim, zone.dbm_ptr(), zone._dim);
    }
  
    
    std::size_t zone_t::hash() const
    {
      return tchecker::offset_dbm::hash(dbm_ptr(), _dim);
    }
    
    
    std::ostream & zone_t::output(std::ostream & os, tchecker::clock_index_t const & index) const
    {
      return tchecker::offset_dbm::output(os, dbm_ptr(), _dim, [&] (tchecker::clock_id_t id) { return index.value(id); });
    }
    
    
    tchecker::dbm::db_t * zone_t::dbm()
    {
      return dbm_ptr();
    }
    
    
    tchecker::dbm::db_t const * zone_t::dbm() const
    {
      return dbm_ptr();
    }
    
    
    zone_t::zone_t(tchecker::clock_id_t dim) : _dim(dim)
    {}
    
    
    zone_t::zone_t(tchecker::offset_dbm::zone_t const & zone) : _dim(zone._dim)
    {
      memcpy(dbm_ptr(), zone.dbm_ptr(), _dim * _dim * sizeof(tchecker::dbm::db_t));
    }
    
    
    zone_t::~zone_t() = default;
    
    
    
    
    // Allocation and deallocation
    
    void zone_destruct_and_deallocate(tchecker::offset_dbm::zone_t * zone)
    {
      tchecker::offset_dbm::zone_t::destruct(zone);
      delete[] reinterpret_cast<char *>(zone);
    }
    
  } // end of namespace offset_dbm
  
} // end of namespace tchecker
