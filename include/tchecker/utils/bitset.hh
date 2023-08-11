/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_BITSET_HH
#define TCHECKER_BITSET_HH

#include <string>

#include <boost/dynamic_bitset.hpp>

/*!
 \file bitset.hh
 \brief Utility functions on boost dynamic bit sets
 */

namespace tchecker {

/*!
 \brief String representation of a bitset
 \param bitset : a bit set
 \param name : map identifiers to names
 \return string representation of bitset with element names from name
 \throw may throw if name throws
 */
std::string to_string(boost::dynamic_bitset<> const & bitset, std::function<std::string(std::size_t)> name);

/*!
 \brief Lexical ordering on bit sets
 \param bitset1 : first bit set
 \param bitset2 : second bit set
 \pre bitset1 and bitset2 have same size
 \return < 0 if bitset1 is smaller than bitset2, 0 if bitset1 and bitset2 are equal, > 0 if bitset1
 is greater than bitset2
 \throw std::invalid_argument : if bitset1 and bitset2 do not have the same size
*/
int lexical_cmp(boost::dynamic_bitset<> const & bitset1, boost::dynamic_bitset<> const & bitset2);

} // namespace tchecker

#endif // TCHECKER_BITSET_HH