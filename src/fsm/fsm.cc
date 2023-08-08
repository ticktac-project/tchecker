/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <vector>

#include "tchecker/fsm/fsm.hh"
#include "tchecker/variables/clocks.hh"

namespace tchecker {

namespace fsm {

/*!< Place holder clock constraint container */
static tchecker::clock_constraint_container_t place_holder_clkconstr;

/*!< Place holder clock reset container */
static tchecker::clock_reset_container_t place_holder_clkreset;

// Initial edges

tchecker::fsm::initial_range_t initial_edges(tchecker::ta::system_t const & system)
{
  return tchecker::ta::initial_edges(system);
}

// Initial state

tchecker::state_status_t initial(tchecker::ta::system_t const & system,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                 tchecker::fsm::initial_value_t const & initial_range)
{
  tchecker::state_status_t status = tchecker::ta::initial(system, vloc, intval, vedge, place_holder_clkconstr, initial_range);
  place_holder_clkconstr.clear();
  return status;
}

// Final edges

tchecker::fsm::final_range_t final_edges(tchecker::ta::system_t const & system, boost::dynamic_bitset<> const & labels)
{
  return tchecker::ta::final_edges(system, labels);
}

// Final state

tchecker::state_status_t final(tchecker::ta::system_t const & system,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                               tchecker::fsm::final_value_t const & final_value)
{
  tchecker::state_status_t status = tchecker::ta::final(system, vloc, intval, vedge, place_holder_clkconstr, final_value);
  place_holder_clkconstr.clear();
  return status;
}

// Outgoing edges

tchecker::fsm::outgoing_edges_range_t
outgoing_edges(tchecker::ta::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
{
  return tchecker::ta::outgoing_edges(system, vloc);
}

// Next state

tchecker::state_status_t next(tchecker::ta::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::fsm::outgoing_edges_value_t const & edges)
{
  tchecker::state_status_t status =
      tchecker::ta::next(system, vloc, intval, vedge, place_holder_clkconstr, place_holder_clkconstr, place_holder_clkreset,
                         place_holder_clkconstr, edges);
  place_holder_clkconstr.clear();
  place_holder_clkreset.clear();
  return status;
}

// Incoming edges

tchecker::fsm::incoming_edges_range_t
incoming_edges(tchecker::ta::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
{
  return tchecker::ta::incoming_edges(system, vloc);
}

// Previous state

tchecker::state_status_t prev(tchecker::ta::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::fsm::incoming_edges_value_t const & v)
{
  tchecker::state_status_t status =
      tchecker::ta::prev(system, vloc, intval, vedge, place_holder_clkconstr, place_holder_clkconstr, place_holder_clkreset,
                         place_holder_clkconstr, v);
  place_holder_clkconstr.clear();
  place_holder_clkreset.clear();
  return status;
}

/* labels */

boost::dynamic_bitset<> labels(tchecker::ta::system_t const & system, tchecker::fsm::state_t const & s)
{
  return tchecker::ta::labels(system.as_syncprod_system(), s);
}

/* is_valid_final */

bool is_valid_final(tchecker::ta::system_t const & system, tchecker::fsm::state_t const & s)
{
  return tchecker::ta::is_valid_final(system, s);
}

/* is_initial */

bool is_initial(tchecker::ta::system_t const & system, tchecker::fsm::state_t const & s)
{
  return tchecker::ta::is_initial(system, s);
}

/* attributes */

void attributes(tchecker::ta::system_t const & system, tchecker::fsm::state_t const & s, std::map<std::string, std::string> & m)
{
  tchecker::ta::attributes(system, s, m);
}

void attributes(tchecker::ta::system_t const & system, tchecker::fsm::transition_t const & t,
                std::map<std::string, std::string> & m)
{
  tchecker::syncprod::attributes(system.as_syncprod_system(), t, m);
}

/* initialize */

tchecker::state_status_t initialize(tchecker::ta::system_t const & system,
                                    tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                    tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                    tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                    std::map<std::string, std::string> const & attributes)
{
  tchecker::state_status_t status = tchecker::ta::initialize(system, vloc, intval, vedge, place_holder_clkconstr, attributes);
  place_holder_clkconstr.clear();
  return status;
}

/* fsm_t */

fsm_t::fsm_t(std::shared_ptr<tchecker::ta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
             std::size_t block_size, std::size_t table_size)
    : _system(system), _sharing_type(sharing_type),
      _state_allocator(block_size, block_size, _system->processes_count(), block_size,
                       _system->intvars_count(tchecker::VK_FLATTENED), table_size),
      _transition_allocator(block_size, block_size, _system->processes_count(), table_size)
{
}

// Forward

initial_range_t fsm_t::initial_edges() { return tchecker::fsm::initial_edges(*_system); }

void fsm_t::initial(initial_value_t const & init_edge, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::fsm::state_sptr_t s = _state_allocator.construct();
  tchecker::fsm::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::fsm::initial(*_system, *s, *t, init_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}

void fsm_t::initial(std::vector<sst_t> & v, tchecker::state_status_t mask) { tchecker::ts::initial(*this, v, mask); }

outgoing_edges_range_t fsm_t::outgoing_edges(tchecker::fsm::const_state_sptr_t const & s)
{
  return tchecker::fsm::outgoing_edges(*_system, s->vloc_ptr());
}

void fsm_t::next(tchecker::fsm::const_state_sptr_t const & s, outgoing_edges_value_t const & out_edge, std::vector<sst_t> & v,
                 tchecker::state_status_t mask)
{
  tchecker::fsm::state_sptr_t nexts = _state_allocator.clone(*s);
  tchecker::fsm::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::fsm::next(*_system, *nexts, *t, out_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(nexts);
      share(t);
    }
    v.push_back(std::make_tuple(status, nexts, t));
  }
}

void fsm_t::next(tchecker::fsm::const_state_sptr_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::next(*this, s, v, mask);
}

// Backward

final_range_t fsm_t::final_edges(boost::dynamic_bitset<> const & labels)
{
  return tchecker::fsm::final_edges(*_system, labels);
}

void fsm_t::final(final_value_t const & final_edge, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::fsm::state_sptr_t s = _state_allocator.construct();
  tchecker::fsm::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::fsm::final(*_system, *s, *t, final_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}

void fsm_t::final(boost::dynamic_bitset<> const & labels, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::final(*this, labels, v, mask);
}

incoming_edges_range_t fsm_t::incoming_edges(tchecker::fsm::const_state_sptr_t const & s)
{
  return tchecker::fsm::incoming_edges(*_system, s->vloc_ptr());
}

void fsm_t::prev(tchecker::fsm::const_state_sptr_t const & s, incoming_edges_value_t const & in_edge, std::vector<sst_t> & v,
                 tchecker::state_status_t mask)
{
  tchecker::fsm::state_sptr_t prevs = _state_allocator.clone(*s);
  tchecker::fsm::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::fsm::prev(*_system, *prevs, *t, in_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(prevs);
      share(t);
    }
    v.push_back(std::make_tuple(status, prevs, t));
  }
}

void fsm_t::prev(tchecker::fsm::const_state_sptr_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::prev(*this, s, v, mask);
}

// Builder

void fsm_t::build(std::map<std::string, std::string> const & attributes, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::fsm::state_sptr_t s = _state_allocator.construct();
  tchecker::fsm::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::fsm::initialize(*_system, *s, *t, attributes);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}

// Inspector

boost::dynamic_bitset<> fsm_t::labels(tchecker::fsm::const_state_sptr_t const & s) const
{
  return tchecker::fsm::labels(*_system, *s);
}

void fsm_t::attributes(tchecker::fsm::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const
{
  tchecker::fsm::attributes(*_system, *s, m);
}

void fsm_t::attributes(tchecker::fsm::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const
{
  tchecker::fsm::attributes(*_system, *t, m);
}

bool fsm_t::is_valid_final(tchecker::fsm::const_state_sptr_t const & s) const
{
  return tchecker::fsm::is_valid_final(*_system, *s);
}

bool fsm_t::is_initial(tchecker::fsm::const_state_sptr_t const & s) const { return tchecker::fsm::is_initial(*_system, *s); }

// Sharing

void fsm_t::share(tchecker::fsm::state_sptr_t & s) { _state_allocator.share(s); }

void fsm_t::share(tchecker::fsm::transition_sptr_t & t) { _transition_allocator.share(t); }

} // namespace fsm

} // end of namespace tchecker
