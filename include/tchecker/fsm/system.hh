/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_SYSTEM_HH
#define TCHECKER_FSM_SYSTEM_HH

#include <functional>
#include <iostream>
#include <string>

#include <boost/iterator/transform_iterator.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/expression/expression.hh"
#include "tchecker/fsm/details/edge.hh"
#include "tchecker/fsm/details/loc.hh"
#include "tchecker/fsm/details/system.hh"
#include "tchecker/parsing/declaration.hh"
#include "tchecker/statement/statement.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/utils/log.hh"

/*!
 \file system.hh
 \brief System of finite state machines with bounded integer variables
 */

namespace tchecker {
  
  namespace fsm {
    
    // System
    
    class loc_t;   // forward declaration
    class edge_t;  // forward declaration
    
    
    /*!
     \class loc_t
     \brief Location
     */
    class loc_t final : public tchecker::fsm::details::loc_t<tchecker::fsm::edge_t> {
    public:
      using tchecker::fsm::details::loc_t<tchecker::fsm::edge_t>::loc_t;
    };
    
    
    
    
    /*!
     \class edge_t
     \brief Edge
     */
    class edge_t final : public tchecker::fsm::details::edge_t<tchecker::fsm::loc_t> {
    public:
      using tchecker::fsm::details::edge_t<tchecker::fsm::loc_t>::edge_t;
    };
    
    
    
    
    /*!
     \class system_t
     \brief System of finite state machines, with bounded integer variables
     */
    class system_t final : public tchecker::fsm::details::system_t<tchecker::fsm::loc_t, tchecker::fsm::edge_t> {
    public:
      using tchecker::fsm::details::system_t<tchecker::fsm::loc_t, tchecker::fsm::edge_t>::system_t;
      
      // add location
    private:
      /*!
       \brief Type of proxy for const transformation iterator over labels
       */
      using label_trans_iterator_proxy_t = std::function< tchecker::label_id_t(std::string const &) >;
      
      /*!
       \brief Proxy for const transformation iterator over labels
       */
      tchecker::label_id_t label_trans_iterator_proxy(std::string const & label)
      {
        return _labels.key(label);
      }
    public:
      /*!
       \brief Add a location
       \tparam LABELS_ITER : type of iterator on labels
       \param process_name : process name
       \param name : location name
       \param initial : initial location flag
       \param invariant : location invariant
       \param labels_range : range (begin, end) of location labels
       \pre process_name is a declared process, name is not a declared location,
       and invariant is not nullptr
       \note this takes ownership on invariant
       \note LABELS_ITER is supposed to dereference to std::string
       \throw std::invalid_argument : if the precondition is violated
       */
      template <class LABELS_ITER>
      void add_location(std::string const & process_name,
                        std::string const & name,
                        bool initial,
                        tchecker::expression_t * invariant,
                        tchecker::range_t<LABELS_ITER> const & labels)
      {
        static_assert(std::is_same<typename LABELS_ITER::value_type, std::string>::value,
                      "expecting iterator on std::string");
        
        tchecker::fsm::loc_t * loc = nullptr;
        
        try {
          // Process identifier
          tchecker::process_id_t pid = _processes.key(process_name);
          
          // Location identifier
          tchecker::loc_id_t id = next_loc_id();
          
          // Labels
          auto proxy = std::bind(&tchecker::fsm::system_t::label_trans_iterator_proxy, this, std::placeholders::_1);
          
          boost::transform_iterator<label_trans_iterator_proxy_t, LABELS_ITER>
          begin(labels.begin(), proxy), end(labels.end(), proxy);
          
          // Create location
          loc = new tchecker::fsm::loc_t(pid, id, name, initial, invariant, tchecker::make_range(begin, end));
          
          // Add location
          tchecker::fsm::details::system_t<tchecker::fsm::loc_t, tchecker::fsm::edge_t>::add_location(loc);
        }
        catch (...) {
          delete loc;
          throw;
        }
      }
      
      
      /*!
       \brief Add an edge
       \param process_name : process name
       \param src_name : source location name
       \param tgt_name : target location name
       \param event_name : event name
       \param guard : guard
       \param statement : statement
       \pre process_name is a declared process, src_name is a declared
       location of process_name, tgt_name is a declared location of
       process_name, event_name is a declared event, guard is not nullptr
       and statement is not nullptr
       \note this takes ownership on guard and statement
       \throw std::invalid_argument : if the precondition is violated
       */
      void add_edge(std::string const & process_name,
                    std::string const & src_name,
                    std::string const & tgt_name,
                    std::string const & event_name,
                    tchecker::expression_t * guard,
                    tchecker::statement_t * statement);
    };
    
    
    // System builder
    
    /*!
     \brief Builds a system from a declaration
     \param sysdecl : system declaration
     \param log : logging facility
     \return system corresponding to sysdecl
     \throw std::invalid_argument : if sysdecl is not correct
     */
    tchecker::fsm::system_t * build_system(tchecker::parsing::system_declaration_t const & sysdecl, tchecker::log_t & log);
    
    
    // System output
    
    /*!
     \brief System output following tchecker syntax
     \param os : output stream
     \param system : system
     \post system has been output to os
     */
    void tchecker_output(std::ostream & os, tchecker::fsm::system_t const & system);
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_SYSTEM_HH
