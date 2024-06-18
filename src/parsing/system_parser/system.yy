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
%define api.namespace {tchecker::parsing::system}
%define api.prefix {spyy}
%define api.token.constructor
%define api.value.type variant
%define parse.assert true
%define parse.trace true
%define parse.error verbose


%code requires {
  #include <cstdlib>
  #include <iostream>
  #include <sstream>
  #include <limits>
  #include <string>
  #include <vector>
  
  #include <boost/algorithm/string.hpp>
  
  #include "tchecker/basictypes.hh"
  #include "tchecker/parsing/declaration.hh"
  #include "tchecker/utils/log.hh"
}


%param { std::string const & filename }
%param { std::shared_ptr<tchecker::parsing::system_declaration_t> & system_declaration }


%locations


%code {
  // Declare the lexer for the parser's sake.
  tchecker::parsing::system::parser_t::symbol_type spyylex
  (std::string const & filename,
  std::shared_ptr<tchecker::parsing::system_declaration_t> & system_declaration);
  
  // Error detection
  static unsigned int old_error_count;
}

%initial-action {
  // Initialize the initial location.
  @$.begin.filename = @$.end.filename = &const_cast<std::string &>(filename);
  
  old_error_count = tchecker::log_error_count();
};



%token                TOK_CLOCK          "clock"
%token                TOK_COLON          ":"
%token                TOK_AT             "@"
%token                TOK_QMARK          "?"
%token                TOK_LBRACE         "{"
%token                TOK_RBRACE         "}"
%token                TOK_EDGE           "edge"
%token                TOK_EOF          0 "end of file" // 0 means EOF
%token                TOK_EOL            "\n"
%token                TOK_EVENT          "event"
%token <std::string>  TOK_ID             "identifier"
%token                TOK_INT            "int"
%token <std::string>  TOK_INTEGER        "integer value"
%token                TOK_LOCATION       "location"
%token                TOK_PROCESS        "process"
%token                TOK_SYNC           "sync"
%token                TOK_SYSTEM         "system"
%token <std::string>  TOK_TEXT           "text value"


%type <unsigned int>                                                       uinteger
%type <tchecker::integer_t>										                             integer
%type <std::vector<std::shared_ptr<tchecker::parsing::sync_constraint_t>>> sync_constraint_list
%type <std::shared_ptr<tchecker::parsing::sync_constraint_t>>              sync_constraint
%type <std::shared_ptr<tchecker::parsing::attr_t>>                         attr
%type <tchecker::parsing::attributes_t>                                    attr_list
                                                                           non_empty_attr_list
%type <std::string>                                                        text_or_empty
%type <enum tchecker::sync_strength_t>                                     sync_strength

%printer { yyoutput << $$; }                                               <*>;
%printer { yyoutput << * $$; }                                             attr
                                                                           sync_constraint;

%printer {
  for (auto it = $$.begin(); it != $$.end(); ++it) {
    if (it != $$.begin())
      yyoutput << ",";
    yyoutput << *it;
  }
}                                                                          sync_constraint_list;

%start system

%%

system:
eol_sequence TOK_SYSTEM ":" TOK_ID attr_list end_declaration {
  std::stringstream loc;
  loc << @$;
  system_declaration = std::make_shared<tchecker::parsing::system_declaration_t>($4, $5, loc.str());
}
declaration_list
{
  if (tchecker::log_error_count() > old_error_count)
    system_declaration = nullptr;
}
| error TOK_EOF
{
  system_declaration = nullptr;
}
;


declaration_list:
non_empty_declaration_list eol_sequence
{}
| eol_sequence
{}
;


non_empty_declaration_list:
eol_sequence declaration
{}
| non_empty_declaration_list eol_sequence declaration
{}
;


declaration:
TOK_CLOCK ":" uinteger ":" TOK_ID attr_list end_declaration
{
  auto exist_d = system_declaration->get_clock_declaration($5);
  if (exist_d != nullptr)
    std::cerr << tchecker::log_error << @5 << " multiple declarations of clock " << $5 << std::endl;
  else {
    auto intd = system_declaration->get_int_declaration($5);
    if (intd != nullptr)
      std::cerr << tchecker::log_error << @5 << " variable " << $5 << " already declared as an int" << std::endl;
    else {
      try {
        std::stringstream loc;
        loc << @$;
        auto d = std::make_shared<tchecker::parsing::clock_declaration_t>($5, $3, $6, loc.str());
        if ( ! system_declaration->insert_clock_declaration(d) )
          std::cerr << tchecker::log_error << @$ << " insertion of clock declaration failed" << std::endl;
      }
      catch (std::exception const & e) {
        std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
      }
    }
  }
}

| TOK_EDGE ":" TOK_ID ":" TOK_ID ":" TOK_ID ":" TOK_ID attr_list end_declaration
{
  auto proc = system_declaration->get_process_declaration($3);
  if (proc == nullptr)
    std::cerr << tchecker::log_error << @3 << " process " << $3 << " is not declared" << std::endl;
  else {
    auto src = system_declaration->get_location_declaration($3, $5);
    if (src == nullptr)
      std::cerr << tchecker::log_error << @5 << " location " << $5 << " is not declared in process " << $3 << std::endl;
    else {
      auto tgt = system_declaration->get_location_declaration($3, $7);
      if (tgt == nullptr)
        std::cerr << tchecker::log_error << @7 << " location " << $7 << " is not declared in process " << $3 << std::endl;
      else {
        auto event = system_declaration->get_event_declaration($9);
        if (event == nullptr)
          std::cerr << tchecker::log_error << @9 << " event " << $9 << " is not declared" << std::endl;
        else {
          try {
            std::stringstream loc;
            loc << @$;
            auto d = std::make_shared<tchecker::parsing::edge_declaration_t>(proc, src, tgt, event, $10, loc.str());
            if ( ! system_declaration->insert_edge_declaration(d) ) {
              std::cerr << tchecker::log_error << @$ << " insertion of edge declaration failed" << std::endl;
              d = nullptr;
            }
          }
          catch (std::exception const & e) {
            std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
          }
        }
      }
    }
  }
}

| TOK_EVENT ":" TOK_ID attr_list end_declaration
{
  auto exist_d = system_declaration->get_event_declaration($3);
  if (exist_d != nullptr)
    std::cerr << tchecker::log_error << @3 << " multiple declarations of event " << $3 << std::endl;
  else {
    try {
      std::stringstream loc;
      loc << @$;
      auto d = std::make_shared<tchecker::parsing::event_declaration_t>($3, $4, loc.str());
      if ( ! system_declaration->insert_event_declaration(d) ) {
        std::cerr << tchecker::log_error << @$ << " insertion of event declaration failed" << std::endl;
        d = nullptr;
      }
    }
    catch (std::exception const & e) {
      std::cerr << @$ << " " << e.what() << std::endl;
    }
  }
}

| TOK_INT ":" uinteger ":" integer ":" integer ":" integer ":" TOK_ID attr_list end_declaration
{
  auto exist_d = system_declaration->get_int_declaration($11);
  if (exist_d != nullptr)
    std::cerr << tchecker::log_error << @11 << " multiple declarations of int variable " << $11 << std::endl;
  else {
    auto clockd = system_declaration->get_clock_declaration($11);
    if (clockd != nullptr)
      std::cerr << tchecker::log_error << @11 << " variable " << $11 << " already declared as a clock" << std::endl;
    else {
      try {
        std::stringstream loc;
        loc << @$;
        auto d = std::make_shared<tchecker::parsing::int_declaration_t>($11, $3, $5, $7, $9, $12, loc.str());
        if ( ! system_declaration->insert_int_declaration(d) ) {
          std::cerr << tchecker::log_error << @$ << " insertion of int declaration failed" << std::endl;
          d = nullptr;
        }
      }
      catch (std::exception const & e) {
        std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
      }
    }
  }
}

| TOK_LOCATION ":" TOK_ID ":" TOK_ID attr_list end_declaration
{
  auto exist_d = system_declaration->get_location_declaration($3, $5);
  if (exist_d != nullptr)
    std::cerr << tchecker::log_error << @5 << " multiple declarations of location " << $5 << " in process " << $3 << std::endl;
  else {
    auto proc = system_declaration->get_process_declaration($3);
    if (proc == nullptr)
      std::cerr << tchecker::log_error << @3 << " process " << $3 << " is not declared" << std::endl;
    else {
      try {
        std::stringstream loc;
        loc << @$;
        auto d = std::make_shared<location_declaration_t>($5, proc, $6, loc.str());
        if ( ! system_declaration->insert_location_declaration(d) ) {
          std::cerr << tchecker::log_error << @$ << " insertion of location declaration failed" << std::endl;
          d = nullptr;
        }
      }
      catch (std::exception const & e) {
        std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
      }
    }
  }
}

| TOK_PROCESS ":" TOK_ID attr_list end_declaration
{
  auto exist_d = system_declaration->get_process_declaration($3);
  if (exist_d != nullptr)
    std::cerr << tchecker::log_error << @3 << " multiple declarations of process " << $3 << std::endl;
  else {
    try {
      std::stringstream loc;
      loc << @$;
      auto d = std::make_shared<tchecker::parsing::process_declaration_t>($3, $4, loc.str());
      if ( ! system_declaration->insert_process_declaration(d) ) {
        std::cerr << tchecker::log_error << @5 << " insertion of process declaration failed" << std::endl;
        d = nullptr;
      }
    }
    catch (std::exception const & e) {
      std::cerr << tchecker::log_error << @5 << " " << e.what() << std::endl;
    }
  }
}

| TOK_SYNC ":" sync_constraint_list attr_list end_declaration
{
  try {
    std::stringstream loc;
    loc << @$;
    auto d = std::make_shared<tchecker::parsing::sync_declaration_t>($3, $4, loc.str());
    if ( ! system_declaration->insert_sync_declaration(d) ) {
      std::cerr << tchecker::log_error << @$ << " insertion of sync declaration failed" << std::endl;
      d = nullptr;
    }
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @$ << " " << e.what() << std::endl;
  }
}

| error end_declaration
;


attr_list:
"{" non_empty_attr_list "}"
{ $$ = std::move($2); }
| "{" "}"
{ $$.clear(); }
|
{ $$.clear(); }
;


non_empty_attr_list:
attr
{
  $$.insert($1);
}
| non_empty_attr_list ":" attr
{
  $1.insert($3);
  $$ = std::move($1);
}
;


attr:
TOK_ID ":" text_or_empty
{
  if ($1 == "")
    throw std::runtime_error("empty tokens should not be accepted by the parser");
  std::stringstream key_loc, value_loc;
  key_loc << @1;
  value_loc << @3;
  boost::trim($3);
  $$ = std::make_shared<tchecker::parsing::attr_t>($1, $3, tchecker::parsing::attr_parsing_position_t{key_loc.str(), value_loc.str()});
}
;


text_or_empty:
TOK_TEXT
{ $$ = std::move($1); }
|
{ $$.clear(); }
;


sync_constraint_list:
sync_constraint
{
  if ($1 != nullptr)
    $$.push_back($1);
}
| sync_constraint_list ":" sync_constraint
{
  if ($3 != nullptr)
    $1.push_back($3);
  $$ = std::move($1);
}
;


sync_constraint:
TOK_ID "@" TOK_ID sync_strength
{
  auto proc = system_declaration->get_process_declaration($1);
  if (proc == nullptr)
    std::cerr << tchecker::log_error << @1 << " process " << $1 << " is not declared" << std::endl;
  else {
    auto event = system_declaration->get_event_declaration($3);
    if (event == nullptr)
      std::cerr << tchecker::log_error << @3 << " event " << $3 << " is not declared" << std::endl;
    else
      $$ = std::make_shared<tchecker::parsing::sync_constraint_t>(proc, event, $4);
  }
}
;


sync_strength:
"?"
{ $$ = tchecker::SYNC_WEAK; }
|
{ $$ = tchecker::SYNC_STRONG; }
;


end_declaration:
TOK_EOL
| TOK_EOF
;


eol_sequence:
eol_sequence TOK_EOL
|
;


integer:
TOK_INTEGER
{
  try {
    long long l = std::stoll($1, nullptr, 10);
    if ( (l < std::numeric_limits<tchecker::integer_t>::min()) || (l > std::numeric_limits<tchecker::integer_t>::max()) )
      throw std::out_of_range("integer constant our of range");
    $$ = static_cast<tchecker::integer_t>(l);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @1 << " value " << $1 << " out of range ";
    std::cerr << std::numeric_limits<tchecker::integer_t>::min() << "," << std::numeric_limits<tchecker::integer_t>::max();
    std::cerr << std::endl;
    $$ = 0;
  }
}
;


uinteger:
TOK_INTEGER
{
  try {
    unsigned long l = std::stoul($1, nullptr, 10);
    if (l > std::numeric_limits<unsigned int>::max())
      throw std::out_of_range("unsigned integer out of range");
    $$ = static_cast<unsigned int>(l);
  }
  catch (std::exception const & e) {
    std::cerr << tchecker::log_error << @1 << " value " << $1 << " out of range ";
    std::cerr << std::numeric_limits<unsigned int>::min() << "," << std::numeric_limits<unsigned int>::max();
    std::cerr << std::endl;
    $$ = 0;
  }
}
;


%%

void tchecker::parsing::system::parser_t::error(location_type const & l, std::string const & msg)
{
  std::cerr << tchecker::log_error << l << " " << msg << std::endl;
}
