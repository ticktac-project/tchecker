/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/algorithms/ndfs/graph.hh"

namespace tchecker {

namespace algorithms {

namespace ndfs {

node_t::node_t() : _color(tchecker::algorithms::ndfs::WHITE) {}

enum tchecker::algorithms::ndfs::color_t & node_t::color() { return _color; }

enum tchecker::algorithms::ndfs::color_t node_t::color() const { return _color; }

} // namespace ndfs

} // end of namespace algorithms

} // end of namespace tchecker
