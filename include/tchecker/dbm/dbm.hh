/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_DBM_DBM_HH
#define TCHECKER_DBM_DBM_HH

#include <functional>
#include <iostream>

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/db.hh"

/*!
 \file dbm.hh
 \brief DBMs and operations on DBMs
 \note A DBM of dimension dim is represented as a dim*dim array of difference bounds tchecker::dbm::db_t. The difference bound #c
 in [i,j] encodes the constraint x_i - x_j # c. The difference bound in [i,j] for dbm of dimension dim is accessed with
 dbm[i*dim+j]
 A DBM can be seen as weighted graph where nodes are clocks (including 0) and for any two clocks x and y, the weigth of the edge
 x -> y is the difference bound #c of the constraint y - x #c in the DBM.
 A DBM is consistent if for every clock x, the bound for x - x is <=0, and if the bound for 0 - x is less-than or equal-to <=0
 (i.e. clocks are positive or zero). Notice that consistent zones are not empty.
 A DBM is tight if for every constraint x - y # c, the bound #c is the shortest-path from y to x in the corresponding graph. Every
 non-empty DBM has a unique corresponding tight DBM. Empty DBMs have a negative cycle on some clock x. Hence they cannot be
 tightened: there is no shortest path from x to x.
 */

namespace tchecker {
  
  namespace dbm {
  
    namespace details{
      /*!
       * \brief Auxialliry class used to store all information necessary for non-trivial
       * resets.
       */
      class reset_struct_t{
      public:
        /*!
         * \brief The structure represents M'(x',y'). The default constructor corresponds to
         * M'(x',y') = M(x,y)
         * M'(x',y') <-> x'-y' <= c' <-> (x+v_x)-(y+v_y)<=c' <-> M(x,y)+(v_x-v_y) <= c' -> M'(x',y') = M(x,y)+(v_x-v_y)
         * @param x : id of clock x; x' = x + v_x
         * @param y : id of clock y; y' = y + v_y
         */
        reset_struct_t(tchecker::clock_id_t x, tchecker::clock_id_t y);
      
        void set_x(tchecker::clock_id_t x, tchecker::dbm::db_t v_x);
        void set_y(tchecker::clock_id_t y, tchecker::dbm::db_t v_y);
        bool is_mod() const;
        void visit_reset(const clock_reset_t & reset);
        void visit_reset(const clock_reset_container_t & reset_vec);
        tchecker::dbm::db_t compute(tchecker::dbm::db_t const * const dbm, tchecker::clock_id_t dim)const;
    
    
      private:
        tchecker::clock_id_t x_,y_;
        tchecker::dbm::db_t v_x_,v_y_;
        bool is_mod_;
        bool is_set_x_, is_set_y_;
      };
    
      /*!
       * \brief Fill a vector with default resets (dbm_new == dbm_old)
       * @param vec
       * @param dim
       */
      void fill_reset_vector(std::vector<reset_struct_t> & vec, const tchecker::clock_id_t dim);
    
      /*!
       *\brief Apply the resets
       * @param dbm1 : modifiable dbm for zone after reset
       * @param dbm2 : constant dbm for zone before reset
       * @param dim : dimension of the zone
       * @param vec : vector of resets
       * @param apply_all : Whether all resets should be applied no matter if the value of dbm_new_ij == dbm_old_ij
       */
      void apply_reset_vector(tchecker::dbm::db_t * dbm1, tchecker::dbm::db_t const * const dbm2, const tchecker::clock_id_t dim, const std::vector<reset_struct_t> & vec, bool apply_all=false);
    
    } // end details
  
    /*!
     \brief Status of a DBM
     */
    enum status_t {
      EMPTY = 0,
      NON_EMPTY,
      MAY_BE_EMPTY,
    };
    
    /*!
     \brief Universal zone
     \param dbm : a DBM
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim >= 1 (checked by assertion)
     \post dbm represents the true/universal zone (all valuations, positive and negative)
     dbm is tight.
     */
    void universal(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Universal positive zone
     \param dbm : a DBM
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim >= 1 (checked by assertion)
     \post dbm represents the positive true/universal zone (i.e. conjunction x >= 0 for all clocks x)
     dbm is tight.
     */
    void universal_positive(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Empty zone
     \param dbm : a DBM
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim >= 1 (checked by assertion)
     \post dbm represents the empty zone (is_empty_0() returns true on dbm)
     dbm IS NOT TIGHT (empty DBMs cannot be tight)
     */
    void empty(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Initialize to zero
     \param dbm : a DBM
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim >= 1 (checked by assertion)
     \post dbm represents the zone that contains the valuation 0 in dimension dim.
     dbm is tight.
     */
    void zero(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Consistency checking
     \param dbm : a dbm
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of tchecker::dbm::db_t.
     dim >= 1 (checked by assertion)
     \return true if the diagonal in dbm is <=0, and false otherwise
     */
    bool is_consistent(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Fast emptiness predicate
     \param dbm : a DBM
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dbm has been tightened
     dim >= 1 (checked by assertion).
     \return true if dbm has a negative difference bound on (0,0), false otherwise
     \note this function only checks the (0,0) entry of dbm. Hence, dbm may be empty but not be detected by this function. However,
     all other functions set (0,0) to a value less-than <=0 when they generate an empty zone. So this function can be used as a
     safe and efficient emptiness check if dbm has been generated by calls to functions in this file.
     */
    bool is_empty_0(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Universality predicate
     \param dbm : a DBM
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim >= 1 (checked by assertion).
     \return true if dbm represents the true/universal zone, false otherwise (see tchecker::dbm::universal)
     */
    bool is_universal(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Positivity predicate
     \param dbm : a DBM
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim >= 1 (checked by assertion).
     \return true if dbm is positive (i.e. for every valuation in dbm, and every clock x, x>=0), false otherwise
     */
    bool is_positive(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Positivity & universality predicate
     \param dbm : a DBM
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim >= 1 (checked by assertion).
     \return true if dbm is the set of all positive valuations, false otherwise
     \note efficient algorithm for tchecker::dbm::is_universal(dbm, dim) && tchecker::dbm::is_positive(dbm dim)
     */
    bool is_universal_positive(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Tightness predicate
     \param dbm : a DBM
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim >= 1 (checked by assertion)
     \return true if dbm is tight, false otherwise
     */
    bool is_tight(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Tighten a DBM
     \param dbm : a DBM
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim >= 1 (checked by assertion)
     dbm is consistent (checked by assertion)
     \post dbm is not nullptr (checked by assertion)
     dbm is tight if it is not empty.
     if dbm is empty, then the difference bound in (0,0) is less-than <=0 (tchecker::dbm::is_empty_0() returns true)
     \return EMPTY if dbm is empty, NON_EMPTY otherwise
     \note Applies Floyd-Warshall algorithm on dbm seen as a weighted graph.
     */
    enum tchecker::dbm::status_t tighten(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Tighten a DBM w.r.t. a constraint
     \param dbm : a DBM
     \param dim : dimension of dbm
     \param x : first clock
     \param y : second clock
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim >= 1 (checked by assertion)
     dbm is consistent (checked by assertion)
     0 <= x < dim (checked by assertion)
     0 <= y < dim (checked by assertion)
     x != y (checked by assertion)
     \post for all clocks u and v, the edge u->v in the graph is tight w.r.t. the edge y->x. That is, the length of the path u->v
     is at most the length of the path u->y->x->v.
     if dbm is empty, then the difference bound in (0,0) is less-than <=0 (tchecker::dbm::is_empty_0() returns true)
     \return EMPTY if dbm is empty, MAY_BE_EMPTY otherwise
     \note if every edge in dbm is tight w.r.t. all other edges except i->j, then after the call, dbm is either empty, or it is
     not empty and tight.
     */
    enum tchecker::dbm::status_t tighten(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x,
                                         tchecker::clock_id_t y);
    
    /*!
     \brief Constrain a DBM
     \param dbm : a dbm
     \param dim : dimension of dbm
     \param x : first clock
     \param y : second clock
     \param cmp : constraint comparator
     \param value : constraint value
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dbm is consistent (checked by assertion)
     dbm is tight (checked by assertion)
     dim >= 1 (checked by assertion).
     0 <= x < dim (checked by assertion)
     0 <= y < dim (checked by assertion)
     value can be represented in a tchecker::dbm::db_t
     \post dbm has been intersected with constraint `x - y # value` where # is < if cmp is LT, and # is <= if cmp is LE
     dbm is tight if it is not empty.
     if dbm is empty, then its difference bound in (0,0) is less-than <=0 (tchecker::dbm::is_empty_0() returns true)
     \return EMPTY if dbm is empty, NON_EMPTY otherwise
     \throw std::invalid_argument : if `cmp value` cannot be represented as a tchecker::dbm::db_t (only if compilation flag
     DBM_UNSAFE is not set)
     */
    enum tchecker::dbm::status_t
    constrain(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::clock_id_t y,
              tchecker::dbm::comparator_t cmp, int32_t value);
    
    /*!
     \brief Equality predicate
     \param dbm1 : a first dbm
     \param dmb2 : a second dbm
     \param dim : dimension of dbm1 and dbm2
     \pre dbm1 and dbm2 are not nullptr (checked by assertion)
     dbm1 and dbm2 are dim*dim arrays of difference bounds
     dbm1 and dbm2 are tight (checked by assertion)
     dim >= 1 (checked by assertion).
     \return true if dbm1 and dbm2 are equal, false otherwise
     */
    bool is_equal(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim);
    
    /*!
     \brief Inclusion predicate
     \param dbm1 : a first dbm
     \param dmb2 : a second dbm
     \param dim : dimension of dbm1 and dbm2
     \pre dbm1 and dbm2 are not nullptr (checked by assertion)
     dbm1 and dbm2 are dim*dim arrays of difference bounds
     dbm1 and dbm2 are tight (checked by assertion)
     dim >= 1 (checked by assertion).
     \return true if dbm1 is included into dbm2, false otherwise
     */
    bool is_le(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim);
    
    /*!
     \brief Reset a clock
     \param dbm : a dbm
     \param dim : dimension of dbm
     \param x : left-value clock
     \param y : right-value clock
     \param value : a value
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dbm is consistent (checked by assertion)
     dbm is tight (checked by assertion)
     dim >= 1 (checked by assertion).
     0 <= x < dim (checked by assertion)
     0 <= y < dim (checked by assertion)
     0 <= value (checked by assertion)
     `<= value` can be represented by tchecker::dbm::db_t (checked by assertion)
     \post dbm has been updated according to reset `x := y + value`
     dbm is consistent (checked by assertion)
     dbm is tight (checked by assertion)
     \note wrapper for dedicated functions depending on y and value. Set y to 0 in order to reset x to constant value. Set value
     to 0 in order to reset x to y. Set both y and value non-zero to reset x to the sum y+value. Increment x by value if y==x.
     \throw std::invalid_argument : if `<= value` cannot be represented as a tchecker::dbm::db_t (only if compilation flag
     DBM_UNSAFE is not set)
     */
    void reset(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::clock_id_t y,
               int32_t value);
    
    /*!
     \brief Reset a clock to a constant
     \param dbm : a dbm
     \param dim : dimension of dbm
     \param x : left-value clock
     \param value : a value
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dbm is consistent (checked by assertion)
     dbm is tight (checked by assertion)
     dim >= 1 (checked by assertion).
     0 <= x < dim (checked by assertion)
     0 <= value *checked by assertion)
     `<= value` can be represented by tchecker::dbm::db_t
     \post dbm has been updated according to reset x := value.
     dbm is consistent.
     dbm is tight.
     \throw std::invalid_argument : if `<= value` cannot be represented as a tchecker::dbm::db_t (only if compilation flag
     DBM_UNSAFE is not set)
     */
    void reset_to_value(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, int32_t value);
    
    /*!
     \brief Reset a clock to another clock
     \param dbm : a dbm
     \param dim : dimension of dbm
     \param x : left-value clock
     \param y : right-value clock
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dbm is consistent (checked by assertion)
     dbm is tight (checked by assertion)
     dim >= 1 (checked by assertion).
     0 <= x < dim (checked by assertion)
     0 <  y < dim (checked by assertion)
     \post dbm has been updated according to reset x := y.
     dbm is consistent.
     dbm is tight.
     */
    void reset_to_clock(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::clock_id_t y);
    
    /*!
     \brief Reset a clock to the sum of a clock and a non-negative integer
     \param dbm : a dbm
     \param dim : dimension of dbm
     \param x : left-value clock
     \param y : right-value clock
     \param value : a value
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dbm is consistent (checked by assertion)
     dbm is tight (checked by assertion)
     dim >= 1 (checked by assertion).
     0 <= x < dim (checked by assertion)
     0 <= y < dim (checked by assertion)
     0 <= value (checked by assertion)
     `<= value` can be represented by tchecker::dbm::db_t (checked by assertion)
     \post dbm has been updated according to reset `x := y + value`
     dbm is consistent (checked by assertion)
     dbm is tight (checked by assertion)
     */
    void reset_to_sum(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::clock_id_t x, tchecker::clock_id_t y,
                      int32_t value);
    
    /*!
     \brief Open up (delay)
     \param dbm : a dbm
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dbm is consistent (checked by assertion)
     dbm is tight (checked by assertion)
     dim >= 1 (checked by assertion).
     \post all upper bounds on clocks in dbm are <infinity.
     dbm is tight.
     */
    void open_up(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Intersection
     \param dbm : a dbm
     \param dbm1 : a dbm
     \param dbm2 : a dbm
     \param dim : dimension of dbm1, dbm1 and dbm2
     \pre dbm, dbm1 and dbm2 are not nullptr (checked by assertion)
     dbm, dbm1 and dbm2 are dim*dim arrays of difference bounds
     dbm1 and dbm2 are consistent (checked by assertion)
     dbm1 and dbm2 are tight (checked by assertion)
     dim >= 1 (checked by assertion).
     \post dbm is the intersection of dbm1 and dbm2
     dbm is consistent
     dbm is tight
     \return EMPTY if the intersection of dbm1 and dbm2 is empty, NON_EMPTY otherwise
     \note dbm can be one of dbm1 or dbm2
     */
    enum tchecker::dbm::status_t intersection(tchecker::dbm::db_t * dbm,
                                              tchecker::dbm::db_t const * dbm1,
                                              tchecker::dbm::db_t const * dbm2,
                                              tchecker::clock_id_t dim);
    
    /*!
     \brief ExtraM extrapolation
     \param dbm : a dbm
     \param dim : dimension of dbm
     \param m : clock bounds
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dbm is consistent (checked by assertion)
     dbm is positive (checked by assertion)
     dbm is tight (checked by assertion)
     dim >= 1 (checked by assertion)
     m is an array of size dim
     m[0] = 0 (checked by assertion)
     m[i] < tchecker::dbm::INF_VALUE for all i>0 (checked by assertion)
     \post extrapolation ExtraM has been applied to dbm w.r.t. clock
     bounds m (see "Lower and upper bounds in zone-based abstractions of timed automata", Behrmann, Bouyer, Larsen and Pelanek.
     Int. J. STTT, 2006)
     \note set m[i] to -tchecker::dbm::INF_VALUE if clock i has no bound
     */
    void extra_m(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, int32_t const * m);
    
    /*!
     \brief ExtraM+ extrapolation
     \param dbm : a dbm
     \param dim : dimension of dbm
     \param m : clock bounds
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dbm is consistent (checked by assertion)
     dbm is positive (checked by assertion)
     dbm is tight (checked by assertion)
     dim >= 1 (checked by assertion)
     m is an array of size dim
     m[0] = 0 (checked by assertion)
     m[i] < tchecker::dbm::INF_VALUE for all i>0 (checked by assertion)
     \post extrapolation ExtraM+ has been applied to dbm w.r.t. clock
     bounds m (see "Lower and upper bounds in zone-based abstractions of timed automata", Behrmann, Bouyer, Larsen and Pelanek.
     Int. J. STTT, 2006)
     \note set m[i] to -tchecker::dbm::INF_VALUE if clock i has no bound
     */
    void extra_m_plus(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, int32_t const * m);
    
    /*!
     \brief ExtraLU extrapolation
     \param dbm : a dbm
     \param dim : dimension of dbm
     \param l : clock lower bounds
     \param u : clock upper bounds
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dbm is consistent (checked by assertion)
     dbm is positive (checked by assertion)
     dbm is tight (checked by assertion)
     dim >= 1 (checked by assertion)
     l and u are arrays of size dim
     l[0] = u[0] = 0 (checked by assertion)
     l[i], u[i] < tchecker::dbm::INF_VALUE for all i>0 (checked by assertion)
     \post extrapolation ExtraLU has been applied to dbm w.r.t. clock
     bounds l and u (see "Lower and upper bounds in zone-based abstractions of timed automata", Behrmann, Bouyer, Larsen and
     Pelanek. Int. J. STTT, 2006)
     \note set l[i]/u[i] to -tchecker::dbm::INF_VALUE if clock i has no lower/upper bound
     */
    void extra_lu(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, int32_t const * l, int32_t const * u);
    
    /*!
     \brief ExtraLU+ extrapolation
     \param dbm : a dbm
     \param dim : dimension of dbm
     \param l : clock lower bounds
     \param u : clock upper bounds
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dbm is consistent (checked by assertion)
     dbm is positive (checked by assertion)
     dbm is tight (checked by assertion)
     dim >= 1 (checked by assertion)
     l and u are arrays of size dim
     l[0] = u[0] = 0 (checked by assertion)
     l[i], u[i] < tchecker::dbm::INF_VALUE for all i>0 (checked by assertion)
     \post extrapolation ExtraLU+ has been applied to dbm w.r.t. clock
     bounds l and u (see "Lower and upper bounds in zone-based abstractions of timed automata", Behrmann, Bouyer, Larsen and
     Pelanek. Int. J. STTT, 2006)
     \note set l[i]/u[i] to -tchecker::dbm::INF_VALUE if clock i has no lower/upper bound
     */
    void extra_lu_plus(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, int32_t const * l, int32_t const * u);
    
    /*!
     \brief Checks inclusion w.r.t. abstraction aLU
     \param dbm1 : a first dbm
     \param dbm2 : a second dbm
     \param dim : dimension of dbm1 and dbm2
     \param l : clock lower bounds
     \param u : clock upper bounds
     \pre dbm1 and dbm2 are not nullptr (checked by assertion)
     dbm1 and dbm2 are dim*dim arrays of difference bounds
     dbm1 and dbm2 are consistent (checked by assertion)
     dbm1 and dbm2 are positive (checked by assertion)
     dbm1 and dbm2 are tight (checked by assertion)
     dim >= 1 (checked by assertion)
     l and u are arrays of size dim
     l[0] = u[0] = 0 (checked by assertion)
     l[i], u[i] < tchecker::dbm::INF_VALUE for all i>0 (checked by assertion)
     \return true if dbm1 <= aLU(dbm2), false otherwise (see "Better abstractions for timed automata", Herbreteau, Srivathsan
     and Walukiewicz. Inf. Comput., 2016)
     \note set l[i]/u[i] to -tchecker::dbm::INF_VALUE if clock i has no lower/upper bound
     */
    bool is_alu_le(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim,
                   int32_t const * l, int32_t const * u);
    
    /*!
     \brief Checks inclusion w.r.t. abstraction aM
     \param dbm1 : a first dbm
     \param dbm2 : a second dbm
     \param dim : dimension of dbm1 and dbm2
     \param m : clock bounds
     \pre dbm1 and dbm2 are not nullptr (checked by assertion)
     dbm1 and dbm2 are dim*dim arrays of difference bounds
     dbm1 and dbm2 are consistent (checked by assertion)
     dbm1 and dbm2 are positive (checked by assertion)
     dbm1 and dbm2 are tight (checked by assertion)
     dim >= 1 (checked by assertion)
     m is an array of size dim
     m[0] = 0 (checked by assertion)
     m[i] < tchecker::dbm::INF_VALUE for all i>0 (checked by assertion)
     \return true if z1 <= aM(z2), false otherwise (see "Better abstractions for timed automata", Herbreteau, Srivathsan
     and Walukiewicz. Inf. Comput., 2016)
     \note set m[i] to -tchecker::dbm::INF_VALUE if clock i has no lower/upper bound
     */
    bool is_am_le(tchecker::dbm::db_t const * dbm1, tchecker::dbm::db_t const * dbm2, tchecker::clock_id_t dim,
                  int32_t const * m);
    
    /*!
     \brief Hash function
     \param dbm : a dbm
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim >= 1 (checked by assertion)
     \return hash code of dbm
     */
    std::size_t hash(tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Output a DBM as a matrix
     \param os : output stream
     \param dbm : a dbm
     \param dim : dimension of dbm
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim >= 1 (checked by assertion).
     \post dbm has been output to os
     \return os after output
     */
    std::ostream & output_matrix(std::ostream & os, tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Output a DBM as a conjunction of constraints
     \param os : output stream
     \param dbm : a dbm
     \param dim : dimension of dbm
     \oaram clock_name : map from clock IDs to strings
     \pre dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim >= 1 (checked by assertion).
     clock_name maps any clock ID in [0,dim) to a name
     \post the relevant constraints in dbm has been output to os. Relevant constraints are those that differ from the
     universal DBM.
     \return os after output
     */
    std::ostream & output(std::ostream & os, tchecker::dbm::db_t const * dbm, tchecker::clock_id_t dim,
                          std::function<std::string(tchecker::clock_id_t)> clock_name);
    
  } // end of namespace dbm
  
} // end of namespace tchecker

#endif // TCHECKER_DBM_DBM_HH
