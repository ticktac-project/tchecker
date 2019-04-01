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
#include "tchecker/utils/iterator.hh"

/*!
 \file synchronization.hh
 \brief Synchronization vectors
 */

namespace tchecker {
  
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
    sync_constraint_t(tchecker::sync_constraint_t const & sync) = default;
    
    /*!
     \brief Move constructor
     \param sync : synchronization constraint
     \post sync has been moved to this
     */
    sync_constraint_t(tchecker::sync_constraint_t && sync) = default;
    
    /*!
     \brief Destructor
     */
    ~sync_constraint_t() = default;
    
    /*!
     \brief Assignment operator
     \param sync : synchronization constraint
     \post this is a copy of sync
     */
    tchecker::sync_constraint_t & operator= (tchecker::sync_constraint_t const & sync) = default;
    
    /*!
     \brief Move assignment operator
     \param sync : synchronization constraint
     \post sync has been moved to this
     */
    tchecker::sync_constraint_t & operator= (tchecker::sync_constraint_t && sync) = default;
    
    /*!
     \brief Accessor
     \return process identifier
     */
    inline tchecker::process_id_t pid() const
    {
      return _pid;
    }
    
    
    /*!
     \brief Accessor
     \return event identifier
     */
    inline tchecker::event_id_t event_id() const
    {
      return _event_id;
    }
    
    
    /*!
     \brief Accessor
     \return strength of synchronization
     */
    inline tchecker::sync_strength_t strength() const
    {
      return _strength;
    }
  private:
    tchecker::process_id_t _pid;          /*!< Process identifier */
    tchecker::event_id_t _event_id;       /*!< Event identifier */
    tchecker::sync_strength_t _strength;  /*!< Synchronization strength */
  };
  
  
  
  
  /*!
   \class synchronization_t
   \brief Synchronization vector
   */
  class synchronization_t {
  public:
    /*!
     \brief Constructor
     \tparam SYNCS_ITER : type of iterator over tuples (process ID, event ID, strength)
     \param id : synchronization identifier
     \param syncs : range of synchronization constraints
     \pre syncs has at most one occurrence of each process ID
     \throw std::invalid_argument : if the preconditon is violated
     \note SYNCS_ITER is supposed to dereference to
     std::tuple<tchecker::process_id_t, tchecker::event_id_t, tchecker::sync_strength_t>
     */
    template <class SYNCS_ITER>
    synchronization_t(tchecker::sync_id_t id, tchecker::range_t<SYNCS_ITER> const & syncs)
    : _id(id)
    {
      static_assert(std::is_same< typename SYNCS_ITER::value_type,
                    std::tuple<tchecker::process_id_t, tchecker::event_id_t, tchecker::sync_strength_t> >::value,
                    "bad iterator");
      
      std::unordered_set<tchecker::process_id_t> sync_processes;
      
      for (auto const sync : syncs) {
        tchecker::process_id_t const pid = std::get<0>(sync);
        tchecker::event_id_t const event_id = std::get<1>(sync);
        tchecker::sync_strength_t const strength = std::get<2>(sync);
        
        if (sync_processes.find(pid) != sync_processes.end())
          throw std::invalid_argument("multiple occurrence of process");
        
        sync_processes.insert(pid);
        _constraints.emplace_back(pid, event_id, strength);
      }
    }
    
    /*!
     \brief Copy constructor
     \param constraints : synchronization constraints
     \post this is a copy of constraints
     */
    synchronization_t(tchecker::synchronization_t const & constraints) = default;
    
    /*!
     \brief Move constructor
     \param constraints : synchronization constraints
     \post constraints has been moved to this
     */
    synchronization_t(tchecker::synchronization_t && constraints) = default;
    
    /*!
     \brief Destructor
     */
    ~synchronization_t() = default;
    
    /*!
     \brief Assignment operator
     */
    tchecker::synchronization_t & operator= (tchecker::synchronization_t const &) = default;
    
    /*!
     \brief Move assignment operator
     */
    tchecker::synchronization_t & operator= (tchecker::synchronization_t &&) = default;
    
    /*!
     \brief Accessor
     \return Identifier
     */
    inline tchecker::sync_id_t id() const
    {
      return _id;
    }
    
    /*!
     \brief Type of iterator over synchronized process-events
     \note dereference to tchecker::sync_constraint_t const
     */
    using const_iterator_t = std::vector<tchecker::sync_constraint_t>::const_iterator;
    
    /*!
     \brief Accessor
     \return range of synchronized process-events
     */
    inline tchecker::range_t<const_iterator_t> synchronization_constraints() const
    {
      return tchecker::make_range(_constraints.begin(), _constraints.end());
    }
  private:
    tchecker::sync_id_t _id;                                /*!< Identifier */
    std::vector<tchecker::sync_constraint_t> _constraints;  /*!< Sync constraints */
  };
  
} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_SYNCHRONIZATION_HH
