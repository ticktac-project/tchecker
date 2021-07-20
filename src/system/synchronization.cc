/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/system/synchronization.hh"

namespace tchecker {

namespace system {

/* sync_constraint_t */

sync_constraint_t::sync_constraint_t(tchecker::process_id_t pid, tchecker::event_id_t event_id,
                                     tchecker::sync_strength_t strength)
    : _pid(pid), _event_id(event_id), _strength(strength)
{
}

/* synchronization_t */

synchronization_t::synchronization_t(tchecker::sync_id_t id, std::vector<tchecker::system::sync_constraint_t> const & v,
                                     tchecker::system::attributes_t const & attr)
    : _id(id), _attr(attr)
{
  for (tchecker::system::sync_constraint_t const & c : v)
    add_synchronization_constraint(c);
}

void synchronization_t::add_synchronization_constraint(tchecker::system::sync_constraint_t const & constr)
{
  // Check that process is not synchronized yet
  for (tchecker::system::sync_constraint_t const & c : _constraints)
    if (c.pid() == constr.pid())
      throw std::invalid_argument("process is already synchronized");
  _constraints.push_back(constr);
}

/* synchronizations_t */

void synchronizations_t::add_synchronization(std::vector<tchecker::system::sync_constraint_t> const & v,
                                             tchecker::system::attributes_t const & attr)
{
  tchecker::sync_id_t id = _syncs.size();
  _syncs.emplace_back(id, v, attr);
}

tchecker::system::synchronization_t const & synchronizations_t::synchronization(tchecker::sync_id_t id) const
{
  if (id >= _syncs.size())
    throw std::invalid_argument("Unknown synchronization");
  return _syncs[id];
}

tchecker::range_t<tchecker::system::synchronizations_t::const_iterator_t> synchronizations_t::synchronizations() const
{
  return tchecker::make_range(_syncs.begin(), _syncs.end());
}

} // end of namespace system

} // end of namespace tchecker
