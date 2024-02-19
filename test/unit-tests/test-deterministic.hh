/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <memory>

#include "tchecker/basictypes.hh"
#include "tchecker/parsing/declaration.hh"
#include "tchecker/system/static_analysis.hh"
#include "tchecker/system/system.hh"

#include "testutils/utils.hh"

TEST_CASE("empty system", "[deterministic]")
{
  std::string model = "system:empty \n\
  ";

  std::shared_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  assert(sysdecl != nullptr);

  tchecker::system::system_t system{*sysdecl};

  REQUIRE(tchecker::system::is_deterministic(system));
}

TEST_CASE("deterministic system", "[deterministic]")
{
  std::string model = "system:deterministic \n\
  \n\
  event:a \n\
  event:b \n\
  event:c \n\
  \n\
  process:P1 \n\
  location:P1:l0{initial: true} \n\
  location:P1:l1 \n\
  edge:P1:l0:l0:a \n\
  edge:P1:l0:l1:b \n\
  edge:P1:l1:l1:b \n\
  edge:P1:l1:l0:a \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial: true} \n\
  edge:P2:l0:l0:a \n\
  edge:P2:l0:l0:b \n\
  edge:P2:l0:l0:c \n\
  \n\
  process:P3 \n\
  location:P3:l0{initial: true} \n\
  location:P3:l1 \n\
  location:P3:l2 \n\
  edge:P3:l0:l1:a \n\
  edge:P3:l1:l2:b \n\
  edge:P3:l2:l0:c \n\
  ";

  std::shared_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  assert(sysdecl != nullptr);

  tchecker::system::system_t system{*sysdecl};

  REQUIRE(tchecker::system::is_deterministic(system));
}

TEST_CASE("system with a single, non-deterministic, process", "[dterministic]")
{
  std::string model = "system:non_deterministic_one \n\
  \n\
  event:a \n\
  event:b \n\
  \n\
  process:P \n\
  location:P:l0{initial: true} \n\
  location:P:l1 \n\
  edge:P:l0:l1:a \n\
  edge:P:l1:l1:b \n\
  edge:P:l1:l0:b \n\
  ";

  std::shared_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  assert(sysdecl != nullptr);

  tchecker::system::system_t system{*sysdecl};

  REQUIRE(!tchecker::system::is_deterministic(system));
}

TEST_CASE("system with a non-deterministic process", "[dterministic]")
{
  std::string model = "system:non_deterministic_two \n\
  \n\
  event:a \n\
  event:b \n\
  event:c \n\
  event:d \n\
  \n\
  process:P1 \n\
  location:P1:l0{initial: true} \n\
  location:P1:l1 \n\
  edge:P1:l0:l0:a \n\
  edge:P1:l0:l1:c \n\
  edge:P1:l1:l1:d \n\
  edge:P1:l1:l0:b \n\
  \n\
  process:P2 \n\
  location:P2:l0{initial: true} \n\
  location:P2:l1 \n\
  edge:P2:l0:l1:a \n\
  edge:P2:l1:l1:b \n\
  edge:P2:l1:l0:b \n\
  ";

  std::shared_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};
  assert(sysdecl != nullptr);

  tchecker::system::system_t system{*sysdecl};

  REQUIRE(!tchecker::system::is_deterministic(system));
}
