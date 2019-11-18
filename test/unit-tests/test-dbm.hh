/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/dbm/dbm.hh"

#define DBM(i,j)       dbm[(i)*dim+(j)]
#define DBM1(i,j)      dbm1[(i)*dim+(j)]
#define DBM2(i,j)      dbm2[(i)*dim+(j)]

TEST_CASE( "is_universal, structural tests", "[dbm]") {
  
  SECTION( "is_universal dim=1" ) {
    // Build universal zone dimension 1
    tchecker::clock_id_t const dim = 1;
    tchecker::dbm::db_t dbm[dim * dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    
    REQUIRE(tchecker::dbm::is_universal(dbm, dim));
  }
  
  SECTION( "is_universal dim>1" ) {
    // Build universal zone dimension > 1
    tchecker::clock_id_t const dim = 3;
    tchecker::dbm::db_t dbm[dim * dim];
    for (tchecker::clock_id_t i = 0; i < dim; ++i)
      for (tchecker::clock_id_t j = 0; j < dim; ++j)
        DBM(i,j) = ((i == j) ? tchecker::dbm::LE_ZERO : tchecker::dbm::LT_INFINITY);
    
    REQUIRE(tchecker::dbm::is_universal(dbm, dim));
  }
  
  SECTION( "is_universal, non universal, dim>1" ) {
    // Build non-universal zone dimension > 1
    tchecker::clock_id_t const dim = 5;
    tchecker::dbm::db_t dbm[dim * dim];
    for (tchecker::clock_id_t i = 0; i < dim; ++i)
      for (tchecker::clock_id_t j = 0; j < dim; ++j)
        DBM(i,j) = tchecker::dbm::LE_ZERO;
    
    REQUIRE_FALSE(tchecker::dbm::is_universal(dbm, dim));
  }
  
}




TEST_CASE( "is_positive, structural test", "[dbm]" ) {
  
  SECTION( "is_positive on positive zone, dim=1" ) {
    tchecker::clock_id_t dim = 1;
    tchecker::dbm::db_t dbm[dim*dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    
    REQUIRE(tchecker::dbm::is_positive(dbm, dim));
  }
  
  SECTION( "is_positive on empty zone, dim=1" ) {
    tchecker::clock_id_t dim = 1;
    tchecker::dbm::db_t dbm[dim*dim];
    DBM(0,0) = tchecker::dbm::LT_ZERO;
    
    REQUIRE(tchecker::dbm::is_positive(dbm, dim));
  }
  
  SECTION( "is_positive on positive zone, dim>1" ) {
    tchecker::clock_id_t dim = 3;
    tchecker::dbm::db_t dbm[dim*dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM(0,2) = tchecker::dbm::db(tchecker::dbm::LT, 0);
    DBM(1,0) = tchecker::dbm::LT_INFINITY;
    DBM(1,1) = tchecker::dbm::LE_ZERO;
    DBM(1,2) = tchecker::dbm::db(tchecker::dbm::LE, 4);
    DBM(2,0) = tchecker::dbm::LT_INFINITY;
    DBM(2,1) = tchecker::dbm::LT_INFINITY;
    DBM(2,2) = tchecker::dbm::LE_ZERO;
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    
    REQUIRE(tchecker::dbm::is_positive(dbm, dim));
  }
  
  SECTION( "is_positive on non-positive zone (1), dim>1" ) {
    tchecker::clock_id_t dim = 3;
    tchecker::dbm::db_t dbm[dim*dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    DBM(0,2) = tchecker::dbm::db(tchecker::dbm::LT, 0);
    DBM(1,0) = tchecker::dbm::LT_INFINITY;
    DBM(1,1) = tchecker::dbm::LE_ZERO;
    DBM(1,2) = tchecker::dbm::LT_INFINITY;
    DBM(2,0) = tchecker::dbm::LT_INFINITY;
    DBM(2,1) = tchecker::dbm::LT_INFINITY;
    DBM(2,2) = tchecker::dbm::LE_ZERO;
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    
    REQUIRE_FALSE(tchecker::dbm::is_positive(dbm, dim));
  }
  
  SECTION( "is_positive on non-positive zone (2), dim>1" ) {
    tchecker::clock_id_t dim = 3;
    tchecker::dbm::db_t dbm[dim*dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LE, -5);
    DBM(0,2) = tchecker::dbm::db(tchecker::dbm::LT, 9);
    DBM(1,0) = tchecker::dbm::LT_INFINITY;
    DBM(1,1) = tchecker::dbm::LE_ZERO;
    DBM(1,2) = tchecker::dbm::LT_INFINITY;
    DBM(2,0) = tchecker::dbm::LT_INFINITY;
    DBM(2,1) = tchecker::dbm::LT_INFINITY;
    DBM(2,2) = tchecker::dbm::LE_ZERO;
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    
    REQUIRE_FALSE(tchecker::dbm::is_positive(dbm, dim));
  }
  
}




TEST_CASE( "is_universal_positive, structural test", "[dbm]" ) {
  
  SECTION( "is_universal_positive on universal-positive zone, dim=1" ) {
    tchecker::clock_id_t dim = 1;
    tchecker::dbm::db_t dbm[dim*dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    
    REQUIRE(tchecker::dbm::is_universal_positive(dbm, dim));
  }
  
  SECTION( "is_universal_positive on empty zone, dim=1" ) {
    tchecker::clock_id_t dim = 1;
    tchecker::dbm::db_t dbm[dim*dim];
    DBM(0,0) = tchecker::dbm::LT_ZERO;
    
    REQUIRE_FALSE(tchecker::dbm::is_universal_positive(dbm, dim));
  }
  
  SECTION( "is_universal_positive on universal-positive zone, dim>1" ) {
    tchecker::clock_id_t dim = 3;
    tchecker::dbm::db_t dbm[dim*dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    DBM(0,1) = tchecker::dbm::LE_ZERO;
    DBM(0,2) = tchecker::dbm::LE_ZERO;
    DBM(1,0) = tchecker::dbm::LT_INFINITY;
    DBM(1,1) = tchecker::dbm::LE_ZERO;
    DBM(1,2) = tchecker::dbm::LT_INFINITY;
    DBM(2,0) = tchecker::dbm::LT_INFINITY;
    DBM(2,1) = tchecker::dbm::LT_INFINITY;
    DBM(2,2) = tchecker::dbm::LE_ZERO;
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    
    REQUIRE(tchecker::dbm::is_universal_positive(dbm, dim));
  }
  
  SECTION( "is_positive on positive, non-universal, zone, dim>1" ) {
    tchecker::clock_id_t dim = 3;
    tchecker::dbm::db_t dbm[dim*dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM(0,2) = tchecker::dbm::db(tchecker::dbm::LT, 0);
    DBM(1,0) = tchecker::dbm::LT_INFINITY;
    DBM(1,1) = tchecker::dbm::LE_ZERO;
    DBM(1,2) = tchecker::dbm::db(tchecker::dbm::LE, 7);
    DBM(2,0) = tchecker::dbm::LT_INFINITY;
    DBM(2,1) = tchecker::dbm::LT_INFINITY;
    DBM(2,2) = tchecker::dbm::LE_ZERO;
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    
    REQUIRE_FALSE(tchecker::dbm::is_universal_positive(dbm, dim));
  }
  
  SECTION( "is_positive on non-positive zone (1), dim>1" ) {
    tchecker::clock_id_t dim = 3;
    tchecker::dbm::db_t dbm[dim*dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    DBM(0,2) = tchecker::dbm::db(tchecker::dbm::LT, 0);
    DBM(1,0) = tchecker::dbm::LT_INFINITY;
    DBM(1,1) = tchecker::dbm::LE_ZERO;
    DBM(1,2) = tchecker::dbm::LT_INFINITY;
    DBM(2,0) = tchecker::dbm::LT_INFINITY;
    DBM(2,1) = tchecker::dbm::LT_INFINITY;
    DBM(2,2) = tchecker::dbm::LE_ZERO;
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    
    REQUIRE_FALSE(tchecker::dbm::is_universal_positive(dbm, dim));
  }
  
  SECTION( "is_positive on non-positive zone (2), dim>1" ) {
    tchecker::clock_id_t dim = 3;
    tchecker::dbm::db_t dbm[dim*dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM(0,2) = tchecker::dbm::db(tchecker::dbm::LT, 2389);
    DBM(1,0) = tchecker::dbm::LT_INFINITY;
    DBM(1,1) = tchecker::dbm::LE_ZERO;
    DBM(1,2) = tchecker::dbm::LT_INFINITY;
    DBM(2,0) = tchecker::dbm::LT_INFINITY;
    DBM(2,1) = tchecker::dbm::LT_INFINITY;
    DBM(2,2) = tchecker::dbm::LE_ZERO;
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    
    REQUIRE_FALSE(tchecker::dbm::is_universal_positive(dbm, dim));
  }
  
}




TEST_CASE( "is_empty_0, structural tests", "[dbm]" ) {
  
  SECTION( "is_empty_0 on negative diagonal, dim=1" ) {
    // Build negative diagonal dimension 1
    tchecker::clock_id_t dim = 1;
    tchecker::dbm::db_t dbm[dim * dim];
    DBM(0,0) = tchecker::dbm::LT_ZERO;
    
    REQUIRE(tchecker::dbm::is_empty_0(dbm, dim));
  }
  
  SECTION( "is_empty_0 on non-negative diagonal, dim=1" ) {
    // Build negative diagonal dimension 1
    tchecker::clock_id_t dim = 1;
    tchecker::dbm::db_t dbm[dim * dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    
    REQUIRE_FALSE(tchecker::dbm::is_empty_0(dbm, dim));
  }
  
  SECTION( "is_empty_0 on negative diagonal, dim>1" ) {
    // Build negative diagonal dimension 1
    tchecker::clock_id_t dim = 5;
    tchecker::dbm::db_t dbm[dim * dim];
    for (tchecker::clock_id_t i = 0; i < dim; ++i)
      for (tchecker::clock_id_t j = 0; j < dim; ++j)
        DBM(i,j) = tchecker::dbm::LE_ZERO;
    DBM(0,0) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    
    REQUIRE(tchecker::dbm::is_empty_0(dbm, dim));
  }
  
  SECTION( "is_empty_0 on non-negative diagonal, dim>1" ) {
    // Build negative diagonal dimension 1
    tchecker::clock_id_t dim = 3;
    tchecker::dbm::db_t dbm[dim * dim];
    for (tchecker::clock_id_t i = 0; i < dim; ++i)
      for (tchecker::clock_id_t j = 0; j < dim; ++j)
        DBM(i,j) = tchecker::dbm::LE_ZERO;
    
    REQUIRE_FALSE(tchecker::dbm::is_empty_0(dbm, dim));
  }
  
  SECTION( "is_empty_0 on non-negative diagonal, but empty zone, dim>1" ) {
    // Build negative diagonal dimension 1
    tchecker::clock_id_t dim = 6;
    tchecker::dbm::db_t dbm[dim * dim];
    for (tchecker::clock_id_t i = 0; i < dim; ++i)
      for (tchecker::clock_id_t j = 0; j < dim; ++j)
        DBM(i,j) = tchecker::dbm::LE_ZERO;
    DBM(1,4) = tchecker::dbm::LT_ZERO;
    
    REQUIRE_FALSE(tchecker::dbm::is_empty_0(dbm, dim));
  }
  
}




TEST_CASE( "is_tight, structural tests", "[dbm]" ) {
  
  SECTION( "is_tight on tight DBM, dim=1" ) {
    // Build DBM, dimension 1
    tchecker::clock_id_t dim = 1;
    tchecker::dbm::db_t dbm[dim * dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
  }
  
  SECTION( "is_tight on tight DBM, dim>1" ) {
    // Build tight DBM x1>0 & x1-x2<=3 & x2<7
    tchecker::clock_id_t dim = 3;
    tchecker::dbm::db_t dbm[dim * dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LT, 0);  // x1>0
    DBM(0,2) = tchecker::dbm::LE_ZERO;                   // x2>=0
    DBM(1,0) = tchecker::dbm::db(tchecker::dbm::LT, 10); // x1<10
    DBM(1,1) = tchecker::dbm::LE_ZERO;
    DBM(1,2) = tchecker::dbm::db(tchecker::dbm::LE, 3);  // x1-x2<=3
    DBM(2,0) = tchecker::dbm::db(tchecker::dbm::LT, 7);  // x2<7
    DBM(2,1) = tchecker::dbm::db(tchecker::dbm::LT, 7);  // x2-x1<7
    DBM(2,2) = tchecker::dbm::LE_ZERO;
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
  }
  
  SECTION( "is_tight on non-tight DBM, dim>1" ) {
    // Build non-tight DBM x1>0 & x1-x2<=3 & x2<7
    tchecker::clock_id_t dim = 3;
    tchecker::dbm::db_t dbm[dim * dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LT, 0);  // x1>0
    DBM(0,2) = tchecker::dbm::LE_ZERO;                   // x2>=0
    DBM(1,0) = tchecker::dbm::LT_INFINITY;               // x1<infinity
    DBM(1,1) = tchecker::dbm::LE_ZERO;
    DBM(1,2) = tchecker::dbm::db(tchecker::dbm::LE, 3);  // x1-x2<=3
    DBM(2,0) = tchecker::dbm::db(tchecker::dbm::LT, 7);  // x2<7
    DBM(2,1) = tchecker::dbm::LT_INFINITY;               // x2-x1<infinity
    DBM(2,2) = tchecker::dbm::LE_ZERO;
    
    REQUIRE_FALSE(tchecker::dbm::is_tight(dbm, dim));
  }
  
}




TEST_CASE( "universal makes universal zones", "[dbm]" ) {
  
  SECTION( "universal dim=1" ) {
    tchecker::clock_id_t const dim = 1;
    tchecker::dbm::db_t dbm[dim * dim];
    
    tchecker::dbm::universal(dbm, dim);
    REQUIRE(tchecker::dbm::is_universal(dbm, dim));
  }
  
  SECTION( "universal dim>1" ) {
    tchecker::clock_id_t const dim = 4;
    tchecker::dbm::db_t dbm[dim * dim];
    
    tchecker::dbm::universal(dbm, dim);
    REQUIRE(tchecker::dbm::is_universal(dbm, dim));
  }
  
}




TEST_CASE( "universal_positive makes universal-positive zones", "[dbm]" ) {
  
  SECTION( "universal_positive dim=1" ) {
    tchecker::clock_id_t const dim = 1;
    tchecker::dbm::db_t dbm[dim * dim];
    
    tchecker::dbm::universal_positive(dbm, dim);
    REQUIRE(tchecker::dbm::is_universal_positive(dbm, dim));
  }
  
  SECTION( "universal_positive dim>1" ) {
    tchecker::clock_id_t const dim = 4;
    tchecker::dbm::db_t dbm[dim * dim];
    
    tchecker::dbm::universal_positive(dbm, dim);
    REQUIRE(tchecker::dbm::is_universal_positive(dbm, dim));
  }
  
}




TEST_CASE( "tighten (full)", "[dbm]" ) {
  
  SECTION( "tighten non-empty dbm, dim=1" ) {
    // Build DBM, dimension 1
    tchecker::clock_id_t dim = 1;
    tchecker::dbm::db_t dbm[dim * dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    
    REQUIRE(tchecker::dbm::tighten(dbm, dim) == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
  }
  
  SECTION( "tighten non-empty, dim>1" ) {
    // Build DBM
    tchecker::clock_id_t dim = 3;
    tchecker::dbm::db_t dbm[dim * dim];
    DBM(0,0) = tchecker::dbm::LE_ZERO;
    DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LT, 0);  // x1>0
    DBM(0,2) = tchecker::dbm::LE_ZERO;                   // x2>=0
    DBM(1,0) = tchecker::dbm::LT_INFINITY;               // x1<infinity
    DBM(1,1) = tchecker::dbm::LE_ZERO;
    DBM(1,2) = tchecker::dbm::db(tchecker::dbm::LE, 3);  // x1-x2<=3
    DBM(2,0) = tchecker::dbm::db(tchecker::dbm::LT, 7);  // x2<7
    DBM(2,1) = tchecker::dbm::LT_INFINITY;               // x2-x1<infinity
    DBM(2,2) = tchecker::dbm::LE_ZERO;
    
    REQUIRE(tchecker::dbm::tighten(dbm, dim) == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    
    REQUIRE(DBM(0,0) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(0,1) == tchecker::dbm::db(tchecker::dbm::LT, 0));  // x1>0
    REQUIRE(DBM(0,2) == tchecker::dbm::LE_ZERO);                   // x2>=0
    REQUIRE(DBM(1,0) == tchecker::dbm::db(tchecker::dbm::LT, 10)); // x1<10
    REQUIRE(DBM(1,1) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(1,2) == tchecker::dbm::db(tchecker::dbm::LE, 3));  // x1-x2<=3
    REQUIRE(DBM(2,0) == tchecker::dbm::db(tchecker::dbm::LT, 7));  // x2<7
    REQUIRE(DBM(2,1) == tchecker::dbm::db(tchecker::dbm::LT, 7));  // x2-x1<7
    REQUIRE(DBM(2,2) == tchecker::dbm::LE_ZERO);
  }
  
  SECTION( "tighten empty, dim>1" ) {
    // Build DBM
    tchecker::clock_id_t dim = 5;
    tchecker::dbm::db_t dbm[dim * dim];
    tchecker::dbm::universal_positive(dbm, dim);
    DBM(1,2) = tchecker::dbm::db(tchecker::dbm::LT, 1);  // x1-x2<1
    DBM(2,4) = tchecker::dbm::db(tchecker::dbm::LE, -2); // x2-x4<=-2
    DBM(4,3) = tchecker::dbm::LE_ZERO;                   // x4-x3<=0
    DBM(3,1) = tchecker::dbm::db(tchecker::dbm::LE, 1);  // x3-x1<=1
    
    REQUIRE(tchecker::dbm::tighten(dbm, dim) == tchecker::dbm::EMPTY);
    REQUIRE(DBM(0,0) < tchecker::dbm::LE_ZERO);
  }
  
}




TEST_CASE( "tighten w.r.t. a difference bound", "[dbm]" ) {
  
  tchecker::clock_id_t dim = 4;
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  
  DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LT, 0);  // x1>0
  DBM(1,2) = tchecker::dbm::db(tchecker::dbm::LE, 4);  // x1-x2<=4
  DBM(2,0) = tchecker::dbm::db(tchecker::dbm::LE, 9);  // x2<=9
  
  SECTION( "tighten non-empty, not tight" ) {
    auto res = tchecker::dbm::tighten(dbm, dim, 1, 2); // tighten w.r.t. x1-x2
    
    REQUIRE(res == tchecker::dbm::MAY_BE_EMPTY);
    
    REQUIRE(DBM(0,0) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(0,1) == tchecker::dbm::db(tchecker::dbm::LT, 0));  // x1>0
    REQUIRE(DBM(0,2) == tchecker::dbm::LE_ZERO);                   // x2>=0
    REQUIRE(DBM(0,3) == tchecker::dbm::LE_ZERO);                   // x3>=0
    REQUIRE(DBM(1,0) == tchecker::dbm::db(tchecker::dbm::LE, 13)); // x1<=13
    REQUIRE(DBM(1,1) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(1,2) == tchecker::dbm::db(tchecker::dbm::LE, 4));  // x1-x2<=4
    REQUIRE(DBM(1,3) == tchecker::dbm::LT_INFINITY);               // x1-x3<inf
    REQUIRE(DBM(2,0) == tchecker::dbm::db(tchecker::dbm::LE, 9));  // x2<=9
    REQUIRE(DBM(2,1) == tchecker::dbm::LT_INFINITY);               // x2-x1<inf
    REQUIRE(DBM(2,2) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(2,3) == tchecker::dbm::LT_INFINITY);               // x2-x3<inf
    REQUIRE(DBM(3,0) == tchecker::dbm::LT_INFINITY);               // x3<inf
    REQUIRE(DBM(3,1) == tchecker::dbm::LT_INFINITY);               // x3-x1<inf
    REQUIRE(DBM(3,2) == tchecker::dbm::LT_INFINITY);               // x3-x2<inf
    REQUIRE(DBM(3,3) == tchecker::dbm::LE_ZERO);
    
    REQUIRE_FALSE(tchecker::dbm::is_tight(dbm, dim));  // x2-x1 is not tight
  }
  
  SECTION( "tighten non-empty, tight" ) {
    auto res = tchecker::dbm::tighten(dbm, dim, 1, 2); // tighten w.r.t. x1-x2
    REQUIRE(res == tchecker::dbm::MAY_BE_EMPTY);
    res = tchecker::dbm::tighten(dbm, dim, 2, 0);      // tighten w.r.t. x2-0
    REQUIRE(res == tchecker::dbm::MAY_BE_EMPTY);
    res = tchecker::dbm::tighten(dbm, dim, 1, 0);      // tighten w.r.t. x1-0
    REQUIRE(res == tchecker::dbm::MAY_BE_EMPTY);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
  }
  
  SECTION( "tighten empty" ) {
    DBM(0,2) = tchecker::dbm::db(tchecker::dbm::LT, -9);   // x2>9
    
    auto res = tchecker::dbm::tighten(dbm, dim, 0, 2);
    
    REQUIRE(res == tchecker::dbm::EMPTY);
    REQUIRE(tchecker::dbm::is_empty_0(dbm, dim));
  }
  
}




TEST_CASE( "constrain", "[dbm]" ) {
  
  tchecker::clock_id_t dim = 5;
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  
  DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LT, 0);  // x1>0
  DBM(1,3) = tchecker::dbm::db(tchecker::dbm::LE, 2);  // x1-x3<=2
  DBM(3,2) = tchecker::dbm::db(tchecker::dbm::LE, 6);  // x3-x2<=6
  
  tchecker::dbm::tighten(dbm, dim);
  
  SECTION( "constrain, no effect" ) {
    auto res = tchecker::dbm::constrain(dbm, dim, 1, 2, tchecker::dbm::LE, 8);
    
    REQUIRE(res == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    
    REQUIRE(DBM(0,0) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(0,1) == tchecker::dbm::db(tchecker::dbm::LT, 0));
    REQUIRE(DBM(0,2) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(0,3) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(0,4) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(1,0) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(1,1) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(1,2) == tchecker::dbm::db(tchecker::dbm::LE, 8));
    REQUIRE(DBM(1,3) == tchecker::dbm::db(tchecker::dbm::LE, 2));
    REQUIRE(DBM(1,4) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(2,0) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(2,1) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(2,2) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(2,3) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(2,4) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(3,0) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(3,1) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(3,2) == tchecker::dbm::db(tchecker::dbm::LE, 6));
    REQUIRE(DBM(3,3) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(3,4) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(4,0) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(4,1) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(4,2) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(4,3) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(4,4) == tchecker::dbm::LE_ZERO);
  }
  
  SECTION( "constrain, effect, non empty" ) {
    auto res = tchecker::dbm::constrain(dbm, dim, 4, 3, tchecker::dbm::LT, -10);
    
    REQUIRE(res == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    
    REQUIRE(DBM(0,0) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(0,1) == tchecker::dbm::db(tchecker::dbm::LT, 0));
    REQUIRE(DBM(0,2) == tchecker::dbm::db(tchecker::dbm::LT, -4));
    REQUIRE(DBM(0,3) == tchecker::dbm::db(tchecker::dbm::LT, -10));
    REQUIRE(DBM(0,4) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(1,0) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(1,1) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(1,2) == tchecker::dbm::db(tchecker::dbm::LE, 8));
    REQUIRE(DBM(1,3) == tchecker::dbm::db(tchecker::dbm::LE, 2));
    REQUIRE(DBM(1,4) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(2,0) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(2,1) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(2,2) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(2,3) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(2,4) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(3,0) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(3,1) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(3,2) == tchecker::dbm::db(tchecker::dbm::LE, 6));
    REQUIRE(DBM(3,3) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(3,4) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(4,0) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(4,1) == tchecker::dbm::LT_INFINITY);
    REQUIRE(DBM(4,2) == tchecker::dbm::db(tchecker::dbm::LT, -4));
    REQUIRE(DBM(4,3) == tchecker::dbm::db(tchecker::dbm::LT, -10));
    REQUIRE(DBM(4,4) == tchecker::dbm::LE_ZERO);
  }
  
  SECTION( "constrain, effect, empty resulting DBM" ) {
    auto res = tchecker::dbm::constrain(dbm, dim, 3, 0, tchecker::dbm::LE, -2);
    REQUIRE(res == tchecker::dbm::EMPTY);
    REQUIRE(tchecker::dbm::is_empty_0(dbm, dim));
  }
  
}




TEST_CASE( "DBM equality", "[dbm]" ) {
  
  tchecker::clock_id_t dim = 5;
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  
  DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LT, 0);  // x1>0
  DBM(1,4) = tchecker::dbm::db(tchecker::dbm::LE, 8);  // x1-x4<=8
  DBM(3,2) = tchecker::dbm::db(tchecker::dbm::LT, -7); // x3-x2<-7
  
  tchecker::dbm::tighten(dbm, dim);
  
  SECTION( "DBM is equal to itself" ) {
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm, dim));
  }
  
  SECTION( "DBM is equal to similar DBM" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm2, dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm2, dbm2, dim));
  }
  
  SECTION( "Distinct DBMs are not equal (1)" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    
    DBM2(3,2) = tchecker::dbm::db(tchecker::dbm::LE, -7);
    tchecker::dbm::tighten(dbm2, dim);
    REQUIRE_FALSE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "Distinct DBMs are not equal (2)" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    
    DBM2(4,1) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    tchecker::dbm::tighten(dbm2, dim);
    REQUIRE_FALSE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "Distinct DBMs are not equal (3)" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    
    DBM2(0,1) = tchecker::dbm::db(tchecker::dbm::LE, -3);
    tchecker::dbm::tighten(dbm2, dim);
    REQUIRE_FALSE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "Distinct DBMs are not equal (4)" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    
    DBM2(2,0) = tchecker::dbm::db(tchecker::dbm::LT, 11);
    tchecker::dbm::tighten(dbm2, dim);
    REQUIRE_FALSE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
}




TEST_CASE( "DBM inclusion", "[dbm]" ) {
  
  tchecker::clock_id_t dim = 4;
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  
  DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LT, 0);  // x1>0
  DBM(1,2) = tchecker::dbm::db(tchecker::dbm::LE, 2);  // x1-x2<=2
  DBM(3,0) = tchecker::dbm::db(tchecker::dbm::LT, 9);  // x3-0<9
  
  tchecker::dbm::tighten(dbm, dim);
  
  SECTION( "DBM is included into itself" ) {
    REQUIRE(tchecker::dbm::is_le(dbm, dbm, dim));
  }
  
  SECTION( "DBM is included into similar DBM" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    
    REQUIRE(tchecker::dbm::is_le(dbm, dbm2, dim));
    REQUIRE(tchecker::dbm::is_le(dbm2, dbm, dim));
    REQUIRE(tchecker::dbm::is_le(dbm2, dbm2, dim));
  }
  
  SECTION( "DBM is included into bigger DBM" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    
    for (tchecker::clock_id_t i = 0; i < dim; ++i) {
      for (tchecker::clock_id_t j = 0; j < dim; ++j) {
        if (i == j)
          continue;
        
        memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
        DBM2(i,j) = tchecker::dbm::add(DBM2(i,j), -1);
        tchecker::dbm::tighten(dbm2, dim);
        
        REQUIRE(tchecker::dbm::is_le(dbm2, dbm, dim));
      }
    }
  }
  
  SECTION( "DBM is not included into smaller DBM (1)" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(3,0) = tchecker::dbm::db(tchecker::dbm::LE, 7);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE_FALSE(tchecker::dbm::is_le(dbm, dbm2, dim));
  }
  
  SECTION( "DBM is not included into smaller DBM (2)" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(1,2) = tchecker::dbm::db(tchecker::dbm::LT, 1);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE_FALSE(tchecker::dbm::is_le(dbm, dbm2, dim));
  }
  
  SECTION( "DBM is not included into smaller DBM (3)" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,2) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE_FALSE(tchecker::dbm::is_le(dbm, dbm2, dim));
  }
  
  SECTION( "DBM is not included into smaller DBM (4)" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(2,3) = tchecker::dbm::db(tchecker::dbm::LE, 19);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE_FALSE(tchecker::dbm::is_le(dbm, dbm2, dim));
  }
  
  SECTION( "DBM is not included into incomparable DBM (1)" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,1) = tchecker::dbm::LE_ZERO;
    DBM2(2,3) = tchecker::dbm::db(tchecker::dbm::LT, 15);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE_FALSE(tchecker::dbm::is_le(dbm, dbm2, dim));
  }
  
  SECTION( "DBM is not included into incomparable DBM (2)" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(1,2) = tchecker::dbm::LT_INFINITY;
    DBM2(2,0) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE_FALSE(tchecker::dbm::is_le(dbm, dbm2, dim));
  }
  
}




TEST_CASE( "reset DBM", "[dbm]" ) {
  
  tchecker::clock_id_t dim = 4;
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  
  // x1 > 2 & 1 <= x2 < 4 & x1-x2 > 1 & x3-x2 >= 5 & x3-x1 <= 8
  
  DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LT, -2);
  DBM(0,2) = tchecker::dbm::db(tchecker::dbm::LE, -1);
  DBM(2,0) = tchecker::dbm::db(tchecker::dbm::LT, 4);
  DBM(2,1) = tchecker::dbm::db(tchecker::dbm::LT, -1);
  DBM(2,3) = tchecker::dbm::db(tchecker::dbm::LE, -5);
  DBM(3,1) = tchecker::dbm::db(tchecker::dbm::LE, 8);
  
  tchecker::dbm::tighten(dbm, dim);
  
  tchecker::dbm::db_t dbm2[dim * dim];
  memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
  
  // dbm is:
  // <=0  <-2  <=-1  <=-6
  // <inf <=0  <inf  <inf
  // <4   <-1  <=0   <=-5
  // <inf <=8  <inf  <=0
  
  SECTION( "reset to zero" ) {
    tchecker::integer_t const value = 0;
    tchecker::clock_id_t const x = 1;
    
    // difference bound (x,k) is (x,0)+(0,k) where (x,0) is value
    // difference bound (k,x) is (k,0)+(0,x) where (0,x) is value
    for (tchecker::clock_id_t k = 0; k < dim; ++k) {
      DBM2(x,k) = tchecker::dbm::add(DBM2(0,k), value);
      DBM2(k,x) = tchecker::dbm::add(DBM2(k,0), -value);
    }
    
    tchecker::dbm::reset(dbm, dim, x, 0, value);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "reset to positive value" ) {
    tchecker::integer_t const value = 3;
    tchecker::clock_id_t const x = 2;
    
    // difference bound (x,k) is (x,0)+(0,k) where (x,0) is value
    // difference bound (k,x) is (k,0)+(0,x) where (0,x) is value
    for (tchecker::clock_id_t k = 0; k < dim; ++k) {
      DBM2(x,k) = tchecker::dbm::add(DBM2(0,k), value);
      DBM2(k,x) = tchecker::dbm::add(DBM2(k,0), -value);
    }
    
    tchecker::dbm::reset(dbm, dim, x, 0, value);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "reset to a big value" ) {
    REQUIRE_THROWS_AS(tchecker::dbm::reset(dbm, dim, 1, 0, tchecker::dbm::MAX_VALUE + 1), std::invalid_argument);
  }
  
  SECTION( "reset to clock (1st)" ) {
    tchecker::clock_id_t const x = 3;
    tchecker::clock_id_t const y = 1;
    
    // x and y become equal, and constraints on y are mimicked in x
    for (tchecker::clock_id_t k = 0; k < dim; ++k) {
      if (x == k)
        continue;
      DBM2(x,k) = DBM2(y,k);
      DBM2(k,x) = DBM2(k,y);
    }
    
    tchecker::dbm::reset(dbm, dim, x, y, 0);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "reset to clock (2nd)" ) {
    tchecker::clock_id_t const x = 2;
    tchecker::clock_id_t const y = 3;
    
    // x becomes an alias of y
    for (tchecker::clock_id_t k = 0; k < dim; ++k) {
      if (x == k)
        continue;
      DBM2(x,k) = DBM2(y,k);
      DBM2(k,x) = DBM2(k,y);
    }
    
    tchecker::dbm::reset(dbm, dim, x, y, 0);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "reset clock to itself" ) {
    tchecker::dbm::reset(dbm, dim, 1, 1, 0);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "reset clock to increment (1st)" ) {
    tchecker::clock_id_t const x = 1;
    tchecker::integer_t const value = 1;
    
    // x' - x <= value & x - k <= c & k - k' <= 0 --> x' - k' <= value + c
    // x - x' <= -value & k - x <= c & k' - k <= 0 ---> k' - x' <= -value + c
    for (tchecker::clock_id_t k = 0; k < dim; ++k) {
      if (x == k)
        continue;
      
      DBM2(x,k) = tchecker::dbm::add(DBM2(x,k), value);
      DBM2(k,x) = tchecker::dbm::add(DBM2(k,x), -value);
    }
    
    tchecker::dbm::reset(dbm, dim, x, x, value);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "reset clock to increment (2nd)" ) {
    tchecker::clock_id_t const x = 2;
    tchecker::integer_t const value = 10;
    
    // x' - x <= value & x - k <= c & k - k' <= 0 --> x' - k' <= value + c
    // x - x' <= -value & k - x <= c & k' - k <= 0 ---> k' - x' <= -value + c
    for (tchecker::clock_id_t k = 0; k < dim; ++k) {
      if (x == k)
        continue;
      
      DBM2(x,k) = tchecker::dbm::add(DBM2(x,k), value);
      DBM2(k,x) = tchecker::dbm::add(DBM2(k,x), -value);
    }
    
    tchecker::dbm::reset(dbm, dim, x, x, value);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "reset clock to increment (3rd)" ) {
    tchecker::clock_id_t const x = 3;
    tchecker::integer_t const value = 7;
    
    // x' - x <= value & x - k <= c & k - k' <= 0 --> x' - k' <= value + c
    // x - x' <= -value & k - x <= c & k' - k <= 0 ---> k' - x' <= -value + c
    for (tchecker::clock_id_t k = 0; k < dim; ++k) {
      if (x == k)
        continue;
      
      DBM2(x,k) = tchecker::dbm::add(DBM2(x,k), value);
      DBM2(k,x) = tchecker::dbm::add(DBM2(k,x), -value);
    }
    
    tchecker::dbm::reset(dbm, dim, x, x, value);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "reset clock to sum (1st)" ) {
    tchecker::clock_id_t const x = 2;
    tchecker::clock_id_t const y = 3;
    tchecker::integer_t const value = 1;
    
    // x - y <= value & y - k <= c --> x - k <= value + c
    // y - x <= -value & k - y <= c --> k - x <= -value + c
    for (tchecker::clock_id_t k = 0; k < dim; ++k) {
      if (x == k)
        continue;
      
      DBM2(x,k) = tchecker::dbm::add(DBM2(y,k), value);
      DBM2(k,x) = tchecker::dbm::add(DBM2(k,y), -value);
    }
    
    tchecker::dbm::reset(dbm, dim, x, y, value);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "reset clock to sum (2nd)" ) {
    tchecker::clock_id_t const x = 3;
    tchecker::clock_id_t const y = 1;
    tchecker::integer_t const value = 8;
    
    // x - y <= value & y - k <= c --> x - k <= value + c
    // y - x <= -value & k - y <= c --> k - x <= -value + c
    for (tchecker::clock_id_t k = 0; k < dim; ++k) {
      if (x == k)
        continue;
      
      DBM2(x,k) = tchecker::dbm::add(DBM2(y,k), value);
      DBM2(k,x) = tchecker::dbm::add(DBM2(k,y), -value);
    }
    
    tchecker::dbm::reset(dbm, dim, x, y, value);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "reset clock to sum (3nd)" ) {
    tchecker::clock_id_t const x = 1;
    tchecker::clock_id_t const y = 2;
    tchecker::integer_t const value = 190;
    
    // x - y <= value & y - k <= c --> x - k <= value + c
    // y - x <= -value & k - y <= c --> k - x <= -value + c
    for (tchecker::clock_id_t k = 0; k < dim; ++k) {
      if (x == k)
        continue;
      
      DBM2(x,k) = tchecker::dbm::add(DBM2(y,k), value);
      DBM2(k,x) = tchecker::dbm::add(DBM2(k,y), -value);
    }
    
    tchecker::dbm::reset(dbm, dim, x, y, value);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
}




TEST_CASE( "DBM open_up (delay)", "[dbm]" ) {
  
  SECTION( "open_up on positive zone has no effect" ) {
    tchecker::clock_id_t const dim = 5;
    tchecker::dbm::db_t dbm[dim * dim];
    
    tchecker::dbm::universal_positive(dbm, dim);
    
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    
    tchecker::dbm::open_up(dbm, dim);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "open_up on zero zone" ) {
    tchecker::clock_id_t const dim = 4;
    tchecker::dbm::db_t dbm[dim * dim];
    tchecker::dbm::zero(dbm, dim);
    
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    for (tchecker::clock_id_t i = 1; i < dim; ++i)
      DBM2(i,0) = tchecker::dbm::LT_INFINITY;
    
    tchecker::dbm::open_up(dbm, dim);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "open_up on some DBM" ) {
    tchecker::clock_id_t const dim = 3;
    tchecker::dbm::db_t dbm[dim * dim];
    tchecker::dbm::universal_positive(dbm, dim);
    for (tchecker::clock_id_t i = 1; i < dim; ++i) {
      DBM(0,i) = tchecker::dbm::db(tchecker::dbm::LT, -i);
      DBM(i,0) = tchecker::dbm::db(tchecker::dbm::LE, i+1);
    }
    tchecker::dbm::tighten(dbm, dim);
    
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    for (tchecker::clock_id_t i = 1; i < dim; ++i)
      DBM2(i,0) = tchecker::dbm::LT_INFINITY;
    
    tchecker::dbm::open_up(dbm, dim);
    
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
}




TEST_CASE( "DBM intersection", "[dbm]" ) {
  
  SECTION( "intersection with universal zone has no effect" ) {
    tchecker::clock_id_t const dim = 3;
    
    tchecker::dbm::db_t dbm1[dim * dim];
    tchecker::dbm::universal_positive(dbm1, dim);
    // 0 <= x1 < 5 & 2 <= x2 <= 3
    DBM1(0,1) = tchecker::dbm::LE_ZERO;
    DBM1(0,2) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM1(1,0) = tchecker::dbm::db(tchecker::dbm::LT, 5);
    DBM1(2,0) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    tchecker::dbm::tighten(dbm1, dim);
    
    tchecker::dbm::db_t dbm2[dim * dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    
    tchecker::dbm::db_t dbm[dim * dim];
    REQUIRE(tchecker::dbm::intersection(dbm, dbm1, dbm2, dim) == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm1, dim));
  }
  
  SECTION( "non-empty intersection" ) {
    tchecker::clock_id_t const dim = 4;
    
    tchecker::dbm::db_t dbm1[dim * dim];
    tchecker::dbm::universal_positive(dbm1, dim);
    // 0 <= x1 < 5 & 2 <= x2 <= 3 & 1 < x3 <= 4
    DBM1(0,1) = tchecker::dbm::LE_ZERO;
    DBM1(0,2) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM1(0,3) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    DBM1(1,0) = tchecker::dbm::db(tchecker::dbm::LT, 5);
    DBM1(2,0) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    DBM1(3,0) = tchecker::dbm::db(tchecker::dbm::LE, 4);
    tchecker::dbm::tighten(dbm1, dim);
    
    tchecker::dbm::db_t dbm2[dim * dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    // 1 <= x1 & 1 < x1 - x2 < 2
    DBM2(0,1) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    DBM2(1,2) = tchecker::dbm::db(tchecker::dbm::LT, 2);
    DBM2(2,1) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::db_t dbm[dim * dim];
    REQUIRE(tchecker::dbm::intersection(dbm, dbm1, dbm2, dim) == tchecker::dbm::NON_EMPTY);
    REQUIRE(tchecker::dbm::is_tight(dbm, dim));
    
    // DBM should be:
    // <=0  <-3  <=-2  <-1
    // <5  <=0  <2  <4
    // <=3  <-1  <=0  <2
    // <=4  <1  <=2  <=0
    REQUIRE(DBM(0,0) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(0,1) == tchecker::dbm::db(tchecker::dbm::LT, -3));
    REQUIRE(DBM(0,2) == tchecker::dbm::db(tchecker::dbm::LE, -2));
    REQUIRE(DBM(0,3) == tchecker::dbm::db(tchecker::dbm::LT, -1));
    REQUIRE(DBM(1,0) == tchecker::dbm::db(tchecker::dbm::LT, 5));
    REQUIRE(DBM(1,1) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(1,2) == tchecker::dbm::db(tchecker::dbm::LT, 2));
    REQUIRE(DBM(1,3) == tchecker::dbm::db(tchecker::dbm::LT, 4));
    REQUIRE(DBM(2,0) == tchecker::dbm::db(tchecker::dbm::LE, 3));
    REQUIRE(DBM(2,1) == tchecker::dbm::db(tchecker::dbm::LT, -1));
    REQUIRE(DBM(2,2) == tchecker::dbm::LE_ZERO);
    REQUIRE(DBM(2,3) == tchecker::dbm::db(tchecker::dbm::LT, 2));
    REQUIRE(DBM(3,0) == tchecker::dbm::db(tchecker::dbm::LE, 4));
    REQUIRE(DBM(3,1) == tchecker::dbm::db(tchecker::dbm::LT, 1));
    REQUIRE(DBM(3,2) == tchecker::dbm::db(tchecker::dbm::LE, 2));
    REQUIRE(DBM(3,3) == tchecker::dbm::LE_ZERO);
  }
 
  SECTION( "empty intersection" ) {
    tchecker::clock_id_t const dim = 4;
    
    tchecker::dbm::db_t dbm1[dim * dim];
    tchecker::dbm::universal_positive(dbm1, dim);
    // 0 <= x1 < 5 & 2 <= x2 <= 3 & 1 < x3 <= 4
    DBM1(0,1) = tchecker::dbm::LE_ZERO;
    DBM1(0,2) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM1(0,3) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    DBM1(1,0) = tchecker::dbm::db(tchecker::dbm::LT, 5);
    DBM1(2,0) = tchecker::dbm::db(tchecker::dbm::LE, 3);
    DBM1(3,0) = tchecker::dbm::db(tchecker::dbm::LE, 4);
    tchecker::dbm::tighten(dbm1, dim);
    
    tchecker::dbm::db_t dbm2[dim * dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    // x1 - x2 > 4
    DBM2(2,1) = tchecker::dbm::db(tchecker::dbm::LT, -4);
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::db_t dbm[dim * dim];
    REQUIRE(tchecker::dbm::intersection(dbm, dbm1, dbm2, dim) == tchecker::dbm::EMPTY);
  }
}



TEST_CASE( "Checking tightness when the 0-row is modified", "[dbm]" ) {
  // inspired from AD94 amd a bug report submitted by Philipp Schlehuber
  
  tchecker::clock_id_t const dim = 3;
  
  // <=0  <-2  <=-1
  // <inf  <=0  <inf
  // <inf  <-1  <=0
  tchecker::dbm::db_t dbm[dim * dim];
  DBM(0,0) = tchecker::dbm::LE_ZERO;
  DBM(0,1) = tchecker::dbm::db(tchecker::dbm::LT, -2);
  DBM(0,2) = tchecker::dbm::db(tchecker::dbm::LE, -1);
  DBM(1,0) = tchecker::dbm::LT_INFINITY;
  DBM(1,1) = tchecker::dbm::LE_ZERO;
  DBM(1,2) = tchecker::dbm::LT_INFINITY;
  DBM(2,0) = tchecker::dbm::LT_INFINITY;
  DBM(2,1) = tchecker::dbm::db(tchecker::dbm::LT, -1);
  DBM(2,2) = tchecker::dbm::LE_ZERO;
  
  // clock bounds
  tchecker::integer_t M[dim] = {0, 1, 1};
  
  SECTION( "ExtraM" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,1) = tchecker::dbm::db(tchecker::dbm::LT, -M[1]);
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::extra_m(dbm, dim, M);
    
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "ExtraM+" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,1) = tchecker::dbm::db(tchecker::dbm::LT, -M[1]);
    DBM2(2,1) = tchecker::dbm::LT_INFINITY;
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::extra_m_plus(dbm, dim, M);
    
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "ExtraLUl" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,1) = tchecker::dbm::db(tchecker::dbm::LT, -M[1]);
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::extra_lu(dbm, dim, M, M);
    
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "ExtraLU+l" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,1) = tchecker::dbm::db(tchecker::dbm::LT, -M[1]);
    DBM2(2,1) = tchecker::dbm::LT_INFINITY;
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::extra_lu_plus(dbm, dim, M, M);
    
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
}



TEST_CASE( "Extrapolations from STTT06", "[dbm]" ) {
  // inspired from "Lower and upper bounds in zone-based abstractions of timed automata",
  // Behrmann, Bouyer, Larsen and Pelanek. Int. J. STTT, 2006.
  
  tchecker::clock_id_t const dim = 3;
  tchecker::clock_id_t const x = 1;
  tchecker::clock_id_t const y = 2;
  
  // x >= 2 & x < 5 & x <= y & y - x < 3
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  DBM(0,x) = tchecker::dbm::db(tchecker::dbm::LE, -2);
  DBM(x,0) = tchecker::dbm::db(tchecker::dbm::LT, 5);
  DBM(x,y) = tchecker::dbm::db(tchecker::dbm::LE, 0);
  DBM(y,x) = tchecker::dbm::db(tchecker::dbm::LT, 3);
  tchecker::dbm::tighten(dbm, dim);
  
  // clock bounds
  tchecker::integer_t m[dim] = {0, 1, 10};
  tchecker::integer_t l[dim] = {0, 1, 1}, u[dim] = {0, 1, 10};
  
  SECTION( "ExtraM" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,x) = tchecker::dbm::db(tchecker::dbm::LT, -m[x]);
    DBM2(x,0) = tchecker::dbm::LT_INFINITY;
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::extra_m(dbm, dim, m);
    
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "ExtraM+" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,x) = tchecker::dbm::db(tchecker::dbm::LT, -m[x]);
    DBM2(x,0) = tchecker::dbm::LT_INFINITY;
    DBM2(x,y) = tchecker::dbm::LT_INFINITY;
    DBM2(y,x) = tchecker::dbm::LT_INFINITY;
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::extra_m_plus(dbm, dim, m);
    
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "ExtraLU" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,x) = tchecker::dbm::db(tchecker::dbm::LT, -u[x]);
    DBM2(x,0) = tchecker::dbm::LT_INFINITY;
    DBM2(y,0) = tchecker::dbm::LT_INFINITY;
    DBM2(y,x) = tchecker::dbm::LT_INFINITY;
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::extra_lu(dbm, dim, l, u);
    
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "ExtraLU+" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,x) = tchecker::dbm::db(tchecker::dbm::LT, -u[x]);
    DBM2(x,0) = tchecker::dbm::LT_INFINITY;
    DBM2(y,0) = tchecker::dbm::LT_INFINITY;
    DBM2(x,y) = tchecker::dbm::LT_INFINITY;
    DBM2(y,x) = tchecker::dbm::LT_INFINITY;
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::extra_lu_plus(dbm, dim, l, u);
    
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "Inclusion between extrapolations and abstractions" ) {
    tchecker::dbm::db_t dbm_m[dim * dim];
    memcpy(dbm_m, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    tchecker::dbm::extra_m(dbm_m, dim, m);
    
    tchecker::dbm::db_t dbm_m_plus[dim * dim];
    memcpy(dbm_m_plus, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    tchecker::dbm::extra_m_plus(dbm_m_plus, dim, m);
    
    tchecker::dbm::db_t dbm_lu[dim * dim];
    memcpy(dbm_lu, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    tchecker::dbm::extra_lu(dbm_lu, dim, l, u);
    
    tchecker::dbm::db_t dbm_lu_plus[dim * dim];
    memcpy(dbm_lu_plus, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    tchecker::dbm::extra_lu_plus(dbm_lu_plus, dim, l, u);
    
    REQUIRE(tchecker::dbm::is_le(dbm, dbm_m, dim));
    REQUIRE_FALSE(tchecker::dbm::is_le(dbm_m, dbm, dim));
    REQUIRE(tchecker::dbm::is_le(dbm_m, dbm_m_plus, dim));
    REQUIRE(tchecker::dbm::is_le(dbm_m, dbm_lu, dim));
    REQUIRE_FALSE(tchecker::dbm::is_le(dbm_m_plus, dbm_m, dim));
    REQUIRE_FALSE(tchecker::dbm::is_le(dbm_lu, dbm_m, dim));
    REQUIRE(tchecker::dbm::is_le(dbm_m_plus, dbm_lu_plus, dim));
    REQUIRE(tchecker::dbm::is_le(dbm_lu, dbm_lu_plus, dim));
    REQUIRE_FALSE(tchecker::dbm::is_le(dbm_lu_plus, dbm_m_plus, dim));
    REQUIRE_FALSE(tchecker::dbm::is_le(dbm_lu_plus, dbm_lu, dim));
    
    REQUIRE(tchecker::dbm::is_am_le(dbm, dbm, dim, m));
    REQUIRE(tchecker::dbm::is_am_le(dbm_m, dbm, dim, m));
    
    REQUIRE(tchecker::dbm::is_alu_le(dbm, dbm, dim, l, u));
    REQUIRE(tchecker::dbm::is_alu_le(dbm_m, dbm, dim, l, u));
    REQUIRE(tchecker::dbm::is_alu_le(dbm_m_plus, dbm, dim, l, u));
    REQUIRE(tchecker::dbm::is_alu_le(dbm_lu, dbm, dim, l, u));
    REQUIRE(tchecker::dbm::is_alu_le(dbm_lu_plus, dbm, dim, l, u));
  }
  
}




TEST_CASE( "Extrapolations boundary case on M/U bounds", "[dbm]" ) {
  // Checks situations where the new bound is <-m[j] or <-u[j]
  
  tchecker::clock_id_t const dim = 3;
  tchecker::clock_id_t const x = 1;
  tchecker::clock_id_t const y = 2;
  
  // y >= 2 & x < 5 & x <= y
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  DBM(0,y) = tchecker::dbm::db(tchecker::dbm::LE, -2);
  DBM(x,0) = tchecker::dbm::db(tchecker::dbm::LT, 5);
  DBM(x,y) = tchecker::dbm::db(tchecker::dbm::LE, 0);
  tchecker::dbm::tighten(dbm, dim);
  
  // clock bounds
  tchecker::integer_t m[dim] = {0, 2, -tchecker::dbm::INF_VALUE};
  tchecker::integer_t l[dim] = {0, 2, -tchecker::dbm::INF_VALUE},
  u[dim] = {0, 1, -tchecker::dbm::INF_VALUE};
  
  SECTION( "ExtraM" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,y) = tchecker::dbm::LE_ZERO;     // <=0 (positive)
    DBM2(x,0) = tchecker::dbm::LT_INFINITY;
    DBM2(x,y) = tchecker::dbm::LT_INFINITY; // <-m[y]
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::extra_m(dbm, dim, m);
    
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "ExtraM+" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,y) = tchecker::dbm::LE_ZERO;     // <=0 (positive)
    DBM2(x,0) = tchecker::dbm::LT_INFINITY;
    DBM2(x,y) = tchecker::dbm::LT_INFINITY; // <-m[y]
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::extra_m_plus(dbm, dim, m);
    
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "ExtraLU" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,y) = tchecker::dbm::LE_ZERO;     // <=0 (positive)
    DBM2(x,0) = tchecker::dbm::LT_INFINITY;
    DBM2(x,y) = tchecker::dbm::LT_INFINITY; // <-u[y]
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::extra_lu(dbm, dim, l, u);
    
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
  
  SECTION( "ExtraLU+" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    memcpy(dbm2, dbm, dim * dim * sizeof(tchecker::dbm::db_t));
    DBM2(0,y) = tchecker::dbm::LE_ZERO;     // <=0 (positive)
    DBM2(x,0) = tchecker::dbm::LT_INFINITY;
    DBM2(x,y) = tchecker::dbm::LT_INFINITY; // <-u[y]
    tchecker::dbm::tighten(dbm2, dim);
    
    tchecker::dbm::extra_lu_plus(dbm, dim, l ,u);
    
    REQUIRE(tchecker::dbm::is_equal(dbm, dbm2, dim));
  }
}




TEST_CASE( "Zone inclusion w.r.t. abstraction aM (1)", "[dbm]" ) {
  tchecker::clock_id_t const dim = 3;
  tchecker::clock_id_t const x = 1;
  tchecker::clock_id_t const y = 2;
  
  // y-x<=0
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  DBM(y,x) = tchecker::dbm::LE_ZERO;
  tchecker::dbm::tighten(dbm, dim);
  
  // aM(dbm) is `dbm \cup x>i` for bound mi
  tchecker::integer_t m1[dim] = {0, 1, 1}, m2[dim] = {0, 2, 2}, m3[dim] = {0, 3, 3};
  
  SECTION( "aM(dbm) contains dbm" ) {
    REQUIRE(tchecker::dbm::is_am_le(dbm, dbm, dim, m1));
    REQUIRE(tchecker::dbm::is_am_le(dbm, dbm, dim, m2));
    REQUIRE(tchecker::dbm::is_am_le(dbm, dbm, dim, m3));
  }
  
  SECTION( "containment of x-y>1" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(y,x) = tchecker::dbm::db(tchecker::dbm::LT, -1);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m1));
    REQUIRE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m2));
    REQUIRE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m3));
  }
  
  SECTION( "containment of x>2" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0,x) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m1));
    REQUIRE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m2));
    REQUIRE_FALSE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m3));
  }
  
  SECTION( "containment of x>=3" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0,x) = tchecker::dbm::db(tchecker::dbm::LE, -3);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m1));
    REQUIRE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m2));
    REQUIRE_FALSE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m3));
  }
  
  SECTION( "containment of y==1" ) {
    tchecker::dbm::db_t dbm2[dim * dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0,y) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    DBM2(y,0) = tchecker::dbm::db(tchecker::dbm::LE, 1);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE_FALSE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m1));
    REQUIRE_FALSE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m2));
    REQUIRE_FALSE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m3));
  }
  
  SECTION( "containment w.r.t. positive zone" ) {
    tchecker::dbm::db_t dbm_positive[dim*dim];
    tchecker::dbm::universal_positive(dbm_positive, dim);
    
    REQUIRE(tchecker::dbm::is_am_le(dbm, dbm_positive, dim, m1));
    
    tchecker::integer_t m_inf[dim] = {0, -tchecker::dbm::INF_VALUE, -tchecker::dbm::INF_VALUE};
    
    REQUIRE(tchecker::dbm::is_am_le(dbm, dbm_positive, dim, m_inf));
    REQUIRE(tchecker::dbm::is_am_le(dbm_positive, dbm, dim, m_inf));
  }
}




TEST_CASE( "Zone inclusion w.r.t. abstraction aM (2)", "[dbm]" ) {
  tchecker::clock_id_t const dim = 3;
  tchecker::clock_id_t const x = 1;
  tchecker::clock_id_t const y = 2;
  
  // 1 <= x <= 3 & 1 <= y <= 3
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  DBM(0,x) = tchecker::dbm::db(tchecker::dbm::LE, -1);
  DBM(x,0) = tchecker::dbm::db(tchecker::dbm::LE, 3);
  DBM(0,y) = tchecker::dbm::db(tchecker::dbm::LE, -1);
  DBM(y,0) = tchecker::dbm::db(tchecker::dbm::LE, 3);
  tchecker::dbm::tighten(dbm, dim);
  
  tchecker::integer_t m[dim] = {0, 2, 2};
  
  SECTION( "1 <= x & 1 <= y is aM contained in dbm" ) {
    tchecker::dbm::db_t dbm2[dim*dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0,x) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    DBM2(0,y) = tchecker::dbm::db(tchecker::dbm::LE, -1);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m));
  }
  
  SECTION( "x < 1 is not aM contained in dbm" ) {
    tchecker::dbm::db_t dbm2[dim*dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(x,0) = tchecker::dbm::db(tchecker::dbm::LT, 1);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE_FALSE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m));
  }
  
  SECTION( "y < 1 is not aM contained in dbm" ) {
    tchecker::dbm::db_t dbm2[dim*dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(y,0) = tchecker::dbm::db(tchecker::dbm::LT, 1);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE_FALSE(tchecker::dbm::is_am_le(dbm2, dbm, dim, m));
  }
}




TEST_CASE( "Zone inclusion w.r.t. abstraction aLU from LICS12", "[dbm]" ) {
  tchecker::clock_id_t const dim = 3;
  tchecker::clock_id_t const x = 1;
  tchecker::clock_id_t const y = 2;
  
  // 0 <= x - y <= 3 (Fig.1 in "Better abstraction for timed automata", Herbreteau, Srivathsan and Walukiewicz, LICS, 2012
  tchecker::dbm::db_t dbm[dim * dim];
  tchecker::dbm::universal_positive(dbm, dim);
  DBM(x,y) = tchecker::dbm::db(tchecker::dbm::LE, 3);
  DBM(y,x) = tchecker::dbm::db(tchecker::dbm::LE, 0);
  tchecker::dbm::tighten(dbm, dim);
  
  tchecker::integer_t l[dim] = {0, 1, 2}, u[dim] = {0, 4 ,4};
  
  SECTION( "aLU(dbm) contains dbm" ) {
    REQUIRE(tchecker::dbm::is_alu_le(dbm, dbm, dim, l, u));
  }
  
  SECTION( "aLU(0 <= x - y <= 3) contains 3 <= x" ) {
    tchecker::dbm::db_t dbm2[dim*dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0,x) = tchecker::dbm::db(tchecker::dbm::LE, -3);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE(tchecker::dbm::is_alu_le(dbm2, dbm, dim, l, u));
  }
  
  SECTION( "aLU(0 <= x - y <= 3) contains 2 < x & 2 <= y" ) {
    tchecker::dbm::db_t dbm2[dim*dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0,x) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    DBM2(0,y) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE(tchecker::dbm::is_alu_le(dbm2, dbm, dim, l, u));
  }
  
  SECTION( "aLU(0 <= x - y <= 3) contains x-y > 3" ) {
    tchecker::dbm::db_t dbm2[dim*dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(y,x) = tchecker::dbm::db(tchecker::dbm::LT, -3);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE(tchecker::dbm::is_alu_le(dbm2, dbm, dim, l, u));
  }
  
  SECTION( "aLU(0 <= x - y <= 3) does not contain x == 2 & y > 2" ) {
    tchecker::dbm::db_t dbm2[dim*dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0,x) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    DBM2(x,0) = tchecker::dbm::db(tchecker::dbm::LE, 2);
    DBM2(0,y) = tchecker::dbm::db(tchecker::dbm::LT, -2);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE_FALSE(tchecker::dbm::is_alu_le(dbm2, dbm, dim, l, u));
  }
  
  SECTION( "aLU(0 <= x - y <= 3) does not contain y >= 2" ) {
    tchecker::dbm::db_t dbm2[dim*dim];
    tchecker::dbm::universal_positive(dbm2, dim);
    DBM2(0,y) = tchecker::dbm::db(tchecker::dbm::LE, -2);
    tchecker::dbm::tighten(dbm2, dim);
    
    REQUIRE_FALSE(tchecker::dbm::is_alu_le(dbm2, dbm, dim, l, u));
  }
  
  SECTION( "aLU(0 <= x - y <= 3) containment w.r.t. positive zone" ) {
    tchecker::dbm::db_t dbm_positive[dim*dim];
    tchecker::dbm::universal_positive(dbm_positive, dim);
    
    REQUIRE(tchecker::dbm::is_alu_le(dbm, dbm_positive, dim, l, u));
    REQUIRE_FALSE(tchecker::dbm::is_alu_le(dbm_positive, dbm, dim, l, u));
    
    tchecker::integer_t l_inf[dim] = {0, -tchecker::dbm::INF_VALUE, -tchecker::dbm::INF_VALUE};
    tchecker::integer_t u_inf[dim] = {0, -tchecker::dbm::INF_VALUE, -tchecker::dbm::INF_VALUE};
    
    REQUIRE(tchecker::dbm::is_alu_le(dbm_positive, dbm, dim, l_inf, u_inf));
  }
}
