/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_BUILDER_HH
#define TCHECKER_TS_BUILDER_HH

/*!
 \file builder.hh
 \brief Transition systems interfaces for state/transition builder
 */

#include <map>
#include <string>
#include <vector>

#include "tchecker/basictypes.hh"

namespace tchecker {

namespace ts {

/*!
 \class builder_t
 \brief Transition system interface, state/transition builder
 \tparam STATE : type of state
 \tparam TRANSITION : type of transition
*/
template <class STATE, class TRANSITION> class builder_t {
public:
  /*!
   \brief Type of state
   */
  using state_t = STATE;

  /*!
   \brief Type of transition
   */
  using transition_t = TRANSITION;

  /*!
  \brief Type of tuples (status, state, transition)
  */
  using sst_t = std::tuple<tchecker::state_status_t, state_t, transition_t>;

  /*!
   \brief Destructor
   */
  virtual ~builder_t() = default;

  /*!
   \brief State/transition building from attributes
   \param attributes : a map of attributes
   \param v : container
   \param mask : mask on states
   \post all tuples (status, s, t) where s and t have been initialized from attrbiutes,
   and status matches mask (i.e. status & mask != 0) have been pushed to v
  */
  virtual void build(std::map<std::string, std::string> const & attributes, std::vector<sst_t> & v,
                     tchecker::state_status_t mask) = 0;
};

} // end of namespace ts

} // end of namespace tchecker

#endif // TCHECKER_TS_BUILDER_HH
