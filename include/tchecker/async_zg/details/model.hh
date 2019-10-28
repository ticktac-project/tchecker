/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ASYNC_ZG_DETAILS_MODEL_HH
#define TCHECKER_ASYNC_ZG_DETAILS_MODEL_HH

#include <cassert>
#include <cstring>
#include <limits>
#include <string>

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/model.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/variables/access.hh"
#include "tchecker/variables/offset_clocks.hh"
#include "tchecker/variables/static_analysis.hh"

/*!
 \file model.hh
 \brief Model for asynchronous zone graph (details)
 */

namespace tchecker {
  
  namespace async_zg {
    
    namespace details {
      
      /*!
       \class model_t
       \brief Model for asynchronous zone graph
       \tparam SYSTEM : type of system, see tchecker::ta::details::model_t
       \tparam VARIABLES : type of model variables, should inherit from tchecker::async_zg::details::variables_t
       \note see tchecker::ta::details::model_t for why instances cannot be constructed.
       */
      template <class SYSTEM, class VARIABLES>
      class model_t : public tchecker::clockbounds::model_t<SYSTEM, VARIABLES> {
      public:
        /*!
         \brief Copy constructor
         */
        model_t(tchecker::async_zg::details::model_t<SYSTEM, VARIABLES> const & model)
        : tchecker::clockbounds::model_t<SYSTEM, VARIABLES>(model),
        _flattened_offset_clocks(model._flattened_offset_clocks)
        {}
        
        /*!
         \brief Move constructor
         */
        model_t(tchecker::async_zg::details::model_t<SYSTEM, VARIABLES> && model)
        : tchecker::clockbounds::model_t<SYSTEM, VARIABLES>(std::move(model)),
        _flattened_offset_clocks(std::move(model._flattened_offset_clocks))
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
        tchecker::async_zg::details::model_t<SYSTEM, VARIABLES> &
        operator= (tchecker::async_zg::details::model_t<SYSTEM, VARIABLES> const & model)
        {
          if (this != &model) {
            tchecker::clockbounds::model_t<SYSTEM, VARIABLES>::operator=(model);
            _flattened_offset_clocks = model._flattened_offset_clocks;
          }
          return *this;
        }
        
        /*!
         \brief Move-assignment operator
         \param model : a model
         \post model has been moved to this
         \return this after assignment
         */
        tchecker::async_zg::details::model_t<SYSTEM, VARIABLES> &
        operator= (tchecker::async_zg::details::model_t<SYSTEM, VARIABLES> && model)
        {
          if (this != &model) {
            tchecker::clockbounds::model_t<SYSTEM, VARIABLES>::operator=(std::move(model));
            _flattened_offset_clocks = std::move(model._flattened_offset_clocks);
          }
          return *this;
        }
        
        /*!
         \brief Accessor
         \return flattened offset clocks
         */
        constexpr inline tchecker::offset_clock_variables_t const & flattened_offset_clock_variables() const
        {
          return _flattened_offset_clocks;
        }
      protected:
        /*!
         \brief Constructor
         \param system : a system
         \param log : logging facility
         \note see tchecker::ta::details::model_t
         \throw std::invalid_argument : if system has shared variables
         \throw std::invalid_argument : if a clock in system is accessed by no process
         \throw std::invalid_argument : if the number of reference/offset variables exceeds the maximum
         value that can be represented by type tchecker::clock_id_t
         */
        explicit model_t(SYSTEM * system, tchecker::log_t & log)
        : tchecker::clockbounds::model_t<SYSTEM, VARIABLES>(system, log)
        {
          tchecker::variable_access_map_t vaccess_map = tchecker::variable_access(*this);
          
          if (vaccess_map.has_shared_variable())
            throw std::invalid_argument("model has shared variable(s)");
          
          _flattened_offset_clocks
          = tchecker::build_from_variable_access(vaccess_map,
                                                 system->processes_count(),
                                                 tchecker::clockbounds::model_t<SYSTEM, VARIABLES>::flattened_clock_variables());
        }
        
        tchecker::offset_clock_variables_t _flattened_offset_clocks;  /*!< Flattened offset clock variables */
      };
      
    } // end of namespace details
    
  } // end of namespace async_zg
  
} // end of namespace tchecker

#endif // TCHECKER_ASYNC_ZG_DETAILS_MODEL_HH



