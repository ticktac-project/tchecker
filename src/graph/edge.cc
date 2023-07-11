/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/graph/edge.hh"

namespace tchecker {

namespace graph {

edge_vedge_t::edge_vedge_t(tchecker::vedge_sptr_t const & vedge) : _vedge(vedge) {}

edge_vedge_t::edge_vedge_t(tchecker::const_vedge_sptr_t const & vedge) : _vedge(vedge) {}

} // namespace graph

} // namespace tchecker