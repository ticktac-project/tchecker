/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_INSPECTOR_HH
#define TCHECKER_TS_INSPECTOR_HH

#include <map>
#include <string>

#include <boost/dynamic_bitset.hpp>

/*!
 \file inspector.hh
 \brief Interfaces for transition system inspection
 */

namespace tchecker {

namespace ts {

/*!
 \class inspector_t
 \brief Access to informations on states and transitions
 \tparam CONST_STATE : type of const state
 \tparam CONST_TRANSITION : type of const transition
*/
template <class CONST_STATE, class CONST_TRANSITION> class inspector_t {
public:
  /*!
  \brief Type of const state
  */
  using const_state_t = CONST_STATE;

  /*!
   \brief Type of const transition
  */
  using const_transition_t = CONST_TRANSITION;

  /*!
   \brief Destructor
  */
  virtual ~inspector_t() = default;

  /*!
   \brief Computes the set of labels of a state
   \param s : a state
   \return the set of labels on state s
   */
  virtual boost::dynamic_bitset<> labels(const_state_t const & s) const = 0;

  /*!
   \brief Accessor to state attributes as strings
   \param s : a state
   \param m : a map of string pairs (key, value)
   \post attributes of state s have been added to map m
   */
  virtual void attributes(const_state_t const & s, std::map<std::string, std::string> & m) const = 0;

  /*!
   \brief Accessor to transition attributes as strings
   \param t : a transition
   \param m : a map of string pairs (key, value)
   \post attributes of transition t have been added to map m
   */
  virtual void attributes(const_transition_t const & t, std::map<std::string, std::string> & m) const = 0;

  /*!
   \brief Checks if a state is a valid final state
   \param s : a state
   \return true if a run ending in s is a valid run, false otherwise
  */
  virtual bool is_valid_final(const_state_t const & s) const = 0;

  /*!
   \brief Checks if a state is initial
   \param s : a state
   \return true if s is an initial state, false otherwise
  */
  virtual bool is_initial(const_state_t const & s) const = 0;
};

} // end of namespace ts

} // end of namespace tchecker

#endif // TCHECKER_TS_INSPECTOR_HH
