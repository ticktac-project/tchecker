/*
* This file is a part of the TChecker project.
*
* See files AUTHORS and LICENSE for copyright details.
*
*/

#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/flat_system/vloc.hh"
#include "tchecker/utils/ordering.hh"

int intcmp(int i, int j) { return i - j; }

TEST_CASE( "lexical ordering on ranges" , "[ordering]" ) {
  
  std::vector<int> v_empty, v1{1,2,3}, v2{1,2,5}, v3{1,2,3,4}, v4{5, 4, 3, 2, 1};
  
  auto rempty = tchecker::make_range(v_empty);
  auto r1 = tchecker::make_range(v1);
  auto r2 = tchecker::make_range(v2);
  auto r3 = tchecker::make_range(v3);
  auto r4 = tchecker::make_range(v4);

  REQUIRE( tchecker::lexical_cmp(rempty, rempty, intcmp) == 0);
  REQUIRE( tchecker::lexical_cmp(rempty, r1, intcmp) < 0);
  REQUIRE( tchecker::lexical_cmp(rempty, r2, intcmp) < 0);
  REQUIRE( tchecker::lexical_cmp(rempty, r3, intcmp) < 0);
  REQUIRE( tchecker::lexical_cmp(rempty, r4, intcmp) < 0);
  
  REQUIRE( tchecker::lexical_cmp(r1, rempty, intcmp) > 0);
  REQUIRE( tchecker::lexical_cmp(r1, r1, intcmp) == 0);
  REQUIRE( tchecker::lexical_cmp(r1, r2, intcmp) < 0);
  REQUIRE( tchecker::lexical_cmp(r1, r3, intcmp) < 0);
  REQUIRE( tchecker::lexical_cmp(r1, r4, intcmp) < 0);
  
  REQUIRE( tchecker::lexical_cmp(r2, rempty, intcmp) > 0);
  REQUIRE( tchecker::lexical_cmp(r2, r1, intcmp) > 0);
  REQUIRE( tchecker::lexical_cmp(r2, r2, intcmp) == 0);
  REQUIRE( tchecker::lexical_cmp(r2, r3, intcmp) > 0);
  REQUIRE( tchecker::lexical_cmp(r2, r4, intcmp) < 0);
  
  REQUIRE( tchecker::lexical_cmp(r3, rempty, intcmp) > 0);
  REQUIRE( tchecker::lexical_cmp(r3, r1, intcmp) > 0);
  REQUIRE( tchecker::lexical_cmp(r3, r2, intcmp) < 0);
  REQUIRE( tchecker::lexical_cmp(r3, r3, intcmp) == 0);
  REQUIRE( tchecker::lexical_cmp(r3, r4, intcmp) < 0);
  
  REQUIRE( tchecker::lexical_cmp(r4, rempty, intcmp) > 0);
  REQUIRE( tchecker::lexical_cmp(r4, r1, intcmp) > 0);
  REQUIRE( tchecker::lexical_cmp(r4, r2, intcmp) > 0);
  REQUIRE( tchecker::lexical_cmp(r4, r3, intcmp) > 0);
  REQUIRE( tchecker::lexical_cmp(r4, r4, intcmp) == 0);
}
