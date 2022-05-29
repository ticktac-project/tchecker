/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TCK_SIMULATE_GRAPH_HH
#define TCHECKER_TCK_SIMULATE_GRAPH_HH

/*!
 \file graph.hh
 \brief Simulation graph
*/

#include <cstdlib>
#include <map>
#include <memory>
#include <ostream>
#include <string>

#include "tchecker/graph/reachability_graph.hh"
#include "tchecker/syncprod/vedge.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/zg/zg.hh"

namespace tchecker {

namespace tck_simulate {

/*!
\class node_t
\brief Nodes os simulation graph
*/
class node_t {
public:
  /*!
   \brief Constructor
   \param s : a zone graph state
   \post this node keeps a shared pointer to s
  */
  node_t(tchecker::zg::state_sptr_t const & s);

  /*!
   \brief Constructor
   \param s : a zone graph state
   \post this node keeps a shared pointer to s
   */
  node_t(tchecker::zg::const_state_sptr_t const & s);

  /*!
  \brief Accessor
  \return shared pointer to zone graph state in this node
  */
  inline tchecker::zg::const_state_sptr_t state_ptr() const { return _state; }

  /*!
  \brief Accessor
  \return zone graph state in this node
  */
  inline tchecker::zg::state_t const & state() const { return *_state; }

private:
  tchecker::zg::const_state_sptr_t _state; /*!< State of the zone graph */
};

/*!
 \class edge_t
 \brief Edge of the simulation graph
*/
class edge_t {
public:
  /*!
   \brief Constructor
   \param t : a zone graph transition
   \post this node keeps a shared pointer on the vedge in t
  */
  edge_t(tchecker::zg::transition_t const & t);

  /*!
   \brief Accessor
   \return zone graph vedge in this edge
  */
  inline tchecker::vedge_t const & vedge() const { return *_vedge; }

  /*!
   \brief Accessor
   \return shared pointer to the zone graph vedge in this edge
  */
  inline tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t const> vedge_ptr() const { return _vedge; }

private:
  tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t const> _vedge; /*!< Tuple of edges */
};

/*!
 \class graph_t
 \brief Simulation graph over the zone graph
*/
class graph_t
    : public tchecker::graph::reachability::multigraph_t<tchecker::tck_simulate::node_t, tchecker::tck_simulate::edge_t> {
public:
  /*!
   \brief Constructor
   \param zg : zone graph
   \param block_size : number of objects allocated in a block
   \note this keeps a pointer on zg
  */
  graph_t(std::shared_ptr<tchecker::zg::zg_t> const & zg, std::size_t block_size);

  /*!
   \brief Destructor
  */
  virtual ~graph_t();

  using tchecker::graph::reachability::multigraph_t<tchecker::tck_simulate::node_t, tchecker::tck_simulate::edge_t>::attributes;

protected:
  /*!
   \brief Accessor to node attributes
   \param n : a node
   \param m : a map (key, value) of attributes
   \post attributes of node n have been added to map m
  */
  virtual void attributes(tchecker::tck_simulate::node_t const & n, std::map<std::string, std::string> & m) const;

  /*!
   \brief Accessor to edge attributes
   \param e : an edge
   \param m : a map (key, value) of attributes
   \post attributes of edge e have been added to map m
  */
  virtual void attributes(tchecker::tck_simulate::edge_t const & e, std::map<std::string, std::string> & m) const;

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
std::ostream & dot_output(std::ostream & os, tchecker::tck_simulate::graph_t const & g, std::string const & name);

} // namespace tck_simulate

} // namespace tchecker

#endif // TCHECKER_TCK_SIMULATE_GRAPH_HH
