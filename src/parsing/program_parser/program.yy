/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

%skeleton "lalr1.cc" // c++
%require "3.0.4"

%defines
%define parser_class_name {parser_t}
%define api.namespace {tchecker::parsing::program}
%define api.prefix {ppyy}
%define api.token.constructor
%define api.value.type variant
%define parse.assert true
%define parse.trace true
%define parse.error verbose


%code requires {
  #include <exception>
  #include <iostream>
  #include <limits>
  #include <memory>
  #include <string>
  #include <tuple>
  
  #include "tchecker/expression/expression.hh"
  #include "tchecker/parsing/parsing.hh"
  #include "tchecker/statement/statement.hh"
  #include "tchecker/utils/log.hh"
  #include "tchecker/basictypes.hh"
}


%param { std::string const & program_context }
%param { std::shared_ptr<tchecker::expression_t> & expr }
%param { std::shared_ptr<tchecker::statement_t> & stmt }

%locations


%code {
  // Declare the lexer for the parser's sake.
  tchecker::parsing::program::parser_t::symbol_type ppyylex
  (std::string const & program_context,
  std::shared_ptr<tchecker::expression_t> & expr,
  std::shared_ptr<tchecker::statement_t> & stmt);
  
  
  // Global variables
  static unsigned int old_error_count;
  
  
  // Fake expression used is case of syntax error to allow parsing of the
  // entire expression
  class fake_expression_t final : public tchecker::lvalue_expression_t {
  public:
    virtual ~fake_expression_t() = default;
    virtual std::ostream & output(std::ostream & os) const override { return os; }
    virtual fake_expression_t * clone() const override { return new fake_expression_t(); }
    virtual void visit(tchecker::expression_visitor_t & v) const override {}
  };

  auto fake_expression = std::make_shared<fake_expression_t>();
  
  // Fake variable expression used is case of syntax error to allow parsing of
  // the entire expression
  class fake_var_expression_t final : public tchecker::var_expression_t {
  public:
    fake_var_expression_t() : tchecker::var_expression_t("tck__") {}
    virtual ~fake_var_expression_t() = default;
    virtual std::ostream & output(std::ostream & os) const override { return os; }
    virtual fake_var_expression_t * clone() const override { return new fake_var_expression_t(); }
    virtual void visit(tchecker::expression_visitor_t & v) const override {}
  };
  
  auto fake_var_expression = std::make_shared<fake_var_expression_t>();
  
  // Fake statement used in case of syntax error to allow parsing of the
  // entire statement
  class fake_statement_t final : public tchecker::statement_t {
  public:
    virtual ~fake_statement_t() = default;
    virtual std::ostream & output(std::ostream & os) const override { return os; }
    virtual fake_statement_t * clone() const override { return new fake_statement_t(); }
    virtual void visit(tchecker::statement_visitor_t & v) const override {}
  };

  auto fake_statement = std::make_shared<fake_statement_t>();
}


%initial-action {
  old_error_count = tchecker::log_error_count();
};


%token                TOK_ASSIGN            "="
%token                TOK_LPAR              "("
%token                TOK_RPAR              ")"
%token                TOK_LBRACKET          "["
%token                TOK_RBRACKET          "]"
%token                TOK_SEMICOLON         ";"
%token                TOK_PLUS              "+"
%token                TOK_MINUS             "-"
%token                TOK_TIMES             "*"
%token                TOK_DIV               "/"
%token                TOK_MODULO            "%"
%token                TOK_LAND              "&&"
%token                TOK_LNOT              "!"
%token                TOK_EQ                "=="
%token                TOK_NEQ               "!="
%token                TOK_LE                "<="
%token                TOK_GE                ">="
%token                TOK_LT                "<"
%token                TOK_GT                ">"
%token                TOK_IF                "if"
%token                TOK_END               "end"
%token                TOK_THEN              "then"
%token                TOK_ELSE              "else"
%token                TOK_WHILE             "while"
%token                TOK_DO                "do"
%token                TOK_NOP               "nop"
%token                TOK_LOCAL             "local"
%token <std::string>  TOK_ID                "identifier"
%token <std::string>  TOK_INTEGER           "integer"
%token                TOK_EOF          0    "end of file"


%nonassoc             "="
%left                 "&&"
%nonassoc             "=="  "!="  "<="  ">="  "<"  ">"
%left                 "+"  "-"
%left                 "*"  "/"  "%"
%left                 "!"
%precedence           UMINUS



%type <std::shared_ptr<tchecker::statement_t>>         assignment
                                                       sequence_statement
                                                       statement
                                                       simple_statement
                                                       if_statement
                                                       loop_statement
                                                       local_statement

%type <std::shared_ptr<tchecker::expression_t>>        atomic_formula
                                                       conjunctive_formula
                                                       non_atomic_conjunctive_formula
                                                       binary_formula
                                                       predicate_formula
                                                       term
%type <std::shared_ptr<tchecker::lvalue_expression_t>> lvalue_term
%type <std::shared_ptr<tchecker::var_expression_t>>    variable_term
%type <enum tchecker::binary_operator_t>               predicate_operator
%type <tchecker::integer_t>                            integer



%printer { yyoutput << $$; }                <*>;
%printer { yyoutput << * $$; }              assignment
                                            atomic_formula
                                            conjunctive_formula
                                            non_atomic_conjunctive_formula
                                            predicate_formula
                                            sequence_statement
                                            statement
                                            lvalue_term
                                            variable_term
                                            term;


%start   program


%%

program:
sequence_statement
{ expr = nullptr;
  if (tchecker::log_error_count() > old_error_count)
    stmt = nullptr;
  else
    stmt = $1;
}
| conjunctive_formula
{
  if (tchecker::log_error_count() > old_error_count)
    expr = nullptr;
  else
    expr = $1;
  stmt = nullptr;
}
;

opt_semicolon :
    ";"
|
;

sequence_statement:
statement opt_semicolon
{ $$ = $1; }
|  statement ";" sequence_statement
{
  try {
    $$ = std::make_shared<tchecker::sequence_statement_t>($1, $3);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_statement;
  }
}
;

statement:
    simple_statement
    { $$ = $1; }
|   if_statement
    { $$ = $1; }
|   loop_statement
    { $$ = $1; }
;

if_statement:
    TOK_IF conjunctive_formula TOK_THEN sequence_statement TOK_END
    {  $$ = std::make_shared<tchecker::if_statement_t>($2, $4, std::make_shared<tchecker::nop_statement_t>()); }
|   TOK_IF conjunctive_formula TOK_THEN sequence_statement TOK_ELSE sequence_statement TOK_END
    {  $$ = std::make_shared<tchecker::if_statement_t>($2, $4, $6); }
;

loop_statement:
    TOK_WHILE conjunctive_formula TOK_DO sequence_statement TOK_END
    { $$ = std::make_shared<tchecker::while_statement_t>($2, $4); }
;

local_statement:
    TOK_LOCAL variable_term
    { $$ = std::make_shared<tchecker::local_var_statement_t>($2); }
|   TOK_LOCAL variable_term TOK_ASSIGN term
    { $$ = std::make_shared<tchecker::local_var_statement_t>($2, $4); }
|   TOK_LOCAL variable_term TOK_LBRACKET term TOK_RBRACKET
    { $$ = std::make_shared<tchecker::local_array_statement_t>($2, $4); }
;

simple_statement :
    assignment
    { $$ = $1; }
|   "nop"
    { $$ = std::make_shared<tchecker::nop_statement_t>(); }
|   local_statement
    { $$ = $1; }
;


assignment:
lvalue_term "=" term
{
  try {
    $$ = std::make_shared<tchecker::assign_statement_t>($1, $3);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_statement;
  }
}
;


conjunctive_formula:
atomic_formula
{ $$ = $1; }
| non_atomic_conjunctive_formula
{ $$ = $1; }
;


non_atomic_conjunctive_formula:
"(" non_atomic_conjunctive_formula ")"
{
  try {
    $$ = std::make_shared<tchecker::par_expression_t>($2);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
| atomic_formula "&&" conjunctive_formula
{
  try {
    $$ = std::make_shared<tchecker::binary_expression_t>(tchecker::EXPR_OP_LAND, $1, $3);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
;


atomic_formula:
term
{ $$ = $1; }
| predicate_formula
{ $$ = $1; }
| "!" atomic_formula
{
  try {
    $$ = std::make_shared<tchecker::unary_expression_t>(tchecker::EXPR_OP_LNOT, $2);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
;


predicate_formula:
"(" predicate_formula ")"
{
  try {
    $$ = std::make_shared<tchecker::par_expression_t>($2);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
| binary_formula predicate_operator term
{
  try {
    auto left = std::dynamic_pointer_cast<tchecker::binary_expression_t>($1);
    if (! tchecker::is_less(left->binary_operator()) || ! tchecker::is_less($2)) {
      tchecker::parsing::program::parser_t::error(@$, "Only < and <= are allowed in combined expressions");
      $$ = fake_expression;
    }
    else {
      std::shared_ptr<tchecker::expression_t> middle_clone{left->right_operand().clone()};
      auto right = std::make_shared<tchecker::binary_expression_t>($2, middle_clone, $3);
      $$ = std::make_shared<tchecker::binary_expression_t>(tchecker::EXPR_OP_LAND, left, right);
    }
  } 
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
| binary_formula
{ $$ = $1; }
;


binary_formula:
term predicate_operator term
{
  try {
    $$ = std::make_shared<tchecker::binary_expression_t>($2, $1, $3);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
;


predicate_operator:
"=="      { $$ = tchecker::EXPR_OP_EQ; }
| "!="    { $$ = tchecker::EXPR_OP_NEQ; }
| "<"     { $$ = tchecker::EXPR_OP_LT; }
| "<="    { $$ = tchecker::EXPR_OP_LE; }
| ">="    { $$ = tchecker::EXPR_OP_GE; }
| ">"     { $$ = tchecker::EXPR_OP_GT; }
;


term:
integer
{
  try {
    $$ = std::make_shared<tchecker::int_expression_t>($1);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
| lvalue_term
{ $$ = $1; }
| "(" term ")"
{
  try {
    $$ = std::make_shared<tchecker::par_expression_t>($2);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
| "-" term        %prec UMINUS
{
  try {
    $$ = std::make_shared<tchecker::unary_expression_t>(tchecker::EXPR_OP_NEG, $2);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
| term "+" term
{
  try {
    $$ = std::make_shared<tchecker::binary_expression_t>(tchecker::EXPR_OP_PLUS, $1, $3);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
| term "-" term
{
  try {
    $$ = std::make_shared<tchecker::binary_expression_t>(tchecker::EXPR_OP_MINUS, $1, $3);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
| term "*" term
{
  try {
    $$ = std::make_shared<tchecker::binary_expression_t>(tchecker::EXPR_OP_TIMES, $1, $3);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
| term "/" term
{
  try {
    $$ = std::make_shared<tchecker::binary_expression_t>(tchecker::EXPR_OP_DIV, $1, $3);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
| term "%" term
{
  try {
    $$ = std::make_shared<tchecker::binary_expression_t>(tchecker::EXPR_OP_MOD, $1, $3);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
| TOK_LPAR TOK_IF conjunctive_formula TOK_THEN term TOK_ELSE term TOK_RPAR
{
  try {
    $$ = std::make_shared<tchecker::ite_expression_t>($3, $5, $7);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
;


lvalue_term:
variable_term
{ $$ = $1; }
| variable_term "[" term "]"
{
  try {
    $$ = std::make_shared<tchecker::array_expression_t>($1, $3);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_expression;
  }
}
;


variable_term:
TOK_ID
{
  try {
    $$ = std::make_shared<tchecker::var_expression_t>($1);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
    $$ = fake_var_expression;
  }
}
;


integer:
TOK_INTEGER
{
  try {
    long long l = std::stoll($1, nullptr, 10);
    if ( (l < std::numeric_limits<integer_t>::min()) || (l > std::numeric_limits<integer_t>::max()) )
      throw std::out_of_range("");
    $$ = static_cast<tchecker::integer_t>(l);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @1 << " value " << $1 << " out of range ";
    std::cerr << std::numeric_limits<integer_t>::min() << "," << std::numeric_limits<integer_t>::max() << std::endl;
    $$ = 0;
  }
}
;


%%


void tchecker::parsing::program::parser_t::error(location_type const & l, std::string const & msg)
{
  if (program_context.empty())
    std::cerr << tchecker::log_error << l << " " << msg << std::endl;
  else
    std::cerr << tchecker::log_error << program_context << " " << msg << " (at " << l << ")" << std::endl;
}
