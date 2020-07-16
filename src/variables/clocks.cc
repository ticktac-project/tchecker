/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <functional>
#include <stdexcept>

#if BOOST_VERSION <= 106600
# include <boost/functional/hash.hpp>
#else
# include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/utils/ordering.hh"
#include "tchecker/variables/clocks.hh"

namespace tchecker {
  
  /* clock_constraint_t */
  
  clock_constraint_t::clock_constraint_t(tchecker::clock_id_t id1,
                                         tchecker::clock_id_t id2,
                                         enum tchecker::clock_constraint_t::comparator_t cmp,
                                         tchecker::integer_t value)
  : _id1(id1), _id2(id2), _cmp(cmp), _value(value)
  {}
  
  
  bool operator== (tchecker::clock_constraint_t const & c1, tchecker::clock_constraint_t const & c2)
  {
    return (c1.id1() == c2.id1() &&
            c1.id2() == c2.id2() &&
            c1.comparator() == c2.comparator() &&
            c1.value() == c2.value());
  }
  
  
  bool operator!= (tchecker::clock_constraint_t const & c1, tchecker::clock_constraint_t const & c2)
  {
    return !(c1 == c2);
  }
  
  
  std::size_t hash_value(tchecker::clock_constraint_t const & c)
  {
    std::size_t h = 0;
    boost::hash_combine(h, c.id1());
    boost::hash_combine(h, c.id2());
    boost::hash_combine(h, c.comparator());
    boost::hash_combine(h, c.value());
    return h;
  }
  
  
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
  
  
  int lexical_cmp(tchecker::clock_constraint_t const & c1, tchecker::clock_constraint_t const & c2)
  {
    if (c1.id1() != c2.id1())
      return (c1.id1() < c2.id2() ? -1 : 1);
    if (c1.id2() != c2.id2())
      return (c1.id2() < c2.id2() ? -1 : 1);
    if (c1.comparator() != c2.comparator())
      return (c1.comparator() == tchecker::clock_constraint_t::LT ? -1 : 1);
    return (c1.value() == c2.value() ? 0 : (c1.value() < c2.value() ? -1 : 1));
  }
  
  
  int lexical_cmp(tchecker::clock_constraint_container_t const & c1,
                  tchecker::clock_constraint_container_t const & c2)
  {
    return tchecker::lexical_cmp
    <tchecker::clock_constraint_container_const_iterator_t,
    tchecker::clock_constraint_container_const_iterator_t,
    int (*) (tchecker::clock_constraint_t const &, tchecker::clock_constraint_t const &)
    >
    (c1.begin(), c1.end(), c2.begin(), c2.end(), tchecker::lexical_cmp);
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
  
  
  bool operator== (tchecker::clock_reset_t const & r1, tchecker::clock_reset_t const & r2)
  {
    return (r1.left_id() == r2.left_id() &&
            r1.right_id() == r2.right_id() &&
            r1.value() == r2.value());
  }
  
  
  bool operator!= (tchecker::clock_reset_t const & r1, tchecker::clock_reset_t const & r2)
  {
    return !(r1 == r2);
  }
  
  
  std::size_t hash_value(tchecker::clock_reset_t const & r)
  {
    std::size_t h = 0;
    boost::hash_combine(h, r.left_id());
    boost::hash_combine(h, r.right_id());
    boost::hash_combine(h, r.value());
    return h;
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
  
  
  int lexical_cmp(tchecker::clock_reset_t const & r1, tchecker::clock_reset_t const & r2)
  {
    if (r1.left_id() != r2.left_id())
      return (r1.left_id() < r2.left_id() ? -1 : 1);
    if (r1.right_id() != r2.right_id())
      return (r1.right_id() < r2.right_id() ? -1 : 1);
    return (r1.value() == r2.value() ? 0 : (r1.value() < r2.value() ? -1 : 1));
  }
  
  
  int lexical_cmp(tchecker::clock_reset_container_t const & c1,
                  tchecker::clock_reset_container_t const & c2)
  {
    return tchecker::lexical_cmp
    <tchecker::clock_reset_container_const_iterator_t,
    tchecker::clock_reset_container_const_iterator_t,
    int (*) (tchecker::clock_reset_t const &, tchecker::clock_reset_t const &)
    >
    (c1.begin(), c1.end(), c2.begin(), c2.end(), tchecker::lexical_cmp);
  }
  
} // end of namespace tchecker
