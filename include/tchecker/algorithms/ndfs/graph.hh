/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_NDFS_GRAPH_HH
#define TCHECKER_ALGORITHMS_NDFS_GRAPH_HH

/*!
 \file graph.hh
 \brief Graphs for the nested DFS algorithm
 */

namespace tchecker {

namespace algorithms {

namespace ndfs {

/*!
 \enum color_t
 \brief Node colors for the nested DFS algorithm
*/
enum color_t {
  WHITE,
  CYAN,
  BLUE,
  RED,
};

/*!
 \class node_t
 \brief Nodes for the nested DFS algorithm
*/
class node_t {
public:
  /*!
  \brief Constructor
  \post this node has color white
  */
  node_t();

  /*!
  \brief Accessor
  \returns a reference to this node color
  */
  enum tchecker::algorithms::ndfs::color_t & color();

  /*!
   \brief Accessor
   \return the color of this node
  */
  enum tchecker::algorithms::ndfs::color_t color() const;

private:
  enum tchecker::algorithms::ndfs::color_t _color; /*!< Node color */
};

} // end of namespace ndfs

} // end of namespace algorithms

} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_NDFS_GRAPH_HH
