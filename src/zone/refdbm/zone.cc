/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/zone/refdbm/zone.hh"
#include "tchecker/dbm/refdbm.hh"

namespace tchecker {

namespace refdbm {

tchecker::refdbm::zone_t & zone_t::operator=(tchecker::refdbm::zone_t const & zone)
{
  if (_ref_clocks->size() != zone._ref_clocks->size())
    throw std::invalid_argument("Zone reference clocks mismatch");

  if (this != &zone)
    memcpy(dbm_ptr(), zone.dbm_ptr(), _ref_clocks->size() * _ref_clocks->size() * sizeof(tchecker::dbm::db_t));

  return *this;
}

bool zone_t::is_empty() const { return tchecker::refdbm::is_empty_0(dbm_ptr(), *_ref_clocks); }

bool zone_t::is_universal_positive() const { return tchecker::refdbm::is_universal_positive(dbm_ptr(), *_ref_clocks); }

bool zone_t::operator==(tchecker::refdbm::zone_t const & zone) const
{
  return (_ref_clocks == zone._ref_clocks) && tchecker::refdbm::is_equal(dbm_ptr(), zone.dbm_ptr(), *_ref_clocks);
}

bool zone_t::operator==(tchecker::zone_t const & zone) const
{
  tchecker::refdbm::zone_t const & refdbm_zone = dynamic_cast<tchecker::refdbm::zone_t const &>(zone);
  return (*this == refdbm_zone);
}

bool zone_t::operator!=(tchecker::refdbm::zone_t const & zone) const { return !(*this == zone); }

bool zone_t::operator!=(tchecker::zone_t const & zone) const { return !(*this == zone); }

bool zone_t::operator<=(tchecker::refdbm::zone_t const & zone) const
{
  return (_ref_clocks == zone._ref_clocks) && tchecker::refdbm::is_le(dbm_ptr(), zone.dbm_ptr(), *_ref_clocks);
}

bool zone_t::operator<=(tchecker::zone_t const & zone) const
{
  tchecker::refdbm::zone_t const & refdbm_zone = dynamic_cast<tchecker::refdbm::zone_t const &>(zone);
  return (*this <= refdbm_zone);
}

bool zone_t::am_le(tchecker::refdbm::zone_t const & zone, tchecker::clockbounds::map_t const & m) const
{
  return (_ref_clocks == zone._ref_clocks) && tchecker::refdbm::is_am_le(dbm_ptr(), zone.dbm_ptr(), *_ref_clocks, m.ptr());
}

bool zone_t::am_le(tchecker::zone_t const & zone, tchecker::clockbounds::map_t const & m) const
{
  tchecker::refdbm::zone_t const & refdbm_zone = dynamic_cast<tchecker::refdbm::zone_t const &>(zone);
  return am_le(refdbm_zone, m);
}

bool zone_t::alu_le(tchecker::refdbm::zone_t const & zone, tchecker::clockbounds::map_t const & l,
                    tchecker::clockbounds::map_t const & u) const
{
  return (_ref_clocks == zone._ref_clocks) &&
         tchecker::refdbm::is_alu_le(dbm_ptr(), zone.dbm_ptr(), *_ref_clocks, l.ptr(), u.ptr());
}

bool zone_t::alu_le(tchecker::zone_t const & zone, tchecker::clockbounds::map_t const & l,
                    tchecker::clockbounds::map_t const & u) const
{
  tchecker::refdbm::zone_t const & refdbm_zone = dynamic_cast<tchecker::refdbm::zone_t const &>(zone);
  return alu_le(refdbm_zone, l, u);
}

int zone_t::lexical_cmp(tchecker::refdbm::zone_t const & zone) const
{
  return tchecker::refdbm::lexical_cmp(dbm_ptr(), *_ref_clocks, zone.dbm_ptr(), *zone._ref_clocks);
}

int zone_t::lexical_cmp(tchecker::zone_t const & zone) const
{
  tchecker::refdbm::zone_t const & refdbm_zone = dynamic_cast<tchecker::refdbm::zone_t const &>(zone);
  return lexical_cmp(refdbm_zone);
}

std::size_t zone_t::hash() const
{
  std::size_t h = tchecker::refdbm::hash(dbm_ptr(), *_ref_clocks);
  boost::hash_combine(h, _ref_clocks.get());
  return h;
}

std::size_t zone_t::dim() const { return _ref_clocks->size(); }

std::shared_ptr<tchecker::reference_clock_variables_t const> zone_t::reference_clock_variables() const { return _ref_clocks; }

std::ostream & zone_t::output(std::ostream & os, tchecker::clock_index_t const & index) const
{
  return tchecker::refdbm::output(os, dbm_ptr(), *_ref_clocks);
}

tchecker::dbm::db_t * zone_t::dbm() { return dbm_ptr(); }

tchecker::dbm::db_t const * zone_t::dbm() const { return dbm_ptr(); }

void zone_t::to_dbm(tchecker::dbm::db_t * dbm) const { memcpy(dbm, dbm_ptr(), dim() * dim() * sizeof(tchecker::dbm::db_t)); }

zone_t::zone_t(std::shared_ptr<tchecker::reference_clock_variables_t const> const & ref_clocks) : _ref_clocks(ref_clocks)
{
  tchecker::refdbm::universal(dbm_ptr(), *_ref_clocks);
}

zone_t::zone_t(tchecker::refdbm::zone_t const & zone) : _ref_clocks(zone._ref_clocks) { zone.to_dbm(dbm_ptr()); }

zone_t::~zone_t() = default;

} // end of namespace refdbm

} // end of namespace tchecker
