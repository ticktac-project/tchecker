/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_EXPRESSION_TYPED_HH
#define TCHECKER_EXPRESSION_TYPED_HH

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
    EXPR_TYPE_BAD = 0,              // Bad type
    EXPR_TYPE_INTTERM,              // Term with integer value
    EXPR_TYPE_INTVAR,               // Integer variable (with size=1)
    EXPR_TYPE_INTARRAY,             // Array of integer variables (with size>1)
    EXPR_TYPE_LOCALINTVAR,          // Local integer variable (with size=1)
    EXPR_TYPE_LOCALINTARRAY,        // Local array of integer variables (with size>1)
    EXPR_TYPE_INTLVALUE,            // Assignable integer expr (left value)
    EXPR_TYPE_LOCALINTLVALUE,       // Assignable local sinteger expr (left value)
    EXPR_TYPE_CLKVAR,               // Clock variable (with size=1)
    EXPR_TYPE_CLKARRAY,             // Array of clock variables (with size>1)
    EXPR_TYPE_CLKLVALUE,            // Assignable clock expr (left value)
    EXPR_TYPE_CLKDIFF,              // Difference of two clock lvalues
    EXPR_TYPE_INTCLKSUM,            // Sum of an integer term and a clock lvalue
    EXPR_TYPE_ATOMIC_PREDICATE,     // Atomic integer predicate
    EXPR_TYPE_CLKCONSTR_SIMPLE,     // Atomic simple clock constraint
    EXPR_TYPE_CLKCONSTR_DIAGONAL,   // Atomic diagonal clock constraint
    EXPR_TYPE_CONJUNCTIVE_FORMULA,  // Conjunction of atomic formulas
  };
  
  
  /*!
   \brief Output operator for expression types
   \param os : output stream
   \param type : expression type
   \post type has been output to os
   \return os after output
   */
  std::ostream & operator<< (std::ostream & os, enum tchecker::expression_type_t type);
  
  
  
  
  // Typed expressions
  
  class typed_int_expression_t;           // forward declarations
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
    tchecker::typed_expression_visitor_t & operator= (tchecker::typed_expression_visitor_t const &) = default;
    
    /*!
     \brief Move assignment operator
     */
    tchecker::typed_expression_visitor_t & operator= (tchecker::typed_expression_visitor_t &&) = default;
    
    
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
     \brief Accessor
     \return this expression's type
     */
    inline enum tchecker::expression_type_t type() const
    {
      return _type;
    }
    
    
    /*!
     \brief Visit
     \param v : typed expression visitor
     */
    void visit(tchecker::typed_expression_visitor_t & v) const;
    
    using tchecker::expression_t::visit;     // untyped expression visitor
  protected:
    /*!
     \brief Visit
     \param v : typed expression visitor
     */
    virtual void do_visit(tchecker::typed_expression_visitor_t & v) const = 0;
    
    using tchecker::expression_t::do_visit;  // untyped expression visitor
    
    
    enum tchecker::expression_type_t const _type;  /*!< Expression type */
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
  };
  
  
  
  
  /*!
   \class make_typed_expression_t
   \tparam EXPR : type of expression
   \brief Typed expression maker
   */
  template <class EXPR>
  class make_typed_expression_t : public tchecker::typed_expression_t, public EXPR {
  public:
    /*!
     \brief Constructor
     \param type : expression type
     \param args : arguments to a constructor of EXPR
     */
    template <class ... ARGS>
    make_typed_expression_t(enum tchecker::expression_type_t type, ARGS && ... args)
    : tchecker::typed_expression_t(type), EXPR(args...)
    {}
  protected:
    /*!
     \brief Output the expression
     \param os : output stream
     */
    virtual std::ostream & do_output(std::ostream & os) const
    {
      return this->EXPR::do_output(os);
    }
    
    
    /*!
     \brief Visit
     \param v : untyped expression visitor
     */
    virtual void do_visit(tchecker::expression_visitor_t & v) const
    {
      this->EXPR::do_visit(v);
    }
    
    
    /*!
     \brief Visit
     \param v : typed expression visitor
     */
    virtual void do_visit(tchecker::typed_expression_visitor_t & v) const = 0;
  };
  
  
  
  
  /*!
   \class make_typed_lvalue_expression_t
   \tparam EXPR : type of expression
   \brief Typed left-value expression maker
   */
  template <class EXPR>
  class make_typed_lvalue_expression_t : public tchecker::typed_lvalue_expression_t, public EXPR {
  public:
    /*!
     \brief Constructor
     \param type : expression type
     \param args : arguments to a constructor of EXPR
     */
    template <class ... ARGS>
    make_typed_lvalue_expression_t(enum tchecker::expression_type_t type, ARGS && ... args)
    : tchecker::typed_lvalue_expression_t(type), EXPR(args...)
    {}
  protected:
    /*!
     \brief Output the expression
     \param os : output stream
     */
    virtual std::ostream & do_output(std::ostream & os) const
    {
      return this->EXPR::do_output(os);
    }
    
    
    /*!
     \brief Visit
     \param v : untyped expression visitor
     */
    virtual void do_visit(tchecker::expression_visitor_t & v) const
    {
      this->EXPR::do_visit(v);
    }
    
    
    /*!
     \brief Visit
     \param v : typed expression visitor
     */
    virtual void do_visit(tchecker::typed_expression_visitor_t & v) const = 0;
  };
  
  
  
  
  /*!
   \class typed_int_expression_t
   \brief Typed integer expression
   */
  class typed_int_expression_t
  : public tchecker::make_typed_expression_t<tchecker::int_expression_t> {
  public:
    using tchecker::make_typed_expression_t<tchecker::int_expression_t>::make_typed_expression_t;
  protected:
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::expression_t * do_clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_expression_visitor_t & v) const;
    
    using tchecker::make_typed_expression_t<tchecker::int_expression_t>::do_visit;
  };
  
  
  
  
  /*!
   \class typed_var_expression_t
   \brief Typed variable expression
   */
  class typed_var_expression_t : public tchecker::make_typed_lvalue_expression_t<tchecker::var_expression_t> {
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
    typed_var_expression_t(enum tchecker::expression_type_t type,
                           std::string const & name,
                           tchecker::variable_id_t id,
                           tchecker::variable_size_t size);
    
    
    /*!
     \brief Accessor
     \return variable identifier
     */
    inline tchecker::variable_id_t id() const
    {
      return _id;
    }
    
    
    /*!
     \brief Accessor
     \return variable size
     */
    inline tchecker::variable_size_t size() const
    {
      return _size;
    }
  protected:
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::expression_t * do_clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_expression_visitor_t & v) const;
    
    using tchecker::make_typed_lvalue_expression_t<tchecker::var_expression_t>::do_visit;
    
    tchecker::variable_id_t const _id;      /*!< Variable identifier */
    tchecker::variable_size_t const _size;  /*!< Variable size */
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
    typed_bounded_var_expression_t(enum tchecker::expression_type_t type,
                                   std::string const & name,
                                   tchecker::variable_id_t id,
                                   tchecker::variable_size_t size,
                                   tchecker::integer_t min,
                                   tchecker::integer_t max);
    
    
    /*!
     \brief Accessor
     \return variable minimal value
     */
    inline tchecker::integer_t min() const
    {
      return _min;
    }
    
    
    /*!
     \brief Accessor
     \return variable maximal value
     */
    inline tchecker::integer_t max() const
    {
      return _max;
    }
  protected:
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::expression_t * do_clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_expression_visitor_t & v) const;
    
    using tchecker::typed_var_expression_t::do_visit;
    
    tchecker::integer_t const _min;   /*!< Variable minimal value */
    tchecker::integer_t const _max;   /*!< Variable maximal value */
  };
  
  
  
  
  /*!
   \class typed_array_expression_t
   \brief Typed array expression
   */
  class typed_array_expression_t : public tchecker::make_typed_lvalue_expression_t<tchecker::array_expression_t> {
  public:
    /*!
     \brief Constructor
     \param type : expression type
     \param variable : array variable
     \param offset : offset
     \pre variable != nullptr and offset != nullptr
     \note this takes ownership on variable and offset
     \throw std::invalid_argument : if precondition is violated
     \note see tchecker::array_expression_t
     */
    typed_array_expression_t(enum tchecker::expression_type_t type,
                             tchecker::typed_var_expression_t const * variable,
                             tchecker::typed_expression_t const * offset);
    
    
    /*!
     \brief Accessor
     \return variable as a typed expression
     */
    inline tchecker::typed_var_expression_t const & variable() const
    {
      return * dynamic_cast<tchecker::typed_var_expression_t const *>
      (_variable);
    }
    
    
    /*!
     \brief Accessor
     \return offset as a typed expression
     */
    inline tchecker::typed_expression_t const & offset() const
    {
      return * dynamic_cast<tchecker::typed_expression_t const *>
      (_offset);
    }
    
    
    /*!
     \brief Accessor
     \return size
     */
    inline tchecker::variable_size_t size() const
    {
      return 1;   // [] operator makes size 1
    }
  protected:
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::expression_t * do_clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_expression_visitor_t & v) const;
    
    using tchecker::make_typed_lvalue_expression_t<tchecker::array_expression_t>::do_visit;
  };
  
  
  
  
  /*!
   \class typed_par_expression_t
   \brief Typed parenthesis expression
   */
  class typed_par_expression_t : public tchecker::make_typed_expression_t<tchecker::par_expression_t> {
  public:
    /*!
     \brief Constructor
     \param type : type of expression
     \param expr : sub expression
     \note this claims ownership on expr
     */
    typed_par_expression_t(enum tchecker::expression_type_t type,
                           tchecker::typed_expression_t * expr)
    : tchecker::make_typed_expression_t<tchecker::par_expression_t>(type, expr)
    {}
    
    
    /*!
     \brief Accessor
     \return operand
     */
    inline tchecker::typed_expression_t const & expr() const
    {
      return dynamic_cast<tchecker::typed_expression_t const &>(this->tchecker::par_expression_t::expr());
    }
  protected:
    /*!
     \brief Clone (cast to typed_expression_t)
     \return typed operand clone
     */
    tchecker::typed_expression_t * expr_clone() const
    {
      return dynamic_cast<tchecker::typed_expression_t *>(this->tchecker::par_expression_t::expr().clone());
    }
    
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::expression_t * do_clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_expression_visitor_t & v) const;
    
    using tchecker::make_typed_expression_t<tchecker::par_expression_t>::do_visit;
  };
  
  
  
  
  /*!
   \class typed_binary_expression_t
   \brief Typed binary expression
   */
  class typed_binary_expression_t : public tchecker::make_typed_expression_t<tchecker::binary_expression_t> {
  public:
    /*!
     \brief Constructor
     \param type : type of expression
     \param op : operator
     \param left : left operand
     \param right : right operand
     */
    typed_binary_expression_t(enum tchecker::expression_type_t type,
                              enum tchecker::binary_operator_t op,
                              tchecker::typed_expression_t * left,
                              tchecker::typed_expression_t * right);
    
    
    /*!
     \brief Accessor
     \return left operand
     */
    tchecker::typed_expression_t const & left_operand() const;
    
    
    /*!
     \brief Accessor
     \return right operand
     */
    tchecker::typed_expression_t const & right_operand() const;
  protected:
    /*!
     \brief Clone (cast to typed_expression_t)
     \return typed left operand clone
     */
    tchecker::typed_expression_t * left_operand_clone() const;
    
    
    /*!
     \brief Clone (cast to typed_expression_t)
     \return typed right operand clone
     */
    tchecker::typed_expression_t * right_operand_clone() const;
    
    
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::expression_t * do_clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_expression_visitor_t & v) const;
    
    using tchecker::make_typed_expression_t<tchecker::binary_expression_t>::do_visit;
  };
  
  
  
  
  /*!
   \class typed_unary_expression_t
   \brief Typed unary expression
   */
  class typed_unary_expression_t : public tchecker::make_typed_expression_t<tchecker::unary_expression_t> {
  public:
    /*!
     \brief Constructor
     \param type : type of expression
     \param op : operator
     \param operand : operand
     */
    typed_unary_expression_t(enum tchecker::expression_type_t type,
                             enum tchecker::unary_operator_t op,
                             tchecker::typed_expression_t * operand);
    
    
    /*!
     \brief Accessor
     \return operand
     */
    tchecker::typed_expression_t const & operand() const;
  protected:
    /*!
     \brief Clone (cast to typed_expression_t)
     \return typed operand clone
     */
    tchecker::typed_expression_t * operand_clone() const;
    
    
    /*!
     \brief Clone
     \return clone of this
     */
    virtual tchecker::expression_t * do_clone() const;
    
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_expression_visitor_t & v) const;
    
    using tchecker::make_typed_expression_t<tchecker::unary_expression_t>::do_visit;
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
     */
    typed_simple_clkconstr_expression_t(enum tchecker::expression_type_t type,
                                        enum tchecker::binary_operator_t op,
                                        tchecker::typed_expression_t * left,
                                        tchecker::typed_expression_t * right);
    
    
    /*!
     \brief Accessor
     \return clock expression
     */
    inline tchecker::typed_lvalue_expression_t const & clock() const
    {
      return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(left_operand());
    }
    
    
    /*!
     \brief Accessor
     \return bound expression
     */
    inline tchecker::typed_expression_t const & bound() const
    {
      return right_operand();
    }
  protected:
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_expression_visitor_t & v) const;
    
    using tchecker::typed_binary_expression_t::do_visit;
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
     */
    typed_diagonal_clkconstr_expression_t(enum tchecker::expression_type_t type,
                                          enum tchecker::binary_operator_t op,
                                          tchecker::typed_expression_t * left,
                                          tchecker::typed_expression_t * right);
    
    
    /*!
     \brief Accessor
     \return first clock expression (i.e. x in x - y # c)
     */
    inline tchecker::typed_lvalue_expression_t const & first_clock() const
    {
      auto const & diagonal = dynamic_cast<tchecker::typed_binary_expression_t const &>(left_operand());  // left expr is x - y
      return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(diagonal.left_operand());
    }
    
    
    /*!
     \brief Accessor
     \return second clock expression (i.e. y in x - y # c)
     */
    inline tchecker::typed_lvalue_expression_t const & second_clock() const
    {
      auto const & diagonal = dynamic_cast<tchecker::typed_binary_expression_t const &>(left_operand());  // left expr is x - y
      return dynamic_cast<tchecker::typed_lvalue_expression_t const &>(diagonal.right_operand());
    }
    
    
    /*!
     \brief Accessor
     \return bound expression
     */
    inline tchecker::typed_expression_t const & bound() const
    {
      return right_operand();
    }
  protected:
    /*!
     \brief Visit
     \param v : visitor
     */
    virtual void do_visit(tchecker::typed_expression_visitor_t & v) const;
    
    using tchecker::typed_binary_expression_t::do_visit;
  };

    /*!
     \class typed_ite_expression_t
     \brief Typed binary expression
     */
    class typed_ite_expression_t : public tchecker::make_typed_expression_t<tchecker::ite_expression_t> {
     public:
      /*!
       \brief Constructor
       \param type : type of expression
       \param cond : condition expression
       \param then_value : value if cond is true
       \param else_value : value if cond is false
       */
      typed_ite_expression_t(tchecker::expression_type_t type,
                             tchecker::typed_expression_t * cond,
                             tchecker::typed_expression_t * then_value,
                             tchecker::typed_expression_t * else_value);


      /*!
       \brief Accessor
       \return condition expression
       */
      tchecker::typed_expression_t const & condition() const;


      /*!
       \brief Accessor
       \return THEN value
       */
      tchecker::typed_expression_t const & then_value() const;


      /*!
       \brief Accessor
       \return ELSE value
       */
      tchecker::typed_expression_t const & else_value() const;
     protected:
      /*!
       \brief Clone (cast to typed_expression_t)
       \return typed condition clone
       */
      tchecker::typed_expression_t * condition_clone() const;

      /*!
       \brief Clone (cast to typed_expression_t)
       \return typed then value clone
       */
      tchecker::typed_expression_t * then_value_clone() const;


      /*!
       \brief Clone (cast to typed_expression_t)
       \return typed else expression clone
       */
      tchecker::typed_expression_t * else_value_clone() const;


      /*!
       \brief Clone
       \return clone of this
       */
      virtual tchecker::expression_t * do_clone() const;

      /*!
       \brief Visit
       \param v : visitor
       */
      virtual void do_visit(tchecker::typed_expression_visitor_t & v) const;

      using tchecker::make_typed_expression_t<tchecker::ite_expression_t>::do_visit;
    };

} // end of namespace tchecker

#endif // TCHECKER_EXPRESSION_TYPED_HH

