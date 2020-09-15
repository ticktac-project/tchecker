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

enum tchecker::state_status_t initialize(tchecker::ta::system_t const & system,
                                         tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                         tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                         tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                         tchecker::clock_constraint_container_t & invariant,
                                         tchecker::ta::initial_iterator_value_t const & initial_range)
{
  // intialize vloc and vedge
  auto status = tchecker::syncprod::initialize(system.as_syncprod_system(), vloc, vedge, initial_range);
  return status;

  // initialize intval
  auto const & intvars = system.integer_variables().flattened();
  tchecker::intvar_id_t intvars_size = intvars.size();
  for (tchecker::intvar_id_t id = 0; id < intvars_size; ++id)
    (*intval)[id] = intvars.info(id).initial_value();

  // check invariant
  tchecker::vm_t & vm = system.vm();
  for (tchecker::loc_id_t loc_id : *vloc)
    if (vm.run(system.invariant_bytecode(loc_id), *intval, invariant, throw_clkreset) != 1)
      return tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED;

  return tchecker::STATE_OK;
}

enum tchecker::state_status_t next(tchecker::ta::system_t const & system,
                                   tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t> const & vloc,
                                   tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t> const & intval,
                                   tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge,
                                   tchecker::clock_constraint_container_t & src_invariant,
                                   tchecker::clock_constraint_container_t & guard, tchecker::clock_reset_container_t & reset,
                                   tchecker::clock_constraint_container_t & tgt_invariant,
                                   tchecker::ta::outgoing_edges_iterator_value_t const & edges)
{
  tchecker::vm_t & vm = system.vm();

  // check source invariant
  for (tchecker::loc_id_t loc_id : *vloc)
    if (vm.run(system.invariant_bytecode(loc_id), *intval, src_invariant, throw_clkreset) != 1)
      return tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED;

  // compute next vloc
  auto status = tchecker::syncprod::next(system.as_syncprod_system(), vloc, vedge, edges);
  if (status != tchecker::STATE_OK)
    return status;

  // check guards
  for (tchecker::system::edge_const_shared_ptr_t const & edge : edges)
    if (vm.run(system.guard_bytecode(edge->pid()), *intval, guard, throw_clkreset) != 1)
      return tchecker::STATE_INTVARS_GUARD_VIOLATED;

  // apply statements
  for (tchecker::system::edge_const_shared_ptr_t const & edge : edges)
    if (vm.run(system.statement_bytecode(edge->pid()), *intval, throw_clkconstr, reset) != 1)
      return tchecker::STATE_INTVARS_STATEMENT_FAILED;

  // check target invariant
  for (tchecker::loc_id_t loc_id : *vloc)
    if (vm.run(system.invariant_bytecode(loc_id), *intval, tgt_invariant, throw_clkreset) != 1)
      return tchecker::STATE_INTVARS_TGT_INVARIANT_VIOLATED;

  return tchecker::STATE_OK;
}

/* ta_t */

ta_t::ta_t(tchecker::ta::system_t const & system) : _system(system) {}

tchecker::range_t<tchecker::ta::initial_iterator_t> ta_t::initial() { return tchecker::ta::initial(_system); }

enum tchecker::state_status_t ta_t::initialize(tchecker::ta::state_t & s, tchecker::ta::transition_t & t,
                                               tchecker::ta::initial_iterator_value_t const & v)
{
  return tchecker::ta::initialize(_system, s.vloc_ptr(), s.intval_ptr(), t.vedge_ptr(), t.tgt_invariant_container(), v);
}

tchecker::range_t<tchecker::ta::outgoing_edges_iterator_t> ta_t::outgoing_edges(tchecker::ta::state_t const & s)
{
  return tchecker::ta::outgoing_edges(_system, s.vloc_ptr());
}

enum tchecker::state_status_t ta_t::next(tchecker::ta::state_t & s, tchecker::ta::transition_t & t,
                                         tchecker::ta::outgoing_edges_iterator_value_t const & v)
{
  return tchecker::ta::next(_system, s.vloc_ptr(), s.intval_ptr(), t.vedge_ptr(), t.src_invariant_container(),
                            t.guard_container(), t.reset_container(), t.tgt_invariant_container(), v);
}

tchecker::ta::system_t const & ta_t::system() const { return _system; }

/* delay_allowed */

bool delay_allowed(tchecker::ta::system_t const & system, tchecker::vloc_t const & vloc)
{
  for (tchecker::loc_id_t loc_id : vloc)
    if (system.is_committed(loc_id) || system.is_urgent(loc_id))
      return false;
  return true;
}

void delay_allowed(tchecker::ta::system_t const & system, tchecker::vloc_t const & vloc, boost::dynamic_bitset<> & allowed)
{
  assert(vloc.size() == allowed.size());
  std::size_t size = vloc.size();
  for (std::size_t i = 0; i < size; ++i)
    allowed[i] = !system.is_urgent(vloc[i]) && !system.is_committed(vloc[i]);
}

} // end of namespace ta

} // end of namespace tchecker
