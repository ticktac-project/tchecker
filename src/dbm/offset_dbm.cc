/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/dbm/offset_dbm.hh"
#include "tchecker/utils/ordering.hh"

#define DBM(i,j)         dbm[(i)*dim+(j)]
#define OFFSET_DBM(i,j)  offset_dbm[(i)*offset_dim+(j)]

namespace tchecker {
  
  namespace offset_dbm {
    
    void universal(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim)
    {
      assert(offset_dbm != nullptr);
      assert(offset_dim >= 1);
      tchecker::dbm::universal(offset_dbm, offset_dim);
    }
    
    
    void empty(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim)
    {
      assert(offset_dbm != nullptr);
      assert(offset_dim >= 1);
      tchecker::dbm::empty(offset_dbm, offset_dim);
    }
    
    
    void universal_positive(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim,
                            tchecker::clock_id_t refcount, tchecker::clock_id_t const * refmap)
    {
      assert(offset_dbm != nullptr);
      assert(refcount >= 1);
      assert(refcount <= offset_dim);
      assert(refmap != nullptr);
      
      tchecker::offset_dbm::universal(offset_dbm, offset_dim);
      // clocks are non-negative: x>=0 <=> X>=RX <=> RX-X<=0
      for (tchecker::clock_id_t i = refcount; i < offset_dim; ++i)
        OFFSET_DBM(refmap[i], i) = tchecker::dbm::LE_ZERO;
    }
    
    
    void zero(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim)
    {
      assert(offset_dbm != nullptr);
      assert(offset_dim >= 1);
      tchecker::dbm::zero(offset_dbm, offset_dim);
    }
    
    
    bool is_empty_0(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim)
    {
      assert(offset_dbm != nullptr);
      assert(offset_dim >= 1);
      return tchecker::dbm::is_empty_0(offset_dbm, offset_dim);
    }
    
    
    bool is_universal(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim)
    {
      assert(offset_dbm != nullptr);
      assert(offset_dim >= 1);
      return tchecker::dbm::is_universal(offset_dbm, offset_dim);
    }
    
    
    bool is_positive(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim,
                     tchecker::clock_id_t refcount, tchecker::clock_id_t const * refmap)
    {
      assert(offset_dbm != nullptr);
      assert(tchecker::offset_dbm::is_tight(offset_dbm, offset_dim));
      assert(offset_dim >= 1);
      assert(refmap != nullptr);
      
      // RX-X are less-or-equal to <=0
      for (tchecker::clock_id_t i = refcount; i < offset_dim; ++i)
        if (OFFSET_DBM(refmap[i], i) > tchecker::dbm::LE_ZERO)
          return false;
      return true;
    }
    
    
    bool is_universal_positive(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim,
                               tchecker::clock_id_t refcount, tchecker::clock_id_t const * refmap)
    {
      assert(offset_dbm != nullptr);
      assert(tchecker::offset_dbm::is_tight(offset_dbm, offset_dim));
      assert(1 <= refcount);
      assert(refcount <= offset_dim);
      assert(refmap != nullptr);
      
      // <inf everywhere, except <=0 on the diagonal and for RX-X
      for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
        for (tchecker::clock_id_t j = 0; j < offset_dim; ++j) {
          tchecker::clock_id_t rj = refmap[j];
          tchecker::dbm::db_t expected = ((i == j) || (i == rj)
                                          ? tchecker::dbm::LE_ZERO
                                          : tchecker::dbm::LT_INFINITY);
          if (OFFSET_DBM(i, j) != expected)
            return false;
        }
      }
      return true;
    }
    
    
    bool is_tight(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim)
    {
      assert(offset_dbm != nullptr);
      assert(offset_dim >= 1);
      return tchecker::dbm::is_tight(offset_dbm, offset_dim);
    }
    
    
    bool is_synchronized(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim,
                         tchecker::clock_id_t refcount)
    {
      assert(offset_dbm != nullptr);
      assert(tchecker::dbm::is_consistent(offset_dbm, offset_dim));
      assert(tchecker::offset_dbm::is_tight(offset_dbm, offset_dim));
      assert(1 <= refcount);
      assert(refcount <= offset_dim);
      
      for (tchecker::clock_id_t i = 0; i < refcount - 1; ++i) {
        tchecker::clock_id_t j = i+1;
        if ((OFFSET_DBM(i,j) != tchecker::dbm::LE_ZERO) || (OFFSET_DBM(j,i) != tchecker::dbm::LE_ZERO))
          return false;
      }
      return true;
    }
    
    
    bool is_equal(tchecker::dbm::db_t const * offset_dbm1, tchecker::dbm::db_t const * offset_dbm2,
                  tchecker::clock_id_t offset_dim)
    {
      assert(offset_dbm1 != nullptr);
      assert(offset_dbm2 != nullptr);
      assert(tchecker::offset_dbm::is_tight(offset_dbm1, offset_dim));
      assert(tchecker::offset_dbm::is_tight(offset_dbm2, offset_dim));
      assert(tchecker::dbm::is_consistent(offset_dbm1, offset_dim));
      assert(tchecker::dbm::is_consistent(offset_dbm2, offset_dim));
      assert(offset_dim >= 1);
      return tchecker::dbm::is_equal(offset_dbm1, offset_dbm2, offset_dim);
    }
    
    
    bool is_le(tchecker::dbm::db_t const * offset_dbm1, tchecker::dbm::db_t const * offset_dbm2,
               tchecker::clock_id_t offset_dim)
    {
      assert(offset_dbm1 != nullptr);
      assert(offset_dbm2 != nullptr);
      assert(tchecker::offset_dbm::is_tight(offset_dbm1, offset_dim));
      assert(tchecker::offset_dbm::is_tight(offset_dbm2, offset_dim));
      assert(tchecker::dbm::is_consistent(offset_dbm1, offset_dim));
      assert(tchecker::dbm::is_consistent(offset_dbm2, offset_dim));
      assert(offset_dim >= 1);
      return tchecker::dbm::is_le(offset_dbm1, offset_dbm2, offset_dim);
    }
    
    
    std::size_t hash(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim)
    {
      assert(offset_dbm != nullptr);
      assert(offset_dim >= 1);
      return tchecker::dbm::hash(offset_dbm, offset_dim);
    }
    
    
    enum tchecker::dbm::status_t
    constrain(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim, tchecker::clock_id_t x,
              tchecker::clock_id_t y, tchecker::dbm::comparator_t cmp, tchecker::integer_t value)
    {
      assert(offset_dbm != nullptr);
      assert(tchecker::offset_dbm::is_tight(offset_dbm, offset_dim));
      assert(offset_dim >= 1);
      assert(x < offset_dim);
      assert(y < offset_dim);
      assert(x != y);
      return tchecker::dbm::constrain(offset_dbm, offset_dim, x, y, cmp, value);
    }
    
    
    enum tchecker::dbm::status_t synchronize(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim,
                                             tchecker::clock_id_t refcount)
    {
      assert(offset_dbm != nullptr);
      assert(tchecker::dbm::is_consistent(offset_dbm, offset_dim));
      assert(tchecker::offset_dbm::is_tight(offset_dbm, offset_dim));
      assert(1 <= refcount);
      assert(refcount <= offset_dim);
      
      for (tchecker::clock_id_t x = 0; x < refcount - 1; ++x) {
        OFFSET_DBM(x, x+1) = tchecker::dbm::min(OFFSET_DBM(x, x+1), tchecker::dbm::LE_ZERO);
        OFFSET_DBM(x+1, x) = tchecker::dbm::min(OFFSET_DBM(x+1, x), tchecker::dbm::LE_ZERO);
      }
      
      // Optimized tightening: Floyd-Warshall algorithm w.r.t. reference clocks
      for (tchecker::clock_id_t r = 0; r < refcount; ++r) {
        for (tchecker::clock_id_t x = 0; x < offset_dim; ++x) {
          if ((x == r) || (OFFSET_DBM(x,r) == tchecker::dbm::LT_INFINITY))
            continue;   // optimization
          
          for (tchecker::clock_id_t y = 0; y < offset_dim; ++y) {
            if (y == r)
              continue;  // optimization
            
            OFFSET_DBM(x,y) = tchecker::dbm::min(tchecker::dbm::sum(OFFSET_DBM(x,r), OFFSET_DBM(r,y)), OFFSET_DBM(x,y));
          }
          
          if (OFFSET_DBM(x,x) < tchecker::dbm::LE_ZERO) {
            OFFSET_DBM(0,0) = tchecker::dbm::LT_ZERO;
            return tchecker::dbm::EMPTY;
          }
        }
      }
      
      assert(tchecker::dbm::is_consistent(offset_dbm, offset_dim));
      assert(tchecker::offset_dbm::is_tight(offset_dbm, offset_dim));
      
      return tchecker::dbm::NON_EMPTY;
    }
    
    
    void reset_to_refclock(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim,
                           tchecker::clock_id_t x, tchecker::clock_id_t refcount,
                           tchecker::clock_id_t const * refmap)
    {
      assert(offset_dbm != nullptr);
      assert(tchecker::dbm::is_consistent(offset_dbm, offset_dim));
      assert(tchecker::offset_dbm::is_tight(offset_dbm, offset_dim));
      assert(offset_dim >= 1);
      assert(x < offset_dim);
      
      if (refmap[x] == x)
        return;
      
      // x is identified to rx w.r.t. all clocks z
      for (tchecker::clock_id_t z = 0; z < offset_dim; ++z) {
        OFFSET_DBM(x,z) = OFFSET_DBM(refmap[x],z);
        OFFSET_DBM(z,x) = OFFSET_DBM(z,refmap[x]);
      }
      OFFSET_DBM(x,x) = tchecker::dbm::LE_ZERO; // cheaper than testing in loop
      
      assert(tchecker::dbm::is_consistent(offset_dbm, offset_dim));
      assert(tchecker::offset_dbm::is_tight(offset_dbm, offset_dim));
    }
    
    
    void asynchronous_open_up(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim,
                              tchecker::clock_id_t refcount)
    {
      assert(offset_dbm != nullptr);
      assert(tchecker::dbm::is_consistent(offset_dbm, offset_dim));
      assert(tchecker::offset_dbm::is_tight(offset_dbm, offset_dim));
      assert(1 <= refcount);
      assert(refcount <= offset_dim);
      
      // X - R < inf for all X and R (including X=R')
      for (tchecker::clock_id_t r = 0; r < refcount; ++r) {
        for (tchecker::clock_id_t i = 0; i < offset_dim; ++i)
          OFFSET_DBM(i,r) = tchecker::dbm::LT_INFINITY;
        OFFSET_DBM(r,r) = tchecker::dbm::LE_ZERO;
      }
      
      assert(tchecker::dbm::is_consistent(offset_dbm, offset_dim));
      assert(tchecker::offset_dbm::is_tight(offset_dbm, offset_dim));
    }
    
    
    void asynchronous_open_up(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim,
                              tchecker::clock_id_t refcount, boost::dynamic_bitset<> const & delay_allowed)
    {
      assert(offset_dbm != nullptr);
      assert(tchecker::dbm::is_consistent(offset_dbm, offset_dim));
      assert(tchecker::offset_dbm::is_tight(offset_dbm, offset_dim));
      assert(1 <= refcount);
      assert(refcount <= offset_dim);
      assert(refcount == delay_allowed.size());
      
      // X - R < inf for all X and R s.t. delay is allowed for R (including X=R')
      for (tchecker::clock_id_t r = 0; r < refcount; ++r) {
        if (! delay_allowed[r])
          continue;
        
        for (tchecker::clock_id_t i = 0; i < offset_dim; ++i)
          OFFSET_DBM(i,r) = tchecker::dbm::LT_INFINITY;
        OFFSET_DBM(r,r) = tchecker::dbm::LE_ZERO;
      }
      
      assert(tchecker::dbm::is_consistent(offset_dbm, offset_dim));
      assert(tchecker::offset_dbm::is_tight(offset_dbm, offset_dim));
    }
    
    
    enum tchecker::dbm::status_t tighten(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim)
    {
      assert(offset_dbm != nullptr);
      assert(offset_dim >= 1);
      return tchecker::dbm::tighten(offset_dbm, offset_dim);
    }
    
    
    void to_dbm(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim,
                tchecker::clock_id_t refcount, tchecker::clock_id_t const * refmap,
                tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim)
    {
      assert(offset_dbm != nullptr);
      assert(tchecker::dbm::is_consistent(offset_dbm, offset_dim));
      assert(tchecker::offset_dbm::is_tight(offset_dbm, offset_dim));
      assert(tchecker::offset_dbm::is_synchronized(offset_dbm, offset_dim, refcount) );
      assert(1 <= refcount);
      assert(refcount <= offset_dim);
      assert(refmap != nullptr);
      assert(dbm != nullptr);
      assert(dim == offset_dim - refcount + 1);
      
      for (tchecker::clock_id_t i = 1; i < dim; ++i) {
        auto reference_i = tchecker::offset_dbm::reference_id(i, refcount, refmap);
        auto offset_i = tchecker::offset_dbm::offset_id(i, refcount);
        DBM(0,i) = OFFSET_DBM(reference_i, offset_i);
        DBM(i,0) = OFFSET_DBM(offset_i, reference_i);
        for (tchecker::clock_id_t j = i; j < dim; ++j) {
          auto offset_j = tchecker::offset_dbm::offset_id(j, refcount);
          DBM(i,j) = OFFSET_DBM(offset_i, offset_j);
          if (i != j)
            DBM(j,i) = OFFSET_DBM(offset_j, offset_i);
        }
      }
      DBM(0,0) = tchecker::dbm::LE_ZERO;
      
      assert(tchecker::dbm::is_tight(dbm, dim));
    }
    
    
    std::ostream & output_matrix(std::ostream & os, tchecker::dbm::db_t const * offset_dbm,
                                 tchecker::clock_id_t offset_dim)
    {
      return tchecker::dbm::output_matrix(os, offset_dbm, offset_dim);
    }
    
    
    std::ostream & output(std::ostream & os, tchecker::dbm::db_t const * offset_dbm,
                          tchecker::clock_id_t offset_dim,
                          std::function<std::string(tchecker::clock_id_t)> clock_name)
    {
      bool first = true;
      
      os << "(";

      for (tchecker::clock_id_t i = 0; i < offset_dim; ++i) {
        for (tchecker::clock_id_t j = i+1; j < offset_dim; ++j) {
          tchecker::dbm::db_t cij = OFFSET_DBM(i,j), cji = OFFSET_DBM(j,i);
          // vi == vj + k
          if (tchecker::dbm::sum(cij, cji) == tchecker::dbm::LE_ZERO) {
            if (! first)
              os << " & ";
            first = false;
            
            os << clock_name(i) << "=" << clock_name(j);
            tchecker::integer_t vij = tchecker::dbm::value(cij);
            if (vij > 0)
              os << "+" << tchecker::dbm::value(cij);
            else if (vij < 0)
              os << "-" << - tchecker::dbm::value(cij);
          }
          // k1 <= xi - xj <= k2
          else if ((cij != tchecker::dbm::LT_INFINITY) || (cji != tchecker::dbm::LT_INFINITY)) {
            if (! first)
              os << " & ";
            first = false;
            
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


    int lexical_cmp(tchecker::dbm::db_t const * offset_dbm1, tchecker::clock_id_t offset_dim1,
                    tchecker::dbm::db_t const * offset_dbm2, tchecker::clock_id_t offset_dim2)
    {
      assert(offset_dbm1 != nullptr);
      assert(offset_dbm2 != nullptr);
      assert(offset_dim1 >= 1);
      assert(offset_dim2 >= 1);
      return tchecker::lexical_cmp(offset_dbm1, offset_dbm1 + offset_dim1 * offset_dim1,
                                   offset_dbm2, offset_dbm2 + offset_dim2 * offset_dim2,
                                   tchecker::dbm::db_cmp);
    }
    
  } // end of namespace offset_dbm
  
} // end of namespace tchecker
