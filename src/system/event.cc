/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/system/event.hh"

namespace tchecker {

namespace system {

/* events_t */

void events_t::add_event(std::string const & name, tchecker::system::attributes_t const & attributes)
{
  tchecker::event_id_t id = _events_attributes.size();
  _events_index.add(name, id);
  _events_attributes.emplace_back(attributes);
}

tchecker::system::events_t::events_identifiers_range_t events_t::events_identifiers() const
{
  return tchecker::make_integer_range<tchecker::event_id_t>(0, events_count());
}

tchecker::system::attributes_t const & events_t::event_attributes(tchecker::event_id_t id) const
{
  if (id >= events_count())
    throw std::invalid_argument("Unknown event");
  return _events_attributes[id];
}

bool events_t::is_event(tchecker::event_id_t id) const { return (id < events_count()); }

bool events_t::is_event(std::string const & event) const
{
  return _events_index.find_key(event) != _events_index.end_key_map();
}

} // end of namespace system

} // end of namespace tchecker
