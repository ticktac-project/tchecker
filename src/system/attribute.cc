/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/system/attribute.hh"

namespace tchecker {

namespace system {

/* attr_parsing_position_t */

attr_parsing_position_t::attr_parsing_position_t(tchecker::parsing::attr_parsing_position_t const & parsing_position)
    : tchecker::parsing::attr_parsing_position_t(parsing_position)
{
}

/* attr_t */

attr_t::attr_t(tchecker::parsing::attr_t const & attr) : tchecker::parsing::attr_t(attr) {}

/* attributes_t */

attributes_t::attributes_t(tchecker::parsing::attributes_t const & attributes)
{
  for (tchecker::parsing::attr_t const & attr : attributes)
    add_attribute(attr.key(), attr.value(), tchecker::system::attr_parsing_position_t{attr.parsing_position()});
}

void attributes_t::add_attribute(std::string const & key, std::string const & value,
                                 tchecker::system::attr_parsing_position_t const & parsing_position)
{
  _map.insert(std::make_pair(key, tchecker::system::attr_t{key, value, parsing_position}));
}

void attributes_t::add_attributes(tchecker::system::attributes_t const & attr)
{
  for (auto && attr : attr.range())
    add_attribute(attr.key(), attr.value(), attr.parsing_position());
}

attributes_t::const_iterator_t::const_iterator_t(map_t::const_iterator const & it) : map_t::const_iterator(it) {}

tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> attributes_t::range(std::string const & key) const
{
  auto range = _map.equal_range(key);
  return tchecker::make_range<tchecker::system::attributes_t::const_iterator_t>(range.first, range.second);
}

tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> attributes_t::range() const
{
  return tchecker::make_range<tchecker::system::attributes_t::const_iterator_t>(_map.begin(), _map.end());
}

} // end of namespace system

} // end of namespace tchecker
