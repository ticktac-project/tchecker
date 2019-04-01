/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <string>
#include <vector>

#include "tchecker/system/output.hh"
#include "tchecker/ta/details/builder.hh"
#include "tchecker/ta/details/output.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/utils/log.hh"

namespace tchecker {
  
  namespace ta {
    
    
    // system_t
    
    void system_t::add_edge(std::string const & process_name, std::string const & src_name, std::string const & tgt_name,
                            std::string const & event_name, tchecker::expression_t * guard, tchecker::statement_t * statement)
    {
      tchecker::ta::edge_t * edge = nullptr;
      
      try {
        // Process identifier
        tchecker::process_id_t pid = _processes.key(process_name);
        
        // Identifier
        tchecker::edge_id_t id = next_edge_id();
        
        // Locations and event
        tchecker::ta::loc_t * src = this->location(process_name, src_name);
        tchecker::ta::loc_t * tgt = this->location(process_name, tgt_name);
        tchecker::event_id_t event_id = _events.key(event_name);
        
        // Create the edge
        edge = new tchecker::ta::edge_t(pid, id, src, tgt, event_id, guard, statement);
        
        // Add the edge
        tchecker::ta::details::system_t<tchecker::ta::loc_t, tchecker::ta::edge_t>::add_edge(edge, src, tgt);
      }
      catch (...) {
        delete edge;
        throw;
      }
    }
    
    
    
    
    // System builder
    
    /*!
     \class location_attributes_parser_t
     \brief Parser for location attributes
     \note Interpreted attributes: committed, initial, invariant, labels and
     urgent
     */
    class location_attributes_parser_t final : public tchecker::ta::details::location_attributes_parser_t {
    public:
      using tchecker::ta::details::location_attributes_parser_t::location_attributes_parser_t;
    };
    
    
    
    
    /*!
     \class edge_attributes_parser_t
     \brief Parser for edge attributes
     \note Interpreted attributes: do and provided
     */
    class edge_attributes_parser_t final : public tchecker::ta::details::edge_attributes_parser_t {
    public:
      using tchecker::ta::details::edge_attributes_parser_t::edge_attributes_parser_t;
    };
    
    
    
    
    /*!
     \class system_builder_t
     \brief Fills system from declarations
     \note Interpreted location attributes: committed, initial, invariant,
     labels and urgent
     \note Interpreted edge attributes: do and provided
     */
    class system_builder_t final : public tchecker::ta::details::system_builder_t<tchecker::ta::system_t> {
    public:
      /*!
       \brief Constructor
       */
      system_builder_t(tchecker::log_t & log)
      : tchecker::ta::details::system_builder_t<tchecker::ta::system_t>(log)
      {}
      
      
      /*!
       \brief Copy constructor (DELETED)
       */
      system_builder_t(tchecker::ta::system_builder_t const &) = delete;
      
      
      /*!
       \brief Move constructor (DELETED)
       */
      system_builder_t(tchecker::ta::system_builder_t &&) = delete;
      
      
      /*!
       \brief Destructor
       */
      virtual ~system_builder_t() = default;
      
      
      /*!
       \brief Assignment operator (DELETED)
       */
      tchecker::ta::system_builder_t & operator= (tchecker::ta::system_builder_t const &) = delete;
      
      
      /*!
       \brief Move assignment operator (DELETED)
       */
      tchecker::ta::system_builder_t & operator= (tchecker::ta::system_builder_t &&) = delete;
      
      
      /*!
       \brief Add location
       \note Intepreted attributes: initial, invariant and labels
       */
      virtual void visit(tchecker::parsing::location_declaration_t const & d)
      {
        bool committed = false;
        bool initial = false;
        bool urgent = false;
        tchecker::expression_t * invariant = nullptr;
        std::vector<std::string> labels;
        
        try {
          tchecker::ta::location_attributes_parser_t
          attr_parser(this->_log, committed, initial, urgent, invariant, labels);
          
          attr_parser.parse(d.attributes());
          
          // index new labels
          for (std::string const & label : labels)
            if (this->_system->labels().find_value(label) == this->_system->labels().end_value_map())
              this->_system->add_label(label);
          
          this->_system->add_location(d.process().name(), d.name(), initial, committed, urgent, invariant,
                                      tchecker::make_range(labels.begin(), labels.end()));
        }
        catch (std::exception const & e) {
          this->_log.error("location " + d.name() + " in process " + d.process().name(), e.what());
          delete invariant;
          throw;
        }
      }
      
      
      /*!
       \brief Add edges
       \note Intepreted attributes: provided and do
       */
      virtual void visit(tchecker::parsing::edge_declaration_t const & d)
      {
        tchecker::expression_t * guard = nullptr;
        tchecker::statement_t * statement = nullptr;
        
        try {
          tchecker::ta::edge_attributes_parser_t
          attr_parser(this->_log, guard, statement);
          
          attr_parser.parse(d.attributes());
          
          this->_system->add_edge(d.process().name(), d.src().name(), d.tgt().name(), d.event().name(), guard, statement);
        }
        catch (std::exception const & e) {
          this->_log.error("edge " + d.src().name() + "->" + d.tgt().name() + " in process " + d.process().name(), e.what());
          delete guard;
          delete statement;
          throw;
        }
      }
      
      
      // Other methods visit()
      using tchecker::ta::details::system_builder_t<tchecker::ta::system_t>::visit;
    };
    
    
    
    
    // build_system
    
    tchecker::ta::system_t * build_system(tchecker::parsing::system_declaration_t const & sysdecl, tchecker::log_t & log)
    {
      tchecker::ta::system_builder_t v(log);
      
      try {
        sysdecl.visit(v);
      }
      catch (...) {
        throw;
      }
      
      return v.release();
    }
    
    
    
    
    // Output system
    
    void tchecker_output_location(std::ostream & os, tchecker::ta::system_t const & s, tchecker::ta::loc_t const & loc)
    {
      tchecker::tchecker_output_location(os, s, loc);
      os << "{";
      if (loc.initial()) {
        tchecker::fsm::details::tchecker_output_location_initial_flag(os, s, loc);
        os << " : ";
      }
      if (loc.committed()) {
        tchecker::ta::details::tchecker_output_location_committed_flag(os, s, loc);
        os << " : ";
      }
      if (loc.urgent()) {
        tchecker::ta::details::tchecker_output_location_urgent_flag(os, s, loc);
        os << " : ";
      }
      tchecker::fsm::details::tchecker_output_location_invariant(os, s, loc);
      os << " : ";
      tchecker::fsm::details::tchecker_output_location_labels(os, s, loc);
      os << "}";
      os << std::endl;
    }
    
    
    void tchecker_output_edge(std::ostream & os, tchecker::ta::system_t const & s, tchecker::ta::edge_t const & edge)
    {
      tchecker::tchecker_output_edge(os, s, edge);
      os << "{";
      tchecker::fsm::details::tchecker_output_edge_guard(os, s, edge);
      os << " : ";
      tchecker::fsm::details::tchecker_output_edge_statement(os, s, edge);
      os << "}";
      os << std::endl;
    }
    
    
    void tchecker_output(std::ostream & os, tchecker::ta::system_t const & system)
    {
      tchecker::tchecker_output_system_name(os, system);
      tchecker::tchecker_output_system_events(os, system);
      tchecker::tchecker_output_system_processes(os, system);
      tchecker::tchecker_output_system_synchronizations(os, system);
      tchecker::fsm::details::tchecker_output_system_intvars(os, system);
      tchecker::ta::details::tchecker_output_system_clocks(os, system);
      tchecker::tchecker_output_system_locations
      (os, system, tchecker::ta::tchecker_output_location);
      tchecker::tchecker_output_system_edges
      (os, system, tchecker::ta::tchecker_output_edge);
    }
    
  } // end of namespace ta
  
} // end of namespace tchecker
