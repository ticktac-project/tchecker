/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_EXPRESSION_STATIC_ANALYSIS_HH
#define TCHECKER_EXPRESSION_STATIC_ANALYSIS_HH

#include <unordered_set>

#include "tchecker/basictypes.hh"
#include "tchecker/expression/expression.hh"
#include "tchecker/expression/typed_expression.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file static_analysis.hh
 \brief Static analysis of expressions
 */

namespace tchecker {

// Constant expression evaluation

/*!
 \brief Checks if an expression has a constant value
 \param expr : expression
 \return true if expr has a constant value, false otherwise
 */
bool has_const_value(tchecker::expression_t const & expr);

/*!
 \brief Evaluate a constant expression
 \param expr : expression to evaluate
 \return value of expr
 \throw std::invalid_argument : if expr is not a constant expression (i.e. it contains variables)
 \throw std::runtime_error : should never happen (code safety)
 */
tchecker::integer_t const_evaluate(tchecker::expression_t const & expr);

/*!
 \brief Evaluate a constant expression
 \param expr : expression to evaluate
 \param value : a value
 \return value of expr if expr is a constant expression (i.e. it does not contain any variable), value otherwise
 \throw std::runtime_error : should never happen (code safety)
 */
tchecker::integer_t const_evaluate(tchecker::expression_t const & expr, tchecker::integer_t value);

// Variable IDs extraction

/*!
 \brief Extract variable IDs of an lvalue expression
 \param expr : expression
 \return for expressions of type tchecker::typed_var_expression_t or tchecker::typed_bounded_var_expression_t, the range
 containing the ID of the variable. For expressions of type tchecker::typed_array_expression_t (i.e. x[e]), returns the ID
 of x[e] if e is a constant expression that can be evaluated statically, and the entire range of IDs of array x if the value
 of e cannot be determined
 \throw std::invalid_argument : if expr is not of type type tchecker::typed_var_expression_t,
 tchecker::typed_bounded_var_expression_t or tchecker::typed_array_expression_t
 */
tchecker::range_t<tchecker::variable_id_t> extract_lvalue_variable_ids(tchecker::typed_lvalue_expression_t const & expr);

/*!
 \brief Extract variable IDs from base of an array expression
 \param expr : an lvalue expression
 \param clocks : a set fo clock IDs
 \param intvars : a set of integer variable IDs
 \post if expr is an array expression base[offset], the identifiers of base have been added to clocks or intvars depending on
 the type of the variable. In particular, if offset can be statically evaluated, then base+offset is added to the set
 corresponding to the type of base, otherwise, all base+k for k in the domain of base have been added to clocks or intvars
 depending on the type of base If expr is not an array expression, this function does nothing
 */
void extract_lvalue_base_variable_ids(tchecker::typed_lvalue_expression_t const & expr,
                                      std::unordered_set<tchecker::clock_id_t> & clocks,
                                      std::unordered_set<tchecker::intvar_id_t> & intvars);

/*!
 \brief Extract variables IDs from offset of an array expression
 \param expr : an lvalue expression
 \param clocks : a set of clock IDs
 \param intvars : a set of integer variable IDs
 \post if expr is an array expression base[offset], the identifiers of every variable appearing in offset have been added to
 clocks or intvars depending on the type of the variable. In particular, if an expression of the form x[e] appears in offset,
 then if e can be statically evaluated, then x+e is added to the set corresponding to the type of x, otherwise, all x+k for k in
 the domain of x is added to the corresponding set of identifiers. If expr is not an array expression, this function does
 nothing.
 */
void extract_lvalue_offset_variable_ids(tchecker::typed_lvalue_expression_t const & expr,
                                        std::unordered_set<tchecker::clock_id_t> & clocks,
                                        std::unordered_set<tchecker::intvar_id_t> & intvars);

/*!
 \brief Extract typed variables IDs from an expression
 \param expr : expression
 \param clocks : a set of clock IDs
 \param intvars : a set of integer variable IDs
 \post for every occurrence of a variable x in expr, x has been added to clocks if x is a clock, and to
 intvars if x is an integer variable. For expressions of type tchecker::typed_array_expression_t (i.e. x[e]),
 if e can be statically evaluated then x[e] is added to the set according to the type of x. Otherwise,
 x[k] is added to set (according to the type of x) for every k in the domain of x.
 */
void extract_variables(tchecker::typed_expression_t const & expr, std::unordered_set<tchecker::clock_id_t> & clocks,
                       std::unordered_set<tchecker::intvar_id_t> & intvars);

/*!
 \brief Type of flags of contained clock constraints
 */
struct has_clock_constraints_t {
  bool simple;   /*!< Has simple clock constraints */
  bool diagonal; /*!< Has diagonal clock constraints */
};

/*!
 \brief Compute types of clock constraints in a typed expression
 \param expr : expression
 \return types of clock constraints contained in expr
 */
tchecker::has_clock_constraints_t has_clock_constraints(tchecker::typed_expression_t const & expr);

} // end of namespace tchecker

#endif // TCHECKER_EXPRESSION_STATIC_ANALYSIS_HH
