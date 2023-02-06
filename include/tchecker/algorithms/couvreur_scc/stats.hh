/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_COUVREUR_SCC_STATS_HH
#define TCHECKER_ALGORITHMS_COUVREUR_SCC_STATS_HH

#include <map>
#include <string>

#include "tchecker/algorithms/stats.hh"

/*!
 \file stats.hh
 \brief Statistics for Couvreur's SCC algorithm
 */

namespace tchecker {

namespace algorithms {

namespace couvscc {

/*!
 \class stats_t
 \brief Statistics for Couvreur's SCC algorithm
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
   \return A reference to the number of visited transitions
   */
  unsigned long & visited_transitions();

  /*!
   \brief Accessor
   \return the number of visited transitions
   */
  unsigned long visited_transitions() const;

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
  unsigned long _visited_states;      /*!< Number of visited states */
  unsigned long _visited_transitions; /*!< Number of visited transitions */
  unsigned long _stored_states;       /*!< Number of stored states */
  bool _cycle;                        /*!< Reachability of satisfying cycle */
};

} // namespace couvscc

} // end of namespace algorithms

} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_COUVREUR_SCC_STATS_HH
