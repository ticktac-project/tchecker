/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <sstream>

namespace tchecker {

/*!
 \brief Split a string into a vector of strings
 \param s : a string
 \param d : a delimiter
 \return vector of substrings in s delimited by d
*/
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
