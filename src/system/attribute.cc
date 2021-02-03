/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/system/attribute.hh"

namespace tchecker {

namespace system {

attributes_t::attributes_t(tchecker::parsing::attributes_t const & attributes)
{
  for (tchecker::parsing::attr_t const & attr : attributes)
    add_attribute(attr.key(), attr.value());
}

void attributes_t::add_attribute(std::string const & key, std::string const & value)
{
  _attr.insert(std::make_pair(key, value));
}

void attributes_t::add_attributes(tchecker::system::attributes_t const & attr)
{
  for (auto && [key, value] : attr.attributes())
    add_attribute(key, value);
}

tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> attributes_t::values(std::string const & key) const
{
  auto range = _attr.equal_range(key);
  return tchecker::make_range(range.first, range.second);
}

tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> attributes_t::attributes() const
{
  return tchecker::make_range(_attr.begin(), _attr.end());
}

} // end of namespace system

} // end of namespace tchecker
