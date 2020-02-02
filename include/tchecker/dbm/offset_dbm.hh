/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_OFFSET_DBM_HH
#define TCHECKER_OFFSET_DBM_HH

#include <boost/dynamic_bitset/dynamic_bitset.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/dbm/dbm.hh"

/*!
 \file offset_dbm.hh
 \brief Functions on offset DBMs
 \note An offset DBM is a DBM where the first refcount variables are reference variables, and
 the other variables are offset variables. Each offset variable has a corresponding reference
 variable. The reference map refmap associates to each offset variable X, its reference clock
 RX=refmap[X]. Reference clocks are mapped to themselves (i.e. RX=refmap[RX]).
 
 The value of usual clock x is  represented as X-RX, the difference between the corresponding
 offset variable X and the corresponding  reference clock RX. Observe that RX plays the same
 role as the fictituous clock 0 with usual clock. In particular, time flows back in past by letting
 referene clock decrease.
 
 for more details on offset DBMs, see "Partial Order Reduction for Timed Systems", J. Bengtsson,
 B. Jonsson, J. Lilis and Wang Yi, CONCUR, 1998.
 */

namespace tchecker {
  
  namespace offset_dbm {
    
    /*!
     \brief Compute offset variable identifier of a clock
     \param id : a clock identifier
     \param refcount : number of reference clocks
     \pre id >= 1 (checked by assertion)
     \return the offset variable identifier corresponding to id w.r.t. refcount
     */
    constexpr tchecker::clock_id_t offset_id(tchecker::clock_id_t id, tchecker::clock_id_t refcount)
    {
      assert(id >= 1);
      return (refcount + id - 1);
    }
    
    
    /*!
     \brief Compute reference clock identifier of a clock
     \param id : a clock identifier
     \param refcount : number of reference clocks
     \param refmap : map reference clocks/offset variables to reference clocks
     \pre id >= 1 and offset_id(id, refcount) is in the range of refmap (not checked)
     \return the offset reference clock identifier for clock identifier id
     */
    constexpr tchecker::clock_id_t reference_id(tchecker::clock_id_t id, tchecker::clock_id_t refcount,
                                                tchecker::clock_id_t const * refmap)
    {
      return refmap[offset_id(id, refcount)];
    }
    
    
    /*!
     \brief Compute clock identifier of a reference clock/offset variable
     \param offset_id : an reference clock/offset variable identifier
     \param refcount : number of reference clocks
     \return the clock identifier corresponding to offset_id. All offset_id less than refcount are mapped to
     the zero clock
     */
    constexpr tchecker::clock_id_t clock_id(tchecker::clock_id_t offset_id, tchecker::clock_id_t refcount)
    {
      return (offset_id < refcount ? 0 : offset_id - refcount + 1);
    }
    
    
    /*!
     \brief Universal offset DBM
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dim >= 1 (checked by assertion)
     \post offset_dbm is the universal offset DBM of dimension offset_dim.
     offset_dbm is tight.
     */
    void universal(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim);
    
    /*!
     \brief Empty offset DBM
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dim >= 1 (checked by assertion)
     \post offset_dbm represents the empty offset zone (is_empty_0() returns true on offset_dbm)
     offset_dbm IS NOT TIGHT (empty offset DBMs cannot be tight)
     */
    void empty(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim);
    
    /*!
     \brief Positive universal offset DBM
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \param refcount : number of reference clocks
     \param refmap : map to reference clocks
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     1 <= refcount <= offset_dim (checked by assertion)
     the first refcount variables of offset_dbm are reference clocks, the next ones are offset variables.
     refmap maps each variable in offset_dbm to its reference clock.
     \post offset_dbm is the universal positive offset DBM of dimension offset_dim with refcount reference clocks
     and reference map refmap.
     offset_dbm is tight.
     */
    void universal_positive(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim,
                            tchecker::clock_id_t refcount, tchecker::clock_id_t const * refmap);
    
    /*!
     \brief Initialize to zero
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dim >= 1 (checked by assertion)
     \post offset_dbm represents the zone where all variables are equal to 0.
     offset_dbm is tight.
     */
    void zero(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim);
    
    /*!
     \brief Fast emptiness predicate
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dbm has been tightened
     offset_dim >= 1 (checked by assertion).
     \return true if offset_dbm has a negative difference bound on 0, false otherwise
     \note this function only checks the (0,0) entry of offset_dbm. Hence, offset_dbm may be empty but not be detected by
     this function.
     However, all other functions set (0,0) to a value less-than <=0 when they generate an empty zone. So this function can
     be used as a safe and efficient emptiness check if offset_dbm has been generated by calls to functions in this file
     (in particular, when offset_dbm has just been tightened)
     */
    bool is_empty_0(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim);
    
    /*!
     \brief Universality predicate
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \return true if z is universal, false otherwise
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dbm is tight (checked by assertion)
     offset_dim >= 1 (checked by assertion)
     */
    bool is_universal(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim);
    
    /*!
     \brief Positivity predicate
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \param refcount : number of reference clocks
     \param refmap : map to reference clocks
     \return true if z is positive, false otherwise
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dbm is tight (checked by assertion)
     1 <= refcount <= offset_dim (checked by assertion)
     refmap maps each variable in offset_dbm to its reference clock.
     */
    bool is_positive(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim,
                     tchecker::clock_id_t refcount, tchecker::clock_id_t const * refmap);
    
    /*!
     \brief Positive universality predicate
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \param refcount : number of reference clocks
     \param refmap : map to reference clocks
     \return true if z is universal positive, false otherwise
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dbm is tight (checked by assertion)
     1 <= refcount <= offset_dim (checked by assertion)
     refmap maps each variable in offset_dbm to its reference clock.
     */
    bool is_universal_positive(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim,
                               tchecker::clock_id_t refcount, tchecker::clock_id_t const * refmap);
    
    /*!
     \brief Tightness predicate
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dim >= 1 (checked by assertion)
     \return true if offset_dbm is tight, false otherwise
     */
    bool is_tight(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim);
    
    /*!
     \brief Synchronized predicate
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \param refcount : number of reference clocks
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dbm is consistent (checked by assertion)
     offset_dbm is tight (checked by assertion)
     1 <= refcount <= offset_dim (checked by assertion)
     the first refcount clocks of offset_dbm are reference clocks.
     \return true if reference clocks are equal to each other in offset_dbm, false otherwise
     */
    bool is_synchronized(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim,
                         tchecker::clock_id_t refcount);
    
    /*!
     \brief Equality predicate
     \param offset_dbm1 : a first offset DBM
     \param offset_dbm2 : a second offset DBM
     \param offset_dim : dimension of offset_dbm1 and offset_dbm2
     \pre offset_dbm1 and offset_dbm2 are not nullptr (checked by assertion)
     offset_dbm1 and offset_dbm2 are offset_dim*offset_dim arrays of difference bounds
     offset_dbm1 and offset_dbm2 are consistent (checked by assertion)
     offset_dbm1 and offset_dbm2 are tight (checked by assertion)
     offset_dim >= 1 (checked by assertion).
     \return true if offset_dbm1 and offset_dbm2 are equal, false otherwise
     */
    bool is_equal(tchecker::dbm::db_t const * offset_dbm1, tchecker::dbm::db_t const * offset_dbm2,
                  tchecker::clock_id_t offset_dim);
    
    /*!
     \brief Inclusion predicate
     \param offset_dbm1 : a first offset DBM
     \param offset_dbm2 : a second offset DBM
     \param offset_dim : dimension of offset_dbm1 and offset_dbm2
     \pre offset_dbm1 and offset_dbm2 are not nullptr (checked by assertion)
     offset_dbm1 and offset_dbm2 are offset_dim*offset_dim arrays of difference bounds
     offset_dbm1 and offset_dbm2 are consistent (checked by assertion)
     offset_dbm1 and offset_dbm2 are tight (checked by assertion)
     offset_dim >= 1 (checked by assertion).
     \return true if offset_dbm1 is included into offset_dbm2, false otherwise
     */
    bool is_le(tchecker::dbm::db_t const * offset_dbm1, tchecker::dbm::db_t const * offset_dbm2,
               tchecker::clock_id_t offset_dim);
    
    /*!
     \brief Hash function
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \return hash value for offset_dbm
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dim >= 1 (checked by assertion)
     \note if offset_dbm is not tight, the returned hash code may differ from the hash code of its corresponding tight DBM
     */
    std::size_t hash(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim);
    
    /*!
     \brief Constrain an offset DBM
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \param refcount : number of reference clocks
     \param refmap : map to reference clocks
     \param x : index of first variable
     \param y : index of second variable
     \param cmp : constraint comparator
     \param value : constraint value
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dbm is tight (checked by assertion)
     0 <= x < offset_dim (checked by assertion)
     0 <= y < offset_dim (checked by assertion)
     x != y (checked by assertion)
     offset_dim >= 1 (checked by assertion)
     \post offset_dbm has been intersected with constraint `x - y # value` where # is < if cmp is LT, and # is <= if cmp is LE
     offset_dbm is tight if it is not empty.
     if offset_dbm is empty, then its difference bound in (0,0) is less-than <=0 (tchecker::dbm::is_empty_0() returns true)
     \return EMPTY if offset_dbm is empty, NON_EMPTY otherwise
     \throw std::invalid_argument : if `cmp value` cannot be represented as a tchecker::dbm::db_t (only if compilation flag
     DBM_UNSAFE is not set)
     */
    enum tchecker::dbm::status_t
    constrain(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim, tchecker::clock_id_t x,
              tchecker::clock_id_t y, tchecker::dbm::comparator_t cmp, tchecker::integer_t value);
    
    /*!
     \brief Restriction to synchronized valuations
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \param refcount : number of reference clocks
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dbm is consistent (checked by assertion)
     offset_dbm is tight (checked by assertion)
     1 <= refcount <= offset_dim (checked by assertion)
     \post offset_dbm has been restricted to its subset of synchronized valuations
     \return tchecker::dbm::EMPTY if synchronized offset_dbm is empty, tchecker::dbm::NON_EMPTY otherwise
     */
    enum tchecker::dbm::status_t
    synchronize(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim,
                tchecker::clock_id_t refcount);
    
    /*!
     \brief Reset variable to its reference clock
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \param x : index of variable
     \param refcount : number of reference clocks
     \param refmap : map to reference clocks
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dbm is consistent (checked by assertion)
     offset_dbm is tight (checked by assertion)
     1 <= refcount <= offset_dim (checked by assertion)
     x < offset_dim (checked by assertion)
     refmap maps each variable in offset_dbm to its reference clock.
     \post variable x has been updated to value refmap[x] in offset_dbm, other
     variables are unchanged.
     offset_dbm is tight
     */
    void reset_to_refclock(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim,
                           tchecker::clock_id_t x, tchecker::clock_id_t refcount,
                           tchecker::clock_id_t const * refmap);
    
    /*!
     \brief Asynchronous open-up (delay)
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \param refcount : number of reference clocks
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dbm is consistent (checked by assertion)
     offset_dbm is tight (checked by assertion)
     1 <= refcount <= offset_dim (checked by assertion)
     the first refcount variables of offset_dbm are reference clocks.
     \post reference clocks in offset_dbm have no upper bound and are not related to each other
     (i.e. r1 - r2 < inf, for any two reference clocks r1 and r2, r1 != r2).
     offset_dbm is tight.
     */
    void asynchronous_open_up(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim,
                              tchecker::clock_id_t refcount);
    
    /*!
     \brief Asynchronous open-up (delay)
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \param refcount : number of reference clocks
     \param delay_allowed : reference clocks allowed to delay
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dbm is consistent (checked by assertion)
     offset_dbm is tight (checked by assertion)
     1 <= refcount <= offset_dim (checked by assertion)
     the first refcount variables of offset_dbm are reference clocks.
     delay_allowed has size refcount (checked by assertion)
     \post reference clocks in offset_dbm with delay_allowed are unbounded (i.e. x-r<inf for every reference clock r and
     any variable x, including x=r').
     reference clocks in offset_dbm without delay_allowed are unchanged
     offset_dbm is tight.
     */
    void asynchronous_open_up(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim,
                              tchecker::clock_id_t refcount, boost::dynamic_bitset<> const & delay_allowed);
    
    /*!
     \brief Tighten an offset DBM
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dim >= 1 (checked by assertion)
     \post offset_dbm is tight if offset_dbm is not empty.
     if offset_dbm is empty, then the difference bound in (0,0) is less-than <=0 (tchecker::dbm::is_empty_0() returns true)
     \return EMPTY if offset_dbm is empty, NON_EMPTY otherwise
     \note Applies Floyd-Warshall algorithm on offset_dbm seen as a weighted graph.
     */
    enum tchecker::dbm::status_t tighten(tchecker::dbm::db_t * offset_dbm, tchecker::clock_id_t offset_dim);
    
    /*!
     \brief Extract a DBM from an offset DBM
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \param refcount : number of reference clocks
     \param refmap : map to reference clocks
     \param dbm : a DBM
     \param dim : dimension of dbm
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is an offset_dim*offset_dim array of difference bounds (checked by assertion)
     1 <= refcount <= dim (checked by assertion)
     the first refcount clocks of offset_dbm are reference clocks.
     refmap is not nullptr (checked by assertion)
     refmap maps each variable in offset_dbm to its reference clock.
     offset_dbm is not empty.
     offset_dbm is consistent (checked by assertion)
     offset_dbm is tight (checked by assertion)
     offset_dbm is synchronized (checked by assertion)
     dbm is not nullptr (checked by assertion)
     dbm is a dim*dim array of difference bounds
     dim is equal to offset_dim - refcount + 1 (checked by assertion)
     \post dbm is the zone extracted from offset_dbm by identifying the reference clocks in offset_dbm to the
     0 clock in dbm.
     dbm is tight.
     */
    void to_dbm(tchecker::dbm::db_t const * offset_dbm, tchecker::clock_id_t offset_dim,
                tchecker::clock_id_t refcount, tchecker::clock_id_t const * refmap,
                tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim);
    
    /*!
     \brief Output an offset DBM as a matrix
     \param os : output stream
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dim >= 1 (checked by assertion).
     \post offset_dbm has been output to os
     \return os after output
     */
    std::ostream & output_matrix(std::ostream & os, tchecker::dbm::db_t const * offset_dbm,
                                 tchecker::clock_id_t offset_dim);
    
    /*!
     \brief Output an offset DBM as a conjunction of constraints
     \param os : output stream
     \param offset_dbm : an offset DBM
     \param offset_dim : dimension of offset_dbm
     \oaram clock_name : map from clock IDs to strings
     \pre offset_dbm is not nullptr (checked by assertion)
     offset_dbm is a offset_dim*offset_dim array of difference bounds
     offset_dim >= 1 (checked by assertion).
     clock_name maps any clock ID in [0,offset_dim) to a name
     \post the relevant constraints in offset_dbm has been output to os. Relevant constraints are those that differ
     from the universal DBM.
     \return os after output
     */
    std::ostream & output(std::ostream & os, tchecker::dbm::db_t const * offset_dbm,
                          tchecker::clock_id_t offset_dim,
                          std::function<std::string(tchecker::clock_id_t)> clock_name);

    /*!
     \brief Lexical ordering
     \param offset_dbm1 : first offset dbm
     \param offset_dim1 : dimension of offset_dbm1
     \param offset_dbm2 : second offset_dbm
     \param offset_dim2 : dimension of offset_dbm2
     \pre offset_dbm1 and offset_dbm2 are ot nullptr (checked by assertion)
     offset_dbm1 is a offset_dim1*offset_dim1 array of difference bounds
     offset_dbm2 is a offset_dim2*offset_dim2 array of difference bounds
     offset_dim1 >= 1 and offset_dim2 >= 1 (checked by assertion)
     \return 0 if offset_dbm1 and offset_dbm2 are equal, a negative value if offset_dbm1 is smaller than offset_dbm2 w.r.t. lexical ordering,
     and a positive value otherwise
     */
    int lexical_cmp(tchecker::dbm::db_t const * offset_dbm1, tchecker::clock_id_t offset_dim1,
                    tchecker::dbm::db_t const * offset_dbm2, tchecker::clock_id_t offset_dim2);
    
  } // end of namespace offset_dbm
  
} // end of namespace tchecker

#endif // TCHECKER_OFFSET_DBM_HH
