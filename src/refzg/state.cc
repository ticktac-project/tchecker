/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/refzg/state.hh"

namespace tchecker {

namespace refzg {

state_t::state_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                 tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone)
    : tchecker::ta::state_t(vloc, intval), _zone(zone)
{
  assert(_zone.ptr() != nullptr);
}

state_t::state_t(tchecker::ta::state_t const & s, tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                 tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone)
    : tchecker::ta::state_t(s, vloc, intval), _zone(zone)
{
  assert(_zone.ptr() != nullptr);
}

bool operator==(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2)
{
  return tchecker::ta::operator==(s1, s2) && (s1.zone() == s2.zone());
}

bool operator!=(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2) { return !(s1 == s2); }

std::size_t hash_value(tchecker::refzg::state_t const & s)
{
  std::size_t h = tchecker::ta::hash_value(s);
  boost::hash_combine(h, s.zone());
  return h;
}

bool operator<=(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2)
{
  return tchecker::ta::operator==(s1, s2) && (s1.zone() <= s2.zone());
}

bool is_alu_star_le(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2,
                    tchecker::clockbounds::map_t const & l, tchecker::clockbounds::map_t const & u)
{
  return tchecker::ta::operator==(s1, s2) && s1.zone().is_alu_star_le(s2.zone(), l, u);
}

bool is_time_elapse_alu_star_le(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2,
                                tchecker::clockbounds::map_t const & l, tchecker::clockbounds::map_t const & u)
{
  return tchecker::ta::operator==(s1, s2) && s1.zone().is_time_elapse_alu_star_le(s2.zone(), l, u);
}

bool is_sync_alu_le(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2,
                    tchecker::clockbounds::map_t const & l, tchecker::clockbounds::map_t const & u)
{
  return tchecker::ta::operator==(s1, s2) && s1.zone().is_sync_alu_le(s2.zone(), l, u);
}

int lexical_cmp(tchecker::refzg::state_t const & s1, tchecker::refzg::state_t const & s2)
{
  int ta_cmp = tchecker::ta::lexical_cmp(s1, s2);
  if (ta_cmp != 0)
    return ta_cmp;
  return s1.zone().lexical_cmp(s2.zone());
}

} // end of namespace refzg

} // end of namespace tchecker
