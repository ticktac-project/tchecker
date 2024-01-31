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
#include "tchecker/ta/system.hh"

/*!
 \file solver.hh
 \brief Solvers for computing the clock bounds of a system of timed processes
 */

namespace tchecker {

namespace clockbounds {

/*!
 \class df_solver_t
 \brief Clock bounds solver for diagonal free timed automata
 \note This solver is based on two papers "Static guards analysis in timed automata verification", Behrmann,
 Bouyer, Fleury and Larsen, TACAS 2003 and "Forward analysis of updatable timed automata", Bouyer, FMSD 2004.

 We allow guards of the form x # c for clock x, integer c and # any of <, <=, ==, >= and >. We allow statements
 of the form x:=c, x:=y and x:=y+c for clocks x and y, and integer c.

 We associate to every location l and every clock x two variables L_{l,x} and U_{l,x} for the maximum lower bound
 on x in l, and the maximum upper bound on x in l respectively.

 Then to every transition l1 - g;s -> l2 in the automaton we associate the Diophantine equations:
 . L_{x,l1} >= c              if x > c / x >= c / x == c appears in g
 . U_{x,l1} >= c              if x < c / x <= c / x == c appears in g
 . L_{x,l1} >= L_{y,l2} - c   if y := x + c appears in s
 . U_{x,l1} >= U_{x,l2} - c   ...
 . L_{x,l1} >= L_{x,l2}       if x is not assigned to in s
 . U_{x,l1} >= U_{x,l2}       ...

 . L_{x,l1} >= L_{y,n} - c   if y := x + c appears in s and n is any location of any process that share y
 . U_{x,l1} >= U_{y,n} - c   if y := x + c appears in s and n is any location of any process that share y

 We take the conjunction of all the constraints for every transition in the automaton. The resulting system of
 Diophantine inequations is solved by computing the minimal path from L_{x,l} and U_{x,l} to 0 for every clock x
 and every location l, in the graph of the system of inequations.

 This class provides methods to specify the constraints from the transitions of an automaton, and a method to
 solve the system of inequations, and compute the resulting bounds.
*/

class df_solver_t {
public:
  /*!
  \brief Constructor
  \param system : a system of timed processes
  \post this is a clear()-ed clock bounds solver for timed automaton system
  \throw std::invalid_argument : if system.loc_number * system.clock_number is too big (overflow)
  */
  df_solver_t(tchecker::ta::system_t const & system);

  /*!
  \brief Copy constructor
  \param solver : a solver
  \post this is a copy of solver
  */
  df_solver_t(tchecker::clockbounds::df_solver_t const & solver);

  /*!
  \brief Move constructor
  \param solver : a solver
  \post solver has been moved to this
  */
  df_solver_t(tchecker::clockbounds::df_solver_t && solver);

  /*!
  \brief Destructor
  */
  ~df_solver_t();

  /*!
  \brief Assignment operator
  \param solver : a solver
  \post this is a copy of solver
  \return this
  */
  tchecker::clockbounds::df_solver_t & operator=(tchecker::clockbounds::df_solver_t const & solver);

  /*!
  \brief Move assignment operator
  \param solver : a solver
  \post solver has been moved to this
  \return this
  */
  tchecker::clockbounds::df_solver_t & operator=(tchecker::clockbounds::df_solver_t && solver);

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
  \brief Add a constraint for assignment x := y + c
  \param l1 : source location ID
  \param l2 : target location ID
  \param x : lvalue clock ID
  \param y : rvalue clock ID
  \param c : constant
  \pre 0 <= l1, l2 < _loc_number (checked by assertion)
  \pre 0 <= x < _clock_number (checked by assertion)
  \pre 0 <= y < _clock_number or y == tchecker::REFCLOCK_ID (checked by assertion)
  \post The constraints L_{y,l1} >= L_{x,l2} - c   and   U_{y,l1} >= U_{x,l2} - c have been added to the
  system of inequations if not const assignment (i.e. y != tchecker::REFCLOCK_ID)
  The constraints L_{y,l1} >= L_{x,m} - c   and   U_{y,l1} >= U_{x,m} - c have been added to the
  system of inequations for any location m that does not belong to the same process than l1, if not consr
  assignment (y != tchecker::REFCLOCK_ID) and not identity (i.e. y!=x or c!=0)
  */
  void add_assignment(tchecker::loc_id_t l1, tchecker::loc_id_t l2, tchecker::clock_id_t x, tchecker::clock_id_t y,
                      tchecker::integer_t c);

  /*!
   \brief Solve amd fill local LU bounds map
   \param map : local LU clock bounds map
   \pre the number of clocks and locations in this solver coincide with those in map
   \post map has been filled with the smallest solution to the constraints in this solver, if any
   \return true if the constraints in this solver have a solution,
   false otherwise
   \throw std::invalid_argument : if the number of clocks or the number of locations in this solver
   differs from those in maps
   */
  bool solve(tchecker::clockbounds::local_lu_map_t & map);

protected:
  /*!
  \brief Accessor
  \param l : location ID
  \param x : clock ID
  \pre 0 <= l < _loc_number (checked by assertion) and 0 <= x < _clock_number (checked by assertion)
  \return The index of bound variable for clock x in location l
  */
  std::size_t index(tchecker::loc_id_t l, tchecker::clock_id_t x) const;

  /*!
   \brief Accessor
   \param l : location ID
   \param x : clock ID
   \pre 0 <= l < _loc_number (checked by assertion)
   \pre 0 <= x < _clock_number (checked by assertion)
   \pre the L matrix is tight and consistent (checked by assertion)
   \return Minimum feasible value for L_{l,x} according to the system of constraints
   */
  tchecker::clockbounds::bound_t L(tchecker::loc_id_t l, tchecker::clock_id_t x) const;

  /*!
  \brief Accessor
  \param l : location ID
  \param x : clock ID
  \pre 0 <= l < _loc_number (checked by assertion)
  \pre 0 <= x < _clock_number (checked by assertion)
  \pre tje U matrix is tight and consistent (checked by assertion)
  \return Minimum feasible value for U_{l,x} according to the system of constraints
  */
  tchecker::clockbounds::bound_t U(tchecker::loc_id_t l, tchecker::clock_id_t x) const;

  /*!
   \brief Update entry [i,j] of the L matrix
   \param i : row index
   \param j : column index
   \param cmp : comparison operator
   \param value : constant
   \pre 0 <= i < _dim (checked by assertion)
   \pre 0 <= j < _dim (checked by assertion)
   \post the [i,j] entry of the L matrix has been set to (cmp, value) if it is stronger
   than the current value, it has been left unchanged otherwise
   \post the flag _updated_L has been set to true if the L matrix entry has been modified,
   it has been left unmodified otherwise
   */
  void updateL(tchecker::clock_id_t i, tchecker::clock_id_t j, tchecker::ineq_cmp_t cmp, tchecker::integer_t value);

  /*!
   \brief Update entry [i,j] of the U matrix
   \param i : row index
   \param j : column index
   \param cmp : comparison operator
   \param value : constant
   \pre 0 <= i < _dim (checked by assertion)
   \pre 0 <= j < _dim (checked by assertion)
   \post the [i,j] entry of the U matric has been set to (cmp, value) if it is stronger
   than the current value, it has been left unchanged otherwise
   \post the flag _updated_U has been set to true if the U matrix entry has been modified,
   it has been left unmodified otherwise
   */
  void updateU(tchecker::clock_id_t i, tchecker::clock_id_t j, tchecker::ineq_cmp_t cmp, tchecker::integer_t value);

  /*!
   \brief Ensure that the L and U matrices are tight
   \post the L and U matrices are tight, if consistent
   \return true if L and U are tight and consistent, false otherwise
   */
  bool ensure_tight();

  tchecker::loc_id_t _loc_number;               /*!< Number of locations */
  tchecker::clock_id_t _clock_number;           /*!< Number of clocks */
  std::vector<tchecker::process_id_t> _loc_pid; /*!< Map: location ID -> process ID */
  std::size_t _dim;                             /*!< Dimension of the equations */
  tchecker::dbm::db_t * _L;                     /*!< Inequations on lower bounds L_{x,l} */
  tchecker::dbm::db_t * _U;                     /*!< Inequations on upper bounds U_{x,l} */
  bool _updated_L;                              /*!< Flags whether the L matrix needs tightening */
  bool _updated_U;                              /*!< Flags whether the U matrix needs tightening */
};

/*!
 \brief Add clock bounds constraints from a location
 \param inv : invariant
 \param loc : location identifier
 \param solver : clock bounds solver
 \post All clock bound constraints from invariant inv in location loc have been added to solver
 \throw std:runtime_error : if inv contains a diagonal clock constraint
 */
void add_location_constraints(tchecker::typed_expression_t const & inv, tchecker::loc_id_t loc,
                              std::shared_ptr<tchecker::clockbounds::df_solver_t> const & solver);

/*!
 \brief Add clock bounds constraints for an edge
 \param guard : guard
 \param stmt : statement
 \param src : source location
 \param tgt : target location
 \param solver : clock bounds solver
 \post All clock bound constraints from guard and stmt on edge src -> tgt have been added to solver
 \throw std::runtime_error : if guard contains a diagonal clock constraint, or if clock updates cannot
 be computed from stmt (e.g. non-constant clock reset in a while statement, etc)
*/
void add_edge_constraints(tchecker::typed_expression_t const & guard, tchecker::typed_statement_t const & stmt,
                          tchecker::loc_id_t src, tchecker::loc_id_t tgt,
                          std::shared_ptr<tchecker::clockbounds::df_solver_t> const & solver);

/*!
 \brief Computes clock bound maps from a system of timed processes
 \param system : a system of timed processes
 \param clockbounds : clock bound maps
 \pre clockbounds maps have the same numbers of clocks and locations as system
 \return true if system has computable clock bounds and all clock bounds map have been
 filled, false otherwise
 \post if system has a solution, then clockbounds has been filled with the computed clock
 bounds, otherwise, clockbounds is empty
 */
bool compute_clockbounds(tchecker::ta::system_t const & system, tchecker::clockbounds::clockbounds_t & clockbounds);

/*!
 \brief Allocates and computes clock bound maps from a system of timed processes
 \param system : a system of timed processes
 \return clock bounds maps for system, nullptr if clock bounds computation failed
 */
tchecker::clockbounds::clockbounds_t * compute_clockbounds(tchecker::ta::system_t const & system);

} // end of namespace clockbounds

} // end of namespace tchecker

#endif // TCHECKER_CLOCKBOUNDS_SOLVER_HH
