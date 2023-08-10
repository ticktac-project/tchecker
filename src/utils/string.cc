/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>

#include "tchecker/utils/string.hh"

namespace tchecker {

std::vector<std::string> split(std::string const & s, char d)
{
  std::stringstream sstream(s);
  std::vector<std::string> v;
  std::string substr;
  while (std::getline(sstream, substr, d))
    v.push_back(substr);
  return v;
}

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

} // namespace tchecker
