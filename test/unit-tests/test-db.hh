/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <limits>

#include "tchecker/dbm/db.hh"

#define DB(x,y)  tchecker::dbm::db((x), (y))
#define DB_LE    tchecker::dbm::LE
#define DB_LT    tchecker::dbm::LT
#define SUM(x,y) tchecker::dbm::sum((x), (y))
#define ADD(x,y) tchecker::dbm::add((x), (y))
#define HASH(x)  tchecker::dbm::hash(x)
#define CMP(x)   tchecker::dbm::comparator(x)
#define VAL(x)   tchecker::dbm::value(x)
#ifndef TCHECKER_CONFIG_HH
#error "no TCHECKER_CONFIG_HH"
#endif
#if (INTEGER_T_SIZE==64)
// This has to be kept coherent with the definition of integer
using test_int_t = int64_t; // Modify here
const test_int_t max_int_used = std::numeric_limits<int64_t>::max() >> 1;
const test_int_t min_int_used = std::numeric_limits<int64_t>::min() >> 1;
#elif (INTEGER_T_SIZE==32)
// This has to be kept coherent with the definition of integer
using test_int_t = int32_t; // Modify here
const test_int_t max_int_used = std::numeric_limits<int32_t>::max() >> 1;
const test_int_t min_int_used = std::numeric_limits<int32_t>::min() >> 1;
#elif (INTEGER_T_SIZE==16)
// This has to be kept coherent with the definition of integer
using test_int_t = int16_t; // Modify here
const test_int_t max_int_used = std::numeric_limits<int16_t>::max() >> 1;
const test_int_t min_int_used = std::numeric_limits<int16_t>::min() >> 1;
#else
#error Only 64/32/16 int bit supported
#endif
TEST_CASE( "construction of upper bounds", "[db]" ) {
  
  tchecker::dbm::db_t le_1 = DB(DB_LE, 1);   // <= 1
  tchecker::dbm::db_t lt_1 = DB(DB_LT, 1);   // < 1
  tchecker::dbm::db_t lt_m1 = DB(DB_LT, -1); // < -1
  tchecker::dbm::db_t le_m3 = DB(DB_LE, -3); // <= -3
  
  SECTION( "provided upper bounds" ) {
    REQUIRE(CMP(tchecker::dbm::LE_ZERO) == tchecker::dbm::LE);
    REQUIRE(VAL(tchecker::dbm::LE_ZERO) == 0);
    REQUIRE(CMP(tchecker::dbm::LT_ZERO) == tchecker::dbm::LT);
    REQUIRE(VAL(tchecker::dbm::LT_ZERO) == 0);
    REQUIRE(CMP(tchecker::dbm::LT_INFINITY) == tchecker::dbm::LT);
    REQUIRE(VAL(tchecker::dbm::LT_INFINITY) == max_int_used);
  }
  
  SECTION( "user defined upper bounds" ) {
    REQUIRE(CMP(le_1) == tchecker::dbm::LE);
    REQUIRE(VAL(le_1) == 1);
    REQUIRE(CMP(lt_1) == tchecker::dbm::LT);
    REQUIRE(VAL(lt_1) == 1);
    REQUIRE(CMP(lt_m1) == tchecker::dbm::LT);
    REQUIRE(VAL(lt_m1) == -1);
    REQUIRE(CMP(le_m3) == tchecker::dbm::LE);
    REQUIRE(VAL(le_m3) == -3);
  }
  
  SECTION( "non representable upper bounds" ) {
    REQUIRE_THROWS_AS(DB(tchecker::dbm::LT, max_int_used), std::invalid_argument);
    REQUIRE_THROWS_AS(DB(tchecker::dbm::LT, max_int_used + 1), std::invalid_argument);
    REQUIRE_THROWS_AS(DB(tchecker::dbm::LT, min_int_used - 1), std::invalid_argument);
    REQUIRE_NOTHROW(DB(tchecker::dbm::LT, max_int_used - 1));
    REQUIRE_NOTHROW(DB(tchecker::dbm::LT, min_int_used));
  }
  
}




TEST_CASE( "hash values of upper bounds", "[db]" ) {
  
  tchecker::dbm::db_t le_1a = DB(tchecker::dbm::LE, 1); // <= 1
  tchecker::dbm::db_t le_1b = DB(tchecker::dbm::LE, 1); // <= 1
  tchecker::dbm::db_t lt_0 = DB(tchecker::dbm::LT, 0);  // < 0
  
  SECTION( "same upper bounds have same hash code" ) {
    REQUIRE(HASH(le_1a) == HASH(le_1b));
    REQUIRE(HASH(lt_0) == HASH(tchecker::dbm::LT_ZERO));
  }
  
  SECTION( "distinct values have distinct hash code" ) {
    REQUIRE(HASH(le_1a) != HASH(lt_0));
    REQUIRE(HASH(le_1a) != HASH(tchecker::dbm::LT_INFINITY));
    REQUIRE(HASH(tchecker::dbm::LT_INFINITY) != HASH(lt_0));
  }
}




TEST_CASE( "comparators <,<=,>=,>", "[db]" ) {
  
  tchecker::dbm::db_t le_1 = DB(tchecker::dbm::LE, 1);   // <= 1
  tchecker::dbm::db_t lt_1 = DB(tchecker::dbm::LT, 1);   // < 1
  tchecker::dbm::db_t le_m1 = DB(tchecker::dbm::LE, -1); // <= -1
  tchecker::dbm::db_t lt_m5 = DB(tchecker::dbm::LT, -5); // < -5
  
  SECTION( "less-than comparator <" ) {
    REQUIRE(lt_1 < le_1);
    REQUIRE(le_m1 < le_1);
    REQUIRE(le_m1 < lt_1);
    REQUIRE(lt_m5 < le_1);
    REQUIRE(lt_m5 < lt_1);
    REQUIRE(lt_m5 < le_m1);
    
    REQUIRE_FALSE(le_1 < le_1);
    REQUIRE_FALSE(lt_1 < lt_1);
    REQUIRE_FALSE(le_m1 < le_m1);
    REQUIRE_FALSE(lt_m5 < lt_m5);
    
    REQUIRE_FALSE(le_1 < lt_1);
    REQUIRE_FALSE(le_1 < le_m1);
    REQUIRE_FALSE(le_1 < lt_m5);
    REQUIRE_FALSE(lt_1 < le_m1);
    REQUIRE_FALSE(lt_1 < lt_m5);
    REQUIRE_FALSE(le_m1 < lt_m5);
  }
  
  SECTION( "less-than comparator < w.r.t. <=0" ) {
    REQUIRE_FALSE(tchecker::dbm::LE_ZERO < tchecker::dbm::LE_ZERO);
    
    REQUIRE(tchecker::dbm::LE_ZERO < le_1);
    REQUIRE(tchecker::dbm::LE_ZERO < lt_1);
    REQUIRE(le_m1 < tchecker::dbm::LE_ZERO);
    REQUIRE(lt_m5 < tchecker::dbm::LE_ZERO);
    
    REQUIRE_FALSE(le_1 < tchecker::dbm::LE_ZERO);
    REQUIRE_FALSE(lt_1 < tchecker::dbm::LE_ZERO);
    REQUIRE_FALSE(tchecker::dbm::LE_ZERO < le_m1);
    REQUIRE_FALSE(tchecker::dbm::LE_ZERO < lt_m5);
    
    REQUIRE(tchecker::dbm::LT_ZERO < tchecker::dbm::LE_ZERO);
    REQUIRE_FALSE(tchecker::dbm::LE_ZERO < tchecker::dbm::LT_ZERO);
  }
  
  SECTION( "less-than comparator < w.r.t. <infinity" ) {
    REQUIRE_FALSE(tchecker::dbm::LT_INFINITY < tchecker::dbm::LT_INFINITY);
    
    REQUIRE(le_1 < tchecker::dbm::LT_INFINITY);
    REQUIRE(lt_1 < tchecker::dbm::LT_INFINITY);
    REQUIRE(le_m1 < tchecker::dbm::LT_INFINITY);
    REQUIRE(lt_m5 < tchecker::dbm::LT_INFINITY);
    
    REQUIRE_FALSE(tchecker::dbm::LT_INFINITY < le_1);
    REQUIRE_FALSE(tchecker::dbm::LT_INFINITY < lt_1);
    REQUIRE_FALSE(tchecker::dbm::LT_INFINITY < le_m1);
    REQUIRE_FALSE(tchecker::dbm::LT_INFINITY < lt_m5);
    
    REQUIRE(tchecker::dbm::LE_ZERO < tchecker::dbm::LT_INFINITY);
    REQUIRE_FALSE(tchecker::dbm::LT_INFINITY < tchecker::dbm::LE_ZERO);
  }
  
  SECTION( "less-than-or-equal-to comparator <=" ) {
    REQUIRE(lt_1 <= lt_1);
    REQUIRE(le_1 <= le_1);
    REQUIRE(le_m1 <= le_m1);
    REQUIRE(lt_m5 <= lt_m5);
    
    REQUIRE(lt_1 <= le_1);
    REQUIRE(le_m1 <= le_1);
    REQUIRE(le_m1 <= lt_1);
    REQUIRE(lt_m5 <= le_1);
    REQUIRE(lt_m5 <= lt_1);
    REQUIRE(lt_m5 <= le_m1);
    
    REQUIRE_FALSE(le_1 <= lt_1);
    REQUIRE_FALSE(le_1 <= le_m1);
    REQUIRE_FALSE(le_1 <= lt_m5);
    REQUIRE_FALSE(lt_1 <= le_m1);
    REQUIRE_FALSE(lt_1 <= lt_m5);
    REQUIRE_FALSE(le_m1 <= lt_m5);
  }
  
  SECTION( "less-than-or-equal-to comparator <= w.r.t. <=0" ) {
    REQUIRE(tchecker::dbm::LE_ZERO <= tchecker::dbm::LE_ZERO);
    REQUIRE(tchecker::dbm::LE_ZERO <= le_1);
    REQUIRE(tchecker::dbm::LE_ZERO <= lt_1);
    REQUIRE(le_m1 <= tchecker::dbm::LE_ZERO);
    REQUIRE(lt_m5 <= tchecker::dbm::LE_ZERO);
    
    REQUIRE_FALSE(le_1 <= tchecker::dbm::LE_ZERO);
    REQUIRE_FALSE(lt_1 <= tchecker::dbm::LE_ZERO);
    REQUIRE_FALSE(tchecker::dbm::LE_ZERO <= le_m1);
    REQUIRE_FALSE(tchecker::dbm::LE_ZERO <= lt_m5);
  }
  
  SECTION( "less-than-or-equal-to comparator <= w.r.t. <infinity" ) {
    REQUIRE(tchecker::dbm::LT_INFINITY <= tchecker::dbm::LT_INFINITY);
    
    REQUIRE(le_1 <= tchecker::dbm::LT_INFINITY);
    REQUIRE(lt_1 <= tchecker::dbm::LT_INFINITY);
    REQUIRE(le_m1 <= tchecker::dbm::LT_INFINITY);
    REQUIRE(lt_m5 <= tchecker::dbm::LT_INFINITY);
    
    REQUIRE_FALSE(tchecker::dbm::LT_INFINITY <= le_1);
    REQUIRE_FALSE(tchecker::dbm::LT_INFINITY <= lt_1);
    REQUIRE_FALSE(tchecker::dbm::LT_INFINITY <= le_m1);
    REQUIRE_FALSE(tchecker::dbm::LT_INFINITY <= lt_m5);
    
    REQUIRE(tchecker::dbm::LE_ZERO <= tchecker::dbm::LT_INFINITY);
    REQUIRE_FALSE(tchecker::dbm::LT_INFINITY <= tchecker::dbm::LE_ZERO);
  }
  
  SECTION( "greater-than-or-equal-to comparator >=" ) {
    REQUIRE(lt_1 >= lt_1);
    REQUIRE(le_1 >= le_1);
    REQUIRE(le_m1 >= le_m1);
    REQUIRE(lt_m5 >= lt_m5);
    
    REQUIRE(le_1 >= lt_1);
    REQUIRE(le_1 >= le_m1);
    REQUIRE(le_1 >= lt_m5);
    REQUIRE(lt_1 >= le_m1);
    REQUIRE(lt_1 >= lt_m5);
    REQUIRE(le_m1 >= lt_m5);
    
    REQUIRE_FALSE(lt_1 >= le_1);
    REQUIRE_FALSE(le_m1 >= le_1);
    REQUIRE_FALSE(le_m1 >= lt_1);
    REQUIRE_FALSE(lt_m5 >= le_1);
    REQUIRE_FALSE(lt_m5 >= lt_1);
    REQUIRE_FALSE(lt_m5 >= le_m1);
  }
  
  SECTION( "greater-than-or-equal-to comparator >= w.r.t. <=0" ) {
    REQUIRE(tchecker::dbm::LE_ZERO >= tchecker::dbm::LE_ZERO);
    
    REQUIRE(le_1 >= tchecker::dbm::LE_ZERO);
    REQUIRE(lt_1 >= tchecker::dbm::LE_ZERO);
    REQUIRE(tchecker::dbm::LE_ZERO >= le_m1);
    REQUIRE(tchecker::dbm::LE_ZERO >= lt_m5);
    
    REQUIRE_FALSE(tchecker::dbm::LE_ZERO >= le_1);
    REQUIRE_FALSE(tchecker::dbm::LE_ZERO >= lt_1);
    REQUIRE_FALSE(le_m1 >= tchecker::dbm::LE_ZERO);
    REQUIRE_FALSE(lt_m5 >= tchecker::dbm::LE_ZERO);
  }
  
  SECTION( "greater-than-or-equal-to comparator >= w.r.t. <infinity" ) {
    REQUIRE(tchecker::dbm::LT_INFINITY >= tchecker::dbm::LT_INFINITY);
    
    REQUIRE(tchecker::dbm::LT_INFINITY >= le_1);
    REQUIRE(tchecker::dbm::LT_INFINITY >= lt_1);
    REQUIRE(tchecker::dbm::LT_INFINITY >= le_m1);
    REQUIRE(tchecker::dbm::LT_INFINITY >= lt_m5);
    
    REQUIRE_FALSE(le_1 >= tchecker::dbm::LT_INFINITY);
    REQUIRE_FALSE(lt_1 >= tchecker::dbm::LT_INFINITY);
    REQUIRE_FALSE(le_m1 >= tchecker::dbm::LT_INFINITY);
    REQUIRE_FALSE(lt_m5 >= tchecker::dbm::LT_INFINITY);
    
    REQUIRE(tchecker::dbm::LT_INFINITY >= tchecker::dbm::LE_ZERO);
    REQUIRE_FALSE(tchecker::dbm::LE_ZERO >= tchecker::dbm::LT_INFINITY);
  }
  
  SECTION( "greater-than comparator >" ) {
    REQUIRE(le_1 > lt_1);
    REQUIRE(le_1 > le_m1);
    REQUIRE(le_1 > lt_m5);
    REQUIRE(lt_1 > le_m1);
    REQUIRE(lt_1 > lt_m5);
    REQUIRE(le_m1 > lt_m5);
    
    REQUIRE_FALSE(lt_1 > le_1);
    REQUIRE_FALSE(le_m1 > le_1);
    REQUIRE_FALSE(le_m1 > lt_1);
    REQUIRE_FALSE(lt_m5 > le_1);
    REQUIRE_FALSE(lt_m5 > lt_1);
    REQUIRE_FALSE(lt_m5 > le_m1);
    
    REQUIRE_FALSE(le_1 > le_1);
    REQUIRE_FALSE(lt_1 > lt_1);
    REQUIRE_FALSE(le_m1 > le_m1);
    REQUIRE_FALSE(lt_m5 > lt_m5);
  }
  
  SECTION( "Greater-than comparator > w.r.t. <=0" ) {
    REQUIRE_FALSE(tchecker::dbm::LE_ZERO > tchecker::dbm::LE_ZERO);
    
    REQUIRE(le_1 > tchecker::dbm::LE_ZERO);
    REQUIRE(lt_1 > tchecker::dbm::LE_ZERO);
    REQUIRE(tchecker::dbm::LE_ZERO > le_m1);
    REQUIRE(tchecker::dbm::LE_ZERO > lt_m5);
    
    REQUIRE_FALSE(tchecker::dbm::LE_ZERO > le_1);
    REQUIRE_FALSE(tchecker::dbm::LE_ZERO > lt_1);
    REQUIRE_FALSE(le_m1 > tchecker::dbm::LE_ZERO);
    REQUIRE_FALSE(lt_m5 > tchecker::dbm::LE_ZERO);
    
    REQUIRE(tchecker::dbm::LE_ZERO > tchecker::dbm::LT_ZERO);
    REQUIRE_FALSE(tchecker::dbm::LT_ZERO > tchecker::dbm::LE_ZERO);
  }
  
  SECTION( "greater-than comparator > w.r.t. <infinity" ) {
    REQUIRE_FALSE(tchecker::dbm::LT_INFINITY > tchecker::dbm::LT_INFINITY);
    
    REQUIRE(tchecker::dbm::LT_INFINITY > le_1);
    REQUIRE(tchecker::dbm::LT_INFINITY > lt_1);
    REQUIRE(tchecker::dbm::LT_INFINITY > le_m1);
    REQUIRE(tchecker::dbm::LT_INFINITY > lt_m5);
    
    REQUIRE_FALSE(le_1 > tchecker::dbm::LT_INFINITY);
    REQUIRE_FALSE(lt_1 > tchecker::dbm::LT_INFINITY);
    REQUIRE_FALSE(le_m1 > tchecker::dbm::LT_INFINITY);
    REQUIRE_FALSE(lt_m5 > tchecker::dbm::LT_INFINITY);
    
    REQUIRE(tchecker::dbm::LT_INFINITY > tchecker::dbm::LE_ZERO);
    REQUIRE_FALSE(tchecker::dbm::LE_ZERO > tchecker::dbm::LT_INFINITY);
  }
}




TEST_CASE( "sum", "[db]" ) {
  
  tchecker::dbm::db_t le_1 = DB(tchecker::dbm::LE, 1);     // <= 1
  tchecker::dbm::db_t lt_2 = DB(tchecker::dbm::LT, 2);     // < 2
  tchecker::dbm::db_t le_m1 = DB(tchecker::dbm::LE, -1);   // <= -1
  tchecker::dbm::db_t lt_m11 = DB(tchecker::dbm::LT, -11); // < -11

  tchecker::dbm::db_t le_big = DB(tchecker::dbm::LE, max_int_used - 1);
  tchecker::dbm::db_t le_small = DB(tchecker::dbm::LE, min_int_used);
  
  SECTION( "sum of upper bounds" ) {
    REQUIRE(SUM(le_1, le_1) == DB(DB_LE, 2));
    REQUIRE(SUM(le_1, lt_2) == DB(DB_LT, 3));
    REQUIRE(SUM(le_1, le_m1) == DB(DB_LE, 0));
    REQUIRE(SUM(le_1, lt_m11) == DB(DB_LT, -10));
    
    REQUIRE(SUM(lt_2, lt_2) == DB(DB_LT, 4));
    REQUIRE(SUM(lt_2, le_m1) == DB(DB_LT, 1));
    REQUIRE(SUM(lt_2, lt_m11) == DB(DB_LT, -9));
    
    REQUIRE(SUM(le_m1, le_m1) == DB(DB_LE, -2));
    REQUIRE(SUM(le_m1, lt_m11) == DB(DB_LT, -12));
    
    REQUIRE(SUM(lt_m11, lt_m11) == DB(DB_LT, -22));
  }
  
  SECTION( "<=0 is neutral for sum" ) {
    REQUIRE(SUM(tchecker::dbm::LE_ZERO, tchecker::dbm::LE_ZERO) == tchecker::dbm::LE_ZERO);
    REQUIRE(SUM(tchecker::dbm::LE_ZERO, le_1) == le_1);
    REQUIRE(SUM(tchecker::dbm::LE_ZERO, lt_2) == lt_2);
    REQUIRE(SUM(tchecker::dbm::LE_ZERO, le_m1) == le_m1);
    REQUIRE(SUM(tchecker::dbm::LE_ZERO, lt_m11) == lt_m11);
  }
  
  SECTION( "<infinity is absorbing for sum" ) {
    REQUIRE(SUM(tchecker::dbm::LT_INFINITY, tchecker::dbm::LT_INFINITY) == tchecker::dbm::LT_INFINITY);
    REQUIRE(SUM(tchecker::dbm::LT_INFINITY, le_1) == tchecker::dbm::LT_INFINITY);
    REQUIRE(SUM(tchecker::dbm::LT_INFINITY, lt_2) == tchecker::dbm::LT_INFINITY);
    REQUIRE(SUM(tchecker::dbm::LT_INFINITY, le_m1) == tchecker::dbm::LT_INFINITY);
    REQUIRE(SUM(tchecker::dbm::LT_INFINITY, lt_m11) == tchecker::dbm::LT_INFINITY);
  }
  
  SECTION( "sum is commutative" ) {
    REQUIRE(SUM(le_1, lt_2) == SUM(lt_2, le_1));
    REQUIRE(SUM(le_1, le_m1) == SUM(le_m1, le_1));
    REQUIRE(SUM(le_1, lt_m11) == SUM(lt_m11, le_1));
    REQUIRE(SUM(lt_2, le_m1) == SUM(le_m1, lt_2));
    REQUIRE(SUM(lt_2, lt_m11) == SUM(lt_m11, lt_2));
    REQUIRE(SUM(le_m1, lt_m11) == SUM(lt_m11, le_m1));
    
    REQUIRE(SUM(le_1, tchecker::dbm::LE_ZERO) == SUM(tchecker::dbm::LE_ZERO, le_1));
    REQUIRE(SUM(lt_2, tchecker::dbm::LE_ZERO) == SUM(tchecker::dbm::LE_ZERO, lt_2));
    REQUIRE(SUM(le_m1, tchecker::dbm::LE_ZERO) == SUM(tchecker::dbm::LE_ZERO, le_m1));
    REQUIRE(SUM(lt_m11, tchecker::dbm::LE_ZERO) == SUM(tchecker::dbm::LE_ZERO, lt_m11));
    
    REQUIRE(SUM(le_1, tchecker::dbm::LT_INFINITY) == SUM(tchecker::dbm::LT_INFINITY, le_1));
    REQUIRE(SUM(lt_2, tchecker::dbm::LT_INFINITY) == SUM(tchecker::dbm::LT_INFINITY, lt_2));
    REQUIRE(SUM(le_m1, tchecker::dbm::LT_INFINITY) == SUM(tchecker::dbm::LT_INFINITY, le_m1));
    REQUIRE(SUM(lt_m11, tchecker::dbm::LT_INFINITY) == SUM(tchecker::dbm::LT_INFINITY, lt_m11));
    
    REQUIRE(SUM(tchecker::dbm::LE_ZERO, tchecker::dbm::LT_INFINITY) == SUM(tchecker::dbm::LT_INFINITY, tchecker::dbm::LE_ZERO));
  }
  
  SECTION( "sum is associative" ) {
    REQUIRE(SUM(le_1, SUM(lt_2, le_m1)) == SUM(SUM(le_1, lt_2), le_m1));
    REQUIRE(SUM(lt_m11, SUM(le_1, le_m1)) == SUM(SUM(lt_m11, le_1), le_m1));
    REQUIRE(SUM(lt_2, SUM(tchecker::dbm::LE_ZERO, le_m1)) == SUM(SUM(lt_2, tchecker::dbm::LE_ZERO), le_m1));
    REQUIRE(SUM(lt_2, SUM(tchecker::dbm::LT_INFINITY, le_m1)) == SUM(SUM(lt_2, tchecker::dbm::LT_INFINITY), le_m1));
    REQUIRE(SUM(tchecker::dbm::LT_INFINITY, SUM(le_1, lt_m11)) == SUM(SUM(tchecker::dbm::LT_INFINITY, le_1), lt_m11));
  }
  
  SECTION( "detection of underflow and overflow" ) {
    REQUIRE_THROWS_AS(SUM(le_big, le_1), std::invalid_argument);
    REQUIRE_THROWS_AS(SUM(le_big, lt_2), std::invalid_argument);
    REQUIRE_THROWS_AS(SUM(le_big, le_big), std::invalid_argument);
    REQUIRE_THROWS_AS(SUM(le_small, le_m1), std::invalid_argument);
    REQUIRE_THROWS_AS(SUM(le_small, lt_m11), std::invalid_argument);
    REQUIRE_THROWS_AS(SUM(le_small, le_small), std::invalid_argument);
  }
}




TEST_CASE( "add", "[db]" ) {
  tchecker::dbm::db_t le_3 = DB(DB_LE, 3);   // <= 3
  tchecker::dbm::db_t lt_m2 = DB(DB_LT, -2); // < -2

  tchecker::dbm::db_t le_big = DB(tchecker::dbm::LE, max_int_used - 1);
  tchecker::dbm::db_t le_small = DB(tchecker::dbm::LE, min_int_used);
  
  SECTION( "add value to upper bound" ) {
    REQUIRE(ADD(le_3, 0) == le_3);
    REQUIRE(ADD(le_3, 1) == DB(DB_LE, 4));
    REQUIRE(ADD(le_3, 124) == DB(DB_LE, 127));
    REQUIRE(ADD(le_3, -3) == tchecker::dbm::LE_ZERO);
#if (INTEGER_T_SIZE > 16)
    REQUIRE(ADD(le_3, -39287) == DB(DB_LE, -39284));
#else
      REQUIRE(ADD(le_3, -3287) == DB(DB_LE, -3284));
#endif
    
    REQUIRE(ADD(lt_m2, 0) == DB(DB_LT, -2));
    REQUIRE(ADD(lt_m2, 1) == DB(DB_LT, -1));
    REQUIRE(ADD(lt_m2, 2397) == DB(DB_LT, 2395));
    REQUIRE(ADD(lt_m2, -103) == DB(DB_LT, -105));
  }
  
  SECTION( "add value to <=0" ) {
    REQUIRE(ADD(tchecker::dbm::LE_ZERO, 0) == tchecker::dbm::LE_ZERO);
    REQUIRE(ADD(tchecker::dbm::LE_ZERO, 1) == DB(DB_LE, 1));
    REQUIRE(ADD(tchecker::dbm::LE_ZERO, 1209) == DB(DB_LE, 1209));
    REQUIRE(ADD(tchecker::dbm::LE_ZERO, -12021) == DB(DB_LE, -12021));
  }
  
  SECTION( "<infinity is absorbing for add" ) {
    REQUIRE(ADD(tchecker::dbm::LT_INFINITY, 0) == tchecker::dbm::LT_INFINITY);
    REQUIRE(ADD(tchecker::dbm::LT_INFINITY, 1298) == tchecker::dbm::LT_INFINITY);
    REQUIRE(ADD(tchecker::dbm::LT_INFINITY, -10892) == tchecker::dbm::LT_INFINITY);
  }
  
  SECTION( "detection of underflow and overflow" ) {
    REQUIRE_NOTHROW(ADD(le_big, 0));
    REQUIRE_THROWS_AS(ADD(le_big, 1), std::invalid_argument);
    REQUIRE_THROWS_AS(ADD(le_big, 1109), std::invalid_argument);
    REQUIRE_NOTHROW(ADD(le_small, 0));
    REQUIRE_THROWS_AS(ADD(le_small, -1), std::invalid_argument);
    REQUIRE_THROWS_AS(ADD(le_small, -1012), std::invalid_argument);
  }
}




TEST_CASE( "min et max", "[db]" ) {
  
  tchecker::dbm::db_t le_1 = DB(DB_LE, 1);   // <= 1
  tchecker::dbm::db_t lt_1 = DB(DB_LT, 1);   // < 1
  tchecker::dbm::db_t le_m4 = DB(DB_LE, -4); // <= -4
  tchecker::dbm::db_t lt_m1 = DB(DB_LT, -1); // < -1
  
  SECTION( "min of bounds" ) {
    REQUIRE(tchecker::dbm::min(le_1, le_1) == le_1);
    REQUIRE(tchecker::dbm::min(le_1, lt_1) == lt_1);
    REQUIRE(tchecker::dbm::min(le_1, le_m4) == le_m4);
    REQUIRE(tchecker::dbm::min(le_1, lt_m1) == lt_m1);
    
    REQUIRE(tchecker::dbm::min(lt_1, le_1) == lt_1);
    REQUIRE(tchecker::dbm::min(lt_1, lt_1) == lt_1);
    REQUIRE(tchecker::dbm::min(lt_1, le_m4) == le_m4);
    REQUIRE(tchecker::dbm::min(lt_1, lt_m1) == lt_m1);
    
    REQUIRE(tchecker::dbm::min(le_m4, le_1) == le_m4);
    REQUIRE(tchecker::dbm::min(le_m4, lt_1) == le_m4);
    REQUIRE(tchecker::dbm::min(le_m4, le_m4) == le_m4);
    REQUIRE(tchecker::dbm::min(le_m4, lt_m1) == le_m4);
    
    REQUIRE(tchecker::dbm::min(lt_m1, le_1) == lt_m1);
    REQUIRE(tchecker::dbm::min(lt_m1, lt_1) == lt_m1);
    REQUIRE(tchecker::dbm::min(lt_m1, le_m4) == le_m4);
    REQUIRE(tchecker::dbm::min(lt_m1, lt_m1) == lt_m1);
  }
  
  SECTION( "min w.r.t. <=0" ) {
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LE_ZERO, le_1) == tchecker::dbm::LE_ZERO);
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LE_ZERO, lt_1) == tchecker::dbm::LE_ZERO);
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LE_ZERO, le_m4) == le_m4);
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LE_ZERO, lt_m1) == lt_m1);
    
    REQUIRE(tchecker::dbm::min(le_1, tchecker::dbm::LE_ZERO) == tchecker::dbm::LE_ZERO);
    REQUIRE(tchecker::dbm::min(lt_1, tchecker::dbm::LE_ZERO) == tchecker::dbm::LE_ZERO);
    REQUIRE(tchecker::dbm::min(le_m4, tchecker::dbm::LE_ZERO) == le_m4);
    REQUIRE(tchecker::dbm::min(lt_m1, tchecker::dbm::LE_ZERO) == lt_m1);
    
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LE_ZERO, tchecker::dbm::LE_ZERO) == tchecker::dbm::LE_ZERO);
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LT_ZERO, tchecker::dbm::LE_ZERO) == tchecker::dbm::LT_ZERO);
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LE_ZERO, tchecker::dbm::LT_ZERO) == tchecker::dbm::LT_ZERO);
  }
  
  SECTION( "min w.r.t. <infinity" ) {
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LT_INFINITY, le_1) == le_1);
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LT_INFINITY, lt_1) == lt_1);
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LT_INFINITY, le_m4) == le_m4);
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LT_INFINITY, lt_m1) == lt_m1);
    
    REQUIRE(tchecker::dbm::min(le_1, tchecker::dbm::LT_INFINITY) == le_1);
    REQUIRE(tchecker::dbm::min(lt_1, tchecker::dbm::LT_INFINITY) == lt_1);
    REQUIRE(tchecker::dbm::min(le_m4, tchecker::dbm::LT_INFINITY) == le_m4);
    REQUIRE(tchecker::dbm::min(lt_m1, tchecker::dbm::LT_INFINITY) == lt_m1);
    
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LT_INFINITY, tchecker::dbm::LT_INFINITY) == tchecker::dbm::LT_INFINITY);
    
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LT_INFINITY, tchecker::dbm::LE_ZERO) == tchecker::dbm::LE_ZERO);
    REQUIRE(tchecker::dbm::min(tchecker::dbm::LE_ZERO, tchecker::dbm::LT_INFINITY) == tchecker::dbm::LE_ZERO);
  }
  
  SECTION( "max of bounds" ) {
    REQUIRE(tchecker::dbm::max(le_1, le_1) == le_1);
    REQUIRE(tchecker::dbm::max(le_1, lt_1) == le_1);
    REQUIRE(tchecker::dbm::max(le_1, le_m4) == le_1);
    REQUIRE(tchecker::dbm::max(le_1, lt_m1) == le_1);
    
    REQUIRE(tchecker::dbm::max(lt_1, le_1) == le_1);
    REQUIRE(tchecker::dbm::max(lt_1, lt_1) == lt_1);
    REQUIRE(tchecker::dbm::max(lt_1, le_m4) == lt_1);
    REQUIRE(tchecker::dbm::max(lt_1, lt_m1) == lt_1);
    
    REQUIRE(tchecker::dbm::max(le_m4, le_1) == le_1);
    REQUIRE(tchecker::dbm::max(le_m4, lt_1) == lt_1);
    REQUIRE(tchecker::dbm::max(le_m4, le_m4) == le_m4);
    REQUIRE(tchecker::dbm::max(le_m4, lt_m1) == lt_m1);
    
    REQUIRE(tchecker::dbm::max(lt_m1, le_1) == le_1);
    REQUIRE(tchecker::dbm::max(lt_m1, lt_1) == lt_1);
    REQUIRE(tchecker::dbm::max(lt_m1, le_m4) == lt_m1);
    REQUIRE(tchecker::dbm::max(lt_m1, lt_m1) == lt_m1);
  }
  
  SECTION( "max w.r.t. <=0" ) {
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LE_ZERO, le_1) == le_1);
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LE_ZERO, lt_1) == lt_1);
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LE_ZERO, le_m4) == tchecker::dbm::LE_ZERO);
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LE_ZERO, lt_m1) == tchecker::dbm::LE_ZERO);
    
    REQUIRE(tchecker::dbm::max(le_1, tchecker::dbm::LE_ZERO) == le_1);
    REQUIRE(tchecker::dbm::max(lt_1, tchecker::dbm::LE_ZERO) == lt_1);
    REQUIRE(tchecker::dbm::max(le_m4, tchecker::dbm::LE_ZERO) == tchecker::dbm::LE_ZERO);
    REQUIRE(tchecker::dbm::max(lt_m1, tchecker::dbm::LE_ZERO) == tchecker::dbm::LE_ZERO);
    
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LE_ZERO, tchecker::dbm::LE_ZERO) == tchecker::dbm::LE_ZERO);
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LT_ZERO, tchecker::dbm::LE_ZERO) == tchecker::dbm::LE_ZERO);
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LE_ZERO, tchecker::dbm::LT_ZERO) == tchecker::dbm::LE_ZERO);
  }
  
  SECTION( "max w.r.t. <infinity" ) {
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LT_INFINITY, le_1) == tchecker::dbm::LT_INFINITY);
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LT_INFINITY, lt_1) == tchecker::dbm::LT_INFINITY);
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LT_INFINITY, le_m4) == tchecker::dbm::LT_INFINITY);
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LT_INFINITY, lt_m1) == tchecker::dbm::LT_INFINITY);
    
    REQUIRE(tchecker::dbm::max(le_1, tchecker::dbm::LT_INFINITY) == tchecker::dbm::LT_INFINITY);
    REQUIRE(tchecker::dbm::max(lt_1, tchecker::dbm::LT_INFINITY) == tchecker::dbm::LT_INFINITY);
    REQUIRE(tchecker::dbm::max(le_m4, tchecker::dbm::LT_INFINITY) == tchecker::dbm::LT_INFINITY);
    REQUIRE(tchecker::dbm::max(lt_m1, tchecker::dbm::LT_INFINITY) == tchecker::dbm::LT_INFINITY);
    
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LT_INFINITY, tchecker::dbm::LT_INFINITY) == tchecker::dbm::LT_INFINITY);
    
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LT_INFINITY, tchecker::dbm::LE_ZERO) == tchecker::dbm::LT_INFINITY);
    REQUIRE(tchecker::dbm::max(tchecker::dbm::LE_ZERO, tchecker::dbm::LT_INFINITY) == tchecker::dbm::LT_INFINITY);
  }
  
}
