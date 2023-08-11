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

} // namespace tchecker
