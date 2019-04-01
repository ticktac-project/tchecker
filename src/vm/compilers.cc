/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <limits>
#include <vector>

#include "tchecker/expression/type_inference.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/vm/compilers.hh"

namespace tchecker {
  
  // Expression compiler
  
  namespace details {
    
    template <class BYTECODE_BACK_INSERTER>
    class lvalue_expression_compiler_t;    // forward declaration
    
    
    template <class BYTECODE_BACK_INSERTER>
    class rvalue_expression_compiler_t;    // forward declaration
    
    
    /*!
     \class lvalue_expression_compiler_t
     \brief Visitor for compilation of left-value expressions
     */
    template <class BYTECODE_BACK_INSERTER>
    class lvalue_expression_compiler_t final : public tchecker::typed_expression_visitor_t {
    public:
      /*!
       \brief Constructor
       \param back_inserter : back inserter for bytecode
       */
      lvalue_expression_compiler_t(BYTECODE_BACK_INSERTER bytecode_back_inserter)
      : _bytecode_back_inserter(bytecode_back_inserter)
      {}
      
      
      /*!
       \brief Copy constructor (DELETED)
       */
      lvalue_expression_compiler_t(tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> const &) = delete;
      
      
      /*!
       \brief Move constructor (DELETED)
       */
      lvalue_expression_compiler_t(tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> &&) = delete;
      
      
      /*!
       \brief Destructor
       */
      virtual ~lvalue_expression_compiler_t() = default;
      
      
      /*!
       \brief Assignment operator (DELETED)
       */
      tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> &
      operator= (tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> const &) = delete;
      
      
      /*!
       \brief Move assignment operator (DELETED)
       */
      tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> &
      operator= (tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> &&) = delete;
      
      
      // Byte code compilers
      
      /*
       VM_PUSH ID(expr.name())
       */
      virtual void visit(tchecker::typed_var_expression_t const & expr)
      {
        if ((expr.type() != tchecker::EXPR_TYPE_CLKVAR) &&
            (expr.type() != tchecker::EXPR_TYPE_INTVAR) &&
            (expr.type() != tchecker::EXPR_TYPE_CLKARRAY) &&
            (expr.type() != tchecker::EXPR_TYPE_INTARRAY))
          throw std::invalid_argument("invalid expression");
        
        // Write bytecode
        _bytecode_back_inserter = tchecker::VM_PUSH;
        _bytecode_back_inserter = expr.id();
      }
      
      
      /*
       same tchecker::typed_var_expression_t
       
       VM_PUSH ID(expr.name())
       */
      virtual void visit(tchecker::typed_bounded_var_expression_t const & expr)
      {
        this->visit(dynamic_cast<tchecker::typed_var_expression_t const &>(expr));
      }
      
      
      /*
       VM_PUSH ID(expr.variable())
       insert expr.offset() bytecode
       VM_FAILNOTIN lowoffset(expr.variable()) highoffset(expr.variable())
       VM_SUM
       */
      virtual void visit(tchecker::typed_array_expression_t const & expr)
      {
        if ((expr.type() != tchecker::EXPR_TYPE_CLKLVALUE) &&
            (expr.type() != tchecker::EXPR_TYPE_INTLVALUE))
          throw std::invalid_argument("invalid expression");
        
        // offset bounds
        auto const id = expr.variable().id();
        auto const lowoffset = 0;
        auto const highoffset = expr.variable().size() - 1;
        
        // Write bytecode
        tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> rvalue_expression_compiler(_bytecode_back_inserter);
        
        _bytecode_back_inserter = tchecker::VM_PUSH;
        _bytecode_back_inserter = id;
        expr.offset().visit(rvalue_expression_compiler);
        _bytecode_back_inserter = tchecker::VM_FAILNOTIN;
        _bytecode_back_inserter = lowoffset;
        _bytecode_back_inserter = highoffset;
        _bytecode_back_inserter = tchecker::VM_SUM;
      }
      
      
      virtual void visit(tchecker::typed_int_expression_t const & expr)
      { throw std::invalid_argument("not an lvalue expression"); }
      
      virtual void visit(tchecker::typed_par_expression_t const & expr)
      { throw std::invalid_argument("not an lvalue expression"); }
      
      virtual void visit(tchecker::typed_binary_expression_t const & expr)
      { throw std::invalid_argument("not an lvalue expression"); }
      
      virtual void visit(tchecker::typed_unary_expression_t const & expr)
      { throw std::invalid_argument("not an lvalue expression"); }
      
      virtual void visit(tchecker::typed_simple_clkconstr_expression_t const & expr)
      { throw std::invalid_argument("not a bounded variable"); }
      
      virtual void visit(tchecker::typed_diagonal_clkconstr_expression_t const & expr)
      { throw std::invalid_argument("not a bounded variable"); }
    protected:
      BYTECODE_BACK_INSERTER _bytecode_back_inserter;  /*!< Bytecode */
    };
    
    
    
    
    /*!
     \class rvalue_expression_compiler_t
     \brief Visitor for compilation of right-value expressions
     */
    template <class BYTECODE_BACK_INSERTER>
    class rvalue_expression_compiler_t final : public tchecker::typed_expression_visitor_t {
    public:
      /*!
       \brief Constructor
       \param back_inserter : back inserter for bytecode
       */
      rvalue_expression_compiler_t(BYTECODE_BACK_INSERTER bytecode_back_inserter)
      : _bytecode_back_inserter(bytecode_back_inserter)
      {}
      
      
      /*!
       \brief Copy constructor (DELETED)
       */
      rvalue_expression_compiler_t(tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> const &) = delete;
      
      
      /*!
       \brief Move constructor (DELETED)
       */
      rvalue_expression_compiler_t(tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> &&) = delete;
      
      
      /*!
       \brief Destructor
       */
      virtual ~rvalue_expression_compiler_t() = default;
      
      
      /*!
       \brief Assignment operator (DELETED)
       */
      tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> &
      operator= (tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> const &) = delete;
      
      
      /*!
       \brief Move assignment operator (DELETED)
       */
      tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> &
      operator= (tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> &&) = delete;
      
      
      // Bytecode compilers
      
      /*
       VM_PUSH ID(expr.name())
       VM_VALUEAT
       */
      virtual void visit(tchecker::typed_var_expression_t const & expr)
      {
        if ((expr.type() != tchecker::EXPR_TYPE_CLKVAR) &&
            (expr.type() != tchecker::EXPR_TYPE_INTVAR) &&
            (expr.type() != tchecker::EXPR_TYPE_CLKARRAY) &&
            (expr.type() != tchecker::EXPR_TYPE_INTARRAY))
          throw std::invalid_argument("invalid expression");
        
        // Write bytecode (similar to lvalue, except last instruction)
        tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> lvalue_expression_compiler(_bytecode_back_inserter);
        
        expr.visit(lvalue_expression_compiler);
        _bytecode_back_inserter = tchecker::VM_VALUEAT;
      }
      
      
      /*
       same as tchecker::typed_var_expression_t
       
       VM_PUSH ID(expr.name())
       VM_VALUEAT
       */
      virtual void visit(tchecker::typed_bounded_var_expression_t const & expr)
      {
        this->visit(dynamic_cast<tchecker::typed_var_expression_t const &>(expr));
      }
      
      
      /*
       VM_PUSH ID(expr.name())
       insert expr.offset() bytecode
       VM_FAILNOTIN lowoffset(expr.name()) highoffset(expr.name()))
       VM_SUM
       VM_VALUEAT
       */
      virtual void visit(tchecker::typed_array_expression_t const & expr)
      {
        if ((expr.type() != tchecker::EXPR_TYPE_CLKLVALUE) &&
            (expr.type() != tchecker::EXPR_TYPE_INTLVALUE))
          throw std::invalid_argument("invalid expression");
        
        // Write bytecode (similar to lvalue, except last instruction)
        tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> lvalue_expression_compiler(_bytecode_back_inserter);
        
        expr.visit(lvalue_expression_compiler);
        _bytecode_back_inserter = tchecker::VM_VALUEAT;
      }
      
      
      /*
       VM_PUSH expr.value()
       */
      virtual void visit(tchecker::typed_int_expression_t const & expr)
      {
        if (expr.type() != tchecker::EXPR_TYPE_INTTERM)
          throw std::invalid_argument("invalid expression");
        
        // Write bytecode
        _bytecode_back_inserter = tchecker::VM_PUSH;
        _bytecode_back_inserter = expr.value();
      }
      
      
      /*
       Bytecode of operand
       */
      virtual void visit(tchecker::typed_par_expression_t const & expr)
      {
        if (expr.type() == tchecker::EXPR_TYPE_BAD)
          throw std::invalid_argument("invalid expression");
        
        // Write bytecode
        expr.expr().visit(*this);
      }
      
      
      /*
       see compile_binary_expression
       */
      virtual void visit(tchecker::typed_binary_expression_t const & expr)
      {
        // LAND expression
        if (expr.binary_operator() == tchecker::EXPR_OP_LAND) {
          if (expr.type() != tchecker::EXPR_TYPE_CONJUNCTIVE_FORMULA)
            throw std::invalid_argument("invalid expression");
          
          compile_binary_expression(expr);
          _bytecode_back_inserter = tchecker::VM_RETZ;   // optimization: return as soon as conjunct is false
        }
        // LT, LE, EQ, NEQ, GE, GT expression
        else if (tchecker::predicate(expr.binary_operator())) {
          if (expr.type() != tchecker::EXPR_TYPE_ATOMIC_PREDICATE)
            throw std::invalid_argument("invalid expression");
          
          compile_binary_expression(expr);
        }
        // arithmetic operators
        else {
          if (expr.type() != tchecker::EXPR_TYPE_INTTERM)
            throw std::invalid_argument("invalid expression");
          
          compile_binary_expression(expr);
        }
      }
      
      
      /*
       see compile_unary_expression
       */
      virtual void visit(tchecker::typed_unary_expression_t const & expr)
      {
        // NEG expresison
        if (expr.unary_operator() == tchecker::EXPR_OP_NEG) {
          if (expr.type() != tchecker::EXPR_TYPE_INTTERM)
            throw std::invalid_argument("invalid expression");
        }
        // LNOT expression
        else if (expr.unary_operator() == tchecker::EXPR_OP_LNOT) {
          if (expr.type() != tchecker::EXPR_TYPE_ATOMIC_PREDICATE)
            throw std::invalid_argument("invalid expression");
        }
        
        compile_unary_expression(expr);
      }
      
      
      /*
       see compile_clock_predicate
       */
      virtual void visit(tchecker::typed_simple_clkconstr_expression_t const & expr)
      {
        if (expr.type() != tchecker::EXPR_TYPE_CLKCONSTR_SIMPLE)
          throw std::invalid_argument("invalid expression");
        
        tchecker::typed_var_expression_t zero_clock(tchecker::EXPR_TYPE_CLKVAR, tchecker::zero_clock_name,
                                                    tchecker::zero_clock_id, 1);
        
        compile_clock_predicate(expr.clock(), zero_clock, expr.bound(), operator_to_instruction(expr.binary_operator()));
      }
      
      
      /*
       see compile_clock_predicate
       */
      virtual void visit(tchecker::typed_diagonal_clkconstr_expression_t const & expr)
      {
        if (expr.type() != tchecker::EXPR_TYPE_CLKCONSTR_DIAGONAL)
          throw std::invalid_argument("invalid expression");
        
        compile_clock_predicate(expr.first_clock(), expr.second_clock(), expr.bound(),
                                operator_to_instruction(expr.binary_operator()));
      }
    private:
      /*
       \brief Translates expression binary operators into bytecode instructions
       \param op : operator
       \return corresponding instruction
       */
      enum tchecker::instruction_t operator_to_instruction(enum tchecker::binary_operator_t op)
      {
        switch (op) {
          case tchecker::EXPR_OP_LAND:   return tchecker::VM_LAND;
          case tchecker::EXPR_OP_LT:     return tchecker::VM_LT;
          case tchecker::EXPR_OP_LE:     return tchecker::VM_LE;
          case tchecker::EXPR_OP_EQ:     return tchecker::VM_EQ;
          case tchecker::EXPR_OP_NEQ:    return tchecker::VM_NE;
          case tchecker::EXPR_OP_GE:     return tchecker::VM_GE;
          case tchecker::EXPR_OP_GT:     return tchecker::VM_GT;
          case tchecker::EXPR_OP_MINUS:  return tchecker::VM_MINUS;
          case tchecker::EXPR_OP_PLUS:   return tchecker::VM_SUM;
          case tchecker::EXPR_OP_TIMES:  return tchecker::VM_MUL;
          case tchecker::EXPR_OP_DIV:    return tchecker::VM_DIV;
          case tchecker::EXPR_OP_MOD:    return tchecker::VM_MOD;
          default:                       throw std::runtime_error("incomplete swicth statement");
        }
      }
      
      
      /*
       \brief Translates expression unary operators into bytecode instructions
       \param op : operator
       \return corresponding instruction
       */
      enum tchecker::instruction_t operator_to_instruction(enum tchecker::unary_operator_t op)
      {
        switch (op) {
          case tchecker::EXPR_OP_NEG:    return tchecker::VM_NEG;
          case tchecker::EXPR_OP_LNOT:   return tchecker::VM_LNOT;
          default:                       throw std::runtime_error("incomplete swicth statement");
        }
      }
      
      
      /*
       \brief Compiler for clock predicate of the form
       clock1 - clock2 # bound   where # is <,<=,==,>=,> depending on instruction
       
       \pre instruction is one of tchecker::VM_LT, tchecker::VM_LE,
       tchecker::VM_EQ, tchecker::VM_GE or tchecker::VM_GT
       \throw std::invalid_argument : if the precondition is violated
       */
      void compile_clock_predicate(tchecker::typed_expression_t const & clock1,
                                   tchecker::typed_expression_t const & clock2,
                                   tchecker::typed_expression_t const & bound,
                                   enum tchecker::instruction_t instruction)
      {
        // bound negation
        auto * bound_clone = dynamic_cast<tchecker::typed_expression_t *>(bound.clone());
        tchecker::typed_unary_expression_t neg_bound(tchecker::type_neg(bound.type()), tchecker::EXPR_OP_NEG, bound_clone);
        
        // write bytecode
        if (instruction == tchecker::VM_LT)
          compile_clock_constraint(clock1, clock2, tchecker::clock_constraint_t::LT, bound);
        else if (instruction == tchecker::VM_LE)
          compile_clock_constraint(clock1, clock2, tchecker::clock_constraint_t::LE, bound);
        else if (instruction == tchecker::VM_GE)
          compile_clock_constraint(clock2, clock1, tchecker::clock_constraint_t::LE, neg_bound);
        else if (instruction == tchecker::VM_GT)
          compile_clock_constraint(clock2, clock1, tchecker::clock_constraint_t::LT, neg_bound);
        else if (instruction == tchecker::VM_EQ) {
          compile_clock_constraint(clock1, clock2, tchecker::clock_constraint_t::LE, bound);
          compile_clock_constraint(clock2, clock1, tchecker::clock_constraint_t::LE, neg_bound);
          _bytecode_back_inserter = tchecker::VM_LAND;
        }
        else
          throw std::invalid_argument("invalid instruction");
      }
      
      
      /*
       Compile clock constraint: first - second </<= bound
       
       insert first bytecode
       insert second bytecode
       insert bound bytecode
       VM_CLKCONSTR cmp
       VM_PUSH 1              // abstraction of constraint at syntax level
       */
      void compile_clock_constraint(tchecker::typed_expression_t const & first,
                                    tchecker::typed_expression_t const & second,
                                    enum tchecker::clock_constraint_t::comparator_t cmp,
                                    tchecker::typed_expression_t const & bound)
      {
        tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> clocks_compiler(_bytecode_back_inserter);
        
        first.visit(clocks_compiler);
        second.visit(clocks_compiler);
        bound.visit(*this);
        _bytecode_back_inserter = tchecker::VM_CLKCONSTR;
        _bytecode_back_inserter = cmp;
        _bytecode_back_inserter = tchecker::VM_PUSH;
        _bytecode_back_inserter = 1;
      }
      
      
      /*
       insert expr.left_operand() bytecode
       insert expr.right_operand() bytecode
       instruction from expr.op()
       */
      void compile_binary_expression(tchecker::typed_binary_expression_t const & expr)
      {
        expr.left_operand().visit(*this);
        expr.right_operand().visit(*this);
        _bytecode_back_inserter = operator_to_instruction(expr.binary_operator());
      }
      
      
      /*
       insert expr.operand() bytecode
       instruction from expr.op()
       */
      void compile_unary_expression(tchecker::typed_unary_expression_t const & expr)
      {
        expr.operand().visit(*this);
        _bytecode_back_inserter = operator_to_instruction(expr.unary_operator());
      }
      
      
      BYTECODE_BACK_INSERTER _bytecode_back_inserter;  /*!< Bytecode */
    };
    
  } // end of namespace details
  
  
  
  
  tchecker::bytecode_t * compile(tchecker::typed_expression_t const & expr)
  {
    try {
      if (expr.type() == tchecker::EXPR_TYPE_BAD)
        throw std::invalid_argument("invalid expression");
      
      // Compilation
      std::vector<tchecker::bytecode_t> bytecode;
      auto back_inserter = std::back_inserter(bytecode);
      
      tchecker::details::rvalue_expression_compiler_t<decltype(back_inserter)> compiler(back_inserter);
      expr.visit(compiler);
      back_inserter = tchecker::VM_RET;
      
      // Production
      if (bytecode.size() == 0)
        throw std::runtime_error("compilation produced no bytecode");
      if (bytecode.back() != tchecker::VM_RET)
        throw std::runtime_error("bytecode is not null-terminated");
      
      tchecker::bytecode_t * b = new tchecker::bytecode_t[bytecode.size()];
      std::memcpy(b, bytecode.data(), bytecode.size() * sizeof(tchecker::bytecode_t));
      
      return b;
    }
    catch (std::invalid_argument const & e) {
      throw std::invalid_argument("compiling expression " + expr.to_string() +
                                  ", " + e.what());
    }
    catch (std::runtime_error const & e) {
      throw std::runtime_error("compiling expression " + expr.to_string() +
                               ", " + e.what());
    }
  }
  
  
  
  
  // Statement compiler
  
  namespace details {
    
    /*!
     \class variable_bounds_visitor_t
     \brief Visitor for determining variable bounds
     */
    class variable_bounds_visitor_t : public tchecker::typed_expression_visitor_t {
    public:
      /*!
       \brief Constructor
       */
      variable_bounds_visitor_t()
      : _min(std::numeric_limits<tchecker::integer_t>::min()),
      _max(std::numeric_limits<tchecker::integer_t>::max())
      {}
      
      
      /*!
       \brief Copy constructor (DELETED)
       */
      variable_bounds_visitor_t(tchecker::details::variable_bounds_visitor_t const &) = delete;
      
      
      /*!
       \brief Move constructor (DELETED)
       */
      variable_bounds_visitor_t(tchecker::details::variable_bounds_visitor_t &&) = delete;
      
      
      /*!
       \brief Destructor
       */
      virtual ~variable_bounds_visitor_t() = default;
      
      
      /*!
       \brief Assignment operator (DELETED)
       */
      tchecker::details::variable_bounds_visitor_t & operator= (tchecker::details::variable_bounds_visitor_t const &) = delete;
      
      
      /*!
       \brief Move assignment operator (DELETED)
       */
      tchecker::details::variable_bounds_visitor_t & operator= (tchecker::details::variable_bounds_visitor_t &&) = delete;
      
      
      /*!
       \brief Accessor
       \return minimal value
       */
      inline tchecker::integer_t min() const
      {
        return _min;
      }
      
      
      /*!
       \brief Accessor
       \return maximal value
       */
      inline tchecker::integer_t max() const
      {
        return _max;
      }
      
      
      // Visitors
      
      virtual void visit(tchecker::typed_bounded_var_expression_t const & expr)
      {
        _min = expr.min();
        _max = expr.max();
      }
      
      
      virtual void visit(tchecker::typed_array_expression_t const & expr)
      {
        expr.variable().visit(*this);
      }
      
      
      virtual void visit(tchecker::typed_int_expression_t const & expr)
      { throw std::invalid_argument("not a bounded variable"); }
      
      virtual void visit(tchecker::typed_var_expression_t const & expr)
      { throw std::invalid_argument("not a bounded variable"); }
      
      virtual void visit(tchecker::typed_par_expression_t const & expr)
      { throw std::invalid_argument("not a bounded variable"); }
      
      virtual void visit(tchecker::typed_binary_expression_t const & expr)
      { throw std::invalid_argument("not a bounded variable"); }
      
      virtual void visit(tchecker::typed_unary_expression_t const & expr)
      { throw std::invalid_argument("not a bounded variable"); }
      
      virtual void visit(tchecker::typed_simple_clkconstr_expression_t const & expr)
      { throw std::invalid_argument("not a bounded variable"); }
      
      virtual void visit(tchecker::typed_diagonal_clkconstr_expression_t const & expr)
      { throw std::invalid_argument("not a bounded variable"); }
    protected:
      tchecker::integer_t _min;  /*!< Variable minimal value */
      tchecker::integer_t _max;  /*!< Variable maximal value */
    };
    
    
    
    
    /*!
     \class statement_compiler_t
     \brief Visitor for compilation of statements
     */
    template <class BYTECODE_BACK_INSERTER>
    class statement_compiler_t final : public tchecker::typed_statement_visitor_t {
    public:
      /*!
       \brief Constructor
       \param back_inserter : back inserter for bytecode
       */
      statement_compiler_t(BYTECODE_BACK_INSERTER bytecode_back_inserter)
      : _bytecode_back_inserter(bytecode_back_inserter)
      {}
      
      
      /*!
       \brief Copy constructor (DELETED)
       */
      statement_compiler_t(tchecker::details::statement_compiler_t<BYTECODE_BACK_INSERTER> const &) = delete;
      
      
      /*!
       \brief Move constructor (DELETED)
       */
      statement_compiler_t(tchecker::details::statement_compiler_t<BYTECODE_BACK_INSERTER> &&) = delete;
      
      
      /*!
       \brief Destructor
       */
      virtual ~statement_compiler_t() = default;
      
      
      /*!
       \brief Assignment operator (DELETED)
       */
      tchecker::details::statement_compiler_t<BYTECODE_BACK_INSERTER> &
      operator= (tchecker::details::statement_compiler_t<BYTECODE_BACK_INSERTER> const &) = delete;
      
      
      /*!
       \brief Move assignment operator (DELETED)
       */
      tchecker::details::statement_compiler_t<BYTECODE_BACK_INSERTER> &
      operator= (tchecker::details::statement_compiler_t<BYTECODE_BACK_INSERTER> &&) = delete;
      
      
      // Byte code compilers
      
      /*
       VM_NOP
       */
      virtual void visit(tchecker::typed_nop_statement_t const & stmt)
      {
        if (stmt.type() == tchecker::STMT_TYPE_BAD)
          throw std::invalid_argument("invalid statement");
        
        // Write bytecode
        _bytecode_back_inserter = tchecker::VM_NOP;
      }
      
      
      /*
       see compile_int_assign
       */
      virtual void visit(tchecker::typed_assign_statement_t const & stmt)
      {
        if (stmt.type() != tchecker::STMT_TYPE_INTASSIGN)
          throw std::invalid_argument("invalid statement");
        
        compile_int_assign(stmt.lvalue(), stmt.rvalue());
      }
      
      
      /*
       see compile_clock_reset
       */
      virtual void visit(tchecker::typed_int_to_clock_assign_statement_t const & stmt)
      {
        if (stmt.type() != tchecker::STMT_TYPE_CLKASSIGN_INT)
          throw std::invalid_argument("invalid statement");
        
        tchecker::typed_var_expression_t zero_clock(tchecker::EXPR_TYPE_CLKVAR, tchecker::zero_clock_name,
                                                    tchecker::zero_clock_id, 1);
        compile_clock_reset(stmt.clock(), stmt.value(), zero_clock);
      }
      
      
      /*
       see compile_clock_reset
       */
      virtual void visit(tchecker::typed_clock_to_clock_assign_statement_t const & stmt)
      {
        if (stmt.type() != tchecker::STMT_TYPE_CLKASSIGN_CLK)
          throw std::invalid_argument("invalid statement");
        
        tchecker::typed_int_expression_t zero(tchecker::EXPR_TYPE_INTTERM, 0);
        compile_clock_reset(stmt.lclock(), zero, stmt.rclock());
      }
      
      
      /*
       see compile_clock_reset
       */
      virtual void visit(tchecker::typed_sum_to_clock_assign_statement_t const & stmt)
      {
        if (stmt.type() != tchecker::STMT_TYPE_CLKASSIGN_SUM)
          throw std::invalid_argument("invalid statement");
        
        compile_clock_reset(stmt.lclock(), stmt.value(), stmt.rclock());
      }
      
      
      /*
       insert stmt.first() bytecode
       insert stmt.second() bytecode
       */
      virtual void visit(tchecker::typed_sequence_statement_t const & stmt)
      {
        if (stmt.type() == tchecker::STMT_TYPE_BAD)
          throw std::invalid_argument("invalid statement");
        
        // Write bytecode
        stmt.first().visit(*this);
        stmt.second().visit(*this);
      }
    protected:
      /*
       Compiler for clock reset:  lvalue = int_rvalue + clock_rvalue
       
       insert lvalue bytecode
       insert int_rvalue bytecode
       insert clock_rvalue bytecode
       VM_CLKRESET
       */
      void compile_clock_reset(tchecker::typed_lvalue_expression_t const & lvalue,
                               tchecker::typed_expression_t const & int_rvalue,
                               tchecker::typed_lvalue_expression_t const & clock_rvalue)
      {
        tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> clocks_compiler(_bytecode_back_inserter);
        tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> int_rvalue_compiler(_bytecode_back_inserter);
        
        lvalue.visit(clocks_compiler);
        int_rvalue.visit(int_rvalue_compiler);
        clock_rvalue.visit(clocks_compiler);
        _bytecode_back_inserter = tchecker::VM_CLKRESET;
      }
      
      
      /*
       insert lvalue bytecode
       insert rvalue bytecode
       VM_FAILNOTIN lowvalue(lvalue) highvalue(lvalue)
       VM_ASSIGN
       */
      void compile_int_assign(tchecker::typed_lvalue_expression_t const & lvalue, tchecker::typed_expression_t const & rvalue)
      {
        // Get lvalue bounds
        tchecker::details::variable_bounds_visitor_t bounds_visitor;
        lvalue.visit(bounds_visitor);
        
        // Write bytecode
        tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> lvalue_compiler(_bytecode_back_inserter);
        tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> rvalue_compiler(_bytecode_back_inserter);
        
        lvalue.visit(lvalue_compiler);
        rvalue.visit(rvalue_compiler);
        _bytecode_back_inserter = tchecker::VM_FAILNOTIN;
        _bytecode_back_inserter = bounds_visitor.min();
        _bytecode_back_inserter = bounds_visitor.max();
        _bytecode_back_inserter = tchecker::VM_ASSIGN;
      }
      
      
      BYTECODE_BACK_INSERTER _bytecode_back_inserter;  /*!< Bytecode back ins. */
    };
    
  } // end of namespace details
  
  
  
  
  tchecker::bytecode_t * compile(tchecker::typed_statement_t const & stmt)
  {
    try {
      if (stmt.type() == tchecker::STMT_TYPE_BAD)
        throw std::invalid_argument("invalid statement");
      
      std::vector<tchecker::bytecode_t> bytecode;
      auto back_inserter = std::back_inserter(bytecode);
      
      tchecker::details::statement_compiler_t<decltype(back_inserter)> compiler(back_inserter);
      stmt.visit(compiler);
      back_inserter = tchecker::VM_PUSH;  // return code...
      back_inserter = 1;                  // ...for statement
      back_inserter = tchecker::VM_RET;
      
      if (bytecode.size() == 0)
        throw std::runtime_error("compilation produced no bytecode");
      if (bytecode.back() != tchecker::VM_RET)
        throw std::runtime_error("bytecode is not null-terminated");
      
      tchecker::bytecode_t * b = new tchecker::bytecode_t[bytecode.size()];
      std::memcpy(b, bytecode.data(), bytecode.size() * sizeof(tchecker::bytecode_t));
      
      return b;
    }
    catch (std::invalid_argument const & e) {
      throw std::invalid_argument("compiling statement " + stmt.to_string() + ", " + e.what());
    }
    catch (std::runtime_error const & e) {
      throw std::runtime_error("compiling statement " + stmt.to_string() + ", " + e.what());
    }
  }
  
} // end of namespace tchecker

