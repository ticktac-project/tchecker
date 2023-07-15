/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <memory>

#include "tchecker/parsing/declaration.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/system/system.hh"
#include "tchecker/variables/intvars.hh"

#include "utils.hh"

TEST_CASE("vloc from string", "[from_string]")
{
  std::string model = "system:from_string \n\
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

  std::unique_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};

  REQUIRE(sysdecl.get() != nullptr);

  tchecker::system::system_t system{*sysdecl};
  tchecker::process_id_t const processes_count = static_cast<tchecker::process_id_t>(system.processes_count());
  tchecker::vloc_t * vloc = tchecker::vloc_allocate_and_construct(processes_count, processes_count);

  SECTION("Initialize vloc from a valid string")
  {
    REQUIRE_NOTHROW(tchecker::from_string(*vloc, system, "<l0,l1,l0>"));

    tchecker::loc_id_t loc_ids[3] = {0, 3, 4};
    for (tchecker::process_id_t pid = 0; pid < processes_count; ++pid)
      REQUIRE((*vloc)[pid] == loc_ids[pid]);
  }

  SECTION("Initialize vloc from an invalid string (missing >)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*vloc, system, "<l0,l1,l0"), std::invalid_argument);
  }

  SECTION("Initialize vloc from an invalid string (missing <)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*vloc, system, "l0,l1,l0>"), std::invalid_argument);
  }

  SECTION("Initialize vloc from an invalid string (too many >)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*vloc, system, "<l0,l1,l0>>"), std::invalid_argument);
  }

  SECTION("Initialize vloc from a string with unknown location name")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*vloc, system, "<l4,l1,l0>"), std::invalid_argument);
  }

  SECTION("Initialize vloc from a string with too many location names")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*vloc, system, "<l1,l1,l0,l7>"), std::invalid_argument);
  }

  SECTION("Initialize vloc from a string too few location names")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*vloc, system, "<l0,l0>"), std::invalid_argument);
  }

  tchecker::vloc_destruct_and_deallocate(vloc);
}

TEST_CASE("intval from string", "[from_string]")
{
  std::string model = "system:from_string \n\
  event:a1 \n\
  event:a2 \n\
  event:a3 \n\
  \n\
  int:1:0:2:1:i1 \n\
  int:3:-2:5:0:i3 \n\
  \n\
  process:P1 \n\
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
  edge:P3:l0:l1:a3{provided: i3[1]<3} \n\
  \n\
  sync:P1@a1:P2@a2\n";

  std::unique_ptr<tchecker::parsing::system_declaration_t const> sysdecl{tchecker::test::parse(model)};

  REQUIRE(sysdecl.get() != nullptr);

  tchecker::system::system_t system{*sysdecl};
  unsigned short const flatvars_count = static_cast<unsigned short>(system.integer_variables().flattened().size());
  tchecker::intval_t * intval = tchecker::intval_allocate_and_construct(flatvars_count, flatvars_count);

  SECTION("Initialize intval from a valid string")
  {
    REQUIRE_NOTHROW(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=1,i3[0]=-1,i3[1]=0,i3[2]=4"));

    REQUIRE((*intval)[0] == 1);
    REQUIRE((*intval)[1] == -1);
    REQUIRE((*intval)[2] == 0);
    REQUIRE((*intval)[3] == 4);
  }

  SECTION("Initialize intval from a valid string (unordered variables)")
  {
    REQUIRE_NOTHROW(tchecker::from_string(*intval, system.integer_variables().flattened(), "i3[1]=2,i3[0]=-1,i1=0,i3[2]=5"));

    REQUIRE((*intval)[0] == 0);
    REQUIRE((*intval)[1] == -1);
    REQUIRE((*intval)[2] == 2);
    REQUIRE((*intval)[3] == 5);
  }

  SECTION("Initialize intval from an invalid string (syntax error: missing lhs)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=2,=3,i3[0]=1,i3[1]=2"),
                      std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (syntax error: missing rhs)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=2,i3[2],i3[0]=1,i3[1]=2"),
                      std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (missing variable)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=2,i3[0]=3"),
                      std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (array variable not indexed)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=2,i3=3,i3[0]=1,i3[1]=3"),
                      std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (unknown variable)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i3[0]=1,i2=5,i1=0,i3[1]=2"),
                      std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (unknown variable: index out of array)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i3[0]=1,i1=1,i3[5]=0,i3[1]=2"),
                      std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (multiple assignments to one variable)")
  {
    REQUIRE_THROWS_AS(
        tchecker::from_string(*intval, system.integer_variables().flattened(), "i3[0]=1,i3[1]=0,i3[2]=-2,i1=1,i3[2]=0"),
        std::invalid_argument);
  }

  SECTION("Initialize intval from an invalid string (value out of variable range)")
  {
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=1,i3[0]=0,i3[1]=1,i3[2]=6"),
                      std::invalid_argument);
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=-11,i3[0]=0,i3[1]=1,i3[2]=0"),
                      std::invalid_argument);
    REQUIRE_THROWS_AS(tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=1,i3[0]=-5,i3[1]=1,i3[2]=6"),
                      std::invalid_argument);
    REQUIRE_THROWS_AS(
        tchecker::from_string(*intval, system.integer_variables().flattened(), "i1=19901,i3[0]=-2,i3[1]=1,i3[2]=4"),
        std::invalid_argument);
  }

  tchecker::intval_destruct_and_deallocate(intval);
}