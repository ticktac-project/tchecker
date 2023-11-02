/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <iterator>

#include "tchecker/variables/access.hh"

namespace tchecker {

namespace details {

template <class ID_SET, class MAP, class KEY, class ID> static void add(MAP & m, KEY && k, ID id)
{
  auto it = m.find(k);
  if (it == m.end())
    m.insert(std::make_pair(k, ID_SET{id}));
  else
    (*it).second.insert(id);
}

} // end of namespace details

void variable_access_map_t::clear()
{
  _v2p_map.clear();
  _p2v_map.clear();
  assert(_empty_pid_set.empty());
  assert(_empty_vid_set.empty());
}

void variable_access_map_t::add(tchecker::variable_id_t vid, enum tchecker::variable_type_t vtype,
                                enum tchecker::variable_access_t vaccess, tchecker::process_id_t pid)
{
  if ((vtype != tchecker::VTYPE_CLOCK) && (vtype != tchecker::VTYPE_INTVAR))
    throw std::invalid_argument("Invalid type of variable");
  if ((vaccess != tchecker::VACCESS_READ) && (vaccess != tchecker::VACCESS_WRITE))
    throw std::invalid_argument("Invalid type of variable access");

  tchecker::details::add<pid_set_t>(_v2p_map, v2p_key_t{vid, vtype, vaccess}, pid);
  tchecker::details::add<pid_set_t>(_v2p_map, v2p_key_t{vid, vtype, tchecker::VACCESS_ANY}, pid);

  tchecker::details::add<vid_set_t>(_p2v_map, p2v_key_t{pid, vtype, vaccess}, vid);
  tchecker::details::add<vid_set_t>(_p2v_map, p2v_key_t{pid, vtype, tchecker::VACCESS_ANY}, vid);
}

bool variable_access_map_t::has_shared_variable() const
{
  for (auto && v2pid : _v2p_map)
    if (v2pid.second.size() > 1)
      return true;
  return false;
}

bool variable_access_map_t::has_shared_variable(enum tchecker::variable_type_t vtype) const
{
  for (auto && v2pid : _v2p_map) {
    if (v2pid.second.size() <= 1)
      continue;
    auto && [id, type, access] = v2pid.first;
    if (type == vtype)
      return true;
  }
  return false;
}

tchecker::range_t<tchecker::variable_access_map_t::process_id_iterator_t>
variable_access_map_t::accessing_processes(tchecker::variable_id_t vid, enum tchecker::variable_type_t vtype,
                                           enum tchecker::variable_access_t vaccess) const
{
  auto it = _v2p_map.find(v2p_key_t{vid, vtype, vaccess});
  if (it == _v2p_map.end())
    return {_empty_pid_set.begin(), _empty_pid_set.end()};
  return {(*it).second.begin(), (*it).second.end()};
}

process_id_t variable_access_map_t::accessing_process(tchecker::variable_id_t vid, enum tchecker::variable_type_t vtype,
                                                      enum tchecker::variable_access_t vaccess) const
{
  auto range = accessing_processes(vid, vtype, vaccess);
  auto d = std::distance(range.begin(), range.end());
  if (d == 0)
    throw std::invalid_argument("Unknown variable");
  if (d > 1)
    throw std::invalid_argument("Shared variable");
  return *range.begin();
}

tchecker::range_t<tchecker::variable_access_map_t::variable_id_iterator_t>
variable_access_map_t::accessed_variables(tchecker::process_id_t pid, enum tchecker::variable_type_t vtype,
                                          enum tchecker::variable_access_t vaccess) const
{
  auto it = _p2v_map.find(p2v_key_t{pid, vtype, vaccess});
  if (it == _p2v_map.end())
    return {_empty_vid_set.begin(), _empty_vid_set.end()};
  return {(*it).second.begin(), (*it).second.end()};
}

bool variable_access_map_t::access_variable(tchecker::process_id_t pid, tchecker::variable_id_t vid,
                                            enum tchecker::variable_type_t vtype,
                                            enum tchecker::variable_access_t vaccess) const
{
  auto accessed_variables_range = accessed_variables(pid, vtype, vaccess);
  for (tchecker::variable_id_t accessed_vid : accessed_variables_range)
    if (accessed_vid == vid)
      return true;
  return false;
}

} // end of namespace tchecker
