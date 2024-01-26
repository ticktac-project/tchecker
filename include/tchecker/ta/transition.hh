/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_TRANSITION_HH
#define TCHECKER_TA_TRANSITION_HH

#include "tchecker/syncprod/transition.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/utils/shared_objects.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file transition.hh
 \brief Transitions for timed automata
 */

namespace tchecker {

namespace ta {

/*!
 \class transition_t
 \brief Transition for timed automata
 */
class transition_t : public tchecker::syncprod::transition_t {
public:
  /*!
   \brief Constructor
   \param vedge : tuple of edges
   \pre vedge must not point to nullptr (checked by assertion)
   */
  explicit transition_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge);

  /*!
   \brief Partial copy constructor
   \param t : a transition
   \param vedge : tuple of edges
   \pre vedge must not point to nullptr (checked by assertion)
   \note the transition is copied from t, except the tuple of edges which is initialized from vedge
   */
  transition_t(tchecker::ta::transition_t const & t, tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t> const & vedge);

  /*!
   \brief Copy constructor (deleted)
   */
  transition_t(tchecker::ta::transition_t const &) = delete;

  /*!
   \brief Move constructor
   */
  transition_t(tchecker::ta::transition_t &&) = default;

  /*!
   \brief Destructor
   */
  ~transition_t() = default;

  /*!
   \brief Assignment operator (deleted)
   */
  tchecker::ta::transition_t & operator=(tchecker::ta::transition_t const &) = delete;

  /*!
   \brief Move-assignment operator
   */
  tchecker::ta::transition_t & operator=(tchecker::ta::transition_t &&) = default;

  // Container accessors

  /*!
   \brief Accessor
   \return this transition's source invariant container
   */
  inline tchecker::clock_constraint_container_t & src_invariant_container() { return _src_invariant; }

  /*!
   \brief Accessor
   \return this transition's source invariant container
   */
  inline tchecker::clock_constraint_container_t const & src_invariant_container() const { return _src_invariant; }

  /*!
   \brief Accessor
   \return this transition's guard container
   */
  inline tchecker::clock_constraint_container_t & guard_container() { return _guard; }

  /*!
   \brief Accessor
   \return this transition's guard container
   */
  inline tchecker::clock_constraint_container_t const & guard_container() const { return _guard; }

  /*!
   \brief Accessor
   \return this transition's reset container
   */
  inline tchecker::clock_reset_container_t & reset_container() { return _reset; }

  /*!
   \brief Accessor
   \return this transition's reset container
   */
  inline tchecker::clock_reset_container_t const & reset_container() const { return _reset; }

  /*!
   \brief Accessor
   \return this transition's target invariant container
   */
  inline tchecker::clock_constraint_container_t & tgt_invariant_container() { return _tgt_invariant; }

  /*!
   \brief Accessor
   \return this transition's target invariant container
   */
  inline tchecker::clock_constraint_container_t const & tgt_invariant_container() const { return _tgt_invariant; }

  // Range accessors

  /*!
   \brief Accessor
   \return this transitions's source invariant
   */
  tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> src_invariant() const;

  /*!
   \brief Accessor
   \return this transition's guard
   */
  tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> guard() const;

  /*!
   \brief Accessor
   \return this transition's reset
   */
  tchecker::range_t<tchecker::clock_reset_container_const_iterator_t> reset() const;

  /*!
   \brief Accessor
   \return this transition's target invariant
   */
  tchecker::range_t<tchecker::clock_constraint_container_const_iterator_t> tgt_invariant() const;

protected:
  tchecker::clock_constraint_container_t _src_invariant; /*!< Source invariant */
  tchecker::clock_constraint_container_t _guard;         /*!< Guard */
  tchecker::clock_reset_container_t _reset;              /*!< Reset */
  tchecker::clock_constraint_container_t _tgt_invariant; /*!< Target invariant */
};

/*!
 \brief Equality check
 \param t1 : transition
 \param t2 : transition
 \return true if t1 and t2 have same tuple of edges, same source invariant, same guard, same resets and same target invariant,
 false otherwise
 */
bool operator==(tchecker::ta::transition_t const & t1, tchecker::ta::transition_t const & t2);

/*!
 \brief Disequality check
 \param t1 : transition
 \param t2 : transition
 \return true if t1 and t2 have different tuples of edges or differene source invariants, or different guard or difference
 resets or different taget invariant, false otherwise
 */
inline bool operator!=(tchecker::ta::transition_t const & t1, tchecker::ta::transition_t const & t2) { return !(t1 == t2); }

/*!
 \brief Equality check for shared transitions
 \param t1 : transition
 \param t2 : transition
 \return true if t1 and t2 have same tuple of edges, same source invariant, same guard, same resets and same target invariant,
 false otherwise
 \note note this should only be used on transitions that have shared internal components: this
 function checks pointer equality (not values equality)
 */
bool shared_equal_to(tchecker::ta::transition_t const & t1, tchecker::ta::transition_t const & t2);

/*!
 \brief Hash
 \param t : transition
 \return Hash value for transition t
 */
std::size_t hash_value(tchecker::ta::transition_t const & t);

/*!
 \brief Hash for shared transitions
 \param t : transition
 \return Hash value for transition t
 \note note this should only be used on transitions that have shared internal components: this function
 hashes the pointers (not the values)
 */
std::size_t shared_hash_value(tchecker::ta::transition_t const & t);

/*!
 \brief Lexical ordering on transitions of timed automata
 \param t1 : first transition
 \param t2 : second transition
 \return 0 if t1 and t2 are equal, a negative value if t1 is smaller than t2 w.r.t. lexical ordering on tuple of edges, source
 invariant, guard, reset, target invariant, a positive value otherwise
 */
int lexical_cmp(tchecker::ta::transition_t const & t1, tchecker::ta::transition_t const & t2);

/*!
 \brief Type of shared transition
 */
using shared_transition_t = tchecker::make_shared_t<tchecker::ta::transition_t>;

/*!
\brief Type of pointer to shared transition
*/
using transition_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::ta::shared_transition_t>;

/*!
\brief Type of pointer to shared const transition
*/
using const_transition_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::ta::shared_transition_t const>;

} // end of namespace ta

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class tchecker::ta::transition_t
 */
template <> class allocation_size_t<tchecker::ta::transition_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::ta::transition_t
   \return allocation size for objects of class tchecker::ta::transition_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... /*args*/)
  {
    return sizeof(tchecker::ta::transition_t);
  }
};

} // end of namespace tchecker

#endif // TCHECKER_TA_TRANSITION_HH
