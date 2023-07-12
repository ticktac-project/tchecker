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
    : tchecker::size_info_t(size), _min(min), _max(max), _initial_value(initial_value)
{
  if ((_min > _initial_value) || (_initial_value > _max))
    throw std::invalid_argument("expecting min <= initial_value <= max");
}

/* intvar_values_range */

tchecker::intvar_values_range_t intvar_values_range(tchecker::intvar_info_t const & intvar_info)
{
  return tchecker::make_range(tchecker::integer_iterator_t{intvar_info.min()},
                              tchecker::integer_iterator_t{intvar_info.max() + 1});
}

/* integer_variables_t */

tchecker::intvar_id_t integer_variables_t::declare(std::string const & name, tchecker::intvar_id_t size,
                                                   tchecker::integer_t min, tchecker::integer_t max,
                                                   tchecker::integer_t initial)
{
  tchecker::intvar_info_t info{size, min, max, initial};
  return tchecker::array_variables_t<tchecker::intvar_id_t, tchecker::intvar_info_t, tchecker::intvar_index_t>::declare(name,
                                                                                                                        info);
}

/* flat_integer_variables_valuations_range  */

tchecker::flat_integer_variables_valuations_range_t
flat_integer_variables_valuations_range(tchecker::flat_integer_variables_t const & intvars)
{
  tchecker::flat_integer_variables_valuations_iterator_t it;
  for (tchecker::intvar_id_t id = 0; id < intvars.size(); ++id)
    it.push_back(tchecker::intvar_values_range(intvars.info(id)));
  return tchecker::make_range(it, tchecker::past_the_end_iterator);
}

/* intval_t */

void intval_destruct_and_deallocate(tchecker::intval_t * v)
{
  tchecker::intval_t::destruct(v);
  delete[] reinterpret_cast<char *>(v);
}

std::ostream & output(std::ostream & os, tchecker::intval_t const & intval, tchecker::intvar_index_t const & index)
{
  auto const size = index.size();

  for (tchecker::intvar_id_t id = 0; id < size; ++id) {
    if (id > 0)
      os << ",";
    os << index.value(id) << "=" << intval[id];
  }
  return os;
}

std::string to_string(tchecker::intval_t const & intval, tchecker::intvar_index_t const & index)
{
  std::stringstream sstream;
  output(sstream, intval, index);
  return sstream.str();
}

int lexical_cmp(tchecker::intval_t const & intval1, tchecker::intval_t const & intval2)
{
  return tchecker::lexical_cmp(
      intval1.begin(), intval1.end(), intval2.begin(), intval2.end(),
      [](tchecker::integer_t i1, tchecker::integer_t i2) -> int { return (i1 < i2 ? -1 : (i1 == i2 ? 0 : 1)); });
}

} // end of namespace tchecker
