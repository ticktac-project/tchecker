/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALGORITHMS_STATS_HH
#define TCHECKER_ALGORITHMS_STATS_HH

#include <chrono>
#include <map>
#include <string>

/*!
 \file stats.hh
 \brief Statistics for algorithms
 */

namespace tchecker {

namespace algorithms {

/*!
 \class stats_t
 \brief Statistics for algorithms
 */
class stats_t {
public:
  /*!
   \brief Set starting time
  */
  void set_start_time();

  /*!
  \brief Accessor
  \return starting time
  */
  std::chrono::time_point<std::chrono::steady_clock> start_time() const;

  /*!
   \brief Set ending time
  */
  void set_end_time();

  /*!
  \brief Accessor
  \return ending time
  */
  std::chrono::time_point<std::chrono::steady_clock> end_time() const;

  /*!
   \brief Accessor
   \return running time in seconds (difference between ending time and starting time)
  */
  double running_time() const;

  /*!
   \brief Extract statistics as attributes (key, value)
   \param m : attributes map
   \post Starting time, ending time and running time have been added to m
  */
  void attributes(std::map<std::string, std::string> & m) const;

private:
  std::chrono::time_point<std::chrono::steady_clock> _start_time; /*!< Start time */
  std::chrono::time_point<std::chrono::steady_clock> _end_time;   /*!< End time */
};

} // end of namespace algorithms

} // end of namespace tchecker

#endif // TCHECKER_ALGORITHMS_STATS_HH