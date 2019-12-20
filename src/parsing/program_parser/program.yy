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
  #include <limits>
  #include <sstream>
  #include <string>
  #include <tuple>
  
  #include "tchecker/basictypes.hh"
  #include "tchecker/expression/expression.hh"
  #include "tchecker/parsing/parsing.hh"
  #include "tchecker/statement/statement.hh"
  #include "tchecker/utils/log.hh"
}


%param { std::string const & program_context }
%param { tchecker::log_t & log }
%param { tchecker::expression_t * & expr }
%param { tchecker::statement_t * & stmt }

%locations


%code {
  // Declare the lexer for the parser's sake.
  tchecker::parsing::program::parser_t::symbol_type ppyylex
  (std::string const & program_context,
  tchecker::log_t & log,
  tchecker::expression_t * & expr,
  tchecker::statement_t * & stmt);
  
  
  // Global variables
  static unsigned int error_count;
  
  
  // Fake expression used is case of syntax error to allow parsing of the
  // entire expression
  class fake_expression_t final : public tchecker::lvalue_expression_t {
    public:
    virtual ~fake_expression_t() = default;
    private:
    virtual std::ostream & do_output(std::ostream & os) const
    {
      return os;
    }
    
    virtual tchecker::expression_t * do_clone() const
    {
      return new fake_expression_t();
    }
    
    virtual void do_visit(tchecker::expression_visitor_t & v) const
    {}
  };
  
  
  // Fake variable expression used is case of syntax error to allow parsing of
  // the entire expression
  class fake_var_expression_t final : public tchecker::var_expression_t {
    public:
    fake_var_expression_t() : tchecker::var_expression_t("fake") {}
    
    virtual ~fake_var_expression_t() = default;
    private:
    virtual std::ostream & do_output(std::ostream & os) const
    {
      return os;
    }
    
    virtual tchecker::expression_t * do_clone() const
    {
      return new fake_var_expression_t();
    }
    
    virtual void do_visit(tchecker::expression_visitor_t & v) const
    {}
  };
  
  
  
  // Fake statement used in case of syntax error to allow parsing of the
  // entire statement
  class fake_statement_t final : public tchecker::statement_t {
    public:
    virtual ~fake_statement_t() = default;
    private:
    virtual std::ostream & do_output(std::ostream & os) const
    {
      return os;
    }
    
    virtual tchecker::statement_t * do_clone() const
    {
      return new fake_statement_t();
    }
    
    virtual void do_visit(tchecker::statement_visitor_t & v) const
    {}
  };
}


%initial-action {
  error_count = 0;
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



%type <tchecker::statement_t *>             assignment
                                            sequence_statement
                                            statement
                                            simple_statement
                                            if_statement
                                            loop_statement
                                            local_statement

%type <tchecker::expression_t *>            atomic_formula
                                            conjunctive_formula
                                            non_atomic_conjunctive_formula
                                            predicate_formula
                                            term
%type <tchecker::lvalue_expression_t *>			lvalue_term
%type <tchecker::var_expression_t *>        variable_term
%type <enum tchecker::binary_operator_t>    predicate_operator
%type <tchecker::integer_t>                 integer



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
  if (error_count > 0) {
    stmt = nullptr;
    delete $1;
  }
  else
    stmt = $1;
}
| conjunctive_formula
{
  if (error_count > 0) {
    expr = nullptr;
    delete $1;
  }
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
    $$ = new tchecker::sequence_statement_t($1, $3);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_statement_t();
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
    {  $$ = new tchecker::if_statement_t($2, $4, new tchecker::nop_statement_t()); }
|   TOK_IF conjunctive_formula TOK_THEN sequence_statement TOK_ELSE sequence_statement TOK_END
    {  $$ = new tchecker::if_statement_t($2, $4, $6); }
;

loop_statement:
    TOK_WHILE conjunctive_formula TOK_DO sequence_statement TOK_END
    { $$ = new tchecker::while_statement_t($2, $4); }
;

local_statement:
    TOK_LOCAL variable_term
    { $$ = new tchecker::local_var_statement_t($2); }
|   TOK_LOCAL variable_term TOK_ASSIGN term
    { $$ = new tchecker::local_var_statement_t($2, $4); }
|   TOK_LOCAL variable_term TOK_LBRACKET term TOK_RBRACKET
    { $$ = new tchecker::local_array_statement_t($2, $4); }
;

simple_statement :
    assignment
    { $$ = $1; }
|   "nop"
    { $$ = new tchecker::nop_statement_t(); }
|   local_statement
    { $$ = $1; }
;


assignment:
lvalue_term "=" term
{
  try {
    $$ = new tchecker::assign_statement_t($1, $3);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_statement_t();
  }
}
;


conjunctive_formula:
atomic_formula
{ $$ = $1; }
| non_atomic_conjunctive_formula
{ $$ =$1; }
;


non_atomic_conjunctive_formula:
"(" non_atomic_conjunctive_formula ")"
{
  try {
    $$ = new tchecker::par_expression_t($2);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
  }
}
| atomic_formula "&&" conjunctive_formula
{
  try {
    $$ = new tchecker::binary_expression_t(tchecker::EXPR_OP_LAND, $1, $3);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
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
    $$ = new tchecker::unary_expression_t(tchecker::EXPR_OP_LNOT, $2);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
  }
}
;


predicate_formula:
"(" predicate_formula ")"
{
  try {
    $$ = new tchecker::par_expression_t($2);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
  }
}
| term predicate_operator term
{
  try {
    $$ = new tchecker::binary_expression_t($2, $1, $3);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
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
    $$ = new tchecker::int_expression_t($1);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
  }
}
| lvalue_term
{ $$ = $1; }
| "(" term ")"
{
  try {
    $$ = new tchecker::par_expression_t($2);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
  }
}
| "-" term        %prec UMINUS
{
  try {
    $$ = new tchecker::unary_expression_t(tchecker::EXPR_OP_NEG, $2);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
  }
}
| term "+" term
{
  try {
    $$ = new tchecker::binary_expression_t(tchecker::EXPR_OP_PLUS, $1, $3);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
  }
}
| term "-" term
{
  try {
    $$ = new tchecker::binary_expression_t(tchecker::EXPR_OP_MINUS, $1, $3);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
  }
}
| term "*" term
{
  try {
    $$ = new tchecker::binary_expression_t(tchecker::EXPR_OP_TIMES, $1, $3);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
  }
}
| term "/" term
{
  try {
    $$ = new tchecker::binary_expression_t(tchecker::EXPR_OP_DIV, $1, $3);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
  }
}
| term "%" term
{
  try {
    $$ = new tchecker::binary_expression_t(tchecker::EXPR_OP_MOD, $1, $3);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
  }
}
| TOK_LPAR TOK_IF conjunctive_formula TOK_THEN term TOK_ELSE term TOK_RPAR
{
  try {
    $$ = new tchecker::ite_expression_t($3, $5, $7);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
  }
}
;


lvalue_term:
variable_term
{ $$ = $1; }
| variable_term "[" term "]"
{
  try {
    $$ = new tchecker::array_expression_t($1, $3);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_expression_t();
  }
}
;


variable_term:
TOK_ID
{
  try {
    $$ = new tchecker::var_expression_t($1);
  }
  catch (std::exception const & e) {
    error(@$, e.what());
    $$ = new fake_var_expression_t();
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
    error(@1, "value " + $1 + " out of range " + std::to_string(std::numeric_limits<integer_t>::min()) + "," +
    std::to_string(std::numeric_limits<integer_t>::max()) );
    $$ = 0;
  }
}
;


%%


void tchecker::parsing::program::parser_t::error(location_type const & l, std::string const & msg)
{
  if (program_context.empty()) {
    log.error(l, msg);
  }
  else {
    std::stringstream strl;
    strl << l;
    log.error(program_context, msg + " (at " + strl.str() + ")");
  }
  ++ error_count;
}
