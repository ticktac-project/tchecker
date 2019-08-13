/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_VARIABLES_HH
#define TCHECKER_TA_DETAILS_VARIABLES_HH

#include "tchecker/fsm/details/variables.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"
#include "tchecker/vm/variables.hh"

/*!
 \file variables.hh
 \brief Variables for timed automata model
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \class variables_t
       \brief Model variables for timed automata
       */
      class variables_t : public tchecker::fsm::details::variables_t {
      public:
        /*!
         \brief Constructor
         \tparam SYSTEM : type of system, should inherit from tchecker::ta::details::system_t
         \param system : a system
         \post the model variables have been built from system
         */
        template <class SYSTEM>
        variables_t(SYSTEM const & system)
        : variables_t(system.intvars(), system.clocks())
        {}
        
        /*!
         \brief Copy constructor
         */
        variables_t(tchecker::ta::details::variables_t const &) = default;
        
        /*!
         \brief Move constructor
         */
        variables_t(tchecker::ta::details::variables_t &&) = default;
        
        /*!
         \brief Destructor
         */
        ~variables_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::ta::details::variables_t & operator= (tchecker::ta::details::variables_t const &) = default;
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::ta::details::variables_t & operator= (tchecker::ta::details::variables_t &&) = default;
        
        /*!
         \brief Accessor
         \return System clock variables
         */
        inline constexpr tchecker::clock_variables_t const & system_clocks() const
        {
          return _system_clocks;
        }
        
        /*!
         \brief Accessor
         \return Flattened clock variables (virtual machine clocks)
         */
        inline constexpr tchecker::flat_clock_variables_t const & flattened_clocks() const
        {
          return _vm_variables.clocks();
        }
      protected:
        /*!
         \brief Constructor
         \param intvars : bounded integer variables
         \param clocks : clock variables
         */
        variables_t(tchecker::integer_variables_t const & intvars, tchecker::clock_variables_t const & clocks)
        : tchecker::fsm::details::variables_t(intvars, clocks),
        _system_clocks(clocks)
        {}
        
        tchecker::clock_variables_t _system_clocks;  /*!< System clock variables */
      };
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_VARIABLES_HH
