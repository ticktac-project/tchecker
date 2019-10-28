/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_CLOCKBOUNDS_SOLVER_HH
#define TCHECKER_CLOCKBOUNDS_SOLVER_HH

#include <algorithm>
#include <functional>

#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/dbm/dbm.hh"
#include "tchecker/expression/typed_expression.hh"
#include "tchecker/statement/typed_statement.hh"

/*!
 \file solver.hh
 \brief Solvers for computing the clock bounds of a model
 */

namespace tchecker {
  
  namespace clockbounds {
    
    namespace diagonal_free {
      
      /*!
       \class solver_t
       \brief Clock bounds solver for diagonal free timed automata
       \note This solver is based on two papers "Static guards analysis in timed automata verification", Behrmann, Bouyer,
       Fleury and Larsen, TACAS 2003 and "Forward analysis of updatable timed automata", Bouyer, FMSD 2004.
       
       We allow guards of the form x # c for clock x, integer c and # any of <, <=, ==, >= and >. We allow statements of the
       form x:=c, x:=y and x:=y+c for clocks x and y, and integer c.
       
       We associate to every location l and every clock x two variables L_{l,x} and U_{l,x} for the maximum lower bound on x in l,
       and the maximum upper bound on x in l respectively.
       
       Then to every transition l1 - g;s -> l2 in the automaton we associate the Diophantine equations:
       . L_{x,l1} >= c              if x > c / x >= c / x == c appears in g
       . U_{x,l1} >= c              if x < c / x <= c / x == c appears in g
       . L_{x,l1} >= L_{y,l2} - c   if y := x + c appears in s (and x <= d / x < d is not in g)
       . U_{x,l1} >= U_{x,l2} - c   ...
       . L_{x,l1} >= L_{x,l2}       if x is not assigned to in s
       . U_{x,l1} >= U_{x,l2}       ...
       
       We take the conjunction of all the constraints for every transition in the automaton. The resulting system of Diophantine
       inequations is solved by computing the minimal path from L_{x,l} and U_{x,l} to 0 for every clock x and every location l,
       in the graph of the system of inequations.
       
       This class provides methods to specify the constraints from the transitions of an automaton, and a method to solve the
       system of inequations, and compute the resulting bounds.
       */
      class solver_t {
      public:
        /*!
         \brief Constructor
         \param loc_number : number of locations
         \param clock_number : number of clocks, including the zero clock
         \param loc_pid : map from location IDs to process IDs
         \pre clock_number >= 1 (checked by assertion), and loc_pid associates to every location ID, the ID of the process
         that owns the location
         \post this is a clear()-ed clock bounds solver for a timed automaton with loc_number locations and clock_number clocks
         \throw std::invalid_argument : if loc_number * clock_number is too big (overflow)
         */
        solver_t(tchecker::loc_id_t loc_number,
                 tchecker::clock_id_t clock_number,
                 std::function<tchecker::process_id_t(tchecker::loc_id_t)> && loc_pid);
        
        /*!
         \brief Copy constructor
         \param solver : a solver
         \post this is a copy of solver
         */
        solver_t(tchecker::clockbounds::diagonal_free::solver_t const & solver);
        
        /*!
         \brief Move constructor
         \param solver : a solver
         \post solver has been moved to this
         */
        solver_t(tchecker::clockbounds::diagonal_free::solver_t && solver);
        
        /*!
         \brief Destructor
         */
        ~solver_t();
        
        /*!
         \brief Assignment operator
         \param solver : a solver
         \post this is a copy of solver
         \return this
         */
        tchecker::clockbounds::diagonal_free::solver_t & operator= (tchecker::clockbounds::diagonal_free::solver_t const & solver);
        
        /*!
         \brief Move assignment operator
         \param solver : a solver
         \post solver has been moved to this
         \return this
         */
        tchecker::clockbounds::diagonal_free::solver_t & operator= (tchecker::clockbounds::diagonal_free::solver_t && solver);
        
        /*!
         \brief Accessor
         \return The number of clocks
         */
        tchecker::clock_id_t clock_number() const;
        
        /*!
         \brief Accessor
         \return The number of locations
         */
        tchecker::clock_id_t loc_number() const;
        
        /*!
         \brief Accessor
         \param l : location ID
         \param x : clock ID
         \pre 0 <= l < _loc_number (checked by assertion) and 0 <= x < _clock_number (checked by assertion)
         \return Minimum feasible value for L_{l,x} according to the system of constraints
         \note this value is only meaningful when the system has been solved and it has a solution
         */
        tchecker::clockbounds::bound_t L(tchecker::loc_id_t l, tchecker::clock_id_t x) const;
        
        /*!
         \brief Accessor
         \param l : location ID
         \param x : clock ID
         \pre 0 <= l < _loc_number (checked by assertion) and 0 <= x < _clock_number (checked by assertion)
         \return Minimum feasible value for U_{l,x} according to the system of constraints
         \note this value is only meaningful when the system has been solved and it has a solution
         */
        tchecker::clockbounds::bound_t U(tchecker::loc_id_t l, tchecker::clock_id_t x) const;
        
        /*!
         \brief Accessor
         \return true is the system of inequations has a solution, false otherwise
         */
        bool has_solution() const;
        
        /*!
         \brief Clear
         \post this is an empty system of Diophantine equations (i.e. no constraint)
         */
        void clear();
        
        /*!
         \brief Add a constraint for guard x > c or x >= c
         \param l : location ID
         \param x : clock ID
         \param c : lower bound
         \pre 0 <= l < _loc_number (checked by assertion) and 0 <= x < _clock_number (checked by assertion)
         \post The constraint L_{x,l} >= c has been added to the system of inequations
         */
        void add_lower_bound_guard(tchecker::loc_id_t l, tchecker::clock_id_t x, tchecker::integer_t c);
        
        /*!
         \brief Add a constraint for guard x < c or x <= c
         \param l : location ID
         \param x : clock ID
         \param c : upper bound
         \pre 0 <= l < _loc_number (checked by assertion) and 0 <= x < _clock_number (checked by assertion)
         \post The constraint U_{x,l} >= c has been added to the system of inequations
         */
        void add_upper_bound_guard(tchecker::loc_id_t l, tchecker::clock_id_t x, tchecker::integer_t c);
        
        /*!
         \brief Add a constraint for assignment y := x + c
         \param l1 : source location ID
         \param l2 : target location ID
         \param y : lvalue clock ID
         \param x : rvalue clock ID
         \param c : constant
         \pre 0 <= l1, l2 < _loc_number (checked by assertion) and 0 <= y, x < _clock_number (checked by assertion)
         \post The constraints L_{x,l1} >= L_{y,l2} - c   and   U_{x,l1} >= U_{y,l2} - c have been added to the system of
         inequations
         */
        void add_assignment(tchecker::loc_id_t l1, tchecker::loc_id_t l2, tchecker::clock_id_t y, tchecker::clock_id_t x,
                            tchecker::integer_t c);
        
        /*!
         \brief Add a constraint no assignment
         \param l1 : source location
         \param l2 : target location
         \param x : clock ID
         \pre 0 <= l1, l2 < _loc_number (checked by assertion) and 0 <= x < _clock_number (checked by assertion)
         \post the constraints L_{x,l1} >= L_{x,l2} and U_{x,l1} >= U_{x,l2} have been added to the system of inequations
         */
        void add_no_assignement(tchecker::loc_id_t l1, tchecker::loc_id_t l2, tchecker::clock_id_t x);
      protected:
        /*!
         \brief Accessor
         \param l : location ID
         \param x : clock ID
         \pre 0 <= l < _loc_number (checked by assertion) and 0 <= x < _clock_number (checked by assertion)
         \return The index of bound variable for clock x in location l
         */
        std::size_t index(tchecker::loc_id_t l, tchecker::clock_id_t x) const;
        
        tchecker::loc_id_t _loc_number;                                      /*!< Number of locations */
        tchecker::clock_id_t _clock_number;                                  /*!< Number of clocks */
        std::function<tchecker::process_id_t(tchecker::loc_id_t)> _loc_pid;  /*!< Map: location ID -> process ID */
        std::size_t _dim;                                                    /*!< Dimension of the equations */
        tchecker::dbm::db_t * _L;                                            /*!< Inequations on lower bounds L_{x,l} */
        tchecker::dbm::db_t * _U;                                            /*!< Inequations on upper bounds U_{x,l} */
        bool _has_solution;                                                  /*!< Flags existence of a solution */
      };
      
      
      
      
      /*!
       \brief Add clock bounds constraints from an invariant
       \param inv : invariant
       \param loc : location
       \param solver : clock bounds solver
       \post All clock bound constraints from invariant expr in location loc have been added to solver
       */
      void add_invariant_constraints(tchecker::typed_expression_t const & inv,
                                     tchecker::loc_id_t loc,
                                     tchecker::clockbounds::diagonal_free::solver_t & solver);
      
      
      /*!
       \brief Add clock bounds constraints for an edge
       \param guard : guard
       \param stmt : statement
       \param src : source location
       \param tgt : target location
       \param solver : clock bounds solver
       \post All clock bound constraints from guard and stmt on edge src->tgt have been added to solver
       */
      void add_edge_constraints(tchecker::typed_expression_t const & guard,
                                tchecker::typed_statement_t const & stmt,
                                tchecker::loc_id_t src,
                                tchecker::loc_id_t tgt,
                                tchecker::clockbounds::diagonal_free::solver_t & solver);
      
      
      /*!
       \brief Solve clock bounds constraints from a model
       \brief model : a model, should derive from tchecker::ta::details::model_t
       \return A diagonal-free clock bound constraints solver which provides minimal clock bounds for model (if any suitable
       clock bounds exist)
       */
      template <class MODEL>
      tchecker::clockbounds::diagonal_free::solver_t solve(MODEL const & model)
      {
        tchecker::loc_id_t loc_nb = model.system().locations_count();
        tchecker::clock_id_t clock_nb = model.flattened_clock_variables().flattened_size();
        
        tchecker::clockbounds::diagonal_free::solver_t solver(loc_nb, clock_nb,
                                                              [&] (tchecker::loc_id_t loc_id) {
                                                                return model.system().location(loc_id)->pid();
                                                              });
        
        for (auto const * loc : model.system().locations())
          tchecker::clockbounds::diagonal_free::add_invariant_constraints(model.typed_invariant(loc->id()), loc->id(), solver);
        
        for (auto const * edge : model.system().edges())
          tchecker::clockbounds::diagonal_free::add_edge_constraints(model.typed_guard(edge->id()),
                                                                     model.typed_statement(edge->id()),
                                                                     edge->src()->id(),
                                                                     edge->tgt()->id(),
                                                                     solver);
        return solver;
      }
      
      
      
      /*!
       \brief Fill a global LU map from a solver
       \param solver : a solver
       \param map : a global LU map
       \pre solver and map have the same number of clocks and the same number
       of locations (checked by assertion), solver has a solution
       \post map has been filled with clockbounds from solver
       \throw std::invalid_argument : if solver has no solution
       */
      void fill_global_lu_map(tchecker::clockbounds::diagonal_free::solver_t const & solver,
                              tchecker::clockbounds::global_lu_map_t & map);
      
      
      /*!
       \brief Fill a local LU map from a solver
       \param solver : a solver
       \param map : a local LU map
       \pre solver and map have the same number of clocks and the same number
       of locations (checked by assertion), solver has a solution
       \post map has been filled with clockbounds from solver
       \throw std::invalid_argument : if solver has no solution
       */
      void fill_local_lu_map(tchecker::clockbounds::diagonal_free::solver_t const & solver,
                             tchecker::clockbounds::local_lu_map_t & map);
      
      
      /*!
       \brief Fill a global M map from a solver
       \param solver : a solver
       \param map : a global M map
       \pre solver and map have the same number of clocks and the same number
       of locations (checked by assertion), solver has a solution
       \post map has been filled with clockbounds from solver
       \throw std::invalid_argument : if solver has no solution
       */
      void fill_global_m_map(tchecker::clockbounds::diagonal_free::solver_t const & solver,
                             tchecker::clockbounds::global_m_map_t & map);
      
      
      /*!
       \brief Fill a local M map from a solver
       \param solver : a solver
       \param map : a local M map
       \pre solver and map have the same number of clocks and the same number
       of locations (checked by assertion), solver has a solution
       \post map has been filled with clockbounds from solver
       \throw std::invalid_argument : if solver has no solution
       */
      void fill_local_m_map(tchecker::clockbounds::diagonal_free::solver_t const & solver,
                            tchecker::clockbounds::local_m_map_t & map);
      
    } // end of namespace diagonal_free
    
    
    
    
    
    /*!
     \brief Computes a local LU clock bound map from a model
     \param model : model, should derive from tchecker::ta::details::model_t
     \return a local LU clock bound map if model admits clock bounds, nullptr otherwise
     */
    template <class MODEL>
    tchecker::clockbounds::local_lu_map_t * compute_local_lu_map(MODEL const & model)
    {
      // Solve
      tchecker::clockbounds::diagonal_free::solver_t solver = tchecker::clockbounds::diagonal_free::solve(model);
      if ( ! solver.has_solution() )
        return nullptr;
      
      // Fill the map
      tchecker::loc_id_t loc_nb = model.system().locations_count();
      tchecker::clock_id_t clock_nb = model.vm_variables().clocks(model.system()).layout().size();
      tchecker::clockbounds::local_lu_map_t * map = new tchecker::clockbounds::local_lu_map_t(loc_nb, clock_nb);
      
      tchecker::clockbounds::diagonal_free::fill_local_lu_map(solver, *map);
      
      return map;
    }
    
    
    
    
    /*!
     \brief Computes a global LU clock bound map from a model
     \param model : model, should derive from tchecker::ta::details::model_t
     \return a global LU clock bound map if model admits clock bounds, nullptr otherwise
     */
    template <class MODEL>
    tchecker::clockbounds::global_lu_map_t * compute_global_lu_map(MODEL const & model)
    {
      // Solve
      tchecker::clockbounds::diagonal_free::solver_t solver = tchecker::clockbounds::diagonal_free::solve(model);
      if ( ! solver.has_solution() )
        return nullptr;
      
      // Fill the map
      tchecker::clock_id_t clock_nb = model.vm_variables().clocks(model.system()).layout().size();
      tchecker::clockbounds::global_lu_map_t * map = new tchecker::clockbounds::global_lu_map_t(clock_nb);
      
      tchecker::clockbounds::diagonal_free::fill_global_lu_map(solver, *map);
      
      return map;
    }
    
    
    
    
    /*!
     \brief Computes a local M clock bound map from a model
     \param model : model, should derive from tchecker::ta::details::model_t
     \return a local M clock bound map if model admits clock bounds, nullptr otherwise
     */
    template <class MODEL>
    tchecker::clockbounds::local_m_map_t * compute_local_m_map(MODEL const & model)
    {
      // Solve
      tchecker::clockbounds::diagonal_free::solver_t solver = tchecker::clockbounds::diagonal_free::solve(model);
      if ( ! solver.has_solution() )
        return nullptr;
      
      // Fill the map
      tchecker::loc_id_t loc_nb = model.system().locations_count();
      tchecker::clock_id_t clock_nb = model.vm_variables().clocks(model.system()).layout().size();
      tchecker::clockbounds::local_m_map_t * map = new tchecker::clockbounds::local_m_map_t(loc_nb, clock_nb);
      
      tchecker::clockbounds::diagonal_free::fill_local_m_map(solver, *map);
      
      return map;
    }
    
    
    
    
    /*!
     \brief Computes a global M clock bound map from a model
     \param model : model, should derive from tchecker::ta::details::model_t
     \return a global M clock bound map if model admits clock bounds, nullptr otherwise
     */
    template <class MODEL>
    tchecker::clockbounds::global_m_map_t * compute_global_m_map(MODEL const & model)
    {
      // Solve
      tchecker::clockbounds::diagonal_free::solver_t solver = tchecker::clockbounds::diagonal_free::solve(model);
      if ( ! solver.has_solution() )
        return nullptr;
      
      // Fill the maps
      tchecker::clock_id_t clock_nb = model.vm_variables().clocks(model.system()).layout().size();
      tchecker::clockbounds::global_m_map_t * map = new tchecker::clockbounds::global_m_map_t(clock_nb);
      
      tchecker::clockbounds::diagonal_free::fill_global_m_map(solver, *map);
      
      return map;
    }
    
    
    
    
    /*!
     \brief Computes all clock bounds map from a model
     \param model : model, should derive from tchecker::ta::details::model_t
     \param global_lu_map : a global LU clock bounds map
     \param local_lu_map : a local LU clock bounds map
     \param global_m_map : a global M clock bounds map
     \param local_m_map : a local M clock bounds map
     \pre all maps have same number of clocks and same number of locations as model (checked
     by assertion)
     \return true if model has computable clock bounds and all clock bounds map have been
     filled, false otherwise
     \post if model has a solution, then global_lu_map has been filled with global LU bounds
     for model, local_lu_map has been fileld by local LU bounds for model, global_m_map
     has been filled with global M bounds for model, and local_m_map has been filled with
     local M bounds for model
     */
    template <class MODEL>
    bool compute_all_clockbounds_map(MODEL const & model,
                                     tchecker::clockbounds::global_lu_map_t & global_lu_map,
                                     tchecker::clockbounds::local_lu_map_t & local_lu_map,
                                     tchecker::clockbounds::global_m_map_t & global_m_map,
                                     tchecker::clockbounds::local_m_map_t & local_m_map)
    {
      // Solve
      tchecker::clockbounds::diagonal_free::solver_t solver = tchecker::clockbounds::diagonal_free::solve(model);
      if ( ! solver.has_solution() )
        return false;
      
      // Fill the maps
      tchecker::clockbounds::diagonal_free::fill_global_lu_map(solver, global_lu_map);
      tchecker::clockbounds::diagonal_free::fill_local_lu_map(solver, local_lu_map);
      tchecker::clockbounds::diagonal_free::fill_global_m_map(solver, global_m_map);
      tchecker::clockbounds::diagonal_free::fill_local_m_map(solver, local_m_map);
      
      return true;
    }
    
  } // end of namespace clockbounds
  
} // end of namespace tchecker

#endif // TCHECKER_CLOCKBOUNDS_SOLVER_HH
