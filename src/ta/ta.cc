/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <vector>

#include "tchecker/ta/ta.hh"

namespace tchecker {

namespace ta {

/* Helpers */

/*!
 \class throw_container_t
 \brief Container that throws on call to push_back()
 \tparam CONTAINER : type of embedded container
 */
template <class CONTAINER> class throw_container_t : public CONTAINER {
public:
  template <class T> void push_back(T && t) const { throw std::runtime_error("throw_container_t::push_back() called"); }
};

/*!< Throw clock constraint container */
static tchecker::ta::throw_container_t<tchecker::clock_constraint_container_t> throw_clkconstr;

/*!< Throw clock reset container */
static tchecker::ta::throw_container_t<tchecker::clock_reset_container_t> throw_clkreset;

/* Semantics functions */

tchecker::state_status_t initial(tchecker::ta::system_t const & system,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                 tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                 tchecker::clock_constraint_container_t & invariant,
                                 tchecker::ta::initial_value_t const & initial_range)
{
  // intialize vloc and vedge
  auto status = tchecker::syncprod::initial(system.as_syncprod_system(), vloc, vedge, initial_range);
  if (status != STATE_OK)
    return status;

  // initialize intval
  auto const & intvars = system.integer_variables().flattened();
  tchecker::intvar_id_t intvars_size = intvars.size();
  for (tchecker::intvar_id_t id = 0; id < intvars_size; ++id)
    (*intval)[id] = intvars.info(id).initial_value();

  // check invariant
  tchecker::vm_t & vm = system.vm();
  for (tchecker::loc_id_t loc_id : *vloc)
    if (vm.run(system.invariant_bytecode(loc_id), *intval, invariant, throw_clkreset) == 0)
      return tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED;

  return tchecker::STATE_OK;
}

tchecker::ta::final_range_t final_edges(tchecker::ta::system_t const & system, boost::dynamic_bitset<> const & labels)
{
  tchecker::ta::final_iterator_t it{tchecker::syncprod::final_edges(system.as_syncprod_system(), labels),
                                    tchecker::flat_integer_variables_valuations_range(system.integer_variables().flattened())};
  return tchecker::make_range(it, tchecker::past_the_end_iterator);
}

tchecker::state_status_t final(tchecker::ta::system_t const & system,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                               tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                               tchecker::clock_constraint_container_t & invariant,
                               tchecker::ta::final_value_t const & final_value)
{
  auto && [edges, valuation] = final_value;

  // compute vloc and vedge from final edges
  auto status = tchecker::syncprod::final(system.as_syncprod_system(), vloc, vedge, edges);
  if (status != STATE_OK)
    return status;

  // compute intval
  auto const & intvars = system.integer_variables().flattened();
  tchecker::intvar_id_t intvars_size = intvars.size();
  tchecker::intvar_id_t id = 0;
  for (tchecker::integer_t v : valuation) {
    if (id >= intvars_size)
      throw std::runtime_error("tchecker::ta::final: valuation of incompatible size");
    (*intval)[id] = v;
    ++id;
  }

  // check invariant
  tchecker::vm_t & vm = system.vm();
  for (tchecker::loc_id_t loc_id : *vloc)
    if (vm.run(system.invariant_bytecode(loc_id), *intval, invariant, throw_clkreset) == 0)
      return tchecker::STATE_INTVARS_TGT_INVARIANT_VIOLATED;

  return tchecker::STATE_OK;
}

tchecker::state_status_t next(tchecker::ta::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::clock_constraint_container_t & src_invariant,
                              tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                              tchecker::clock_constraint_container_t & tgt_invariant,
                              tchecker::ta::outgoing_edges_value_t const & edges)
{
  tchecker::vm_t & vm = system.vm();

  // check source invariant
  for (tchecker::loc_id_t loc_id : *vloc)
    if (vm.run(system.invariant_bytecode(loc_id), *intval, src_invariant, throw_clkreset) == 0)
      return tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED;

  // compute next vloc
  auto status = tchecker::syncprod::next(system.as_syncprod_system(), vloc, vedge, edges);
  if (status != tchecker::STATE_OK)
    return status;

  // check guards
  for (tchecker::system::edge_const_shared_ptr_t const & edge : edges)
    if (vm.run(system.guard_bytecode(edge->id()), *intval, guard, throw_clkreset) == 0)
      return tchecker::STATE_INTVARS_GUARD_VIOLATED;

  // apply statements
  for (tchecker::system::edge_const_shared_ptr_t const & edge : edges)
    if (vm.run(system.statement_bytecode(edge->id()), *intval, throw_clkconstr, reset) == 0)
      return tchecker::STATE_INTVARS_STATEMENT_FAILED;

  // check target invariant
  for (tchecker::loc_id_t loc_id : *vloc)
    if (vm.run(system.invariant_bytecode(loc_id), *intval, tgt_invariant, throw_clkreset) == 0)
      return tchecker::STATE_INTVARS_TGT_INVARIANT_VIOLATED;

  return tchecker::STATE_OK;
}

tchecker::ta::incoming_edges_range_t
incoming_edges(tchecker::ta::system_t const & system,
               tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc)
{
  tchecker::ta::incoming_edges_iterator_t it{
      tchecker::syncprod::incoming_edges(system.as_syncprod_system(), vloc),
      tchecker::flat_integer_variables_valuations_range(system.integer_variables().flattened())};
  return tchecker::make_range(it, tchecker::past_the_end_iterator);
}

static void copy(std::vector<tchecker::loc_id_t> & copy, tchecker::vloc_t const & vloc)
{
  assert(copy.size() == vloc.capacity());
  for (tchecker::loc_id_t i = 0; i < vloc.capacity(); ++i)
    copy[i] = vloc[i];
}

static void copy(tchecker::vloc_t & vloc, std::vector<tchecker::loc_id_t> const & copy)
{
  assert(vloc.capacity() == copy.size());
  for (tchecker::loc_id_t i = 0; i < copy.size(); ++i)
    vloc[i] = copy[i];
}

static void copy(std::vector<tchecker::integer_t> & copy, tchecker::intvars_valuation_t const & intval)
{
  assert(copy.size() == intval.capacity());
  for (tchecker::intvar_id_t i = 0; i < intval.capacity(); ++i)
    copy[i] = intval[i];
}

static void copy(tchecker::intvars_valuation_t & intval, std::vector<tchecker::integer_t> const & copy)
{
  assert(intval.capacity() == copy.size());
  for (tchecker::intvar_id_t i = 0; i < copy.size(); ++i)
    intval[i] = copy[i];
}

static bool operator!=(tchecker::vloc_t const & vloc, std::vector<tchecker::loc_id_t> const & copy)
{
  assert(vloc.capacity() == copy.size());
  for (tchecker::loc_id_t id = 0; id < vloc.capacity(); ++id)
    if (vloc[id] != copy[id])
      return true;
  return false;
}

static bool operator!=(tchecker::intvars_valuation_t const & intval, std::vector<tchecker::integer_t> const & copy)
{
  assert(intval.capacity() == copy.size());
  for (tchecker::intvar_id_t id = 0; id < intval.size(); ++id)
    if (intval[id] != copy[id])
      return true;
  return false;
}

tchecker::state_status_t prev(tchecker::ta::system_t const & system,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                              tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                              tchecker::clock_constraint_container_t & src_invariant,
                              tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                              tchecker::clock_constraint_container_t & tgt_invariant,
                              tchecker::ta::incoming_edges_value_t const & v)
{
  auto && [edges, valuation] = v;

  // Make a copy of target vloc and intval
  std::vector<tchecker::loc_id_t> vloc_tgt(vloc->capacity());
  copy(vloc_tgt, *vloc);
  std::vector<tchecker::integer_t> intval_tgt(intval->capacity());
  copy(intval_tgt, *intval);

  // Update vloc and intval to source according to v
  for (tchecker::system::edge_const_shared_ptr_t const & e : edges)
    (*vloc)[e->pid()] = e->src();
  tchecker::intvar_id_t id = 0;
  for (tchecker::integer_t i : valuation) {
    (*intval)[id] = i;
    ++id;
  }

  // Make a copy of source vloc and intval
  std::vector<tchecker::loc_id_t> vloc_src(vloc->capacity());
  copy(vloc_src, *vloc);
  std::vector<tchecker::integer_t> intval_src(intval->capacity());
  copy(intval_src, *intval);

  // Apply transition
  tchecker::state_status_t status =
      tchecker::ta::next(system, vloc, intval, vedge, src_invariant, guard, reset, tgt_invariant, edges);
  if (status != tchecker::STATE_OK)
    return status;

  // Check target vloc and intval are matched
  if (*vloc != vloc_tgt || *intval != intval_tgt)
    return tchecker::STATE_INCOMPATIBLE_EDGE;

  // Update to source vloc and intval
  copy(*vloc, vloc_src);
  copy(*intval, intval_src);
  return tchecker::STATE_OK;
}

/* delay_allowed */

bool delay_allowed(tchecker::ta::system_t const & system, tchecker::vloc_t const & vloc)
{
  for (tchecker::loc_id_t loc_id : vloc)
    if (system.is_committed(loc_id) || system.is_urgent(loc_id))
      return false;
  return true;
}

boost::dynamic_bitset<> delay_allowed(tchecker::ta::system_t const & system, tchecker::reference_clock_variables_t const & r,
                                      tchecker::vloc_t const & vloc)
{
  std::size_t const size = vloc.size();
  std::vector<tchecker::clock_id_t> const & procmap = r.procmap();
  boost::dynamic_bitset<> allowed{r.refcount()};
  allowed.set();
  for (std::size_t i = 0; i < size; ++i)
    if (system.is_urgent(vloc[i]) || system.is_committed(vloc[i]))
      allowed.reset(procmap[i]);
  return allowed;
}

/* sync_refclocks */

boost::dynamic_bitset<> sync_refclocks(tchecker::ta::system_t const & system, tchecker::reference_clock_variables_t const & r,
                                       tchecker::vedge_t const & vedge)
{
  std::vector<tchecker::clock_id_t> const & procmap = r.procmap();
  boost::dynamic_bitset<> refclocks{r.refcount()};
  refclocks.reset();
  for (tchecker::edge_id_t id : vedge)
    refclocks.set(procmap[system.edge(id)->pid()]);
  return refclocks;
}

/* labels */

boost::dynamic_bitset<> labels(tchecker::ta::system_t const & system, tchecker::ta::state_t const & s)
{
  return tchecker::syncprod::labels(system.as_syncprod_system(), s);
}

/* is_valid_final */

bool is_valid_final(tchecker::ta::system_t const & system, tchecker::ta::state_t const & s)
{
  return tchecker::syncprod::is_valid_final(system.as_syncprod_system(), s);
}

/* is_initial */

bool is_initial(tchecker::ta::system_t const & system, tchecker::intvars_valuation_t const & v)
{
  assert(v.capacity() == system.integer_variables().flattened().size());
  tchecker::intvar_id_t const nvars = v.capacity();
  for (tchecker::intvar_id_t i = 0; i < nvars; ++i)
    if (v[i] != system.integer_variables().flattened().info(i).initial_value())
      return false;
  return true;
}

bool is_initial(tchecker::ta::system_t const & system, tchecker::ta::state_t const & s)
{
  return tchecker::syncprod::is_initial(system.as_syncprod_system(), s) && tchecker::ta::is_initial(system, s.intval());
}

/* attributes */

void attributes(tchecker::ta::system_t const & system, tchecker::ta::state_t const & s, std::map<std::string, std::string> & m)
{
  tchecker::syncprod::attributes(system.as_syncprod_system(), s, m);
  m["intval"] = tchecker::to_string(s.intval(), system.integer_variables().flattened().index());
}

void attributes(tchecker::ta::system_t const & system, tchecker::ta::transition_t const & t,
                std::map<std::string, std::string> & m)
{
  tchecker::syncprod::attributes(system.as_syncprod_system(), t, m);
  m["src_invariant"] = tchecker::to_string(t.src_invariant_container(), system.clock_variables().index());
  m["guard"] = tchecker::to_string(t.guard_container(), system.clock_variables().index());
  m["reset"] = tchecker::to_string(t.reset_container(), system.clock_variables().index());
  m["tgt_invariant"] = tchecker::to_string(t.tgt_invariant_container(), system.clock_variables().index());
}

/* ta_t */

ta_t::ta_t(std::shared_ptr<tchecker::ta::system_t const> const & system, enum tchecker::ts::sharing_type_t sharing_type,
           std::size_t block_size, std::size_t table_size)
    : _system(system), _sharing_type(sharing_type),
      _state_allocator(block_size, block_size, _system->processes_count(), block_size,
                       _system->intvars_count(tchecker::VK_FLATTENED), table_size),
      _transition_allocator(block_size, block_size, _system->processes_count(), table_size)
{
}

// Forward

tchecker::ta::initial_range_t ta_t::initial_edges() { return tchecker::ta::initial_edges(*_system); }

void ta_t::initial(tchecker::ta::initial_value_t const & init_edge, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ta::state_sptr_t s = _state_allocator.construct();
  tchecker::ta::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::ta::initial(*_system, *s, *t, init_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}

void ta_t::initial(std::vector<sst_t> & v, tchecker::state_status_t mask) { tchecker::ts::initial(*this, v, mask); }

tchecker::ta::outgoing_edges_range_t ta_t::outgoing_edges(tchecker::ta::const_state_sptr_t const & s)
{
  return tchecker::ta::outgoing_edges(*_system, s->vloc_ptr());
}

void ta_t::next(tchecker::ta::const_state_sptr_t const & s, tchecker::ta::outgoing_edges_value_t const & out_edge,
                std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ta::state_sptr_t nexts = _state_allocator.clone(*s);
  tchecker::ta::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::ta::next(*_system, *nexts, *t, out_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(nexts);
      share(t);
    }
    v.push_back(std::make_tuple(status, nexts, t));
  }
}

void ta_t::next(tchecker::ta::const_state_sptr_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::next(*this, s, v, mask);
}

// Backward

final_range_t ta_t::final_edges(boost::dynamic_bitset<> const & labels) { return tchecker::ta::final_edges(*_system, labels); }

void ta_t::final(final_value_t const & final_edge, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ta::state_sptr_t s = _state_allocator.construct();
  tchecker::ta::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::ta::final(*_system, *s, *t, final_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(s);
      share(t);
    }
    v.push_back(std::make_tuple(status, s, t));
  }
}

void ta_t::final(boost::dynamic_bitset<> const & labels, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::final(*this, labels, v, mask);
}

incoming_edges_range_t ta_t::incoming_edges(tchecker::ta::const_state_sptr_t const & s)
{
  return tchecker::ta::incoming_edges(*_system, s->vloc_ptr());
}

void ta_t::prev(tchecker::ta::const_state_sptr_t const & s, incoming_edges_value_t const & in_edge, std::vector<sst_t> & v,
                tchecker::state_status_t mask)
{
  tchecker::ta::state_sptr_t prevs = _state_allocator.clone(*s);
  tchecker::ta::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::ta::prev(*_system, *prevs, *t, in_edge);
  if (status & mask) {
    if (_sharing_type == tchecker::ts::SHARING) {
      share(prevs);
      share(t);
    }
    v.push_back(std::make_tuple(status, prevs, t));
  }
}

void ta_t::prev(tchecker::ta::const_state_sptr_t const & s, std::vector<sst_t> & v, tchecker::state_status_t mask)
{
  tchecker::ts::prev(*this, s, v, mask);
}

// Inspector

boost::dynamic_bitset<> ta_t::labels(tchecker::ta::const_state_sptr_t const & s) const
{
  return tchecker::ta::labels(*_system, *s);
}

void ta_t::attributes(tchecker::ta::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const
{
  tchecker::ta::attributes(*_system, *s, m);
}

void ta_t::attributes(tchecker::ta::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const
{
  tchecker::ta::attributes(*_system, *t, m);
}

bool ta_t::is_valid_final(tchecker::ta::const_state_sptr_t const & s) const
{
  return tchecker::ta::is_valid_final(*_system, *s);
}

bool ta_t::is_initial(tchecker::ta::const_state_sptr_t const & s) const { return ta::is_initial(*_system, *s); }

// Sharing

void ta_t::share(tchecker::ta::state_sptr_t & s) { _state_allocator.share(s); }

void ta_t::share(tchecker::ta::transition_sptr_t & t) { _transition_allocator.share(t); }

} // end of namespace ta

} // end of namespace tchecker
