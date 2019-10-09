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
  
  void clock_reset_container_t::clear(){
    std::vector<tchecker::clock_reset_t>::clear();
    _left_ids.clear();
    _right_ids.clear();
    _is_safe = true;
  }
  
  void clock_reset_container_t::emplace_back(tchecker::clock_id_t left_id, tchecker::clock_id_t right_id,
                                             tchecker::integer_t value) {
    assert(left_id != 0);
    //Check if clock was already assigned
    for (std::vector<tchecker::clock_id_t>::const_iterator it_left = _left_ids.cbegin(); it_left != _left_ids.cend(); ++it_left){
      assert(left_id != *it_left); // Every clock is allowed to be assigned only once. todo move this assert into the parser
    }
    _left_ids.push_back(left_id);
  
    std::vector<tchecker::clock_reset_t>::emplace_back(left_id, right_id, value);
  
    if (right_id!=0) {
      _right_ids.push_back(right_id);
      calc_safety(); // As we do not know how many will be inserted we have to recompute this each time
    }
  }
  
  bool clock_reset_container_t::is_safe() const {
    return _is_safe;
  }
  
  inline void clock_reset_container_t::calc_safety(){
    // Naive approach; Number of resets usually small so direct comparison is ok
    // If it is already unsafe it will stay unsafe
    if (_is_safe){
      for (const tchecker::clock_id_t & r_id : _right_ids) {
        for (const tchecker::clock_id_t &l_id : _left_ids) {
          if (r_id == l_id){
            _is_safe = false;
            return; //unsafe is insafe, no way around it
          }
        }
      }
    }
  
  }
  
  
} // end of namespace tchecker
