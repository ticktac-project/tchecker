/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>
#include <stdexcept>

#include "tchecker/utils/ordering.hh"
#include "tchecker/variables/intvars.hh"

namespace tchecker {
  
  /* intvar_info_t */
  
  intvar_info_t::intvar_info_t(unsigned int size, tchecker::integer_t min, tchecker::integer_t max,
                               tchecker::integer_t initial_value)
  : tchecker::size_info_t(size),
  _min(min),
  _max(max),
  _initial_value(initial_value)
  {
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
  
  
  int lexical_cmp(tchecker::intvars_valuation_t const & intvars_val1,
                  tchecker::intvars_valuation_t const & intvars_val2)
  {
    return tchecker::lexical_cmp(intvars_val1.begin(), intvars_val1.end(),
                                 intvars_val2.begin(), intvars_val2.end(),
                                 [] (tchecker::integer_t i1, tchecker::integer_t i2) -> int
                                 { return (i1 < i2 ? -1 : (i1 == i2 ? 0 : 1)); });
  }
  
} // end of namespace tchecker
