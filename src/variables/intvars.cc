/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "tchecker/utils/ordering.hh"
#include "tchecker/utils/string.hh"
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
  if (intvar_info.max() == std::numeric_limits<tchecker::integer_t>::max())
    throw std::overflow_error("tchecker::intvar_values_range: cannot build range");
  return tchecker::make_integer_range(intvar_info.min(), static_cast<tchecker::integer_t>(intvar_info.max() + 1));
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

void from_string(tchecker::intval_t & intval, tchecker::flat_integer_variables_t const & variables, std::string const & str)
{
  std::vector<std::tuple<std::string, std::string>> parsed_assignements;
  boost::dynamic_bitset<> assigned_variables{variables.size()};

  // split str according to commas
  std::vector<std::string> str_assignments = tchecker::split(str, ',');

  // then, for each resulting string, split according to =, and check we get exactly two strings (lhs, rhs)
  for (std::string const & assignment : str_assignments) {
    std::vector<std::string> parts = tchecker::split(assignment, '=');
    if (parts.size() != 2)
      throw std::invalid_argument("syntax error: " + assignment + " (should have exactly one = operator)");
    parsed_assignements.emplace_back(parts[0], parts[1]);
  }

  // for each result:
  for (auto && [lhs, rhs] : parsed_assignements) {
    // - check that lhs is a flat variable name, get corresponding index, check that it has not been updated yet
    tchecker::intvar_id_t id;
    try {
      id = variables.id(lhs);
    }
    catch (std::invalid_argument & e) {
      throw std::invalid_argument(lhs + " unknown variable name");
    }

    if (assigned_variables[id])
      throw std::invalid_argument("multiple assignments for variable " + lhs);

    // - check that rhs is an int, that its value fits in the domain of the lhs variable
    const std::regex int_regex("(-)?[0-9]+");
    if (!std::regex_match(rhs, int_regex))
      throw std::invalid_argument("syntax error: " + rhs + " is not an integer");

    long long value = std::strtoll(rhs.c_str(), nullptr, 10);
    if (value < variables.info(id).min() || value > variables.info(id).max())
      throw std::invalid_argument(rhs + " out-of-range of variable " + lhs +
                                  " domain: " + std::to_string(variables.info(id).min()) + "," +
                                  std::to_string(variables.info(id).max()) + ")");

    // - then update the lhs variable value with the rhs in intval, and store index in updated var set
    intval[id] = static_cast<tchecker::integer_t>(value);
    assigned_variables[id] = 1;
  }

  // check that every variable has been updated from the set of updated variables
  if (!assigned_variables.all())
    throw std::invalid_argument("some variables are not assigned in " + str);
}

int lexical_cmp(tchecker::intval_t const & intval1, tchecker::intval_t const & intval2)
{
  return tchecker::lexical_cmp(
      intval1.begin(), intval1.end(), intval2.begin(), intval2.end(),
      [](tchecker::integer_t i1, tchecker::integer_t i2) -> int { return (i1 < i2 ? -1 : (i1 == i2 ? 0 : 1)); });
}

} // end of namespace tchecker
