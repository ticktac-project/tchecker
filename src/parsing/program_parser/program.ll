/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

%{
#include <cstdlib>
#include <memory>
#include <sstream>

#include "tchecker/expression/expression.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/statement/statement.hh"
#include "tchecker/utils/log.hh"

#include "program.tab.hh"


// Tell Flex the lexer's prototype ...
#define YY_DECL \
tchecker::parsing::program::parser_t::symbol_type ppyylex \
(std::string const & program_context, \
std::shared_ptr<tchecker::expression_t> & expr, \
std::shared_ptr<tchecker::statement_t> & stmt)

// Work around an incompatibility in flex (at least versions
// 2.5.31 through 2.5.33): it generates code that does
// not conform to C89.  See Debian bug 333231
// <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.
#undef yywrap
#define yywrap() 1

// The location of the current token.
static tchecker::parsing::program::location loc;
	
void pp_reset_locations()
{
	loc.initialize(nullptr, 1, 1);
}

using namespace tchecker::parsing;
%}

%option noyywrap nounput batch noinput
/*%option debug*/

id         [[:alpha:]_$][[:alnum:]_]*
integer    [0-9]+
blankspace [ \t\r]
newline    [\n]

%{
  // Code run each time a pattern is matched.
	// move token's length wide
  #define YY_USER_ACTION  loc.columns(static_cast<int>(ppyyleng));
%}

%%

%{
	// Code run each time ppyylex is called.
	loc.step();
%}

"if"           { return program::parser_t::make_TOK_IF(loc); }
"end"          { return program::parser_t::make_TOK_END(loc); }
"then"         { return program::parser_t::make_TOK_THEN(loc); }
"else"         { return program::parser_t::make_TOK_ELSE(loc); }
"while"        { return program::parser_t::make_TOK_WHILE(loc); }
"do"           { return program::parser_t::make_TOK_DO(loc); }
"local"        { return program::parser_t::make_TOK_LOCAL(loc); }

"="            { return program::parser_t::make_TOK_ASSIGN(loc); }
"("            { return program::parser_t::make_TOK_LPAR(loc); }
")"            { return program::parser_t::make_TOK_RPAR(loc); }
"["            { return program::parser_t::make_TOK_LBRACKET(loc); }
"]"            { return program::parser_t::make_TOK_RBRACKET(loc); }
";"            { return program::parser_t::make_TOK_SEMICOLON(loc); }
"+"            { return program::parser_t::make_TOK_PLUS(loc); }
"-"            { return program::parser_t::make_TOK_MINUS(loc); }
"*"            { return program::parser_t::make_TOK_TIMES(loc); }
"/"            { return program::parser_t::make_TOK_DIV(loc); }
"%"            { return program::parser_t::make_TOK_MODULO(loc); }
"&&"           { return program::parser_t::make_TOK_LAND(loc); }
"!"            { return program::parser_t::make_TOK_LNOT(loc); }
"=="           { return program::parser_t::make_TOK_EQ(loc); }
"!="           { return program::parser_t::make_TOK_NEQ(loc); }
"<="           { return program::parser_t::make_TOK_LE(loc); }
">="           { return program::parser_t::make_TOK_GE(loc); }
"<"            { return program::parser_t::make_TOK_LT(loc); }
">"            { return program::parser_t::make_TOK_GT(loc); }
"nop"          { return program::parser_t::make_TOK_NOP(loc); }
{integer}      { return program::parser_t::make_TOK_INTEGER(ppyytext,loc); }
{id}           { return program::parser_t::make_TOK_ID(ppyytext, loc); }
{newline}+     { loc.lines(static_cast<int>(ppyyleng)); loc.step(); }
{blankspace}+  { loc.step(); }
<<EOF>>        { return program::parser_t::make_TOK_EOF(loc); }

<*>.|{newline} { std::stringstream msg;
                 msg << loc << " Invalid character: " << ppyytext;
                 throw std::runtime_error(msg.str()); }

%%


namespace tchecker {
	
	namespace parsing {
		
		void parse_program(std::string const & prog_context,
                       std::string const & prog_str,
                       std::shared_ptr<tchecker::expression_t> & expr,
                       std::shared_ptr<tchecker::statement_t> & stmt)
		{
			std::size_t old_error_count = tchecker::log_error_count();
      
			expr = nullptr;
			stmt = nullptr;
      
			// Scan from expr
			YY_BUFFER_STATE previous_buffer = YY_CURRENT_BUFFER;
			YY_BUFFER_STATE current_buffer = ppyy_scan_string(prog_str.c_str());
			
			// Initialise
			pp_reset_locations();
			BEGIN INITIAL;
			
			// Parse
			try {
				tchecker::parsing::program::parser_t parser(prog_context, expr, stmt);
				parser.parse();
				ppyy_delete_buffer(current_buffer);
				ppyy_switch_to_buffer(previous_buffer);
			}
			catch (...) {
				expr = nullptr;
				stmt = nullptr;
				ppyy_delete_buffer(current_buffer);
				ppyy_switch_to_buffer(previous_buffer);
				throw;
			}
      
			if (tchecker::log_error_count() > old_error_count) {
				expr = nullptr;
				stmt = nullptr;
			}
		}

		
		
		
		std::shared_ptr<tchecker::expression_t> parse_expression(std::string const & expr_context, std::string const & expr_str)
		{
			std::shared_ptr<tchecker::expression_t> expr{nullptr};
			std::shared_ptr<tchecker::statement_t> stmt{nullptr};
			
			try {
				parse_program(expr_context, expr_str, expr, stmt);
				assert( (expr.get() == nullptr) || (stmt.get() == nullptr) );
				if (stmt.get() != nullptr)
					std::cerr << tchecker::log_error << expr_context << " unexpected statement, expression expected" << std::endl;
			}
			catch (...) {
				throw;
			}
			
			return expr;
		}
		
		
		
		
		std::shared_ptr<tchecker::statement_t> parse_statement(std::string const & stmt_context, std::string const & stmt_str)
		{
			std::shared_ptr<tchecker::expression_t> expr{nullptr};
			std::shared_ptr<tchecker::statement_t> stmt{nullptr};
			
			try {
				parse_program(stmt_context, stmt_str, expr, stmt);
				assert( (expr.get() == nullptr) || (stmt.get() == nullptr) );
				if (expr.get() != nullptr)
					std::cerr << tchecker::log_error << stmt_context << " unexpected expression, statement expected" << std::endl;
			}
			catch (...) {
				throw;
			}
			
			return stmt;
		}
		
	}
	
}

