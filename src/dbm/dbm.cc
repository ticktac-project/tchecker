/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <cassert>

#if BOOST_VERSION <= 106600
# include <boost/functional/hash.hpp>
#else
# include <boost/container_hash/hash.hpp>
#endif

#include "tchecker/dbm/dbm.hh"
#include "tchecker/utils/ordering.hh"

namespace tchecker {
  
  namespace dbm {
    
#define DBM(i,j)          dbm[(i)*dim+(j)]
#define DBM1(i,j)         dbm1[(i)*dim+(j)]
#define DBM2(i,j)         dbm2[(i)*dim+(j)]
    

    void universal(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      for (tchecker::clock_id_t i = 0; i < dim; ++i) {
        for (tchecker::clock_id_t j = 0; j < dim; ++j)
          DBM(i,j) = tchecker::dbm::LT_INFINITY;
        DBM(i,i) = tchecker::dbm::LE_ZERO;
      }
      
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
    }
    
    
    void universal_positive(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      for (tchecker::clock_id_t i = 0; i < dim; ++i)
        for (tchecker::clock_id_t j = 0; j < dim; ++j)
          DBM(i,j) = ((i == j) || (i == 0) ? tchecker::dbm::LE_ZERO : tchecker::dbm::LT_INFINITY);
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_positive(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
    }
    
    
    void empty(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      tchecker::dbm::universal(dbm, dim);
      DBM(0,0) = tchecker::dbm::LT_ZERO;
    }
    
    
    void zero(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      for (tchecker::clock_id_t i = 0; i < dim; ++i)
        for (tchecker::clock_id_t j = 0; j < dim; ++j)
          DBM(i,j) = tchecker::dbm::LE_ZERO;
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
    }
    
    
    bool is_consistent(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      for (tchecker::clock_id_t i = 0; i < dim; ++i)
        if (DBM(i,i) != tchecker::dbm::LE_ZERO)
          return false;
      return true;
    }
    
    
    bool is_empty_0(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      return (DBM(0,0) < tchecker::dbm::LE_ZERO);
    }
    
    
    bool is_universal(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      // <inf everywhere except <=0 on diagonal
      for (tchecker::clock_id_t i = 0; i < dim; ++i) {
        if (DBM(i,i) != tchecker::dbm::LE_ZERO)
          return false;
        
        for (tchecker::clock_id_t j = 0; j < dim; ++j) {
          if ((i != j) && (DBM(i,j) != tchecker::dbm::LT_INFINITY))
            return false;
        }
      }
      return true;
    }
    
    
    bool is_positive(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      for (tchecker::clock_id_t x = 0; x < dim; ++x)
        if (DBM(0,x) > tchecker::dbm::LE_ZERO)
          return false;
      return true;
    }
    
    
    bool is_universal_positive(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      for (tchecker::clock_id_t y = 0; y < dim; ++y)
        if (DBM(0,y) != tchecker::dbm::LE_ZERO)
          return false;
      for (tchecker::clock_id_t x = 1; x < dim; ++x) {
        if ((DBM(x,0) != tchecker::dbm::LT_INFINITY) ||
            (DBM(x,x) != tchecker::dbm::LE_ZERO))
          return false;
        for (tchecker::clock_id_t y = x+1; y < dim; ++y)
          if ((DBM(x,y) != tchecker::dbm::LT_INFINITY) ||
              (DBM(y,x) != tchecker::dbm::LT_INFINITY))
            return false;
      }
      return true;
    }
    
    
    bool is_tight(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      for (tchecker::clock_id_t i = 0; i < dim; ++i)
        for (tchecker::clock_id_t j = 0; j < dim; ++j)
          for (tchecker::clock_id_t k = 0; k < dim; ++k)
            if (tchecker::dbm::sum(DBM(i,k), DBM(k,j)) < DBM(i,j))
              return false;
      return true;
    }
    
    
    enum tchecker::dbm::status_t tighten(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_consistent(dbm, dim));
      for (tchecker::clock_id_t k = 0; k < dim; ++k) {
        for (tchecker::clock_id_t i = 0 ; i < dim; ++i) {
          if ((i == k) || (DBM(i,k) == tchecker::dbm::LT_INFINITY)) // optimization
            continue;
          for (tchecker::clock_id_t j = 0; j < dim; ++j)
            DBM(i,j) = tchecker::dbm::min(tchecker::dbm::sum(DBM(i,k), DBM(k,j)), DBM(i,j));
          if (DBM(i,i) < tchecker::dbm::LE_ZERO) {
            DBM(0,0) = tchecker::dbm::LT_ZERO;
            return tchecker::dbm::EMPTY;
          }
        }
      }
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
      return tchecker::dbm::NON_EMPTY;
    }
    
    
    enum tchecker::dbm::status_t tighten(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim,
                                         tchecker::clock_id_t x, tchecker::clock_id_t y)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_consistent(dbm, dim));
      
      if (DBM(x,y) == tchecker::dbm::LT_INFINITY)
        return tchecker::dbm::MAY_BE_EMPTY;
      
      for (tchecker::clock_id_t i = 0; i < dim; ++i) {
        // tighten i->y w.r.t. i->x->y
        if (i != x) {
          tchecker::dbm::db_t db_ixy = tchecker::dbm::sum(DBM(i,x), DBM(x,y));
          if (db_ixy >= DBM(i,y))
            continue;
          DBM(i,y) = db_ixy;
        }
        
        // tighten i->j w.r.t. i->y->j
        for (tchecker::clock_id_t j = 0; j < dim; ++j)
          DBM(i,j) = tchecker::dbm::min(DBM(i,j), tchecker::dbm::sum(DBM(i,y), DBM(y,j)));
        
        if (DBM(i,i) < tchecker::dbm::LE_ZERO) {
          DBM(0,0) = tchecker::dbm::LT_ZERO;
          return tchecker::dbm::EMPTY;
        }
      }
      
      return tchecker::dbm::MAY_BE_EMPTY;
    }
    
    
    enum tchecker::dbm::status_t
    constrain(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x,
              tchecker::clock_id_t y, tchecker::dbm::comparator_t cmp, tchecker::integer_t value)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
      assert(x < dim);
      assert(y < dim);
      
      tchecker::dbm::db_t db = tchecker::dbm::db(cmp, value);
      if (db >= DBM(x,y))
        return tchecker::dbm::NON_EMPTY;
      
      DBM(x,y) = db;
      
      auto res = tchecker::dbm::tighten(dbm, dim, x, y);
      
      if (res == tchecker::dbm::MAY_BE_EMPTY)
        res = tchecker::dbm::NON_EMPTY;  // since dbm was tight before
      
      assert((res == tchecker::dbm::EMPTY) || tchecker::dbm::is_consistent(dbm, dim));
      assert((res == tchecker::dbm::EMPTY) || tchecker::dbm::is_tight(dbm, dim));
      
      return res;
    }
    
    
    bool is_equal(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim)
    {
      assert(dbm1 != nullptr);
      assert(dbm2 != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_tight(dbm1, dim));
      assert(tchecker::dbm::is_tight(dbm2, dim));
      
      for (tchecker::clock_id_t i = 0; i < dim; ++i)
        for (tchecker::clock_id_t j = 0; j < dim; ++j)
          if (DBM1(i,j) != DBM2(i,j))
            return false;
      return true;
    }
    
    
    bool is_le(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim)
    {
      assert(dbm1 != nullptr);
      assert(dbm2 != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_tight(dbm1, dim));
      assert(tchecker::dbm::is_tight(dbm2, dim));
      
      for (tchecker::clock_id_t i = 0; i < dim; ++i)
        for (tchecker::clock_id_t j = 0; j < dim; ++j)
          if (DBM1(i,j) > DBM2(i,j))
            return false;
      return true;
    }
    
    
    void reset(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x,
               tchecker::clock_id_t y, tchecker::integer_t value)
    {
      if (y == 0)
        reset_to_value(dbm, dim, x, value);
      else if (value == 0)
        reset_to_clock(dbm, dim, x, y);
      else
        reset_to_sum(dbm, dim, x, y, value);
    }
    
    
    void reset_to_value(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x,
                        tchecker::integer_t value)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
      assert(x < dim);
      assert(0 <= value);
      
      // set x == value
      DBM(x,0) = tchecker::dbm::db(tchecker::dbm::LE, value);
      DBM(0,x) = tchecker::dbm::db(tchecker::dbm::LE, -value);
      
      // tighten: x->y is set to x->0->y and y->x to y->0->x for all y!=0
      for (tchecker::clock_id_t y = 1; y < dim; ++y) {
        DBM(x,y) = tchecker::dbm::sum(DBM(x,0), DBM(0,y));
        DBM(y,x) = tchecker::dbm::sum(DBM(y,0), DBM(0,x));
      }
      
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
    }
    
    
    void reset_to_clock(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x,
                        tchecker::clock_id_t y)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
      assert(x < dim);
      assert(0 < y);
      assert(y < dim);
      
      // x is identified to y w.r.t. all clocks z
      for (tchecker::clock_id_t z = 0; z < dim; ++z) {
        DBM(x,z) = DBM(y,z);
        DBM(z,x) = DBM(z,y);
      }
      DBM(x,x) = tchecker::dbm::LE_ZERO; // cheaper than testing in loop
      
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
    }
    
    
    void reset_to_sum(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x,
                      tchecker::clock_id_t y, tchecker::integer_t value)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
      assert(x < dim);
      assert(y < dim);
      assert(0 <= value);
      
      // NB: this is a reset not a constraint. Hence, resetting x:=y+value
      // only impacts difference bounds involving x (i.e. x-z and z-x
      // for all z including y and 0). Other difference bounds u-v for
      // any u,v distinct from x are not touched since the values of u
      // and v are not modified when x is reset to y+value
      
      // If x == y:
      // x' - x <= v & x - z <= d & z = z' --> x' - z' <= d+v
      // x - x' <= -v & z - x <= d & z = z' --> z' - x' <= d-v
      
      // If x != y:
      // x - y <= v & y - z <= d --> x - z <= d+v
      // y - x <= -v & z - y <= d --> z - x <= d-v
      for (tchecker::clock_id_t z = 0; z < dim; ++z) {
        DBM(x,z) = tchecker::dbm::add(DBM(y,z), value);
        DBM(z,x) = tchecker::dbm::add(DBM(z,y), -value);
      }
      DBM(x,x) = tchecker::dbm::LE_ZERO; // cheaper than testing in loop
      
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
    }
    
    
    void open_up(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
      
      for (tchecker::clock_id_t i = 1; i < dim; ++i)
        DBM(i,0) = tchecker::dbm::LT_INFINITY;
      
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
    }
    
    
    enum tchecker::dbm::status_t intersection(tchecker::dbm::db_t * dbm,
                                              tchecker::dbm::db_t const * dbm1,
                                              tchecker::dbm::db_t const * dbm2,
                                              tchecker::clock_id_t dim)
    {
      assert(dim >= 1);
      assert(dbm != nullptr);
      assert(dbm1 != nullptr);
      assert(dbm2 != nullptr);
      assert(tchecker::dbm::is_consistent(dbm1, dim));
      assert(tchecker::dbm::is_consistent(dbm2, dim));
      assert(tchecker::dbm::is_tight(dbm1, dim));
      assert(tchecker::dbm::is_tight(dbm2, dim));
      
      for (tchecker::clock_id_t i = 0; i < dim; ++i)
        for (tchecker::clock_id_t j = 0; j < dim; ++j)
          DBM(i,j) = tchecker::dbm::min(DBM1(i,j), DBM2(i,j));
      
      return tchecker::dbm::tighten(dbm, dim);
    }
    
    
    void extra_m(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t const * m)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_positive(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
      assert(m[0] == 0);
      
      bool modified = false;
      
      // let DBM(i,j) be (#,cij)
      // DBM(i,j) becomes  <inf    if  cij > m[i]
      //                   <-m[j]  if -cij > m[j]
      //          unchanged otherwise
      
      // i=0 (first row), only the second case applies
      for (tchecker::clock_id_t j = 1; j < dim; ++j) {
        assert(m[j] < tchecker::dbm::INF_VALUE);
        if (DBM(0,j) == tchecker::dbm::LE_ZERO)
          continue;
        tchecker::integer_t c0j = tchecker::dbm::value(DBM(0,j));
        if (-c0j > m[j]) {
          DBM(0,j) = (m[j] == -tchecker::dbm::INF_VALUE ? tchecker::dbm::LE_ZERO : tchecker::dbm::db(tchecker::dbm::LT, -m[j]));
          modified = true;
        }
      }
      
      // i>0, both cases apply
      for (tchecker::clock_id_t i = 1; i < dim; ++i) {
        assert(m[i] < tchecker::dbm::INF_VALUE);
        
        for (tchecker::clock_id_t j = 0; j < dim; ++j) {
          assert(m[j] < tchecker::dbm::INF_VALUE);
          
          if (i == j)
            continue;
          if (DBM(i,j) == tchecker::dbm::LT_INFINITY)
            continue;

          tchecker::integer_t cij = tchecker::dbm::value(DBM(i,j));
          if (cij > m[i]) {
            DBM(i,j) = tchecker::dbm::LT_INFINITY;
            modified = true;
          }
          else if (-cij > m[j]) {
            DBM(i,j) = (m[j] == -tchecker::dbm::INF_VALUE ? tchecker::dbm::LT_INFINITY
                        : tchecker::dbm::db(tchecker::dbm::LT, -m[j]));
            modified = true;
          }
        }
      }
      
      if (modified)
        tchecker::dbm::tighten(dbm, dim);
      
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_positive(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
    }
    
    
    void extra_m_plus(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t const * m)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_positive(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
      assert(m[0] == 0);
      
      bool modified = false;
      
      // let DBM(i,j) be (#,cij)
      // DBM(i,j) becomes  <inf    if  cij > m[i]
      //                   <inf    if -c0i > m[i]
      //                   <inf    if -c0j > m[j], i != 0
      //                   <-m[j]  if -cij > m[j], i  = 0
      //          unchanged otherwise
      
      // NB: the first line (i.e. 0-line) must be modified last to keep
      // c0i and c0j intact for 2nd and 3rd cases
      
      // i > 0, all cases except the 4th apply
      for (tchecker::clock_id_t i = 1; i < dim; ++i) {
        assert(m[i] < tchecker::dbm::INF_VALUE);

        tchecker::integer_t c0i = tchecker::dbm::value(DBM(0,i));
        
        if (-c0i > m[i]) { // 2nd case
          for (tchecker::clock_id_t j = 0; j < dim; ++j) {
            if (i == j)
              continue;
            if (DBM(i,j) == tchecker::dbm::LT_INFINITY)
              continue;
            
            DBM(i,j) = tchecker::dbm::LT_INFINITY;
            modified = true;
          }
        }
        else { // 1st and 3rd cases apply
          for (tchecker::clock_id_t j = 0; j < dim; ++j) {
            assert(m[j] < tchecker::dbm::INF_VALUE);
            
            if (i == j)
              continue;
            if (DBM(i,j) == tchecker::dbm::LT_INFINITY)
              continue;

            tchecker::integer_t c0j = tchecker::dbm::value(DBM(0,j));
            tchecker::integer_t cij = tchecker::dbm::value(DBM(i,j));
            
            if ((cij > m[i]) || (-c0j > m[j])) {
              DBM(i,j) = tchecker::dbm::LT_INFINITY;
              modified = true;
            }
          }
        }
      }
      
      // i = 0, only the 4th case apply
      assert(m[0] < tchecker::dbm::INF_VALUE);
      for (tchecker::clock_id_t j = 1; j < dim; ++j) {
        assert(m[j] < tchecker::dbm::INF_VALUE);

        tchecker::integer_t c0j = tchecker::dbm::value(DBM(0,j));
        if (-c0j > m[j]) {
          DBM(0,j) = (m[j] == -tchecker::dbm::INF_VALUE ? tchecker::dbm::LE_ZERO : tchecker::dbm::db(tchecker::dbm::LT, -m[j]));
          modified = true;
        }
      }
      
      if (modified)
        tchecker::dbm::tighten(dbm, dim);
      
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_positive(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
    }
    
    
    void extra_lu(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t const * l,
                  tchecker::integer_t const * u)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_positive(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
      assert(l[0] == 0);
      assert(u[0] == 0);
      
      bool modified = false;
      
      // let DBM(i,j) be (#,cij)
      // DBM(i,j) becomes  <inf    if  cij > l[i]
      //                   <-u[j]  if -cij > u[j]
      //          unchanged otherwise
      
      // i=0 (first row), only second case applies
      for (tchecker::clock_id_t j = 1; j < dim; ++j) {
        assert(u[j] < tchecker::dbm::INF_VALUE);
        if (DBM(0,j) == tchecker::dbm::LE_ZERO)
          continue;
        tchecker::integer_t c0j = tchecker::dbm::value(DBM(0,j));
        if (-c0j > u[j]) {
          DBM(0,j) = (u[j] == -tchecker::dbm::INF_VALUE ? tchecker::dbm::LE_ZERO : tchecker::dbm::db(tchecker::dbm::LT, -u[j]));
          modified = true;
        }
      }
      
      // i>0, both cases apply
      for (tchecker::clock_id_t i = 1; i < dim; ++i) {
        assert(l[i] < tchecker::dbm::INF_VALUE);
        assert(u[i] < tchecker::dbm::INF_VALUE);
        
        for (tchecker::clock_id_t j = 0; j < dim; ++j) {
          assert(l[j] < tchecker::dbm::INF_VALUE);
          assert(u[j] < tchecker::dbm::INF_VALUE);
          
          if (i == j)
            continue;
          if (DBM(i,j) == tchecker::dbm::LT_INFINITY)
            continue;

          tchecker::integer_t cij = tchecker::dbm::value(DBM(i,j));
          if (cij > l[i]) {
            DBM(i,j) = tchecker::dbm::LT_INFINITY;
            modified = true;
          }
          else if (-cij > u[j]) {
            DBM(i,j) = (u[j] == -tchecker::dbm::INF_VALUE ? tchecker::dbm::LT_INFINITY
                        : tchecker::dbm::db(tchecker::dbm::LT, -u[j]));
            modified = true;
          }
        }
      }
      
      if (modified)
        tchecker::dbm::tighten(dbm, dim);
      
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_positive(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
    }
    
    
    void extra_lu_plus(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::integer_t const * l,
                       tchecker::integer_t const * u)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_positive(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
      assert(l[0] == 0);
      assert(u[0] == 0);
      
      bool modified = false;
      
      // let DBM(i,j) be (#,cij)
      // DBM(i,j) becomes  <inf    if  cij > l[i]
      //                   <inf    if -c0i > l[i]
      //                   <inf    if -c0j > u[j], i != 0
      //                   <-u[j]  if -c0j > u[j], i  = 0
      //          unchanged otherwise
      
      // NB: the first line (i.e. 0-line) must be modified last to keep
      // c0i and c0j intact for 2nd, 3rd and 4th cases
      
      // i > 0, all cases except the 4th apply
      for (tchecker::clock_id_t i = 1; i < dim; ++i) {
        assert(l[i] < tchecker::dbm::INF_VALUE);
        assert(u[i] < tchecker::dbm::INF_VALUE);

        tchecker::integer_t c0i = tchecker::dbm::value(DBM(0,i));
        
        if (-c0i > l[i]) { // 2nd case
          for (tchecker::clock_id_t j = 0; j < dim; ++j) {
            if (i == j)
              continue;
            if (DBM(i,j) == tchecker::dbm::LT_INFINITY)
              continue;
            
            DBM(i,j) = tchecker::dbm::LT_INFINITY;
            modified = true;
          }
        }
        else { // 1st and 3rd cases apply
          for (tchecker::clock_id_t j = 0; j < dim; ++j) {
            assert(l[j] < tchecker::dbm::INF_VALUE);
            assert(u[j] < tchecker::dbm::INF_VALUE);
            
            if (i == j)
              continue;
            if (DBM(i,j) == tchecker::dbm::LT_INFINITY)
              continue;

            tchecker::integer_t c0j = tchecker::dbm::value(DBM(0,j));
            tchecker::integer_t cij = tchecker::dbm::value(DBM(i,j));
            
            if ((cij > l[i]) || (-c0j > u[j])) {
              DBM(i,j) = tchecker::dbm::LT_INFINITY;
              modified = true;
            }
          }
        }
      }
      
      // i = 0, only the 4th case apply
      assert(l[0] < tchecker::dbm::INF_VALUE);
      assert(u[0] < tchecker::dbm::INF_VALUE);
      
      for (tchecker::clock_id_t j = 1; j < dim; ++j) {
        assert(l[j] < tchecker::dbm::INF_VALUE);
        assert(u[j] < tchecker::dbm::INF_VALUE);

        tchecker::integer_t c0j = tchecker::dbm::value(DBM(0,j));
        if (-c0j > u[j]) {
          DBM(0,j) = (u[j] == -tchecker::dbm::INF_VALUE ? tchecker::dbm::LE_ZERO : tchecker::dbm::db(tchecker::dbm::LT, -u[j]));
          modified = true;
        }
      }
      
      if (modified)
        tchecker::dbm::tighten(dbm, dim);
      
      assert(tchecker::dbm::is_consistent(dbm, dim));
      assert(tchecker::dbm::is_positive(dbm, dim));
      assert(tchecker::dbm::is_tight(dbm, dim));
    }
    
    
    bool is_alu_le(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim,
                   tchecker::integer_t const * l, tchecker::integer_t const * u)
    {
      assert(dbm1 != nullptr);
      assert(dbm2 != nullptr);
      assert(dim >= 1);
      assert(tchecker::dbm::is_consistent(dbm1, dim));
      assert(tchecker::dbm::is_consistent(dbm2, dim));
      assert(tchecker::dbm::is_positive(dbm1, dim));
      assert(tchecker::dbm::is_positive(dbm2, dim));
      assert(tchecker::dbm::is_tight(dbm1, dim));
      assert(tchecker::dbm::is_tight(dbm2, dim));
      assert(l[0] == 0);
      assert(u[0] == 0);
      
      // dbm1 not included in aLU(dbm2) if there is x and y s.t.
      //     dbm1[0x] >= (<= -u[x])
      // &&  dbm2[yx] < dbm1[yx]
      // &&  dbm2[yx] + (< -l[y]) < dbm1[0x]
      
      for (tchecker::clock_id_t x = 0; x < dim; ++x) {
        assert(u[x] < tchecker::dbm::INF_VALUE);
        
        // Skip x as 1st condition cannot be satisfied
        if (u[x] == - tchecker::dbm::INF_VALUE)
          continue;
        
        // Check 1st condition
        if (DBM1(0,x) < tchecker::dbm::db(tchecker::dbm::LE, -u[x]))
          continue;
        
        for (tchecker::clock_id_t y = 0; y < dim; ++y) {
          assert(l[y] < tchecker::dbm::INF_VALUE);
          
          if (x == y)
            continue;
          
          // Skip y as 3rd condition cannot be satisfied
          if (l[y] == - tchecker::dbm::INF_VALUE)
            continue;
          
          // Check 2nd and 3rd conditions
          if ((DBM2(y,x) < DBM1(y,x)) &&
              (tchecker::dbm::sum(DBM2(y,x), tchecker::dbm::db(tchecker::dbm::LT, -l[y])) < DBM1(0,x)))
            return false;
        }
      }
      
      return true;
    }
    
    
    bool is_am_le(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2,
                  tchecker::clock_id_t dim, tchecker::integer_t const * m)
    {
      return tchecker::dbm::is_alu_le(dbm1, dbm2, dim, m, m);
    }
    
    
    std::size_t hash(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
    {
      std::size_t seed = 0;
      for (tchecker::clock_id_t k = 0; k < dim*dim; ++k)
        boost::hash_combine(seed, tchecker::dbm::hash(dbm[k]));
      return seed;
    }
    
    
    std::ostream & output_matrix(std::ostream & os, tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim)
    {
      assert(dbm != nullptr);
      assert(dim >= 1);
      
      for (tchecker::clock_id_t i = 0; i < dim; ++i) {
        for (tchecker::clock_id_t j = 0; j < dim; ++j) {
          tchecker::dbm::output(os, DBM(i,j));
          os << '\t';
        }
        os << std::endl;
      }
      
      return os;
    }
    
    
    std::ostream & output(std::ostream & os, tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim,
                          std::function<std::string(tchecker::clock_id_t)> clock_name)
    {
      os << "(";
      
      // output: x # c (first row/column)
      for (tchecker::clock_id_t j = 1; j < dim; ++j) {
        tchecker::dbm::db_t c0j = DBM(0,j), cj0 = DBM(j,0);
        if (j > 1)
          os << " & ";
        // xj = k
        if (tchecker::dbm::sum(c0j, cj0) == tchecker::dbm::LE_ZERO)
          os << clock_name(j) << "=" << tchecker::dbm::value(cj0);
        // k1 <= xj <= k2
        else {
          os << - tchecker::dbm::value(c0j) << tchecker::dbm::comparator_str(c0j) << clock_name(j);
          if (cj0 != tchecker::dbm::LT_INFINITY)
            os << tchecker::dbm::comparator_str(cj0) << tchecker::dbm::value(cj0);
        }
      }
      
      // output: x-y # c (other rows/columns)
      for (tchecker::clock_id_t i = 1; i < dim; ++i) {
        for (tchecker::clock_id_t j = i+1; j < dim; ++j) {
          tchecker::dbm::db_t cij = DBM(i,j), cji = DBM(j,i);
          // xi == xj + k
          if (tchecker::dbm::sum(cij, cji) == tchecker::dbm::LE_ZERO) {
            os << " & ";
            os << clock_name(i) << "=" << clock_name(j);
            tchecker::integer_t vij = tchecker::dbm::value(cij);
            if (vij > 0)
              os << "+" << tchecker::dbm::value(cij);
            else if (vij < 0)
              os << "-" << - tchecker::dbm::value(cij);
          }
          // k1 <= xi - xj <= k2
          else if ((cij != tchecker::dbm::LT_INFINITY) ||
                   (cji != tchecker::dbm::LT_INFINITY)) {
            os << " & ";
            
            if (cji != tchecker::dbm::LT_INFINITY)
              os << - tchecker::dbm::value(cji) << tchecker::dbm::comparator_str(cji);
            
            os << clock_name(i) << "-" << clock_name(j);
            
            if (cij != tchecker::dbm::LT_INFINITY)
              os << tchecker::dbm::comparator_str(cij) << tchecker::dbm::value(cij);
          }
        }
      }
      
      os << ")";
      
      return os;
    }


    int lexical_cmp(tchecker::dbm::db_t const * dbm1, tchecker::clock_id_t dim1,
                    tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim2)
    {
      assert(dbm1 != nullptr);
      assert(dbm2 != nullptr);
      assert(dim1 >= 1);
      assert(dim2 >= 1);
      return tchecker::lexical_cmp(dbm1, dbm1 + dim1 * dim1,
                                   dbm2, dbm2 + dim2 * dim2,
                                   tchecker::dbm::db_cmp);
    }
    
  } // end of namespace dbm
  
} // end of namespace tchecker
