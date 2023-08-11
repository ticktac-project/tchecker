/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>

#include "tchecker/utils/bitset.hh"

namespace tchecker {

std::string to_string(boost::dynamic_bitset<> const & bitset, std::function<std::string(std::size_t)> name)
{
  std::stringstream ss;
  ss << "{";
  auto first = bitset.find_first();
  for (auto i = first; i != bitset.npos; i = bitset.find_next(i)) {
    if (i != first)
      ss << ",";
    ss << name(i);
  }
  ss << "}";
  return ss.str();
}

int lexical_cmp(boost::dynamic_bitset<> const & bitset1, boost::dynamic_bitset<> const & bitset2)
{
  if (bitset1.size() != bitset2.size())
    throw std::invalid_argument("tchecker::lexical_cmp: bit sets of incompatible sizes");

  std::size_t i1 = bitset1.find_first(), i2 = bitset2.find_first();
  for (; (i1 != bitset1.npos) && (i2 != bitset2.npos); i1 = bitset1.find_next(i1), i2 = bitset2.find_next(i2)) {
    if (i1 < i2)
      return 1;
    else if (i1 > i2)
      return -1;
  }

  if (i1 == bitset1.npos) {
    if (i2 == bitset2.npos)
      return 0;
    else
      return -1;
  }

  return 1;
}

} // namespace tchecker