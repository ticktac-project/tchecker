/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <ostream>
#include <stdexcept>

#include "tchecker/parsing/declaration.hh"

namespace tchecker {

namespace parsing {

/* attr_parsing_position_t */

attr_parsing_position_t::attr_parsing_position_t() = default;

attr_parsing_position_t::attr_parsing_position_t(std::string const & key_position, std::string const & value_position)
    : _key_position(key_position), _value_position(value_position)
{
}

/* attr_t */

attr_t::attr_t(std::string const & key, std::string const & value,
               tchecker::parsing::attr_parsing_position_t const & parsing_position)
    : _key(key), _value(value), _parsing_position(parsing_position)
{
}

std::ostream & operator<<(std::ostream & os, tchecker::parsing::attr_t const & attr)
{
  os << attr.key() << ": " << attr.value();
  return os;
}

/* attributes_t */

void attributes_t::clear() { _attr.clear(); }

bool attributes_t::empty() const { return _attr.empty(); }

std::size_t attributes_t::size() const { return _attr.size(); }

void attributes_t::insert(std::shared_ptr<tchecker::parsing::attr_t> const & attr) { _attr.insert({attr->key(), attr}); }

tchecker::parsing::attributes_t::const_iterator_t::const_iterator_t(
    tchecker::parsing::attributes_t::map_t::const_iterator const & it)
    : tchecker::parsing::attributes_t::map_t::const_iterator(it)
{
}

tchecker::parsing::attributes_t::const_iterator_t attributes_t::begin() const
{
  return tchecker::parsing::attributes_t::const_iterator_t(_attr.begin());
}

tchecker::parsing::attributes_t::const_iterator_t attributes_t::end() const
{
  return tchecker::parsing::attributes_t::const_iterator_t(_attr.end());
}

tchecker::range_t<tchecker::parsing::attributes_t::const_iterator_t> attributes_t::attributes() const
{
  return tchecker::make_range(begin(), end());
}

tchecker::range_t<tchecker::parsing::attributes_t::const_iterator_t> attributes_t::attributes(std::string const & key) const
{
  auto range = _attr.equal_range(key);
  return tchecker::make_range(tchecker::parsing::attributes_t::const_iterator_t(range.first),
                              tchecker::parsing::attributes_t::const_iterator_t(range.second));
}

std::ostream & operator<<(std::ostream & os, tchecker::parsing::attributes_t const & attr)
{
  if (attr.size() == 0)
    return os;

  os << "{";
  for (auto it = attr.begin(); it != attr.end(); ++it) {
    if (it != attr.begin())
      os << " : ";
    os << *it;
  }
  os << "}";
  return os;
}

/* declaration_t */

declaration_t::declaration_t() = default;

declaration_t::declaration_t(tchecker::parsing::attributes_t const & attr, std::string const & context)
    : _attr(attr), _context(context)
{
}

declaration_t::declaration_t(tchecker::parsing::declaration_t const &) = default;

declaration_t::declaration_t(tchecker::parsing::declaration_t &&) = default;

declaration_t::~declaration_t() = default;

tchecker::parsing::declaration_t & declaration_t::operator=(tchecker::parsing::declaration_t const &) = default;

tchecker::parsing::declaration_t & declaration_t::operator=(tchecker::parsing::declaration_t &&) = default;

std::ostream & operator<<(std::ostream & os, tchecker::parsing::declaration_t const & decl) { return decl.output(os); }

/* inner_declaration_t */

inner_declaration_t::inner_declaration_t(tchecker::parsing::inner_declaration_t const &) = default;

inner_declaration_t::inner_declaration_t(tchecker::parsing::inner_declaration_t &&) = default;

inner_declaration_t::~inner_declaration_t() = default;

tchecker::parsing::inner_declaration_t &
inner_declaration_t::operator=(tchecker::parsing::inner_declaration_t const &) = default;

tchecker::parsing::inner_declaration_t & inner_declaration_t::operator=(tchecker::parsing::inner_declaration_t &&) = default;

/* clock_declaration_t */

clock_declaration_t::clock_declaration_t(std::string const & name, unsigned int size,
                                         tchecker::parsing::attributes_t const & attr, std::string const & context)
    : tchecker::parsing::inner_declaration_t(attr, context), _name(name), _size(size)
{
  if (_name.empty())
    throw std::invalid_argument("clock declaration has empty name");

  if (_size < 1)
    throw std::invalid_argument("clock declaration has size < 1");
}

clock_declaration_t::clock_declaration_t(tchecker::parsing::clock_declaration_t const &) = default;

clock_declaration_t::clock_declaration_t(tchecker::parsing::clock_declaration_t &&) = default;

clock_declaration_t::~clock_declaration_t() = default;

tchecker::parsing::clock_declaration_t &
clock_declaration_t::operator=(tchecker::parsing::clock_declaration_t const &) = default;

tchecker::parsing::clock_declaration_t & clock_declaration_t::operator=(tchecker::parsing::clock_declaration_t &&) = default;

void clock_declaration_t::visit(tchecker::parsing::declaration_visitor_t & v) const { v.visit(*this); }

std::ostream & clock_declaration_t::output(std::ostream & os) const { return os << "clock:" << _size << ":" << _name << _attr; }

/* int_declaration_t */

int_declaration_t::int_declaration_t(std::string const & name, unsigned int size, tchecker::integer_t min,
                                     tchecker::integer_t max, tchecker::integer_t init,
                                     tchecker::parsing::attributes_t const & attr, std::string const & context)
    : tchecker::parsing::inner_declaration_t(attr, context), _name(name), _size(size), _min(min), _max(max), _init(init)
{
  if (_name.empty())
    throw std::invalid_argument("int declaration has empty name");
  if (_size < 1)
    throw std::invalid_argument("int declaration has size < 1");
  if (_min > _max)
    throw std::invalid_argument("int declaration has min > max");
  if (_init < _min)
    throw std::invalid_argument("int declaration has init < min");
  if (_init > _max)
    throw std::invalid_argument("int declaration has init > max");
}

int_declaration_t::int_declaration_t(tchecker::parsing::int_declaration_t const &) = default;

int_declaration_t::int_declaration_t(tchecker::parsing::int_declaration_t &&) = default;

int_declaration_t::~int_declaration_t() = default;

tchecker::parsing::int_declaration_t & int_declaration_t::operator=(tchecker::parsing::int_declaration_t const &) = default;

tchecker::parsing::int_declaration_t & int_declaration_t::operator=(tchecker::parsing::int_declaration_t &&) = default;

void int_declaration_t::visit(tchecker::parsing::declaration_visitor_t & v) const { v.visit(*this); }

std::ostream & int_declaration_t::output(std::ostream & os) const
{
  return os << "int:" << _size << ":" << _min << ":" << _max << ":" << _init << ":" << _name << _attr;
}

/* process_declaration_t */

process_declaration_t::process_declaration_t(std::string const & name, tchecker::parsing::attributes_t const & attr,
                                             std::string const & context)
    : tchecker::parsing::inner_declaration_t(attr, context), _name(name)
{
  if (_name.empty())
    throw std::invalid_argument("process declaration has empty name");
}

process_declaration_t::process_declaration_t(tchecker::parsing::process_declaration_t const &) = default;

process_declaration_t::process_declaration_t(tchecker::parsing::process_declaration_t &&) = default;

process_declaration_t::~process_declaration_t() = default;

tchecker::parsing::process_declaration_t &
process_declaration_t::operator=(tchecker::parsing::process_declaration_t const &) = default;

tchecker::parsing::process_declaration_t &
process_declaration_t::operator=(tchecker::parsing::process_declaration_t &&) = default;

void process_declaration_t::visit(tchecker::parsing::declaration_visitor_t & v) const { v.visit(*this); }

std::ostream & process_declaration_t::output(std::ostream & os) const { return os << "process:" << _name << _attr; }

/* event_declaration_t */

event_declaration_t::event_declaration_t(std::string const & name, tchecker::parsing::attributes_t const & attr,
                                         std::string const & context)
    : tchecker::parsing::inner_declaration_t(attr, context), _name(name)
{
  if (_name.empty())
    throw std::invalid_argument("event declaration has empty name");
}

event_declaration_t::event_declaration_t(tchecker::parsing::event_declaration_t const &) = default;

event_declaration_t::event_declaration_t(tchecker::parsing::event_declaration_t &&) = default;

event_declaration_t::~event_declaration_t() = default;

tchecker::parsing::event_declaration_t &
event_declaration_t::operator=(tchecker::parsing::event_declaration_t const &) = default;

tchecker::parsing::event_declaration_t & event_declaration_t::operator=(tchecker::parsing::event_declaration_t &&) = default;

void event_declaration_t::visit(tchecker::parsing::declaration_visitor_t & v) const { v.visit(*this); }

std::ostream & event_declaration_t::output(std::ostream & os) const { return os << "event:" << _name << _attr; }

/* location_declaration_t */

location_declaration_t::location_declaration_t(std::string const & name,
                                               std::shared_ptr<tchecker::parsing::process_declaration_t const> const & process,
                                               tchecker::parsing::attributes_t const & attr, std::string const & context)
    : tchecker::parsing::inner_declaration_t(attr, context), _name(name), _process(process)
{
  if (_name.empty())
    throw std::invalid_argument("location declaration has empty name");
}

location_declaration_t::location_declaration_t(tchecker::parsing::location_declaration_t const &) = default;

location_declaration_t::location_declaration_t(tchecker::parsing::location_declaration_t &&) = default;

location_declaration_t::~location_declaration_t() = default;

tchecker::parsing::location_declaration_t &
location_declaration_t::operator=(tchecker::parsing::location_declaration_t const &) = default;

tchecker::parsing::location_declaration_t &
location_declaration_t::operator=(tchecker::parsing::location_declaration_t &&) = default;

void location_declaration_t::visit(tchecker::parsing::declaration_visitor_t & v) const { v.visit(*this); }

std::ostream & location_declaration_t::output(std::ostream & os) const
{
  return os << "location:" << _process->name() << ":" << _name << _attr;
}

/* edge_declaration_t */

edge_declaration_t::edge_declaration_t(std::shared_ptr<tchecker::parsing::process_declaration_t const> const & process,
                                       std::shared_ptr<tchecker::parsing::location_declaration_t const> const & src,
                                       std::shared_ptr<tchecker::parsing::location_declaration_t const> const & tgt,
                                       std::shared_ptr<tchecker::parsing::event_declaration_t const> const & event,
                                       tchecker::parsing::attributes_t const & attr, std::string const & context)
    : tchecker::parsing::inner_declaration_t(attr, context), _process(process), _src(src), _tgt(tgt), _event(event)
{
  if (_src->process().name() != _process->name())
    throw std::invalid_argument("error, edge declaration has source location that does not belong to process");

  if (_tgt->process().name() != _process->name())
    throw std::invalid_argument("error, edge declaration has target location does not belong to process");
}

edge_declaration_t::edge_declaration_t(tchecker::parsing::edge_declaration_t const &) = default;

edge_declaration_t::edge_declaration_t(tchecker::parsing::edge_declaration_t &&) = default;

edge_declaration_t::~edge_declaration_t() = default;

tchecker::parsing::edge_declaration_t & edge_declaration_t::operator=(tchecker::parsing::edge_declaration_t const &) = default;

tchecker::parsing::edge_declaration_t & edge_declaration_t::operator=(tchecker::parsing::edge_declaration_t &&) = default;

void edge_declaration_t::visit(tchecker::parsing::declaration_visitor_t & v) const { v.visit(*this); }

std::ostream & edge_declaration_t::output(std::ostream & os) const
{
  return os << "edge:" << _process->name() << ":" << _src->name() << ":" << _tgt->name() << ":" << _event->name() << _attr;
}

/* sync_constraint_t */

sync_constraint_t::sync_constraint_t(std::shared_ptr<tchecker::parsing::process_declaration_t const> const & process,
                                     std::shared_ptr<tchecker::parsing::event_declaration_t const> const & event,
                                     enum tchecker::sync_strength_t strength)
    : _process(process), _event(event), _strength(strength)
{
}

std::ostream & operator<<(std::ostream & os, tchecker::parsing::sync_constraint_t const & c)
{
  return os << c._process->name() << "@" << c._event->name() << c._strength;
}

/* sync_declaration_t */

sync_declaration_t::sync_declaration_t(std::vector<std::shared_ptr<tchecker::parsing::sync_constraint_t>> const & syncs,
                                       tchecker::parsing::attributes_t const & attr, std::string const & context)
    : tchecker::parsing::inner_declaration_t(attr, context), _syncs(syncs)
{
  if (_syncs.size() == 0)
    throw std::invalid_argument("synchronization declaration is empty");

  for (auto it1 = _syncs.begin(); it1 != _syncs.end(); ++it1) {
    auto it2 = it1;
    ++it2;
    for (; it2 != _syncs.end(); ++it2) {
      if ((*it1)->process().name() == (*it2)->process().name())
        throw std::invalid_argument("same process synchronized multiple times");
    }
  }
}

sync_declaration_t::sync_declaration_t(tchecker::parsing::sync_declaration_t const &) = default;

sync_declaration_t::sync_declaration_t(tchecker::parsing::sync_declaration_t &&) = default;

sync_declaration_t::~sync_declaration_t() = default;

tchecker::parsing::sync_declaration_t & sync_declaration_t::operator=(tchecker::parsing::sync_declaration_t const &) = default;

tchecker::parsing::sync_declaration_t & sync_declaration_t::operator=(tchecker::parsing::sync_declaration_t &&) = default;

void sync_declaration_t::visit(tchecker::parsing::declaration_visitor_t & v) const { v.visit(*this); }

std::ostream & sync_declaration_t::output(std::ostream & os) const
{
  os << "sync:";
  for (auto it = _syncs.begin(); it != _syncs.end(); ++it) {
    if (it != _syncs.begin())
      os << " : ";
    os << *it;
  }
  os << _attr;

  return os;
}

/* system_declaration_t */

system_declaration_t::system_declaration_t(std::string const & name, tchecker::parsing::attributes_t const & attr,
                                           std::string const & context)
    : tchecker::parsing::declaration_t(attr, context), _name(name)
{
  if (name.empty())
    throw std::invalid_argument("system declaration has empty name");
}

system_declaration_t::system_declaration_t(tchecker::parsing::system_declaration_t const &) = default;

system_declaration_t::system_declaration_t(tchecker::parsing::system_declaration_t &&) = default;

system_declaration_t::~system_declaration_t() = default;

tchecker::parsing::system_declaration_t &
system_declaration_t::operator=(tchecker::parsing::system_declaration_t const &) = default;

tchecker::parsing::system_declaration_t & system_declaration_t::operator=(tchecker::parsing::system_declaration_t &&) = default;

tchecker::range_t<system_declaration_t::const_iterator_t> system_declaration_t::declarations() const
{
  return tchecker::make_range(_decl.begin(), _decl.end());
}

std::shared_ptr<tchecker::parsing::int_declaration_t const>
system_declaration_t::get_int_declaration(std::string const & name) const
{
  return get_declaration(name, _ints);
}

bool system_declaration_t::insert_int_declaration(std::shared_ptr<tchecker::parsing::int_declaration_t> const & d)
{
  assert(d != nullptr);
  return insert_declaration(d->name(), d, _ints);
}

std::shared_ptr<tchecker::parsing::clock_declaration_t const>
system_declaration_t::get_clock_declaration(std::string const & name) const
{
  return get_declaration(name, _clocks);
}

bool system_declaration_t::insert_clock_declaration(std::shared_ptr<tchecker::parsing::clock_declaration_t> const & d)
{
  assert(d != nullptr);
  return insert_declaration(d->name(), d, _clocks);
}

std::shared_ptr<tchecker::parsing::process_declaration_t const>
system_declaration_t::get_process_declaration(std::string const & name) const
{
  return get_declaration(name, _procs);
}

bool system_declaration_t::insert_process_declaration(std::shared_ptr<tchecker::parsing::process_declaration_t> const & d)
{
  assert(d != nullptr);
  return insert_declaration(d->name(), d, _procs);
}

std::shared_ptr<tchecker::parsing::event_declaration_t const>
system_declaration_t::get_event_declaration(std::string const & name) const
{
  return get_declaration(name, _events);
}

bool system_declaration_t::insert_event_declaration(std::shared_ptr<tchecker::parsing::event_declaration_t> const & d)
{
  assert(d != nullptr);
  return insert_declaration(d->name(), d, _events);
}

std::shared_ptr<tchecker::parsing::location_declaration_t const>
system_declaration_t::get_location_declaration(std::string const & proc, std::string const & name) const
{
  return get_declaration(location_map_key(proc, name), _locs);
}

bool system_declaration_t::insert_location_declaration(std::shared_ptr<tchecker::parsing::location_declaration_t> const & d)
{
  assert(d != nullptr);
  auto key = location_map_key(d->process().name(), d->name());
  return insert_declaration(key, d, _locs);
}

bool system_declaration_t::insert_edge_declaration(std::shared_ptr<tchecker::parsing::edge_declaration_t> const & d)
{
  assert(d != nullptr);
  _decl.push_back(d);
  return true;
}

bool system_declaration_t::insert_sync_declaration(std::shared_ptr<tchecker::parsing::sync_declaration_t> const & d)
{
  assert(d != nullptr);
  _decl.push_back(d);
  return true;
}

void system_declaration_t::visit(tchecker::parsing::declaration_visitor_t & v) const { v.visit(*this); }

std::ostream & system_declaration_t::output(std::ostream & os) const
{
  os << "system:" << _name << _attr << std::endl;
  for (std::shared_ptr<tchecker::parsing::inner_declaration_t const> const & d : _decl)
    os << *d << std::endl;
  return os;
}

} // end of namespace parsing

} // end of namespace tchecker
