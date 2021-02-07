/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TS_TRANSITION_HH
#define TCHECKER_TS_TRANSITION_HH

#include <cstdlib>

/*!
 \file transition.hh
 \brief Basic transition for transition systems
 */

namespace tchecker {

namespace ts {

/*!
 \class transition_t
 \brief Empty transition for transition systems
 */
class transition_t {
};

/*!
 \brief Equality check
 \param t1 : transition
 \param t2 : transition
 \return true
 */
inline bool operator==(tchecker::ts::transition_t const & t1, tchecker::ts::transition_t const & t2) { return true; }

/*!
 \brief Disequality check
 \param t1 : transition
 \param t2 : transition
 \return false
 */
inline bool operator!=(tchecker::ts::transition_t const & t1, tchecker::ts::transition_t const & t2) { return !(t1 == t2); }

/*!
 \brief Hash
 \param t : transition
 \return hash value for transtion t
 */
inline std::size_t hash_value(tchecker::ts::transition_t const & t) { return 0; }

/*!
 \brief Lexical ordering on transitions of a transition system
 \param t1 : first transition
 \param t2 : second transition
 \return 0 (all transitions are equal)
 */
constexpr inline int lexical_cmp(tchecker::ts::transition_t const & t1, tchecker::ts::transition_t const & t2) { return 0; }

} // end of namespace ts

} // end of namespace tchecker

#endif // TCHECKER_TS_TRANSITION_HH
