/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/system/loc.hh"

namespace tchecker {

namespace system {

/* loc_t */

loc_t::loc_t(tchecker::process_id_t pid, tchecker::loc_id_t id, std::string const & name,
             tchecker::system::attributes_t const & attributes)
    : _pid(pid), _id(id), _name(name), _attributes(attributes)
{
  if (_name.empty())
    throw std::invalid_argument("empty name");
}

/* locs_t */

locs_t::locs_t(tchecker::system::locs_t const & locs) { add_locations(locs); }

locs_t::locs_t(tchecker::system::locs_t && locs)
    : _locs(std::move(locs._locs)), _process_locs(std::move(locs._process_locs)), _initial_locs(std::move(locs._initial_locs)),
      _locs_index(std::move(locs._locs_index))
{
}

locs_t::~locs_t() { clear(); }

tchecker::system::locs_t & locs_t::operator=(tchecker::system::locs_t const & locs)
{
  if (this != &locs) {
    clear();
    add_locations(locs);
  }
  return *this;
}

tchecker::system::locs_t & locs_t::operator=(tchecker::system::locs_t && locs)
{
  if (this != &locs) {
    clear();
    _locs = std::move(locs._locs);
    _process_locs = std::move(locs._process_locs);
    _initial_locs = std::move(locs._initial_locs);
    _locs_index = std::move(locs._locs_index);
  }
  return *this;
}

void locs_t::clear()
{
  _locs_index.clear();
  _process_locs.clear();
  _initial_locs.clear();
  _locs.clear();
}

void locs_t::add_location(tchecker::process_id_t pid, std::string const & name,
                          tchecker::system::attributes_t const & attributes)
{
  tchecker::loc_id_t id = _locs.size();

  if (!tchecker::valid_loc_id(id))
    throw std::runtime_error("add_location: invalid location identifier");

  tchecker::system::loc_shared_ptr_t loc(new tchecker::system::loc_t(pid, id, name, attributes));

  if (pid >= _locs_index.size())
    _locs_index.resize(pid + 1);
  _locs_index[pid].add(name, loc); // may throw

  if (pid >= _process_locs.size())
    _process_locs.resize(pid + 1);
  _process_locs[pid].push_back(loc);

  _locs.push_back(loc);

  auto range = attributes.range("initial");
  if (range.begin() != range.end()) {
    if (pid >= _initial_locs.size())
      _initial_locs.resize(pid + 1);
    _initial_locs[pid].push_back(loc);
  }

  assert(_locs.back()->id() == _locs.size() - 1);
}

tchecker::system::locs_t::locations_identifiers_range_t locs_t::locations_identifiers() const
{
  return tchecker::make_integer_range<tchecker::loc_id_t>(0, locations_count());
}

void locs_t::add_locations(tchecker::system::locs_t const & locs)
{
  for (tchecker::system::loc_const_shared_ptr_t loc : locs._locs)
    add_location(loc->pid(), loc->name(), loc->attributes());
}

tchecker::system::loc_const_shared_ptr_t locs_t::location(tchecker::process_id_t pid, std::string const & name) const
{
  if (pid >= _locs_index.size())
    throw std::invalid_argument("Unknow pid, cannot find location");
  return _locs_index[pid].value(name);
}

locs_t::const_iterator_t::const_iterator_t(std::vector<tchecker::system::loc_shared_ptr_t>::const_iterator const & it)
    : std::vector<tchecker::system::loc_shared_ptr_t>::const_iterator(it)
{
}

tchecker::range_t<tchecker::system::locs_t::const_iterator_t> locs_t::locations() const
{
  return tchecker::make_range(const_iterator_t(_locs.begin()), const_iterator_t(_locs.end()));
}

tchecker::range_t<tchecker::system::locs_t::const_iterator_t> locs_t::locations(tchecker::process_id_t pid) const
{
  if (pid >= _process_locs.size())
    throw std::invalid_argument("tchecker::locs_t::locations: pid out of range");
  return tchecker::make_range(const_iterator_t{_process_locs[pid].begin()}, const_iterator_t{_process_locs[pid].end()});
}

tchecker::range_t<tchecker::system::locs_t::const_iterator_t> locs_t::initial_locations(tchecker::process_id_t pid) const
{
  if (pid >= _initial_locs.size())
    return tchecker::make_range(tchecker::system::locs_t::const_iterator_t(_empty_locs.begin()),
                                tchecker::system::locs_t::const_iterator_t(_empty_locs.end()));
  return tchecker::make_range(tchecker::system::locs_t::const_iterator_t(_initial_locs[pid].begin()),
                              tchecker::system::locs_t::const_iterator_t(_initial_locs[pid].end()));
}

bool locs_t::is_location(tchecker::loc_id_t id) const { return (id < locations_count()); }

bool locs_t::is_location(tchecker::process_id_t pid, std::string const & name) const
{
  if (pid >= _locs_index.size())
    return false;
  return _locs_index[pid].find_key(name) != _locs_index[pid].end_key_map();
}

bool locs_t::is_initial_location(tchecker::loc_id_t id) const
{
  if (!is_location(id))
    throw std::invalid_argument("Unknown location identifier");
  tchecker::process_id_t pid = _locs[id]->pid();
  for (tchecker::system::loc_shared_ptr_t const & loc : _initial_locs[pid]) {
    if (loc->id() == id)
      return true;
  }
  return false;
}

// Empty set of locations
std::vector<tchecker::system::loc_shared_ptr_t> const locs_t::_empty_locs;

} // end of namespace system

} // end of namespace tchecker
