/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZONE_DBM_SEMANTICS_HH
#define TCHECKER_ZONE_DBM_SEMANTICS_HH

#include <cstring>

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/model.hh"
#include "tchecker/dbm/dbm.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/zone/dbm/extrapolation.hh"
#include "tchecker/zone/dbm/zone.hh"

/*!
 \file semantics.hh
 \brief Operational semantics on DBM zones
 */

namespace tchecker {
  
  namespace dbm {
    
    namespace details {
      
      /*!
       \brief Constrain a DBM with clock constraints
       \param dbm : a DBM
       \param dim : dimension of dbm
       \param constraints : container of clock constraints
       \pre dbm is not empty
       dbm is tight
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
       \brief Reset clocks on a DBM
       \param dbm : a DBM
       \param dim : dimension of dbm
       \param resets : container of clock resets
       \pre dbm is not empty
       dbm is tight
       dbm is a dim*dim array of difference bounds
       every clock in resets belongs to 0..dim-1.
       \post dbm has been updated w.r.t. every clock reset in resets
       dbm is tight
       */
      void reset(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_reset_container_t const & resets);
      
    } // end of namespace details
    
    
    
    
    /*!
     \class elapsed_semantics_t
     \brief Elapsed zone semantics for DBMs
     \tparam EXTRAPOLATION : a zone DBM extrapolation, should implement tchecker::dbm::extrapolation_t
     */
    template <class EXTRAPOLATION>
    class elapsed_semantics_t : private EXTRAPOLATION {
    public:
      /*!
       \brief Type of extrapolation
       */
      using extrapolation_t = EXTRAPOLATION;
      
      /*!
       \brief Type of zones
       */
      using zone_t = tchecker::dbm::zone_t;
      
      /*!
       \brief Constructor
       \param model : a model
       \tparam MODEL : type of model, should inherit from tchecker::clockbounds::model_t
       */
      template <class MODEL>
      elapsed_semantics_t(MODEL const & model)
      : EXTRAPOLATION(model)
      {}
      
      /*!
       \brief Copy constructor
       */
      elapsed_semantics_t(tchecker::dbm::elapsed_semantics_t<EXTRAPOLATION> const &) = default;
      
      /*!
       \brief Move constructor
       */
      elapsed_semantics_t(tchecker::dbm::elapsed_semantics_t<EXTRAPOLATION> &&) = default;
      
      /*!
       \brief Destructor
       */
      ~elapsed_semantics_t() = default;
      
      /*!
       \brief Assignment operator
       */
      tchecker::dbm::elapsed_semantics_t<EXTRAPOLATION> &
      operator= (tchecker::dbm::elapsed_semantics_t<EXTRAPOLATION> const &) = default;
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::dbm::elapsed_semantics_t<EXTRAPOLATION> &
      operator= (tchecker::dbm::elapsed_semantics_t<EXTRAPOLATION> &&) = default;
      
      /*!
       \brief Compute initial zone
       \param zone : a zone
       \param delay_allowed : true if delay is allowed in initial state
       \param invariant : invariant
       \param vloc : tuple of initial locations
       \post zone is the zero zone elapsed (if delay_allowed), then intersected with invariant, and
       then extrapolated w.r.t. clock bounds in vloc
       \return STATE_OK if the resulting zone is not empty, and STATE_CLOCKS_SRC_INVARIANT_VIOLATED
       if invariant does not hold in the initial zone
       */
      template <class VLOC>
      enum tchecker::state_status_t initialize(tchecker::dbm::zone_t & zone,
                                               bool delay_allowed,
                                               tchecker::clock_constraint_container_t const & invariant,
                                               VLOC const & vloc)
      {
        tchecker::dbm::db_t * dbm = zone.dbm();
        auto dim = zone.dim();
        
        tchecker::dbm::zero(dbm, dim);
        
        if ( ! tchecker::dbm::details::constrain(dbm, dim, invariant) )
          return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;
        
        if (delay_allowed) {
          tchecker::dbm::open_up(dbm, dim);
          
          if ( ! tchecker::dbm::details::constrain(dbm, dim, invariant) )
            return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;
        }
        
        EXTRAPOLATION::extrapolate(dbm, dim, vloc);
        
        return tchecker::STATE_OK;
      }
      
      /*!
       \brief Compute next zone
       \param zone : a zone
       \param src_delay_allowed : true if delay allowed in source state
       \param src_invariant : invariant in source state
       \param guard : transition guard
       \param clkreset : transition reset
       \param tgt_delay_allowed : true if delay allowed in target state
       \param tgt_invariant : invariant in target state
       \param tgt_vloc : tuple of locations in target state
       \pre zone is not empty
       zone is tight
       \post zone has been updated to:
       delay((zone \cap guard)[clkreset] \cap tgt_invariant) \cap tgt_invariant
       where delay is applied only if tgt_delay_allowed, and then extrapolated w.r.t.
       clock bounds in tgt_vloc
       \return STATE_OK if the resulting zone is not empty, STATE_CLOCKS_GUARD_VIOLATED
       if guard does not hold in zone, and STATE_CLOCKS_TGT_INVARIANT_VIOLATED does not
       hold in (zone \cap guatd)[clkreset] or in delay((zone \cap guard)[clkreset])
       */
      template <class VLOC>
      enum tchecker::state_status_t next(tchecker::dbm::zone_t & zone,
                                         bool src_delay_allowed,
                                         tchecker::clock_constraint_container_t const & src_invariant,
                                         tchecker::clock_constraint_container_t const & guard,
                                         tchecker::clock_reset_container_t const & clkreset,
                                         bool tgt_delay_allowed,
                                         tchecker::clock_constraint_container_t const & tgt_invariant,
                                         VLOC const & tgt_vloc)
      {
        tchecker::dbm::db_t * dbm = zone.dbm();
        auto dim = zone.dim();
        
        if ( ! tchecker::dbm::details::constrain(dbm, dim, guard) )
          return tchecker::STATE_CLOCKS_GUARD_VIOLATED;
        
        tchecker::dbm::details::reset(dbm, dim, clkreset);
        
        if ( ! tchecker::dbm::details::constrain(dbm, dim, tgt_invariant) )
          return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;
        
        if (tgt_delay_allowed) {
          tchecker::dbm::open_up(dbm, dim);
          
          if ( ! tchecker::dbm::details::constrain(dbm, dim, tgt_invariant) )
            return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;
        }
        
        EXTRAPOLATION::extrapolate(dbm, dim, tgt_vloc);
        
        return tchecker::STATE_OK;
      }
    };
    
    
    
    
    /*!
     \class non_elapsed_semantics_t
     \brief Non elapsed zone semantics for DBMs
     \tparam EXTRAPOLATION : a zone DBM extrapolation, should implement tchecker::dbm::extrapolation_t
     */
    template <class EXTRAPOLATION>
    class non_elapsed_semantics_t : private EXTRAPOLATION {
    public:
      /*!
       \brief Type of extrapolation
       */
      using extrapolation_t = EXTRAPOLATION;
      
      /*!
       \brief Type of zones
       */
      using zone_t = tchecker::dbm::zone_t;
      
      /*!
       \brief Constructor
       \param model : a model
       \tparam MODEL : type of model, should inherit from tchecker::clockbounds::model_t
       */
      template <class MODEL>
      non_elapsed_semantics_t(MODEL const & model)
      : EXTRAPOLATION(model)
      {}
      
      /*!
       \brief Copy constructor
       */
      non_elapsed_semantics_t(tchecker::dbm::non_elapsed_semantics_t<EXTRAPOLATION> const &) = default;
      
      /*!
       \brief Move constructor
       */
      non_elapsed_semantics_t(tchecker::dbm::non_elapsed_semantics_t<EXTRAPOLATION> &&) = default;
      
      /*!
       \brief Destructor
       */
      ~non_elapsed_semantics_t() = default;
      
      /*!
       \brief Assignment operator
       */
      tchecker::dbm::non_elapsed_semantics_t<EXTRAPOLATION> &
      operator= (tchecker::dbm::non_elapsed_semantics_t<EXTRAPOLATION> const &) = default;
      
      /*!
       \brief Move-assignment operator
       */
      tchecker::dbm::non_elapsed_semantics_t<EXTRAPOLATION> &
      operator= (tchecker::dbm::non_elapsed_semantics_t<EXTRAPOLATION> &&) = default;
      
      /*!
       \brief Compute initial zone
       \param zone : a zone
       \param delay_allowed : true if delay is allowed in initial state
       \param invariant : invariant
       \param vloc : tuple of initial locations
       \post zone is the zero zone intersected with invariant, then extrapolated
       w.r.t. clock bounds in vloc and EXTRAPOLATION
       \return STATE_OK if the resulting zone is not empty, and
       STATE_CLOCKS_SRC_INVARIANT_VIOLATED if invariant does not hold in the initial
       zone
       */
      template <class VLOC>
      enum tchecker::state_status_t initialize(tchecker::dbm::zone_t & zone,
                                               bool delay_allowed,
                                               tchecker::clock_constraint_container_t const & invariant,
                                               VLOC const & vloc)
      {
        tchecker::dbm::db_t * dbm = zone.dbm();
        auto dim = zone.dim();
        
        tchecker::dbm::zero(dbm, dim);
        
        if ( ! tchecker::dbm::details::constrain(dbm, dim, invariant) )
          return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;
        
        EXTRAPOLATION::extrapolate(dbm, dim, vloc);
        
        return tchecker::STATE_OK;
      }
      
      /*!
       \brief Compute next zone
       \param zone : a zone
       \param src_delay_allowed : true if delay allowed in source state
       \param src_invariant : invariant in source state
       \param guard : transition guard
       \param clkreset : transition reset
       \param tgt_delay_allowed : true if delay allowed in target state
       \param tgt_invariant : invariant in target state
       \param tgt_vloc : tuple of locations in target state
       \pre zone is not empty
       zone is tight
       zone satisfies the invariant in the source state (guaranteed if zone was returned by this
       class)
       \post zone has been updated to
       (delay(zone) \cap src_invariant \cap guard)[clkreset] \cap tgt_invariant
       where delay is applied only if src_delay_allowed, then extrapolated
       w.r.t. clock bounds in vloc and EXTRAPOLATION
       \return STATE_OK if the result zone is not empty, STATE_CLOCKS_SRC_INVARIANT_VIOLATED
       if src_invariant does bot hold in delay(zone) (should never occur thanks due to precond),
       STATE_CLOCKS_GUARD_VIOLATED if guard does not hold in (delay(zone) \cap src_invariant),
       and STATE_CLOCKS_TGT_INVARIANT_VIOLATED if tgt_invariant does not hold in
       (delay(zone) \cap src_invariant \cap guard)[clkreset]
       */
      template <class VLOC>
      enum tchecker::state_status_t next(tchecker::dbm::zone_t & zone,
                                         bool src_delay_allowed,
                                         tchecker::clock_constraint_container_t const & src_invariant,
                                         tchecker::clock_constraint_container_t const & guard,
                                         tchecker::clock_reset_container_t const & clkreset,
                                         bool tgt_delay_allowed,
                                         tchecker::clock_constraint_container_t const & tgt_invariant,
                                         VLOC const & tgt_vloc)
      {
        tchecker::dbm::db_t * dbm = zone.dbm();
        auto dim = zone.dim();
        
        if (src_delay_allowed) {
          tchecker::dbm::open_up(dbm, dim);
          
          if ( ! tchecker::dbm::details::constrain(dbm, dim, src_invariant) )
            return tchecker::STATE_CLOCKS_SRC_INVARIANT_VIOLATED;  // should never occur
        }
        
        if ( ! tchecker::dbm::details::constrain(dbm, dim, guard) )
          return tchecker::STATE_CLOCKS_GUARD_VIOLATED;
        
        tchecker::dbm::details::reset(dbm, dim, clkreset);
        
        if ( ! tchecker::dbm::details::constrain(dbm, dim, tgt_invariant) )
          return tchecker::STATE_CLOCKS_TGT_INVARIANT_VIOLATED;
        
        EXTRAPOLATION::extrapolate(dbm, dim, tgt_vloc);
        
        return tchecker::STATE_OK;
      }
    };
    
    
    
    
    // Instances
    
    using elapsed_no_extrapolation_t = tchecker::dbm::elapsed_semantics_t<tchecker::dbm::no_extrapolation_t>;
    using elapsed_extraLU_global_t = tchecker::dbm::elapsed_semantics_t<tchecker::dbm::extraLU_global_t>;
    using elapsed_extraLU_local_t = tchecker::dbm::elapsed_semantics_t<tchecker::dbm::extraLU_local_t>;
    using elapsed_extraLUplus_global_t = tchecker::dbm::elapsed_semantics_t<tchecker::dbm::extraLUplus_global_t>;
    using elapsed_extraLUplus_local_t = tchecker::dbm::elapsed_semantics_t<tchecker::dbm::extraLUplus_local_t>;
    using elapsed_extraM_global_t = tchecker::dbm::elapsed_semantics_t<tchecker::dbm::extraM_global_t>;
    using elapsed_extraM_local_t = tchecker::dbm::elapsed_semantics_t<tchecker::dbm::extraM_local_t>;
    using elapsed_extraMplus_global_t = tchecker::dbm::elapsed_semantics_t<tchecker::dbm::extraMplus_global_t>;
    using elapsed_extraMplus_local_t = tchecker::dbm::elapsed_semantics_t<tchecker::dbm::extraMplus_local_t>;
    using non_elapsed_no_extrapolation_t = tchecker::dbm::non_elapsed_semantics_t<tchecker::dbm::no_extrapolation_t>;
    using non_elapsed_extraLU_global_t = tchecker::dbm::non_elapsed_semantics_t<tchecker::dbm::extraLU_global_t>;
    using non_elapsed_extraLU_local_t = tchecker::dbm::non_elapsed_semantics_t<tchecker::dbm::extraLU_local_t>;
    using non_elapsed_extraLUplus_global_t = tchecker::dbm::non_elapsed_semantics_t<tchecker::dbm::extraLUplus_global_t>;
    using non_elapsed_extraLUplus_local_t = tchecker::dbm::non_elapsed_semantics_t<tchecker::dbm::extraLUplus_local_t>;
    using non_elapsed_extraM_global_t = tchecker::dbm::non_elapsed_semantics_t<tchecker::dbm::extraM_global_t>;
    using non_elapsed_extraM_local_t = tchecker::dbm::non_elapsed_semantics_t<tchecker::dbm::extraM_local_t>;
    using non_elapsed_extraMplus_global_t = tchecker::dbm::non_elapsed_semantics_t<tchecker::dbm::extraMplus_global_t>;
    using non_elapsed_extraMplus_local_t = tchecker::dbm::non_elapsed_semantics_t<tchecker::dbm::extraMplus_local_t>;
    
  } // end of namespace dbm
  
} // end of namespace tchecker

#endif // TCHECKER_ZONE_DBM_SEMANTICS_HH


