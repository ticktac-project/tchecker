/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <functional>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/basictypes.hh"
#include "tchecker/utils/ordering.hh"
#include "tchecker/variables/clocks.hh"

namespace tchecker {

/* clock_variables_t */

tchecker::clock_id_t clock_variables_t::declare(std::string const & name, tchecker::clock_id_t size)
{
  tchecker::clock_info_t info{size};
  tchecker::clock_id_t id =
      tchecker::array_variables_t<tchecker::clock_id_t, tchecker::clock_info_t, tchecker::clock_index_t>::declare(name, info);
  if (id >= tchecker::REFCLOCK_ID || (tchecker::REFCLOCK_ID - id < size))
    throw std::runtime_error("no clock identifier left");
  return id;
}

/* clock_constraint_t */

clock_constraint_t::clock_constraint_t(tchecker::clock_id_t id1, tchecker::clock_id_t id2, enum tchecker::ineq_cmp_t cmp,
                                       tchecker::integer_t value)
    : _id1(id1), _id2(id2), _cmp(cmp), _value(value)
{
  if (_id1 == tchecker::REFCLOCK_ID && _id2 == tchecker::REFCLOCK_ID)
    throw std::invalid_argument("Invalid clock identifiers in clock constraint");
}

bool operator==(tchecker::clock_constraint_t const & c1, tchecker::clock_constraint_t const & c2)
{
  return (c1.id1() == c2.id1() && c1.id2() == c2.id2() && c1.comparator() == c2.comparator() && c1.value() == c2.value());
}

bool operator!=(tchecker::clock_constraint_t const & c1, tchecker::clock_constraint_t const & c2) { return !(c1 == c2); }

std::size_t hash_value(tchecker::clock_constraint_t const & c)
{
  std::size_t h = 0;
  boost::hash_combine(h, c.id1());
  boost::hash_combine(h, c.id2());
  boost::hash_combine(h, c.comparator());
  boost::hash_combine(h, c.value());
  return h;
}

std::ostream & operator<<(std::ostream & os, tchecker::clock_constraint_t const & c)
{
  os << c._id1 << "-" << c._id2 << (c._cmp == tchecker::LT ? "<" : "<=") << c._value;
  return os;
}

std::ostream & output(std::ostream & os, tchecker::clock_constraint_t const & c, tchecker::clock_index_t const & index)
{
  tchecker::clock_id_t id1 = c.id1(), id2 = c.id2();
  if (id1 != tchecker::REFCLOCK_ID)
    os << index.value(id1);
  if (id2 != tchecker::REFCLOCK_ID) {
    if (id1 != tchecker::REFCLOCK_ID)
      os << "-";
    os << index.value(id2);
  }
  if (id1 != tchecker::REFCLOCK_ID)
    os << (c.comparator() == tchecker::LT ? "<" : "<=");
  else
    os << (c.comparator() == tchecker::LT ? ">" : ">=");
  os << (id1 != tchecker::REFCLOCK_ID ? c.value() : -c.value());
  return os;
}

std::string to_string(tchecker::clock_constraint_t const & c, tchecker::clock_index_t const & index)
{
  std::stringstream ss;
  tchecker::output(ss, c, index);
  return ss.str();
}

int lexical_cmp(tchecker::clock_constraint_t const & c1, tchecker::clock_constraint_t const & c2)
{
  if (c1.id1() != c2.id1())
    return (c1.id1() < c2.id2() ? -1 : 1);
  if (c1.id2() != c2.id2())
    return (c1.id2() < c2.id2() ? -1 : 1);
  if (c1.comparator() != c2.comparator())
    return (c1.comparator() == tchecker::LT ? -1 : 1);
  return (c1.value() == c2.value() ? 0 : (c1.value() < c2.value() ? -1 : 1));
}

int lexical_cmp(tchecker::clock_constraint_container_t const & c1, tchecker::clock_constraint_container_t const & c2)
{
  return tchecker::lexical_cmp<tchecker::clock_constraint_container_const_iterator_t,
                               tchecker::clock_constraint_container_const_iterator_t,
                               int (*)(tchecker::clock_constraint_t const &, tchecker::clock_constraint_t const &)>(
      c1.begin(), c1.end(), c2.begin(), c2.end(), tchecker::lexical_cmp);
}

std::string to_string(tchecker::clock_constraint_container_t const & c, tchecker::clock_index_t const & index)
{
  std::stringstream ss;
  tchecker::output_clock_constraints(ss, tchecker::make_range(c.begin(), c.end()), index);
  return ss.str();
}

/* clock_reset_t */

clock_reset_t::clock_reset_t(tchecker::clock_id_t left_id, tchecker::clock_id_t right_id, tchecker::integer_t value)
    : _left_id(left_id), _right_id(right_id), _value(value)
{
  if (_left_id == tchecker::REFCLOCK_ID)
    throw std::invalid_argument("reference clock is not a left-value clock");

  if (_value < 0)
    throw std::invalid_argument("reset value should be >= 0");
}

bool operator==(tchecker::clock_reset_t const & r1, tchecker::clock_reset_t const & r2)
{
  return (r1.left_id() == r2.left_id() && r1.right_id() == r2.right_id() && r1.value() == r2.value());
}

bool operator!=(tchecker::clock_reset_t const & r1, tchecker::clock_reset_t const & r2) { return !(r1 == r2); }

std::size_t hash_value(tchecker::clock_reset_t const & r)
{
  std::size_t h = 0;
  boost::hash_combine(h, r.left_id());
  boost::hash_combine(h, r.right_id());
  boost::hash_combine(h, r.value());
  return h;
}

std::ostream & operator<<(std::ostream & os, tchecker::clock_reset_t const & r)
{
  os << r._left_id << "=" << r._right_id << "+" << r._value;
  return os;
}

std::ostream & output(std::ostream & os, tchecker::clock_reset_t const & r, tchecker::clock_index_t const & index)
{
  os << index.value(r.left_id());
  os << "=";
  tchecker::clock_id_t right_id = r.right_id();
  tchecker::integer_t value = r.value();
  if (right_id != tchecker::REFCLOCK_ID) {
    if (value != 0)
      os << value << "+";
    os << index.value(right_id);
  }
  else
    os << value;
  return os;
}

std::string to_string(tchecker::clock_reset_t const & r, tchecker::clock_index_t const & index)
{
  std::stringstream ss;
  tchecker::output(ss, r, index);
  return ss.str();
}

int lexical_cmp(tchecker::clock_reset_t const & r1, tchecker::clock_reset_t const & r2)
{
  if (r1.left_id() != r2.left_id())
    return (r1.left_id() < r2.left_id() ? -1 : 1);
  if (r1.right_id() != r2.right_id())
    return (r1.right_id() < r2.right_id() ? -1 : 1);
  return (r1.value() == r2.value() ? 0 : (r1.value() < r2.value() ? -1 : 1));
}

int lexical_cmp(tchecker::clock_reset_container_t const & c1, tchecker::clock_reset_container_t const & c2)
{
  return tchecker::lexical_cmp<tchecker::clock_reset_container_const_iterator_t,
                               tchecker::clock_reset_container_const_iterator_t,
                               int (*)(tchecker::clock_reset_t const &, tchecker::clock_reset_t const &)>(
      c1.begin(), c1.end(), c2.begin(), c2.end(), tchecker::lexical_cmp);
}

std::string to_string(tchecker::clock_reset_container_t const & c, tchecker::clock_index_t const & index)
{
  std::stringstream ss;
  tchecker::output_clock_resets(ss, tchecker::make_range(c.begin(), c.end()), index);
  return ss.str();
}

void clock_reset_to_constraints(tchecker::clock_reset_t const & r, tchecker::clock_constraint_container_t & cc)
{
  cc.push_back(tchecker::clock_constraint_t{r.left_id(), r.right_id(), tchecker::clock_constraint_t::LE, r.value()});
  cc.push_back(tchecker::clock_constraint_t{r.right_id(), r.left_id(), tchecker::clock_constraint_t::LE, -r.value()});
}

void clock_resets_to_constraints(tchecker::clock_reset_container_t const & rc, tchecker::clock_constraint_container_t & cc)
{
  for (tchecker::clock_reset_t const & r : rc)
    tchecker::clock_reset_to_constraints(r, cc);
}

/* reference_clock_variables_t */

reference_clock_variables_t::reference_clock_variables_t(std::vector<std::string> const & proc_refname_map)
    : _refcount(0), _procmap(proc_refname_map.size())
{
  if (proc_refname_map.empty())
    throw std::invalid_argument("Empty reference clocks not allowed");
  assert(proc_refname_map.size() < std::numeric_limits<tchecker::process_id_t>::max());
  for (tchecker::process_id_t pid = 0; pid < proc_refname_map.size(); ++pid) {
    tchecker::clock_id_t refid = 0;
    try {
      refid = tchecker::flat_clock_variables_t::id(proc_refname_map[pid]);
    }
    catch (...) {
      refid = declare_reference_clock(proc_refname_map[pid]);
    }
    _procmap[pid] = refid;
  }
}

tchecker::clock_id_t reference_clock_variables_t::declare(std::string const & name, std::string const & refclock)
{
  tchecker::clock_id_t refid = 0;
  try {
    refid = tchecker::flat_clock_variables_t::id(refclock);
  }
  catch (...) {
    throw std::invalid_argument("unknown reference clock " + refclock);
  }
  assert(refid < _refcount);
  tchecker::clock_id_t id = declare(name, refid);
  return id;
}

tchecker::clock_constraint_t reference_clock_variables_t::translate(tchecker::clock_constraint_t const & c) const
{
  assert(c.id1() != tchecker::REFCLOCK_ID || c.id2() != tchecker::REFCLOCK_ID);
  assert(c.id1() == tchecker::REFCLOCK_ID || c.id1() < size() - _refcount);
  assert(c.id2() == tchecker::REFCLOCK_ID || c.id2() < size() - _refcount);

  tchecker::clock_id_t id1 = c.id1();
  id1 = (id1 == tchecker::REFCLOCK_ID ? refclock_of_system_clock(c.id2()) : translate_system_clock(id1));
  tchecker::clock_id_t id2 = c.id2();
  id2 = (id2 == tchecker::REFCLOCK_ID ? refclock_of_system_clock(c.id1()) : translate_system_clock(id2));

  return tchecker::clock_constraint_t{id1, id2, c.comparator(), c.value()};
}

tchecker::clock_reset_t reference_clock_variables_t::translate(tchecker::clock_reset_t const & r) const
{
  assert(r.left_id() < size() - _refcount);
  assert(r.right_id() == tchecker::REFCLOCK_ID || r.right_id() < size() - _refcount);

  tchecker::clock_id_t left_id = translate_system_clock(r.left_id());
  tchecker::clock_id_t right_id = r.right_id();
  right_id = (right_id == tchecker::REFCLOCK_ID ? refclock_of_system_clock(r.left_id()) : translate_system_clock(right_id));

  return tchecker::clock_reset_t{left_id, right_id, r.value()};
}

tchecker::clock_id_t reference_clock_variables_t::declare(std::string const & name, tchecker::clock_id_t refid)
{
  tchecker::clock_id_t size = this->size();
  assert(refid < _refcount || refid == size); // declared reference clock or declaring a reference clock

  tchecker::clock_id_t id = size;
  tchecker::clock_info_t info{1}; // size 1
  tchecker::flat_clock_variables_t::declare(id, name, info);

  _refmap.resize(id + 1);
  _refmap[id] = refid;

  return id;
}

tchecker::clock_id_t reference_clock_variables_t::declare_reference_clock(std::string const & name)
{
  if (_refcount != size())
    throw std::runtime_error("Reference clocks must be declared before clock variables");

  tchecker::clock_id_t id = declare(name, _refcount);
  assert(id == _refcount);
  ++_refcount;

  return id;
}

/* Reference clock builders */

tchecker::reference_clock_variables_t single_reference_clocks(tchecker::flat_clock_variables_t const & flat_clocks,
                                                              tchecker::process_id_t proc_count)
{
  std::string const zero_clock_name = "0";

  std::vector<std::string> proc_refname_map(proc_count, zero_clock_name);

  tchecker::reference_clock_variables_t reference_clocks(proc_refname_map);

  tchecker::clock_id_t clocks_count = flat_clocks.size();
  for (tchecker::clock_id_t clock_id = 0; clock_id < clocks_count; ++clock_id) {
    std::string const & clock_name = flat_clocks.index().value(clock_id);
    reference_clocks.declare(clock_name, zero_clock_name);
  }

  return reference_clocks;
}

tchecker::reference_clock_variables_t process_reference_clocks(tchecker::variable_access_map_t const & vaccess_map,
                                                               tchecker::flat_clock_variables_t const & flat_clocks,
                                                               tchecker::process_id_t proc_count)
{
  if (proc_count == 0)
    throw std::invalid_argument("number of processes should be > 0");

  tchecker::clock_id_t const flat_clocks_count = flat_clocks.size();

  if (proc_count > std::numeric_limits<tchecker::clock_id_t>::max())
    throw std::invalid_argument("too many processes");

  unsigned long long all_clocks_count = proc_count + flat_clocks_count;
  if (all_clocks_count > std::numeric_limits<tchecker::clock_id_t>::max())
    throw std::invalid_argument("too many offset variables");

  // compute reference clock names
  std::vector<std::string> proc_refname_map;
  for (tchecker::process_id_t pid = 0; pid < proc_count; ++pid)
    proc_refname_map.push_back("$" + std::to_string(pid));

  tchecker::reference_clock_variables_t reference_clocks(proc_refname_map);

  // declare clock variables
  for (tchecker::clock_id_t clock_id = 0; clock_id < flat_clocks_count; ++clock_id) {
    auto accessing_processes = vaccess_map.accessing_processes(clock_id, tchecker::VTYPE_CLOCK, tchecker::VACCESS_ANY);

    if (std::distance(accessing_processes.begin(), accessing_processes.end()) != 1)
      throw std::invalid_argument("bad clock access, cannot build reference clocks");

    tchecker::process_id_t pid = *accessing_processes.begin();
    std::string const & clock_name = flat_clocks.index().value(clock_id);
    reference_clocks.declare("$" + clock_name, proc_refname_map[pid]);
  }

  return reference_clocks;
}

} // end of namespace tchecker
