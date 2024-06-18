/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_SYNCHRONIZATION_HH
#define TCHECKER_SYSTEM_SYNCHRONIZATION_HH

#include <tuple>
#include <unordered_set>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/system/attribute.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file synchronization.hh
 \brief Synchronization vectors
 */

namespace tchecker {

namespace system {

/*!
 \class sync_constraint_t
 \brief Synchronization constraint (process ID, event ID, strength)
 */
class sync_constraint_t {
public:
  /*!
   \brief Constructor
   \param pid : process identifier
   \param event_id : event identifier
   \param strength : synchronization strength
   */
  sync_constraint_t(tchecker::process_id_t pid, tchecker::event_id_t event_id, tchecker::sync_strength_t strength);

  /*!
   \brief Copy constructor
   \param sync : synchronization constraint
   \post this is a copy of sync
   */
  sync_constraint_t(tchecker::system::sync_constraint_t const & sync) = default;

  /*!
   \brief Move constructor
   \param sync : synchronization constraint
   \post sync has been moved to this
   */
  sync_constraint_t(tchecker::system::sync_constraint_t && sync) = default;

  /*!
   \brief Destructor
   */
  ~sync_constraint_t() = default;

  /*!
   \brief Assignment operator
   \param sync : synchronization constraint
   \post this is a copy of sync
   */
  tchecker::system::sync_constraint_t & operator=(tchecker::system::sync_constraint_t const & sync) = default;

  /*!
   \brief Move assignment operator
   \param sync : synchronization constraint
   \post sync has been moved to this
   */
  tchecker::system::sync_constraint_t & operator=(tchecker::system::sync_constraint_t && sync) = default;

  /*!
   \brief Equality predicate
   \param sync : synchronization constraint
   \return true if this and sync have same pid, same event_id, and same strength, false otherwise
  */
  bool operator==(tchecker::system::sync_constraint_t const & sync) const;

  /*!
   \brief Disequality predicate
   \param sync : synchronization constraint
   \return true if this and sync have same pid, same event_id, and same strength, false otherwise
  */
  inline bool operator!=(tchecker::system::sync_constraint_t const & sync) const { return !(*this == sync); }

  /*!
   \brief Accessor
   \return process identifier
   */
  inline tchecker::process_id_t pid() const { return _pid; }

  /*!
   \brief Accessor
   \return event identifier
   */
  inline tchecker::event_id_t event_id() const { return _event_id; }

  /*!
   \brief Accessor
   \return strength of synchronization
   */
  inline tchecker::sync_strength_t strength() const { return _strength; }

private:
  tchecker::process_id_t _pid;         /*!< Process identifier */
  tchecker::event_id_t _event_id;      /*!< Event identifier */
  tchecker::sync_strength_t _strength; /*!< Synchronization strength */
};

/*!
 \class synchronization_t
 \brief Synchronization vector
 */
class synchronization_t {
public:
  /*!
   \brief Constructor
   \param id : synchronization identifier
   \param v : synchronization constraints
   \param attributes : synchronization attributes
   \throw std::invalid_argument : if is is not a valid synchronization identifier
   */
  synchronization_t(tchecker::sync_id_t id, std::vector<tchecker::system::sync_constraint_t> const & v,
                    tchecker::system::attributes_t const & attributes = tchecker::system::attributes_t());

  /*!
   \brief Copy constructor
   \param constraints : synchronization constraints
   \post this is a copy of constraints
   */
  synchronization_t(tchecker::system::synchronization_t const & constraints) = default;

  /*!
   \brief Move constructor
   \param constraints : synchronization constraints
   \post constraints has been moved to this
   */
  synchronization_t(tchecker::system::synchronization_t && constraints) = default;

  /*!
   \brief Destructor
   */
  ~synchronization_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::system::synchronization_t & operator=(tchecker::system::synchronization_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::system::synchronization_t & operator=(tchecker::system::synchronization_t &&) = default;

  /*!
   \brief Add a synchronization constraint
   \param constr : synchronization constraints
   \pre the process in constr is not already synchronized
   \post constr has been added
   \throw std::invalid_argument : if the process in constr is already synchronized
   */
  void add_synchronization_constraint(tchecker::system::sync_constraint_t const & constr);

  /*!
   \brief Accessor
   \return Identifier
   */
  inline tchecker::sync_id_t id() const { return _id; }

  /*!
   \brief Accessor
   \return size, i.e. number of constraints
   */
  inline std::size_t size() const { return _constraints.size(); }

  /*!
   \brief Type of iterator over synchronized process-events
   \note dereference to tchecker::sync_constraint_t const
   */
  using const_iterator_t = std::vector<tchecker::system::sync_constraint_t>::const_iterator;

  /*!
   \brief Accessor
   \return range of synchronized process-events
   */
  inline tchecker::range_t<tchecker::system::synchronization_t::const_iterator_t> synchronization_constraints() const
  {
    return tchecker::make_range(_constraints.begin(), _constraints.end());
  }

  /*!
   \brief Accessor
   \return attributes
   */
  inline tchecker::system::attributes_t const & attributes() const { return _attributes; }

private:
  tchecker::sync_id_t _id;                                       /*!< Identifier */
  std::vector<tchecker::system::sync_constraint_t> _constraints; /*!< Sync constraints */
  tchecker::system::attributes_t _attributes;                    /*!< Attributes */
};

/*!
 \class synchronizations_t
 \brief Collection of synchronization vectors
 */
class synchronizations_t {
public:
  /*!
   \brief Add a synchronization
   \param v : synchronization constraints
   \param attr : synchronization attributes
   \post a synchronization built from v has been added with attributes attr
   \throw std::invalid_argument : if v already exists
   \throw std::invalid_argument : if too many synchronizations
   */
  void add_synchronization(std::vector<tchecker::system::sync_constraint_t> const & v,
                           tchecker::system::attributes_t const & attr = tchecker::system::attributes_t());

  /*!
   \brief Accessor
   \return number of synchronizations
   \note all integers in 0..synchronizations_count()-1 are valid synchronization identifiers
   */
  inline std::size_t synchronizations_count() const { return _syncs.size(); }

  /*!
   \brief Type of range of synchronizations identifiers
   */
  using synchronizations_identifiers_range_t = tchecker::integer_range_t<tchecker::sync_id_t>;

  /*!
   \brief Accessor
   \return range of synchronizations identifiers 0..synchronizations_count()-1
   */
  synchronizations_identifiers_range_t synchronizations_identifiers() const;

  /*!
   \brief Accessor
   \param id : synchronization identifier
   \return synchronization with identifier id
   \throw std::invalid_argument : if id is not a valid synchronization identifier
   */
  tchecker::system::synchronization_t const & synchronization(tchecker::sync_id_t id) const;

  /*!
   \brief Type of iterator over synchronizations
   */
  using const_iterator_t = std::vector<tchecker::system::synchronization_t>::const_iterator;

  /*!
   \brief Accessor
   \return range of synchronizations
   */
  tchecker::range_t<tchecker::system::synchronizations_t::const_iterator_t> synchronizations() const;

  /*!
   \brief Membership predicate
   \param v : vector of synchronization constraints
   \return true if this set of synchronizations already contains (a permutation of) v,
   false otherwise
  */
  bool contains(std::vector<tchecker::system::sync_constraint_t> const & v) const;

private:
  std::vector<tchecker::system::synchronization_t> _syncs; /*!< Synchronization vectors */
};

} // end of namespace system

} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_SYNCHRONIZATION_HH
