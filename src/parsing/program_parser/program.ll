/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

%{
#include <cstdlib>
#include <sstream>

#include "tchecker/expression/expression.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/statement/statement.hh"
#include "tchecker/utils/log.hh"

#include "program.tab.hh"


// Tell Flex the lexer's prototype ...
#define YY_DECL \
tchecker::parsing::program::parser_t::symbol_type pplex \
(std::string const & program_context, \
tchecker::log_t & log, \
tchecker::expression_t * & expr, \
tchecker::statement_t * & stmt)


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

id       [[:alpha:]_][[:alnum:]_]*
integer  [0-9]+

%{
  // Code run each time a pattern is matched.
	// move token's length wide
  #define YY_USER_ACTION  loc.columns(static_cast<int>(yyleng));
%}

%%

%{
	// Code run each time pplex is called.
	loc.step();
%}

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
{integer}      { return program::parser_t::make_TOK_INTEGER(yytext,loc); }
{id}           { return program::parser_t::make_TOK_ID(yytext, loc); }
[\n]+          { loc.lines(static_cast<int>(yyleng)); loc.step(); }
[ \t]+         { loc.step(); }
<<EOF>>        { return program::parser_t::make_TOK_EOF(loc); }

<*>.|\n        { std::stringstream msg;
                 msg << loc << " Invalid character: " << yytext;
                 throw std::runtime_error(msg.str()); }

%%


namespace tchecker {
	
	namespace parsing {
		
		void parse_program(std::string const & prog_context,
                       std::string const & prog_str,
                       tchecker::log_t & log,
                       tchecker::expression_t * & expr,
                       tchecker::statement_t * & stmt)
		{
			expr = nullptr;
			stmt = nullptr;
      
			// Scan from expr
      YY_BUFFER_STATE previous_buffer = YY_CURRENT_BUFFER;
			YY_BUFFER_STATE current_buffer = yy_scan_string(prog_str.c_str());
			
			// Initialise
			pp_reset_locations();
			BEGIN INITIAL;
			
			// Parse
			try {
				tchecker::parsing::program::parser_t parser(prog_context, log, expr, stmt);
				parser.parse();
        yy_delete_buffer(current_buffer);
        yy_switch_to_buffer(previous_buffer);
			}
			catch (...) {
        yy_delete_buffer(current_buffer);
        yy_switch_to_buffer(previous_buffer);
				throw;
			}
		}

		
		
		
		tchecker::expression_t * parse_expression(std::string const & expr_context,
																							std::string const & expr_str,
																							tchecker::log_t & log)
		{
			tchecker::expression_t * expr = nullptr;
      tchecker::statement_t * stmt = nullptr;
			
			try {
				parse_program(expr_context, expr_str, log, expr, stmt);
				assert( (expr == nullptr) || (stmt == nullptr) );
				if (stmt != nullptr) {
					delete stmt;
					log.error(expr_context, "unexpected statement, expression expected");
				}
			}
			catch (...) {
				throw;
			}
			
			return expr;
		}
		
		
		
		
    tchecker::statement_t * parse_statement(std::string const & stmt_context,
																						std::string const & stmt_str,
																						tchecker::log_t & log)
		{
			tchecker::expression_t * expr = nullptr;
			tchecker::statement_t * stmt = nullptr;
			
			try {
				parse_program(stmt_context, stmt_str, log, expr, stmt);
				assert( (expr == nullptr) || (stmt == nullptr) );
				if (expr != nullptr) {
					delete expr;
					log.error(stmt_context, "unexpected expression, statement expected");
				}
			}
			catch (...) {
				throw;
			}
			
			return stmt;
		}
		
	}
	
}

