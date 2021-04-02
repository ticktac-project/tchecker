/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#if BOOST_VERSION <= 106600
#include <boost/functional/hash.hpp>
#else
#include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/zg/transition.hh"

namespace tchecker {

namespace zg {

bool operator==(tchecker::zg::transition_t const & t1, tchecker::zg::transition_t const & t2)
{
  return tchecker::ta::operator==(t1, t2);
}

std::size_t hash_value(tchecker::zg::transition_t const & t) { return tchecker::ta::hash_value(t); }

int lexical_cmp(tchecker::zg::transition_t const & t1, tchecker::zg::transition_t const & t2)
{
  return tchecker::ta::lexical_cmp(t1, t2);
}

} // end of namespace zg

} // end of namespace tchecker
