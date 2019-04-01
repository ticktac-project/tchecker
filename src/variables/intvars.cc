/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>
#include <stdexcept>

#include "tchecker/variables/intvars.hh"

namespace tchecker {
  
  /* intvar_info_t */
  
  intvar_info_t::intvar_info_t(unsigned int size, tchecker::integer_t min, tchecker::integer_t max,
                               tchecker::integer_t initial_value)
  : _size(size),
  _min(min),
  _max(max),
  _initial_value(initial_value)
  {
    if (_size == 0)
      throw std::invalid_argument("expecting size > 0");
    
    if ( (_min > _initial_value) || (_initial_value > _max) )
      throw std::invalid_argument("expecting min <= initial_value <= max");
  }
  
  
  
  
  /* intvars_valuation_t */
  
  void intvars_valuation_destruct_and_deallocate(tchecker::intvars_valuation_t * v)
  {
    tchecker::intvars_valuation_t::destruct(v);
    delete[] reinterpret_cast<char *>(v);
  }
  
  
  std::ostream & output(std::ostream & os, tchecker::intvars_valuation_t const & intvars_val,
                        tchecker::intvar_index_t const & index)
  {
    auto const size = index.size();
    
    for (tchecker::intvar_id_t id = 0; id < size; ++id) {
      if (id > 0)
        os << ",";
      os << index.value(id) << "=" << intvars_val[id];
    }
    return os;
  }
  
  
  std::string to_string(tchecker::intvars_valuation_t const & intvars_val,
                        tchecker::intvar_index_t const & index)
  {
    std::stringstream sstream;
    output(sstream, intvars_val, index);
    return sstream.str();
  }
  
} // end of namespace tchecker
