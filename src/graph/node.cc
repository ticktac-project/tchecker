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

#include "tchecker/graph/node.hh"

namespace tchecker {

namespace graph {

/* node_flags_t */

node_flags_t::node_flags_t(bool initial, bool final) : _flags(0)
{
  if (initial)
    _flags = _flags | tchecker::graph::node_flags_t::NODE_INITIAL;
  if (final)
    _flags = _flags | tchecker::graph::node_flags_t::NODE_FINAL;
}

void node_flags_t::initial(bool status)
{
  if (status)
    _flags = _flags | tchecker::graph::node_flags_t::NODE_INITIAL;
  else
    _flags = _flags & ~tchecker::graph::node_flags_t::NODE_INITIAL;
}

void node_flags_t::final(bool status)
{
  if (status)
    _flags = _flags | tchecker::graph::node_flags_t::NODE_FINAL;
  else
    _flags = _flags & ~tchecker::graph::node_flags_t::NODE_FINAL;
}

bool operator==(tchecker::graph::node_flags_t const & n1, tchecker::graph::node_flags_t const & n2)
{
  return (n1.initial() == n2.initial()) && (n1.final() == n2.final());
}

bool operator!=(tchecker::graph::node_flags_t const & n1, tchecker::graph::node_flags_t const & n2) { return !(n1 == n2); }

std::size_t hash_value(tchecker::graph::node_flags_t const & n)
{
  std::size_t h = boost::hash_value(n.initial());
  boost::hash_combine(h, n.final());
  return h;
}

int lexical_cmp(tchecker::graph::node_flags_t const & n1, tchecker::graph::node_flags_t const & n2)
{
  if (n1.initial() != n2.initial())
    return n1.initial() - n2.initial();
  return n1.final() - n2.final();
}

void attributes(tchecker::graph::node_flags_t const & n, std::map<std::string, std::string> & m)
{
  if (n.initial())
    m["initial"] = "true";
  if (n.final())
    m["final"] = "true";
}

/* node_zg_state_t */

node_zg_state_t::node_zg_state_t(tchecker::zg::state_sptr_t const & s) : _state(s) {}

node_zg_state_t::node_zg_state_t(tchecker::zg::const_state_sptr_t const & s) : _state(s) {}

/* node_refzg_state_t */

node_refzg_state_t::node_refzg_state_t(tchecker::refzg::state_sptr_t const & s) : _state(s) {}

node_refzg_state_t::node_refzg_state_t(tchecker::refzg::const_state_sptr_t const & s) : _state(s) {}

} // namespace graph

} // namespace tchecker