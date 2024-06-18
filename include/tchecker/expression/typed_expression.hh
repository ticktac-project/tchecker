/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TYPED_EXPRESSION_HH
#define TCHECKER_TYPED_EXPRESSION_HH

#include <iostream>
#include <string>

#include "tchecker/expression/expression.hh"

/*!
 \file typed_expression.hh
 \brief Typed expressions
 */

namespace tchecker {

// Expression type

/*!
 \brief Type of expressions
 */
enum expression_type_t {
  EXPR_TYPE_BAD = 0,             // Bad type
  EXPR_TYPE_INTTERM,             // Term with integer value
  EXPR_TYPE_INTVAR,              // Integer variable (with size=1)
  EXPR_TYPE_INTARRAY,            // Array of integer variables (with size>1)
  EXPR_TYPE_LOCALINTVAR,         // Local integer variable (with size=1)
  EXPR_TYPE_LOCALINTARRAY,       // Local array of integer variables (with size>1)
  EXPR_TYPE_INTLVALUE,           // Assignable integer expr (left value)
  EXPR_TYPE_LOCALINTLVALUE,      // Assignable local integer expr (left value)
  EXPR_TYPE_CLKVAR,              // Clock variable (with size=1)
  EXPR_TYPE_CLKARRAY,            // Array of clock variables (with size>1)
  EXPR_TYPE_CLKLVALUE,           // Assignable clock expr (left value)
  EXPR_TYPE_CLKDIFF,             // Difference of two clock lvalues
  EXPR_TYPE_INTCLKSUM,           // Sum of an integer term and a clock lvalue
  EXPR_TYPE_ATOMIC_PREDICATE,    // Atomic integer predicate
  EXPR_TYPE_CLKCONSTR_SIMPLE,    // Atomic simple clock constraint
  EXPR_TYPE_CLKCONSTR_DIAGONAL,  // Atomic diagonal clock constraint
  EXPR_TYPE_CONJUNCTIVE_FORMULA, // Conjunction of atomic formulas
};

/*!
 \brief Output operator for expression types
 \param os : output stream
 \param type : expression type
 \post type has been output to os
 \return os after output
 */
std::ostream & operator<<(std::ostream & os, enum tchecker::expression_type_t type);

// Typed expressions

class typed_int_expression_t; // forward declarations
class typed_var_expression_t;
class typed_bounded_var_expression_t;
class typed_array_expression_t;
class typed_par_expression_t;
class typed_binary_expression_t;
class typed_unary_expression_t;
class typed_simple_clkconstr_expression_t;
class typed_diagonal_clkconstr_expression_t;
class typed_ite_expression_t;

/*!
 \class typed_expression_visitor_t
 \brief Visitor pattern for typed expressions
 */
class typed_expression_visitor_t {
public:
  /*!
   \brief Constructor
   */
  typed_expression_visitor_t() = default;

  /*!
   \brief Copy constructor
   */
  typed_expression_visitor_t(tchecker::typed_expression_visitor_t const &) = default;

  /*!
   \brief Destructor
   */
  virtual ~typed_expression_visitor_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::typed_expression_visitor_t & operator=(tchecker::typed_expression_visitor_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::typed_expression_visitor_t & operator=(tchecker::typed_expression_visitor_t &&) = default;

  /*!
   \brief Visitors
   */
  virtual void visit(tchecker::typed_int_expression_t const &) = 0;
  virtual void visit(tchecker::typed_var_expression_t const &) = 0;
  virtual void visit(tchecker::typed_bounded_var_expression_t const &) = 0;
  virtual void visit(tchecker::typed_array_expression_t const &) = 0;
  virtual void visit(tchecker::typed_par_expression_t const &) = 0;
  virtual void visit(tchecker::typed_binary_expression_t const &) = 0;
  virtual void visit(tchecker::typed_unary_expression_t const &) = 0;
  virtual void visit(tchecker::typed_simple_clkconstr_expression_t const &) = 0;
  virtual void visit(tchecker::typed_diagonal_clkconstr_expression_t const &) = 0;
  virtual void visit(tchecker::typed_ite_expression_t const &) = 0;
};

/*!
 \class typed_expression_t
 \brief Expression along with its type
 */
class typed_expression_t : public virtual tchecker::expression_t {
public:
  /*!
   \brief Constructor
   \param type : expression type
   */
  typed_expression_t(enum tchecker::expression_type_t type);

  /*!
   \brief Destructor
   */
  virtual ~typed_expression_t() = default;

  /*!
   \brief Accessor
   \return this expression's type
   */
  inline enum tchecker::expression_type_t type() const { return _type; }

  /*!
   \brief Clone
   \return a clone of this
   */
  virtual tchecker::typed_expression_t * clone() const = 0;

  /*!
   \brief Visit
   \param v : typed expression visitor
   */
  virtual void visit(tchecker::typed_expression_visitor_t & v) const = 0;

  /*!
   \brief Untyped visit
   \param v : expression visitor
   */
  virtual void visit(tchecker::expression_visitor_t & v) const = 0;

protected:
  enum tchecker::expression_type_t const _type; /*!< Expression type */
};

/*!
 \class typed_lvalue_expression_t
 \brief Typed left-value expressions
 */
class typed_lvalue_expression_t : public tchecker::typed_expression_t, public virtual tchecker::lvalue_expression_t {
public:
  /*!
   \brief Constructor
   \param type : expression type
   */
  typed_lvalue_expression_t(enum tchecker::expression_type_t type);

  /*!
   \brief Destructor
   */
  virtual ~typed_lvalue_expression_t() = default;

  /*!
   \brief Accessor
   \return size (1 if assignable, > 1 otherwise)
   */
  virtual tchecker::variable_size_t size() const = 0;

  /*!
   \brief Clone
   \return a clone of this
   */
  virtual tchecker::typed_lvalue_expression_t * clone() const override = 0;
};

/*!
 \class typed_int_expression_t
 \brief Typed integer expression
 */
class typed_int_expression_t : public tchecker::typed_expression_t, public tchecker::int_expression_t {
public:
  /*!
   \brief Constructor
   \param type : expression type
   \param value : integer constant
   */
  typed_int_expression_t(tchecker::expression_type_t type, tchecker::integer_t value);

  /*!
   \brief Destructor
   */
  virtual ~typed_int_expression_t() = default;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_int_expression_t * clone() const override;

  /*!
   \brief Tyoed visit
   \param v : typed expression visitor
   */
  virtual void visit(tchecker::typed_expression_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : expression visitor
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;
};

/*!
 \class typed_var_expression_t
 \brief Typed variable expression
 */
class typed_var_expression_t : public tchecker::typed_lvalue_expression_t, public tchecker::var_expression_t {
public:
  /*!
   \brief Constructor
   \param type : expression type
   \param name : variable name
   \param id : variable identifier
   \param size : variable size
   \pre size >= 1
   \throw std::invalid_argument : if precondition is violated
   */
  typed_var_expression_t(enum tchecker::expression_type_t type, std::string const & name, tchecker::variable_id_t id,
                         tchecker::variable_size_t size);

  /*!
   \brief Destructor
   */
  virtual ~typed_var_expression_t() = default;

  /*!
   \brief Accessor
   \return variable identifier
   */
  inline tchecker::variable_id_t id() const { return _id; }

  /*!
   \brief Accessor
   \return variable size
   */
  inline tchecker::variable_size_t size() const override { return _size; }

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_var_expression_t * clone() const override;

  /*!
   \brief Typed visit
   \param v : typed expression visitor
   */
  virtual void visit(tchecker::typed_expression_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : expression visitor
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;

protected:
  tchecker::variable_id_t const _id;     /*!< Variable identifier */
  tchecker::variable_size_t const _size; /*!< Variable size */
};

/*!
 \class typed_bounded_var_expression_t
 \brief Typed bounded variable expression
 */
class typed_bounded_var_expression_t : public tchecker::typed_var_expression_t {
public:
  /*!
   \brief Constructor
   \param type : expression type
   \param name : variable name
   \param id : variable identifier
   \param size : variable size
   \param min : variable minimal value
   \param max : variable maximal value
   \pre size >= 1 and min <= max
   \throw std::invalid_argument : if precondition is violated
   */
  typed_bounded_var_expression_t(enum tchecker::expression_type_t type, std::string const & name, tchecker::variable_id_t id,
                                 tchecker::variable_size_t size, tchecker::integer_t min, tchecker::integer_t max);

  /*!
   \brief Destructor
   */
  virtual ~typed_bounded_var_expression_t() = default;

  /*!
   \brief Accessor
   \return variable minimal value
   */
  inline tchecker::integer_t min() const { return _min; }

  /*!
   \brief Accessor
   \return variable maximal value
   */
  inline tchecker::integer_t max() const { return _max; }

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_bounded_var_expression_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed expression visitor
   */
  virtual void visit(tchecker::typed_expression_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : expression visitor
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;

protected:
  tchecker::integer_t const _min; /*!< Variable minimal value */
  tchecker::integer_t const _max; /*!< Variable maximal value */
};

/*!
 \class typed_array_expression_t
 \brief Typed array expression
 */
class typed_array_expression_t : public tchecker::typed_lvalue_expression_t, public tchecker::array_expression_t {
public:
  /*!
   \brief Constructor
   \param type : expression type
   \param variable : array variable
   \param offset : offset
   \pre variable and offset are not nullptr
   \throw std::invalid_argument : if variable or offset is nullptr
   \note this keeps pointers on variable and on offset
   \note see tchecker::array_expression_t
   */
  typed_array_expression_t(enum tchecker::expression_type_t type,
                           std::shared_ptr<tchecker::typed_var_expression_t const> const & variable,
                           std::shared_ptr<tchecker::typed_expression_t const> const & offset);

  /*!
   \brief Destructor
   */
  virtual ~typed_array_expression_t() = default;

  /*!
   \brief Accessor
   \return size
   */
  inline tchecker::variable_size_t size() const override
  {
    return 1; // array expression corresponds to var[offset], of size 1
  }

  /*!
   \brief Accessor
   \return typed variable
   */
  tchecker::typed_var_expression_t const & variable() const;

  /*!
   \brief Accessor
   \return typed variable pointer
   */
  std::shared_ptr<tchecker::typed_var_expression_t const> variable_ptr() const;

  /*!
   \brief Accessor
   \return typed offset expression
   */
  tchecker::typed_expression_t const & offset() const;

  /*!
   \brief Accessor
   \return typed offset pointer
   */
  std::shared_ptr<tchecker::typed_expression_t const> offset_ptr() const;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_array_expression_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed expression visitor
   */
  virtual void visit(tchecker::typed_expression_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : expression visitor
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;
};

/*!
 \class typed_par_expression_t
 \brief Typed parenthesis expression
 */
class typed_par_expression_t : public tchecker::typed_expression_t, public tchecker::par_expression_t {
public:
  /*!
   \brief Constructor
   \param type : type of expression
   \param expr : sub expression
   \note this keeps a pointer on expr
   */
  typed_par_expression_t(enum tchecker::expression_type_t type,
                         std::shared_ptr<tchecker::typed_expression_t const> const & expr);

  /*!
   \brief Destructor
   */
  virtual ~typed_par_expression_t() = default;

  /*!
   \brief Accessor
   \return typed sub expression
   */
  tchecker::typed_expression_t const & expr() const;

  /*!
   \brief Accessor
   \return typed sub expression pointer
   */
  std::shared_ptr<tchecker::typed_expression_t const> expr_ptr() const;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_par_expression_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed expression visitor
   */
  virtual void visit(tchecker::typed_expression_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : expression visitor
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;
};

/*!
 \class typed_binary_expression_t
 \brief Typed binary expression
 */
class typed_binary_expression_t : public tchecker::typed_expression_t, public tchecker::binary_expression_t {
public:
  /*!
   \brief Constructor
   \param type : type of expression
   \param op : operator
   \param left : left operand
   \param right : right operand
   \note this keeps pointers on left and on right
   */
  typed_binary_expression_t(enum tchecker::expression_type_t type, enum tchecker::binary_operator_t op,
                            std::shared_ptr<tchecker::typed_expression_t const> const & left,
                            std::shared_ptr<tchecker::typed_expression_t const> const & right);

  /*!
   \brief Accessor
   \return typed left operand
   */
  tchecker::typed_expression_t const & left_operand() const;

  /*!
   \brief Accessor
   \return shared pointer to typed left operand
   */
  std::shared_ptr<tchecker::typed_expression_t const> left_operand_ptr() const;

  /*!
   \brief Accessor
   \return typed right operand
   */
  tchecker::typed_expression_t const & right_operand() const;

  /*!
   \brief Accessor
   \return shared pointer to typed right operand
   */
  std::shared_ptr<tchecker::typed_expression_t const> right_operand_ptr() const;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_binary_expression_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed expression visitor
   */
  virtual void visit(tchecker::typed_expression_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : typed expression visitor
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;
};

/*!
 \class typed_unary_expression_t
 \brief Typed unary expression
 */
class typed_unary_expression_t : public tchecker::typed_expression_t, public tchecker::unary_expression_t {
public:
  /*!
   \brief Constructor
   \param type : type of expression
   \param op : operator
   \param operand : operand
   */
  typed_unary_expression_t(enum tchecker::expression_type_t type, enum tchecker::unary_operator_t op,
                           std::shared_ptr<tchecker::typed_expression_t const> const & operand);

  /*!
   \brief Accessor
   \return typed operand
   */
  tchecker::typed_expression_t const & operand() const;

  /*!
   \brief Accessor
   \return shared pointer to typed operand
   */
  std::shared_ptr<tchecker::typed_expression_t const> operand_ptr() const;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_unary_expression_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed expression visitor
   */
  virtual void visit(tchecker::typed_expression_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : expression visitor
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;
};

/*!
 \class typed_simple_clkconstr_expression_t
 \brief Typed simple clock constraint expression (x # c)
 */
class typed_simple_clkconstr_expression_t : public tchecker::typed_binary_expression_t {
public:
  /*!
   \brief Constructor
   \param type : type of expression
   \param op : operator
   \param left : left operand
   \param right : right operand
   \pre left is clock-valued
   \pre right is interger-valued
   \throw std::invalid_argument : if left is not clocl-valued, or if right is not integer-valued
   */
  typed_simple_clkconstr_expression_t(enum tchecker::expression_type_t type, enum tchecker::binary_operator_t op,
                                      std::shared_ptr<tchecker::typed_expression_t const> const & left,
                                      std::shared_ptr<tchecker::typed_expression_t const> const & right);

  /*!
   \brief Accessor
   \return clock expression
   */
  tchecker::typed_lvalue_expression_t const & clock() const;

  /*!
   \brief Accessor
   \return shared pointer to clock expression
   */
  std::shared_ptr<tchecker::typed_lvalue_expression_t const> clock_ptr() const;

  /*!
   \brief Accessor
   \return bound expression
   */
  tchecker::typed_expression_t const & bound() const;

  /*!
   \brief Accessor
   \return shared pointer to bound expression
   */
  std::shared_ptr<tchecker::typed_expression_t const> bound_ptr() const;

  /*!
   \brief Visit
   \param v : visitor
   */
  virtual void visit(tchecker::typed_expression_visitor_t & v) const override;

  /*!
   \brief Visit
   \param v : visitor
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;
};

/*!
 \class typed_diagonal_clkconstr_expression_t
 \brief Typed diagonal clock constraint expression (x - y # c)
 */
class typed_diagonal_clkconstr_expression_t : public tchecker::typed_binary_expression_t {
public:
  /*!
   \brief Constructor
   \param type : type of expression
   \param op : operator
   \param left : left operand
   \param right : right operand
   \pre left is a clock difference expression
   \pre right is integer-valued
   \throw std::invalid_argument : if left is not a clock difference expression,
   or if right is not integer-valued
   */
  typed_diagonal_clkconstr_expression_t(enum tchecker::expression_type_t type, enum tchecker::binary_operator_t op,
                                        std::shared_ptr<tchecker::typed_expression_t const> const & left,
                                        std::shared_ptr<tchecker::typed_expression_t const> const & right);

  /*!
   \brief Accessor
   \return first clock expression (i.e. x in x - y # c)
   */
  tchecker::typed_lvalue_expression_t const & first_clock() const;

  /*!
   \brief Accessor
   \return shared pointer to first clock expression (i.e. x in x - y # c)
   */
  std::shared_ptr<tchecker::typed_lvalue_expression_t const> first_clock_ptr() const;

  /*!
   \brief Accessor
   \return second clock expression (i.e. y in x - y # c)
   */
  tchecker::typed_lvalue_expression_t const & second_clock() const;

  /*!
   \brief Accessor
   \return shared pointer to second clock expression (i.e. y in x - y # c)
   */
  std::shared_ptr<tchecker::typed_lvalue_expression_t const> second_clock_ptr() const;

  /*!
   \brief Accessor
   \return bound expression
   */
  tchecker::typed_expression_t const & bound() const;

  /*!
   \brief Accessor
   \return shared pointer to bound expression
   */
  std::shared_ptr<tchecker::typed_expression_t const> bound_ptr() const;

  /*!
   \brief Visit
   \param v : visitor
   */
  virtual void visit(tchecker::typed_expression_visitor_t & v) const override;

  /*!
   \brief Visit
   \param v : visitor
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;
};

/*!
 \class typed_ite_expression_t
 \brief Typed binary expression
 */
class typed_ite_expression_t : public tchecker::typed_expression_t, public tchecker::ite_expression_t {
public:
  /*!
   \brief Constructor
   \param type : type of expression
   \param cond : condition expression
   \param then_value : value if cond is true
   \param else_value : value if cond is false
   \pre then_value and else_value are integer-valued
   \throw std::invalid_argument : if then_value or else-value is not integer-valued
   */
  typed_ite_expression_t(tchecker::expression_type_t type, std::shared_ptr<tchecker::typed_expression_t const> const & cond,
                         std::shared_ptr<tchecker::typed_expression_t const> const & then_value,
                         std::shared_ptr<tchecker::typed_expression_t const> const & else_value);

  /*!
   \brief Accessor
   \return typed condition
   */
  tchecker::typed_expression_t const & condition() const;

  /*!
   \brief Accessor
   \return shared pointer to typed condition
   */
  std::shared_ptr<tchecker::typed_expression_t const> condition_ptr() const;

  /*!
   \brief Accessor
   \return typed then-value
   */
  tchecker::typed_expression_t const & then_value() const;

  /*!
   \brief Accessor
   \return shared pointer to typed then-value
   */
  std::shared_ptr<tchecker::typed_expression_t const> then_value_ptr() const;

  /*!
   \brief Accessor
   \return typed else-value
   */
  tchecker::typed_expression_t const & else_value() const;

  /*!
   \brief Accessor
   \return shared pinter to typed else-value
   */
  std::shared_ptr<tchecker::typed_expression_t const> else_value_ptr() const;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_ite_expression_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed expression visitor
   */
  virtual void visit(tchecker::typed_expression_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : expression visitor
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;
};

} // end of namespace tchecker

#endif // TCHECKER_TYPED_EXPRESSION_HH
