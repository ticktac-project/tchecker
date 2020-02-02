/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZONE_OFFSET_DBM_SEMANTICS_HH
#define TCHECKER_ZONE_OFFSET_DBM_SEMANTICS_HH

#include <cassert>
#include <cstring>
#include <functional>

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/model.hh"
#include "tchecker/dbm/offset_dbm.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/zone/dbm/extrapolation.hh"
#include "tchecker/zone/dbm/zone.hh"
#include "tchecker/zone/offset_dbm/zone.hh"

/*!
 \file semantics.hh
 \brief Operational semantics on offset DBM zones
 */

namespace tchecker {
  
  namespace offset_dbm {
    
    namespace details {
      
      /*!
       \brief Constrain an offset DBM with clock constraints
       \param dbm : an offset DBM
       \param dim : dimension of dbm
       \param constraints : container of variable constraints
       \pre dbm is not nullptr (checked by assertion)
       dbm is not empty (checked by assertion)
       dbm is tight (checked by assertion)
       dbm is a dim*dim array of difference bounds
       every clock in constraints belongs to 0..dim-1.
       \post dbm has been intersected with constraints.
       dbm is tight if it is not empty.
       \return true is dbm is not empty after intersection with constraints, false otherwise
       */
      bool constrain(tchecker::dbm::db_t * dbm,
                     tchecker::clock_id_t dim,
                     tchecker::clock_constraint_container_t const & constraints);
      
      
      /*!
       \brief Reset clocks on an offset DBM
       \param dbm : an offset DBM
       \param dim : dimension of dbm
       \param refcount : number of reference clocks in dbm
       \param refmap : map to reference clocks
       \param resets : container of clock resets
       \pre dbm is not nullptr (checked by assertion)
       dbm is not empty (checked by assertion)
       dbm is tight (checked by assertion)
       dbm is a dim*dim array of difference bounds
       1 <= refcount <= dim (checked by assertion)
       refmap is not nullptr (checked by assertion)
       every clock in resets belongs to 0..dim-1 (checked by assertion)
       resets only reset to reference clock (checked by assertion)
       \post dbm has been updated w.r.t. every clock reset in resets
       dbm is tight
       \throw std::invalid_argument : if resets contains a reset x := y + c where c != 0 or y is not the refeence clock
       of x
       */
      void reset(tchecker::dbm::db_t * dbm,
                 tchecker::clock_id_t dim,
                 tchecker::clock_id_t refcount,
                 tchecker::clock_id_t const * refmap,
                 tchecker::clock_reset_container_t const & resets);
      
      
      /*!
       \class sync_zone_computer_t
       \brief Compute the zone of synchronized valuations within an offset zone
       */
      class sync_zone_computer_t {
      public:
        /*!
         \brief Constructor
         \param offset_dim : dimension of offset DBMs
         \param refcount : number of reference clocks in dbm
         \param refmap : map to reference clocks
         */
        sync_zone_computer_t(tchecker::clock_id_t offset_dim, tchecker::clock_id_t refcount,
                             tchecker::clock_id_t const * refmap);
        
        /*!
         \brief Copy constructor
         */
        sync_zone_computer_t(tchecker::offset_dbm::details::sync_zone_computer_t const &);
        
        /*!
         \brief Move constructor
         */
        sync_zone_computer_t(tchecker::offset_dbm::details::sync_zone_computer_t &&);
        
        /*!
         \brief Destructor
         */
        ~sync_zone_computer_t();
        
        /*!
         \brief Assignment operator
         */
        tchecker::offset_dbm::details::sync_zone_computer_t &
        operator= (tchecker::offset_dbm::details::sync_zone_computer_t const &);
        
        /*!
         \brief Move-assignment operator
         */
        tchecker::offset_dbm::details::sync_zone_computer_t &
        operator= (tchecker::offset_dbm::details::sync_zone_computer_t &&);
        
        /*!
         \brief Compute the DBM of synchronized valuations in an offset DBM
         \param offset_dbm : an offset DBM
         \param offset_dim : dimension of offset_dbm
         \param dbm : a DBM
         \param dim : dimension of dbm
         \pre offset_dim is equal to _offset_dim (checked by assertion)
         dim is equal to _offset_dim - _refcount + 1 (checked by assertion)
         \post dbm is the set of synchronized valuations in offset_dbm
         \return tchecker::dbm::EMPTY is dbm is empty, tchecker::dbm::NON_EMPTY otherwise
         */
        enum tchecker::dbm::status_t sync_zone(tchecker::dbm::db_t const * const offset_dbm,
                                               tchecker::clock_id_t offset_dim,
                                               tchecker::dbm::db_t * dbm,
                                               tchecker::clock_id_t dim);
      private:
        tchecker::dbm::db_t * _offset_dbm;    /*!< Offset DBM (for computations) */
        tchecker::clock_id_t _offset_dim;     /*!< Dimension of _offset_dbm */
        std::size_t _refcount;                /*!< Number of reference variables */
        tchecker::clock_id_t * _refmap;       /*!< Map from variables to reference clocks */
      };
      
    } // end of namespace details
    
    
    
    
    /*!
     \class elapsed_semantics_t
     \brief Elapsed zone semantics for offset DBMs
     \tparam EXTRAPOLATION : a zone DBM extrapolation, should implement tchecker::dbm::extrapolation_t
     */
    template <class EXTRAPOLATION>
    class elapsed_semantics_t : private EXTRAPOLATION {
    public:
      /*!
       \brief Type of offet zones
       */
      using offset_zone_t = tchecker::offset_dbm::zone_t;
      
      /*!
       \brief Type of synchronized zones
       */
      using sync_zone_t = tchecker::dbm::zone_t;
      
      /*!
       \brief Constructor
       \param model : a model
       \tparam MODEL : type of model, should inherit from tchecker::async_zg::details::model_t
       */
      template <class MODEL>
      explicit elapsed_semantics_t(MODEL const & model)
      : EXTRAPOLATION(model),
      _offset_dim(model.flattened_offset_clock_variables().size()),
      _refcount(model.flattened_offset_clock_variables().refcount()),
      _refmap(nullptr),
      _sync_zone_computer(nullptr)
      {
        _refmap = new tchecker::clock_id_t[_offset_dim];
        std::memcpy(_refmap, model.flattened_offset_clock_variables().refmap(), _offset_dim * sizeof(*_refmap));
        _sync_zone_computer = new tchecker::offset_dbm::details::sync_zone_computer_t(_offset_dim, _refcount,
                                                                                      _refmap);
      }
      
      /*!
       \brief Copy constructor
       \param zs : zone semantics
       \post this is a copy of s
       */
      elapsed_semantics_t(tchecker::offset_dbm::elapsed_semantics_t<EXTRAPOLATION> const & zs)
      : EXTRAPOLATION(zs), _offset_dim(zs._offset_dim), _refcount(zs._refcount), _refmap(nullptr)
      {
        _refmap = new tchecker::clock_id_t[_offset_dim];
        std::memcpy(_refmap, zs._refmap, _offset_dim * sizeof(*_refmap));
        _sync_zone_computer = new tchecker::offset_dbm::details::sync_zone_computer_t(_offset_dim, _refcount,
                                                                                      _refmap);
      }
      
      /*!
       \brief Move constructor
       \param zs : zone semantics
       \post zs has been moved to this
       */
      elapsed_semantics_t(tchecker::offset_dbm::elapsed_semantics_t<EXTRAPOLATION> && zs)
      : EXTRAPOLATION(std::move(zs)), _offset_dim(zs._offset_dim), _refcount(zs._refcount), _refmap(zs._refmap),
      _sync_zone_computer(zs._sync_zone_computer)
      {
        zs._refmap = nullptr;
        zs._sync_zone_computer = nullptr;
      }
      
      /*!
       \brief Destructor
       */
      ~elapsed_semantics_t()
      {
        delete[] _refmap;
        delete _sync_zone_computer;
      }
      
      /*!
       \brief Assignment operator
       \param zs : zone semantics
       \post this is a copy of zs
       \return this after assigment
       */
      tchecker::offset_dbm::elapsed_semantics_t<EXTRAPOLATION> &
      operator= (tchecker::offset_dbm::elapsed_semantics_t<EXTRAPOLATION> const & zs)
      {
        if (this != &zs) {
          EXTRAPOLATION::operator=(zs);
          _offset_dim = zs._offset_dim;
          _refcount = zs._refcount;
          delete[] _refmap;
          _refmap = new tchecker::clock_id_t[_offset_dim];
          std::memcpy(_refmap, zs._refmap, _offset_dim * sizeof(*_refmap));
          delete[] _sync_zone_computer;
          _sync_zone_computer = new tchecker::offset_dbm::details::sync_zone_computer_t(_offset_dim, _refcount,
                                                                                        _refmap);
        }
        return *this;
      }
      
      /*!
       \brief Move-assignment operator
       \param zs : zone semantics
       \post zs has been moved to this
       \return this after assigment
       */
      tchecker::offset_dbm::elapsed_semantics_t<EXTRAPOLATION> &
      operator= (tchecker::offset_dbm::elapsed_semantics_t<EXTRAPOLATION> && zs)
      {
        if (this != &zs) {
          EXTRAPOLATION::operator=(std::move(zs));
          _offset_dim = zs._offset_dim;
          _refcount = zs._refcount;
          delete[] _refmap;
          _refmap = zs._refmap;
          zs._refmap = nullptr;
          delete[] _sync_zone_computer;
          _sync_zone_computer = zs._sync_zone_computer;
          zs._sync_zone_computer = nullptr;
        }
        return *this;
      }
      
      /*!
       \brief Compute initial zone
       \param offset_zone : an offset zone
       \param delay_allowed : bit vector telling which process is allowed to delay
       \param invariant : invariant
       \param vloc : tuple of initial locations
       \pre invariant is on offset variables, offset_zone dimension == _offset_dim (checked by assertion)
       delay_allowed and vloc have the same size (checked by assertion)
       \post offset_zone is the zero zone elapsed (for processes allowed to delay), then intersected with invariant.
       \return STATE_OK if the resulting zones are not empty, and STATE_CLOCKS_SRC_INVARIANT_VIOLATED
       if invariant does not hold in the initial zone
       */
      template <class VLOC>
      enum tchecker::state_status_t initialize(tchecker::offset_dbm::zone_t & offset_zone,
                                               boost::dynamic_bitset<> const & delay_allowed,
                                               tchecker::clock_constraint_container_t const & invariant,
                                               VLOC const & vloc)
      {
        tchecker::dbm::db_t * offset_dbm = offset_zone.dbm();
        auto offset_dim = offset_zone.dim();
        
        assert(offset_dim == _offset_dim);
        
        tchecker::offset_dbm::zero(offset_dbm, offset_dim);
        
        if (tchecker::offset_dbm::details::constrain(offset_dbm, offset_dim, invariant) == tchecker::dbm::EMPTY)
          return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;
        
        tchecker::offset_dbm::asynchronous_open_up(offset_dbm, offset_dim, _refcount, delay_allowed);
        
        if (tchecker::offset_dbm::details::constrain(offset_dbm, offset_dim, invariant) == tchecker::dbm::EMPTY)
          return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;
        
        return tchecker::STATE_OK;
      }
      
      /*!
       \brief Compute initial zone
       \param offset_zone : an offset zone
       \param sync_zone : a zone (of synchronized valuations)
       \param delay_allowed : bit vector telling which process is allowed to delay
       \param invariant : invariant
       \param vloc : tuple of initial locations
       \pre invariant is on offset variables.
       offset_zone dimension == _offset_dim (checked by assertion)
       offset_zone and sync_zone have compatible dimensions (checked by assertion)
       delay_allowed and vloc have the same size (checked by assertion)
       \post offset_zone is the zero zone elapsed (for processes allowed to delay)), then intersected with invariant.
       sync_zone is the set of synchronized valuations in offset_zone
       \return STATE_EMPTY_ZONE if the resulting sync_zone is empty, same as
       tchecker::offset_dbm::elapsed_semantics_t::initialize otherwise
       */
      template <class VLOC>
      enum tchecker::state_status_t initialize(tchecker::offset_dbm::zone_t & offset_zone,
                                               tchecker::dbm::zone_t & sync_zone,
                                               boost::dynamic_bitset<> const & delay_allowed,
                                               tchecker::clock_constraint_container_t const & invariant,
                                               VLOC const & vloc)
      {
        assert( _offset_dim == offset_zone.dim() );
        assert( sync_zone.dim() == offset_zone.dim() - _refcount + 1 );
        assert( vloc.size() == delay_allowed.size() );
        
        enum tchecker::state_status_t status = initialize(offset_zone, delay_allowed, invariant, vloc);
        if (status != tchecker::STATE_OK)
          return status;
        
        tchecker::dbm::db_t * sync_dbm = sync_zone.dbm();
        auto sync_dim = sync_zone.dim();
        
        if (_sync_zone_computer->sync_zone(offset_zone.dbm(), offset_zone.dim(), sync_dbm, sync_dim)
            == tchecker::dbm::EMPTY)
          return tchecker::STATE_EMPTY_ZONE;
        
        EXTRAPOLATION::extrapolate(sync_dbm, sync_dim, vloc);
        
        return tchecker::STATE_OK;
      }
      
      /*!
       \brief Compute next zone
       \param offset_zone : an offset zone
       \param src_delay_allowed : bit vector telling which process is allowed to delay in source state
       \param src_invariant : invariant in source state
       \param guard : transition guard
       \param clkreset : transition reset
       \param tgt_delay_allowed : bit vector telling which process is allowed to delay in target state
       \param tgt_invariant : invariant in target state
       \param tgt_vloc : tuple of locations in target state
       \pre src_invariant, guard and tgt_invariant are on offset variables.
       clkreset is on offset variables and it only contains resets of variables to
       the corresponding reference clock.
       offset_zone dimension == _offset_dim
       srd_delay_allowed and tgt_delay_allowed have the same size as tgt_vloc (checked by assertion)
       \post zone has been updated to:
       delay((zone \cap guard)[clkreset] \cap tgt_invariant) \cap tgt_invariant
       where delay is applied only to processes which are allowed to delay in target state
       \return STATE_OK if the resulting zone is not empty, STATE_CLOCKS_GUARD_VIOLATED
       if guard does not hold in zone, and STATE_CLOCKS_TGT_INVARIANT_VIOLATED does not
       hold in (zone \cap guatd)[clkreset] or in delay((zone \cap guard)[clkreset])
       */
      template <class VLOC>
      enum tchecker::state_status_t next(tchecker::offset_dbm::zone_t & offset_zone,
                                         boost::dynamic_bitset<> const & src_delay_allowed,
                                         tchecker::clock_constraint_container_t const & src_invariant,
                                         tchecker::clock_constraint_container_t const & guard,
                                         tchecker::clock_reset_container_t const & clkreset,
                                         boost::dynamic_bitset<> const & tgt_delay_allowed,
                                         tchecker::clock_constraint_container_t const & tgt_invariant,
                                         VLOC const & tgt_vloc)
      {
        assert( src_delay_allowed.size() == tgt_delay_allowed.size() );
        assert( tgt_delay_allowed.size() == tgt_vloc.size() );
        
        tchecker::dbm::db_t * offset_dbm = offset_zone.dbm();
        auto offset_dim = offset_zone.dim();
        
        assert(offset_dim == _offset_dim);
        
        if (tchecker::offset_dbm::details::constrain(offset_dbm, offset_dim, guard) == tchecker::dbm::EMPTY)
          return tchecker::STATE_CLOCKS_GUARD_VIOLATED;
        
        tchecker::offset_dbm::details::reset(offset_dbm, offset_dim, _refcount, _refmap, clkreset);
        
        if (tchecker::offset_dbm::details::constrain(offset_dbm, offset_dim, tgt_invariant)
            == tchecker::dbm::EMPTY)
          return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;
        
        tchecker::offset_dbm::asynchronous_open_up(offset_dbm, offset_dim, _refcount, tgt_delay_allowed);
        
        if (tchecker::offset_dbm::details::constrain(offset_dbm, offset_dim, tgt_invariant)
            == tchecker::dbm::EMPTY)
          return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;
        
        return tchecker::STATE_OK;
      }
      
      /*!
       \brief Compute next zone
       \param offset_zone : an offset zone
       \param sync_zone : a zone (of synchronized valuations)
       \param src_delay_allowed : bit vector telling which process is allowed to delay in source state
       \param src_invariant : invariant in source state
       \param guard : transition guard
       \param clkreset : transition reset
       \param tgt_delay_allowed : bit vector telling which process is allowed to delay in target state
       \param tgt_invariant : invariant in target state
       \param tgt_vloc : tuple of locations in target state
       \pre src_invariant, guard and tgt_invariant are on offset variables.
       clkreset is on offset variables and it only contains resets of variables to
       the corresponding reference clock.
       offset_zone dimension == _offset_dim
       offset_zone and sync_zone have compatible dimensions (checked by assertion)
       src_delay_allowed and tgt_delay_allowed have same size as tgt_vloc (checked by assertion)
       \post zone has been updated to:
       delay((zone \cap guard)[clkreset] \cap tgt_invariant) \cap tgt_invariant
       where delay is applied only to processes that are allowed to delay in target state
       sync_zone is the set of synchronized valuations in offset_zone
       \return STATE_EMPTY_ZONE if the resulting sync_zone is empty, same as
       tchecker::offset_dbm::elapsed_semantics_t::next otherwise
       */
      template <class VLOC>
      enum tchecker::state_status_t next(tchecker::offset_dbm::zone_t & offset_zone,
                                         tchecker::dbm::zone_t & sync_zone,
                                         boost::dynamic_bitset<> const & src_delay_allowed,
                                         tchecker::clock_constraint_container_t const & src_invariant,
                                         tchecker::clock_constraint_container_t const & guard,
                                         tchecker::clock_reset_container_t const & clkreset,
                                         boost::dynamic_bitset<> const & tgt_delay_allowed,
                                         tchecker::clock_constraint_container_t const & tgt_invariant,
                                         VLOC const & tgt_vloc)
      {
        assert( offset_zone.dim() == _offset_dim );
        assert( sync_zone.dim() == offset_zone.dim() - _refcount + 1 );
        assert( src_delay_allowed.size() == tgt_delay_allowed.size() );
        assert( tgt_delay_allowed.size() == tgt_vloc.size() );
        
        enum tchecker::state_status_t status = next(offset_zone, src_delay_allowed, src_invariant, guard,
                                                    clkreset, tgt_delay_allowed, tgt_invariant, tgt_vloc);
        
        if (status != tchecker::STATE_OK)
          return status;
        
        tchecker::dbm::db_t * sync_dbm = sync_zone.dbm();
        auto sync_dim = sync_zone.dim();
        
        if (_sync_zone_computer->sync_zone(offset_zone.dbm(), offset_zone.dim(), sync_dbm, sync_dim)
            == tchecker::dbm::EMPTY)
          return tchecker::STATE_EMPTY_ZONE;
        
        EXTRAPOLATION::extrapolate(sync_dbm, sync_dim, tgt_vloc);
        
        return tchecker::STATE_OK;
      }
    private:
      tchecker::clock_id_t _offset_dim;                                          /*!< Dimension of offset zones */
      tchecker::clock_id_t _refcount;                                            /*!< Number of reference clocks */
      tchecker::clock_id_t * _refmap;                                            /*!< Map from variables to reference clock */
      tchecker::offset_dbm::details::sync_zone_computer_t * _sync_zone_computer; /*!< Synchronized zone computer */
    };
    
    
    
    
    /*!
     \class non_elapsed_semantics_t
     \brief Non elapsed zone semantics for offset DBMs
     \tparam EXTRAPOLATION : a zone DBM extrapolation, should implement tchecker::dbm::extrapolation_t
     */
    template <class EXTRAPOLATION>
    class non_elapsed_semantics_t : private EXTRAPOLATION {
    public:
      /*!
       \brief Type of offet zones
       */
      using offset_zone_t = tchecker::offset_dbm::zone_t;
      
      /*!
       \brief Type of synchronized zones
       */
      using sync_zone_t = tchecker::dbm::zone_t;
      
      /*!
       \brief Constructor
       \param model : a model
       \tparam MODEL : type of model, should inherit from tchecker::async_zg::details::model_t
       */
      template <class MODEL>
      explicit non_elapsed_semantics_t(MODEL const & model)
      : EXTRAPOLATION(model),
      _offset_dim(model.flattened_offset_clock_variables().size()),
      _refcount(model.flattened_offset_clock_variables().refcount()),
      _refmap(nullptr),
      _sync_zone_computer(nullptr)
      {
        _refmap = new tchecker::clock_id_t[_offset_dim];
        std::memcpy(_refmap, model.flattened_offset_clock_variables().refmap(), _offset_dim * sizeof(*_refmap));
        _sync_zone_computer = new tchecker::offset_dbm::details::sync_zone_computer_t(_offset_dim, _refcount,
                                                                                      _refmap);
      }
      
      /*!
       \brief Copy constructor
       \param zs : zone semantics
       \post this is a copy of s
       */
      non_elapsed_semantics_t(tchecker::offset_dbm::non_elapsed_semantics_t<EXTRAPOLATION> const & zs)
      : EXTRAPOLATION(zs), _offset_dim(zs._offset_dim), _refcount(zs._refcount), _refmap(nullptr),
      _sync_zone_computer(nullptr)
      {
        _refmap = new tchecker::clock_id_t[_offset_dim];
        std::memcpy(_refmap, zs._refmap, _offset_dim * sizeof(*_refmap));
        _sync_zone_computer = new tchecker::offset_dbm::details::sync_zone_computer_t(_offset_dim, _refcount,
                                                                                      _refmap);
      }
      
      /*!
       \brief Move constructor
       \param zs : zone semantics
       \post zs has been moved to this
       */
      non_elapsed_semantics_t(tchecker::offset_dbm::non_elapsed_semantics_t<EXTRAPOLATION> && zs)
      : EXTRAPOLATION(std::move(zs)), _offset_dim(zs._offset_dim), _refcount(zs._refcount), _refmap(zs._refmap),
      _sync_zone_computer(zs._sync_zone_computer)
      {
        zs._refmap = nullptr;
        zs._sync_zone_computer = nullptr;
      }
      
      /*!
       \brief Destructor
       */
      ~non_elapsed_semantics_t()
      {
        delete[] _refmap;
        delete _sync_zone_computer;
      }
      
      /*!
       \brief Assignment operator
       \param zs : zone semantics
       \post this is a copy of zs
       \return this after assigment
       */
      tchecker::offset_dbm::non_elapsed_semantics_t<EXTRAPOLATION> &
      operator= (tchecker::offset_dbm::non_elapsed_semantics_t<EXTRAPOLATION> const & zs)
      {
        if (this != &zs) {
          EXTRAPOLATION::operator=(zs);
          _offset_dim = zs._offset_dim;
          _refcount = zs._refcount;
          delete[] _refmap;
          _refmap = new tchecker::clock_id_t[_offset_dim];
          std::memcpy(_refmap, zs._refmap, _offset_dim * sizeof(*_refmap));
          delete[] _sync_zone_computer;
          _sync_zone_computer = new tchecker::offset_dbm::details::sync_zone_computer_t(_offset_dim, _refcount,
                                                                                        _refmap);
        }
        return *this;
      }
      
      /*!
       \brief Move-assignment operator
       \param zs : zone semantics
       \post zs has been moved to this
       \return this after assigment
       */
      tchecker::offset_dbm::non_elapsed_semantics_t<EXTRAPOLATION> &
      operator= (tchecker::offset_dbm::non_elapsed_semantics_t<EXTRAPOLATION> && zs)
      {
        if (this != &zs) {
          EXTRAPOLATION::operator=(std::move(zs));
          _offset_dim = zs._offset_dim;
          _refcount = zs._refcount;
          delete[] _refmap;
          _refmap = zs._refmap;
          zs._refmap = nullptr;
          delete[] _sync_zone_computer;
          _sync_zone_computer = zs._sync_zone_computer;
          zs._sync_zone_computer = nullptr;
        }
        return *this;
      }
      
      /*!
       \brief Compute initial zone
       \param offset_zone : an offset zone
       \param delay_allowed : bit vector telling which process is allowed to delay
       \param invariant : invariant
       \param vloc : tuple of initial locations
       \pre invariant is on offset variables.
       offset_zone dimension == _ofsset_dim (checked by assertion)
       delay_allowed has same size as vloc (checked by assertion)
       \post offset_zone is the zero zone intersected with invariant
       \return STATE_OK if the resulting zones are not empty, and STATE_CLOCKS_SRC_INVARIANT_VIOLATED
       if invariant does not hold in the initial zones
       */
      template <class VLOC>
      enum tchecker::state_status_t initialize(tchecker::offset_dbm::zone_t & offset_zone,
                                               boost::dynamic_bitset<> const & delay_allowed,
                                               tchecker::clock_constraint_container_t const & invariant,
                                               VLOC const & vloc)
      {
        assert( delay_allowed.size() == vloc.size() );
        
        tchecker::dbm::db_t * offset_dbm = offset_zone.dbm();
        auto offset_dim = offset_zone.dim();
        
        assert(offset_dim == _offset_dim);
        
        tchecker::offset_dbm::zero(offset_dbm, offset_dim);
        
        if (tchecker::offset_dbm::details::constrain(offset_dbm, offset_dim, invariant) == tchecker::dbm::EMPTY)
          return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;
        
        return tchecker::STATE_OK;
      }
      
      /*!
       \brief Compute initial zone
       \param offset_zone : an offset zone
       \param sync_zone : a zone (of synchronized valuations)
       \param delay_allowed : bit vector telling which process is allowed to delay
       \param invariant : invariant
       \param vloc : tuple of initial locations
       \pre invariant is on offset variables.
       offset_zone dimension == _offset_dim (checked by assertion)
       offset_zone and sync_zone have compatible dimensions (checked by assertion)
       delay_allowed has same size as vloc (checked by assertion)
       \post offset_zone is the zero zone intersected with invariant.
       sync_zone is the set of synchronized valuations in offset_zone
       \return STATE_EMPTY_ZONE if the resulting sync_zone is empty, same as
       tchecker::offset_dbm::elapsed_semantics_t::initialize otherwise
       */
      template <class VLOC>
      enum tchecker::state_status_t initialize(tchecker::offset_dbm::zone_t & offset_zone,
                                               tchecker::dbm::zone_t & sync_zone,
                                               boost::dynamic_bitset<> const & delay_allowed,
                                               tchecker::clock_constraint_container_t const & invariant,
                                               VLOC const & vloc)
      {
        assert( offset_zone.dim() == _offset_dim );
        assert( sync_zone.dim() == offset_zone.dim() - _refcount + 1 );
        assert( delay_allowed.size() == vloc.size() );
        
        enum tchecker::state_status_t status = initialize(offset_zone, delay_allowed, invariant, vloc);
        if (status != tchecker::STATE_OK)
          return status;
        
        tchecker::dbm::db_t * sync_dbm = sync_zone.dbm();
        auto sync_dim = sync_zone.dim();
        
        if (_sync_zone_computer->sync_zone(offset_zone.dbm(), offset_zone.dim(), sync_dbm, sync_dim)
            == tchecker::dbm::EMPTY)
          return tchecker::STATE_EMPTY_ZONE;
        
        EXTRAPOLATION::extrapolate(sync_dbm, sync_dim, vloc);
        
        return tchecker::STATE_OK;
      }
      
      /*!
       \brief Compute next zone
       \param offset_zone : an offset zone
       \param src_delay_allowed : bit vector telling which process is allowed to delay in source state
       \param src_invariant : invariant in source state
       \param guard : transition guard
       \param clkreset : transition reset
       \param tgt_delay_allowed : bit vector telling which process is allowed to delay in target state
       \param tgt_invariant : invariant in target state
       \param tgt_vloc : tuple of locations in target state
       \pre zone satisfies the invariant in the source state (guaranteed if zone
       was returned by this class).
       src_invariant, guard and tgt_invariant are on offset variables.
       clkreset is on offset variables and it only contains resets of variables to
       the corresponding reference clock.
       offset_zone dimension == _offset_dim (checked by assertion)
       src_delay_allowed and tgt_delay_allowed have same size as tgt_vloc (checked by assertion)
       \post zone has been updated to
       (delay(zone) \cap src_invariant \cap guard)[clkreset] \cap tgt_invariant
       where delay is applied only to proceses allowed to delay in source state
       \return STATE_OK if the resulting zones are not empty, STATE_CLOCKS_SRC_INVARIANT_VIOLATED
       if src_invariant does bot hold in delay(zone) (should never occur thanks due to precond)
       STATE_CLOCKS_GUARD_VIOLATED if guard does not hold in (delay(zone) \cap src_invariant),
       and STATE_CLOCKS_TGT_INVARIANT_VIOLATED does not hold in
       (delay(zone) \cap src_invariant \cap guard)[clkreset]
       */
      template <class VLOC>
      enum tchecker::state_status_t next(tchecker::offset_dbm::zone_t & offset_zone,
                                         boost::dynamic_bitset<> const & src_delay_allowed,
                                         tchecker::clock_constraint_container_t const & src_invariant,
                                         tchecker::clock_constraint_container_t const & guard,
                                         tchecker::clock_reset_container_t const & clkreset,
                                         boost::dynamic_bitset<> const & tgt_delay_allowed,
                                         tchecker::clock_constraint_container_t const & tgt_invariant,
                                         VLOC const & tgt_vloc)
      {
        assert( src_delay_allowed.size() == tgt_delay_allowed.size() );
        assert( tgt_delay_allowed.size() == tgt_vloc.size() );
        
        tchecker::dbm::db_t * offset_dbm = offset_zone.dbm();
        auto offset_dim = offset_zone.dim();
        
        assert(offset_dim == _offset_dim);
        
        tchecker::offset_dbm::asynchronous_open_up(offset_dbm, offset_dim, _refcount, src_delay_allowed);
        
        if (tchecker::offset_dbm::details::constrain(offset_dbm, offset_dim, src_invariant)
            == tchecker::dbm::EMPTY)
          return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;  // should never occur
        
        if (tchecker::offset_dbm::details::constrain(offset_dbm, offset_dim, guard) == tchecker::dbm::EMPTY)
          return tchecker::STATE_CLOCKS_GUARD_VIOLATED;
        
        tchecker::offset_dbm::details::reset(offset_dbm, offset_dim, _refcount, _refmap, clkreset);
        
        if (tchecker::offset_dbm::details::constrain(offset_dbm, offset_dim, tgt_invariant)
            == tchecker::dbm::EMPTY)
          return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;
        
        return tchecker::STATE_OK;
      }
      
      /*!
       \brief Compute next zone
       \param offset_zone : an offset zone
       \param sync_zone : a zone (of synchronized valuations)
       \param src_delay_allowed : bit vector telling which process is allowed to delay in source state
       \param src_invariant : invariant in source state
       \param guard : transition guard
       \param clkreset : transition reset
       \param tgt_delay_allowed : bit vector telling which process is allowed to delay in target state
       \param tgt_invariant : invariant in target state
       \param tgt_vloc : tuple of locations in target state
       \pre zone satisfies the invariant in the source state (guaranteed if zone
       was returned by this class).
       src_invariant, guard and tgt_invariant are on offset variables.
       clkreset is on offset variables and it only contains resets of variables to
       the corresponding reference clock.
       offset_zone dimension == _offset_dim (checked by assertion)
       offset_zone and sync_zone have compatible dimensions (checked by assertion)
       src_delay_allowed and tgt_delay_allowed have same size as tgt_vloc (checked by assertion)
       \post zone has been updated to
       (delay(zone) \cap src_invariant \cap guard)[clkreset] \cap tgt_invariant
       where delay is applied only to processes allowed to delay in source state
       sync_zone is the set of synchronized valuations in offset_zone
       \return STATE_EMPTY_ZONE if the resulting sync_zone is empty, same as
       tchecker::offset_dbm::elapsed_semantics_t::initialize otherwise
       */
      template <class VLOC>
      enum tchecker::state_status_t next(tchecker::offset_dbm::zone_t & offset_zone,
                                         tchecker::dbm::zone_t & sync_zone,
                                         boost::dynamic_bitset<> const & src_delay_allowed,
                                         tchecker::clock_constraint_container_t const & src_invariant,
                                         tchecker::clock_constraint_container_t const & guard,
                                         tchecker::clock_reset_container_t const & clkreset,
                                         boost::dynamic_bitset<> const & tgt_delay_allowed,
                                         tchecker::clock_constraint_container_t const & tgt_invariant,
                                         VLOC const & tgt_vloc)
      {
        assert( offset_zone.dim() == _offset_dim );
        assert( sync_zone.dim() == offset_zone.dim() - _refcount + 1 );
        assert( src_delay_allowed.size() == tgt_delay_allowed.size() );
        assert( tgt_delay_allowed.size() == tgt_vloc.size() );
        
        enum tchecker::state_status_t status = next(offset_zone, src_delay_allowed, src_invariant, guard,
                                                    clkreset, tgt_delay_allowed, tgt_invariant, tgt_vloc);
        
        if (status != tchecker::STATE_OK)
          return status;
        
        tchecker::dbm::db_t * sync_dbm = sync_zone.dbm();
        auto sync_dim = sync_zone.dim();
        
        if (_sync_zone_computer->sync_zone(offset_zone.dbm(), offset_zone.dim(), sync_dbm, sync_dim)
            == tchecker::dbm::EMPTY)
          return tchecker::STATE_EMPTY_ZONE;
        
        EXTRAPOLATION::extrapolate(sync_dbm, sync_dim, tgt_vloc);
        
        return tchecker::STATE_OK;
      }
    private:
      tchecker::clock_id_t _offset_dim;                                           /*!< Dimension of offset zones */
      tchecker::clock_id_t _refcount;                                             /*!< Number of reference clocks */
      tchecker::clock_id_t * _refmap;                                             /*!< Map from variables to reference clock */
      tchecker::offset_dbm::details::sync_zone_computer_t * _sync_zone_computer;  /*!< Synchronized zone computer */
    };
    
    
    
    
    // Instances
    
    using elapsed_no_extrapolation_t = tchecker::offset_dbm::elapsed_semantics_t<tchecker::dbm::no_extrapolation_t>;
    using elapsed_extraLU_global_t = tchecker::offset_dbm::elapsed_semantics_t<tchecker::dbm::extraLU_global_t>;
    using elapsed_extraLU_local_t = tchecker::offset_dbm::elapsed_semantics_t<tchecker::dbm::extraLU_local_t>;
    using elapsed_extraLUplus_global_t = tchecker::offset_dbm::elapsed_semantics_t<tchecker::dbm::extraLUplus_global_t>;
    using elapsed_extraLUplus_local_t = tchecker::offset_dbm::elapsed_semantics_t<tchecker::dbm::extraLUplus_local_t>;
    using elapsed_extraM_global_t = tchecker::offset_dbm::elapsed_semantics_t<tchecker::dbm::extraM_global_t>;
    using elapsed_extraM_local_t = tchecker::offset_dbm::elapsed_semantics_t<tchecker::dbm::extraM_local_t>;
    using elapsed_extraMplus_global_t = tchecker::offset_dbm::elapsed_semantics_t<tchecker::dbm::extraMplus_global_t>;
    using elapsed_extraMplus_local_t = tchecker::offset_dbm::elapsed_semantics_t<tchecker::dbm::extraMplus_local_t>;
    using non_elapsed_no_extrapolation_t = tchecker::offset_dbm::non_elapsed_semantics_t<tchecker::dbm::no_extrapolation_t>;
    using non_elapsed_extraLU_global_t = tchecker::offset_dbm::non_elapsed_semantics_t<tchecker::dbm::extraLU_global_t>;
    using non_elapsed_extraLU_local_t = tchecker::offset_dbm::non_elapsed_semantics_t<tchecker::dbm::extraLU_local_t>;
    using non_elapsed_extraLUplus_global_t = tchecker::offset_dbm::non_elapsed_semantics_t<tchecker::dbm::extraLUplus_global_t>;
    using non_elapsed_extraLUplus_local_t = tchecker::offset_dbm::non_elapsed_semantics_t<tchecker::dbm::extraLUplus_local_t>;
    using non_elapsed_extraM_global_t = tchecker::offset_dbm::non_elapsed_semantics_t<tchecker::dbm::extraM_global_t>;
    using non_elapsed_extraM_local_t = tchecker::offset_dbm::non_elapsed_semantics_t<tchecker::dbm::extraM_local_t>;
    using non_elapsed_extraMplus_global_t = tchecker::offset_dbm::non_elapsed_semantics_t<tchecker::dbm::extraMplus_global_t>;
    using non_elapsed_extraMplus_local_t = tchecker::offset_dbm::non_elapsed_semantics_t<tchecker::dbm::extraMplus_local_t>;
    
  } // end of namespace offset_dbm
  
} // end of namespace tchecker

#endif // TCHECKER_ZONE_OFFSET_DBM_SEMANTICS_HH


