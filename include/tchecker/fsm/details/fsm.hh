/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_FSM_DETAILS_FSM_HH
#define TCHECKER_FSM_DETAILS_FSM_HH

#include "tchecker/basictypes.hh"
#include "tchecker/flat_system/flat_system.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/vm/vm.hh"

/*!
 \file fsm.hh
 \brief Finite state machine (extensible)
 */

namespace tchecker {
  
  namespace fsm {
    
    namespace details {
      
      /*!
       \class fsm_t
       \brief Finite state machine details
       */
      template <class MODEL, class VLOC, class INTVARS_VAL>
      class fsm_t : protected tchecker::flat_system::flat_system_t<MODEL, VLOC> {
      public:
        /*!
         \brief Type of model
         */
        using model_t = MODEL;
        
        /*!
         \brief Type of tuple of locations
         */
        using vloc_t = VLOC;
        
        /*!
         \brief Type of valuation of bounded integer variables
         */
        using intvars_valuation_t = INTVARS_VAL;
        
        /*!
         \brief Constructor
         \param model : a model
         \post this is a finite state machine over model
         */
        explicit fsm_t(MODEL & model)
        : tchecker::flat_system::flat_system_t<MODEL, VLOC>(model),
        _vm(model.flattened_integer_variables().size(), model.flattened_clock_variables().size())
        {}
        
        /*!
         \brief Copy constructor
         */
        fsm_t(tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL> const &) = default;
        
        /*!
         \brief Move constructor
         */
        fsm_t(tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL> &&) = default;
        
        /*!
         \brief Destructor
         */
        ~fsm_t() = default;
        
        /*!
         \brief Assignment operator (deleted)
         */
        tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL> &
        operator= (tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL> const &) = delete;
        
        /*!
         \brief Move-assigment operator (deleted)
         */
        tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL> &
        operator= (tchecker::fsm::details::fsm_t<MODEL, VLOC, INTVARS_VAL> &&) = delete;
        
        /*!
         \brief Type of iterator over initial states
         */
        using initial_iterator_t = typename tchecker::flat_system::flat_system_t<MODEL, VLOC>::initial_iterator_t;
        
        /*!
         \brief Accessor
         \return iterator over initial states
         */
        tchecker::range_t<initial_iterator_t> initial() const
        {
          return tchecker::flat_system::flat_system_t<MODEL, VLOC>::initial();
        }
        
        /*!
         \brief Dereference type for iterator over initial states
         */
        using initial_iterator_value_t =
        typename tchecker::flat_system::flat_system_t<MODEL, VLOC>::initial_iterator_value_t;
        
        /*!
         \brief Initialize state
         \param vloc : tuple of locations
         \param intvars_val : valuation of bounded integer variables
         \param initial_range : range of initial locations
         \param invariant : container for invariant clock constraints
         \pre intvars_val size is compatible with the model's bounded integer variables
         \post vloc and intvars_val have been initialized w.r.t. the locations
         in initial_range and the initial valuation of bounded integer variables.
         Clock invariants have been inserted into invariant
         \return STATE_OK if initialization succeeded, STATE_SRC_INVARIANT_VIOLATED
         if the initial valuation of integer variables does not satisfy the
         invariant of initial locations in initial_range
         \throw std::invalid_argument : if the precondition is violated
         \throw std::runtime_error : if initial locations invariant generates
         clock resets
         \throw std::runtime_error : if evaluation of invariant throws an exception
         */
        enum tchecker::state_status_t initialize(VLOC & vloc,
                                                 INTVARS_VAL & intvars_val,
                                                 initial_iterator_value_t const & initial_range,
                                                 tchecker::clock_constraint_container_t & invariant)
        {
          if (! _vm.compatible(intvars_val))
            throw std::invalid_argument("Incompatible variables and valuation");
          
          // intialize vloc
          auto status = tchecker::flat_system::flat_system_t<MODEL, VLOC>::initialize(vloc, initial_range);
          if (status != tchecker::STATE_OK)
            return status;
          
          // initialize intvars_val
          auto const & intvars = this->_model.flattened_integer_variables();
          for (auto const & p : intvars.index()) {
            tchecker::intvar_id_t id = p.first;
            intvars_val[id] = intvars.info(id).initial_value();
          }
          
          // check invariant
          for (typename VLOC::loc_t const * loc : vloc)
            if (check_location_invariant(loc, intvars_val, invariant) != 1)
              return tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED;
          
          return tchecker::STATE_OK;
        }
        
        /*!
         \brief Type of iterator over outgoing edges
         */
        using outgoing_edges_iterator_t =
        typename tchecker::flat_system::flat_system_t<MODEL, VLOC>::outgoing_edges_iterator_t;
        
        /*!
         \brief Accessor
         \param vloc : tuple of locations
         \return range of outgoing synchronized and asynchronous edges from vloc
         */
        tchecker::range_t<outgoing_edges_iterator_t> outgoing_edges(VLOC const & vloc) const
        {
          return tchecker::flat_system::flat_system_t<MODEL, VLOC>::outgoing_edges(vloc);
        }
        
        /*!
         \brief Type of iterator over a synchronized outgoing edge
         \note type dereferenced by outgoing_edges_iterator_t, corresponds to tchecker::vedge_iterator_t
         */
        using outgoing_edges_iterator_value_t =
        typename tchecker::flat_system::flat_system_t<MODEL, VLOC>::outgoing_edges_iterator_value_t;
        
        /*!
         \brief Compute next state
         \param vloc : tuple of locations
         \param intvars_val : integer variables valuation
         \param vedge : range of edges in a vedge
         \param src_invariant : container for clock constraints in the invariant of
         vloc before it is updated
         \param guard : container for clock constraints in vedge guard
         \param clkreset : container for clock resets in vedge statement
         \param tgt_invariant : container for clock constraints in the invariant of
         vloc after it is updated
         \pre intvars_val size is compatible with the model's bounded integer variables.
         The source location of edges in edges_range match the locations in vloc.
         No process has more than one edge in edges_range.
         The pid of every process in vedge is less than the size of vloc
         \post the locations in vloc have been updated to target locations
         of edges for processes in vedge, and have been left unchanged for the other
         processes. The values of variables in intvars_val have been updated according
         to the statements in vedge. Clock constraints from the invariant of vloc before
         it is updated have been pushed to src_invariant. Clock constraints from the
         guard in vedge have been pushed into guard. Clock resets from the statement in
         vedge have been pushed into clkreset. And clock constraints from the invariant
         in the updated vloc have been pushed into tgt_invariant
         \return STATE_OK if state computation succeeded, STATE_INCOMPATIBLE_EDGE if
         the source locations in vedge do not match vloc, STATE_SRC_INVARIANT_VIOLATED
         if the valuation intvars_val does not satisfy the invariant in vloc,
         STATE_GUARD_VIOLATED if the values in intvars_val do not satisfy the guard of
         vedge, STATE_STATEMENT_FAILED if statements in vedge cannot be applied to
         intvars_val, STATE_TGT_INVARIANT_VIOLATED if the valuatin of the integer
         variables after statement execution do not satisfy the invariant of updated
         vloc
         \throw std::invalid_argument : if the intvars_val size is not compatible with
         the model's bounded integer variables
         \throw std::invalid_argument : if the pid of an edge in vedge is greater or
         equal to the size of vloc
         \throw std::runtime_error : if the guard in vedge generates clock resets, or
         if the statements in vedge generate clock constraints, or if the invariant in
         updated vloc generates clock resets
         \throw std::runtime_error : if evaluation of invariants, guards or statements
         throws an exception
         */
        enum tchecker::state_status_t next(VLOC & vloc,
                                           INTVARS_VAL & intvars_val,
                                           outgoing_edges_iterator_value_t const & vedge,
                                           tchecker::clock_constraint_container_t & src_invariant,
                                           tchecker::clock_constraint_container_t & guard,
                                           tchecker::clock_reset_container_t & clkreset,
                                           tchecker::clock_constraint_container_t & tgt_invariant)
        {
          if (! _vm.compatible(intvars_val))
            throw std::invalid_argument("Incompatible variables and valuation");
          
          // check source invariant
          for (typename VLOC::loc_t const * loc : vloc)
            if (check_location_invariant(loc, intvars_val, src_invariant) != 1)
              return tchecker::STATE_INTVARS_SRC_INVARIANT_VIOLATED;
          
          // compute next vloc
          auto status = tchecker::flat_system::flat_system_t<MODEL, VLOC>::next(vloc, vedge);
          if (status != tchecker::STATE_OK)
            return status;
          
          // check guards
          auto begin = vedge.begin(), end = vedge.end();
          for (auto it = begin; it != end; ++it)
            if (check_edge_guard(*it, intvars_val, guard) != 1)
              return tchecker::STATE_INTVARS_GUARD_VIOLATED;
          
          // apply statements
          for (auto it = begin; it != end; ++it)
            if (apply_edge_statement(*it, intvars_val, clkreset) != 1)
              return tchecker::STATE_INTVARS_STATEMENT_FAILED;
          
          // check target invariant
          for (typename VLOC::loc_t const * loc : vloc)
            if (check_location_invariant(loc, intvars_val, tgt_invariant) != 1)
              return tchecker::STATE_INTVARS_TGT_INVARIANT_VIOLATED;
          
          return tchecker::STATE_OK;
        }
        
        /*!
         \brief Accessor
         \return Underlying model
         */
        inline constexpr model_t const & model() const
        {
          return tchecker::flat_system::flat_system_t<MODEL, VLOC>::model();
        }
      protected:
        /*!
         \brief Check location invariant
         \param loc : a location
         \param intvars_val : integer variables valuation
         \param invariant : container for clock constraints in loc invariant
         \post loc's invariant has been checked on intvars_val, and all clock constraints
         in loc's invariant have been pushed into invariant
         \return see tchecker::vm_t::run return value
         \throw std::runtime_error : if running loc's invariant bytecode on intvars_val
         throws
         */
        inline tchecker::integer_t check_location_invariant(typename VLOC::loc_t const * loc,
                                                            INTVARS_VAL & intvars_val,
                                                            tchecker::clock_constraint_container_t & invariant)
        {
          try {
            return _vm.run(this->_model.invariant_bytecode(loc->id()), intvars_val, invariant, _throw_clkreset);
          }
          catch (std::exception const & e) {
            throw std::runtime_error(e.what()
                                     + (", in evaluation of " + loc->invariant().to_string() + " from valuation "
                                        + tchecker::to_string(intvars_val,
                                                              this->_model.flattened_integer_variables().index())));
          }
        }
        
        /*!
         \brief Check edge guard
         \param edge : an edge
         \param intvars_val : integer variables valuation
         \param guard : container for clock constraints in edge guard
         \post edge's guard has been checked on intvars_val, and all clock constraints
         in edge's guard have been pushed into guard
         \return see tchecker::vm_t::run return value
         \throw std::runtime_error : if running edge's guard bytecode on intvars_val
         throws
         */
        inline tchecker::integer_t check_edge_guard(typename MODEL::system_t::edge_t const * edge,
                                                    INTVARS_VAL & intvars_val,
                                                    tchecker::clock_constraint_container_t & guard)
        {
          try {
            return _vm.run(this->_model.guard_bytecode(edge->id()), intvars_val, guard, _throw_clkreset);
          }
          catch (std::exception const & e) {
            throw std::runtime_error(e.what()
                                     + (", in evaluation of " + edge->guard().to_string() + " from valuation "
                                        + tchecker::to_string(intvars_val,
                                                              this->_model.flattened_integer_variables().index())));
          }
        }
        
        /*!
         \brief Apply edge statement
         \param edge : an edge
         \param intvars_val : integer variables valuation
         \param clkreset : container for clock resets
         \post intvars_val has been updated following instructions in edge's statement
         bytecode, and all clock resets on edge have been pushed into clkreset
         \return see tchecker::vm_t::run return value
         \throw std::runtime_error : if running edge's statement bytecode on intvars_val
         throws
         */
        inline tchecker::integer_t apply_edge_statement(typename MODEL::system_t::edge_t const * edge,
                                                        INTVARS_VAL & intvars_val,
                                                        tchecker::clock_reset_container_t & clkreset)
        {
          try {
            return _vm.run(this->_model.statement_bytecode(edge->id()), intvars_val, _throw_clkconstr, clkreset);
          }
          catch (std::exception const & e) {
            throw std::runtime_error(e.what()
                                     + (", in evaluation of " + edge->statement().to_string() + " from valuation "
                                        + tchecker::to_string(intvars_val,
                                                              this->_model.flattened_integer_variables().index())));
          }
        }
        
        /*!
         \class throw_container_t
         \brief A clock constraint container that throws when method push_back() is inovked
         */
        template <class CONTAINER>
        class throw_container_t : public CONTAINER {
        public:
          template <class T>
          void push_back(T && t) const
          {
            throw std::runtime_error("throw_container_t::push_back() called");
          }
        };
        
        
        tchecker::vm_t _vm;                                                           /*!< Bytecode interpreter */
        throw_container_t<tchecker::clock_constraint_container_t> _throw_clkconstr;  /*!< Throw clock constraint container */
        throw_container_t<tchecker::clock_reset_container_t> _throw_clkreset;        /*!< Throw clock reset container */
      };
      
    } // end of namespace details
    
  } // end of namespace fsm
  
} // end of namespace tchecker

#endif // TCHECKER_FSM_DETAILS_FSM_HH

