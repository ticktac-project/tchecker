/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COVREACH_STATS_HH
#define TCHECKER_ALGORITHMS_COVREACH_STATS_HH

#include <map>
#include <string>

#include "tchecker/algorithms/stats.hh"

/*!
 \file stats.hh
 \brief Statistics for covering reachability algorithm
 */

namespace tchecker {

namespace algorithms {

namespace covreach {

/*!
 \class stats_t
 \brief Statistics for covering reachability algorithm
 */
class stats_t : public tchecker::algorithms::stats_t {
public:
  /*!
   \brief Constructor
   */
  stats_t();

  /*!
   \brief Accessor
   \return A reference to the number of visited states
  */
  unsigned long & visited_states();

  /*!
   \brief Accessor
   \return the number of visited states
   */
  unsigned long visited_states() const;

  /*!
   \brief Accessor
   \return A reference to the numer of covered states
   */
  unsigned long & covered_states();

  /*!
   \brief Accessor
   \return the numer of covered states
   */
  unsigned long covered_states() const;

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
   \return A reference to the reachable state flag
  */
  bool & reachable();

  /*!
   \brief Accessor
   \return true if a satisfying state is reachable, false otherwise
  */
  bool reachable() const;

  /*!
   \brief Extract statistics as attributes (key, value)
   \param m : attributes map
   \post every statistics has been added to m
  */
  void attributes(std::map<std::string, std::string> & m) const;

private:
  unsigned long _visited_states; /*!< Number of visited states */
  unsigned long _covered_states; /*!< Number of covered states */
  unsigned long _stored_states;  /*!< Number of stored states */
  bool _reachable;               /*!< Reachability of satisfying state */
};

} // end of namespace covreach

} // end of namespace algorithms

} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COVREACH_STATS_HH
