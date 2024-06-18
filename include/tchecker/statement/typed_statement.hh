/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TYPED_STATEMENT_HH
#define TCHECKER_TYPED_STATEMENT_HH

#include <iostream>

#include "tchecker/expression/typed_expression.hh"
#include "tchecker/statement/statement.hh"

/*!
 \file typed_statement.hh
 \brief Typed statements
 */

namespace tchecker {

// Statement type

/*!
 \brief Type of statements
 */
enum statement_type_t {
  STMT_TYPE_BAD = 0,       // Bad type
  STMT_TYPE_NOP,           // No-operation
  STMT_TYPE_INTASSIGN,     // Assignment to integer variable
  STMT_TYPE_CLKASSIGN_INT, // Assignment: integer to clock variable
  STMT_TYPE_CLKASSIGN_CLK, // Assignment: clock to clock variable
  STMT_TYPE_CLKASSIGN_SUM, // Assignment: integer+clock to clock variable
  STMT_TYPE_SEQ,           // Sequence of statements
  STMT_TYPE_IF,            // If-Then-Else statement
  STMT_TYPE_WHILE,         // While statement
  STMT_TYPE_LOCAL_INT,     // Local variable declaration
  STMT_TYPE_LOCAL_ARRAY,   // Local array declaration
};

/*!
 \brief Output operator for statement types
 \param os : output stream
 \param type : statement type
 \post type has been output to os
 \return os after output
 */
std::ostream & operator<<(std::ostream & os, enum tchecker::statement_type_t type);

// Typed statements

class typed_nop_statement_t; // forward declarations
class typed_assign_statement_t;
class typed_int_to_clock_assign_statement_t;
class typed_clock_to_clock_assign_statement_t;
class typed_sum_to_clock_assign_statement_t;
class typed_sequence_statement_t;
class typed_if_statement_t;
class typed_while_statement_t;
class typed_local_var_statement_t;
class typed_local_array_statement_t;

/*!
 \class typed_statement_visitor_t
 \brief Visitor pattern for typed statements
 */
class typed_statement_visitor_t {
public:
  /*!
   \brief Constructor
   */
  typed_statement_visitor_t() = default;

  /*!
   \brief Copy constructor
   */
  typed_statement_visitor_t(tchecker::typed_statement_visitor_t const &) = default;

  /*!
   \brief Move constructor
   */
  typed_statement_visitor_t(tchecker::typed_statement_visitor_t &&) = default;

  /*!
   \brief Destructor
   */
  virtual ~typed_statement_visitor_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::typed_statement_visitor_t & operator=(tchecker::typed_statement_visitor_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::typed_statement_visitor_t & operator=(tchecker::typed_statement_visitor_t &&) = default;

  /*!
   \brief Visitors
   */
  virtual void visit(tchecker::typed_nop_statement_t const &) = 0;
  virtual void visit(tchecker::typed_assign_statement_t const &) = 0;
  virtual void visit(tchecker::typed_int_to_clock_assign_statement_t const &) = 0;
  virtual void visit(tchecker::typed_clock_to_clock_assign_statement_t const &) = 0;
  virtual void visit(tchecker::typed_sum_to_clock_assign_statement_t const &) = 0;
  virtual void visit(tchecker::typed_sequence_statement_t const &) = 0;
  virtual void visit(tchecker::typed_if_statement_t const &) = 0;
  virtual void visit(tchecker::typed_while_statement_t const &) = 0;
  virtual void visit(tchecker::typed_local_var_statement_t const &) = 0;
  virtual void visit(tchecker::typed_local_array_statement_t const &) = 0;
};

/*!
 \class typed_statement_t
 \brief Statement along with its type
 */
class typed_statement_t : public virtual tchecker::statement_t {
public:
  /*!
   \brief Constructor
   \param type : statement type
   */
  typed_statement_t(enum tchecker::statement_type_t type);

  /*!
   \brief Destructor
   */
  virtual ~typed_statement_t();

  /*!
   \brief Accessor
   \return this statement's type
   */
  inline enum tchecker::statement_type_t type() const { return _type; }

  /*!
   \brief Clone
   \return A clone of this statement
   */
  virtual tchecker::typed_statement_t * clone() const override = 0;

  /*!
   \brief Visit
   \param v : typed statement visitor
   */
  virtual void visit(tchecker::typed_statement_visitor_t & v) const = 0;

  /*!
   \brief Untyped visit
   \param v : statement visitor
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override = 0;

protected:
  enum tchecker::statement_type_t const _type; /*!< Statement type */
};

/*!
 \class typed_nop_statement_t
 \brief Typed nop statement
 */
class typed_nop_statement_t : public tchecker::typed_statement_t, public tchecker::nop_statement_t {
public:
  /*!
   \brief Constructor
   \param type : statement type
   */
  typed_nop_statement_t(enum tchecker::statement_type_t type);

  /*!
   \brief Destructor
   */
  virtual ~typed_nop_statement_t();

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_nop_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed statement visitor
   */
  virtual void visit(tchecker::typed_statement_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : statement visitor
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;
};

/*!
 \class typed_assign_statement_t
 \brief Typed assign statement
 */
class typed_assign_statement_t : public tchecker::typed_statement_t, public tchecker::assign_statement_t {
public:
  /*!
   \brief Constructor
   \param type : statement type
   \param lvalue : left value expression
   \param rvalue : right value expression
   \pre lvalue->size() == 1 (assignable)
   \post this keeps poiinters on lvalue and on rvalue and represents statement `lvalue=rvalue` with type
   \throw std::invalid_argument : if precondition is violated
   \note this takes ownership on lvalue and rvalue
   */
  typed_assign_statement_t(enum tchecker::statement_type_t type,
                           std::shared_ptr<tchecker::typed_lvalue_expression_t const> const & lvalue,
                           std::shared_ptr<tchecker::typed_expression_t const> const & rvalue);

  /*!
   \brief Destructor
   */
  virtual ~typed_assign_statement_t();

  /*!
   \brief Accessor
   \return typed left value
   */
  tchecker::typed_lvalue_expression_t const & lvalue() const;

  /*!
   \brief Accessor
   \return shared pointer to typed left value
   */
  std::shared_ptr<tchecker::typed_lvalue_expression_t const> lvalue_ptr() const;

  /*!
   \brief Accessor
   \return typed right value
   */
  tchecker::typed_expression_t const & rvalue() const;

  /*!
   \brief Accessor
   \return shared pointer to typed right value
   */
  std::shared_ptr<tchecker::typed_expression_t const> rvalue_ptr() const;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_assign_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed statement isitor
   */
  virtual void visit(tchecker::typed_statement_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : statement visitor
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;
};

/*!
 \class typed_int_to_clock_assign_statement_t
 \brief Typed int-to-clock assign statement (x := c)
 */
class typed_int_to_clock_assign_statement_t : public tchecker::typed_assign_statement_t {
public:
  /*!
   \brief Constructor
   \param type : statement type
   \param lvalue : left value expression
   \param rvalue : right value expression
   \pre lvalue->size() == 1 (assignable)
   \pre lvalue is an assignable clock
   \pre rvalue is integer valued
   \post this keeps pointers to lvalue and rvalue, and represents assignement `lvalue=rvalue` with type
   \throw std::invalid_argument : if the size of lvalue is greater than 1, or if lvalue is not an
   assignable clock, or if ravlue is not integer valued
   */
  typed_int_to_clock_assign_statement_t(enum tchecker::statement_type_t type,
                                        std::shared_ptr<tchecker::typed_lvalue_expression_t const> const & lvalue,
                                        std::shared_ptr<tchecker::typed_expression_t const> const & rvalue);

  /*!
   \brief Destructor
   */
  virtual ~typed_int_to_clock_assign_statement_t();

  /*!
   \brief Accessor
   \return clock expression (i.e. x in x:=c)
   */
  tchecker::typed_lvalue_expression_t const & clock() const;

  /*!
   \brief Accessor
   \return shared pointer to clock expression x in x:=c
   */
  std::shared_ptr<tchecker::lvalue_expression_t const> clock_ptr() const;

  /*!
   \brief Accessor
   \return value expression (i.e. c in x:=c)
   */
  tchecker::typed_expression_t const & value() const;

  /*!
   \brief Accessor
   \return shared pointer to value expression c in x:=c
   */
  std::shared_ptr<tchecker::typed_expression_t const> value_ptr() const;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::typed_int_to_clock_assign_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed statement visitor
   */
  virtual void visit(tchecker::typed_statement_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : statement visitor
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;
};

/*!
 \class typed_clock_to_clock_assign_statement_t
 \brief Typed clock-to-clock assign statement (x := y)
 */
class typed_clock_to_clock_assign_statement_t : public tchecker::typed_assign_statement_t {
public:
  /*!
   \brief Constructor
   \param type : statement type
   \param lvalue : left value expression
   \param rvalue : right value expression
   \pre lvalue->size() == 1 (assignable)
   \pre lvalue is an assignable clock
   \pre rvalue is clock valued
   \post this keeps pointers to lvalue and rvalue, and represents `lavlue = rvalue` with type
   \throw std::invalid_argument : if the size of lvalue is greater than 1, or if lvalue is not an
   assignable clock, or if rvalue is not clock valued
   */
  typed_clock_to_clock_assign_statement_t(enum tchecker::statement_type_t type,
                                          std::shared_ptr<tchecker::typed_lvalue_expression_t const> const & lvalue,
                                          std::shared_ptr<tchecker::typed_lvalue_expression_t const> const & rvalue);

  /*!
   \brief Destructor
   */
  virtual ~typed_clock_to_clock_assign_statement_t();

  /*!
   \brief Accessor
   \return lvalue clock expression (i.e. x in x:=y)
   */
  tchecker::typed_lvalue_expression_t const & lclock() const;

  /*!
   \brief Accessor
   \return shared pointer to lvalue clock expression x in x:=y
   */
  std::shared_ptr<tchecker::typed_lvalue_expression_t const> lclock_ptr() const;

  /*!
   \brief Accessor
   \return rvalue value expression (i.e. y in x:=y)
   */
  tchecker::typed_lvalue_expression_t const & rclock() const;

  /*!
   \brief Accessor
   \return shared pointer to rvalue clock expression y in x:=y
   */
  std::shared_ptr<tchecker::typed_lvalue_expression_t const> rclock_ptr() const;

  /*!
   \brief Clone
   \return a clone of this
   */
  virtual tchecker::typed_clock_to_clock_assign_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed statement visitor
   */
  virtual void visit(tchecker::typed_statement_visitor_t & v) const override;

  /*!
 \brief Untyped visit
 \param v : statement visitor
 */
  virtual void visit(tchecker::statement_visitor_t & v) const override;
};

/*!
 \class typed_sum_to_clock_assign_statement_t
 \brief Typed sum-to-clock assign statement (x := c+y)
 */
class typed_sum_to_clock_assign_statement_t : public tchecker::typed_assign_statement_t {
public:
  /*!
   \brief Constructor
   \param type : statement type
   \param lvalue : left value expression
   \param rvalue : right value expression
   \pre lvalue->size() == 1 (assignable)
   \pre lvalue is an assignable clock
   \pre rvalue has type EXPR_TYPE_INTCLKSUM
   \post this keeps pointers to lvalue and rvalue, representing statement `lvalue = rvalue` with type
   \throw std::invalid_argument : if lvalue has size greater than 1, or if lvalue is not an
   assignable clock, or if rvalue does not have type EXPR_TYPE_INTCLKSUM
   */
  typed_sum_to_clock_assign_statement_t(enum tchecker::statement_type_t type,
                                        std::shared_ptr<tchecker::typed_lvalue_expression_t const> const & lvalue,
                                        std::shared_ptr<tchecker::typed_expression_t const> const & rvalue);

  /*!
   \brief Destructor
   */
  virtual ~typed_sum_to_clock_assign_statement_t();

  /*!
   \brief Accessor
   \return lvalue clock expression (i.e. x in x:=c+y)
   */
  tchecker::typed_lvalue_expression_t const & lclock() const;

  /*!
   \brief Accessor
   \return shared pointer to lvalue clock expression x in x:=c+y
   */
  std::shared_ptr<tchecker::typed_lvalue_expression_t const> lclock_ptr() const;

  /*!
   \brief Accessor
   \return rvalue clock expression (i.e. y in x:=c+y)
   */
  tchecker::typed_lvalue_expression_t const & rclock() const;

  /*!
   \brief Accessor
   \return shared pointer to lvalue clock expression y in x:=c+y
   */
  std::shared_ptr<tchecker::typed_lvalue_expression_t const> rclock_ptr() const;

  /*!
   \brief Accessor
   \return value expression (i.e. c in x:=c+y)
   */
  tchecker::typed_expression_t const & value() const;

  /*!
   \brief Accessor
   \return shared pointer to value expression c in x:=c+y
   */
  std::shared_ptr<tchecker::typed_expression_t const> value_ptr() const;

  /*!
   \brief Clone
   \return A clone of this
   */
  virtual tchecker::typed_sum_to_clock_assign_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed statement visitor
   */
  virtual void visit(tchecker::typed_statement_visitor_t & v) const override;

  /*!
   \brief Untyped isit
   \param v : statement visitor
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;
};

/*!
 \class typed_sequence_statement_t
 \brief Typed sequence statement
 */
class typed_sequence_statement_t : public tchecker::typed_statement_t, public tchecker::sequence_statement_t {
public:
  /*!
   \brief Constructor
   \param type : statement type
   \param first : first statement
   \param second : second statement
   \post this keeps pointers to first and second, representing statement `first; second`
   \note this takes ownership on first and second
   */
  typed_sequence_statement_t(enum tchecker::statement_type_t type,
                             std::shared_ptr<tchecker::typed_statement_t const> const & first,
                             std::shared_ptr<tchecker::typed_statement_t const> const & second);

  /*!
   \brief Destructor
   */
  virtual ~typed_sequence_statement_t();

  /*!
   \brief Accessor
   \return typed first statement
   */
  tchecker::typed_statement_t const & first() const;

  /*!
   \brief Accessor
   \return shared pointer to typed first statement
   */
  std::shared_ptr<tchecker::typed_statement_t const> first_ptr() const;

  /*!
   \brief Accessor
   \return typed second statement
   */
  tchecker::typed_statement_t const & second() const;

  /*!
   \brief Accessor
   \return shared pointer to typed second statement
   */
  std::shared_ptr<tchecker::typed_statement_t const> second_ptr() const;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_sequence_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed statement visitor
   */
  virtual void visit(tchecker::typed_statement_visitor_t & v) const override;

  /*!
   \brief Untyped tisit
   \param v : statement visitor
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;
};

/*!
 \class typed_if_statement_t
 \brief Typed if statement
 */
class typed_if_statement_t : public tchecker::typed_statement_t, public tchecker::if_statement_t {

public:
  /*!
   \brief Constructor
   \param type : statement type
   \param cond : condition expression
   \param then_stmt : first statement
   \param else_stmt : second statement
   \post this keeps pointers on cond, on then_stmt and on else_stmt, representing statement
   `if (cond) then then_stmt else else_stmt` with type
   \note this takes ownership on parameters
   */
  typed_if_statement_t(enum tchecker::statement_type_t type, std::shared_ptr<tchecker::typed_expression_t const> const & cond,
                       std::shared_ptr<tchecker::typed_statement_t const> const & then_stmt,
                       std::shared_ptr<tchecker::typed_statement_t const> const & else_stmt);

  /*!
   \brief Destructor
   */
  virtual ~typed_if_statement_t();

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
   \return typed then-statement
   */
  tchecker::typed_statement_t const & then_stmt() const;

  /*!
   \brief Accessor
   \return shared pointer to typed then-statement
   */
  std::shared_ptr<tchecker::typed_statement_t const> then_stmt_ptr() const;

  /*!
   \brief Accessor
   \return typed else-statement
   */
  tchecker::typed_statement_t const & else_stmt() const;

  /*!
   \brief Accessor
   \return shared pointer to typed else-statement
   */
  std::shared_ptr<tchecker::typed_statement_t const> else_stmt_ptr() const;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_if_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed statement visitor
   */
  virtual void visit(tchecker::typed_statement_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : statement visitor
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;
};

/*!
 \class typed_while_statement_t
 \brief Typed while statement
 */
class typed_while_statement_t : public tchecker::typed_statement_t, public tchecker::while_statement_t {
public:
  /*!
   \brief Constructor
   \param type : statement type
   \param cond : condition expression
   \param stmt : iterated statement
   \post this keeps pointers on cond and on stmt, representing statement `while (cond) do stmt` with type
   \note this takes ownership on parameters
   */
  typed_while_statement_t(enum tchecker::statement_type_t type,
                          std::shared_ptr<tchecker::typed_expression_t const> const & cond,
                          std::shared_ptr<tchecker::typed_statement_t const> const & stmt);

  /*!
   \brief Destructor
   */
  virtual ~typed_while_statement_t();

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
   \return typed statement
   */
  tchecker::typed_statement_t const & statement() const;

  /*!
   \brief Accessor
   \return shared pointer to typed statement
   */
  std::shared_ptr<tchecker::typed_statement_t const> statement_ptr() const;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_while_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed statement visitor
   */
  virtual void visit(tchecker::typed_statement_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : statement visitor
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;
};

/*!
 \class typed_local_var_statement_t
 \brief Typed local variable declaration
 */
class typed_local_var_statement_t : public tchecker::typed_statement_t, public tchecker::local_var_statement_t {
public:
  /*!
   \brief Constructor
   \param type : statement type
   \param variable : the local variable
   \post this keeps a pointer to variable, representing declaration of local variable with type.
   The variable is initialized to 0
   */
  typed_local_var_statement_t(enum tchecker::statement_type_t type,
                              std::shared_ptr<tchecker::typed_var_expression_t const> const & variable);

  /*!
   \brief Constructor
   \param type : statement type
   \param variable : the local variable
   \param init : initial value
   \post this keeps a pointer to variable and a pointer to init, representing declaration
   of local variable initialized to init
   */
  typed_local_var_statement_t(enum tchecker::statement_type_t type,
                              std::shared_ptr<tchecker::typed_var_expression_t const> const & variable,
                              std::shared_ptr<tchecker::typed_expression_t const> const & init);

  /*!
   \brief Destructor
   */
  virtual ~typed_local_var_statement_t();

  /*!
   \brief Accessor
   \return typed variable
   */
  tchecker::typed_var_expression_t const & variable() const;

  /*!
   \brief Accessor
   \return shared pointer to typed variable
   */
  std::shared_ptr<tchecker::typed_var_expression_t const> variable_ptr() const;

  /*!
   \brief Accessor
   \return typed initial value
   */
  tchecker::typed_expression_t const & initial_value() const;

  /*!
   \brief Accessor
   \return shared pointer to typed initial value
   */
  std::shared_ptr<tchecker::typed_expression_t const> initial_value_ptr() const;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_local_var_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed statement visitor
   */
  virtual void visit(tchecker::typed_statement_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : statement visitor
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;
};

/*!
 \class typed_local_array_statement_t
 \brief Typed local array declaration statement
 */
class typed_local_array_statement_t : public tchecker::typed_statement_t, public tchecker::local_array_statement_t {
public:
  /*!
   \brief Constructor
   \param type : statement type
   \param variable : name of the array
   \param size : expression that represents the size of the array
   \note this takes ownership on parameters
   */
  typed_local_array_statement_t(enum tchecker::statement_type_t type,
                                std::shared_ptr<tchecker::typed_var_expression_t const> const & variable,
                                std::shared_ptr<tchecker::typed_expression_t const> const & size);

  /*!
   \brief Destructor
   */
  virtual ~typed_local_array_statement_t();

  /*!
   \brief Accessor
   \return typed variable
   */
  tchecker::typed_var_expression_t const & variable() const;

  /*!
   \brief Accessor
   \return shared pointer to typed variable
   */
  std::shared_ptr<tchecker::typed_var_expression_t const> variable_ptr() const;

  /*!
   \brief Accessor
   \return typed size
   */
  tchecker::typed_expression_t const & size() const;

  /*!
   \brief Accessor
   \return shared pointer to typed size
   */
  std::shared_ptr<tchecker::typed_expression_t const> size_ptr() const;

  /*!
   \brief Clone
   \return clone of this
   */
  virtual tchecker::typed_local_array_statement_t * clone() const override;

  /*!
   \brief Visit
   \param v : typed statement visitor
   */
  virtual void visit(tchecker::typed_statement_visitor_t & v) const override;

  /*!
   \brief Untyped visit
   \param v : statement visitor
   */
  virtual void visit(tchecker::statement_visitor_t & v) const override;
};

} // end of namespace tchecker

#endif // TCHECKER_TYPED_STATEMENT_HH
