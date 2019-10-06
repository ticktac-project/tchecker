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
  #include <limits>
  #include <sstream>
  #include <string>
  #include <vector>
  
  #include <boost/algorithm/string.hpp>
  
  #include "tchecker/basictypes.hh"
  #include "tchecker/parsing/declaration.hh"
  #include "tchecker/utils/log.hh"
}


%param { std::string const & filename }
%param { tchecker::log_t & log }
%param { tchecker::parsing::system_declaration_t * & system_declaration }


%locations


%code {
  // Declare the lexer for the parser's sake.
  tchecker::parsing::system::parser_t::symbol_type spyylex
  (std::string const & filename,
  tchecker::log_t & log,
  tchecker::parsing::system_declaration_t * & system_declaration);
  
  // Error detection
  static unsigned int error_count;
}

%initial-action {
  // Initialize the initial location.
  @$.begin.filename = @$.end.filename = &const_cast<std::string &>(filename);
  
  error_count = 0;
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


%type <unsigned int>                                                uinteger
%type <tchecker::integer_t>										                      integer
%type <std::vector<tchecker::parsing::sync_constraint_t const *>>   sync_constraint_list
%type <tchecker::parsing::sync_constraint_t *>                      sync_constraint
%type <tchecker::parsing::attr_t *>                                 attr
%type <tchecker::parsing::attributes_t>                             attr_list
                                                                    non_empty_attr_list
%type <std::string>                                                 text_or_empty
%type <enum tchecker::sync_strength_t>                              sync_strength

%printer { yyoutput << $$; }                                        <*>;
%printer { yyoutput << * $$; }                                      attr
                                                                    sync_constraint;
%printer {
  for (auto it = $$.begin(); it != $$.end(); ++it) {
    if (it != $$.begin())
    yyoutput << ",";
    yyoutput << **it;
  }
}                                                                   sync_constraint_list;

%start system

%%

system:
eol_sequence TOK_SYSTEM ":" TOK_ID {
  system_declaration = new system_declaration_t($4);
}
"\n" eol_sequence declaration_list
{
  if (error_count > 0) {
    delete system_declaration;
    system_declaration = nullptr;
  }
}
| error TOK_EOF
{
  delete system_declaration;
  system_declaration = nullptr;
}
;


declaration_list:
non_empty_declaration_list eol_sequence
{}
|
{}
;


non_empty_declaration_list:
declaration
{}
| non_empty_declaration_list eol_sequence declaration
{}
;


declaration:
TOK_CLOCK ":" uinteger ":" TOK_ID "\n"
{
  auto const * d = system_declaration->get_clock_declaration($5);
  if (d != nullptr)
  error(@5, "multiple declarations of clock " + $5);
  else {
    auto const * intd = system_declaration->get_int_declaration($5);
    if (intd != nullptr)
    error(@5, "variable " + $5 + " already declared as an int");
    else {
      try {
        d = new tchecker::parsing::clock_declaration_t($5, $3);
        if ( ! system_declaration->insert_clock_declaration(d) ) {
          error(@$, "insertion of clock declaration failed");
          delete d;
        }
      }
      catch (std::exception const & e) {
        error(@$, e.what());
      }
    }
  }
}

| TOK_EDGE ":" TOK_ID ":" TOK_ID ":" TOK_ID ":" TOK_ID attr_list "\n"
{
  auto const * proc = system_declaration->get_process_declaration($3);
  if (proc == nullptr)
  error(@3, "process " + $3 + " is not declared");
  else {
    auto const * src = system_declaration->get_location_declaration($3, $5);
    if (src == nullptr)
      error(@5, "location " + $5 + " is not declared in process " +$3);
    else {
      auto const * tgt = system_declaration->get_location_declaration($3, $7);
      if (tgt == nullptr)
        error(@7, "location " + $7 + " is not declared in process " + $3);
      else {
        auto const * event = system_declaration->get_event_declaration($9);
        if (event == nullptr)
          error(@9, "event " + $9 + " is not declared");
        else {
          try {
            auto * d = new tchecker::parsing::edge_declaration_t(*proc, *src, *tgt, *event, std::move($10));
            if ( ! system_declaration->insert_edge_declaration(d) ) {
              error(@$, "insertion of edge declaration failed");
              delete d;
            }
          }
          catch (std::exception const & e) {
            error(@$, e.what());
          }
        }
      }
    }
  }
}

| TOK_EVENT ":" TOK_ID "\n"
{
  auto const * d = system_declaration->get_event_declaration($3);
  if (d != nullptr)
    error(@3, "multiple declarations of event " + $3);
  else {
    try {
      d = new tchecker::parsing::event_declaration_t($3);
      if ( ! system_declaration->insert_event_declaration(d) ) {
        error(@$, "insertion of event declaration failed");
        delete d;
      }
    }
    catch (std::exception const & e) {
      error(@$, e.what());
    }
  }
}

| TOK_INT ":" uinteger ":" integer ":" integer ":" integer ":" TOK_ID "\n"
{
  auto const * d = system_declaration->get_int_declaration($11);
  if (d != nullptr)
    error(@11, "multiple declarations of int variable " + $11);
  else {
    auto const * clockd = system_declaration->get_clock_declaration($11);
    if (clockd != nullptr)
      error(@11, "variable " + $11 + " already declared as a clock");
    else {
      try {
        d = new tchecker::parsing::int_declaration_t($11, $3, $5, $7, $9);
        if ( ! system_declaration->insert_int_declaration(d) ) {
          error(@$, "insertion of int declaration failed");
          delete d;
        }
      }
      catch (std::exception const & e) {
        error(@$, e.what());
      }
    }
  }
}

| TOK_LOCATION ":" TOK_ID ":" TOK_ID attr_list "\n"
{
  auto const * d = system_declaration->get_location_declaration($3, $5);
  if (d != nullptr)
    error(@5, "multiple declarations of location " + $5 + " in process " + $3);
  else {
    auto const * proc = system_declaration->get_process_declaration($3);
    if (proc == nullptr)
      error(@3, "process " + $3 + " is not declared");
    else {
      try {
        d = new location_declaration_t($5, *proc, std::move($6));
        if ( ! system_declaration->insert_location_declaration(d) ) {
          error(@$, "insertion of location declaration failed");
          delete d;
        }
      }
      catch (std::exception const & e) {
        error(@$, e.what());
      }
    }
  }
}

| TOK_PROCESS ":" TOK_ID "\n"
{
  auto const * d = system_declaration->get_process_declaration($3);
  if (d != nullptr)
    error(@3, "multiple declarations of process " + $3);
  else {
    try {
      d = new tchecker::parsing::process_declaration_t($3);
      if ( ! system_declaration->insert_process_declaration(d) ) {
        error(@$, "insertion of process declaration failed");
        delete d;
      }
    }
    catch (std::exception const & e) {
      error(@$, e.what());
    }
  }
}

| TOK_SYNC ":" sync_constraint_list "\n"
{
  try {
    auto const * d = new tchecker::parsing::sync_declaration_t(std::move($3));
    if ( ! system_declaration->insert_sync_declaration(d) ) {
      error(@$, "insertion of sync declaration failed");
      delete d;
    }
  }
  catch (std::exception const & e) {
    error(@$, e.what());
  }
}

| error "\n"
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
  if ($1 != nullptr) {
    if ( ! $$.insert($1) ) {
      error(@1, "multiple instances of attribute " + $1->key());
      delete $1;
    }
  }
}
| non_empty_attr_list ":" attr
{
  if ($3 != nullptr) {
    if ( ! $1.insert($3) ) {
      error(@3, "multiple instances of attribute " + $3->key());
      delete $3;
    }
  }
  $$ = std::move($1);
}
;


attr:
TOK_ID ":" text_or_empty
{
  $$ = nullptr;
  if ($1 == "")
    error(@1, "empty attribute key");
  else {
    std::stringstream key_loc, value_loc;
    key_loc << @1;
    value_loc << @3;
    boost::trim($3);
    $$ = new tchecker::parsing::attr_t($1, $3, key_loc.str(), value_loc.str());
  }
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
  $$ = nullptr;
  auto const * proc = system_declaration->get_process_declaration($1);
  if (proc == nullptr)
  error(@1, "process " + $1 + " is not declared");
  else {
    auto const * event = system_declaration->get_event_declaration($3);
    if (event == nullptr)
    error(@3, "event " + $3 + " is not declared");
    else {
      try {
        $$ = new tchecker::parsing::sync_constraint_t(*proc, *event, $4);
      }
      catch (std::exception const & e) {
        error(@$, e.what());
      }
    }
  }
}
;


sync_strength:
"?"
{ $$ = tchecker::SYNC_WEAK; }
|
{ $$ = tchecker::SYNC_STRONG; }
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
    error(@1, "value " + $1 + " out of range " + std::to_string(std::numeric_limits<tchecker::integer_t>::min()) +
    "," + std::to_string(std::numeric_limits<tchecker::integer_t>::max()) );
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
    error(@1, "value " + $1 + " out of range " + std::to_string(std::numeric_limits<unsigned int>::min()) +
    "," + std::to_string(std::numeric_limits<unsigned int>::max()) );
    $$ = 0;
  }
}
;


%%

void tchecker::parsing::system::parser_t::error(location_type const & l, std::string const & msg)
{
  log.error(l, msg);
  ++ error_count;
}
