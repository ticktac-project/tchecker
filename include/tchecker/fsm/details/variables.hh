/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_DETAILS_VARIABLES_HH
#define TCHECKER_FSM_DETAILS_VARIABLES_HH

#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"
#include "tchecker/vm/variables.hh"

/*!
 \file variables.hh
 \brief Variables for finite state machine model
 */

namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \class variables_t
       \brief Model variables for finite state machines
       */
      class variables_t {
      public:
        /*!
         \brief Constructor
         \param SYSTEM : type of system, should inherit from tchecker::fsm::details::system_t
         \param system : a system
         \post the model variables have been built from system
         */
        template <class SYSTEM>
        variables_t(SYSTEM const & system)
        : variables_t(system.intvars())
        {}
        
        /*!
         \brief Copy constructor
         */
        variables_t(tchecker::fsm::details::variables_t const &) = default;
        
        /*!
         \brief Move constructor
         */
        variables_t(tchecker::fsm::details::variables_t &&) = default;
        
        /*!
         \brief Destructor
         */
        ~variables_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::fsm::details::variables_t & operator= (tchecker::fsm::details::variables_t const &) = default;
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::fsm::details::variables_t & operator= (tchecker::fsm::details::variables_t &&) = default;

        /*!
         \brief Accessor
         \return System integer variables
         */
        inline constexpr tchecker::integer_variables_t const & system_bounded_integers() const
        {
          return _system_intvars;
        }
        
        /*!
         \brief Accessor
         \return Flattened bounded integers (virtual machine bounded integers)
         */
        inline constexpr tchecker::flat_integer_variables_t const & flattened_bounded_integers() const
        {
          return _vm_variables.bounded_integers();
        }
        
        /*!
         \brief Accessor
         \return Virtual machine variables
         */
        inline constexpr tchecker::vm_variables_t const & vm_variables() const
        {
          return _vm_variables;
        }
      protected:
        /*!
         \brief Constructor
         \param intvars : bounded integer variables
         \post System bounded integer variables and virtual machine variables have been
         built from intvars. The virtual machine variables have empty clocks.
         */
        variables_t(tchecker::integer_variables_t const & intvars)
        : _system_intvars(intvars), _vm_variables(intvars)
        {}

        /*!
         \brief Constructor
         \param intvars : bounded integer variables
         \param clocks : clock variables
         \post System bounded integer variables and virtual machine variables have been
         built from intvars and clocks.
         */
        variables_t(tchecker::integer_variables_t const & intvars, tchecker::clock_variables_t const & clocks)
        : _system_intvars(intvars), _vm_variables(intvars, clocks)
        {}
        
        tchecker::integer_variables_t _system_intvars;  /*!< System bounded integer variables */
        tchecker::vm_variables_t _vm_variables;         /*!< Virtual machine variables */
      };
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_DETAILS_VARIABLES_HH
