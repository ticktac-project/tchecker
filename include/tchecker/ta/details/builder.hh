/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_BUILDER_HH
#define TCHECKER_TA_DETAILS_BUILDER_HH

#include <string>
#include <vector>

#include "tchecker/expression/expression.hh"
#include "tchecker/fsm/details/builder.hh"
#include "tchecker/utils/log.hh"

/*!
 \file builder.hh
 \brief System builder for timed automata
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \class location_attributes_parser_t
       \brief Parser for location attributes
       \note Interpreted attributes: committed, initial, invariant, labels and
       urgent
       */
      class location_attributes_parser_t : public tchecker::fsm::details::location_attributes_parser_t {
      public:
        /*!
         \brief Constructor
         \param log : logging facility
         \param initial : location initial flag
         \param committed : location committed flag
         \param urgent : location urgent flag
         \param invariant : location invariant
         \param labels : location labels
         \post attributes have been initialised
         */
        location_attributes_parser_t(tchecker::log_t & log,
                                     bool & initial,
                                     bool & committed,
                                     bool & urgent,
                                     tchecker::expression_t * & invariant,
                                     std::vector<std::string> & labels);
        
        
        /*!
         \brief Copy constructor (DELETED)
         */
        location_attributes_parser_t(tchecker::ta::details::location_attributes_parser_t const &) = delete;
        
        
        /*!
         \brief Move constructor (DELETED)
         */
        location_attributes_parser_t(tchecker::ta::details::location_attributes_parser_t &&) = delete;
        
        
        /*!
         \brief Destructor
         \note no destructor called
         */
        ~location_attributes_parser_t() = default;
        
        
        /*!
         \brief Assignment operator (DELETED)
         */
        tchecker::ta::details::location_attributes_parser_t &
        operator= (tchecker::ta::details::location_attributes_parser_t const &) = delete;
        
        
        /*!
         \brief Move assignment operator (DELETED)
         */
        tchecker::ta::details::location_attributes_parser_t &
        operator= (tchecker::ta::details::location_attributes_parser_t &&) = delete;
      protected:
        /*!
         \brief Attribute parser
         \param attr : attribute
         */
        virtual void do_attr(tchecker::parsing::attr_t const & attr);
        
        bool & _committed;   /*!< Committed flag */
        bool & _urgent;      /*!< Urgent flag */
      };
      
      
      
      
      /*!
       \brief Parser for edges atributes
       \note Interpreted attributes: do and provided
       */
      using edge_attributes_parser_t = tchecker::fsm::details::edge_attributes_parser_t;
      
      
      
      
      /*!
       \class system_builder_t
       \tparam SYSTEM : type of system, should derive from tchecker::system_t
       and have constructor SYSTEM(std::string const &)
       \brief Fills system from declarations
       \note this class is virtual since location and edge declaration
       visitors are not defined
       */
      template <class SYSTEM>
      class system_builder_t : public tchecker::fsm::details::system_builder_t<SYSTEM> {
      public:
        /*!
         \brief Constructor
         */
        explicit system_builder_t(tchecker::log_t & log) : tchecker::fsm::details::system_builder_t<SYSTEM>(log)
        {}
        
        /*!
         \brief Copy constructor
         */
        system_builder_t(tchecker::ta::details::system_builder_t<SYSTEM> const &) = default;
        
        /*!
         \brief Move constructor
         */
        system_builder_t(tchecker::ta::details::system_builder_t<SYSTEM> &&) = default;
        
        /*!
         \brief Destructor
         */
        virtual ~system_builder_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::ta::details::system_builder_t<SYSTEM> &
        operator= (tchecker::ta::details::system_builder_t<SYSTEM> const &) = default;
        
        /*!
         \brief Move assignment operator
         */
        tchecker::ta::details::system_builder_t<SYSTEM> &
        operator= (tchecker::ta::details::system_builder_t<SYSTEM> &&) = default;
        
        /*!
         \brief Add clock variable
         */
        virtual void visit(tchecker::parsing::clock_declaration_t const & d)
        {
          try {
            this->_system->add_clock(d.name(), d.size());
          }
          catch (std::exception const & e) {
            this->_log.error("clock variable " + d.name(), e.what());
            throw;
          }
        }
        
        // Other methods visit()
        using tchecker::fsm::details::system_builder_t<SYSTEM>::visit;
      };
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_BUILDER_HH

