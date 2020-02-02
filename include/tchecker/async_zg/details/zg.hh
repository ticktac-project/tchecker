/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ASYNC_ZG_DETAILS_ZG_HH
#define TCHECKER_ASYNC_ZG_DETAILS_ZG_HH

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/offset_dbm.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file zg.hh
 \brief Asynchronous zone graph (details)
 */

namespace tchecker {
  
  namespace async_zg {
    
    namespace details {
      
      /*!
       \class zg_t
       \brief Asynchronous zone graph (details)
       \tparam TA : type of timed automaton, should inherit from tchecker::ta::details::ta_t
       \tparam ASYNC_ZONE_SEMANTICS : type of asynchronous zone semantics, should implement tchecker::zone::semantics_t
       */
      template <class TA, class ASYNC_ZONE_SEMANTICS>
      class zg_t {
      public:
        /*!
         \brief Type of model
         */
        using model_t = typename TA::model_t;
        
        /*!
         \brief Type of tuple of locations
         */
        using vloc_t = typename TA::vloc_t;
        
        /*!
         \brief Type of valuation of bounded integer variables
         */
        using intvars_valuation_t = typename TA::intvars_valuation_t;
        
        /*!
         \brief Type of offset zones
         */
        using offset_zone_t = typename ASYNC_ZONE_SEMANTICS::offset_zone_t;
        
        /*!
         \brief Type of synchronized zones
         */
        using sync_zone_t = typename ASYNC_ZONE_SEMANTICS::sync_zone_t;
        
        
        /*!
         \brief Constructor
         \tparam ASYNC_MODEL : type of model, should derive from tchecker::zg::async::details::model_t
         \param model : a model
         \note this keeps a pointer to the refmap in model
         */
        template <class ASYNC_MODEL>
        explicit zg_t(ASYNC_MODEL & model)
        : _ta(model),
        _async_zone_semantics(model),
        _refcount(model.flattened_offset_clock_variables().refcount()),
        _refmap(model.flattened_offset_clock_variables().refmap()),
        _src_delay_allowed(model.system().processes_count()),
        _tgt_delay_allowed(model.system().processes_count())
        {}
        
        /*!
         \brief Copy constructor
         */
        zg_t(tchecker::async_zg::details::zg_t<TA, ASYNC_ZONE_SEMANTICS> const &) = default;
        
        /*!
         \brief Move constructor
         */
        zg_t(tchecker::async_zg::details::zg_t<TA, ASYNC_ZONE_SEMANTICS> &&) = default;
        
        /*!
         \brief Destructor
         */
        ~zg_t() = default;
        
        /*!
         \brief Assignment operator
         \param zg : a zone graph
         \post this is a copy of zg
         \return this after assignment
         */
        tchecker::async_zg::details::zg_t<TA, ASYNC_ZONE_SEMANTICS> &
        operator= (tchecker::async_zg::details::zg_t<TA, ASYNC_ZONE_SEMANTICS> const & zg) = default;
        
        /*!
         \brief Move-assignment operator
         \param zg : a zone graph
         \post zg has been moved to this
         \return this after assignment
         */
        tchecker::async_zg::details::zg_t<TA, ASYNC_ZONE_SEMANTICS> &
        operator= (tchecker::async_zg::details::zg_t<TA, ASYNC_ZONE_SEMANTICS> && zg) = default;
        
        /*!
         \brief Type of iterator over initial states
         */
        using initial_iterator_t = typename TA::initial_iterator_t;
        
        /*!
         \brief Accessor
         \return iterator over initial states
         */
        inline tchecker::range_t<initial_iterator_t> initial() const
        {
          return _ta.initial();
        }
        
        /*!
         \brief Dereference type for iterator over initial states
         */
        using initial_iterator_value_t = typename TA::initial_iterator_value_t;
        
        /*!
         \brief Initialize state
         \param vloc : tuple of locations
         \param intvars_val : valuation of integer variables
         \param offset_zone : an offset zone
         \param initial_range : range of locations
         \param invariant : a tchecker::clock_constraint_t container
         \pre vloc, intvars_val, offset_zone, and initial_range have dimensions corresponding to number of processes,
         number of integer variables, number of offset clocks and number of processes in the timed automaton
         \post vloc has been initialized to initial locations in initial_range. intvars_val has been initialized to
         the initial value of integer variables. offset_zone has been initialized to the initial zone according to the chosen
         zone semantics. All constraints in the invariant of vloc have been added to invariant and taken into account for zone
         according to the chosen zone semantics
         \return STATE_OK if vloc, intvars_val and zone have been updated, see tchecker::ta::details::ta_t::initialize
         for other possible values when initializing the integer variables valuation fails, and see
         tchecker/zone/offset_dbm/semantics.hh for possible values when initializing the zone fails
         */
        enum tchecker::state_status_t initialize(vloc_t & vloc,
                                                 intvars_valuation_t & intvars_val,
                                                 offset_zone_t & offset_zone,
                                                 initial_iterator_value_t const & initial_range,
                                                 tchecker::clock_constraint_container_t & invariant)
        {
          auto status = _ta.initialize(vloc, intvars_val, initial_range, invariant);
          if (status != tchecker::STATE_OK)
            return status;
          translate_invariant(invariant);
          tchecker::ta::delay_allowed(vloc, _src_delay_allowed);
          return _async_zone_semantics.initialize(offset_zone, _src_delay_allowed,
                                                  _offset_src_invariant, vloc);
        }
        
        /*!
         \brief Initialize state
         \param vloc : tuple of locations
         \param intvars_val : valuation of integer variables
         \param offset_zone : an offset zone
         \param sync_zone : a synchronized zone
         \param initial_range : range of locations
         \param invariant : a tchecker::clock_constraint_t container
         \pre sync_zone has dimension corresponding to number of (non-offset) clocks. See initialize() for other parameters
         \post sync_zone is the set of synchronized valuations in offset_zone. See initialize() for other parameters
         \return STATE_EMPTY_ZONE if sync_zone is empty. See initialize() for other return values
         */
        enum tchecker::state_status_t initialize(vloc_t & vloc,
                                                 intvars_valuation_t & intvars_val,
                                                 offset_zone_t & offset_zone,
                                                 sync_zone_t & sync_zone,
                                                 initial_iterator_value_t const & initial_range,
                                                 tchecker::clock_constraint_container_t & invariant)
        {
          auto status = _ta.initialize(vloc, intvars_val, initial_range, invariant);
          if (status != tchecker::STATE_OK)
            return status;
          translate_invariant(invariant);
          tchecker::ta::delay_allowed(vloc, _src_delay_allowed);
          return _async_zone_semantics.initialize(offset_zone, sync_zone, _src_delay_allowed,
                                                  _offset_src_invariant, vloc);
        }
        
        /*!
         \brief Type of iterator over outgoing edges
         */
        using outgoing_edges_iterator_t = typename TA::outgoing_edges_iterator_t;
        
        /*!
         \brief Accessor
         \param vloc : tuple of locations
         \return range of outgoing synchronized and asynchronous edges from vloc
         */
        inline tchecker::range_t<outgoing_edges_iterator_t> outgoing_edges(vloc_t const & vloc) const
        {
          return _ta.outgoing_edges(vloc);
        }
        
        /*!
         \brief Dereference type for iterator over outgoing edges
         */
        using outgoing_edges_iterator_value_t = typename TA::outgoing_edges_iterator_value_t;
        
        /*!
         \brief Compute next state
         \param vloc : tuple of locations
         \param intvars_val : valuation of integer variables
         \param offset_zone : an offset zone
         \param vedge : range of synchronized edges
         \param src_invariant : a tchecker::clock_constraint_t container
         \param guard : a tchecker::clock_constraint_t container
         \param clkreset : a tchecker::clock_reset_t container
         \param tgt_invariant : a tchecker::clock_constraint_t container
         \pre vloc, intvars_val, offset_zone, and vedge have dimensions corresponding to number of processes,
         number of integer variables, number of clocks and number of processes in the timed automaton
         \post vloc, intvars_val and offset_zone have been updated according to the transition that consists in
         the synchronized edges in vedge and according to the chosen zone semantics. All the clock constraints
         from the invariant of vloc before updating have been added to src_invariant. All the clock
         constraints from the invariant of vloc after updating have been added to tgt_invariant.
         All the clcok constraints from the guards in vedge have been added to guard. All the
         clock resets from the resets in vedge have been added to clkreset.
         \return STATE_OK if vloc, intvars_val and offset_zone have been updated, see tchecker::ta::details::ta_t::next
         for other possible values when updating the integer variables valuation fails, and see
         tchecker::zone_semantics_t::next for possible values when updating the offset_zone fails
         \note this method does not clear src_invariant, guard, clkreset and tgt_invariant. It adds clock
         contraints/resets to these containers. Every clock constraint/reset in the containers when the
         function is called are taken into account to update zone
         */
        enum tchecker::state_status_t next(vloc_t & vloc,
                                           intvars_valuation_t & intvars_val,
                                           offset_zone_t & offset_zone,
                                           outgoing_edges_iterator_value_t const & vedge,
                                           tchecker::clock_constraint_container_t & src_invariant,
                                           tchecker::clock_constraint_container_t & guard,
                                           tchecker::clock_reset_container_t & clkreset,
                                           tchecker::clock_constraint_container_t & tgt_invariant)
        {
          tchecker::ta::delay_allowed(vloc, _src_delay_allowed);
          auto status = _ta.next(vloc, intvars_val, vedge, src_invariant, guard, clkreset, tgt_invariant);
          if (status != tchecker::STATE_OK)
            return status;
          tchecker::ta::delay_allowed(vloc, _tgt_delay_allowed);
          translate_guard_reset_invariants(src_invariant, guard, clkreset, tgt_invariant);
          reference_clock_synchronization(vedge, _offset_guard);
          return _async_zone_semantics.next(offset_zone, _src_delay_allowed, _offset_src_invariant,
                                            _offset_guard, _offset_clkreset, _tgt_delay_allowed,
                                            _offset_tgt_invariant, vloc);
        }
        
        /*!
         \brief Compute next state
         \param vloc : tuple of locations
         \param intvars_val : valuation of integer variables
         \param offset_zone : an offset zone
         \param sync_zone : a synchronized zone
         \param vedge : range of synchronized edges
         \param src_invariant : a tchecker::clock_constraint_t container
         \param guard : a tchecker::clock_constraint_t container
         \param clkreset : a tchecker::clock_reset_t container
         \param tgt_invariant : a tchecker::clock_constraint_t container
         \pre sync_zone has dimension corresponding to number of (non-offset) clocks. See next() for other parameters
         \post sync_zone is the set of synchronized valuations in offset_zone. See next() for other parameters
         \return STATE_EMPTY_ZONE if sync_zone is empty. See next() for other return values
         */
        enum tchecker::state_status_t next(vloc_t & vloc,
                                           intvars_valuation_t & intvars_val,
                                           offset_zone_t & offset_zone,
                                           sync_zone_t & sync_zone,
                                           outgoing_edges_iterator_value_t const & vedge,
                                           tchecker::clock_constraint_container_t & src_invariant,
                                           tchecker::clock_constraint_container_t & guard,
                                           tchecker::clock_reset_container_t & clkreset,
                                           tchecker::clock_constraint_container_t & tgt_invariant)
        {
          tchecker::ta::delay_allowed(vloc, _src_delay_allowed);
          auto status = _ta.next(vloc, intvars_val, vedge, src_invariant, guard, clkreset, tgt_invariant);
          if (status != tchecker::STATE_OK)
            return status;
          tchecker::ta::delay_allowed(vloc, _tgt_delay_allowed);
          translate_guard_reset_invariants(src_invariant, guard, clkreset, tgt_invariant);
          reference_clock_synchronization(vedge, _offset_guard);
          return _async_zone_semantics.next(offset_zone, sync_zone, _src_delay_allowed, _offset_src_invariant,
                                            _offset_guard, _offset_clkreset, _tgt_delay_allowed,
                                            _offset_tgt_invariant, vloc);
        }
        
        /*!
         \brief Accessor
         \return Underlying model
         */
        inline constexpr model_t const & model() const
        {
          return _ta.model();
        }
      private:
        /*!
         \brief Translate invariant
         \param invariant : a tchecker::clock_constraint_t container
         \post invariant has been translated into _offset_src_invariant
         */
        void translate_invariant(tchecker::clock_constraint_container_t & invariant)
        {
          _offset_src_invariant.clear();
          translate(invariant, _offset_src_invariant);
        }
        
        /*!
         \brief Translate guard, reset and invariants
         \param src_invariant : a tchecker::clock_constraint_t container
         \param guard : a tchecker::clock_constraint_t container
         \param clkreset : a tchecker::clock_reset_t container
         \param tgt_invariant : a tchecker::clock_constraint_t container
         \post src_invariant has been translated into _offset_src_invariant.
         guard has been translated into _offset_guard.
         clkreset has been translated into _offset_clkreset.
         tgt_invariant has been transated into _offset_tgt_invariant
         */
        void translate_guard_reset_invariants(tchecker::clock_constraint_container_t & src_invariant,
                                              tchecker::clock_constraint_container_t & guard,
                                              tchecker::clock_reset_container_t & clkreset,
                                              tchecker::clock_constraint_container_t & tgt_invariant)
        {
          _offset_src_invariant.clear();
          _offset_guard.clear();
          _offset_clkreset.clear();
          _offset_tgt_invariant.clear();
          translate(src_invariant, _offset_src_invariant);
          translate(guard, _offset_guard);
          translate(clkreset, _offset_clkreset);
          translate(tgt_invariant, _offset_tgt_invariant);
        }
        
        /*!
         \brief Translate from clocks to offset clocks
         \param constraints : a clock constraint container
         \param offset_constraints : an offset clock constraint container
         \pre every clock constraint in constraints is expressed over system clocks
         \post every clock constraint from constraints has been translated into an offset clock constraint
         in offset_constraints
         \throw std::invalid_argument : if constraints cannot be translated (e.g. 0-0<=c constraint)
         */
        void translate(tchecker::clock_constraint_container_t const & constraints,
                       tchecker::clock_constraint_container_t & offset_constraints)
        {
          tchecker::clock_id_t offset_id1, offset_id2;
          for (auto & c : constraints) {
            if ((c.id1() == tchecker::zero_clock_id) && (c.id2() == tchecker::zero_clock_id))
              throw std::invalid_argument("cannot translate clock constraint of the form 0-0 <= c");
            offset_id1
            = (c.id1() == tchecker::zero_clock_id
               ? tchecker::offset_dbm::reference_id(c.id2(), _refcount, _refmap)
               : tchecker::offset_dbm::offset_id(c.id1(), _refcount));
            offset_id2
            = (c.id2() == tchecker::zero_clock_id
               ? tchecker::offset_dbm::reference_id(c.id1(), _refcount, _refmap)
               : tchecker::offset_dbm::offset_id(c.id2(), _refcount));
            offset_constraints.emplace_back(offset_id1, offset_id2, c.comparator(), c.value());
          }
        }
        
        /*!
         \brief Translate from system clocks to offset clcoks
         \param resets : a clock reset container
         \param offset_resets : an offset clock reset container
         \pre resets contains only resets to zero, and every clock reset in resets is expressed over
         system clocks
         \post every clock reset in resets has been translated into an offset clock reset in offset_resets
         \throw std::invalid_argument : if resets cannot be translated (e.g. no-zero reset)
         */
        void translate(tchecker::clock_reset_container_t const & resets, tchecker::clock_reset_container_t & offset_resets)
        {
          for (auto & r : resets) {
            if (! r.reset_to_zero())
              throw std::invalid_argument("cannot translate non-zero clock resets");
            tchecker::clock_id_t offset_left = tchecker::offset_dbm::offset_id(r.left_id(), _refcount);
            tchecker::clock_id_t offset_right = tchecker::offset_dbm::reference_id(r.left_id(), _refcount,
                                                                                   _refmap);
            offset_resets.emplace_back(offset_left, offset_right, r.value());
          }
        }
        
        /*!
         \brief Computer synchronization constraint
         \param vedge : tuple of synchronized edges
         \param offset_container : offset clock constraint container
         \post adds synchronization constraint (@1==@2==...==@k) for tuple of synchronized edges
         over procsses 1,2,...,k to offset_container
         */
        void reference_clock_synchronization(outgoing_edges_iterator_value_t const & vedge,
                                             tchecker::clock_constraint_container_t & offset_container)
        {
          auto it = vedge.begin(), end = vedge.end();
          if (it == end)
            return;
          auto itprec = it;
          ++it;
          for (; it != end; ++it, ++itprec) {
            auto pid = (*it)->pid(), pidprec = (*itprec)->pid();
            offset_container.emplace_back(_refmap[pid], _refmap[pidprec], tchecker::clock_constraint_t::LE, 0);
            offset_container.emplace_back(_refmap[pidprec], _refmap[pid], tchecker::clock_constraint_t::LE, 0);
          }
        }
        
        TA _ta;                                                         /*!< Timed automaton */
        ASYNC_ZONE_SEMANTICS _async_zone_semantics;                     /*!< Asynchronous zone semantics */
        tchecker::clock_id_t _refcount;                                 /*!< Number of reference clocks */
        tchecker::clock_id_t const * _refmap;                           /*!< Map offset clocks to their reference clock */
        tchecker::clock_constraint_container_t _offset_src_invariant;   /*!< Source state invariant over offset clocks */
        tchecker::clock_constraint_container_t _offset_guard;           /*!< Guard over offset clocks */
        tchecker::clock_reset_container_t _offset_clkreset;             /*!< Resets over offset clocks */
        tchecker::clock_constraint_container_t _offset_tgt_invariant;   /*!< Target state invariant over offset clocks */
        boost::dynamic_bitset<> _src_delay_allowed;                     /*!< Bit vector of processes allowed to delay */
        boost::dynamic_bitset<> _tgt_delay_allowed;                     /*!< Bit vector of processes allowed to delay */
      };
      
    } // end of namespace details
    
  } // end of namespace async_zg
  
} // end of namespace tchecker

#endif // TCHECKER_ASYNC_ZG_DETAILS_ZG_HH

