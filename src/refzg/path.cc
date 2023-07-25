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

/* finite_path_t */

void finite_path_t::attributes(tchecker::refzg::path::node_t const & n, std::map<std::string, std::string> & m) const
{
  tchecker::ts::finite_path_t<tchecker::refzg::refzg_t, tchecker::refzg::path::node_t,
                              tchecker::refzg::path::edge_t>::attributes(n, m);
  if (n.initial())
    m["initial"] = "true";
  if (n.final())
    m["final"] = "true";
}

void finite_path_t::attributes(tchecker::refzg::path::edge_t const & e, std::map<std::string, std::string> & m) const
{
  tchecker::ts::finite_path_t<tchecker::refzg::refzg_t, tchecker::refzg::path::node_t,
                              tchecker::refzg::path::edge_t>::attributes(e, m);
}

/* node_le_t */

bool node_le_t::operator()(tchecker::refzg::path::finite_path_t::node_sptr_t const & n1,
                           tchecker::refzg::path::finite_path_t::node_sptr_t const & n2) const
{
  return tchecker::refzg::path::lexical_cmp(static_cast<tchecker::refzg::path::node_t const &>(*n1),
                                            static_cast<tchecker::refzg::path::node_t const &>(*n2)) < 0;
}

/* edge_le_t */

bool edge_le_t::operator()(tchecker::refzg::path::finite_path_t::edge_sptr_t const & e1,
                           tchecker::refzg::path::finite_path_t::edge_sptr_t const & e2) const
{
  return tchecker::refzg::path::lexical_cmp(static_cast<tchecker::refzg::path::edge_t const &>(*e1),
                                            static_cast<tchecker::refzg::path::edge_t const &>(*e2)) < 0;
}

/* dot_output */

std::ostream & dot_output(std::ostream & os, tchecker::refzg::path::finite_path_t const & path, std::string const & name)
{
  return tchecker::graph::dot_output<tchecker::refzg::path::finite_path_t, tchecker::refzg::path::node_le_t,
                                     tchecker::refzg::path::edge_le_t>(os, path, name);
}

/* compute_symbolic_run */

tchecker::refzg::path::finite_path_t * compute_symbolic_run(std::shared_ptr<tchecker::refzg::refzg_t> const & refzg,
                                                            tchecker::vloc_t const & initial_vloc,
                                                            std::vector<tchecker::const_vedge_sptr_t> const & seq)
{
  tchecker::refzg::path::finite_path_t * path = new tchecker::refzg::path::finite_path_t{refzg};

  tchecker::refzg::const_state_sptr_t s{tchecker::refzg::initial(*refzg, initial_vloc)};
  if (s.ptr() == nullptr) {
    delete path;
    throw std::invalid_argument("No initial state with given tuple of locations");
  }

  path->add_first_node(s);
  path->first()->initial(true);

  for (tchecker::const_vedge_sptr_t const & vedge_ptr : seq) {
    s = path->last()->state_ptr();
    auto && [nexts, nextt] = tchecker::refzg::next(*refzg, s, *vedge_ptr);
    if (nexts.ptr() == nullptr || nextt.ptr() == nullptr) {
      delete path;
      throw std::invalid_argument("Sequence is not feasible from given initial locations");
    }
    path->extend_back(nextt, nexts);
  }

  return path;
}

} // namespace path

} // namespace refzg

} // namespace tchecker
