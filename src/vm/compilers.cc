/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <limits>
#include <tchecker/statement/static_analysis.hh>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/expression/type_inference.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/vm/compilers.hh"

namespace tchecker {

// Forward declaration of utility routines
namespace details {

std::vector<tchecker::bytecode_t>::size_type compile_tmp_statement(tchecker::typed_statement_t const & stmt,
                                                                   std::vector<tchecker::bytecode_t> & container);

std::vector<tchecker::bytecode_t>::size_type compile_tmp_rvalue_expression(tchecker::typed_expression_t const & expr,
                                                                           std::vector<tchecker::bytecode_t> & container);

template <class BYTECODE_BACK_INSERTER>
inline void append_bytecode(BYTECODE_BACK_INSERTER & bbi, std::vector<tchecker::bytecode_t> bytecode)
{
  for (auto b : bytecode)
    bbi = b;
}
} // namespace details

// Expression compiler

namespace details {

template <class BYTECODE_BACK_INSERTER> class lvalue_expression_compiler_t; // forward declaration

template <class BYTECODE_BACK_INSERTER> class rvalue_expression_compiler_t; // forward declaration

/*!
 \class lvalue_expression_compiler_t
 \brief Visitor for compilation of left-value expressions
 */
template <class BYTECODE_BACK_INSERTER> class lvalue_expression_compiler_t final : public tchecker::typed_expression_visitor_t {
public:
  /*!
   \brief Constructor
   \param back_inserter : back inserter for bytecode
   */
  lvalue_expression_compiler_t(BYTECODE_BACK_INSERTER bytecode_back_inserter) : _bytecode_back_inserter(bytecode_back_inserter)
  {
  }

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
  operator=(tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> const &) = delete;

  /*!
   \brief Move assignment operator (DELETED)
   */
  tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> &
  operator=(tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> &&) = delete;

  // Byte code compilers

  /*
   VM_PUSH ID(expr.name())
   */
  virtual void visit(tchecker::typed_var_expression_t const & expr)
  {
    if ((expr.type() != tchecker::EXPR_TYPE_CLKVAR) && (expr.type() != tchecker::EXPR_TYPE_INTVAR) &&
        (expr.type() != tchecker::EXPR_TYPE_LOCALINTVAR) && (expr.type() != tchecker::EXPR_TYPE_CLKARRAY) &&
        (expr.type() != tchecker::EXPR_TYPE_INTARRAY) && (expr.type() != tchecker::EXPR_TYPE_LOCALINTARRAY))
      invalid_expression(expr, "a variable");

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
    if ((expr.type() != tchecker::EXPR_TYPE_CLKLVALUE) && (expr.type() != tchecker::EXPR_TYPE_INTLVALUE) &&
        (expr.type() != tchecker::EXPR_TYPE_LOCALINTLVALUE))
      invalid_expression(expr, "a lvalue");

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

  virtual void visit(tchecker::typed_int_expression_t const & expr) { not_supported(expr); }

  virtual void visit(tchecker::typed_par_expression_t const & expr) { not_supported(expr); }

  virtual void visit(tchecker::typed_binary_expression_t const & expr) { not_supported(expr); }

  virtual void visit(tchecker::typed_unary_expression_t const & expr) { not_supported(expr); }

  virtual void visit(tchecker::typed_simple_clkconstr_expression_t const & expr) { not_supported(expr); }

  virtual void visit(tchecker::typed_diagonal_clkconstr_expression_t const & expr) { not_supported(expr); }

  virtual void visit(tchecker::typed_ite_expression_t const & expr) { not_supported(expr); }

protected:
  void not_supported(tchecker::typed_expression_t const & expr)
  {
    throw std::invalid_argument("not lvalue expression: " + expr.to_string());
  }

  void invalid_expression(tchecker::typed_expression_t const & expr, std::string expected)
  {
    throw std::invalid_argument("invalid expression '" + expr.to_string() + "' where " + expected + " is expected.");
  }

  BYTECODE_BACK_INSERTER _bytecode_back_inserter; /*!< Bytecode */
};

/*!
 \class rvalue_expression_compiler_t
 \brief Visitor for compilation of right-value expressions
 */
template <class BYTECODE_BACK_INSERTER> class rvalue_expression_compiler_t final : public tchecker::typed_expression_visitor_t {
public:
  /*!
   \brief Constructor
   \param back_inserter : back inserter for bytecode
   */
  rvalue_expression_compiler_t(BYTECODE_BACK_INSERTER bytecode_back_inserter) : _bytecode_back_inserter(bytecode_back_inserter)
  {
  }

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
  operator=(tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> const &) = delete;

  /*!
   \brief Move assignment operator (DELETED)
   */
  tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> &
  operator=(tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> &&) = delete;

  // Bytecode compilers

  /*
   VM_PUSH ID(expr.name())
   VM_VALUEAT
   */
  virtual void visit(tchecker::typed_var_expression_t const & expr)
  {
    if ((expr.type() != tchecker::EXPR_TYPE_CLKVAR) && (expr.type() != tchecker::EXPR_TYPE_INTVAR) &&
        (expr.type() != tchecker::EXPR_TYPE_LOCALINTVAR) && (expr.type() != tchecker::EXPR_TYPE_CLKARRAY) &&
        (expr.type() != tchecker::EXPR_TYPE_INTARRAY) && (expr.type() != tchecker::EXPR_TYPE_LOCALINTARRAY))
      invalid_expression(expr, "a variable");

    // Write bytecode (similar to lvalue, except last instruction)
    tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> lvalue_expression_compiler(_bytecode_back_inserter);

    expr.visit(lvalue_expression_compiler);
    if ((expr.type() != tchecker::EXPR_TYPE_LOCALINTVAR) && (expr.type() != tchecker::EXPR_TYPE_LOCALINTARRAY))
      _bytecode_back_inserter = tchecker::VM_VALUEAT;
    else
      _bytecode_back_inserter = tchecker::VM_VALUEAT_FRAME;
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
    if ((expr.type() != tchecker::EXPR_TYPE_CLKLVALUE) && (expr.type() != tchecker::EXPR_TYPE_INTLVALUE) &&
        (expr.type() != tchecker::EXPR_TYPE_LOCALINTLVALUE))
      invalid_expression(expr, "an lvalue");

    // Write bytecode (similar to lvalue, except last instruction)
    tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> lvalue_expression_compiler(_bytecode_back_inserter);

    expr.visit(lvalue_expression_compiler);
    if (expr.type() == tchecker::EXPR_TYPE_LOCALINTLVALUE)
      _bytecode_back_inserter = tchecker::VM_VALUEAT_FRAME;
    else
      _bytecode_back_inserter = tchecker::VM_VALUEAT;
  }

  /*
   VM_PUSH expr.value()
   */
  virtual void visit(tchecker::typed_int_expression_t const & expr)
  {
    if (expr.type() != tchecker::EXPR_TYPE_INTTERM)
      invalid_expression(expr, "a integer term");

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
      invalid_expression(expr, "a parenthesized");

    // Write bytecode
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
        invalid_expression(expr, "a conjunction");

      compile_land_expression(expr);
    }
    // LT, LE, EQ, NEQ, GE, GT expression
    else if (tchecker::predicate(expr.binary_operator())) {
      if (expr.type() != tchecker::EXPR_TYPE_ATOMIC_PREDICATE)
        invalid_expression(expr, "an atomic predicate");

      compile_binary_expression(expr);
    }
    // arithmetic operators
    else {
      if (expr.type() != tchecker::EXPR_TYPE_INTTERM)
        invalid_expression(expr, "an integer expression");

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
        invalid_expression(expr, "an unary expression");
    }
    // LNOT expression
    else if (expr.unary_operator() == tchecker::EXPR_OP_LNOT) {
      if (expr.type() != tchecker::EXPR_TYPE_ATOMIC_PREDICATE)
        invalid_expression(expr, "an unary expression");
    }

    compile_unary_expression(expr);
  }

  /*
   see compile_clock_predicate
   */
  virtual void visit(tchecker::typed_simple_clkconstr_expression_t const & expr)
  {
    if (expr.type() != tchecker::EXPR_TYPE_CLKCONSTR_SIMPLE)
      invalid_expression(expr, "a simple clock constraint");

    tchecker::typed_var_expression_t ref_clock(tchecker::EXPR_TYPE_CLKVAR, tchecker::REFCLOCK_NAME, tchecker::REFCLOCK_ID, 1);

    compile_clock_predicate(expr.clock(), ref_clock, expr.bound_ptr(), operator_to_instruction(expr.binary_operator()));
  }

  /*
   see compile_clock_predicate
   */
  virtual void visit(tchecker::typed_diagonal_clkconstr_expression_t const & expr)
  {
    if (expr.type() != tchecker::EXPR_TYPE_CLKCONSTR_DIAGONAL)
      invalid_expression(expr, "a diagonal clock constraint");

    compile_clock_predicate(expr.first_clock(), expr.second_clock(), expr.bound_ptr(),
                            operator_to_instruction(expr.binary_operator()));
  }

  /*
   see compile_ite_expression
   */
  virtual void visit(tchecker::typed_ite_expression_t const & expr)
  {
    if (expr.type() != tchecker::EXPR_TYPE_INTTERM)
      invalid_expression(expr, "an if-then-else");

    compile_ite_expression(expr.condition(), expr.then_value(), expr.else_value());
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
    case tchecker::EXPR_OP_LAND:
      return tchecker::VM_LAND;
    case tchecker::EXPR_OP_LT:
      return tchecker::VM_LT;
    case tchecker::EXPR_OP_LE:
      return tchecker::VM_LE;
    case tchecker::EXPR_OP_EQ:
      return tchecker::VM_EQ;
    case tchecker::EXPR_OP_NEQ:
      return tchecker::VM_NE;
    case tchecker::EXPR_OP_GE:
      return tchecker::VM_GE;
    case tchecker::EXPR_OP_GT:
      return tchecker::VM_GT;
    case tchecker::EXPR_OP_MINUS:
      return tchecker::VM_MINUS;
    case tchecker::EXPR_OP_PLUS:
      return tchecker::VM_SUM;
    case tchecker::EXPR_OP_TIMES:
      return tchecker::VM_MUL;
    case tchecker::EXPR_OP_DIV:
      return tchecker::VM_DIV;
    case tchecker::EXPR_OP_MOD:
      return tchecker::VM_MOD;
    default:
      throw std::runtime_error("incomplete switch statement");
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
    case tchecker::EXPR_OP_NEG:
      return tchecker::VM_NEG;
    case tchecker::EXPR_OP_LNOT:
      return tchecker::VM_LNOT;
    default:
      throw std::runtime_error("incomplete switch statement");
    }
  }

  /*!
   \brief Compiler for clock predicate of the form
   clock1 - clock2 # bound   where # is <,<=,==,>=,> depending on instruction

   \pre instruction is one of tchecker::VM_LT, tchecker::VM_LE,
   tchecker::VM_EQ, tchecker::VM_GE or tchecker::VM_GT
   \throw std::invalid_argument : if the precondition is violated
   */
  void compile_clock_predicate(tchecker::typed_expression_t const & clock1, tchecker::typed_expression_t const & clock2,
                               std::shared_ptr<tchecker::typed_expression_t const> const & bound,
                               enum tchecker::instruction_t instruction)
  {
    // bound negation
    tchecker::typed_unary_expression_t neg_bound(tchecker::type_neg(bound->type()), tchecker::EXPR_OP_NEG, bound);

    // write bytecode
    if (instruction == tchecker::VM_LT)
      compile_clock_constraint(clock1, clock2, tchecker::LT, *bound);
    else if (instruction == tchecker::VM_LE)
      compile_clock_constraint(clock1, clock2, tchecker::LE, *bound);
    else if (instruction == tchecker::VM_GE)
      compile_clock_constraint(clock2, clock1, tchecker::LE, neg_bound);
    else if (instruction == tchecker::VM_GT)
      compile_clock_constraint(clock2, clock1, tchecker::LT, neg_bound);
    else if (instruction == tchecker::VM_EQ) {
      compile_clock_constraint(clock1, clock2, tchecker::LE, *bound);
      compile_clock_constraint(clock2, clock1, tchecker::LE, neg_bound);
      _bytecode_back_inserter = tchecker::VM_LAND;
    }
    else
      throw std::invalid_argument("invalid instruction");
  }

  /*!
   \brief Compile clock constraint: first - second </<= bound

   insert first bytecode
   insert second bytecode
   insert bound bytecode
   VM_CLKCONSTR cmp
   VM_PUSH 1              // abstraction of constraint at syntax level
   */
  void compile_clock_constraint(tchecker::typed_expression_t const & first, tchecker::typed_expression_t const & second,
                                enum tchecker::ineq_cmp_t cmp, tchecker::typed_expression_t const & bound)
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
   * insert expr.left_operand() bytecode
   * insert conditional jump JMPZ over second operand bytecode
   * insert expr.right_operand() bytecode
   */
  void compile_land_expression(tchecker::typed_binary_expression_t const & expr)
  {
    tchecker::typed_int_expression_t zero(EXPR_TYPE_INTTERM, 0);
    compile_ite_expression(expr.left_operand(), expr.right_operand(), zero);
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

  /*
   * insert cond bycode
   *
   insert expr.condition() bytecode
   insert expr.then_value() bytecode
   insert expr.else_value() bytecode
   */
  void compile_ite_expression(tchecker::typed_expression_t const & cond, tchecker::typed_expression_t const & then_value,
                              tchecker::typed_expression_t const & else_value)
  {
    // pre-compute 'then' and 'else' bytecodes
    std::vector<tchecker::bytecode_t> then_bytecode;
    auto then_len = compile_tmp_rvalue_expression(then_value, then_bytecode);

    std::vector<tchecker::bytecode_t> else_bytecode;
    auto else_len = compile_tmp_rvalue_expression(else_value, else_bytecode);

    // generation of ite bytecode
    //  - insert guard bytecode
    cond.visit(*this);

    //  - insert a conditional jump over 'then' bytecode. Jump instruction
    //    move IP relatively to the address following the JMP.
    _bytecode_back_inserter = tchecker::VM_JMPZ;
    _bytecode_back_inserter = then_len + 2;

    //  - insert bytecode for the 'then' statement
    append_bytecode(_bytecode_back_inserter, then_bytecode);

    //  - insert a jump over 'else' bytecode.
    _bytecode_back_inserter = tchecker::VM_JMP;
    _bytecode_back_inserter = else_len;

    //  - insert bytecode for the 'else' statement
    append_bytecode(_bytecode_back_inserter, else_bytecode);
  }

  void invalid_expression(tchecker::typed_expression_t const & expr, std::string expected)
  {
    throw std::invalid_argument("invalid expression '" + expr.to_string() + "' where " + expected + " is expected.");
  }

  BYTECODE_BACK_INSERTER _bytecode_back_inserter; /*!< Bytecode */
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
    throw std::invalid_argument("compiling expression " + expr.to_string() + ", " + e.what());
  }
  catch (std::runtime_error const & e) {
    throw std::runtime_error("compiling expression " + expr.to_string() + ", " + e.what());
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
      : _min(std::numeric_limits<tchecker::integer_t>::min()), _max(std::numeric_limits<tchecker::integer_t>::max())
  {
  }

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
  tchecker::details::variable_bounds_visitor_t & operator=(tchecker::details::variable_bounds_visitor_t const &) = delete;

  /*!
   \brief Move assignment operator (DELETED)
   */
  tchecker::details::variable_bounds_visitor_t & operator=(tchecker::details::variable_bounds_visitor_t &&) = delete;

  /*!
   \brief Accessor
   \return minimal value
   */
  inline tchecker::integer_t min() const { return _min; }

  /*!
   \brief Accessor
   \return maximal value
   */
  inline tchecker::integer_t max() const { return _max; }

  // Visitors

  virtual void visit(tchecker::typed_bounded_var_expression_t const & expr)
  {
    _min = expr.min();
    _max = expr.max();
  }

  virtual void visit(tchecker::typed_array_expression_t const & expr) { expr.variable().visit(*this); }

  virtual void visit(tchecker::typed_int_expression_t const & expr) { not_supported(expr); }

  virtual void visit(tchecker::typed_var_expression_t const & expr) { not_supported(expr); }

  virtual void visit(tchecker::typed_par_expression_t const & expr) { not_supported(expr); }

  virtual void visit(tchecker::typed_binary_expression_t const & expr) { not_supported(expr); }

  virtual void visit(tchecker::typed_unary_expression_t const & expr) { not_supported(expr); }

  virtual void visit(tchecker::typed_simple_clkconstr_expression_t const & expr) { not_supported(expr); }

  virtual void visit(tchecker::typed_diagonal_clkconstr_expression_t const & expr) { not_supported(expr); }

  virtual void visit(tchecker::typed_ite_expression_t const & expr) { not_supported(expr); }

protected:
  void not_supported(tchecker::typed_expression_t const & expr)
  {
    throw std::invalid_argument("not a bounded variable: " + expr.to_string());
  }

  tchecker::integer_t _min; /*!< Variable minimal value */
  tchecker::integer_t _max; /*!< Variable maximal value */
};

/*!
 \class statement_compiler_t
 \brief Visitor for compilation of statements
 */
template <class BYTECODE_BACK_INSERTER> class statement_compiler_t final : public tchecker::typed_statement_visitor_t {
public:
  /*!
   \brief Constructor
   \param back_inserter : back inserter for bytecode
   */
  statement_compiler_t(BYTECODE_BACK_INSERTER bytecode_back_inserter) : _bytecode_back_inserter(bytecode_back_inserter) {}

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
  operator=(tchecker::details::statement_compiler_t<BYTECODE_BACK_INSERTER> const &) = delete;

  /*!
   \brief Move assignment operator (DELETED)
   */
  tchecker::details::statement_compiler_t<BYTECODE_BACK_INSERTER> &
  operator=(tchecker::details::statement_compiler_t<BYTECODE_BACK_INSERTER> &&) = delete;

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

    tchecker::typed_var_expression_t ref_clock(tchecker::EXPR_TYPE_CLKVAR, tchecker::REFCLOCK_NAME, tchecker::REFCLOCK_ID, 1);
    compile_clock_reset(stmt.clock(), stmt.value(), ref_clock);
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

  /*
   * see compile_if_then_else
   */
  virtual void visit(tchecker::typed_if_statement_t const & stmt)
  {
    if (stmt.type() == tchecker::STMT_TYPE_BAD)
      throw std::invalid_argument("invalid statement");

    compile_if_then_else(stmt.condition(), stmt.then_stmt(), stmt.else_stmt());
  }

  /*
   * see compile_while
   */
  virtual void visit(tchecker::typed_while_statement_t const & stmt)
  {
    if (stmt.type() == tchecker::STMT_TYPE_BAD)
      throw std::invalid_argument("invalid statement");

    compile_while(stmt.condition(), stmt.statement());
  }

  virtual void visit(tchecker::typed_local_var_statement_t const & stmt)
  {
    tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> lvalue_compiler(_bytecode_back_inserter);
    stmt.variable().visit(lvalue_compiler);
    tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> rvalue_compiler(_bytecode_back_inserter);
    stmt.initial_value().visit(rvalue_compiler);
    _bytecode_back_inserter = VM_INIT_FRAME;
  }

  virtual void visit(tchecker::typed_local_array_statement_t const & stmt)
  {
    tchecker::variable_size_t id = stmt.variable().id();
    tchecker::variable_size_t asize = stmt.variable().size();

    for (tchecker::variable_size_t i = 0; i < asize; i++) {
      _bytecode_back_inserter = VM_PUSH;
      _bytecode_back_inserter = id + i;
      _bytecode_back_inserter = VM_PUSH;
      _bytecode_back_inserter = 0;
      _bytecode_back_inserter = VM_INIT_FRAME;
    }
  }

protected:
  /*
   Compiler for clock reset:  lvalue = int_rvalue + clock_rvalue

   insert lvalue bytecode
   insert int_rvalue bytecode
   insert clock_rvalue bytecode
   VM_CLKRESET
   */
  void compile_clock_reset(tchecker::typed_lvalue_expression_t const & lvalue, tchecker::typed_expression_t const & int_rvalue,
                           tchecker::typed_lvalue_expression_t const & clock_rvalue)
  {
    tchecker::details::lvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> clocks_compiler(_bytecode_back_inserter);
    tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> int_rvalue_compiler(_bytecode_back_inserter);

    lvalue.visit(clocks_compiler);
    clock_rvalue.visit(clocks_compiler);
    int_rvalue.visit(int_rvalue_compiler);
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
    if (lvalue.type() == EXPR_TYPE_LOCALINTLVALUE || lvalue.type() == EXPR_TYPE_LOCALINTVAR)
      _bytecode_back_inserter = tchecker::VM_ASSIGN_FRAME;
    else
      _bytecode_back_inserter = tchecker::VM_ASSIGN;
  }

  /*
   * insert cond bytecode
   * VM_JMPZ over 'then' bytecode to 'else' bytecode
   * insert 'then' bytecode
   * VM_JMP over 'else' bytecode
   * insert 'else' bytecode
   */
  void compile_if_then_else(tchecker::typed_expression_t const & cond, tchecker::typed_statement_t const & then_stmt,
                            tchecker::typed_statement_t const & else_stmt)
  {
    tchecker::details::rvalue_expression_compiler_t<BYTECODE_BACK_INSERTER> rvalue_compiler(_bytecode_back_inserter);

    // pre-compute then an else bytecodes
    std::vector<tchecker::bytecode_t> then_bytecode;
    auto then_len = compile_tmp_statement(then_stmt, then_bytecode);

    std::vector<tchecker::bytecode_t> else_bytecode;
    auto else_len = compile_tmp_statement(else_stmt, else_bytecode);

    // generation of ite bytecode
    //  - insert guard bytecode
    cond.visit(rvalue_compiler);

    //  - insert a conditional jump over 'then' bytecode. Jump instruction
    //    move IP relatively to the address following the JMP.
    _bytecode_back_inserter = tchecker::VM_JMPZ;
    _bytecode_back_inserter = then_len + 2;

    //  - insert bytecode for the 'then' statement
    append_bytecode(_bytecode_back_inserter, then_bytecode);

    //  - insert a jump over 'else' bytecode.
    _bytecode_back_inserter = tchecker::VM_JMP;
    _bytecode_back_inserter = else_len;

    //  - insert bytecode for the 'else' statement
    append_bytecode(_bytecode_back_inserter, else_bytecode);
  }

  /*
   * insert cond bytecode
   * VM_JMPZ over 'stmt' bytecode and unconditional loop instruction
   * insert 'stmt' bytecode
   * VM_JMP back to cond bytecode
   */
  void compile_while(tchecker::typed_expression_t const & cond, tchecker::typed_statement_t const & stmt)
  {
    // pre-compute cond and stmt bytecodes to get their respective lengths

    std::vector<tchecker::bytecode_t> cond_bytecode;
    auto cond_len = compile_tmp_rvalue_expression(cond, cond_bytecode);

    std::vector<tchecker::bytecode_t> stmt_bytecode;
    auto stmt_len = compile_tmp_statement(stmt, stmt_bytecode);

    // generation of while bytecode
    //  - insert 'cond' bytecode
    append_bytecode(_bytecode_back_inserter, cond_bytecode);

    //  - insert a conditional jump over 'stmt' bytecode and loop instruction
    _bytecode_back_inserter = tchecker::VM_JMPZ;
    _bytecode_back_inserter = stmt_len + 2;

    //  - insert bytecode for the 'stmt' statement
    append_bytecode(_bytecode_back_inserter, stmt_bytecode);

    //  - insert a loop back to 'cond' bytecode
    _bytecode_back_inserter = tchecker::VM_JMP;
    _bytecode_back_inserter = -(cond_len + 2 + stmt_len + 2);
  }

  BYTECODE_BACK_INSERTER _bytecode_back_inserter; /*!< Bytecode back ins. */
};

} // end of namespace details

// Utility routines
namespace details {
std::vector<tchecker::bytecode_t>::size_type compile_tmp_statement(tchecker::typed_statement_t const & stmt,
                                                                   std::vector<tchecker::bytecode_t> & container)
{
  auto sz = container.size();
  auto back_inserter = std::back_inserter(container);
  bool localdecl = tchecker::has_local_declarations(stmt);
  if (localdecl)
    back_inserter = VM_PUSH_FRAME;

  tchecker::details::statement_compiler_t<decltype(back_inserter)> compiler(back_inserter);
  stmt.visit(compiler);
  if (localdecl)
    back_inserter = VM_POP_FRAME;

  if (container.size() - sz == 0)
    throw std::runtime_error("compilation produced no bytecode");

  return container.size() - sz;
}

std::vector<tchecker::bytecode_t>::size_type compile_tmp_rvalue_expression(tchecker::typed_expression_t const & expr,
                                                                           std::vector<tchecker::bytecode_t> & container)
{
  auto sz = container.size();
  auto back_inserter = std::back_inserter(container);
  tchecker::details::rvalue_expression_compiler_t<decltype(back_inserter)> compiler(back_inserter);

  expr.visit(compiler);

  if (container.size() - sz == 0)
    throw std::runtime_error("compilation produced no bytecode");

  return container.size() - sz;
}
} // end of namespace details

tchecker::bytecode_t * compile(tchecker::typed_statement_t const & stmt)
{
  try {
    if (stmt.type() == tchecker::STMT_TYPE_BAD)
      throw std::invalid_argument("invalid statement");

    std::vector<tchecker::bytecode_t> bytecode;
    auto back_inserter = std::back_inserter(bytecode);

    bool localdecl = tchecker::has_local_declarations(stmt);
    if (localdecl)
      back_inserter = VM_PUSH_FRAME;
    tchecker::details::statement_compiler_t<decltype(back_inserter)> compiler(back_inserter);
    stmt.visit(compiler);
    if (localdecl)
      back_inserter = VM_POP_FRAME;
    back_inserter = tchecker::VM_PUSH; // return code...
    back_inserter = 1;                 // ...for statement
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
