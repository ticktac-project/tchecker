/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_SHARING_HH
#define TCHECKER_TS_SHARING_HH

/*!
 \file sharing.hh
 \brief Interfaces for state and transition sharing
 */

namespace tchecker {

namespace ts {

/*!
 \brief Type of sharing
*/
enum sharing_type_t {
  NO_SHARING, /*!< No sharing */
  SHARING,    /*!< State/transition components sharing */
};

/*!
 \class sharing_t
 \brief State and transition coponents sharing
 \tparam STATE : type of state
 \tparam TRANSITION : type of transition
 */
template <class STATE, class TRANSITION> class sharing_t {
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
   \brief Destructor
  */
  virtual ~sharing_t() = default;

  /*!
   \brief Share state components
   \param s : a state
   \post internal components in s have been shared
  */
  virtual void share(state_t & s) = 0;

  /*!
   \brief Share transition components
   \param t : a transition
   \post internal components in t have been shared
  */
  virtual void share(transition_t & t) = 0;
};

} // end of namespace ts

} // end of namespace tchecker

#endif // TCHECKER_TS_SHARING_HH
