/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <iterator>

#include "tchecker/variables/access.hh"

namespace tchecker {

void variable_access_map_t::clear()
{
  _v2p_map.clear();
  _p2v_map.clear();
  _variables.clear();
  _processes.clear();
}


void variable_access_map_t::add(tchecker::variable_id_t vid,
                                enum tchecker::variable_type_t vtype,
                                enum tchecker::variable_access_t vaccess,
                                tchecker::process_id_t pid)
{
  if ((vtype != tchecker::CLOCK) && (vtype != tchecker::INTVAR))
    throw std::invalid_argument("Invalid type of variable");
  if ((vaccess != tchecker::READ) && (vaccess != tchecker::WRITE))
    throw std::invalid_argument("Invalid type of variable access");
  
  _variables.insert({vid, vtype});
  _processes.insert(pid);
  
  _v2p_map.insert(std::make_pair(v2p_key_t{vid, vtype, vaccess}, pid));
  _v2p_map.insert(std::make_pair(v2p_key_t{vid, vtype, tchecker::ANY}, pid));
  
  _p2v_map.insert(std::make_pair(p2v_key_t{pid, vtype, vaccess}, vid));
  _p2v_map.insert(std::make_pair(p2v_key_t{pid, vtype, tchecker::ANY}, vid));
}


bool variable_access_map_t::has_shared_variable() const
{
  for (auto && [vid, vtype] : _variables) {
    auto range = _v2p_map.equal_range({vid, vtype, tchecker::ANY});
    if (std::distance(range.first, range.second) > 1)
      return true;
  }
  return false;
}


tchecker::range_t<tchecker::variable_access_map_t::process_id_iterator_t>
variable_access_map_t::accessing_processes(tchecker::variable_id_t vid,
                                           enum tchecker::variable_type_t vtype,
                                           enum tchecker::variable_access_t vaccess) const
{
  auto p = _v2p_map.equal_range({vid, vtype, vaccess});
  return {p.first, p.second};
}


process_id_t variable_access_map_t::accessing_process(tchecker::variable_id_t vid,
                                                      enum tchecker::variable_type_t vtype,
                                                      enum tchecker::variable_access_t vaccess) const
{
  auto range = accessing_processes(vid, vtype, vaccess);
  auto d = std::distance(range.begin(), range.end());
  if (d == 0)
    throw std::invalid_argument("Unknown variable");
  if (d > 1)
    throw std::invalid_argument("Shared variable");
  return * range.begin();
}


tchecker::range_t<tchecker::variable_access_map_t::variable_id_iterator_t>
variable_access_map_t::accessed_variables(tchecker::process_id_t pid,
                                          enum tchecker::variable_type_t vtype,
                                          enum tchecker::variable_access_t vaccess) const
{
  auto p = _p2v_map.equal_range({pid, vtype, vaccess});
  return {p.first, p.second};
}

} // end of namespace tchecker
