/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/zg/path.hh"

namespace tchecker {

namespace zg {

namespace path {

/* node_t */

node_t::node_t(tchecker::zg::state_sptr_t const & s, bool initial, bool final)
    : tchecker::graph::node_flags_t(initial, final), tchecker::graph::node_zg_state_t(s)
{
}

node_t::node_t(tchecker::zg::const_state_sptr_t const & s, bool initial, bool final)
    : tchecker::graph::node_flags_t(initial, final), tchecker::graph::node_zg_state_t(s)
{
}

/* lexical_cmp on nodes */

int lexical_cmp(tchecker::zg::path::node_t const & n1, tchecker::zg::path::node_t const & n2)
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

edge_t::edge_t(tchecker::zg::const_transition_sptr_t const & t) : _t(t) {}

edge_t::edge_t(tchecker::zg::transition_sptr_t const & t) : _t(t) {}

edge_t::edge_t(tchecker::zg::path::edge_t const & e) : _t(e._t) {}

edge_t::edge_t(tchecker::zg::path::edge_t && e) : _t(std::move(e._t)) {}

tchecker::zg::path::edge_t & edge_t::operator=(tchecker::zg::path::edge_t const & e)
{
  if (this != &e)
    _t = e._t;
  return *this;
}

tchecker::zg::path::edge_t & edge_t::operator=(tchecker::zg::path::edge_t && e)
{
  if (this != &e)
    _t = std::move(e._t);
  return *this;
}

/* lexical_cmp on path edges */

int lexical_cmp(tchecker::zg::path::edge_t const & e1, tchecker::zg::path::edge_t const & e2)
{
  return tchecker::zg::lexical_cmp(e1.transition(), e2.transition()) < 0;
}

/* finite_path_t */

void finite_path_t::attributes(tchecker::zg::path::node_t const & n, std::map<std::string, std::string> & m) const
{
  tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::node_t, tchecker::zg::path::edge_t>::attributes(
      n.state_ptr(), m);
  if (n.initial())
    m["initial"] = "true";
  if (n.final())
    m["final"] = "true";
}

void finite_path_t::attributes(tchecker::zg::path::edge_t const & e, std::map<std::string, std::string> & m) const
{
  tchecker::ts::finite_path_t<tchecker::zg::zg_t, tchecker::zg::path::node_t, tchecker::zg::path::edge_t>::attributes(
      e.transition_ptr(), m);
}

/* node_le_t */

bool node_le_t::operator()(tchecker::zg::path::finite_path_t::node_sptr_t const & n1,
                           tchecker::zg::path::finite_path_t::node_sptr_t const & n2) const
{
  return tchecker::zg::path::lexical_cmp(static_cast<tchecker::zg::path::node_t const &>(*n1),
                                         static_cast<tchecker::zg::path::node_t const &>(*n2)) < 0;
}

/* edge_le_t */

bool edge_le_t::operator()(tchecker::zg::path::finite_path_t::edge_sptr_t const & e1,
                           tchecker::zg::path::finite_path_t::edge_sptr_t const & e2) const
{
  return tchecker::zg::path::lexical_cmp(static_cast<tchecker::zg::path::edge_t const &>(*e1),
                                         static_cast<tchecker::zg::path::edge_t const &>(*e2)) < 0;
}

/* dot_output */

std::ostream & dot_output(std::ostream & os, tchecker::zg::path::finite_path_t const & path, std::string const & name)
{
  return tchecker::graph::dot_output<tchecker::zg::path::finite_path_t, tchecker::zg::path::node_le_t,
                                     tchecker::zg::path::edge_le_t>(os, path, name);
}

} // namespace path

} // namespace zg

} // namespace tchecker
