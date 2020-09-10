/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>

#include "tchecker/system/clock.hh"

namespace tchecker {

namespace system {

/* clocks_t */

void clocks_t::add_clock(std::string const & name, tchecker::clock_id_t size, tchecker::system::attributes_t const & attr)
{
  tchecker::clock_id_t id = _clock_variables.declare(name, size);
  _clock_variables_attr.emplace(id, attr);
  assert(_clock_variables.size(tchecker::VK_DECLARED) == _clock_variables_attr.size());
}

tchecker::system::attributes_t const & clocks_t::clock_attributes(tchecker::clock_id_t id) const
{
  try {
    return _clock_variables_attr.at(id);
  }
  catch (...) {
    throw std::invalid_argument("Unknown clock identifier");
  }
}

bool clocks_t::is_clock(std::string const & name) const { return _clock_variables.is_variable(name); }

} // end of namespace system

} // end of namespace tchecker
