/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/refzg/path.hh"

namespace tchecker {

namespace refzg {

namespace path {

/* node_t */

node_t::node_t(tchecker::refzg::state_sptr_t const & s, bool initial, bool final)
    : tchecker::graph::node_flags_t(initial, final), tchecker::graph::node_refzg_state_t(s)
{
}

node_t::node_t(tchecker::refzg::const_state_sptr_t const & s, bool initial, bool final)
    : tchecker::graph::node_flags_t(initial, final), tchecker::graph::node_refzg_state_t(s)
{
}

/* lexical_cmp on nodes */

int lexical_cmp(tchecker::refzg::path::node_t const & n1, tchecker::refzg::path::node_t const & n2)
{
  int state_cmp = tchecker::ta::lexical_cmp(static_cast<tchecker::ta::state_t const &>(n1.state()),
                                            static_cast<tchecker::ta::state_t const &>(n2.state()));
  if (state_cmp != 0)
    return state_cmp;
  if (n1.initial() != n2.initial())
    return n1.initial() < n2.initial();
  return n1.final() < n2.final();
}

/* edge_t */

edge_t::edge_t(tchecker::refzg::const_transition_sptr_t const & t) : _t(t) {}

edge_t::edge_t(tchecker::refzg::transition_sptr_t const & t) : _t(t) {}

edge_t::edge_t(tchecker::refzg::path::edge_t const & e) : _t(e._t) {}

edge_t::edge_t(tchecker::refzg::path::edge_t && e) : _t(std::move(e._t)) {}

tchecker::refzg::path::edge_t & edge_t::operator=(tchecker::refzg::path::edge_t const & e)
{
  if (this != &e)
    _t = e._t;
  return *this;
}

tchecker::refzg::path::edge_t & edge_t::operator=(tchecker::refzg::path::edge_t && e)
{
  if (this != &e)
    _t = std::move(e._t);
  return *this;
}

/* lexical_cmp on path edges */

int lexical_cmp(tchecker::refzg::path::edge_t const & e1, tchecker::refzg::path::edge_t const & e2)
{
  return tchecker::refzg::lexical_cmp(e1.transition(), e2.transition()) < 0;
}

} // namespace path

} // namespace refzg

} // namespace tchecker
