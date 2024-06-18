/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <memory>
#include <string>

#include "tchecker/parsing/declaration.hh"
#include "tchecker/ta/system.hh"
#include "tchecker/utils/log.hh"

#include "utils.hh"

TEST_CASE("no throw if no weakly synchronized events", "[guard_weak_sync]")
{
  std::string model = "system:no_weakly_sync \n\
  event:a1 \n\
  event:a2 \n\
  event:a3 \n\
  \n\
  process:P1 \n\
  int:1:1:1:1:i1 \n\
  location:P1:l0{initial:} \n\
  location:P1:l1 \n\
  edge:P1:l0:l1:a1{provided: i1>0} \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial:} \n\
  location:P2:l1 \n\
  edge:P2:l0:l1:a2 \n\
  \n\
  process:P3 \n\
  int:1:1:1:1:i3 \n\
  location:P3:l0{initial:} \n\
  location:P3:l1 \n\
  edge:P3:l0:l1:a3{provided: i3<3} \n\
  \n\
  sync:P1@a1:P2@a2\n";

  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{tchecker::test::parse(model)};

  REQUIRE(sysdecl != nullptr);
  REQUIRE_NOTHROW(tchecker::ta::system_t(*sysdecl));
}

TEST_CASE("no throw if weakly synchronized events have no guard", "[guard_weak_sync]")
{
  std::string model = "system:weakly_sync_no_guard \n\
  event:a1 \n\
  event:a2 \n\
  event:a3 \n\
  \n\
  process:P1 \n\
  int:1:1:1:1:i1 \n\
  location:P1:l0{initial:} \n\
  location:P1:l1 \n\
  edge:P1:l0:l1:a1{provided: i1>0} \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial:} \n\
  location:P2:l1 \n\
  edge:P2:l0:l1:a2 \n\
  \n\
  process:P3 \n\
  location:P3:l0{initial:} \n\
  location:P3:l1 \n\
  edge:P3:l0:l1:a3 \n\
  \n\
  sync:P1@a1:P2@a2?:P3@a3?\n";

  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{tchecker::test::parse(model)};

  REQUIRE(sysdecl != nullptr);
  REQUIRE_NOTHROW(tchecker::ta::system_t(*sysdecl));
}

TEST_CASE("throw if first weakly synchronized event has a guard", "[guard_weak_sync]")
{
  std::string model = "system:first_weakly_sync_with_guard \n\
  event:a1 \n\
  event:a2 \n\
  event:a3 \n\
  \n\
  process:P1 \n\
  int:1:1:1:1:i1 \n\
  location:P1:l0{initial:} \n\
  location:P1:l1 \n\
  edge:P1:l0:l1:a1{provided: i1>0} \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial:} \n\
  location:P2:l1 \n\
  edge:P2:l0:l1:a2 \n\
  \n\
  process:P3 \n\
  location:P3:l0{initial:} \n\
  location:P3:l1 \n\
  edge:P3:l0:l1:a3 \n\
  \n\
  sync:P1@a1?:P2@a2:P3@a3\n";

  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{tchecker::test::parse(model)};

  REQUIRE(sysdecl != nullptr);
  REQUIRE_THROWS_AS(tchecker::ta::system_t(*sysdecl), std::invalid_argument);
}

TEST_CASE("throw if last weakly synchronized event has a guard", "[guard_weak_sync]")
{
  std::string model = "system:last_weakly_sync_with_guard \n\
  event:a1 \n\
  event:a2 \n\
  event:a3 \n\
  \n\
  process:P1 \n\
  location:P1:l0{initial:} \n\
  location:P1:l1 \n\
  edge:P1:l0:l1:a1 \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial:} \n\
  location:P2:l1 \n\
  edge:P2:l0:l1:a2 \n\
  \n\
  process:P3 \n\
  int:1:1:1:1:i3 \n\
  location:P3:l0{initial:} \n\
  location:P3:l1 \n\
  edge:P3:l0:l1:a3{provided: i3 < 3} \n\
  \n\
  sync:P1@a1:P2@a2:P3@a3?\n";

  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{tchecker::test::parse(model)};

  REQUIRE(sysdecl != nullptr);
  REQUIRE_THROWS_AS(tchecker::ta::system_t(*sysdecl), std::invalid_argument);
}

TEST_CASE("throw if middle weakly synchronized event has a guard", "[guard_weak_sync]")
{
  std::string model = "system:middle_weakly_sync_with_guard \n\
  event:a1 \n\
  event:a2 \n\
  event:a3 \n\
  \n\
  process:P1 \n\
  location:P1:l0{initial:} \n\
  location:P1:l1 \n\
  edge:P1:l0:l1:a1 \n\
  \n\
  process:P2 \n\
  int:1:1:1:1:i2 \n\
  location:P2:l0{initial:} \n\
  location:P2:l1 \n\
  edge:P2:l0:l1:a2{provided: i2==4} \n\
  \n\
  process:P3 \n\
  location:P3:l0{initial:} \n\
  location:P3:l1 \n\
  edge:P3:l0:l1:a3 \n\
  \n\
  sync:P1@a1:P2@a2?:P3@a3\n";

  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{tchecker::test::parse(model)};

  REQUIRE(sysdecl != nullptr);
  REQUIRE_THROWS_AS(tchecker::ta::system_t(*sysdecl), std::invalid_argument);
}

TEST_CASE("throw if some weakly synchronized event has a guard, several transitions", "[guard_weak_sync]")
{
  std::string model = "system:middle_weakly_sync_with_guard \n\
  event:a1 \n\
  event:a2 \n\
  event:a3 \n\
  \n\
  process:P1 \n\
  int:1:1:1:1:i1 \n\
  location:P1:l0{initial:} \n\
  location:P1:l1 \n\
  location:P1:l2 \n\
  edge:P1:l0:l1:a1 \n\
  edge:P1:l1:l2:a1{provided: i1 == 5} \n\
  \n\
  process:P2 \n\
  int:1:1:1:1:i2 \n\
  location:P2:l0{initial:} \n\
  location:P2:l1 \n\
  edge:P2:l0:l1:a2{provided: i2==4} \n\
  \n\
  process:P3 \n\
  location:P3:l0{initial:} \n\
  location:P3:l1 \n\
  location:P3:l2 \n\
  edge:P3:l0:l1:a3 \n\
  edge:P3:l0:l2:a3 \n\
  \n\
  sync:P1@a1?:P2@a2:P3@a3?\n";

  std::shared_ptr<tchecker::parsing::system_declaration_t> sysdecl{tchecker::test::parse(model)};

  REQUIRE(sysdecl != nullptr);
  REQUIRE_THROWS_AS(tchecker::ta::system_t(*sysdecl), std::invalid_argument);
}
