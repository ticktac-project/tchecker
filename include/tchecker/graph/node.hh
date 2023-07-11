/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GRAPH_NODE_HH
#define TCHECKER_GRAPH_NODE_HH

#include <map>
#include <string>

#include "tchecker/refzg/state.hh"
#include "tchecker/zg/state.hh"

namespace tchecker {

namespace graph {

/*!
 \class node_flags_t
 \brief Node with initial/final flags
*/
class node_flags_t {
private:
  /*!
  \brief Flags on nodes
  */
  using flags_t = unsigned char;

  flags_t const NODE_INITIAL = 1; /*!< Initial node */
  flags_t const NODE_FINAL = 2;   /*!< Final node */

public:
  /*!
   \brief Constructor
   \param initial : initial node flag
   \param final : final node flag
   \post thos node has initial/final node flags as specified
   */
  node_flags_t(bool initial = false, bool final = false);

  /*!
  \brief Accessor
  \return true if this node is initial, false otherwise
  */
  inline bool initial() const { return (_flags & tchecker::graph::node_flags_t::NODE_INITIAL) != 0; }

  /*!
  \brief Set initial status
  \param status : initial status
  \post this node is marked 'initial' if status is true, 'non initial' otherwise
  */
  void initial(bool status);

  /*!
  \brief Accessor
  \return true if this node is final, false otherwise
  */
  inline bool final() const { return (_flags & tchecker::graph::node_flags_t::NODE_FINAL) != 0; }

  /*!
  \brief Set final status
  \param status : final status
  \post this node is marked 'final' if status is true, 'non final' otherwise
  */
  void final(bool status);

private:
  tchecker::graph::node_flags_t::flags_t _flags; /*!< Node flags */
};

/*!
 \brief Lexical ordering on nodes
 \param n1 : a node
 \param n2 : a node
 \return 0 if n1 and n2 are equal, a negative value if n1 is smaller than n2 w.r.t. initial/final flags, a positive value
 otherwise
 */
int lexical_cmp(tchecker::graph::node_flags_t const & n1, tchecker::graph::node_flags_t const & n2);

/*!
 \brief Accessor to node attributes
 \param n : a node
 \param m : a map (key, value) of attributes
 \post attributes of node n have been added to map m
*/
void attributes(tchecker::graph::node_flags_t const & n, std::map<std::string, std::string> & m);

/*!
 \struct node_zg_state_t
 \brief Graph node that points to a state of a zone graph
 */
struct node_zg_state_t {
  /*!
   \brief Constructor
   \param s : a zone graph state
   \post this node keeps a shared pointer to s
  */
  node_zg_state_t(tchecker::zg::state_sptr_t const & s);

  /*!
   \brief Constructor
   \param s : a zone graph state
   \post this node keeps a shared pointer to s
   */
  node_zg_state_t(tchecker::zg::const_state_sptr_t const & s);

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
 \struct node_refzg_state_t
 \brief Graph node that points to a state of a zone-graph with reference clocks
 */
struct node_refzg_state_t {
  /*!
   \brief Constructor
   \param s : a zone graph state
   \post this node keeps a shared pointer to s
  */
  node_refzg_state_t(tchecker::refzg::state_sptr_t const & s);

  /*!
   \brief Constructor
   \param s : a zone graph state
   \post this node keeps a shared pointer to s
   */
  node_refzg_state_t(tchecker::refzg::const_state_sptr_t const & s);

  /*!
  \brief Accessor
  \return shared pointer to zone graph state in this node
  */
  inline tchecker::refzg::const_state_sptr_t state_ptr() const { return _state; }

  /*!
  \brief Accessor
  \return zone graph state in this node
  */
  inline tchecker::refzg::state_t const & state() const { return *_state; }

private:
  tchecker::refzg::const_state_sptr_t _state; /*!< State of the zone graph with reference clocks */
};

} // namespace graph

} // namespace tchecker

#endif // TCHECKER_GRAPH_NODE_HH