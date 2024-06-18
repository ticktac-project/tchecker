/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_EXPRESSION_HH
#define TCHECKER_EXPRESSION_HH

#include <iostream>
#include <memory>
#include <string>

#include "tchecker/basictypes.hh"

/*!
 \file expression.hh
 \brief Syntax tree for expressions
 */

namespace tchecker {

class expression_visitor_t; // forward declaration

/*!
 \class expression_t
 \brief Base class for expressions
 */
class expression_t {
public:
  /*!
   \brief Destructor
   */
  virtual ~expression_t() = default;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::expression_t * clone() const = 0;

  /*!
   \brief Visit
   \param v : visitor
   */
  virtual void visit(tchecker::expression_visitor_t & v) const = 0;

  /*!
   \brief Output the expression
   \param os : output stream
   \return os after this expression has been output
   */
  virtual std::ostream & output(std::ostream & os) const = 0;

  /*!
   \brief Accessor
   \return this expression as a string
   */
  std::string to_string() const;

private:
  friend std::ostream & operator<<(std::ostream & os, tchecker::expression_t const & expr);
};

/*!
 \brief Output operator for expressions
 \param os : output stream
 \param expr : expression
 \post expr has been output to os
 \return os after expr has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::expression_t const & expr);

/*!
 \class int_expression_t
 \brief Integer constant
 */
class int_expression_t : public virtual tchecker::expression_t {
public:
  /*!
   \brief Constructor
   \param value : integer constant
   */
  explicit int_expression_t(tchecker::integer_t value);

  /*!
   \brief Destructor
   */
  virtual ~int_expression_t() = default;

  /*!
   \brief Accessor
   \return Value
   */
  inline tchecker::integer_t value() const { return _value; }

  /*!
   \brief Output the expression
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::int_expression_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;

protected:
  tchecker::integer_t _value; /*!< Value */
};

/*!
 \class lvalue_expression_t
 \brief Expression that can serve as left values
 */
class lvalue_expression_t : public virtual tchecker::expression_t {
public:
  /*!
   \brief Destructor
   */
  virtual ~lvalue_expression_t() = default;

  /*!
   \brief Clone
   \note Specifies that an lvalue expression clone is an lvalue expression
   */
  virtual tchecker::lvalue_expression_t * clone() const override = 0;
};

/*!
 \class var_expression_t
 \brief Variable
 */
class var_expression_t : public virtual tchecker::lvalue_expression_t {
public:
  /*!
   \brief Constructor
   \param name : variable name
   \pre name is not empty
   \throw std::invalid_argument : if name is empty
   */
  explicit var_expression_t(std::string const & name);

  /*!
   \brief Destructor
   */
  virtual ~var_expression_t() = default;

  /*!
   \brief Accessor
   \return Name
   */
  inline std::string const & name() const { return _name; }

  /*!
   \brief Output the expression
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::var_expression_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;

protected:
  std::string const _name; /*!< Variable name */
};

/*!
 \class array_expression_t
 \brief Cell in an array (i.e. varariable[offset])
 */
class array_expression_t : public virtual tchecker::lvalue_expression_t {
public:
  /*!
   \brief Constructor
   \param variable : array variable
   \param offset : offset
   \pre offset != nullptr and variable != nullptr
   \throw std::invalid_argument : if offset == nullptr or name == nullptr
   \note this keeps pointers on offset and on variable
   */
  array_expression_t(std::shared_ptr<tchecker::var_expression_t const> const & variable,
                     std::shared_ptr<tchecker::expression_t const> const & offset);

  /*!
   \brief Destructor
   */
  virtual ~array_expression_t();

  /*!
   \brief Accessor
   \return Variable
   */
  inline tchecker::var_expression_t const & variable() const { return (*_variable); }

  /*!
   \brief Accessor
   \return shared pointer to variable expression
   */
  inline std::shared_ptr<tchecker::var_expression_t const> const & variable_ptr() const { return _variable; }

  /*!
   \brief Accessor
   \return Offset
   */
  inline tchecker::expression_t const & offset() const { return (*_offset); }

  /*!
   \brief Accessor
   \return shared pointer to offset expression
   */
  std::shared_ptr<tchecker::expression_t const> const & offset_ptr() const { return _offset; }

  /*!
   \brief Output the expression
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::array_expression_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::var_expression_t const> _variable; /*!< Array variable */
  std::shared_ptr<tchecker::expression_t const> _offset;       /*!< Offset */
};

/*!
 \class par_expression_t
 \brief Expression inside parenthesis
 */
class par_expression_t : public virtual tchecker::expression_t {
public:
  /*!
   \brief Constructor
   \param expr : operand
   \pre expr is not nullptr
   \throw std::invalid_argument : if expr is nullptr
   \note this keeps a pointer on expr
   */
  par_expression_t(std::shared_ptr<tchecker::expression_t const> const & expr);

  /*!
   \brief Destructor
   */
  virtual ~par_expression_t();

  /*!
   \brief Accessor
   \return sub expression
   */
  inline tchecker::expression_t const & expr() const { return *_expr; }

  /*!
   \brief Accessor
   \return a shared pointer to sub expression
   */
  inline std::shared_ptr<tchecker::expression_t const> const & expr_ptr() const { return _expr; }

  /*!
   \brief Output the expression
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::par_expression_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::expression_t const> _expr; /*!< Expression */
};

/*!
 \brief Type of unary operators
 */
enum unary_operator_t {
  EXPR_OP_NEG,  // unary minus
  EXPR_OP_LNOT, // logical not
};

/*!
 \brief Output operator for unary operators
 \param os : output stream
 \param op : unary operator
 \post op has been output to os
 \return os after op has been output
 */
std::ostream & operator<<(std::ostream & os, enum unary_operator_t op);

/*!
 \class unary_expression_t
 \brief Application of an unary operator to an expression
 */
class unary_expression_t : public virtual tchecker::expression_t {
public:
  /*!
   \brief Constructor
   \param op : operator
   \param expr : operand
   \pre expr is not nullptr
   \throw std::invalid_argument : if expr is nullptr
   \note this keeps a pointer on expr
   */
  unary_expression_t(enum unary_operator_t op, std::shared_ptr<tchecker::expression_t const> const & expr);

  /*!
   \brief Destructor
   \post the operand expression has been deleted
   */
  virtual ~unary_expression_t();

  /*!
   \brief Accessor
   \return Operand
   */
  inline enum unary_operator_t unary_operator() const { return _operator; }

  /*!
   \brief Accessor
   \return Operand
   */
  inline tchecker::expression_t const & operand() const { return (*_expr); }

  /*!
   \brief Accessor
   \return a shared pointer on operand
   */
  inline std::shared_ptr<tchecker::expression_t const> const & operand_ptr() const { return _expr; }

  /*!
   \brief Output the expression
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::unary_expression_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;

protected:
  enum unary_operator_t _operator;                     /*!< Operator */
  std::shared_ptr<tchecker::expression_t const> _expr; /*!< Operand */
};

/*!
 \brief Type of binary operators
 */
enum binary_operator_t {
  EXPR_OP_LAND,
  EXPR_OP_LT,
  EXPR_OP_LE,
  EXPR_OP_EQ,
  EXPR_OP_NEQ,
  EXPR_OP_GE,
  EXPR_OP_GT,
  EXPR_OP_MINUS,
  EXPR_OP_PLUS,
  EXPR_OP_TIMES,
  EXPR_OP_DIV,
  EXPR_OP_MOD,
};

/*!
 \brief Output operator for binary operators
 \param os : output stream
 \param op : binary operator
 \post op has been output to os
 \return os after op has been output
 */
std::ostream & operator<<(std::ostream & os, enum binary_operator_t op);

/*!
 \brief Predicate check
 \param op : operator
 \return true if op is a predicate (LT, LE, EQ, NEQ, GE, GT), false otherwise
 */
bool predicate(enum tchecker::binary_operator_t op);

/*!
 \brief Check if binary operator is LE or LT
 \param op : operator
 \return true if op is LE or LT, false otherwise
*/
bool is_less(enum tchecker::binary_operator_t op);

/*!
 \brief Reverse comparison operator
 \param op : comparison operator
 \return revered op: < becomes >, <= becomes >=, == stays ==, != stays !=,
 >= becomes <= and > becomes <
 \throw std::invalid_argument : if op is not a comparison operator
*/
enum tchecker::binary_operator_t reverse_cmp(enum tchecker::binary_operator_t op);

/*!
 \class binary_expression_t
 \brief Application of a binary operator to expressions
 */
class binary_expression_t : public virtual tchecker::expression_t {
public:
  /*!
   \brief Constructor
   \param op : operaror
   \param left : left operand
   \param right : right operand
   \pre left and right are not nullptr
   \throw std::invalid_argument : if left or right is nullptr
   \note this keeps pointers on left and on right
   */
  binary_expression_t(enum binary_operator_t op, std::shared_ptr<tchecker::expression_t const> const & left,
                      std::shared_ptr<tchecker::expression_t const> const & right);

  /*!
   \brief Destructor
   \post the operand expressions have been deleted
   */
  virtual ~binary_expression_t();

  /*!
   \brief Accessor
   \return Operator
   */
  enum binary_operator_t binary_operator() const { return _op; }

  /*!
   \brief Accessor
   \return Left operand
   */
  inline tchecker::expression_t const & left_operand() const { return (*_left); }

  /*!
   \brief Accessor
   \return shared pointer on left operand
   */
  inline std::shared_ptr<tchecker::expression_t const> const & left_operand_ptr() const { return _left; }

  /*!
   \brief Accessor
   \return Right operand
   */
  inline tchecker::expression_t const & right_operand() const { return (*_right); }

  /*!
   \brief Accessor
   \return shared pointer on right operand
   */
  inline std::shared_ptr<tchecker::expression_t const> const & right_operand_ptr() const { return _right; }

  /*!
   \brief Output the expression
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::binary_expression_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;

protected:
  enum binary_operator_t _op;                           /*!< Operator */
  std::shared_ptr<tchecker::expression_t const> _left;  /*!< Left operand */
  std::shared_ptr<tchecker::expression_t const> _right; /*!< Right operand */
};

/*!
 \class ite_expression_t
 \brief Application of if-then-else operator to expressions
 */
class ite_expression_t : public virtual tchecker::expression_t {
public:
  /*!
   \brief Constructor
   \param condition : the decision expression
   \param then_value : the value of the expression if the condition is true
   \param else_value : the value of the expression if the condition is false
   \pre condition, then_value and else_value are not nullptr
   \throw std::invalid_argument : if an argument is nullptr
   \note this keeps pointers on its arguments
   */
  ite_expression_t(std::shared_ptr<tchecker::expression_t const> const & condition,
                   std::shared_ptr<tchecker::expression_t const> const & then_value,
                   std::shared_ptr<tchecker::expression_t const> const & else_value);
  /*!
   \brief Destructor
   \post the operand expressions have been deleted
   */
  virtual ~ite_expression_t();

  /*!
   \brief Accessor
   \return the decision expression
   */
  inline tchecker::expression_t const & condition() const { return (*_condition); }

  /*!
   \brief Accessor
   \return a shared pointer to decision expression
   */
  inline std::shared_ptr<tchecker::expression_t const> const & condition_ptr() const { return _condition; }

  /*!
   \brief Accessor
   \return the Then value
   */
  inline tchecker::expression_t const & then_value() const { return (*_then_value); }

  /*!
   \brief Accessor
   \return a shared pointer to then value
   */
  inline std::shared_ptr<tchecker::expression_t const> const & then_value_ptr() const { return _then_value; }

  /*!
   \brief Accessor
   \return the Else value
   */
  inline tchecker::expression_t const & else_value() const { return (*_else_value); }

  /*!
   \brief Accessor
   \return a shared pointer to else value
   */
  inline std::shared_ptr<tchecker::expression_t const> const & else_value_ptr() const { return _else_value; }

  /*!
   \brief Output the expression
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::ite_expression_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::expression_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::expression_t const> _condition;  /*!< guard of the ite */
  std::shared_ptr<tchecker::expression_t const> _then_value; /*!< 'then' part of the expression */
  std::shared_ptr<tchecker::expression_t const> _else_value; /*!< 'else' part of the expression */
};

/*!
 \class expression_visitor_t
 \brief Visitor for expressions
 */
class expression_visitor_t {
public:
  /*!
   \brief Constructor
   */
  expression_visitor_t() = default;

  /*!
   \brief Copy constructor
   */
  expression_visitor_t(tchecker::expression_visitor_t const &) = default;

  /*!
   \brief Move constructor
   */
  expression_visitor_t(tchecker::expression_visitor_t &&) = default;

  /*!
   \brief Destructor
   */
  virtual ~expression_visitor_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::expression_visitor_t & operator=(tchecker::expression_visitor_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::expression_visitor_t & operator=(tchecker::expression_visitor_t &&) = default;

  /*!
   \brief Visitors
   */
  virtual void visit(tchecker::int_expression_t const & expr) = 0;
  virtual void visit(tchecker::var_expression_t const & expr) = 0;
  virtual void visit(tchecker::array_expression_t const & expr) = 0;
  virtual void visit(tchecker::par_expression_t const & expr) = 0;
  virtual void visit(tchecker::unary_expression_t const & expr) = 0;
  virtual void visit(tchecker::binary_expression_t const & expr) = 0;
  virtual void visit(tchecker::ite_expression_t const & expr) = 0;
};

} // end namespace tchecker

#endif // TCHECKER_EXPRESSION_HH
