/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_STATEMENT_HH
#define TCHECKER_STATEMENT_HH

#include <iostream>
#include <string>

#include <boost/core/noncopyable.hpp>

#include "tchecker/expression/expression.hh"

/*!
 \file statement.hh
 \brief Syntax tree for statements
 */

namespace tchecker {
  
  class statement_visitor_t;  // forward declaration
  
  
  
  
  /*!
   \class statement_t
   \brief Base class for statements
   */
  class statement_t : private boost::noncopyable {
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
     \brief Assignment operator (DELETED)
     */
    tchecker::statement_t & operator= (tchecker::statement_t const &) = delete;
    
    /*!
     \brief Move assignment operator (DELETED)
     */
    tchecker::statement_t & operator= (tchecker::statement_t &&) = delete;
    
    /*!
     \brief Clone
     \return A clone of this
     */
    tchecker::statement_t * clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     */
    void visit(tchecker::statement_visitor_t & v) const;
    
    /*!
     \brief Accessor
     \return this statement as a string
     */
    std::string to_string() const;
  protected:
    /*!
     \brief Output the statement
     \param os : output stream
     */
    virtual std::ostream & do_output(std::ostream & os) const = 0;
    
    /*!
     \brief Clone
     */
    virtual tchecker::statement_t * do_clone() const = 0;
    
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::statement_visitor_t & v) const =0;
    
    friend std::ostream & operator<< (std::ostream & os, tchecker::statement_t const & stmt);
  };
  
  
  
  /*!
   \brief Output operator for statements
   \param os : output stream
   \param stmt : statement
   \post stmt has been output to os
   \return os after stmt has been output
   */
  std::ostream & operator<< (std::ostream & os, tchecker::statement_t const & stmt);
  
  
  
  
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
  protected:
    /*!
     \brief Output the statement
     \param os : output stream
     \post this has been output to os
     \return os after this has been output
     */
    virtual std::ostream & do_output(std::ostream & os) const;
    
    /*!
     \brief Clone
     \return A clone of this
     */
    virtual tchecker::statement_t * do_clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     \post v.visit(*this) has been called
     */
    virtual void do_visit(tchecker::statement_visitor_t & v) const;
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
     \throw std::invalid_argument : if one of lvalue and rvalue is nullptr
     \note this takes ownerhsip on lvalue and rvalue
     */
    assign_statement_t(tchecker::lvalue_expression_t const * lvalue, tchecker::expression_t const * rvalue);
    
    /*!
     \brief Destructor
     */
    virtual ~assign_statement_t();
    
    /*!
     \brief Accessor
     \return Left value
     */
    inline tchecker::lvalue_expression_t const & lvalue() const
    {
      return (* _lvalue);
    }
    
    /*!
     \brief Accessor
     \return Right value
     */
    inline tchecker::expression_t const & rvalue() const
    {
      return (* _rvalue);
    }
  protected:
    /*!
     \brief Output the statement
     \param os : output stream
     \post this has been output to os
     \return os after this has been output
     */
    virtual std::ostream & do_output(std::ostream & os) const;
    
    /*!
     \brief Clone
     \return A clone of this
     */
    virtual tchecker::statement_t * do_clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     \post v.visit(*this) has been called
     */
    virtual void do_visit(tchecker::statement_visitor_t & v) const;
    
    tchecker::lvalue_expression_t const * _lvalue;   /*!< Left value */
    tchecker::expression_t const * _rvalue;          /*!< Right value*/
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
     \throw std::invalid_argument : if one of first and second is nullptr
     \note this takes ownership on first and second
     */
    sequence_statement_t(tchecker::statement_t const * first, tchecker::statement_t const * second);
    
    /*!
     \brief Destructor
     */
    virtual ~sequence_statement_t();
    
    /*!
     \brief Accessor
     \return First statement
     */
    inline tchecker::statement_t const & first() const
    {
      return (* _first);
    }
    
    /*!
     \brief Accessor
     \return Second statement
     */
    inline tchecker::statement_t const & second() const
    {
      return (* _second);
    }
  protected:
    /*!
     \brief Output the statement
     \param os : output stream
     \post this has been output to os
     \return os after this has been output
     */
    virtual std::ostream & do_output(std::ostream & os) const;
    
    /*!
     \brief Clone
     \return A clone of this
     */
    virtual tchecker::statement_t * do_clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     \post v.visit(*this) has been called
     */
    virtual void do_visit(tchecker::statement_visitor_t & v) const;
    
    tchecker::statement_t const * _first;   /*!< First statement */
    tchecker::statement_t const * _second;  /*!< Second statement */
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
     \throw std::invalid_argument : if an parameter is nullptr
     \note this takes ownership on parameters
     */
    if_statement_t(tchecker::expression_t const *cond,
                   tchecker::statement_t const * then_stmt,
                   tchecker::statement_t const * else_stmt);

    /*!
     \brief Destructor
     */
    virtual ~if_statement_t();

    /*!
     \brief Accessor
     \return Guard of the if statement
     */
    inline tchecker::expression_t const & condition() const
    {
      return (* _condition);
    }

    /*!
     \brief Accessor
     \return Then statement
     */
    inline tchecker::statement_t const & then_stmt() const
    {
      return (* _then_stmt);
    }

    /*!
     \brief Accessor
     \return Second statement
     */
    inline tchecker::statement_t const & else_stmt() const
    {
      return (* _else_stmt);
    }
  protected:
    /*!
     \brief Output the statement
     \param os : output stream
     \post this has been output to os
     \return os after this has been output
     */
    virtual std::ostream & do_output(std::ostream & os) const;

    /*!
     \brief Clone
     \return A clone of this
     */
    virtual tchecker::statement_t * do_clone() const;

    /*!
     \brief Visit
     \param v : visitor
     \post v.visit(*this) has been called
     */
    virtual void do_visit(tchecker::statement_visitor_t & v) const;

    tchecker::expression_t const * _condition;  /*!< Guard of the if statement */
    tchecker::statement_t const * _then_stmt;  /*!< Then statement */
    tchecker::statement_t const * _else_stmt;  /*!< Else statement */
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
     \pre cond != nullptr and stmt != nullptr
     \throw std::invalid_argument : if a parameter is nullptr
     \note this takes ownership on parameters
     */
    while_statement_t(tchecker::expression_t const * cond,
                      tchecker::statement_t const * stmt);

    /*!
     \brief Destructor
     */
    virtual ~while_statement_t();

    /*!
     \brief Accessor
     \return Guard of the if statement
     */
    inline tchecker::expression_t const & condition() const
    {
      return (* _condition);
    }

    /*!
     \brief Accessor
     \return Iterated statement
     */
    inline tchecker::statement_t const & statement() const
    {
      return (* _stmt);
    }

  protected:
    /*!
     \brief Output the statement
     \param os : output stream
     \post this has been output to os
     \return os after this has been output
     */
    virtual std::ostream & do_output(std::ostream & os) const;

    /*!
     \brief Clone
     \return A clone of this
     */
    virtual tchecker::statement_t * do_clone() const;

    /*!
     \brief Visit
     \param v : visitor
     \post v.visit(*this) has been called
     */
    virtual void do_visit(tchecker::statement_visitor_t & v) const;

    tchecker::expression_t const * _condition;  /*!< Guard of the if statement */
    tchecker::statement_t const * _stmt;  /*!< Iterated statement */
  };


    /*!
     \class local_var_statement_t
     \brief declaration of a local integer variable
     */
    class local_var_statement_t : public virtual tchecker::statement_t {
     public:
      /*!
       \brief Constructor
       \param var : the new local variable
       \pre var != nullptr
       \throw std::invalid_argument : if an parameter is nullptr
       \note this takes ownership on parameterss
       */
      local_var_statement_t(tchecker::var_expression_t const *var);

      /*!
       \brief Constructor
       \param var : the new local variable
       \param init : initial value assigned to var
       \pre var != nullptr and init != nullptr
       \throw std::invalid_argument : if an parameter is nullptr
       \note this takes ownership on parameters
       */
      local_var_statement_t(tchecker::var_expression_t const *var,
                            tchecker::expression_t const *init);

      /*!
       \brief Destructor
       */
      virtual ~local_var_statement_t();

      /*!
       \brief Accessor
       \return The variable
       */
      inline tchecker::var_expression_t const & variable() const
      {
        return *_variable;
      }

      /*!
       \brief Accessor
       \return The initial value of the local variable
       */
      inline tchecker::expression_t const & initial_value() const
      {
        return *_initial_value;
      }

     protected:
      /*!
       \brief Output the statement
       \param os : output stream
       \post this has been output to os
       \return os after this has been output
       */
      virtual std::ostream & do_output(std::ostream & os) const;

      /*!
       \brief Clone
       \return A clone of this
       */
      virtual tchecker::statement_t * do_clone() const;

      /*!
       \brief Visit
       \param v : visitor
       \post v.visit(*this) has been called
       */
      virtual void do_visit(tchecker::statement_visitor_t & v) const;

      tchecker::var_expression_t const * _variable;      /*!< Variable  */
      tchecker::expression_t const * _initial_value; /*!< Initial value */
    };

    /*!
     \class local_statement_t
     \brief local declaration
     */
    class local_array_statement_t : public virtual tchecker::statement_t {
     public:
      /*!
       \brief Constructor
       \param variable: the new local variable
       \param size: size expression
       \pre variable != nullptr and size != nullptr
       \throw std::invalid_argument : if an parameter is nullptr
       \note this takes ownership on parameters
       */
      local_array_statement_t(tchecker::var_expression_t const * variable,
                              tchecker::expression_t const * size);

      /*!
       \brief Destructor
       */
      virtual ~local_array_statement_t();

      /*!
       \brief Accessor
       \return Right value
       */
      inline tchecker::var_expression_t const & variable() const
      {
        return (*_variable);
      }

      /*!
       \brief Accessor
       \return Size of the variable
       */
      inline tchecker::expression_t const & size() const
      {
        return (* _size);
      }

     protected:
      /*!
       \brief Output the statement
       \param os : output stream
       \post this has been output to os
       \return os after this has been output
       */
      virtual std::ostream & do_output(std::ostream & os) const;

      /*!
       \brief Clone
       \return A clone of this
       */
      virtual tchecker::statement_t * do_clone() const;

      /*!
       \brief Visit
       \param v : visitor
       \post v.visit(*this) has been called
       */
      virtual void do_visit(tchecker::statement_visitor_t & v) const;

      tchecker::var_expression_t const * _variable;   /*!< Variable  */
      tchecker::expression_t const     * _size;   /*!< Size of the array*/
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
    tchecker::statement_visitor_t & operator= (tchecker::statement_visitor_t const &) = default;
    
    /*!
     \brief Move assignment oeprator
     */
    tchecker::statement_visitor_t & operator= (tchecker::statement_visitor_t &&) = default;
    
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
