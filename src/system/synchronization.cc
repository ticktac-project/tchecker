/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <algorithm>

#include "tchecker/basictypes.hh"
#include "tchecker/system/synchronization.hh"

namespace tchecker {

namespace system {

/* sync_constraint_t */

sync_constraint_t::sync_constraint_t(tchecker::process_id_t pid, tchecker::event_id_t event_id,
                                     tchecker::sync_strength_t strength)
    : _pid(pid), _event_id(event_id), _strength(strength)
{
}

bool sync_constraint_t::operator==(tchecker::system::sync_constraint_t const & sync) const
{
  return (_pid == sync._pid) && (_event_id == sync._event_id) && (_strength == sync._strength);
}

/* synchronization_t */

synchronization_t::synchronization_t(tchecker::sync_id_t id, std::vector<tchecker::system::sync_constraint_t> const & v,
                                     tchecker::system::attributes_t const & attributes)
    : _id(id), _attributes(attributes)
{
  if (!tchecker::valid_sync_id(id))
    throw std::invalid_argument("synchronization::synchronization_t: invalid synchronization identifier");

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
  if (contains(v))
    throw std::invalid_argument("Synchronization already exists");
  tchecker::sync_id_t id = _syncs.size();
  _syncs.emplace_back(id, v, attr);
}

tchecker::system::synchronizations_t::synchronizations_identifiers_range_t
synchronizations_t::synchronizations_identifiers() const
{
  return tchecker::make_integer_range<tchecker::sync_id_t>(0, synchronizations_count());
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

bool synchronizations_t::contains(std::vector<tchecker::system::sync_constraint_t> const & v) const
{
  for (tchecker::system::synchronization_t const & sync : _syncs) {
    tchecker::range_t<tchecker::system::synchronization_t::const_iterator_t> r = sync.synchronization_constraints();
    if (std::is_permutation(r.begin(), r.end(), v.begin(), v.end()))
      return true;
  }
  return false;
}

} // end of namespace system

} // end of namespace tchecker
