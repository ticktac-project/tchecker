/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STATEMENT_TYPED_HH
#define TCHECKER_STATEMENT_TYPED_HH

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
    STMT_TYPE_BAD = 0,         // Bad type
    STMT_TYPE_NOP,             // No-operation
    STMT_TYPE_INTASSIGN,       // Assignment to integer variable
    STMT_TYPE_CLKASSIGN_INT,   // Assignment: integer to clock variable
    STMT_TYPE_CLKASSIGN_CLK,   // Assignment: clock to clock variable
    STMT_TYPE_CLKASSIGN_SUM,   // Assignment: integer+clock to clock variable
    STMT_TYPE_SEQ,             // Sequence of statements
    STMT_TYPE_IF,              // If-Then-Else statement
    STMT_TYPE_WHILE,           // While statement
    STMT_TYPE_LOCAL_INT,       // Local variable declaration
    STMT_TYPE_LOCAL_ARRAY,     // Local array declaration
  };
  
  
  /*!
   \brief Output operator for statement types
   \param os : output stream
   \param type : statement type
   \post type has been output to os
   \return os after output
   */
  std::ostream & operator<< (std::ostream & os, enum tchecker::statement_type_t type);
  
  
  
  
  // Typed statements
  
  class typed_nop_statement_t;      // forward declarations
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
    tchecker::typed_statement_visitor_t & operator= (tchecker::typed_statement_visitor_t const &) = default;
    
    /*!
     \brief Move assignment operator
     */
    tchecker::typed_statement_visitor_t & operator= (tchecker::typed_statement_visitor_t &&) = default;
    
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
     \brief Accessor
     \return this statement's type
     */
    inline enum tchecker::statement_type_t type() const
    {
      return _type;
    }
    
    
    /*!
     \brief Visit
     \param v : typed statement visitor
     */
    void visit(tchecker::typed_statement_visitor_t & v) const;
    
    using tchecker::statement_t::visit;     // untyped visitor
  protected:
    /*!
     \brief Visit
     \param v : typed statement visitor
     */
    virtual void do_visit(tchecker::typed_statement_visitor_t & v) const = 0;
    
    using tchecker::statement_t::do_visit;  // untyped visitor
    
    
    enum tchecker::statement_type_t const _type;  /*!< Statement type */
  };
  
  
  
  
  /*!
   \class make_typed_statement_t
   \tparam STMT : type of statement
   \brief Typed statement maker
   */
  template <class STMT>
  class make_typed_statement_t : public tchecker::typed_statement_t, public STMT {
  public:
    /*!
     \brief Constructor
     \param type : statement type
     \param args : arguments to a constructor of STMT
     */
    template <class ... ARGS>
    make_typed_statement_t(enum tchecker::statement_type_t type, ARGS && ... args)
    : tchecker::typed_statement_t(type), STMT(args...)
    {}
  protected:
    /*!
     \brief Output the statement
     \param os : output stream
     */
    virtual std::ostream & do_output(std::ostream & os) const
    {
      return this->STMT::do_output(os);
    }
    
    
    /*!
     \brief Visit
     \param v : untyped statement visitor
     */
    virtual void do_visit(tchecker::statement_visitor_t & v) const
    {
      this->STMT::do_visit(v);
    }
    
    
    /*!
     \brief Visit
     \param v : typed statement visitor
     */
    virtual void do_visit(tchecker::typed_statement_visitor_t & v) const = 0;
  };
  
  
  
  
  /*!
   \class typed_nop_statement_t
   \brief Typed nop statement
   */
  class typed_nop_statement_t : public tchecker::make_typed_statement_t<tchecker::nop_statement_t> {
  public:
    // Constructor
    using tchecker::make_typed_statement_t<tchecker::nop_statement_t>::make_typed_statement_t;
  protected:
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::statement_t * do_clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_statement_visitor_t & v) const;
    
    using tchecker::make_typed_statement_t<tchecker::nop_statement_t>::do_visit;
  };
  
  
  
  
  /*!
   \class typed_assign_statement_t
   \brief Typed assign statement
   */
  class typed_assign_statement_t : public tchecker::make_typed_statement_t<tchecker::assign_statement_t> {
  public:
    /*!
     \brief Constructor
     \param type : statement type
     \param lvalue : left value expression
     \param rvalue : right value expression
     \pre lvalue->size() == 1 (assignable)
     \throw std::invalid_argument : if precondition is violated
     \note this takes ownership on lvalue and rvalue
     */
    typed_assign_statement_t(enum tchecker::statement_type_t type,
                             tchecker::typed_lvalue_expression_t const * lvalue,
                             tchecker::typed_expression_t const * rvalue);
    
    
    /*!
     \brief Accessor
     \return left value as a typed expression
     */
    inline tchecker::typed_lvalue_expression_t const & lvalue() const
    {
      return
      dynamic_cast<tchecker::typed_lvalue_expression_t const &>
      (tchecker::make_typed_statement_t<tchecker::assign_statement_t>::lvalue());
    }
    
    
    /*!
     \brief Accessor
     \return right value as a typed expression
     */
    inline tchecker::typed_expression_t const & rvalue() const
    {
      return
      dynamic_cast<tchecker::typed_expression_t const &>
      (tchecker::make_typed_statement_t<tchecker::assign_statement_t>::rvalue());
    }
  protected:
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::statement_t * do_clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_statement_visitor_t & v) const;
    
    using tchecker::make_typed_statement_t<tchecker::assign_statement_t>::do_visit;
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
     \throw std::invalid_argument : if precondition is violated
     \note this takes ownership on lvalue and rvalue
     */
    typed_int_to_clock_assign_statement_t(enum tchecker::statement_type_t type,
                                          tchecker::typed_lvalue_expression_t const * lvalue,
                                          tchecker::typed_expression_t const * rvalue);
    
    
    /*!
     \brief Accessor
     \return clock expression (i.e. x in x:=c)
     */
    inline tchecker::typed_lvalue_expression_t const & clock() const
    {
      return lvalue();
    }
    
    
    /*!
     \brief Accessor
     \return value expression (i.e. c in x:=c)
     */
    inline tchecker::typed_expression_t const & value() const
    {
      return rvalue();
    }
  protected:
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_statement_visitor_t & v) const;
    
    using tchecker::typed_assign_statement_t::do_visit;
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
     \throw std::invalid_argument : if precondition is violated
     \note this takes ownership on lvalue and rvalue
     */
    typed_clock_to_clock_assign_statement_t(enum tchecker::statement_type_t type,
                                            tchecker::typed_lvalue_expression_t const * lvalue,
                                            tchecker::typed_lvalue_expression_t const * rvalue);
    
    
    /*!
     \brief Accessor
     \return lvalue clock expression (i.e. x in x:=y)
     */
    inline tchecker::typed_lvalue_expression_t const & lclock() const
    {
      return lvalue();
    }
    
    
    /*!
     \brief Accessor
     \return rvalue value expression (i.e. y in x:=y)
     */
    inline tchecker::typed_lvalue_expression_t const & rclock() const
    {
      return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(rvalue());
    }
  protected:
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_statement_visitor_t & v) const;
    
    using tchecker::typed_assign_statement_t::do_visit;
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
     \pre lvalue->size() == 1 (assignable), and rvalue has type EXPR_TYPE_INTCLKSUM
     \throw std::invalid_argument : if precondition is violated
     \note this takes ownership on lvalue and rvalue
     */
    typed_sum_to_clock_assign_statement_t(enum tchecker::statement_type_t type,
                                          tchecker::typed_lvalue_expression_t const * lvalue,
                                          tchecker::typed_expression_t const * rvalue);
    
    
    /*!
     \brief Accessor
     \return lvalue clock expression (i.e. x in x:=c+y)
     */
    inline tchecker::typed_lvalue_expression_t const & lclock() const
    {
      return lvalue();
    }
    
    
    /*!
     \brief Accessor
     \return rvalue clock expression (i.e. y in x:=c+y)
     */
    inline tchecker::typed_lvalue_expression_t const & rclock() const
    {
      auto const & sum = dynamic_cast<tchecker::typed_binary_expression_t const &>(rvalue());
      return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(sum.right_operand());
    }
    
    
    /*!
     \brief Accessor
     \return value expression (i.e. c in x:=c+y)
     */
    inline tchecker::typed_expression_t const & value() const
    {
      auto const & sum = dynamic_cast<tchecker::typed_binary_expression_t const &>(rvalue());
      return dynamic_cast<tchecker::typed_expression_t const &>(sum.left_operand());
    }
  protected:
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_statement_visitor_t & v) const;
    
    using tchecker::typed_assign_statement_t::do_visit;
  };
  
  
  
  
  /*!
   \class typed_sequence_statement_t
   \brief Typed sequence statement
   */
  class typed_sequence_statement_t : public tchecker::make_typed_statement_t<tchecker::sequence_statement_t> {
  public:
    /*!
     \brief Constructor
     \param type : statement type
     \param first : first statement
     \param second : second statement
     \note this takes ownership on first and second
     */
    typed_sequence_statement_t(enum tchecker::statement_type_t type,
                               tchecker::typed_statement_t const * first,
                               tchecker::typed_statement_t const * second);
    
    
    /*!
     \brief Accessor
     \return first statement
     */
    inline tchecker::typed_statement_t const & first() const
    {
      return
      dynamic_cast<tchecker::typed_statement_t const &>
      (tchecker::make_typed_statement_t<tchecker::sequence_statement_t>::first());
    }
    
    
    /*!
     \brief Accessor
     \return second statement
     */
    inline tchecker::typed_statement_t const & second() const
    {
      return
      dynamic_cast<tchecker::typed_statement_t const &>
      (tchecker::make_typed_statement_t<tchecker::sequence_statement_t>::second());
    }
  protected:
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::statement_t * do_clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_statement_visitor_t & v) const;
    
    using tchecker::make_typed_statement_t<tchecker::sequence_statement_t>::do_visit;
  };
  
  /*!
   \class typed_if_statement_t
   \brief Typed if statement
   */
  class typed_if_statement_t : public tchecker::make_typed_statement_t<tchecker::if_statement_t> {
  public:
    /*!
     \brief Constructor
     \param type : statement type
     \param cond : condition expression
     \param then_stmt : first statement
     \param else_stmt : second statement
     \note this takes ownership on parameters
     */
    typed_if_statement_t(enum tchecker::statement_type_t type,
                               tchecker::typed_expression_t const * cond,
                               tchecker::typed_statement_t const * then_stmt,
                               tchecker::typed_statement_t const * else_stmt);


    /*!
     \brief Accessor
     \return Condition expression
     */
    inline tchecker::typed_expression_t const & condition() const
    {
      return
      dynamic_cast<tchecker::typed_expression_t const &>
      (tchecker::make_typed_statement_t<tchecker::if_statement_t>::condition());
    }

    /*!
     \brief Accessor
     \return Then statement
     */
    inline tchecker::typed_statement_t const & then_stmt() const
    {
      return
      dynamic_cast<tchecker::typed_statement_t const &>
      (tchecker::make_typed_statement_t<tchecker::if_statement_t>::then_stmt ());
    }


    /*!
     \brief Accessor
     \return Else statement
     */
    inline tchecker::typed_statement_t const & else_stmt() const
    {
      return
      dynamic_cast<tchecker::typed_statement_t const &>
      (tchecker::make_typed_statement_t<tchecker::if_statement_t>::else_stmt ());
    }
  protected:
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::statement_t * do_clone() const;

    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_statement_visitor_t & v) const;

    using tchecker::make_typed_statement_t<tchecker::if_statement_t>::do_visit;
  };

  /*!
   \class typed_while_statement_t
   \brief Typed while statement
   */
  class typed_while_statement_t : public tchecker::make_typed_statement_t<tchecker::while_statement_t> {
  public:
    /*!
     \brief Constructor
     \param type : statement type
     \param cond : condition expression
     \param stmt : iterated statement
     \note this takes ownership on parameters
     */
    typed_while_statement_t(enum tchecker::statement_type_t type,
                            tchecker::typed_expression_t const * cond,
                            tchecker::typed_statement_t const * stmt);

    /*!
     \brief Accessor
     \return Condition expression
     */
    inline tchecker::typed_expression_t const & condition() const
    {
      return
      dynamic_cast<tchecker::typed_expression_t const &>
      (tchecker::make_typed_statement_t<tchecker::while_statement_t>::condition());
    }

    /*!
     \brief Accessor
     \return Then statement
     */
    inline tchecker::typed_statement_t const & statement() const
    {
      return
      dynamic_cast<tchecker::typed_statement_t const &>
      (tchecker::make_typed_statement_t<tchecker::while_statement_t>::statement ());
    }

  protected:
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::statement_t * do_clone() const;

    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_statement_visitor_t & v) const;

    using tchecker::make_typed_statement_t<tchecker::while_statement_t>::do_visit;
  };


  /*!
   \class typed_local_var_statement_t
   \brief Typed local variable declaration
   */
  class typed_local_var_statement_t : public tchecker::make_typed_statement_t<tchecker::local_var_statement_t> {
  public:
    /*!
     \brief Constructor
     \param type : statement type
     \param variable : the local variable
     */
    typed_local_var_statement_t(enum tchecker::statement_type_t type,
                                tchecker::typed_var_expression_t const *variable);

    /*!
     \brief Constructor
     \param type : statement type
     \param variable : the local variable
     \param init : the initial value assigned to \a variable
     */
    typed_local_var_statement_t(enum tchecker::statement_type_t type,
                                tchecker::typed_var_expression_t const *variable,
                                tchecker::typed_expression_t const *init);

    /*!
     \brief Accessor
     \return Variable
     */
    inline tchecker::typed_var_expression_t const & variable() const
    {
      return dynamic_cast<tchecker::typed_var_expression_t const &>
      (tchecker::make_typed_statement_t<tchecker::local_var_statement_t>::variable());
    }

    /*!
     \brief Accessor
     \return Initial value
     */
    inline tchecker::typed_expression_t const & initial_value() const
    {
      return dynamic_cast<tchecker::typed_expression_t const &>
      (tchecker::make_typed_statement_t<tchecker::local_var_statement_t>::initial_value());
    }

  protected:
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::statement_t * do_clone() const;

    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_statement_visitor_t & v) const;

    using tchecker::make_typed_statement_t<tchecker::local_var_statement_t>::do_visit;
  };

  /*!
   \class typed_local_array_statement_t
   \brief Typed local array declaration statement
   */
  class typed_local_array_statement_t : public tchecker::make_typed_statement_t<tchecker::local_array_statement_t> {
  public:
    /*!
     \brief Constructor
     \param type : statement type
     \param variable : name of the array
     \param size : expression that represents the size of the array
     \note this takes ownership on parameters
     */
    typed_local_array_statement_t(enum tchecker::statement_type_t type,
                                  tchecker::typed_var_expression_t const *variable,
                                  tchecker::typed_expression_t const * size);

    inline tchecker::typed_var_expression_t const & variable() const
    {
      return dynamic_cast<tchecker::typed_var_expression_t const &>
      (tchecker::make_typed_statement_t<tchecker::local_array_statement_t>::variable ());
    }

    /*!
     \brief Accessor
     \return size of the array
     */
    inline tchecker::typed_expression_t const & size() const
    {
      return
      dynamic_cast<tchecker::typed_expression_t const &>
      (tchecker::make_typed_statement_t<tchecker::local_array_statement_t>::size());
    }

  protected:
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::statement_t * do_clone() const;

    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_statement_visitor_t & v) const;

    using tchecker::make_typed_statement_t<tchecker::local_array_statement_t>::do_visit;
  };


} // end of namespace tchecker

#endif // TCHECKER_STATEMENT_TYPED_HH
