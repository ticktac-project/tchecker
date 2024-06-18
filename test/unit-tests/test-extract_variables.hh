/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <memory>
#include <string>
#include <unordered_set>

#include "tchecker/basictypes.hh"
#include "tchecker/expression/expression.hh"
#include "tchecker/expression/static_analysis.hh"
#include "tchecker/expression/typechecking.hh"
#include "tchecker/expression/typed_expression.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/statement/statement.hh"
#include "tchecker/statement/static_analysis.hh"
#include "tchecker/statement/typechecking.hh"
#include "tchecker/statement/typed_statement.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"

TEST_CASE("expression with no array variables", "[extract_variables]")
{
  tchecker::integer_variables_t intvars;
  intvars.declare("i", 1, 0, 5, 0);
  intvars.declare("j", 1, 0, 1, 1);
  intvars.declare("k", 1, -2, 2, 0);

  tchecker::intvar_id_t i = intvars.id("i");
  tchecker::intvar_id_t j = intvars.id("j");
  tchecker::intvar_id_t k = intvars.id("k");

  tchecker::clock_variables_t clocks;
  clocks.declare("x", 1);
  clocks.declare("y", 1);

  tchecker::clock_id_t x = clocks.id("x");
  tchecker::clock_id_t y = clocks.id("y");

  SECTION("single variable")
  {
    std::string const expr_str{"x"};
    std::shared_ptr<tchecker::expression_t> expr{tchecker::parsing::parse_expression("", expr_str)};
    REQUIRE(expr != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_expression_t> typed_expr{tchecker::typecheck(*expr, lvars, intvars, clocks)};
    REQUIRE(typed_expr != nullptr);

    std::unordered_set<tchecker::clock_id_t> expr_clocks;
    std::unordered_set<tchecker::intvar_id_t> expr_intvars;

    tchecker::extract_variables(*typed_expr, expr_clocks, expr_intvars);

    REQUIRE(expr_intvars.empty());

    REQUIRE(expr_clocks.size() == 1);
    REQUIRE(expr_clocks.find(x) != expr_clocks.end());
  }

  SECTION("simple comparison")
  {
    std::string const expr_str{"x < i"};

    std::shared_ptr<tchecker::expression_t> expr{tchecker::parsing::parse_expression("", expr_str)};
    REQUIRE(expr != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_expression_t> typed_expr{tchecker::typecheck(*expr, lvars, intvars, clocks)};
    REQUIRE(typed_expr != nullptr);

    std::unordered_set<tchecker::clock_id_t> expr_clocks;
    std::unordered_set<tchecker::intvar_id_t> expr_intvars;

    tchecker::extract_variables(*typed_expr, expr_clocks, expr_intvars);

    REQUIRE(expr_intvars.size() == 1);
    REQUIRE(expr_intvars.find(i) != expr_intvars.end());

    REQUIRE(expr_clocks.size() == 1);
    REQUIRE(expr_clocks.find(x) != expr_clocks.end());
  }

  SECTION("complex comparison")
  {
    std::string const expr_str{"x - y < i + 3 * j"};

    std::shared_ptr<tchecker::expression_t> expr{tchecker::parsing::parse_expression("", expr_str)};
    REQUIRE(expr != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_expression_t> typed_expr{tchecker::typecheck(*expr, lvars, intvars, clocks)};
    REQUIRE(typed_expr != nullptr);

    std::unordered_set<tchecker::clock_id_t> expr_clocks;
    std::unordered_set<tchecker::intvar_id_t> expr_intvars;

    tchecker::extract_variables(*typed_expr, expr_clocks, expr_intvars);

    REQUIRE(expr_intvars.size() == 2);
    REQUIRE(expr_intvars.find(i) != expr_intvars.end());
    REQUIRE(expr_intvars.find(j) != expr_intvars.end());

    REQUIRE(expr_clocks.size() == 2);
    REQUIRE(expr_clocks.find(x) != expr_clocks.end());
    REQUIRE(expr_clocks.find(y) != expr_clocks.end());
  }

  SECTION("conjunction")
  {
    std::string const expr_str{"x - y < i + 3 * j && k < i && y >= k - (j / i)"};

    std::shared_ptr<tchecker::expression_t> expr{tchecker::parsing::parse_expression("", expr_str)};
    REQUIRE(expr != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_expression_t> typed_expr{tchecker::typecheck(*expr, lvars, intvars, clocks)};
    REQUIRE(typed_expr != nullptr);

    std::unordered_set<tchecker::clock_id_t> expr_clocks;
    std::unordered_set<tchecker::intvar_id_t> expr_intvars;

    tchecker::extract_variables(*typed_expr, expr_clocks, expr_intvars);

    REQUIRE(expr_intvars.size() == 3);
    REQUIRE(expr_intvars.find(i) != expr_intvars.end());
    REQUIRE(expr_intvars.find(j) != expr_intvars.end());
    REQUIRE(expr_intvars.find(k) != expr_intvars.end());

    REQUIRE(expr_clocks.size() == 2);
    REQUIRE(expr_clocks.find(x) != expr_clocks.end());
    REQUIRE(expr_clocks.find(y) != expr_clocks.end());
  }
}

TEST_CASE("expression with array variables", "[extract_variables]")
{
  tchecker::integer_variables_t intvars;
  intvars.declare("i", 1, 0, 5, 0);
  intvars.declare("j", 1, 0, 1, 1);
  intvars.declare("t", 5, -2, 2, 0);
  intvars.declare("u", 3, 0, 7, 1);

  tchecker::intvar_id_t i = intvars.id("i");
  tchecker::intvar_id_t j = intvars.id("j");
  tchecker::intvar_id_t t = intvars.id("t");
  tchecker::intvar_id_t u = intvars.id("u");

  tchecker::clock_variables_t clocks;
  clocks.declare("x", 1);
  clocks.declare("y", 3);

  tchecker::clock_id_t x = clocks.id("x");
  tchecker::clock_id_t y = clocks.id("y");

  SECTION("single array variable with const index")
  {
    std::string const expr_str{"t[1]"};

    std::shared_ptr<tchecker::expression_t> expr{tchecker::parsing::parse_expression("", expr_str)};
    REQUIRE(expr != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_expression_t> typed_expr{tchecker::typecheck(*expr, lvars, intvars, clocks)};
    REQUIRE(typed_expr != nullptr);

    std::unordered_set<tchecker::clock_id_t> expr_clocks;
    std::unordered_set<tchecker::intvar_id_t> expr_intvars;

    tchecker::extract_variables(*typed_expr, expr_clocks, expr_intvars);

    REQUIRE(expr_intvars.size() == 1);
    REQUIRE(expr_intvars.find(t + 1) != expr_intvars.end());

    REQUIRE(expr_clocks.empty());
  }

  SECTION("single array variable with non-const index")
  {
    std::string const expr_str{"y[i-7*j]"};

    std::shared_ptr<tchecker::expression_t> expr{tchecker::parsing::parse_expression("", expr_str)};
    REQUIRE(expr != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_expression_t> typed_expr{tchecker::typecheck(*expr, lvars, intvars, clocks)};
    REQUIRE(typed_expr != nullptr);

    std::unordered_set<tchecker::clock_id_t> expr_clocks;
    std::unordered_set<tchecker::intvar_id_t> expr_intvars;

    tchecker::extract_variables(*typed_expr, expr_clocks, expr_intvars);

    REQUIRE(expr_intvars.size() == 2);
    REQUIRE(expr_intvars.find(i) != expr_intvars.end());
    REQUIRE(expr_intvars.find(j) != expr_intvars.end());

    std::size_t y_size = clocks.info(y).size();
    REQUIRE(expr_clocks.size() == y_size);
    for (std::size_t idx = 0; idx < y_size; ++idx)
      REQUIRE(expr_clocks.find(static_cast<tchecker::clock_id_t>(y + idx)) != expr_clocks.end());
  }

  SECTION("single array variable with nested array access")
  {
    std::string const expr_str{"y[t[7*i] + u[2]]"};

    std::shared_ptr<tchecker::expression_t> expr{tchecker::parsing::parse_expression("", expr_str)};
    REQUIRE(expr != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_expression_t> typed_expr{tchecker::typecheck(*expr, lvars, intvars, clocks)};
    REQUIRE(typed_expr != nullptr);

    std::unordered_set<tchecker::clock_id_t> expr_clocks;
    std::unordered_set<tchecker::intvar_id_t> expr_intvars;

    tchecker::extract_variables(*typed_expr, expr_clocks, expr_intvars);

    std::size_t t_size = intvars.info(t).size();
    REQUIRE(expr_intvars.size() == t_size + 2); // t[] + i + u[2]
    REQUIRE(expr_intvars.find(i) != expr_intvars.end());
    REQUIRE(expr_intvars.find(u + 2) != expr_intvars.end());
    for (std::size_t idx = 0; idx < t_size; ++idx)
      REQUIRE(expr_intvars.find(static_cast<tchecker::intvar_id_t>(t + idx)) != expr_intvars.end());

    std::size_t y_size = clocks.info(y).size();
    REQUIRE(expr_clocks.size() == y_size);
    for (std::size_t idx = 0; idx < y_size; ++idx)
      REQUIRE(expr_clocks.find(static_cast<tchecker::clock_id_t>(y + idx)) != expr_clocks.end());
  }

  SECTION("complex expression with arrays")
  {
    std::string const expr_str{"y[u[2]] < u[0] && x - y[1] == i - 4*j && u[0] == t[j]"};

    std::shared_ptr<tchecker::expression_t> expr{tchecker::parsing::parse_expression("", expr_str)};
    REQUIRE(expr != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_expression_t> typed_expr = tchecker::typecheck(*expr, lvars, intvars, clocks);
    REQUIRE(typed_expr != nullptr);

    std::unordered_set<tchecker::clock_id_t> expr_clocks;
    std::unordered_set<tchecker::intvar_id_t> expr_intvars;

    tchecker::extract_variables(*typed_expr, expr_clocks, expr_intvars);

    std::size_t t_size = intvars.info(t).size();
    REQUIRE(expr_intvars.size() == t_size + 4); // t[] + i + j + u[0] + u[2]
    REQUIRE(expr_intvars.find(i) != expr_intvars.end());
    REQUIRE(expr_intvars.find(j) != expr_intvars.end());
    REQUIRE(expr_intvars.find(u + 0) != expr_intvars.end());
    REQUIRE(expr_intvars.find(u + 2) != expr_intvars.end());
    for (std::size_t idx = 0; idx < t_size; ++idx)
      REQUIRE(expr_intvars.find(static_cast<tchecker::intvar_id_t>(t + idx)) != expr_intvars.end());

    std::size_t y_size = clocks.info(y).size();
    REQUIRE(expr_clocks.size() == y_size + 1); // y[] + x
    REQUIRE(expr_clocks.find(x) != expr_clocks.end());
    for (std::size_t idx = 0; idx < y_size; ++idx)
      REQUIRE(expr_clocks.find(static_cast<tchecker::clock_id_t>(y + idx)) != expr_clocks.end());
  }
}

TEST_CASE("statements with no array variable", "[extract_variables]")
{
  tchecker::integer_variables_t intvars;
  intvars.declare("i", 1, 0, 5, 0);
  intvars.declare("j", 1, 0, 1, 1);
  intvars.declare("k", 1, -2, 2, 0);

  tchecker::intvar_id_t i = intvars.id("i");
  tchecker::intvar_id_t j = intvars.id("j");
  tchecker::intvar_id_t k = intvars.id("k");

  tchecker::clock_variables_t clocks;
  clocks.declare("x", 1);
  clocks.declare("y", 1);
  clocks.declare("z", 1);

  tchecker::clock_id_t x = clocks.id("x");
  tchecker::clock_id_t y = clocks.id("y");
  tchecker::clock_id_t z = clocks.id("z");

  SECTION("single constant to clock assignment")
  {
    std::string const stmt_str{"x = 0"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.empty());
    REQUIRE(read_intvars.empty());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.size() == 1);
    REQUIRE(written_clocks.find(x) != written_clocks.end());
    REQUIRE(written_intvars.empty());
  }

  SECTION("single integer variable to clock assignment")
  {
    std::string const stmt_str{"x = i"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.empty());
    REQUIRE(read_intvars.size() == 1);
    REQUIRE(read_intvars.find(i) != read_intvars.end());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.size() == 1);
    REQUIRE(written_clocks.find(x) != written_clocks.end());
    REQUIRE(written_intvars.empty());
  }

  SECTION("single clock to clock assignment")
  {
    std::string const stmt_str{"y = x"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.size() == 1);
    REQUIRE(read_clocks.find(x) != read_clocks.end());
    REQUIRE(read_intvars.empty());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.size() == 1);
    REQUIRE(written_clocks.find(y) != written_clocks.end());
    REQUIRE(written_intvars.empty());
  }

  SECTION("single clock sum to clock assignment")
  {
    std::string const stmt_str{"x = j * k + y"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.size() == 1);
    REQUIRE(read_clocks.find(y) != read_clocks.end());
    REQUIRE(read_intvars.size() == 2);
    REQUIRE(read_intvars.find(j) != read_intvars.end());
    REQUIRE(read_intvars.find(k) != read_intvars.end());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.size() == 1);
    REQUIRE(written_clocks.find(x) != written_clocks.end());
    REQUIRE(written_intvars.empty());
  }

  SECTION("single constant to integer variable assignment")
  {
    std::string const stmt_str{"j = 0"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.empty());
    REQUIRE(read_intvars.empty());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.empty());
    REQUIRE(written_intvars.size() == 1);
    REQUIRE(written_intvars.find(j) != written_intvars.end());
  }

  SECTION("single integer expression to integer variable assignment")
  {
    std::string const stmt_str{"i = 3 * j - k * i"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.empty());
    REQUIRE(read_intvars.size() == 3);
    REQUIRE(read_intvars.find(i) != read_intvars.end());
    REQUIRE(read_intvars.find(j) != read_intvars.end());
    REQUIRE(read_intvars.find(k) != read_intvars.end());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.empty());
    REQUIRE(written_intvars.size() == 1);
    REQUIRE(written_intvars.find(i) != written_intvars.end());
  }

  SECTION("multiple assignments to clock variables")
  {
    std::string const stmt_str{"x = 1; y = i + 3 * j; z = 1 + z"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.size() == 1);
    REQUIRE(read_clocks.find(z) != read_clocks.end());
    REQUIRE(read_intvars.size() == 2);
    REQUIRE(read_intvars.find(i) != read_intvars.end());
    REQUIRE(read_intvars.find(j) != read_intvars.end());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.size() == 3);
    REQUIRE(written_clocks.find(x) != written_clocks.end());
    REQUIRE(written_clocks.find(y) != written_clocks.end());
    REQUIRE(written_clocks.find(z) != written_clocks.end());
    REQUIRE(written_intvars.empty());
  }

  SECTION("multiple assignments to bounded integer variables")
  {
    std::string const stmt_str{"i = i + 1; j = 7; k = j - 3 + i"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.empty());
    REQUIRE(read_intvars.size() == 2);
    REQUIRE(read_intvars.find(i) != read_intvars.end());
    REQUIRE(read_intvars.find(j) != read_intvars.end());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.empty());
    REQUIRE(written_intvars.size() == 3);
    REQUIRE(written_intvars.find(i) != written_intvars.end());
    REQUIRE(written_intvars.find(j) != written_intvars.end());
    REQUIRE(written_intvars.find(k) != written_intvars.end());
  }

  SECTION("multiple mixed assignments")
  {
    std::string const stmt_str{"i = i + 1; x = i; j = 7; y = 3; z = 6 + y; k = k + j"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.size() == 1);
    REQUIRE(read_clocks.find(y) != read_clocks.end());
    REQUIRE(read_intvars.size() == 3);
    REQUIRE(read_intvars.find(i) != read_intvars.end());
    REQUIRE(read_intvars.find(j) != read_intvars.end());
    REQUIRE(read_intvars.find(k) != read_intvars.end());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.size() == 3);
    REQUIRE(written_clocks.find(x) != written_clocks.end());
    REQUIRE(written_clocks.find(y) != written_clocks.end());
    REQUIRE(written_clocks.find(z) != written_clocks.end());
    REQUIRE(written_intvars.size() == 3);
    REQUIRE(written_intvars.find(i) != written_intvars.end());
    REQUIRE(written_intvars.find(j) != written_intvars.end());
    REQUIRE(written_intvars.find(k) != written_intvars.end());
  }
}

TEST_CASE("statements with array variables", "[extract_variables]")
{
  tchecker::integer_variables_t intvars;
  intvars.declare("i", 1, 0, 5, 0);
  intvars.declare("j", 1, 0, 1, 1);
  intvars.declare("t", 5, -2, 2, 0);
  intvars.declare("u", 3, 0, 7, 1);

  tchecker::intvar_id_t i = intvars.id("i");
  tchecker::intvar_id_t j = intvars.id("j");
  tchecker::intvar_id_t t = intvars.id("t");
  tchecker::intvar_id_t u = intvars.id("u");

  tchecker::clock_variables_t clocks;
  clocks.declare("x", 1);
  clocks.declare("y", 3);
  clocks.declare("z", 5);

  tchecker::clock_id_t x = clocks.id("x");
  tchecker::clock_id_t y = clocks.id("y");
  tchecker::clock_id_t z = clocks.id("z");

  SECTION("single constant to clock array assignment, const index")
  {
    std::string const stmt_str{"y[0] = 1"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.empty());
    REQUIRE(read_intvars.empty());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.size() == 1);
    REQUIRE(written_clocks.find(y + 0) != written_clocks.end());
    REQUIRE(written_intvars.empty());
  }

  SECTION("single clock to clock array assignment, non const index")
  {
    std::string const stmt_str{"y[i+3*j] = 0"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.empty());
    REQUIRE(read_intvars.size() == 2);
    REQUIRE(read_intvars.find(i) != read_intvars.end());
    REQUIRE(read_intvars.find(j) != read_intvars.end());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.size() == clocks.info(y).size());
    for (std::size_t idx = 0; idx < clocks.info(y).size(); ++idx)
      REQUIRE(written_clocks.find(static_cast<tchecker::clock_id_t>(y + idx)) != written_clocks.end());
    REQUIRE(written_intvars.empty());
  }

  SECTION("single constant to clock array assignment")
  {
    std::string const stmt_str{"y[i] = z[3*j]"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.size() == clocks.info(z).size());
    for (tchecker::clock_id_t id = 0; id < clocks.info(z).size(); ++id)
      REQUIRE(read_clocks.find(z + id) != read_clocks.end());
    REQUIRE(read_intvars.size() == 2);
    REQUIRE(read_intvars.find(i) != read_intvars.end());
    REQUIRE(read_intvars.find(j) != read_intvars.end());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.size() == clocks.info(y).size());
    for (std::size_t idx = 0; idx < clocks.info(y).size(); ++idx)
      REQUIRE(written_clocks.find(static_cast<tchecker::clock_id_t>(y + idx)) != written_clocks.end());
    REQUIRE(written_intvars.empty());
  }

  SECTION("single constant to integer variable array assignment, const index")
  {
    std::string const stmt_str{"t[2] = 4"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.empty());
    REQUIRE(read_intvars.empty());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.empty());
    REQUIRE(written_intvars.size() == 1);
    REQUIRE(written_intvars.find(t + 2) != written_intvars.end());
  }

  SECTION("single constant to integer variable array assignment, non const index")
  {
    std::string const stmt_str{"t[7 * i - 4 * u[0]] = 4"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.empty());
    REQUIRE(read_intvars.size() == 2);
    REQUIRE(read_intvars.find(i) != read_intvars.end());
    REQUIRE(read_intvars.find(u + 0) != read_intvars.end());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.empty());
    REQUIRE(written_intvars.size() == intvars.info(t).size());
    for (tchecker::intvar_id_t id = 0; id < intvars.info(t).size(); ++id)
      REQUIRE(written_intvars.find(t + id) != written_intvars.end());
  }

  SECTION("multiple assignments with array variables")
  {
    std::string const stmt_str{"i = 1; j = j + 1; x = 0; y[2] = i + x; t[2] = j; u[3*j-t[i]] = t[j] - i; z[u[j]] = y[1]"};

    std::shared_ptr<tchecker::statement_t> stmt{tchecker::parsing::parse_statement("", stmt_str)};
    REQUIRE(stmt != nullptr);

    tchecker::integer_variables_t lvars;
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt{tchecker::typecheck(*stmt, lvars, intvars, clocks)};
    REQUIRE(typed_stmt != nullptr);

    std::unordered_set<tchecker::clock_id_t> read_clocks;
    std::unordered_set<tchecker::intvar_id_t> read_intvars;

    tchecker::extract_read_variables(*typed_stmt, read_clocks, read_intvars);
    REQUIRE(read_clocks.size() == 2);
    REQUIRE(read_clocks.find(x) != read_clocks.end());
    REQUIRE(read_clocks.find(y + 1) != read_clocks.end());

    REQUIRE(read_intvars.size() == 2 + intvars.info(t).size() + intvars.info(u).size()); // i, j, t[], u[]
    REQUIRE(read_intvars.find(i) != read_intvars.end());
    REQUIRE(read_intvars.find(j) != read_intvars.end());
    for (tchecker::intvar_id_t id = 0; id < intvars.info(t).size(); ++id)
      REQUIRE(read_intvars.find(t + id) != read_intvars.end());
    for (tchecker::intvar_id_t id = 0; id < intvars.info(u).size(); ++id)
      REQUIRE(read_intvars.find(u + id) != read_intvars.end());

    std::unordered_set<tchecker::clock_id_t> written_clocks;
    std::unordered_set<tchecker::intvar_id_t> written_intvars;

    tchecker::extract_written_variables(*typed_stmt, written_clocks, written_intvars);
    REQUIRE(written_clocks.size() == 2 + clocks.info(z).size()); // x, y[2], z[]
    REQUIRE(written_clocks.find(x) != written_clocks.end());
    REQUIRE(written_clocks.find(y + 2) != written_clocks.end());
    for (tchecker::clock_id_t id = 0; id < clocks.info(z).size(); ++id)
      REQUIRE(written_clocks.find(z + id) != written_clocks.end());

    REQUIRE(written_intvars.size() == 3 + intvars.info(u).size()); // i, j, t[2], u[]
    REQUIRE(written_intvars.find(i) != written_intvars.end());
    REQUIRE(written_intvars.find(j) != written_intvars.end());
    REQUIRE(written_intvars.find(t + 2) != written_intvars.end());
    for (tchecker::intvar_id_t id = 0; id < intvars.info(u).size(); ++id)
      REQUIRE(written_intvars.find(u + id) != written_intvars.end());
  }
}
