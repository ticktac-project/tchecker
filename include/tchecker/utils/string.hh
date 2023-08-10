/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STRING_HH
#define TCHECKER_STRING_HH

#include <functional>
#include <string>
#include <vector>

#include <boost/dynamic_bitset.hpp>

/*!
 \file string.hh
 \brief String manipulation functions
 */

namespace tchecker {

/*!
 \brief Split a string into a vector of strings
 \param s : a string
 \param d : a delimiter
 \return vector of substrings in s delimited by d
*/
std::vector<std::string> split(std::string const & s, char d);

/*!
 \brief String representation of a bitset
 \param bitset : a bit set
 \param name : map identifiers to names
 \return string representation of bitset with element names from name
 \throw may throw if name throws
 */
std::string to_string(boost::dynamic_bitset<> const & bitset, std::function<std::string(std::size_t)> name);

} // namespace tchecker

#endif // TCHECKER_STRING_HH