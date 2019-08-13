/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <stdexcept>

#include "tchecker/variables/clocks.hh"

namespace tchecker {
  
  /* clock_constraint_t */
  
  clock_constraint_t::clock_constraint_t(tchecker::clock_id_t id1,
                                         tchecker::clock_id_t id2,
                                         enum tchecker::clock_constraint_t::comparator_t cmp,
                                         tchecker::integer_t value)
  : _id1(id1), _id2(id2), _cmp(cmp), _value(value)
  {}
  
  
  std::ostream & operator<< (std::ostream & os, tchecker::clock_constraint_t const & c)
  {
    os << c._id1 << "-" << c._id2 << (c._cmp == tchecker::clock_constraint_t::LT ? "<" : "<=") << c._value;
    return os;
  }
  
  
  std::ostream & output(std::ostream & os, tchecker::clock_constraint_t const & c, tchecker::clock_index_t const & index)
  {
    tchecker::clock_id_t id1 = c.id1(), id2 = c.id2();
    if (id1 != tchecker::zero_clock_id)
      os << index.value(id1);
    if (id2 != tchecker::zero_clock_id) {
      if (id1 != tchecker::zero_clock_id)
        os << "-";
      os << index.value(id2);
    }
    if (id1 != tchecker::zero_clock_id)
      os << (c.comparator() == tchecker::clock_constraint_t::LT ? "<" : "<=");
    else
      os << (c.comparator() == tchecker::clock_constraint_t::LT ? ">" : ">=");
    os << (id1 != tchecker::zero_clock_id ? c.value() : - c.value());
    return os;
  }
  
  
  
  
  /* clock_reset_t */
  
  clock_reset_t::clock_reset_t(tchecker::clock_id_t left_id,
                               tchecker::clock_id_t right_id,
                               tchecker::integer_t value)
  : _left_id(left_id),
  _right_id(right_id),
  _value(value)
  {
    if (_left_id == tchecker::zero_clock_id)
      throw std::invalid_argument("zero clock is not a left-value clock");
    
    if (_value < 0)
      throw std::invalid_argument("reset value should be >= 0");
  }
  
  
  std::ostream & operator<< (std::ostream & os, tchecker::clock_reset_t const & r)
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
    if (right_id != tchecker::zero_clock_id) {
      if (value != 0)
        os << value << "+";
      os << index.value(right_id);
    }
    else
      os << value;
    return os;
  }
  
} // end of namespace tchecker
