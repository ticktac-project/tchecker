/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_STATIC_ANALYSIS_HH
#define TCHECKER_SYSTEM_STATIC_ANALYSIS_HH

#include <vector>

#include <boost/container/flat_set.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/system/system.hh"

/*!
 \file static_analysis.hh
 \brief Static analysis on systems of processes
 */

namespace tchecker {

  /*!
   \class process_events_map_t
   \brief Type of map : process ID -> event IDs
   */
  class process_events_map_t {
  public:
    /*!
     \brief Constructor
     \param proc_count : number of processes
     */
    process_events_map_t(tchecker::process_id_t proc_count);
    
    /*!
     \brief Copy-constructor
     */
    process_events_map_t(tchecker::process_events_map_t const &) = default;
    
    /*!
     \brief Move-constructor
     */
    process_events_map_t(tchecker::process_events_map_t &&) = default;
    
    /*!
     \brief Destructor
     */
    ~process_events_map_t() = default;
    
    /*!
     \brief Assignment operator
     */
    tchecker::process_events_map_t & operator= (tchecker::process_events_map_t const &) = default;
    
    /*!
     \brief Move-assignment operator
     */
    tchecker::process_events_map_t & operator= (tchecker::process_events_map_t &&) = default;
    
    /*!
     \brief Insertion
     \param pid : a process ID
     \param event_id : an event ID
     \pre pid < proc_count (checked by assertion)
     \post pid |-> event_id has been added to the map
     */
    void insert(tchecker::process_id_t pid, tchecker::event_id_t event_id);
    
    /*!
     \brief Lookup
     \param pid : a process ID
     \param event_id : an event ID     
     \return true if this maps pid to (a set of events containing) event_id, false otherwise
     */
    bool contains(tchecker::process_id_t pid, tchecker::event_id_t event_id);
  private:
    std::vector<boost::container::flat_set<tchecker::event_id_t>> _map;  /*!< multi-map : process ID -> event IDs */
  };
  
  
  /*!
   \brief Compute weakly synchronized events
   \param range : a range of synchronization vectors
   \param proc_count : number of processes
   \pre all the process IDs in synchronization vectors in range are less than proc_count
   \return a map from process IDs to the set of weakly synchronized event IDs in each process, w.r.t. synchronization vectors
   in range
   */
  tchecker::process_events_map_t
  weakly_synchronized_events(tchecker::range_t<tchecker::const_sync_iterator_t> const & range, tchecker::process_id_t proc_count);
  
  
  /*!
   \brief Compute weakly synchronized events
   \tparam LOC : type of locations
   \tparam EDGE : type of edges
   \param system : a system of processes
   \return a map from process IDs to the set of weakly synchronized event IDs in each process of system
   */
  template <class LOC, class EDGE>
  tchecker::process_events_map_t weakly_synchronized_events(tchecker::system_t<LOC, EDGE> const & system)
  {
    return tchecker::weakly_synchronized_events(system.synchronizations(), system.processes_count());
  }
  
} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_STATIC_ANALYSIS_HH
