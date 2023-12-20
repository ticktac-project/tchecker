/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "tchecker/clockbounds/solver.hh"
#include "tchecker/expression/expression.hh"
#include "tchecker/expression/type_inference.hh"
#include "tchecker/expression/typechecking.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/statement/statement.hh"
#include "tchecker/statement/typechecking.hh"
#include "tchecker/ta/static_analysis.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/utils/log.hh"
#include "tchecker/vm/compilers.hh"

namespace tchecker {

namespace ta {

system_t::system_t(tchecker::parsing::system_declaration_t const & sysdecl) : tchecker::syncprod::system_t(sysdecl)
{
  compute_from_syncprod_system();
}

system_t::system_t(tchecker::system::system_t const & system) : tchecker::syncprod::system_t(system)
{
  compute_from_syncprod_system();
}

system_t::system_t(tchecker::syncprod::system_t const & system) : tchecker::syncprod::system_t(system)
{
  compute_from_syncprod_system();
}

system_t::system_t(tchecker::ta::system_t const & system)
    : tchecker::syncprod::system_t(system.as_syncprod_system()), _vm(system._vm)
{
  compute_from_syncprod_system();
}

tchecker::ta::system_t & system_t::operator=(tchecker::ta::system_t const & system)
{
  if (this != &system) {
    tchecker::syncprod::system_t::operator=(system);
    _vm = system._vm;
    compute_from_syncprod_system();
  }
  return *this;
}

tchecker::system::attribute_keys_map_t const & system_t::known_attributes()
{
  static tchecker::system::attribute_keys_map_t const known_attr{[&]() {
    tchecker::system::attribute_keys_map_t attr(tchecker::syncprod::system_t::known_attributes());
    attr[tchecker::system::ATTR_EDGE].insert("do");
    attr[tchecker::system::ATTR_EDGE].insert("provided");
    attr[tchecker::system::ATTR_LOCATION].insert("invariant");
    attr[tchecker::system::ATTR_LOCATION].insert("urgent");
    return attr;
  }()};
  return known_attr;
}

tchecker::typed_expression_t const & system_t::guard(tchecker::edge_id_t id) const
{
  assert(is_edge(id));
  return *_guards[id]._typed_expr;
}

tchecker::bytecode_t const * system_t::guard_bytecode(tchecker::edge_id_t id) const
{
  assert(is_edge(id));
  return _guards[id]._compiled_expr.get();
}

tchecker::typed_statement_t const & system_t::statement(tchecker::edge_id_t id) const
{
  assert(is_edge(id));
  return *_statements[id]._typed_stmt;
}

tchecker::bytecode_t const * system_t::statement_bytecode(tchecker::edge_id_t id) const
{
  assert(is_edge(id));
  return _statements[id]._compiled_stmt.get();
}

bool system_t::is_urgent(tchecker::loc_id_t id) const
{
  assert(is_location(id));
  return _urgent[id] == 1;
}

tchecker::typed_expression_t const & system_t::invariant(tchecker::loc_id_t id) const
{
  assert(is_location(id));
  return *_invariants[id]._typed_expr;
}

tchecker::bytecode_t const * system_t::invariant_bytecode(tchecker::loc_id_t id) const
{
  assert(is_location(id));
  return _invariants[id]._compiled_expr.get();
}

void system_t::compute_from_syncprod_system()
{
  _invariants.clear();
  _guards.clear();
  _statements.clear();
  _urgent.reset();

  tchecker::loc_id_t const locations_count = this->locations_count();
  tchecker::edge_id_t const edges_count = this->edges_count();

  _invariants.resize(locations_count);
  _guards.resize(edges_count);
  _statements.resize(edges_count);
  _urgent.resize(locations_count);

  for (tchecker::loc_id_t id = 0; id < locations_count; ++id) {
    auto const & attributes = tchecker::syncprod::system_t::location(id)->attributes();
    set_invariant(id, attributes.range("invariant"));
    set_urgent(id, attributes.range("urgent"));
  }

  for (tchecker::edge_id_t id = 0; id < edges_count; ++id) {
    auto const & attributes = tchecker::syncprod::system_t::edge(id)->attributes();
    set_guards(id, attributes.range("provided"));
    set_statements(id, attributes.range("do"));
  }

  if (tchecker::ta::has_guarded_weakly_synchronized_event(*this))
    throw std::invalid_argument("Transitions over weakly synchronized events should not have guards");
}

static std::shared_ptr<tchecker::expression_t>
conjunction_from_attributes(tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & attributes,
                            tchecker::integer_variables_t const & localvars, tchecker::integer_variables_t const & intvars,
                            tchecker::clock_variables_t clocks)
{
  std::shared_ptr<tchecker::expression_t> expr{nullptr};

  for (auto && attr : attributes) {
    // parse
    std::shared_ptr<tchecker::expression_t> value_expr{
        tchecker::parsing::parse_expression(attr.parsing_position().value_position(), attr.value())};

    if (value_expr.get() == nullptr)
      return nullptr;

    // type check
    std::shared_ptr<tchecker::typed_expression_t> typed_value_expr{
        tchecker::typecheck(*value_expr, localvars, intvars, clocks)};

    if (!tchecker::bool_valued(typed_value_expr->type())) {
      std::stringstream oss;
      oss << attr.parsing_position().value_position() << " expression is not bool valued";
      throw std::invalid_argument(oss.str());
    }

    // aggregate
    if (expr.get() != nullptr)
      expr = std::make_shared<tchecker::binary_expression_t>(tchecker::EXPR_OP_LAND, expr, value_expr);
    else
      expr = value_expr;
  }

  // empty list of attributes
  if (expr.get() == nullptr)
    expr = std::make_shared<tchecker::int_expression_t>(1);

  return expr;
}

void system_t::set_invariant(tchecker::loc_id_t id,
                             tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & invariants)
{
  tchecker::integer_variables_t localvars;

  std::shared_ptr<tchecker::expression_t> invariant_expr{
      conjunction_from_attributes(invariants, localvars, integer_variables(), clock_variables())};
  if (invariant_expr.get() == nullptr)
    throw std::invalid_argument("Syntax error");

  std::shared_ptr<tchecker::typed_expression_t> invariant_typed_expr{
      tchecker::typecheck(*invariant_expr, localvars, integer_variables(), clock_variables())};
  assert(tchecker::bool_valued(invariant_typed_expr->type()));

  try {
    std::shared_ptr<tchecker::bytecode_t> invariant_bytecode{tchecker::compile(*invariant_typed_expr),
                                                             std::default_delete<tchecker::bytecode_t[]>()};
    _invariants[id] = {invariant_typed_expr, invariant_bytecode};
  }
  catch (std::exception const & e) {
    std::stringstream oss;
    oss << e.what();
    throw std::invalid_argument(oss.str());
  }
}

void system_t::set_urgent(tchecker::loc_id_t id,
                          tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & flags)
{
  if (flags.begin() != flags.end())
    _urgent[id] = 1;
}

void system_t::set_guards(tchecker::edge_id_t id,
                          tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & guards)
{
  tchecker::integer_variables_t localvars;

  std::shared_ptr<tchecker::expression_t> guard_expr{
      conjunction_from_attributes(guards, localvars, integer_variables(), clock_variables())};
  if (guard_expr.get() == nullptr)
    throw std::invalid_argument("Syntax error");

  std::shared_ptr<tchecker::typed_expression_t> guard_typed_expr{
      tchecker::typecheck(*guard_expr, localvars, integer_variables(), clock_variables())};
  assert(tchecker::bool_valued(guard_typed_expr->type()));

  try {
    std::shared_ptr<tchecker::bytecode_t> guard_bytecode{tchecker::compile(*guard_typed_expr),
                                                         std::default_delete<tchecker::bytecode_t[]>()};
    _guards[id] = {guard_typed_expr, guard_bytecode};
  }
  catch (std::exception const & e) {
    std::stringstream oss;
    oss << e.what();
    throw std::invalid_argument(oss.str());
  }
}

static std::shared_ptr<tchecker::statement_t>
sequence_from_attributes(tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & attributes,
                         tchecker::integer_variables_t const & localvars, tchecker::integer_variables_t const & intvars,
                         tchecker::clock_variables_t const & clocks)
{
  std::shared_ptr<tchecker::statement_t> stmt{nullptr};

  for (auto && attr : attributes) {
    // parse
    std::shared_ptr<tchecker::statement_t> value_stmt{
        tchecker::parsing::parse_statement(attr.parsing_position().value_position(), attr.value())};

    if (value_stmt.get() == nullptr)
      return nullptr;

    // type check
    std::shared_ptr<tchecker::typed_statement_t> typed_value_stmt{
        tchecker::typecheck(*value_stmt, localvars, intvars, clocks, [&](std::string const & e) {
          std::cerr << tchecker::log_error << attr.parsing_position().value_position() << " " << e << std::endl;
        })};

    // aggregate
    if (stmt.get() != nullptr)
      stmt = std::make_shared<tchecker::sequence_statement_t>(stmt, value_stmt);
    else
      stmt = value_stmt;
  }

  // empty list of statements
  if (stmt.get() == nullptr)
    stmt = std::make_shared<tchecker::nop_statement_t>();

  return stmt;
}

void system_t::set_statements(tchecker::edge_id_t id,
                              tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> const & statements)
{
  tchecker::integer_variables_t localvars;

  std::shared_ptr<tchecker::statement_t> stmt{
      sequence_from_attributes(statements, localvars, integer_variables(), clock_variables())};
  if (stmt.get() == nullptr)
    throw std::invalid_argument("Syntax error");

  std::shared_ptr<tchecker::typed_statement_t> typed_stmt{
      tchecker::typecheck(*stmt, localvars, integer_variables(), clock_variables(),
                          [](std::string const & e) { std::cerr << tchecker::log_error << e << std::endl; })};

  try {
    std::shared_ptr<tchecker::bytecode_t> bytecode{tchecker::compile(*typed_stmt),
                                                   std::default_delete<tchecker::bytecode_t[]>()};
    _statements[id] = {typed_stmt, bytecode};
  }
  catch (std::exception const & e) {
    std::stringstream oss;
    oss << e.what();
    throw std::invalid_argument(oss.str());
  }
}

} // end of namespace ta

} // end of namespace tchecker
