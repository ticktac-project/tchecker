/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_DETAILS_BUILDER_HH
#define TCHECKER_FSM_DETAILS_BUILDER_HH

#include <string>
#include <vector>

#include "tchecker/expression/expression.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/statement/statement.hh"
#include "tchecker/system/builder.hh"
#include "tchecker/utils/log.hh"

/*!
 \file builder.hh
 \brief System builder for finite state machines
 */

namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \class location_attributes_parser_t
       \brief Parser for location attributes
       \note Interpreted attributes: initial, invariant and labels
       */
      class location_attributes_parser_t : public tchecker::parsing::attributes_parser_t {
      public:
        /*!
         \brief Constructor
         \param log : logging facility
         \param initial : location initial flag
         \param invariant : location invariant
         \param labels : location labels
         \post attributes have been initialised
         */
        location_attributes_parser_t(tchecker::log_t & log,
                                     bool & initial,
                                     tchecker::expression_t * & invariant,
                                     std::vector<std::string> & labels);
        
        
        /*!
         \brief Copy constructor (DELETED)
         */
        location_attributes_parser_t(tchecker::fsm::details::location_attributes_parser_t const &) = delete;
        
        
        /*!
         \brief Move constructor (DELETED)
         */
        location_attributes_parser_t(tchecker::fsm::details::location_attributes_parser_t &&) = delete;
        
        
        /*!
         \brief Destructor
         \note no destructor called
         */
        ~location_attributes_parser_t() = default;
        
        
        /*!
         \brief Assignment operator (DELETED)
         */
        tchecker::fsm::details::location_attributes_parser_t &
        operator= (tchecker::fsm::details::location_attributes_parser_t const &) = delete;
        
        
        /*!
         \brief Move assignment operator (DELETED)
         */
        tchecker::fsm::details::location_attributes_parser_t &
        operator= (tchecker::fsm::details::location_attributes_parser_t &&) = delete;
        
        
        /*!
         \brief Attributes parser
         \param attributes : attributes
         \post do_attr() has been called on every attribute in attributes and
         attributes are properly set
         */
        void parse(tchecker::parsing::attributes_t const & attributes);
      protected:
        /*!
         \brief Attribute parser
         \param attr : attribute
         */
        virtual void do_attr(tchecker::parsing::attr_t const & attr);
        
        tchecker::log_t & _log;                  /*!< Logging facility */
        bool & _initial;                         /*!< Initial flag */
        tchecker::expression_t * & _invariant;   /*!< Invariant */
        std::vector<std::string> & _labels;      /*!< Labels */
      };
      
      
      
      
      /*!
       \class edge_attributes_parser_t
       \brief Parser for edge attributes
       \note Interpreted attributes: do and provided
       */
      class edge_attributes_parser_t : public tchecker::parsing::attributes_parser_t {
      public:
        /*!
         \brief Constructor
         \param log : logging facility
         \param guard : edge guard
         \param statement : edge statement
         */
        edge_attributes_parser_t(tchecker::log_t & log, tchecker::expression_t * & guard, tchecker::statement_t * & statement);
        
        
        /*!
         \brief Copy constructor (DELETED)
         */
        edge_attributes_parser_t(tchecker::fsm::details::edge_attributes_parser_t const &) = delete;
        
        
        /*!
         \brief Move constructor (DELETED)
         */
        edge_attributes_parser_t(tchecker::fsm::details::edge_attributes_parser_t &&) = delete;
        
        
        /*!
         \brief Destructor
         \note no destructor called
         */
        ~edge_attributes_parser_t() = default;
        
        
        /*!
         \brief Assignment operator (DELETED)
         */
        tchecker::fsm::details::edge_attributes_parser_t &
        operator= (tchecker::fsm::details::edge_attributes_parser_t const &) = delete;
        
        
        /*!
         \brief Move assignment operator (DELETED)
         */
        tchecker::fsm::details::edge_attributes_parser_t &
        operator= (tchecker::fsm::details::edge_attributes_parser_t &&) = delete;
        
        
        /*!
         \brief Attributes parser
         \param attributes : attributes
         \post do_attr() has been called on every attribute in attributes and
         attributes are properly set
         */
        void parse(tchecker::parsing::attributes_t const & attributes);
      protected:
        /*!
         \brief Attribute parser
         \param attr : attribute
         */
        virtual void do_attr(tchecker::parsing::attr_t const & attr);
        
        
        tchecker::log_t & _log;                 /*!< Logging facility */
        tchecker::expression_t * & _guard;      /*!< Guard */
        tchecker::statement_t * & _statement;   /*!< Statement */
      };
      
      
      
      
      /*!
       \class system_builder_t
       \tparam SYSTEM : type of system, should derive from tchecker::system_t
       and have constructor SYSTEM(std::string const &)
       \brief Fills system from declarations
       \note this class is virtual since clock, location and edge declaration
       visitors are not defined
       */
      template <class SYSTEM>
      class system_builder_t : public tchecker::system_builder_t<SYSTEM> {
      public:
        /*!
         \brief Constructor
         */
        explicit system_builder_t(tchecker::log_t & log) : tchecker::system_builder_t<SYSTEM>(log)
        {}
        
        /*!
         \brief Copy constructor
         */
        system_builder_t(tchecker::fsm::details::system_builder_t<SYSTEM> const &) = default;
        
        /*!
         \brief Move constructor
         */
        system_builder_t(tchecker::fsm::details::system_builder_t<SYSTEM> &&) = default;
        
        /*!
         \brief Destructor
         */
        virtual ~system_builder_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::fsm::details::system_builder_t<SYSTEM> &
        operator= (tchecker::fsm::details::system_builder_t<SYSTEM> const &) = default;
        
        /*!
         \brief Move assignment operator
         */
        tchecker::fsm::details::system_builder_t<SYSTEM> &
        operator= (tchecker::fsm::details::system_builder_t<SYSTEM> &&) = default;
        
        /*!
         \brief Add integer variable
         */
        virtual void visit(tchecker::parsing::int_declaration_t const & d)
        {
          try {
            this->_system->add_intvar(d.name(), d.size(), d.min(), d.max(), d.init());
          }
          catch (std::exception const & e) {
            this->_log.error("integer variable " + d.name(), e.what());
            throw;
          }
        }
        
        // Other methods visit()
        using tchecker::system_builder_t<SYSTEM>::visit;
      };
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_DETAILS_BUILDER_HH

