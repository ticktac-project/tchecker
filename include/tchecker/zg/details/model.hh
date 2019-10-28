/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_DETAILS_MODEL_HH
#define TCHECKER_ZG_DETAILS_MODEL_HH

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/model.hh"
#include "tchecker/utils/log.hh"

/*!
 \file model.hh
 \brief Zone graph model (details)
 */

namespace tchecker {
  
  namespace zg {
    
    namespace details {
      
      /*!
       \class model_t
       \brief Model for zone graph
       \tparam SYSTEM : type of system, see tchecker::clockbounds::model_t
       \tparam VARIABLES : type of model variables, should inherit from tchecker::clockbounds::model_t
       \note see tchecker::clockbounds::model_t for why instances cannot be constructed
       */
      template <class SYSTEM, class VARIABLES>
      class model_t : public tchecker::clockbounds::model_t<SYSTEM, VARIABLES> {
      public:
        /*!
         \brief Copy constructor
         */
        model_t(tchecker::zg::details::model_t<SYSTEM, VARIABLES> const & model)
        : tchecker::clockbounds::model_t<SYSTEM, VARIABLES>(model),
        _dimension(model._dimension)
        {}
        
        /*!
         \brief Move constructor
         */
        model_t(tchecker::zg::details::model_t<SYSTEM, VARIABLES> && model)
        : tchecker::clockbounds::model_t<SYSTEM, VARIABLES>(std::move(model)),
        _dimension(model._dimension)
        {}
        
        /*!
         \brief Destructor
         */
        virtual ~model_t() = default;
        
        /*!
         \brief Assignement operator
         \param model : a model
         \post this is a copy of model
         \return this after assignment
         */
        tchecker::zg::details::model_t<SYSTEM, VARIABLES> &
        operator= (tchecker::zg::details::model_t<SYSTEM, VARIABLES> const & model)
        {
          if (this != &model) {
            tchecker::clockbounds::model_t<SYSTEM, VARIABLES>::operator=(model);
            _dimension = model._dimension;
          }
          return *this;
        }
        
        /*!
         \brief Move-assignment operator
         \param model : a model
         \post model has been moved to this
         \return this after assignment
         */
        tchecker::zg::details::model_t<SYSTEM, VARIABLES> &
        operator= (tchecker::zg::details::model_t<SYSTEM, VARIABLES> && model)
        {
          if (this != &model) {
            tchecker::clockbounds::model_t<SYSTEM, VARIABLES>::operator=(std::move(model));
            _dimension = model._dimension;
          }
          return *this;
        }
        
        /*!
         \brief Accessor
         \return Dimension (number of clocks, including the zero clock)
         */
        virtual inline tchecker::clock_id_t dimension() const
        {
          return _dimension;
        }
      protected:
        /*!
         \brief Constructor
         \param system : a system
         \param log : logging facility
         \note see tchecker::ta::details::model_t
         \throw std::invalid_argument : if system has no computable clock bounds
         */
        model_t(SYSTEM * system, tchecker::log_t & log)
        : tchecker::clockbounds::model_t<SYSTEM, VARIABLES>(system, log),
        _dimension(tchecker::clockbounds::model_t<SYSTEM, VARIABLES>::flattened_clock_variables().flattened_size())
        {}
        
        tchecker::clock_id_t _dimension;    /*!< Dimension (number of clocks) */
      };
      
    } // end of namespace details
    
  } // end of namespace zg
  
} // end of namespace tchecker

#endif // TCHECKER_ZG_DETAILS_MODEL_HH


