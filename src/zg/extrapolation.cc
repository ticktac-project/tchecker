/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/zg/extrapolation.hh"
#include "tchecker/clockbounds/solver.hh"

namespace tchecker {

namespace zg {

/* no_extrapolation_t */

void no_extrapolation_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc) {}

/* global_lu_extrapolation_t */

namespace details {

global_lu_extrapolation_t::global_lu_extrapolation_t(
    std::shared_ptr<tchecker::clockbounds::global_lu_map_t const> const & clock_bounds)
    : _clock_bounds(clock_bounds)
{
}

} // end of namespace details

/* global_extra_lu_t */

void global_extra_lu_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  tchecker::dbm::extra_lu(dbm, dim, _clock_bounds->L().ptr(), _clock_bounds->U().ptr());
}

/* global_extra_lu_plus_t */

void global_extra_lu_plus_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  tchecker::dbm::extra_lu_plus(dbm, dim, _clock_bounds->L().ptr(), _clock_bounds->U().ptr());
}

/* local_lu_extrapolation_t */

namespace details {

local_lu_extrapolation_t::local_lu_extrapolation_t(
    std::shared_ptr<tchecker::clockbounds::local_lu_map_t const> const & clock_bounds)
    : _l(nullptr), _u(nullptr), _clock_bounds(clock_bounds)
{
  _l = tchecker::clockbounds::allocate_map(_clock_bounds->clock_number());
  _u = tchecker::clockbounds::allocate_map(_clock_bounds->clock_number());
}

local_lu_extrapolation_t::local_lu_extrapolation_t(tchecker::zg::details::local_lu_extrapolation_t const & e)
    : _clock_bounds(e._clock_bounds)
{
  _l = tchecker::clockbounds::clone_map(*e._l);
  _u = tchecker::clockbounds::clone_map(*e._u);
}

local_lu_extrapolation_t::local_lu_extrapolation_t(tchecker::zg::details::local_lu_extrapolation_t && e)
    : _l(std::move(e._l)), _u(std::move(e._u)), _clock_bounds(std::move(e._clock_bounds))
{
  e._l = nullptr;
  e._u = nullptr;
}

local_lu_extrapolation_t::~local_lu_extrapolation_t()
{
  tchecker::clockbounds::deallocate_map(_l);
  tchecker::clockbounds::deallocate_map(_u);
}

tchecker::zg::details::local_lu_extrapolation_t &
local_lu_extrapolation_t::operator=(tchecker::zg::details::local_lu_extrapolation_t const & e)
{
  if (this != &e) {
    _clock_bounds = e._clock_bounds;
    tchecker::clockbounds::deallocate_map(_l);
    _l = tchecker::clockbounds::clone_map(*e._l);
    tchecker::clockbounds::deallocate_map(_u);
    _u = tchecker::clockbounds::clone_map(*e._u);
  }
  return *this;
}

tchecker::zg::details::local_lu_extrapolation_t &
local_lu_extrapolation_t::operator=(tchecker::zg::details::local_lu_extrapolation_t && e)
{
  if (this != &e) {
    _clock_bounds = std::move(e._clock_bounds);
    _l = std::move(e._l);
    _u = std::move(e._u);
    e._l = nullptr;
    e._u = nullptr;
  }
  return *this;
}

} // end of namespace details

/* local_extra_lu_t */

void local_extra_lu_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  _clock_bounds->bounds(vloc, *_l, *_u);
  tchecker::dbm::extra_lu(dbm, dim, _l->ptr(), _u->ptr());
}

/* local_extra_lu_plus_t */

void local_extra_lu_plus_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  _clock_bounds->bounds(vloc, *_l, *_u);
  tchecker::dbm::extra_lu_plus(dbm, dim, _l->ptr(), _u->ptr());
}

/* global_m_extrapolation_t */

namespace details {

global_m_extrapolation_t::global_m_extrapolation_t(
    std::shared_ptr<tchecker::clockbounds::global_m_map_t const> const & clock_bounds)
    : _clock_bounds(clock_bounds)
{
}

} // end of namespace details

/* global_extra_m_t */

void global_extra_m_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  tchecker::dbm::extra_m(dbm, dim, _clock_bounds->M().ptr());
}

/* global_extra_m_plus_t */

void global_extra_m_plus_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  tchecker::dbm::extra_m_plus(dbm, dim, _clock_bounds->M().ptr());
}

/* local_m_extrapolation_t */

namespace details {

local_m_extrapolation_t::local_m_extrapolation_t(
    std::shared_ptr<tchecker::clockbounds::local_m_map_t const> const & clock_bounds)
    : _m(nullptr), _clock_bounds(clock_bounds)
{
  _m = tchecker::clockbounds::allocate_map(_clock_bounds->clock_number());
}

local_m_extrapolation_t::local_m_extrapolation_t(tchecker::zg::details::local_m_extrapolation_t const & e)
    : _m(nullptr), _clock_bounds(e._clock_bounds)
{
  _m = tchecker::clockbounds::clone_map(*e._m);
}

local_m_extrapolation_t::local_m_extrapolation_t(tchecker::zg::details::local_m_extrapolation_t && e)
    : _m(std::move(e._m)), _clock_bounds(std::move(e._clock_bounds))
{
  e._m = nullptr;
}

local_m_extrapolation_t::~local_m_extrapolation_t() { tchecker::clockbounds::deallocate_map(_m); }

tchecker::zg::details::local_m_extrapolation_t &
local_m_extrapolation_t::operator=(tchecker::zg::details::local_m_extrapolation_t const & e)
{
  if (this != &e) {
    tchecker::clockbounds::deallocate_map(_m);
    _m = tchecker::clockbounds::clone_map(*e._m);
    _clock_bounds = e._clock_bounds;
  }
  return *this;
}

tchecker::zg::details::local_m_extrapolation_t &
local_m_extrapolation_t::operator=(tchecker::zg::details::local_m_extrapolation_t && e)
{
  if (this != &e) {
    _m = std::move(e._m);
    e._m = nullptr;
    _clock_bounds = std::move(e._clock_bounds);
  }
  return *this;
}

} // namespace details

/* local_extra_m_t */

void local_extra_m_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  _clock_bounds->bounds(vloc, *_m);
  tchecker::dbm::extra_m(dbm, dim, _m->ptr());
}

/* local_extra_m_plus_t */

void local_extra_m_plus_t::extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc)
{
  assert(dim == _clock_bounds->clock_number() + 1);
  _clock_bounds->bounds(vloc, *_m);
  tchecker::dbm::extra_m_plus(dbm, dim, _m->ptr());
}

/* factories */

tchecker::zg::extrapolation_t * extrapolation_factory(enum extrapolation_type_t extrapolation_type,
                                                      tchecker::ta::system_t const & system)
{
  if (extrapolation_type == tchecker::zg::NO_EXTRAPOLATION)
    return new tchecker::zg::no_extrapolation_t;

  std::unique_ptr<tchecker::clockbounds::clockbounds_t> clock_bounds{tchecker::clockbounds::compute_clockbounds(system)};
  if (clock_bounds.get() == nullptr)
    return nullptr;

  return tchecker::zg::extrapolation_factory(extrapolation_type, *clock_bounds);
}

tchecker::zg::extrapolation_t * extrapolation_factory(enum extrapolation_type_t extrapolation_type,
                                                      tchecker::clockbounds::clockbounds_t const & clock_bounds)
{
  if (extrapolation_type == tchecker::zg::NO_EXTRAPOLATION)
    return new tchecker::zg::no_extrapolation_t;
  switch (extrapolation_type) {
  case tchecker::zg::EXTRA_LU_GLOBAL:
    return new tchecker::zg::global_extra_lu_t{clock_bounds.global_lu_map()};
  case tchecker::zg::EXTRA_LU_LOCAL:
    return new tchecker::zg::local_extra_lu_t{clock_bounds.local_lu_map()};
  case tchecker::zg::EXTRA_LU_PLUS_GLOBAL:
    return new tchecker::zg::global_extra_lu_plus_t{clock_bounds.global_lu_map()};
  case tchecker::zg::EXTRA_LU_PLUS_LOCAL:
    return new tchecker::zg::local_extra_lu_plus_t{clock_bounds.local_lu_map()};
  case tchecker::zg::EXTRA_M_GLOBAL:
    return new tchecker::zg::global_extra_m_t{clock_bounds.global_m_map()};
  case tchecker::zg::EXTRA_M_LOCAL:
    return new tchecker::zg::local_extra_m_t{clock_bounds.local_m_map()};
  case tchecker::zg::EXTRA_M_PLUS_GLOBAL:
    return new tchecker::zg::global_extra_m_plus_t{clock_bounds.global_m_map()};
  case tchecker::zg::EXTRA_M_PLUS_LOCAL:
    return new tchecker::zg::local_extra_m_plus_t{clock_bounds.local_m_map()};
  default:
    throw std::invalid_argument("Unknown zone extrapolation");
  }
}

} // end of namespace zg

} // end of namespace tchecker
