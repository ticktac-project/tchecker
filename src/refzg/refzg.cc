/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <memory>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/dbm/db.hh"
#include "tchecker/dbm/refdbm.hh"
#include "tchecker/refzg/refzg.hh"
#include "tchecker/variables/static_analysis.hh"

namespace tchecker {

namespace refzg {

/* Semantics functions */

tchecker::state_status_t initial(tchecker::ta::system_t const & system,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                 tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                 tchecker::clock_constraint_container_t & invariant, tchecker::refzg::semantics_t & semantics,
                                 tchecker::integer_t spread, tchecker::refzg::initial_value_t const & initial_range)
{
  tchecker::state_status_t status = tchecker::ta::initial(system, vloc, intval, vedge, invariant, initial_range);
  if (status != tchecker::STATE_OK)
    return status;

  std::shared_ptr<tchecker::reference_clock_variables_t const> r = zone->reference_clock_variables();

  boost::dynamic_bitset<> delay_allowed = tchecker::ta::delay_allowed(system, *r, *vloc);

  tchecker::dbm::db_t * rdbm = zone->dbm();

  return semantics.initial(rdbm, *r, delay_allowed, invariant, spread);
}

tchecker::state_status_t final(tchecker::ta::system_t const & system,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                               tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                               tchecker::clock_constraint_container_t & invariant, tchecker::refzg::semantics_t & semantics,
                               tchecker::integer_t spread, tchecker::refzg::final_value_t const & final_range)
{
  tchecker::state_status_t status = tchecker::ta::final(system, vloc, intval, vedge, invariant, final_range);
  if (status != tchecker::STATE_OK)
    return status;

  std::shared_ptr<tchecker::reference_clock_variables_t const> r = zone->reference_clock_variables();

  boost::dynamic_bitset<> delay_allowed = tchecker::ta::delay_allowed(system, *r, *vloc);

  tchecker::dbm::db_t * rdbm = zone->dbm();

  return semantics.final(rdbm, *r, delay_allowed, invariant, spread);
}

tchecker::state_status_t next(tchecker::ta::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                              tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::clock_constraint_container_t & src_invariant,
                              tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                              tchecker::clock_constraint_container_t & tgt_invariant, tchecker::refzg::semantics_t & semantics,
                              tchecker::integer_t spread, tchecker::refzg::outgoing_edges_value_t const & edges)
{
  std::shared_ptr<tchecker::reference_clock_variables_t const> r = zone->reference_clock_variables();

  boost::dynamic_bitset<> const src_delay_allowed = tchecker::ta::delay_allowed(system, *r, *vloc);

  tchecker::state_status_t status =
      tchecker::ta::next(system, vloc, intval, vedge, src_invariant, guard, reset, tgt_invariant, edges);
  if (status != tchecker::STATE_OK)
    return status;

  boost::dynamic_bitset<> const tgt_delay_allowed = tchecker::ta::delay_allowed(system, *r, *vloc);
  boost::dynamic_bitset<> const sync_refclocks = tchecker::ta::sync_refclocks(system, *r, *vedge);

  tchecker::dbm::db_t * rdbm = zone->dbm();
  return semantics.next(rdbm, *r, src_delay_allowed, src_invariant, sync_refclocks, guard, reset, tgt_delay_allowed,
                        tgt_invariant, spread);
}

tchecker::state_status_t prev(tchecker::ta::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                              tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::clock_constraint_container_t & src_invariant,
                              tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                              tchecker::clock_constraint_container_t & tgt_invariant, tchecker::refzg::semantics_t & semantics,
                              tchecker::integer_t spread, tchecker::refzg::incoming_edges_value_t const & edges)
{
  std::shared_ptr<tchecker::reference_clock_variables_t const> r = zone->reference_clock_variables();

  boost::dynamic_bitset<> const tgt_delay_allowed = tchecker::ta::delay_allowed(system, *r, *vloc);

  tchecker::state_status_t status =
      tchecker::ta::prev(system, vloc, intval, vedge, src_invariant, guard, reset, tgt_invariant, edges);
  if (status != tchecker::STATE_OK)
    return status;

  boost::dynamic_bitset<> const src_delay_allowed = tchecker::ta::delay_allowed(system, *r, *vloc);
  boost::dynamic_bitset<> const sync_refclocks = tchecker::ta::sync_refclocks(system, *r, *vedge);

  tchecker::dbm::db_t * rdbm = zone->dbm();
  return semantics.prev(rdbm, *r, src_delay_allowed, src_invariant, sync_refclocks, guard, reset, tgt_delay_allowed,
                        tgt_invariant, spread);
}

/* labels */

boost::dynamic_bitset<> labels(tchecker::ta::system_t const & system, tchecker::refzg::state_t const & s)
{
  return tchecker::ta::labels(system, s);
}

/* is_valid_final */

bool is_valid_final(tchecker::ta::system_t const & system, tchecker::refzg::state_t const & s)
{
  return !s.zone().is_empty() && s.zone().is_synchronizable();
}

/* is_initial */

bool is_initial(tchecker::ta::system_t const & system, tchecker::refzg::zone_t const & zone)
{
  return tchecker::refdbm::contains_zero(zone.dbm(), *zone.reference_clock_variables());
}

bool is_initial(tchecker::ta::system_t const & system, tchecker::refzg::state_t const & s)
{
  return tchecker::ta::is_initial(system, s) && tchecker::refzg::is_initial(system, s.zone());
}

/* attributes */

void attributes(tchecker::ta::system_t const & system, tchecker::refzg::state_t const & s,
                std::map<std::string, std::string> & m)
{
  tchecker::ta::attributes(system, s, m);
  m["zone"] = tchecker::to_string(s.zone(), system.clock_variables().flattened().index());
}

void attributes(tchecker::ta::system_t const & system, tchecker::refzg::transition_t const & t,
                std::map<std::string, std::string> & m)
{
  tchecker::ta::attributes(system, t, m);
}

/* initialize */

tchecker::state_status_t initialize(tchecker::ta::system_t const & system,
                                    tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                    tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                    tchecker::intrusive_shared_ptr_t<tchecker::refzg::shared_zone_t> const & zone,
                                    tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                    tchecker::clock_constraint_container_t & invariant, tchecker::integer_t spread,
                                    std::map<std::string, std::string> const & attributes)
{
  // initialize vloc, intval and vedge from ta
  auto state_status = tchecker::ta::initialize(system, vloc, intval, vedge, invariant, attributes);
  if (state_status != STATE_OK)
    return state_status;

  std::shared_ptr<tchecker::reference_clock_variables_t const> const r = zone->reference_clock_variables();

  // initialize zone from attributes["zone"]
  tchecker::clock_constraint_container_t clk_constraints;
  try {
    tchecker::clock_variables_t clock_variables = tchecker::clock_variables(*r, system.clock_variables());
    tchecker::from_string(clk_constraints, clock_variables, attributes.at("zone"));
  }
  catch (...) {
    return tchecker::STATE_BAD;
  }

  tchecker::dbm::db_t * rdbm = zone->dbm();
  std::vector<tchecker::clock_id_t> const & refmap = r->refmap();
  tchecker::refdbm::universal_positive(rdbm, *r);
  // NB: do not use constrain(..., clk_constraints) as constraints are expressed over reference clocks
  // and offset clocks, and not system clocks
  for (tchecker::clock_constraint_t const & c : clk_constraints) {
    assert((c.id1() != tchecker::REFCLOCK_ID) || (c.id2() != tchecker::REFCLOCK_ID));
    tchecker::clock_id_t id1 = (c.id1() == tchecker::REFCLOCK_ID ? refmap[c.id2()] : c.id1());
    tchecker::clock_id_t id2 = (c.id2() == tchecker::REFCLOCK_ID ? refmap[c.id1()] : c.id2());
    tchecker::dbm::status_t zone_status = tchecker::refdbm::constrain(rdbm, *r, id1, id2, c.comparator(), c.value());
    if (zone_status == tchecker::dbm::EMPTY)
      return tchecker::STATE_BAD;
  }

  // Apply invariant
  tchecker::dbm::status_t zone_status = tchecker::refdbm::constrain(rdbm, *r, invariant);
  if (zone_status == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;

  // Apply spread
  zone_status = tchecker::refdbm::bound_spread(rdbm, *r, spread);
  if (zone_status == tchecker::dbm::EMPTY)
    return tchecker::STATE_CLOCKS_EMPTY_SPREAD;

  return tchecker::STATE_OK;
}

/* refzg_t */

refzg_t::refzg_t(std::shared_ptr<tchecker::ta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
                 std::shared_ptr<tchecker::reference_clock_variables_t const> const & r,
                 std::shared_ptr<tchecker::refzg::semantics_t> const & semantics, tchecker::integer_t spread,
                 std::size_t block_size, std::size_t table_size)
    : _system(system), _sharing_type(sharing_type), _r(r), _semantics(semantics), _spread(spread),
      _state_allocator(block_size, block_size, _system->processes_count(), block_size,
                       _system->intvars_count(tchecker::VK_FLATTENED), block_size, _r, table_size),
      _transition_allocator(block_size, block_size, _system->processes_count(), table_size)
{
  tchecker::variable_access_map_t va_map = tchecker::variable_access(*system);
  if (va_map.has_shared_variable() && (_r->refcount() > 1))
    throw std::invalid_argument("Zone graph with reference clocks is not sound for systems with shared variables");
}

initial_range_t refzg_t::initial_edges() { return tchecker::refzg::initial_edges(*_system); }

void refzg_t::initial(tchecker::refzg::initial_value_t const & init_edge, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::refzg::state_sptr_t s = _state_allocator.construct();
  tchecker::refzg::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::refzg::initial(*_system, *s, *t, *_semantics, _spread, init_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}

void refzg_t::initial(std::vector<sst_t> & v, tchecker::state_status_t mask) { tchecker::ts::initial(*this, v, mask); }

outgoing_edges_range_t refzg_t::outgoing_edges(tchecker::refzg::const_state_sptr_t const & s)
{
  return tchecker::refzg::outgoing_edges(*_system, s->vloc_ptr());
}

void refzg_t::next(tchecker::refzg::const_state_sptr_t const & s, outgoing_edges_value_t const & out_edge,
                   std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::refzg::state_sptr_t nexts = _state_allocator.clone(*s);
  tchecker::refzg::transition_sptr_t nextt = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::refzg::next(*_system, *nexts, *nextt, *_semantics, _spread, out_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(nexts);
      share(nextt);
    }
    v.push_back(std::make_tuple(status, nexts, nextt));
  }
}

void refzg_t::next(tchecker::refzg::const_state_sptr_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::next(*this, s, v, mask);
}

// Backward

final_range_t refzg_t::final_edges(boost::dynamic_bitset<> const & labels)
{
  return tchecker::refzg::final_edges(*_system, labels);
}

void refzg_t::final(final_value_t const & final_edge, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::refzg::state_sptr_t s = _state_allocator.construct();
  tchecker::refzg::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::refzg::final(*_system, *s, *t, *_semantics, _spread, final_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}

void refzg_t::final(boost::dynamic_bitset<> const & labels, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::final(*this, labels, v, mask);
}

incoming_edges_range_t refzg_t::incoming_edges(tchecker::refzg::const_state_sptr_t const & s)
{
  return tchecker::refzg::incoming_edges(*_system, s->vloc_ptr());
}

void refzg_t::prev(tchecker::refzg::const_state_sptr_t const & s, incoming_edges_value_t const & in_edge,
                   std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::refzg::state_sptr_t prevs = _state_allocator.clone(*s);
  tchecker::refzg::transition_sptr_t prevt = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::refzg::prev(*_system, *prevs, *prevt, *_semantics, _spread, in_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(prevs);
      share(prevt);
    }
    v.push_back(std::make_tuple(status, prevs, prevt));
  }
}

void refzg_t::prev(tchecker::refzg::const_state_sptr_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::prev(*this, s, v, mask);
}

// Builder

void refzg_t::build(std::map<std::string, std::string> const & attributes, std::vector<sst_t> & v,
                    tchecker::state_status_t mask)
{
  tchecker::refzg::state_sptr_t s = _state_allocator.construct();
  tchecker::refzg::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::refzg::initialize(*_system, *s, *t, _spread, attributes);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}

// Inspector

boost::dynamic_bitset<> refzg_t::labels(tchecker::refzg::const_state_sptr_t const & s) const
{
  return tchecker::refzg::labels(*_system, *s);
}

void refzg_t::attributes(tchecker::refzg::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const
{
  tchecker::refzg::attributes(*_system, *s, m);
}

void refzg_t::attributes(tchecker::refzg::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const
{
  tchecker::refzg::attributes(*_system, *t, m);
}

bool refzg_t::is_valid_final(tchecker::refzg::const_state_sptr_t const & s) const
{
  return tchecker::refzg::is_valid_final(*_system, *s);
}

bool refzg_t::is_initial(tchecker::refzg::const_state_sptr_t const & s) const
{
  return tchecker::refzg::is_initial(*_system, *s);
}

// Sharing

void refzg_t::share(tchecker::refzg::state_sptr_t & s) { _state_allocator.share(s); }

void refzg_t::share(tchecker::refzg::transition_sptr_t & t) { _transition_allocator.share(t); }

/* tools */

tchecker::refzg::state_sptr_t initial(tchecker::refzg::refzg_t & refzg, tchecker::vloc_t const & vloc,
                                      tchecker::state_status_t mask)
{
  std::vector<tchecker::refzg::refzg_t::sst_t> v;
  refzg.initial(v, mask);
  for (auto && [status, s, t] : v) {
    if (s->vloc() == vloc)
      return s;
  }
  return nullptr;
}

std::tuple<tchecker::refzg::state_sptr_t, tchecker::refzg::transition_sptr_t>
next(tchecker::refzg::refzg_t & refzg, tchecker::refzg::const_state_sptr_t const & s, tchecker::vedge_t const & vedge,
     tchecker::state_status_t mask)
{
  std::vector<tchecker::refzg::refzg_t::sst_t> v;
  refzg.next(s, v, mask);
  for (auto && [status, nexts, nextt] : v)
    if (nextt->vedge() == vedge)
      return std::make_tuple(nexts, nextt);
  return std::make_tuple(nullptr, nullptr);
}

/* factory */

// Factory of reference clock variables
static std::shared_ptr<tchecker::reference_clock_variables_t const>
reference_clocks_factory(enum tchecker::refzg::reference_clock_variables_type_t type, tchecker::ta::system_t const & system)
{
  switch (type) {
  case tchecker::refzg::SINGLE_REFERENCE_CLOCKS:
    return std::make_shared<tchecker::reference_clock_variables_t const>(
        tchecker::single_reference_clocks(system.clock_variables().flattened(), system.processes_count()));
  case tchecker::refzg::PROCESS_REFERENCE_CLOCKS: {
    tchecker::variable_access_map_t va_map = tchecker::variable_access(system);
    return std::make_shared<tchecker::reference_clock_variables_t const>(
        tchecker::process_reference_clocks(va_map, system.clock_variables().flattened(), system.processes_count()));
  }
  default:
    throw std::invalid_argument("Unknown reference clock variables type");
  }
}

tchecker::refzg::refzg_t * factory(std::shared_ptr<tchecker::ta::system_t const> const & system,
                                   enum tchecker::ts::sharing_type_t sharing_type,
                                   enum tchecker::refzg::reference_clock_variables_type_t refclocks_type,
                                   enum tchecker::refzg::semantics_type_t semantics_type, tchecker::integer_t spread,
                                   std::size_t block_size, std::size_t table_size)
{
  std::shared_ptr<tchecker::reference_clock_variables_t const> r(
      tchecker::refzg::reference_clocks_factory(refclocks_type, *system));
  std::shared_ptr<tchecker::refzg::semantics_t> semantics{tchecker::refzg::semantics_factory(semantics_type)};
  return new tchecker::refzg::refzg_t(system, sharing_type, r, semantics, spread, block_size, table_size);
}

} // end of namespace refzg

} // end of namespace tchecker
