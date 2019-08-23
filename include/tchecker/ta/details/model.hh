/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_DETAILS_MODEL_HH
#define TCHECKER_TA_DETAILS_MODEL_HH

#include "tchecker/fsm/details/model.hh"
#include "tchecker/utils/log.hh"

/*!
 \file model.hh
 \brief Timed automaton model (details)
 */

namespace tchecker {
  
  namespace ta {
    
    namespace details {
      
      /*!
       \class model_t
       \brief Model for timed-automata
       \tparam SYSTEM : type of system, should inherit from tchecker::ta::details::system_t
       \tparam VARIABLES : type of model variables, should inherit from tchecker::ta::details::variables_t
       \note see tchecker::fsm::details::model_t for why instances cannot be constructed.
       */
      template <class SYSTEM, class VARIABLES>
      class model_t : public tchecker::fsm::details::model_t<SYSTEM, VARIABLES> {
      public:
        /*!
         \brief Copy constructor
         */
        model_t(tchecker::ta::details::model_t<SYSTEM, VARIABLES> const &) = default;
        
        /*!
         \brief Move constructor
         */
        model_t(tchecker::ta::details::model_t<SYSTEM, VARIABLES> &&) = default;
        
        /*!
         \brief Destructor
         */
        virtual ~model_t() = default;
        
        /*!
         \brief Assignment operator
         */
        tchecker::ta::details::model_t<SYSTEM, VARIABLES> &
        operator= (tchecker::ta::details::model_t<SYSTEM, VARIABLES> const &) = default;
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::ta::details::model_t<SYSTEM, VARIABLES> &
        operator= (tchecker::ta::details::model_t<SYSTEM, VARIABLES> &&) = default;
      protected:
        /*!
         \brief Constructor
         \param system : a system
         \param log : logging facility
         \note see tchecker::fsm::details::model_t
         */
        model_t(SYSTEM * system, tchecker::log_t & log)
        : tchecker::fsm::details::model_t<SYSTEM, VARIABLES>(system, log)
        {}
      };
      
    } // end of namespace details
    
  } // end of namespace ta
  
} // end of namespace tchecker

#endif // TCHECKER_TA_DETAILS_MODEL_HH

