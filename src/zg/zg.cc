/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/zg/zg.hh"
#include "tchecker/dbm/db.hh"

namespace tchecker {

namespace zg {

/* Semantics functions */

enum tchecker::state_status_t initial(tchecker::ta::system_t const & system,
                                      tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                      tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                      tchecker::intrusive_shared_ptr_t<tchecker::zg::shared_zone_t> const & zone,
                                      tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                      tchecker::clock_constraint_container_t & invariant, tchecker::zg::semantics_t & semantics,
                                      tchecker::zg::extrapolation_t & extrapolation,
                                      tchecker::zg::initial_value_t const & initial_range)
{
  enum tchecker::state_status_t status = tchecker::ta::initial(system, vloc, intval, vedge, invariant, initial_range);
  if (status != tchecker::STATE_OK)
    return status;

  tchecker::dbm::db_t * dbm = zone->dbm();
  tchecker::clock_id_t dim = zone->dim();
  bool delay_allowed = tchecker::ta::delay_allowed(system, *vloc);

  status = semantics.initial(dbm, dim, delay_allowed, invariant);
  if (status != tchecker::STATE_OK)
    return status;

  extrapolation.extrapolate(dbm, dim, *vloc);

  return tchecker::STATE_OK;
}

enum tchecker::state_status_t next(tchecker::ta::system_t const & system,
                                   tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                   tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                   tchecker::intrusive_shared_ptr_t<tchecker::zg::shared_zone_t> const & zone,
                                   tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                   tchecker::clock_constraint_container_t & src_invariant,
                                   tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                                   tchecker::clock_constraint_container_t & tgt_invariant,
                                   tchecker::zg::semantics_t & semantics, tchecker::zg::extrapolation_t & extrapolation,
                                   tchecker::zg::outgoing_edges_value_t const & edges)
{
  bool src_delay_allowed = tchecker::ta::delay_allowed(system, *vloc);

  enum tchecker::state_status_t status =
      tchecker::ta::next(system, vloc, intval, vedge, src_invariant, guard, reset, tgt_invariant, edges);
  if (status != tchecker::STATE_OK)
    return status;

  tchecker::dbm::db_t * dbm = zone->dbm();
  tchecker::clock_id_t dim = zone->dim();
  bool tgt_delay_allowed = tchecker::ta::delay_allowed(system, *vloc);

  status = semantics.next(dbm, dim, src_delay_allowed, src_invariant, guard, reset, tgt_delay_allowed, tgt_invariant);
  if (status != tchecker::STATE_OK)
    return status;

  extrapolation.extrapolate(dbm, dim, *vloc);

  return tchecker::STATE_OK;
}

/* zg_t */

zg_t::zg_t(std::shared_ptr<tchecker::ta::system_t const> const & system,
           std::unique_ptr<tchecker::zg::semantics_t> && semantics,
           std::unique_ptr<tchecker::zg::extrapolation_t> && extrapolation, std::size_t block_size, tchecker::gc_t & gc)
    : _system(system), _semantics(std::move(semantics)), _extrapolation(std::move(extrapolation)),
      _state_allocator(block_size, block_size, _system->processes_count(), block_size,
                       _system->intvars_count(tchecker::VK_FLATTENED), block_size,
                       _system->clocks_count(tchecker::VK_FLATTENED) + 1),
      _transition_allocator(block_size, block_size, _system->processes_count())
{
  _state_allocator.enroll(gc);
  _transition_allocator.enroll(gc);
}

tchecker::zg::initial_range_t zg_t::initial_edges() { return tchecker::zg::initial_edges(*_system); }

std::tuple<enum tchecker::state_status_t, tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t>
zg_t::initial(tchecker::zg::initial_value_t const & v)
{
  tchecker::zg::state_sptr_t s = _state_allocator.construct();
  tchecker::zg::transition_sptr_t t = _transition_allocator.construct();
  enum tchecker::state_status_t status = tchecker::zg::initial(*_system, *s, *t, *_semantics, *_extrapolation, v);
  return std::make_tuple(status, s, t);
}

tchecker::zg::outgoing_edges_range_t zg_t::outgoing_edges(tchecker::zg::const_state_sptr_t const & s)
{
  return tchecker::zg::outgoing_edges(*_system, s->vloc_ptr());
}

std::tuple<enum tchecker::state_status_t, tchecker::zg::state_sptr_t, tchecker::zg::transition_sptr_t>
zg_t::next(tchecker::zg::const_state_sptr_t const & s, tchecker::zg::outgoing_edges_value_t const & v)
{
  tchecker::zg::state_sptr_t nexts = _state_allocator.construct_from_state(*s);
  tchecker::zg::transition_sptr_t t = _transition_allocator.construct();
  enum tchecker::state_status_t status = tchecker::zg::next(*_system, *nexts, *t, *_semantics, *_extrapolation, v);
  return std::make_tuple(status, nexts, t);
}

tchecker::ta::system_t const & zg_t::system() const { return *_system; }

/* factory */

tchecker::zg::zg_t * factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                             enum tchecker::zg::semantics_type_t semantics_type,
                             enum tchecker::zg::extrapolation_type_t extrapolation_type, std::size_t block_size,
                             tchecker::gc_t & gc)
{
  std::unique_ptr<tchecker::zg::extrapolation_t> extrapolation{
      tchecker::zg::extrapolation_factory(extrapolation_type, *system)};
  if (extrapolation.get() == nullptr)
    return nullptr;
  std::unique_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(semantics_type)};
  return new tchecker::zg::zg_t(system, std::move(semantics), std::move(extrapolation), block_size, gc);
}

tchecker::zg::zg_t * factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                             enum tchecker::zg::semantics_type_t semantics_type,
                             enum tchecker::zg::extrapolation_type_t extrapolation_type,
                             tchecker::clockbounds::clockbounds_t const & clock_bounds, std::size_t block_size,
                             tchecker::gc_t & gc)
{
  std::unique_ptr<tchecker::zg::extrapolation_t> extrapolation{
      tchecker::zg::extrapolation_factory(extrapolation_type, clock_bounds)};
  if (extrapolation.get() == nullptr)
    return nullptr;
  std::unique_ptr<tchecker::zg::semantics_t> semantics{tchecker::zg::semantics_factory(semantics_type)};
  return new tchecker::zg::zg_t(system, std::move(semantics), std::move(extrapolation), block_size, gc);
}

} // end of namespace zg

} // end of namespace tchecker
