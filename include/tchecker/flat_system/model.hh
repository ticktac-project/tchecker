/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FLAT_SYSTEM_MODEL_HH
#define TCHECKER_FLAT_SYSTEM_MODEL_HH

#include "tchecker/flat_system/synchronizer.hh"

/*!
 \file model.hh
 \brief Model for a flat system
 */

namespace tchecker {
  
  namespace flat_system {
    
    /*!
     \class model_t
     \brief Model for a flat system: system + synchronizer
     \note Instances cannot be constructed. This is because the goal
     of class model_t is to own a system, and make sure that no other
     class can have a non-const access to the system and modify it.
     Hence model_t should build its system. This is delegated to
     derived classes that should provide a constructor that calls
     the adequate system builder.
     */
    template <class SYSTEM>
    class model_t {
    public:
      /*!
       \brief Type of system
       */
      using system_t = SYSTEM;
      
      /*!
       \brief Copy constructor
       \param model : a model
       \post this is a copy of model
       */
      model_t(tchecker::flat_system::model_t<SYSTEM> const & model)
      {
        _system = new SYSTEM(*model._system);
        _synchronizer = new tchecker::synchronizer_t<SYSTEM>(*_system);
      }
      
      /*!
       \brief Move constructor
       */
      model_t(tchecker::flat_system::model_t<SYSTEM> &&) = default;
      
      /*!
       \brief Destructor
       */
      ~model_t()
      {
        delete _synchronizer;
        delete _system;
      }
      
      /*!
       \brief Assignment operator
       \param model : a model
       \post this is a copy of model
       */
      tchecker::flat_system::model_t<SYSTEM> & operator= (tchecker::flat_system::model_t<SYSTEM> const & model)
      {
        if (this != &model) {
          delete _system;
          _system = new SYSTEM(*model._system);
          delete _synchronizer;
          _synchronizer = new tchecker::synchronizer_t<SYSTEM>(*_system);
        }
        return *this;
      }
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::flat_system::model_t<SYSTEM> & operator= (tchecker::flat_system::model_t<SYSTEM> &&)
      = default;
      
      /*!
       \brief Accessor
       \return system
       */
      SYSTEM const & system() const
      {
        return *_system;
      }
      
      /*!
       \brief Accessor
       \return synchronizer
       */
      tchecker::synchronizer_t<SYSTEM> const & synchronizer() const
      {
        return *_synchronizer;
      }
    protected:
      /*!
       \brief Constructor
       \param system : a system
       \post this consists in system + synchronizer on system edges
       \note this takes ownership on system
       */
      explicit model_t(SYSTEM * system)
      : _system(nullptr),
      _synchronizer(nullptr)
      {
        assert( system != nullptr );
        _system = system;
        _synchronizer = new tchecker::synchronizer_t<SYSTEM>(*_system);
      }
      
      SYSTEM * _system;                                   /*!< System */
      tchecker::synchronizer_t<SYSTEM> * _synchronizer;   /*!< Synchronizer */
    };
    
  } // end of namespace flat_system
  
} // end of namespace tchecker

#endif // TCHECKER_FLAT_SYSTEM_MODEL_HH
