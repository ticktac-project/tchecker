/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_OUTPUT_HH
#define TCHECKER_SYSTEM_OUTPUT_HH

#include <iostream>
#include <string>

#include "tchecker/basictypes.hh"

/*!
 \file output.hh
 \brief System output functions
 */

namespace tchecker {
  
  /*!
   \brief Output system name following tchecker syntax
   \tparam SYSTEM : type of system, should inherit from tchecker::system_t
   \param os : output stream
   \param s : system
   \post s name has been output to os
   */
  template <class SYSTEM>
  void tchecker_output_system_name(std::ostream & os, SYSTEM const & s)
  {
    os << "system:" << s.name() << std::endl;
  }
  
  /*!
   \brief Output events following tchecker syntax
   \tparam SYSTEM : type of system, should inherit from tchecker::system_t
   \param os : output stream
   \param s : system
   \post s events have been output to os
   */
  template <class SYSTEM>
  void tchecker_output_system_events(std::ostream & os, SYSTEM const & s)
  {
    auto const & events = s.events();
    for (auto const & e : events)
      os << "event:" << events.value(e) << std::endl;
  }
  
  /*!
   \brief Output processes following tchecker syntax
   \tparam SYSTEM : type of system, should inherit from tchecker::system_t
   \param os : output stream
   \param s : system
   \post s processes have been output to os
   */
  template <class SYSTEM>
  void tchecker_output_system_processes(std::ostream & os, SYSTEM const & s)
  {
    auto const & processes = s.processes();
    for (auto const & p : processes)
      os << "process:" << processes.value(p) << std::endl;
  }
  
  /*!
   \brief Output synchronizations following tchecker syntax
   \tparam SYSTEM : type of system, should inherit from tchecker::system_t
   \param os : output stream
   \param s : system
   \post s synchronizations have been output to os
   */
  template <class SYSTEM>
  void tchecker_output_system_synchronizations(std::ostream & os, SYSTEM const & s)
  {
    auto const & process_index = s.processes();
    auto const & event_index = s.events();
    
    for (auto const & sync : s.synchronizations()) {
      
      os << "sync:";
      
      bool first = true;
      for (auto const & constr : sync.synchronization_constraints()) {
        std::string const & process_name = process_index.value(constr.pid());
        std::string const & event_name = event_index.value(constr.event_id());
        if (! first)
          os << ":";
        os << process_name << "@" << event_name;
        if (constr.strength() == tchecker::SYNC_WEAK)
          os << "?";
        first = false;
      }
      os << std::endl;
    }
  }
  
  /*!
   \brief Output system locations following tchecker syntax
   \tparam SYSTEM : type of system, should inherit from tchecker::system_t
   \tparam LOC_OUTPUT : type of output function for location, should have
   type void(std::ostream &, SYSTEM const &, SYSTEM::loc_t const &)
   \param os : output stream
   \param s : system
   \param loc_output : location output function
   \post s locations have been been output to os using loc_output
   */
  template <class SYSTEM, class LOC_OUTPUT>
  void tchecker_output_system_locations(std::ostream & os, SYSTEM const & s, LOC_OUTPUT loc_output)
  {
    for (auto const * loc : s.locations())
      loc_output(os, s, *loc);
  }
  
  /*!
   \brief Output system edges following tchecker syntax
   \tparam SYSTEM : type of system, should inherit from tchecker::system_t
   \tparam EDGE_OUTPUT : type of output function for edges, should have
   type void(std::ostream &, SYSTEM const &, SYSTEM::edge_t const &)
   \param os : output stream
   \param s : system
   \param edge_output : edge output function
   \post s edges have been been output to os using edge_output
   */
  template <class SYSTEM, class EDGE_OUTPUT>
  void tchecker_output_system_edges(std::ostream & os, SYSTEM const & s, EDGE_OUTPUT edge_output)
  {
    for (auto const * edge : s.edges())
      edge_output(os, s, *edge);
  }
  
  /*!
   \brief Output location following tchecker syntax
   \tparam SYSTEM : type of system, should inherit from tchecker::system_t
   \param os : output stream
   \param loc : location
   \post loc has been been output to os (location:process_name:name)
   */
  template <class SYSTEM>
  void tchecker_output_location(std::ostream & os, SYSTEM const & s, typename SYSTEM::loc_t const & loc)
  {
    auto const & process_index = s.processes();
    
    std::string const & process_name = process_index.value(loc.pid());
    
    os << "location:" << process_name << ":" << loc.name();
  }
  
  /*!
   \brief Output edge following tchecker syntax
   \tparam SYSTEM : type of system, should inherit from tchecker::system_t
   \param os : output stream
   \param edge : edge
   \post edge has been been output to os
   (edge:process_name:src_name:tgt_name:event_name)
   */
  template <class SYSTEM>
  void tchecker_output_edge(std::ostream & os, SYSTEM const & s, typename SYSTEM::edge_t const & edge)
  {
    auto const & process_index = s.processes();
    auto const & event_index = s.events();
    
    std::string const & process_name = process_index.value(edge.pid());
    std::string const & event_name = event_index.value(edge.event_id());
    
    os << "edge:" << process_name << ":";
    os << edge.src()->name() << ":";
    os << edge.tgt()->name() << ":";
    os << event_name;
  }
  
} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_OUTPUT_HH
