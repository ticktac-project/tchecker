/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_CLOCKBOUNDS_MODEL_HH
#define TCHECKER_CLOCKBOUNDS_MODEL_HH

#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/clockbounds/solver.hh"
#include "tchecker/ta/details/model.hh"

/*!
 \file model.hh
 \brief Clock bounds model
 */

namespace tchecker {
  
  namespace clockbounds {
    
    /*!
     \class model_t
     \brief Interface to model providing clock bounds
     \tparam SYSTEM : type of system, see tchecker::ta::details::model_t
     \tparam VARIABLES : type of variables, should inherit from tchecker::ta::details::variables_t
     */
    template <class SYSTEM, class VARIABLES>
    class model_t : public tchecker::ta::details::model_t<SYSTEM, VARIABLES> {
    public:
      /*!
       \brief Copy constructor
       */
      model_t(tchecker::clockbounds::model_t<SYSTEM, VARIABLES> const & model)
      : tchecker::ta::details::model_t<SYSTEM, VARIABLES>(model),
      _global_lu_map(new tchecker::clockbounds::global_lu_map_t(*model._global_lu_map)),
      _local_lu_map(new tchecker::clockbounds::local_lu_map_t(*model._local_lu_map)),
      _global_m_map(new tchecker::clockbounds::global_m_map_t(*model._global_m_map)),
      _local_m_map(new tchecker::clockbounds::local_m_map_t(*model._local_m_map))
      {}
      
      /*!
       \brief Move constructor
       */
      model_t(tchecker::clockbounds::model_t<SYSTEM, VARIABLES> && model)
      : tchecker::ta::details::model_t<SYSTEM, VARIABLES>(std::move(model)),
      _global_lu_map(model._global_lu_map),
      _local_lu_map(model._local_lu_map),
      _global_m_map(model._global_m_map),
      _local_m_map(model._local_m_map)
      {
        model._global_lu_map = nullptr;
        model._local_lu_map = nullptr;
        model._global_m_map = nullptr;
        model._local_m_map = nullptr;
      }
      
      /*!
       \brief Destructor
       */
      virtual ~model_t()
      {
        delete _global_lu_map;
        delete _local_lu_map;
        delete _global_m_map;
        delete _local_m_map;
      }
      
      /*!
       \brief Assignement operator
       \param model : a model
       \post this is a copy of model
       \return this after assignment
       */
      tchecker::clockbounds::model_t<SYSTEM, VARIABLES> &
      operator= (tchecker::clockbounds::model_t<SYSTEM, VARIABLES> const & model)
      {
        if (this != &model) {
          tchecker::ta::details::model_t<SYSTEM, VARIABLES>::operator=(model);
          delete _global_lu_map;
          delete _local_lu_map;
          delete _global_m_map;
          delete _local_m_map;
          _global_lu_map = new tchecker::clockbounds::global_lu_map_t(*model._global_lu_map);
          _local_lu_map = new tchecker::clockbounds::local_lu_map_t(*model._local_lu_map);
          _global_m_map = new tchecker::clockbounds::global_m_map_t(*model._global_m_map);
          _local_m_map = new tchecker::clockbounds::local_m_map_t(*model._local_m_map);
        }
        return *this;
      }
      
      /*!
       \brief Move-assignment operator
       \param model : a model
       \post model has been moved to this
       \return this after assignment
       */
      tchecker::clockbounds::model_t<SYSTEM, VARIABLES> &
      operator= (tchecker::clockbounds::model_t<SYSTEM, VARIABLES> && model)
      {
        if (this != &model) {
          tchecker::ta::details::model_t<SYSTEM, VARIABLES>::operator=(std::move(model));
          delete _global_lu_map;
          delete _local_lu_map;
          delete _global_m_map;
          delete _local_m_map;
          _global_lu_map = model._global_lu_map;
          _local_lu_map = model._local_lu_map;
          _global_m_map = model._global_m_map;
          _local_m_map = model._local_m_map;
          model._global_lu_map = nullptr;
          model._local_lu_map = nullptr;
          model._global_m_map = nullptr;
          model._local_m_map = nullptr;
        }
        return *this;
      }
      
      /*!
       \brief Accessor
       \return Global LU clock bounds map
       */
      virtual inline tchecker::clockbounds::global_lu_map_t const & global_lu_map() const
      {
        return *_global_lu_map;
      }
      
      /*!
       \brief Accessor
       \return Global M clock bounds map
       */
      virtual inline tchecker::clockbounds::global_m_map_t const & global_m_map() const
      {
        return *_global_m_map;
      }
      
      /*!
       \brief Accessor
       \return Local LU clock bounds map
       */
      virtual inline tchecker::clockbounds::local_lu_map_t const & local_lu_map() const
      {
        return *_local_lu_map;
      }
      
      /*!
       \brief Accessor
       \return Local M clock bounds map
       */
      virtual inline tchecker::clockbounds::local_m_map_t const & local_m_map() const
      {
        return *_local_m_map;
      }
    protected:
      /*!
       \brief Constructor
       \param system : a system
       \param log : logging facility
       \post this model provides clock bounds for system
       \throw std::invalid_argument : if system has no computable clock bounds
       \note see tchecker::ta::details::model_t for why instances cannot be constructed
       */
      explicit model_t(SYSTEM * system, tchecker::log_t & log)
      : tchecker::ta::details::model_t<SYSTEM, VARIABLES>(system, log)
      {
        tchecker::loc_id_t loc_nb = system->locations_count();
        tchecker::clock_id_t clock_nb
        = tchecker::ta::details::model_t<SYSTEM, VARIABLES>::flattened_clock_variables().flattened_size();
        
        _global_lu_map = new tchecker::clockbounds::global_lu_map_t(clock_nb);
        _local_lu_map = new tchecker::clockbounds::local_lu_map_t(loc_nb, clock_nb);
        _global_m_map = new tchecker::clockbounds::global_m_map_t(clock_nb);
        _local_m_map = new tchecker::clockbounds::local_m_map_t(loc_nb, clock_nb);
        
        bool has_clock_bounds
        = tchecker::clockbounds::compute_all_clockbounds_map
        (*dynamic_cast<tchecker::ta::details::model_t<SYSTEM, VARIABLES> const *>(this),
         *_global_lu_map,
         *_local_lu_map,
         *_global_m_map,
         *_local_m_map);
        
        if ( ! has_clock_bounds )
          throw std::invalid_argument("system has no computable clock bounds");
      }
      
      tchecker::clockbounds::global_lu_map_t * _global_lu_map;  /*!< Global LU clock bounds map */
      tchecker::clockbounds::local_lu_map_t * _local_lu_map;    /*!< Local LU clock bounds map */
      tchecker::clockbounds::global_m_map_t * _global_m_map;    /*!< Global M clock bounds map */
      tchecker::clockbounds::local_m_map_t * _local_m_map;      /*!< Local M clock bounds map */
    };
    
  } // end of namespace clockbounds
  
} // end of namespace tchecker

#endif // TCHECKER_CLOCKBOUNDS_MODEL_HH
