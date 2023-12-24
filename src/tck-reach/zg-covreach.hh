/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_COVREACH_ALGORITHM_HH
#define TCHECKER_ZG_COVREACH_ALGORITHM_HH

/*!
 \file zg-covreach.hh
 \brief Covering reachability algorithm over the zone graph with zone inclusion
*/

#include "tchecker/algorithms/covreach/algorithm.hh"
#include "tchecker/graph/edge.hh"
#include "tchecker/graph/node.hh"
#include "tchecker/graph/subsumption_graph.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/waiting/waiting.hh"
#include "tchecker/zg/path.hh"
#include "tchecker/zg/state.hh"
#include "tchecker/zg/transition.hh"
#include "tchecker/zg/zg.hh"

namespace tchecker {

namespace tck_reach {

namespace zg_covreach {

/*!
 \class node_t
 \brief Node of the covering reachability graph of a zone graph
 */
class node_t : public tchecker::waiting::element_t,
               public tchecker::graph::node_flags_t,
               public tchecker::graph::node_zg_state_t {
public:
  /*!
   \brief Constructor
   \param s : a zone graph state
   \param initial : initial node flag
   \param final : final node flag
   \post this node keeps a shared pointer to s, and has initial/final node flags as specified
   */
  node_t(tchecker::zg::state_sptr_t const & s, bool initial = false, bool final = false);

  /*!
   \brief Constructor
   \param s : a zone graph state
   \param initial : initial node flag
   \param final : final node flag
   \post this node keeps a shared pointer to s, and has initial/final node flags as specified
   */
  node_t(tchecker::zg::const_state_sptr_t const & s, bool initial = false, bool final = false);
};

/*!
\class node_hash_t
\brief Hash functor for nodes
*/
class node_hash_t {
public:
  /*!
  \brief Hash function
  \param n : a node
  \return hash value for n based on the discrete part of n (i.e. the tuple of
  locations and integer variable valuations) since we need to cover nodes with
  same discrete part
  */
  std::size_t operator()(tchecker::tck_reach::zg_covreach::node_t const & n) const;
};

/*!
\class node_le_t
\brief Covering predicate for nodes
*/
class node_le_t {
public:
  /*!
  \brief Covering predicate for nodes
  \param n1 : a node
  \param n2 : a node
  \return true if n1 and n2 have same discrete part and the zone of n1 is
  included in the zone of n2, false otherwise
  */
  bool operator()(tchecker::tck_reach::zg_covreach::node_t const & n1,
                  tchecker::tck_reach::zg_covreach::node_t const & n2) const;
};

/*!
 \class edge_t
 \brief Edge of the covering reachability graph of a zone graph
*/
class edge_t : public tchecker::graph::edge_vedge_t {
public:
  /*!
   \brief Constructor
   \param t : a zone graph transition
   \post this node keeps a shared pointer on the vedge in t
  */
  edge_t(tchecker::zg::transition_t const & t);
};

/*!
 \class graph_t
 \brief Reachability graph over the zone graph
*/
class graph_t : public tchecker::graph::subsumption::graph_t<
                    tchecker::tck_reach::zg_covreach::node_t, tchecker::tck_reach::zg_covreach::edge_t,
                    tchecker::tck_reach::zg_covreach::node_hash_t, tchecker::tck_reach::zg_covreach::node_le_t> {
public:
  /*!
   \brief Constructor
   \param zg : zone graph
   \param block_size : number of objects allocated in a block
   \param table_size : size of hash table
   \note this keeps a pointer on zg
  */
  graph_t(std::shared_ptr<tchecker::zg::zg_t> const & zg, std::size_t block_size, std::size_t table_size);

  /*!
   \brief Destructor
  */
  virtual ~graph_t();

  /*!
   \brief Accessor
   \return pointer to internal zone graph
  */
  inline std::shared_ptr<tchecker::zg::zg_t> zg_ptr() { return _zg; }

  /*!
   \brief Accessor
   \return internal zone graph
  */
  inline tchecker::zg::zg_t const & zg() const { return *_zg; }

  using tchecker::graph::subsumption::graph_t<
      tchecker::tck_reach::zg_covreach::node_t, tchecker::tck_reach::zg_covreach::edge_t,
      tchecker::tck_reach::zg_covreach::node_hash_t, tchecker::tck_reach::zg_covreach::node_le_t>::attributes;

protected:
  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
   \post attributes of node n have been added to map m
  */
  virtual void attributes(tchecker::tck_reach::zg_covreach::node_t const & n, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
   \post attributes of edge e have been added to map m
  */
  virtual void attributes(tchecker::tck_reach::zg_covreach::edge_t const & e, std::map<std::string, std::string> & m) const;

private:
  std::shared_ptr<tchecker::zg::zg_t> _zg; /*!< Zone graph */
};

/*!
 \brief Graph output
 \param os : output stream
 \param g : graph
 \param name : graph name
 \post graph g with name has been output to os
*/
std::ostream & dot_output(std::ostream & os, tchecker::tck_reach::zg_covreach::graph_t const & g, std::string const & name);

namespace cex {

/*!
 \brief Type of symbolic counter-example
*/
using symbolic_cex_t = tchecker::zg::path::symbolic::finite_path_t;

/*!
 \brief Compute a symbolic counter-example from a covering reachability graph of a zone graph
 \param g : reachability graph on a zone graph
 \return a finite path from an initial node to a final node in g if any,
 nullptr otherwise
 \note the returned pointer shall be deleted
*/
tchecker::tck_reach::zg_covreach::cex::symbolic_cex_t *
symbolic_counter_example(tchecker::tck_reach::zg_covreach::graph_t const & g);

/*!
 \brief Symbolic counter-example output
 \param os : output stream
 \param cex : counter example
 \param name : counter example name
 \post cex has been output to os
 \return os after output
 */
std::ostream & dot_output(std::ostream & os, tchecker::tck_reach::zg_covreach::cex::symbolic_cex_t const & cex,
                          std::string const & name);

/*!
 \brief Type of concrete counter-example
*/
using concrete_cex_t = tchecker::zg::path::concrete::finite_path_t;

/*!
 \brief Compute a concrete counter-example from a covering reachability graph of a zone graph
 \param g : reachability graph on a zone graph
 \return a finite path from an initial node to a final node in g with concrete clock valuations if any,
 nullptr otherwise
 \note the returned pointer shall be deleted
*/
tchecker::tck_reach::zg_covreach::cex::concrete_cex_t *
concrete_counter_example(tchecker::tck_reach::zg_covreach::graph_t const & g);

/*!
 \brief Concrete counter-example output
 \param os : output stream
 \param cex : counter example
 \param name : counter example name
 \post cex has been output to os
 \return os after output
 */
std::ostream & dot_output(std::ostream & os, tchecker::tck_reach::zg_covreach::cex::concrete_cex_t const & cex,
                          std::string const & name);

} // namespace cex

/*!
 \class algorithm_t
 \brief Covering reachability algorithm over the zone graph
*/
class algorithm_t
    : public tchecker::algorithms::covreach::algorithm_t<tchecker::zg::zg_t, tchecker::tck_reach::zg_covreach::graph_t> {
public:
  using tchecker::algorithms::covreach::algorithm_t<tchecker::zg::zg_t, tchecker::tck_reach::zg_covreach::graph_t>::algorithm_t;
};

/*!
 \brief Run covering reachability algorithm on the zone graph of a system
 \param sysdecl : system declaration
 \param labels : comma-separated string of labels
 \param search_order : search order
 \param covering : covering policy
 \param block_size : number of elements allocated in one block
 \param table_size : size of hash tables
 \pre labels must appear as node attributes in sysdecl
 search_order must be either "dfs" or "bfs"
 \return statistics on the run and the covering reachability graph
 */
std::tuple<tchecker::algorithms::covreach::stats_t, std::shared_ptr<tchecker::tck_reach::zg_covreach::graph_t>>
run(tchecker::parsing::system_declaration_t const & sysdecl, std::string const & labels = "",
    std::string const & search_order = "bfs",
    tchecker::algorithms::covreach::covering_t covering = tchecker::algorithms::covreach::COVERING_FULL,
    std::size_t block_size = 10000, std::size_t table_size = 65536);

} // end of namespace zg_covreach

} // end of namespace tck_reach

} // end of namespace tchecker

#endif // TCHECKER_ZG_COVREACH_ALGORITHM_HH
