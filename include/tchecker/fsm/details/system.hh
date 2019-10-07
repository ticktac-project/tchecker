/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_DETAILS_SYSTEM_HH
#define TCHECKER_FSM_DETAILS_SYSTEM_HH

#include <string>

#include "tchecker/basictypes.hh"
#include "tchecker/fsm/details/edge.hh"
#include "tchecker/fsm/details/loc.hh"
#include "tchecker/system/system.hh"
#include "tchecker/utils/index.hh"
#include "tchecker/variables/intvars.hh"


/*!
 \file system.hh
 \brief System of finite state machines with bounded integer variables (details)
 */

namespace tchecker {
  
  
  /*!
   \class label_index_t
   \brief Index of labels
   */
  class label_index_t
  : public tchecker::autokey_index_t<tchecker::label_id_t, std::string> {
  public:
    using tchecker::autokey_index_t<tchecker::label_id_t, std::string>::autokey_index_t;
  };
  
  
  
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \class system_t
       \tparam LOC : type of locations, should derive from
       tchecker::fsm::details::loc_t
       \tparam EDGE : type of edges, should derive from
       tchecker::fsm::details::edge_t
       \brief System of processes
       \note locations and edges cannot be added on purpose: one should use
       specialized derived classes in order to build models
       */
      template <class LOC, class EDGE>
      class system_t : public tchecker::system_t<LOC, EDGE> {
        
        static_assert(std::is_base_of<tchecker::fsm::details::loc_t<EDGE>, LOC>::value,
                      "bad LOC type");
        
        static_assert(std::is_base_of<tchecker::fsm::details::edge_t<LOC>, EDGE>::value,
                      "bad EDGE type");
      public:
        /*!
         \brief Constructor
         \param name : new name
         \pre name is not empty
         \throw std::invalid_argument : if the precondition is violated
         */
        explicit system_t(std::string const & name) : tchecker::system_t<LOC, EDGE>(name)
        {}
        
        
        /*!
         \brief Copy constructor
         \param s : system
         \post this is a copy of system
         */
        system_t(tchecker::fsm::details::system_t<LOC, EDGE> const & s) = default;
        
        
        /*!
         \brief Move constructor
         \param s : system
         \post system has been moved to this
         */
        system_t(tchecker::fsm::details::system_t<LOC, EDGE> && s) = default;
        
        
        /*!
         \brief Destructor
         */
        ~system_t() = default;
        
        
        /*!
         \brief Assignment operator
         \param s : system
         \post this is a copy of s
         */
        tchecker::fsm::details::system_t<LOC, EDGE> & operator= (tchecker::fsm::details::system_t<LOC, EDGE> const & s) = default;
        
        
        /*!
         \brief Move assignment operator
         \param s : system
         \post s has been moved to this
         */
        tchecker::fsm::details::system_t<LOC, EDGE> & operator= (tchecker::fsm::details::system_t<LOC, EDGE> && s) = default;
        
        
        /*!
         \brief Accessor
         \return label index
         */
        inline tchecker::label_index_t const & labels() const
        {
          return _labels;
        }
        
        
        /*!
         \brief Accessor
         \return Integer variables
         */
        inline tchecker::integer_variables_t const & intvars() const
        {
          return _intvars;
        }
        
        
        /*!
         \brief Accessor
         \return number of labels
         \note all values between 0 and the returned value-1 are valid label
         identifiers
         */
        inline tchecker::label_id_t labels_count() const
        {
          return _labels.size();
        }
        
        
        /*!
         \brief Add a label
         \param name : label name
         \pre label name is not declared yet
         \post the label has been added
         \throw std::invalid_argument : if the precondition is violated
         */
        void add_label(std::string const & name)
        {
          try {
            _labels.add(name);
          }
          catch (...) {
            throw;
          }
        }
        
        
        /*!
         \brief Add an integer variable
         \param name : variable name
         \param dim : dimension (array)
         \param min : minimal value
         \param max : maximal value
         \param initial : initial value
         \pre variable name is not declared yet, dim > 0, and
         min <= initial <= max
         \post the variable has been added
         \throw std::invalid_argument : if the precondition is violated
         */
        inline void add_intvar(std::string const & name,
                               unsigned int dim,
                               tchecker::integer_t min,
                               tchecker::integer_t max,
                               tchecker::integer_t initial)
        {
          _intvars.declare(name, static_cast<tchecker::intvar_id_t>(dim), min, max, initial);
        }
      protected:
        tchecker::label_index_t _labels;          /*!< Labels index */
        tchecker::integer_variables_t _intvars;   /*!< Integer variables */
      };
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_DETAILS_SYSTEM_HH

