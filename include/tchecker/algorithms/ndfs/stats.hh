/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_NDFS_STATS_HH
#define TCHECKER_ALGORITHMS_NDFS_STATS_HH

#include <map>
#include <string>

#include "tchecker/algorithms/stats.hh"

/*!
 \file stats.hh
 \brief Statistics for nested DFS algorithm
 */

namespace tchecker {

namespace algorithms {

namespace ndfs {

/*!
 \class stats_t
 \brief Statistics for nested DFS algorithm
 */
class stats_t : public tchecker::algorithms::stats_t {
public:
  /*!
   \brief Constructor
   */
  stats_t();

  /*!
   \brief Accessor
   \return A reference to the number of visited states on the blue DFS
   */
  unsigned long & visited_states_blue();

  /*!
   \brief Accessor
   \return the number of visited states on the blue DFS
   */
  unsigned long visited_states_blue() const;

  /*!
   \brief Accessor
   \return A reference to the number of visited states on the red DFS
   */
  unsigned long & visited_states_red();

  /*!
   \brief Accessor
   \return the number of visited states on the red DFS
   */
  unsigned long visited_states_red() const;

  /*!
   \brief Accessor
   \return the number of visited states (both blue and red DFS)
   */
  unsigned long visited_states() const;

  /*!
   \brief Accessor
   \return A reference to the number of stored states
   */
  unsigned long & stored_states();

  /*!
   \brief Accessor
   \return The number of stored states
  */
  unsigned long stored_states() const;

  /*!
   \brief Accessor
   \return A reference to the accepting cycle flag
  */
  bool & cycle();

  /*!
   \brief Accessor
   \return true if an accepting cycle is reachable, false otherwise
  */
  bool cycle() const;

  /*!
   \brief Extract statistics as attributes (key, value)
   \param m : attributes map
   \post every statistics has been added to m
  */
  void attributes(std::map<std::string, std::string> & m) const;

private:
  unsigned long _visited_states_blue; /*!< Number of visited states on the blue DFS */
  unsigned long _visited_states_red;  /*!< Number of visited states on the red DFS */
  unsigned long _stored_states;       /*!< Number of stored states */
  bool _cycle;                        /*!< Reachability of satisfying cycle */
};

} // end of namespace ndfs

} // end of namespace algorithms

} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_NDFS_STATS_HH
