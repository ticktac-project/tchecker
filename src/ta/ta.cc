/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

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

bool satisfies(tchecker::ta::system_t const & system, tchecker::ta::state_t const & s, boost::dynamic_bitset<> const & labels)
{
  return tchecker::syncprod::satisfies(system.as_syncprod_system(), s, labels);
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
}

/* ta_t */

ta_t::ta_t(std::shared_ptr<tchecker::ta::system_t const> const & system, std::size_t block_size)
    : _system(system), _state_allocator(block_size, block_size, _system->processes_count(), block_size,
                                        _system->intvars_count(tchecker::VK_FLATTENED)),
      _transition_allocator(block_size, block_size, _system->processes_count())
{
}

tchecker::ta::initial_range_t ta_t::initial_edges() { return tchecker::ta::initial_edges(*_system); }

void ta_t::initial(tchecker::ta::initial_value_t const & init_edge, std::vector<sst_t> & v)
{
  tchecker::ta::state_sptr_t s = _state_allocator.construct();
  tchecker::ta::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::ta::initial(*_system, *s, *t, init_edge);
  v.push_back(std::make_tuple(status, s, t));
}

tchecker::ta::outgoing_edges_range_t ta_t::outgoing_edges(tchecker::ta::const_state_sptr_t const & s)
{
  return tchecker::ta::outgoing_edges(*_system, s->vloc_ptr());
}

void ta_t::next(tchecker::ta::const_state_sptr_t const & s, tchecker::ta::outgoing_edges_value_t const & out_edge,
                std::vector<sst_t> & v)
{
  tchecker::ta::state_sptr_t nexts = _state_allocator.clone(*s);
  tchecker::ta::transition_sptr_t t = _transition_allocator.construct();
  tchecker::state_status_t status = tchecker::ta::next(*_system, *nexts, *t, out_edge);
  v.push_back(std::make_tuple(status, nexts, t));
}

bool ta_t::satisfies(tchecker::ta::const_state_sptr_t const & s, boost::dynamic_bitset<> const & labels) const
{
  return tchecker::ta::satisfies(*_system, *s, labels);
}

void ta_t::attributes(tchecker::ta::const_state_sptr_t const & s, std::map<std::string, std::string> & m) const
{
  tchecker::ta::attributes(*_system, *s, m);
}

void ta_t::attributes(tchecker::ta::const_transition_sptr_t const & t, std::map<std::string, std::string> & m) const
{
  tchecker::ta::attributes(*_system, *t, m);
}

tchecker::ta::system_t const & ta_t::system() const { return *_system; }

} // end of namespace ta

} // end of namespace tchecker
