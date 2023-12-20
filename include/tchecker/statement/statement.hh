/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STATEMENT_HH
#define TCHECKER_STATEMENT_HH

#include <iostream>
#include <memory>
#include <string>

#include "tchecker/expression/expression.hh"

/*!
 \file statement.hh
 \brief Syntax tree for statements
 */

namespace tchecker {

class statement_visitor_t; // forward declaration

/*!
 \class statement_t
 \brief Base class for statements
 */
class statement_t {
public:
  /*!
   \brief Constructor
   */
  statement_t() = default;

  /*!
   \brief Destructor
   */
  virtual ~statement_t() = default;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::statement_t * clone() const = 0;

  /*!
   \brief Visit
   \param v : visitor
   */
  virtual void visit(tchecker::statement_visitor_t & v) const = 0;

  /*!
   \brief Output the statement
   \param os : output stream
   */
  virtual std::ostream & output(std::ostream & os) const = 0;

  /*!
   \brief Accessor
   \return this statement as a string
   */
  std::string to_string() const;

private:
  friend std::ostream & operator<<(std::ostream & os, tchecker::statement_t const & stmt);
};

/*!
 \brief Output operator for statements
 \param os : output stream
 \param stmt : statement
 \post stmt has been output to os
 \return os after stmt has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::statement_t const & stmt);

/*!
 \class nop_statement_t
 \brief No instruction
 */
class nop_statement_t : public virtual tchecker::statement_t {
public:
  /*!
   \brief Constructor
   */
  nop_statement_t();

  /*!
   \brief Destructor
   */
  virtual ~nop_statement_t();

  /*!
   \brief Output the statement
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::nop_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;
};

/*!
 \class assign_statement_t
 \brief assignment
 */
class assign_statement_t : public virtual tchecker::statement_t {
public:
  /*!
   \brief Constructor
   \param lvalue : left-value expression
   \param rvalue : right-value expression
   \pre lvalue != nullptr and rvalue != nullptr
   \post this assignment statement points to left-value lvalue and right-value rvalue
   \throw std::invalid_argument : if lvalue or rvalue is nullptr
   */
  assign_statement_t(std::shared_ptr<tchecker::lvalue_expression_t const> const & lvalue,
                     std::shared_ptr<tchecker::expression_t const> const & rvalue);

  /*!
   \brief Destructor
   */
  virtual ~assign_statement_t();

  /*!
   \brief Accessor
   \return Left value
   */
  inline tchecker::lvalue_expression_t const & lvalue() const { return *_lvalue; }

  /*!
   \brief Accessor
   \return Shared pointer on left value
   */
  inline std::shared_ptr<tchecker::lvalue_expression_t const> const & lvalue_ptr() const { return _lvalue; }

  /*!
   \brief Accessor
   \return Right value
   */
  inline tchecker::expression_t const & rvalue() const { return *_rvalue; }

  /*!
   \brief Accessor
   \return Shared pointer on right value
   */
  inline std::shared_ptr<tchecker::expression_t const> const & rvalue_ptr() const { return _rvalue; }

  /*!
   \brief Output the statement
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::assign_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::lvalue_expression_t const> _lvalue; /*!< Left value */
  std::shared_ptr<tchecker::expression_t const> _rvalue;        /*!< Right value*/
};

/*!
 \class sequence_statement_t
 \brief Sequential composition of statements
 */
class sequence_statement_t : public virtual tchecker::statement_t {
public:
  /*!
   \brief Constructor
   \param first : first statement
   \param second : second statement
   \pre first != nullptr and second != nullptr
   \post this spoints to first and to second (i.e. first; second)
   \throw std::invalid_argument : if first or second is nullptr
   \note this keeps pointers on first and on second
   */
  sequence_statement_t(std::shared_ptr<tchecker::statement_t const> const & first,
                       std::shared_ptr<tchecker::statement_t const> const & second);

  /*!
   \brief Destructor
   */
  virtual ~sequence_statement_t();

  /*!
   \brief Accessor
   \return First statement
   */
  inline tchecker::statement_t const & first() const { return *_first; }

  /*!
   \brief Accessor
   \return Shared pointer on first statement
   */
  inline std::shared_ptr<tchecker::statement_t const> const & first_ptr() const { return _first; }

  /*!
   \brief Accessor
   \return Second statement
   */
  inline tchecker::statement_t const & second() const { return *_second; }

  /*!
   \brief Accessor
   \return Shared pointer on second statement
   */
  inline std::shared_ptr<tchecker::statement_t const> const & second_ptr() const { return _second; }

  /*!
   \brief Output the statement
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override { return os << *_first << "; " << *_second; }

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::sequence_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::statement_t const> _first;  /*!< First statement */
  std::shared_ptr<tchecker::statement_t const> _second; /*!< Second statement */
};

/*!
 \class if_statement_t
 \brief If-Then-Else statement
  */
class if_statement_t : public virtual tchecker::statement_t {
public:
  /*!
   \brief Constructor
   \param cond : condition
   \param then_stmt : then statement
   \param else_stmt : else statement
   \pre cond != nullptr and then_stmt != nullptr and else_stmt != nullptr
   \post this statement points to cond, to then_stmt and to else_stmt (i.e. if (cond) then then_stmt else else_stmt)
   \throw std::invalid_argument : if cond, then_stmt or else_stmt is nullptr
   */
  if_statement_t(std::shared_ptr<tchecker::expression_t const> const & cond,
                 std::shared_ptr<tchecker::statement_t const> const & then_stmt,
                 std::shared_ptr<tchecker::statement_t const> const & else_stmt);

  /*!
   \brief Destructor
   */
  virtual ~if_statement_t();

  /*!
   \brief Accessor
   \return Condition of the if statement
   */
  inline tchecker::expression_t const & condition() const { return *_condition; }

  /*!
   \brief Accessor
   \return Shared pointer on condition of the if statement
   */
  inline std::shared_ptr<tchecker::expression_t const> const & condition_ptr() const { return _condition; }

  /*!
   \brief Accessor
   \return Then statement
   */
  inline tchecker::statement_t const & then_stmt() const { return *_then_stmt; }

  /*!
   \brief Accessor
   \return Shared pointer on then statement
   */
  inline std::shared_ptr<tchecker::statement_t const> const & then_stmt_ptr() const { return _then_stmt; }

  /*!
   \brief Accessor
   \return Second statement
   */
  inline tchecker::statement_t const & else_stmt() const { return *_else_stmt; }

  /*!
   \brief Accessor
   \return Shared pointer on second statement
   */
  inline std::shared_ptr<tchecker::statement_t const> const & else_stmt_ptr() const { return _else_stmt; }

  /*!
   \brief Output the statement
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::if_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::expression_t const> _condition; /*!< Guard of the if statement */
  std::shared_ptr<tchecker::statement_t const> _then_stmt;  /*!< Then statement */
  std::shared_ptr<tchecker::statement_t const> _else_stmt;  /*!< Else statement */
};

/*!
 \class while_statement_t
 \brief while statement
 */
class while_statement_t : public virtual tchecker::statement_t {
public:
  /*!
   \brief Constructor
   \param cond : condition
   \param stmt : iterated statement
   \pre cond and stmt are not nullptr
   \post this keeps pointers to cond and to stmt (i.e. while (cond) stmt end)
   \throw std::invalid_argument : if cond or stmt is nullptr
   \note this keeps pointers on cond and on stmt
   */
  while_statement_t(std::shared_ptr<tchecker::expression_t const> const & cond,
                    std::shared_ptr<tchecker::statement_t const> const & stmt);

  /*!
   \brief Destructor
   */
  virtual ~while_statement_t();

  /*!
   \brief Accessor
   \return Condition of the while loop
   */
  inline tchecker::expression_t const & condition() const { return *_condition; }

  /*!
   \brief Accessor
   \return Shared pointer on the condition of the while loop
   */
  inline std::shared_ptr<tchecker::expression_t const> const & condition_ptr() const { return _condition; }

  /*!
   \brief Accessor
   \return Iterated statement
   */
  inline tchecker::statement_t const & statement() const { return *_stmt; }

  /*!
   \brief Accessor
   \return Shared pointer on iterated statement
   */
  inline std::shared_ptr<tchecker::statement_t const> const & statement_ptr() const { return _stmt; }

  /*!
   \brief Output the statement
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::while_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::expression_t const> _condition; /*!< Guard of the if statement */
  std::shared_ptr<tchecker::statement_t const> _stmt;       /*!< Iterated statement */
};

/*!
 \class local_var_statement_t
 \brief declaration of a local integer variable
 */
class local_var_statement_t : public virtual tchecker::statement_t {
public:
  /*!
   \brief Constructor
   \param variable : the new local variable
   \pre variable != nullptr
   \post this local variable points to variable and has initial value 0
   \throw std::invalid_argument : if varibale is nullptr
   */
  local_var_statement_t(std::shared_ptr<tchecker::var_expression_t const> const & variable);

  /*!
   \brief Constructor
   \param variable : the new local variable
   \param init : initial value assigned to variable
   \pre variable != nullptr and init != nullptr
   \post this keeps pointers to variable and to init (i.e. variable initialized to init)
   \throw std::invalid_argument : if variable of init is nullptr
   */
  local_var_statement_t(std::shared_ptr<tchecker::var_expression_t const> const & variable,
                        std::shared_ptr<tchecker::expression_t const> const & init);

  /*!
   \brief Destructor
   */
  virtual ~local_var_statement_t();

  /*!
   \brief Accessor
   \return The variable
   */
  inline tchecker::var_expression_t const & variable() const { return *_variable; }

  /*!
   \brief Accessor
   \return Shared pointer to the variable
   */
  inline std::shared_ptr<tchecker::var_expression_t const> const & variable_ptr() const { return _variable; }

  /*!
   \brief Accessor
   \return The initial value of the local variable
   */
  inline tchecker::expression_t const & initial_value() const { return *_initial_value; }

  /*!
   \brief Accessor
   \return Shared pointer to the initial value of the local variable
   */
  inline std::shared_ptr<tchecker::expression_t const> const & initial_value_ptr() const { return _initial_value; }

  /*!
   \brief Output the statement
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::local_var_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::var_expression_t const> _variable;  /*!< Variable  */
  std::shared_ptr<tchecker::expression_t const> _initial_value; /*!< Initial value */
};

/*!
 \class local_array_statement_t
 \brief local array declaration
 */
class local_array_statement_t : public virtual tchecker::statement_t {
public:
  /*!
   \brief Constructor
   \param variable: the new local array variable
   \param size: size expression
   \pre variable != nullptr and size != nullptr
   \post this keeps pointers to variable and to size (i.e. variable[size])
   \throw std::invalid_argument : if an parameter is nullptr
   */
  local_array_statement_t(std::shared_ptr<tchecker::var_expression_t const> const & variable,
                          std::shared_ptr<tchecker::expression_t const> const & size);

  /*!
   \brief Destructor
   */
  virtual ~local_array_statement_t();

  /*!
   \brief Accessor
   \return Variable expression
   */
  inline tchecker::var_expression_t const & variable() const { return (*_variable); }

  /*!
   \brief Accessor
   \return Shared pointer to variable expression
   */
  inline std::shared_ptr<tchecker::var_expression_t const> const & variable_ptr() const { return _variable; }

  /*!
   \brief Accessor
   \return Size of the variable
   */
  inline tchecker::expression_t const & size() const { return (*_size); }

  /*!
   \brief Accessor
   \return Shared pointer to size expression
   */
  inline std::shared_ptr<tchecker::expression_t const> const & size_ptr() const { return _size; }

  /*!
   \brief Output the statement
   \param os : output stream
   \post this has been output to os
   \return os after this has been output
   */
  virtual std::ostream & output(std::ostream & os) const override;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::local_array_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : visitor
   \post v.visit(*this) has been called
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;

protected:
  std::shared_ptr<tchecker::var_expression_t const> _variable; /*!< Variable  */
  std::shared_ptr<tchecker::expression_t const> _size;         /*!< Size of the array*/
};

/*!
 \class statement_visitor_t
 \brief Visitor for statements
 */
class statement_visitor_t {
public:
  /*!
   \brief Constructor
   */
  statement_visitor_t() = default;

  /*!
   \brief Copy constructor
   */
  statement_visitor_t(tchecker::statement_visitor_t const &) = default;

  /*!
   \brief Move constructor
   */
  statement_visitor_t(tchecker::statement_visitor_t &&) = default;

  /*!
   \brief Destructor
   */
  virtual ~statement_visitor_t() = default;

  /*!
   \brief Assignment oeprator
   */
  tchecker::statement_visitor_t & operator=(tchecker::statement_visitor_t const &) = default;

  /*!
   \brief Move assignment oeprator
   */
  tchecker::statement_visitor_t & operator=(tchecker::statement_visitor_t &&) = default;

  /*!
   \brief Visitors
   */
  virtual void visit(tchecker::nop_statement_t const &) = 0;
  virtual void visit(tchecker::assign_statement_t const &) = 0;
  virtual void visit(tchecker::sequence_statement_t const &) = 0;
  virtual void visit(tchecker::if_statement_t const &) = 0;
  virtual void visit(tchecker::while_statement_t const &) = 0;
  virtual void visit(tchecker::local_var_statement_t const &) = 0;
  virtual void visit(tchecker::local_array_statement_t const &) = 0;
};

} // end namespace tchecker

#endif // TCHECKER_STATEMENT_HH
