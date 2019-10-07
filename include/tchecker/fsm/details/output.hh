/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_DETAILS_OUTPUT_HH
#define TCHECKER_FSM_DETAILS_OUTPUT_HH

#include <iostream>
#include <string>

#include "tchecker/basictypes.hh"
#include "tchecker/flat_system/vloc.hh"
#include "tchecker/fsm/details/state.hh"
#include "tchecker/fsm/details/transition.hh"
#include "tchecker/ts/output.hh"
#include "tchecker/variables/intvars.hh"


/*!
 \file output.hh
 \brief Outputters for finite state machines
 */

namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \brief Output integer variables following tchecker syntax
       \tparam SYSTEM : type of system, should inherit from
       tchecker::fsm::details::system_t
       \param os : output stream
       \param s : system
       \post s integer variables have been output to os
       */
      template <class SYSTEM>
      void tchecker_output_system_intvars(std::ostream & os, SYSTEM const & s)
      {
        auto const & decl_intvars = s.intvars().index();
        for (auto i : decl_intvars) {
          std::string const & name = decl_intvars.value(i);
          tchecker::intvar_id_t id = decl_intvars.key(i);
          tchecker::intvar_info_t const & info = s.intvars().info(id);
          os << "int:" << info.size() << ":" << info.min() << ":";
          os << info.max() << ":" << info.initial_value() << ":";
          os << name << std::endl;
        }
      }
      
      
      /*!
       \brief Output range of labels following tchecker syntax
       \tparam SYSTEM : type of system, should inherit from
       tchecker::fsm::details::system_t
       \param os : output stream
       \param s : system
       \param loc : locations
       \post loc labels names have been output to os
       */
      template <class SYSTEM>
      void tchecker_output_location_labels(std::ostream & os, SYSTEM const & s, typename SYSTEM::loc_t const & loc)
      {
        auto const & label_index = s.labels();
        
        os << "labels: ";
        
        bool first = true;
        for (tchecker::label_id_t label_id : loc.labels()) {
          if (! first)
            os << ",";
          os << label_index.value(label_id);
          first = false;
        }
      }
      
      
      /*!
       \brief Output location initial flag following tchecker syntax
       \tparam SYSTEM : type of system, should inherit from
       tchecker::fsm::details::system_t
       \param os : output stream
       \param s : system
       \param loc : location
       \post loc initial flag has been output to os
       */
      template <class SYSTEM>
      void tchecker_output_location_initial_flag(std::ostream & os, SYSTEM const & s, typename SYSTEM::loc_t const & loc)
      {
        if (loc.initial())
          os << "initial:";
      }
      
      
      /*!
       \brief Output location invariant following tchecker syntax
       \tparam SYSTEM : type of system, should inherit from
       tchecker::fsm::details::system_t
       \param os : output stream
       \param s : system
       \param loc : invariant
       \post loc invariant has been output to os
       */
      template <class SYSTEM>
      void tchecker_output_location_invariant(std::ostream & os, SYSTEM const & s, typename SYSTEM::loc_t const & loc)
      {
        os << "invariant: " << loc.invariant();
      }
      
      
      /*!
       \brief Output edge guard following tchecker syntax
       \tparam SYSTEM : type of system, should inherit from
       tchecker::fsm::details::system_t
       \param os : output stream
       \param s : system
       \param edge : edge
       \post edge guard has been output to os
       */
      template <class SYSTEM>
      void tchecker_output_edge_guard(std::ostream & os, SYSTEM const & s, typename SYSTEM::edge_t const & edge)
      {
        os << "provided: " << edge.guard();
      }
      
      
      /*!
       \brief Output edge statement following tchecker syntax
       \tparam SYSTEM : type of system, should inherit from
       tchecker::fsm::details::system_t
       \param os : output stream
       \param s : system
       \param edge : edge
       \post edge statement has been output to os
       */
      template <class SYSTEM>
      void tchecker_output_edge_statement(std::ostream & os, SYSTEM const & s, typename SYSTEM::edge_t const & edge)
      {
        os << "do: " << edge.statement();
      }
      
      
      
      
      /*!
       \brief Output state
       \tparam VLOC : type of tuple of locations
       \tparam INTVARS_VAL : type of integer variables valuations
       \tparam VLOC_PTR : type of pointer to tuple of locations
       \tparam INTVARS_VAL_PTR : type of pointer to integer variables valuation
       \param os : output stream
       \param s : state
       \param intvars : integer variables
       \post s has been output to os with integer variables names from intvars
       \return os after output
       */
      template <class VLOC, class INTVARS_VAL, class VLOC_PTR, class INTVARS_VAL_PTR>
      std::ostream & output(std::ostream & os,
                            tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s,
                            tchecker::intvar_index_t const & intvar_index)
      {
        tchecker::ts::output(os, s);
        tchecker::output(os, s.vloc());
        os << " ";
        tchecker::output(os, s.intvars_valuation(), intvar_index);
        return os;
      }
      
      
      
      
      /*!
       \class state_outputter_t
       \brief Outputter for states
       */
      class state_outputter_t : public tchecker::ts::state_outputter_t {
      public:
        /*!
         \brief Constructor
         \param intvar_index : index of integer variables
         \note this keeps a reference on intvar_index
         */
        explicit state_outputter_t(tchecker::intvar_index_t const & intvar_index) : _intvar_index(intvar_index)
        {}
        
        /*!
         \brief Copy constructor
         */
        state_outputter_t(tchecker::fsm::details::state_outputter_t const &) = default;
        
        /*!
         \brief Move constructor
         */
        state_outputter_t(tchecker::fsm::details::state_outputter_t &&) = default;
        
        /*!
         \brief Destructor
         */
        ~state_outputter_t() = default;
        
        /*!
         \brief Assignment operator (deleted)
         */
        tchecker::fsm::details::state_outputter_t & operator= (tchecker::fsm::details::state_outputter_t const &) = delete;
        
        /*!
         \brief Move-assignment operator (deleted)
         */
        tchecker::fsm::details::state_outputter_t & operator= (tchecker::fsm::details::state_outputter_t &&) = delete;
        
        /*!
         \brief Output state
         \tparam VLOC : type of tuple of locations
         \tparam INTVARS_VAL : type of integer variables valuations
         \tparam VLOC_PTR : type of pointer to tuple of locations
         \tparam INTVARS_VAL : type of pointer to integer variables valuation
         \param os : output stream
         \param s : state
         \post see tchecker::fsm::details::output
         \return os after output
         */
        template <class VLOC, class INTVARS_VAL, class VLOC_PTR, class INTVARS_VAL_PTR>
        inline std::ostream & output(std::ostream & os,
                                     tchecker::fsm::details::state_t<VLOC, INTVARS_VAL, VLOC_PTR, INTVARS_VAL_PTR> const & s)
        {
          return tchecker::fsm::details::output(os, s, _intvar_index);
        }
      protected:
        tchecker::intvar_index_t const & _intvar_index;   /*!< Index of integer variables */
      };
      
      
      
      /*!
       \brief Output transition
       \param os : output stream
       \param t : transition
       \post t has been output to os
       \return os after output
       */
      inline std::ostream & output(std::ostream & os, tchecker::fsm::details::transition_t const & t)
      {
        return tchecker::ts::output(os, t);
      }
      
      
      
      
      /*!
       \class transition_outputter_t
       \brief Transition outputter
       */
      class transition_outputter_t : public tchecker::ts::transition_outputter_t {
      public:
        /*!
         \brief Output transition
         \param os : output stream
         \param t : transition
         \post t has been output to os
         \return os after output
         */
        inline std::ostream & output(std::ostream & os, tchecker::fsm::details::transition_t const & t)
        {
          return tchecker::fsm::details::output(os, t);
        }
      };
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_DETAILS_OUTPUT_HH
