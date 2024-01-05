/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <memory>
#include <string>

#include "tchecker/expression/static_analysis.hh"
#include "tchecker/parsing/parsing.hh"
#include "tchecker/statement/clock_updates.hh"
#include "tchecker/statement/static_analysis.hh"
#include "tchecker/statement/typechecking.hh"
#include "tchecker/statement/typed_statement.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"
#include "utils.hh"

/*!
 \brief Check if clock update x+c belongs to updates list l, count times
 \return true if x+c appears count time in l, false otherwise
 */
bool contains(tchecker::clock_updates_list_t const & l, tchecker::clock_id_t x, tchecker::integer_t c, int count = 1)
{
  for (tchecker::clock_update_t const & upd : l) {
    tchecker::integer_t value = tchecker::const_evaluate(upd.value());
    if (upd.clock_id() == x && value == c)
      --count;
  }
  return (count == 0);
}

/*!
 \brief Size of a clock updates list
 \return the size of clock updates list l
 */
std::size_t size(tchecker::clock_updates_list_t const & l) { return std::distance(l.begin(), l.end()); }

TEST_CASE("simple clocks", "[clock updates]")
{
  tchecker::integer_variables_t localvars;
  localvars.declare("l", 1, 0, 10, 0);

  tchecker::integer_variables_t intvars;
  intvars.declare("i", 1, -2, 5, -1);
  intvars.declare("j", 1, 3, 7, 4);

  tchecker::clock_variables_t clocks;
  clocks.declare("x", 1);
  clocks.declare("y", 1);
  clocks.declare("z", 1);
  clocks.declare("w", 1);

  tchecker::clock_id_t const x = clocks.id("x");
  tchecker::clock_id_t const y = clocks.id("y");
  tchecker::clock_id_t const z = clocks.id("z");
  tchecker::clock_id_t const w = clocks.id("w");

  SECTION("default clock updates map is the identity")
  {
    tchecker::clock_updates_map_t m(clocks.size(tchecker::VK_FLATTENED));

    REQUIRE(size(m[x]) == 1);
    REQUIRE(contains(m[x], x, 0));
    REQUIRE(size(m[y]) == 1);
    REQUIRE(contains(m[y], y, 0));
    REQUIRE(size(m[z]) == 1);
    REQUIRE(contains(m[z], z, 0));
    REQUIRE(size(m[w]) == 1);
    REQUIRE(contains(m[w], w, 0));
  }

  SECTION("statement that does not reset a clock yields identity clock updates map")
  {
    std::string const context_str = "";
    std::string const stmt_str = "i=2";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 1);
    REQUIRE(contains(m[x], x, 0));
    REQUIRE(size(m[y]) == 1);
    REQUIRE(contains(m[y], y, 0));
    REQUIRE(size(m[z]) == 1);
    REQUIRE(contains(m[z], z, 0));
    REQUIRE(size(m[w]) == 1);
    REQUIRE(contains(m[w], w, 0));
  }

  SECTION("reset to constants")
  {
    std::string const context_str = "";
    std::string const stmt_str = "x=2; z=7";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 1);
    REQUIRE(contains(m[x], tchecker::REFCLOCK_ID, 2));
    REQUIRE(size(m[y]) == 1);
    REQUIRE(contains(m[y], y, 0));
    REQUIRE(size(m[z]) == 1);
    REQUIRE(contains(m[z], tchecker::REFCLOCK_ID, 7));
    REQUIRE(size(m[w]) == 1);
    REQUIRE(contains(m[w], w, 0));
  }

  SECTION("reset to clock")
  {
    std::string const context_str = "";
    std::string const stmt_str = "z=2; y=z; w=x";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 1);
    REQUIRE(contains(m[x], x, 0));
    REQUIRE(size(m[y]) == 1);
    REQUIRE(contains(m[y], tchecker::REFCLOCK_ID, 2));
    REQUIRE(size(m[z]) == 1);
    REQUIRE(contains(m[z], tchecker::REFCLOCK_ID, 2));
    REQUIRE(size(m[w]) == 1);
    REQUIRE(contains(m[w], x, 0));
  }

  SECTION("reset to sum")
  {
    std::string const context_str = "";
    std::string const stmt_str = "z=2; y=1+w; x=5+y; w=y";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 1);
    REQUIRE(contains(m[x], w, 6));
    REQUIRE(size(m[y]) == 1);
    REQUIRE(contains(m[y], w, 1));
    REQUIRE(size(m[z]) == 1);
    REQUIRE(contains(m[z], tchecker::REFCLOCK_ID, 2));
    REQUIRE(size(m[w]) == 1);
    REQUIRE(contains(m[w], w, 1));
  }

  SECTION("if statement")
  {
    std::string const context_str = "";
    std::string const stmt_str = "\
        z=1; \n\
        if (i==1) then \n\
            x=1+y \n\
        else \n\
            x=2+z \n\
        end; \n\
        y=0";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 2);
    REQUIRE(contains(m[x], y, 1));
    REQUIRE(contains(m[x], tchecker::REFCLOCK_ID, 3));
    REQUIRE(size(m[y]) == 1);
    REQUIRE(contains(m[y], tchecker::REFCLOCK_ID, 0));
    REQUIRE(size(m[z]) == 2);
    REQUIRE(contains(m[z], tchecker::REFCLOCK_ID, 1, 2));
    REQUIRE(size(m[w]) == 2);
    REQUIRE(contains(m[w], w, 0, 2));
  }

  SECTION("nested if statements")
  {
    std::string const context_str = "";
    std::string const stmt_str = "\
        z=1; \n\
        if (i==1) then \n\
            x=1+y \n\
        else \n\
            x=1+x; \n\
            if (j==2) then \n\
                x=2+z; \n\
                y=3 \n\
            else \n\
                w=1+x \n\
            end \n\
        end; \n\
        z=0";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 3);
    REQUIRE(contains(m[x], y, 1));
    REQUIRE(contains(m[x], tchecker::REFCLOCK_ID, 3));
    REQUIRE(contains(m[x], x, 1));
    REQUIRE(size(m[y]) == 3);
    REQUIRE(contains(m[y], y, 0, 2));
    REQUIRE(contains(m[y], tchecker::REFCLOCK_ID, 3));
    REQUIRE(size(m[z]) == 1);
    REQUIRE(contains(m[z], tchecker::REFCLOCK_ID, 0));
    REQUIRE(size(m[w]) == 3);
    REQUIRE(contains(m[w], w, 0, 2));
    REQUIRE(contains(m[w], x, 2));
  }

  SECTION("sequential if statements")
  {
    std::string const context_str = "";
    std::string const stmt_str = "\
        z=1; \n\
        if (i==1) then \n\
            w=2+y \n\
        else \n\
            y=1+z \n\
        end; \n\
        i=j+7; \n\
        if (j==2) then \n\
            x=2+w; \n\
            z=0 \n\
        else \n\
            y=z; \n\
            z=4 \n\
        end";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 4);
    REQUIRE(contains(m[x], x, 0, 2));
    REQUIRE(contains(m[x], y, 4));
    REQUIRE(contains(m[x], w, 2));
    REQUIRE(size(m[y]) == 4);
    REQUIRE(contains(m[y], y, 0));
    REQUIRE(contains(m[y], tchecker::REFCLOCK_ID, 2));
    REQUIRE(contains(m[y], tchecker::REFCLOCK_ID, 1, 2));
    REQUIRE(size(m[z]) == 2);
    REQUIRE(contains(m[z], tchecker::REFCLOCK_ID, 0));
    REQUIRE(contains(m[z], tchecker::REFCLOCK_ID, 4));
    REQUIRE(size(m[w]) == 4);
    REQUIRE(contains(m[w], y, 2, 2));
    REQUIRE(contains(m[w], w, 0, 2));
  }

  SECTION("while statement")
  {
    std::string const context_str = "";
    std::string const stmt_str = "\
        x=0; \n\
        while (i > 0) do \n\
            y=1; \n\
            z=1+x; \n\
            i=i-1 \n\
        end";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 1);
    REQUIRE(contains(m[x], tchecker::REFCLOCK_ID, 0));
    REQUIRE(size(m[y]) == 2);
    REQUIRE(contains(m[y], y, 0));
    REQUIRE(contains(m[y], tchecker::REFCLOCK_ID, 1));
    REQUIRE(size(m[z]) == 0);
    REQUIRE(size(m[w]) == 1);
    REQUIRE(contains(m[w], w, 0));
  }

  SECTION("nested while statements")
  {
    std::string const context_str = "";
    std::string const stmt_str = "\
        w=0; \n\
        while (i > 0) do \n\
            y=1; \n\
            while (j <= 5) do \n\
                w=3; \n\
                j=j+1 \n\
            end; \n\
            z=1+w; \n\
            i=i-1 \n\
        end";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 1);
    REQUIRE(contains(m[x], x, 0));
    REQUIRE(size(m[y]) == 2);
    REQUIRE(contains(m[y], y, 0));
    REQUIRE(contains(m[y], tchecker::REFCLOCK_ID, 1));
    REQUIRE(size(m[z]) == 0);
    REQUIRE(size(m[w]) == 2);
    REQUIRE(contains(m[w], tchecker::REFCLOCK_ID, 0));
    REQUIRE(contains(m[w], tchecker::REFCLOCK_ID, 3));
  }

  SECTION("Sequential while statements")
  {
    std::string const context_str = "";
    std::string const stmt_str = "\
        w=0; \n\
        while (i > 0) do \n\
            y=1; \n\
            z=1+w; \n\
            i=i-1 \n\
        end; \n\
        j=0; \n\
        x=3+y; \n\
        z=2; \n\
        i=6*i+7; \n\
        while (j <= 5) do \n\
            w=3; \n\
            y=1+y; \n\
            j=j+1 \n\
        end";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 2);
    REQUIRE(contains(m[x], y, 3));
    REQUIRE(contains(m[x], tchecker::REFCLOCK_ID, 4));
    REQUIRE(size(m[y]) == 0);
    REQUIRE(size(m[z]) == 1);
    REQUIRE(contains(m[z], tchecker::REFCLOCK_ID, 2));
    REQUIRE(size(m[w]) == 2);
    REQUIRE(contains(m[w], tchecker::REFCLOCK_ID, 0));
    REQUIRE(contains(m[w], tchecker::REFCLOCK_ID, 3));
  }

  SECTION("All statements")
  {
    std::string const context_str = "";
    std::string const stmt_str = "\
        w=0; \n\
        if (i > 0) then \n\
            x=1+x; \n\
            i=0; \n\
        else \n\
            y=1+w; \n\
            local l; \n\
            l=i; \n\
            while (l < 10) do \n\
                z=0; \n\
                w=z \n\
            end \n\
        end";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 2);
    REQUIRE(contains(m[x], x, 1));
    REQUIRE(contains(m[x], x, 0));
    REQUIRE(size(m[y]) == 2);
    REQUIRE(contains(m[y], y, 0));
    REQUIRE(contains(m[y], tchecker::REFCLOCK_ID, 1));
    REQUIRE(size(m[z]) == 3);
    REQUIRE(contains(m[z], z, 0, 2));
    REQUIRE(contains(m[z], tchecker::REFCLOCK_ID, 0));
    REQUIRE(size(m[w]) == 0);
  }
}

TEST_CASE("clock arrays", "[clock updates]")
{
  tchecker::integer_variables_t localvars;
  localvars.declare("l", 1, 0, 10, 0);

  tchecker::integer_variables_t intvars;
  intvars.declare("i", 1, -2, 5, -1);
  intvars.declare("j", 1, 3, 7, 4);

  tchecker::clock_variables_t clocks;
  clocks.declare("x", 1);
  clocks.declare("y", 3);

  tchecker::clock_id_t const x = clocks.id("x");
  tchecker::clock_id_t const y0 = clocks.id("y") + 0;
  tchecker::clock_id_t const y1 = clocks.id("y") + 1;
  tchecker::clock_id_t const y2 = clocks.id("y") + 2;

  SECTION("default clock updates map is the identity")
  {
    tchecker::clock_updates_map_t m(clocks.size(tchecker::VK_FLATTENED));

    REQUIRE(size(m[x]) == 1);
    REQUIRE(contains(m[x], x, 0));
    REQUIRE(size(m[y0]) == 1);
    REQUIRE(contains(m[y0], y0, 0));
    REQUIRE(size(m[y1]) == 1);
    REQUIRE(contains(m[y1], y1, 0));
    REQUIRE(size(m[y2]) == 1);
    REQUIRE(contains(m[y2], y2, 0));
  }

  SECTION("assignments with constant indices")
  {
    std::string const context_str = "";
    std::string const stmt_str = "x=2; y[0]=7; y[2]=1+x";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 1);
    REQUIRE(contains(m[x], tchecker::REFCLOCK_ID, 2));
    REQUIRE(size(m[y0]) == 1);
    REQUIRE(contains(m[y0], tchecker::REFCLOCK_ID, 7));
    REQUIRE(size(m[y1]) == 1);
    REQUIRE(contains(m[y1], y1, 0));
    REQUIRE(size(m[y2]) == 1);
    REQUIRE(contains(m[y2], tchecker::REFCLOCK_ID, 3));
  }

  SECTION("assignments with non-constant indices")
  {
    std::string const context_str = "";
    std::string const stmt_str = "x=y[j]; y[i]=1; y[2]=1+y[1]";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 3);
    REQUIRE(contains(m[x], y0, 0));
    REQUIRE(contains(m[x], y1, 0));
    REQUIRE(contains(m[x], y2, 0));
    REQUIRE(size(m[y0]) == 2);
    REQUIRE(contains(m[y0], y0, 0));
    REQUIRE(contains(m[y0], tchecker::REFCLOCK_ID, 1));
    REQUIRE(size(m[y1]) == 2);
    REQUIRE(contains(m[y1], y1, 0));
    REQUIRE(contains(m[y1], tchecker::REFCLOCK_ID, 1));
    REQUIRE(size(m[y2]) == 2);
    REQUIRE(contains(m[y2], y1, 1));
    REQUIRE(contains(m[y2], tchecker::REFCLOCK_ID, 2));
  }

  SECTION("mixing everything")
  {
    std::string const context_str = "";
    std::string const stmt_str = "\
        y[i+j]=x; \n\
        while (i >= 0) do \n\
            local l; \n\
            l=7; \n\
            if (j == i + l) then \n\
                x=y[l]; \n\
                y[1]=0 \n\
            else \n\
                y[2]=3 \n\
            end \n\
        end; \n\
        y[2]=0 \n\
    ";

    std::shared_ptr<tchecker::statement_t> stmt = tchecker::parsing::parse_statement(context_str, stmt_str);
    std::shared_ptr<tchecker::typed_statement_t> typed_stmt = tchecker::typecheck(*stmt, localvars, intvars, clocks);

    tchecker::clock_updates_map_t m = tchecker::compute_clock_updates(clocks.size(tchecker::VK_FLATTENED), *typed_stmt);

    REQUIRE(size(m[x]) == 0);
    REQUIRE(size(m[y0]) == 4);
    REQUIRE(contains(m[y0], y0, 0, 2));
    REQUIRE(contains(m[y0], x, 0, 2));
    REQUIRE(size(m[y1]) == 5);
    REQUIRE(contains(m[y1], x, 0, 2));
    REQUIRE(contains(m[y1], y1, 0, 2));
    REQUIRE(contains(m[y1], tchecker::REFCLOCK_ID, 0));
    REQUIRE(size(m[y2]) == 1);
    REQUIRE(contains(m[y2], tchecker::REFCLOCK_ID, 0));
  }
}