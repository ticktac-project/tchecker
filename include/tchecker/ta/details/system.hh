/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_SYSTEM_HH
#define TCHECKER_TA_DETAILS_SYSTEM_HH

#include <string>

#include "tchecker/basictypes.hh"
#include "tchecker/fsm/details/system.hh"
#include "tchecker/ta/details/edge.hh"
#include "tchecker/ta/details/loc.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file system.hh
 \brief System of timed processes (details)
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \class system_t
       \tparam LOC : type of locations, should derive from
       tchecker::ta::details::loc_t
       \tparam EDGE : type of edges, should derive from
       tchecker::ta::details::edge_t
       \brief System of timed processes
       \note locations and edges cannot be added on purpose: one should use
       specialized derived classes in order to build models
       */
      template <class LOC, class EDGE>
      class system_t : public tchecker::fsm::details::system_t<LOC, EDGE> {
        
        static_assert(std::is_base_of<tchecker::ta::details::edge_t<LOC>, EDGE>::value, "bad EDGE type");
        
        static_assert(std::is_base_of<tchecker::ta::details::loc_t<EDGE>, LOC>::value, "bad LOC type");
        
      public:
        /*!
         \brief Constructor
         \param name : system name
         \pre name is not empty
         \throw std::invalid_argument : if the precondition is violated
         \throw std::runtime_error : if the zero clock does not have index 0
         */
        explicit system_t(std::string const & name) : tchecker::fsm::details::system_t<LOC, EDGE>(name)
        {
          // add the zero clock
          add_clock(tchecker::zero_clock_name, 1);
          if (_clocks.id(tchecker::zero_clock_name) != tchecker::zero_clock_id)
            throw std::runtime_error("zero clock should have index 0");
        }
        
        /*!
         \brief Copy constructor
         */
        system_t(tchecker::ta::details::system_t<LOC, EDGE> const &) = default;
        
        /*!
         \brief Move constructor
         */
        system_t(tchecker::ta::details::system_t<LOC, EDGE> &&) = default;
        
        /*!
         \brief Destructor
         */
        ~system_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::ta::details::system_t<LOC, EDGE> & operator= (tchecker::ta::details::system_t<LOC, EDGE> const &) = default;
        
        /*!
         \brief Move assignment operator
         */
        tchecker::ta::details::system_t<LOC, EDGE> & operator= (tchecker::ta::details::system_t<LOC, EDGE> &&) = default;
        
        /*!
         \brief Accessor
         \return Clock variables
         */
        inline tchecker::clock_variables_t const & clocks() const
        {
          return _clocks;
        }
        
        /*!
         \brief Add a clock variable
         \param name : clock name
         \param dim : dimension (array)
         \pre variable name is not declared yet and dim > 0
         \post the variable has been added
         \throw std::invalid_argument : if the precondition is violated
         */
        inline void add_clock(std::string const & name, unsigned int dim)
        {
          _clocks.declare(name, static_cast<tchecker::clock_id_t>(dim));
        }
      protected:
        tchecker::clock_variables_t _clocks;  /*!< Clock variables */
      };
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_SYSTEM_HH

