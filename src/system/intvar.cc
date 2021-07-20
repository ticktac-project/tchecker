/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>

#include "tchecker/system/intvar.hh"

namespace tchecker {

namespace system {

/* intvars_t */

void intvars_t::add_intvar(std::string const & name, tchecker::intvar_id_t size, tchecker::integer_t min,
                           tchecker::integer_t max, tchecker::integer_t initial, tchecker::system::attributes_t const & attr)
{
  _integer_variables.declare(name, size, min, max, initial);
  for (tchecker::intvar_id_t i = 0; i < size; ++i)
    _integer_variables_attr.emplace_back(attr);
  assert(_integer_variables.size(tchecker::VK_FLATTENED) == _integer_variables_attr.size());
}

tchecker::system::attributes_t const & intvars_t::intvar_attributes(tchecker::intvar_id_t id) const
{
  if (id >= _integer_variables_attr.size())
    throw std::invalid_argument("Unknown integer variable identifier");
  return _integer_variables_attr[id];
}

bool intvars_t::is_intvar(std::string const & name) const { return _integer_variables.is_variable(name); }

} // end of namespace system

} // end of namespace tchecker
