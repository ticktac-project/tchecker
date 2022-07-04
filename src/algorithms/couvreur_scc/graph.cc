/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/algorithms/couvreur_scc/graph.hh"

namespace tchecker {

namespace algorithms {

namespace couvscc {

node_t::node_t() : _dfsnum(0), _current(false) {}

unsigned int & node_t::dfsnum() { return _dfsnum; }

unsigned int node_t::dfsnum() const { return _dfsnum; }

bool & node_t::current() { return _current; }

bool node_t::current() const { return _current; }

} // namespace couvscc

} // end of namespace algorithms

} // end of namespace tchecker
