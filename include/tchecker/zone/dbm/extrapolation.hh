/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZONE_DBM_EXTRAPOLATION_HH
#define TCHECKER_ZONE_DBM_EXTRAPOLATION_HH

#include <functional>

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/clockbounds/model.hh"
#include "tchecker/clockbounds/vlocbounds.hh"
#include "tchecker/dbm/dbm.hh"

/*!
 \file extrapolation.hh
 \brief Extrapolation on DBMs
 */

namespace tchecker {
  
  namespace dbm {
    
#ifdef HIDDEN_TO_COMPILER
    
    /*!
     \class extrapolation_t
     \brief Interface to zone extrapolation
     */
    class extrapolation_t {
    public:
      /*!
       \brief Extrapolate a DBM
       \param dbm : a DBM
       \param dim : dimension of dbm
       \param vloc : tuple of locations
       \post dbm has been extrapolated w.r.t. clock bounds in vloc
       */
      template <class VLOC>
      void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, VLOC const & loc_ids);
    };
    
#endif // HIDDEN_TO_COMPILER
    
    
    
    /*!
     \brief Type of extrapolation function using M clock bounds
     */
    typedef void (*extra_M_t) (tchecker::dbm::db_t *, tchecker::clock_id_t, int32_t const *);
    
    
    /*!
     \brief Type of extrapolation function using LU clock bounds
     */
    typedef void (*extra_LU_t) (tchecker::dbm::db_t *, tchecker::clock_id_t, int32_t const *, int32_t const *);
    
    
    
    
    /*!
     \class global_M_extrapolation_t
     \brief Zone extrapolation with global M clock bounds
     \tparam EXTRAPOLATION : extrapolation function, uses M clock bounds
     */
    template <extra_M_t EXTRAPOLATION>
    class global_M_extrapolation_t {
    public:
      /*!
       \brief Constructor
       \param model : a model
       \tparam MODEL : type of model, should inherit from tchecker::clockbounds::model_t
       \note this keeps a reference on model.global_m_map()
       */
      template <class MODEL>
      explicit global_M_extrapolation_t(MODEL const & model)
      : _global_m_map(model.global_m_map())
      {
        _M = _global_m_map.get().M().ptr();
      }
      
      /*!
       \brief Copy constructor
       \param e : global M extrapolation
       \post this is a copy of e
       */
      global_M_extrapolation_t(tchecker::dbm::global_M_extrapolation_t<EXTRAPOLATION> const & e)
      : _global_m_map(e._global_m_map)
      {
        _M = _global_m_map.get().M().ptr();
      }
      
      /*!
       \brief Move constructor
       \param e : global M extrapolation
       \post e has been moved to this
       */
      global_M_extrapolation_t(tchecker::dbm::global_M_extrapolation_t<EXTRAPOLATION> && e)
      : _global_m_map(std::move(e._global_m_map)), _M(e._M)
      {
        e._M = nullptr;
      }
      
      /*!
       \brief Destructor
       */
      virtual ~global_M_extrapolation_t() = default;
      // DO NOT DELETE _M (pointer to an object inside _global_m_map)
      
      /*!
       \brief Assignment operator
       \param e : global M extrapolation
       \post this is a copy of e
       \return this after assignment
       */
      tchecker::dbm::global_M_extrapolation_t<EXTRAPOLATION> &
      operator= (tchecker::dbm::global_M_extrapolation_t<EXTRAPOLATION> const & e)
      {
        if (this != &e) {
          _global_m_map = e._global_m_map;
          _M = _global_m_map.get().M().ptr();
        }
        return *this;
      }
      
      /*!
       \brief Move-assignment operator
       \param e : global M extrapolation
       \post e has been moved to this
       \return this after assignment
       */
      tchecker::dbm::global_M_extrapolation_t<EXTRAPOLATION> &
      operator= (tchecker::dbm::global_M_extrapolation_t<EXTRAPOLATION> && e)
      {
        if (this != &e) {
          _global_m_map = std::move(e._global_m_map);
          _M = _global_m_map.get().M().ptr();
          e._M = nullptr;
        }
        return *this;
      }
      
      /*!
       \brief Extrapolate a DBM
       \param dbm : a DBM
       \param dim : dimension of dbm
       \param vloc : tuple of locations
       \post dbm has been extrapolated w.r.t. global M clock bounds and EXTRAPOLATION
       */
      template <class VLOC>
      inline void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, VLOC const & vloc)
      {
        assert(dim == _global_m_map.get().clock_number());
        EXTRAPOLATION(dbm, dim, _M);
      }
    private:
      std::reference_wrapper<tchecker::clockbounds::global_m_map_t const> _global_m_map;  /*!< Global M map */
      tchecker::integer_t const * _M;                                                     /*!< Direct access to M clock bounds */
    };
    
    
    
    
    /*!
     \class local_M_extrapolation_t
     \brief Zone extrapolation with local M clock bounds
     \tparam EXTRAPOLATION : extrapolation function, uses M clock bounds
     */
    template <extra_M_t EXTRAPOLATION>
    class local_M_extrapolation_t {
    public:
      /*!
       \brief Constructor
       \param model : a model
       \tparam MODEL : type of model, should inherit from tchecker::clockbounds::model_t
       \note this keeps a reference on model.local_m_map()
       */
      template <class MODEL>
      explicit local_M_extrapolation_t(MODEL const & model)
      : _local_m_map(model.local_m_map())
      {
        _M = tchecker::clockbounds::allocate_map(_local_m_map.get().clock_number());
      }
      
      /*!
       \brief Copy constructor
       \param e : local M extrapolation
       \post this is a copy of e
       */
      local_M_extrapolation_t(tchecker::dbm::local_M_extrapolation_t<EXTRAPOLATION> const & e)
      : _local_m_map(e._local_m_map)
      {
        _M = tchecker::clockbounds::clone_map(*e._M);
      }
      
      /*!
       \brief Move constructor
       \param e : local M extrapolation
       \post e has been moved to this
       */
      local_M_extrapolation_t(tchecker::dbm::local_M_extrapolation_t<EXTRAPOLATION> && e)
      : _local_m_map(std::move(e._local_m_map))
      {
        _M = e._M;
        e._M = nullptr;
      }
      
      /*!
       \brief Destructor
       */
      virtual ~local_M_extrapolation_t()
      {
        tchecker::clockbounds::deallocate_map(_M);
      }
      
      /*!
       \brief Assignment operator
       \param e : local M extrapolation
       \post this is a copy of e
       \return this after assignment
       */
      tchecker::dbm::local_M_extrapolation_t<EXTRAPOLATION> &
      operator= (tchecker::dbm::local_M_extrapolation_t<EXTRAPOLATION> const & e)
      {
        if (this != &e) {
          _local_m_map = e._local_m_map;
          tchecker::clockbounds::deallocate_map(_M);
          _M = tchecker::clockbounds::clone_map(*e._M);
        }
        return *this;
      }
      
      /*!
       \brief Move-assignment operator
       \param e : local M extrapolation
       \post e has been moved to this
       \return this after assignment
       */
      tchecker::dbm::local_M_extrapolation_t<EXTRAPOLATION> &
      operator= (tchecker::dbm::local_M_extrapolation_t<EXTRAPOLATION> && e)
      {
        if (this != &e) {
          _local_m_map = std::move(e._local_m_map);
          _M = e._M;
          e._M = nullptr;
        }
        return *this;
      }
      
      /*!
       \brief Extrapolate a DBM
       \param dbm : a DBM
       \param dim : dimension of dbm
       \param vloc : tuple of locations
       \post dbm has been extrapolated w.r.t. local M clock bounds in vloc and EXTRAPOLATION
       */
      template <class VLOC>
      inline void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, VLOC const & vloc)
      {
        assert(dim == _local_m_map.get().clock_number());
        tchecker::clockbounds::vloc_bounds(_local_m_map.get(), vloc, *_M);
        EXTRAPOLATION(dbm, dim, _M->ptr());
      }
    private:
      std::reference_wrapper<tchecker::clockbounds::local_m_map_t const> _local_m_map;  /*!< Local M map */
      tchecker::clockbounds::map_t * _M;                                                /*!< M Map */
    };
    
    
    
    
    /*!
     \class global_LU_extrapolation_t
     \brief Zone extrapolation with global LU clock bounds
     \tparam EXTRAPOLATION : extrapolation function, uses LU clock bounds
     */
    template <extra_LU_t EXTRAPOLATION>
    class global_LU_extrapolation_t {
    public:
      /*!
       \brief Constructor
       \param model : a model
       \tparam MODEL : type of model, should inherit from tchecker::clockbounds::model_t
       \note this keeps a reference on model.global_lu_map()
       */
      template <class MODEL>
      explicit global_LU_extrapolation_t(MODEL const & model)
      : _global_lu_map(model.global_lu_map())
      {
        _L = _global_lu_map.get().L().ptr();
        _U = _global_lu_map.get().U().ptr();
      }
      
      /*!
       \brief Copy constructor
       \param e : global LU extrapolation
       \post this is a opy of e
       */
      global_LU_extrapolation_t(tchecker::dbm::global_LU_extrapolation_t<EXTRAPOLATION> const & e)
      : _global_lu_map(e._global_lu_map)
      {
        _L = _global_lu_map.get().L().ptr();
        _U = _global_lu_map.get().U().ptr();
      }
      
      /*!
       \brief Move constructor
       \param e : global LU extrapolation
       \post e has been moved to this
       */
      global_LU_extrapolation_t(tchecker::dbm::global_LU_extrapolation_t<EXTRAPOLATION>  && e)
      : _global_lu_map(std::move(e._global_lu_map))
      {
        _L = _global_lu_map.get().L().ptr();
        _U = _global_lu_map.get().U().ptr();
        e._L = nullptr;
        e._U = nullptr;
      }
      
      /*!
       \brief Destructor
       */
      virtual ~global_LU_extrapolation_t() = default;
      // DO NOT DELETE _L and _U (pointers to objects inside _global_lu_map)
      
      /*!
       \brief Assignment operator
       \param e : global LU extrapolation
       \post this is a copy of e
       \return this after assignment
       */
      tchecker::dbm::global_LU_extrapolation_t<EXTRAPOLATION> const &
      operator= (tchecker::dbm::global_LU_extrapolation_t<EXTRAPOLATION> const & e)
      {
        if (this != e) {
          _global_lu_map = e._global_lu_map;
          _L = _global_lu_map.get().L().ptr();
          _U = _global_lu_map.get().U().ptr();
        }
        return *this;
      }
      
      /*!
       \brief Move assignment operator
       \param e : global LU extrapolation
       \post e has been moved to this
       \return this after assignment
       */
      tchecker::dbm::global_LU_extrapolation_t<EXTRAPOLATION> const &
      operator= (tchecker::dbm::global_LU_extrapolation_t<EXTRAPOLATION> && e)
      {
        if (this != e) {
          _global_lu_map = std::move(e._global_lu_map);
          _L = _global_lu_map.get().L().ptr();
          _U = _global_lu_map.get().U().ptr();
          e._L = nullptr;
          e._U = nullptr;
        }
        return *this;
      }
      
      /*!
       \brief Extrapolate a DBM
       \param dbm : a DBM
       \param dim : dimension of dbm
       \param vloc : tuple of locations
       \post dbm has been extrapolated w.r.t. global LU clock bounds and EXTRAPOLATION
       */
      template <class VLOC>
      inline void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, VLOC const & vloc)
      {
        assert(dim == _global_lu_map.get().clock_number());
        EXTRAPOLATION(dbm, dim, _L, _U);
      }
    private:
      std::reference_wrapper<tchecker::clockbounds::global_lu_map_t const> _global_lu_map;  /*!< Global LU map */
      tchecker::integer_t const * _L;                                                       /*!< Direct access to L clock bounds */
      tchecker::integer_t const * _U;                                                       /*!< Direct access to U clock bounds */
    };
    
    
    
    
    /*!
     \class local_LU_extrapolation_t
     \brief Zone extrapolation with local LU clock bounds
     \tparam EXTRAPOLATION : extrapolation function, uses LU clock bounds
     */
    template <extra_LU_t EXTRAPOLATION>
    class local_LU_extrapolation_t {
    public:
      /*!
       \brief Constructor
       \param model : a model
       \tparam MODEL : type of model, should inherit from tchecker::clockbounds::model_t
       \note this keeps a reference on model.local_lu_map()
       */
      template <class MODEL>
      explicit local_LU_extrapolation_t(MODEL const & model)
      : _local_lu_map(model.local_lu_map())
      {
        _L = tchecker::clockbounds::allocate_map(_local_lu_map.get().clock_number());
        _U = tchecker::clockbounds::allocate_map(_local_lu_map.get().clock_number());
      }
      
      /*!
       \brief Copy constructor
       \param e : local LU extrapolation
       \post this is a copy of e
       */
      local_LU_extrapolation_t(tchecker::dbm::local_LU_extrapolation_t<EXTRAPOLATION> const & e)
      : _local_lu_map(e._local_lu_map)
      {
        _L = tchecker::clockbounds::clone_map(*e._L);
        _U = tchecker::clockbounds::clone_map(*e._U);
      }
      
      /*!
       \brief Move constructor
       \param e : local LU extrapolation
       \post e has been moved to this
       */
      local_LU_extrapolation_t(tchecker::dbm::local_LU_extrapolation_t<EXTRAPOLATION> && e)
      : _local_lu_map(std::move(e._local_lu_map))
      {
        _L = e._L;
        _U = e._U;
        e._L = nullptr;
        e._U = nullptr;
      }
      
      /*!
       \brief Destructor
       */
      virtual ~local_LU_extrapolation_t()
      {
        tchecker::clockbounds::deallocate_map(_U);
        tchecker::clockbounds::deallocate_map(_L);
      }
      
      /*!
       \brief Assignment operator
       \param e : local LU extrapolation
       \post this is a copy of e
       \return this after assignment
       */
      tchecker::dbm::local_LU_extrapolation_t<EXTRAPOLATION> const &
      operator= (tchecker::dbm::local_LU_extrapolation_t<EXTRAPOLATION> const & e)
      {
        if (this != &e) {
          _local_lu_map = e._local_lu_map;
          tchecker::clockbounds::deallocate_map(_U);
          tchecker::clockbounds::deallocate_map(_L);
          _L = tchecker::clockbounds::clone_map(*e._L);
          _U = tchecker::clockbounds::clone_map(*e._U);
        }
        return *this;
      }
      
      /*!
       \brief Move-assignment operator
       \param e : local LU extrapolation
       \post e has been moved to this
       \return this after assignment
       */
      tchecker::dbm::local_LU_extrapolation_t<EXTRAPOLATION> const &
      operator= (tchecker::dbm::local_LU_extrapolation_t<EXTRAPOLATION> && e)
      {
        if (this != &e) {
          _local_lu_map = std::move(e._local_lu_map);
          _L = e._L;
          _U = e._U;
          e._L = nullptr;
          e._U = nullptr;
        }
        return *this;
      }
      
      /*!
       \brief Extrapolate a DBM
       \param dbm : a DBM
       \param dim : dimension of dbm
       \param vloc : tuple of locations
       \post dbm has been extrapolated w.r.t. local LU clock bounds in vloc and EXTRAPOLATION
       */
      template <class VLOC>
      inline void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, VLOC const & vloc)
      {
        assert(dim == _local_lu_map.get().clock_number());
        tchecker::clockbounds::vloc_bounds(_local_lu_map.get(), vloc, *_L, *_U);
        EXTRAPOLATION(dbm, dim, _L->ptr(), _U->ptr());
      }
    private:
      std::reference_wrapper<tchecker::clockbounds::local_lu_map_t const> _local_lu_map;  /*!< Local LU map */
      tchecker::clockbounds::map_t * _L;                                                  /*!< L Map */
      tchecker::clockbounds::map_t * _U;                                                  /*!< U Map */
    };
    
    
    // Instances
    
    using extraM_global_t      = global_M_extrapolation_t<tchecker::dbm::extra_m>;
    using extraM_local_t       = local_M_extrapolation_t<tchecker::dbm::extra_m>;
    using extraMplus_global_t  = global_M_extrapolation_t<tchecker::dbm::extra_m_plus>;
    using extraMplus_local_t   = local_M_extrapolation_t<tchecker::dbm::extra_m_plus>;
    using extraLU_global_t     = global_LU_extrapolation_t<tchecker::dbm::extra_lu>;
    using extraLU_local_t      = local_LU_extrapolation_t<tchecker::dbm::extra_lu>;
    using extraLUplus_global_t = global_LU_extrapolation_t<tchecker::dbm::extra_lu_plus>;
    using extraLUplus_local_t  = local_LU_extrapolation_t<tchecker::dbm::extra_lu_plus>;
    
    
    
    /*!
     \class no_extrapolation_t
     \brief No extrapolation
     */
    class no_extrapolation_t {
    public:
      /*!
       \brief Constructor
       \param model : a model
       \tparam MODEL : type of model
       */
      template <class MODEL>
      explicit no_extrapolation_t(MODEL const & model) {}
      
      /*!
       \brief Copy constructor
       \param e : extrapolation
       \post this is a copy of e
       */
      no_extrapolation_t(tchecker::dbm::no_extrapolation_t const &) = default;
      
      /*!
       \brief Move constructor
       \param e : extrapolation
       \post e has been moved to this
       */
      no_extrapolation_t(tchecker::dbm::no_extrapolation_t &&) = default;
      
      /*!
       \brief Extrapolate a DBM
       \post Does nothing
       */
      template <class VLOC>
      inline void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, VLOC const & vloc) {}
    };
    
  } // end of namespace dbm
  
} // end of namespace tchecker

#endif // TCHECKER_ZONE_DBM_EXTRAPOLATION_HH
