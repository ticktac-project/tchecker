/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COUVREUR_SCC_GRAPH_HH
#define TCHECKER_ALGORITHMS_COUVREUR_SCC_GRAPH_HH

#include "tchecker/graph/node.hh"

/*!
 \file graph.hh
 \brief Graphs for Couvreur's SCC-decomposition-based liveness algorithm
 */

namespace tchecker {

namespace algorithms {

namespace couvscc {

/*!
 \class node_t
 \brief Nodes for Couvreur's SCC algorithm
*/
class node_t : public tchecker::graph::node_flags_t {
public:
  /*!
  \brief Constructor
  \post this node has DFS number 0, current flag false, and initial and final
  flags false
  */
  node_t();

  /*!
   \brief Accessor
   \return a reference to the DFS number of this node
  */
  unsigned int & dfsnum();

  /*!
   \brief Accessor
   \return the DFS number of this node
  */
  unsigned int dfsnum() const;

  /*!
   \brief Accessor
   \return a reference to the current flag of this node
   */
  bool & current();

  /*!
   \brief Accessor
   \return the current flag of this node
  */
  bool current() const;

private:
  unsigned int _dfsnum; /*!< DFS number */
  bool _current;        /*!< Current flag */
};

} // namespace couvscc

} // end of namespace algorithms

} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COUVREUR_SCC_GRAPH_HH
