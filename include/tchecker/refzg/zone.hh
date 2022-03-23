/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_REFZG_ZONE_HH
#define TCHECKER_REFZG_ZONE_HH

#include <memory>
#include <string>

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/dbm/refdbm.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file zone.hh
 \brief Zones implemented as DBMs with reference clocks
 */

namespace tchecker {

namespace refzg {

/*!
 \class zone_t
 \brief Zones implemented as DBMs with reference clocks
 */
class zone_t {
public:
  /*!
   \brief Assignment operator
   \param zone : a reference DBM zone
   \pre this and zone have the same dimension
   \post this is a copy of zone
   \return this after assignment
   \throw std::invalid_argument : if this and zone do not have the same dimension
   */
  tchecker::refzg::zone_t & operator=(tchecker::refzg::zone_t const & zone);

  /*!
   \brief Move assignment operator
   \note deleted (same as assignment operator)
   */
  tchecker::refzg::zone_t & operator=(tchecker::refzg::zone_t && zone) = delete;

  /*!
   \brief Emptiness check
   \return true if this zone is empty, false otherwise
   */
  bool is_empty() const;

  /*!
   \brief Universal-positive check
   \return true if this zone is universal-positive (i.e. no constraint on clocks except x>=rx), false otherwise
   */
  bool is_universal_positive() const;

  /*!
   \brief Checks if the zone contains a synchronized valuation
   \return true is the zone contains a synchronized valuation, false otherwise
  */
  bool is_synchronizable() const;

  /*!
   \brief Equality predicate
   \param zone : a zone
   \return true if this is equal to zone (same shared reference clocks, same
   DBM), false otherwise
   \note Two zones that do not share the same reference clocks are seen as
   different even if their reference clocks are identical
   */
  bool operator==(tchecker::refzg::zone_t const & zone) const;

  /*!
   \brief Disequality predicate
   \param zone : a zone
   \return true if this is not equal to zone, false otherwise
   \note Two zones that do not share the same reference clocks are seen as
   different even if their reference clocks are identical
   */
  bool operator!=(tchecker::refzg::zone_t const & zone) const;

  /*!
   \brief Inclusion check
   \param zone : a zone
   \return true if this is included or equal to zone
   \note Two zones that do not share the same reference clocks are seen as
   not included even if their reference clocks are identical
   */
  bool operator<=(tchecker::refzg::zone_t const & zone) const;

  /*!
   \brief Checks inclusion wrt abstraction aM*
   \param zone : a zone
   \param m : clock bounds
   \return true if this zone is included in aM*(zone), false otherwise
   \pre m maps offset clocks to clock bounds (no bound for reference clocks)
   the first offset clock has index 0 in m
   \note Two zones that do not share the same reference clocks are seen as
   not included even if their reference clocks are identical
   */
  bool is_am_star_le(tchecker::refzg::zone_t const & zone, tchecker::clockbounds::map_t const & m) const;

  /*!
   \brief Checks inclusion wrt abstraction aLU*
   \param zone : a zone
   \param l : clock lower bounds
   \param u : clock upper bounds
   \return true if this zone is included in aLU*(zone), false otherwise
   \pre l and u map offset clocks to clock bounds (no bound for reference clocks)
   the first offset clock has index 0 in l and u
   \note Two zones that do not share the same reference clocks are seen as
   not included even if their reference clocks are identical
   */
  bool is_alu_star_le(tchecker::refzg::zone_t const & zone, tchecker::clockbounds::map_t const & l,
                      tchecker::clockbounds::map_t const & u) const;

  /*!
   \brief Checks inclusion wrt abstraction aM* combined with time-elapse
   \param zone : a zone
   \param m : clock bounds
   \return true if time-elapse(this zone) is included in aM*(time-elapse(zone)),
   false otherwise
   \pre m maps offset clocks to clock bounds (no bound for reference clocks)
   the first offset clock has index 0 in m
   \note Two zones that do not share the same reference clocks are seen as
   not included even if their reference clocks are identical
   */
  bool is_time_elapse_am_star_le(tchecker::refzg::zone_t const & zone, tchecker::clockbounds::map_t const & m) const;

  /*!
   \brief Checks inclusion wrt abstraction aLU* combined aith time-elapse
   \param zone : a zone
   \param l : clock lower bounds
   \param u : clock upper bounds
   \return true if time-elapse(this zone) is included in
   aLU*(time-elapse(zone)), false otherwise
   \pre l and u map offset clocks to clock bounds (no bound for reference clocks)
   the first offset clock has index 0 in l and u
   \note Two zones that do not share the same reference clocks are seen as
   not included even if their reference clocks are identical
   */
  bool is_time_elapse_alu_star_le(tchecker::refzg::zone_t const & zone, tchecker::clockbounds::map_t const & l,
                                  tchecker::clockbounds::map_t const & u) const;

  /*!
   \brief Checks inclusion wrt abstraction sync-aLU
   \param zone : a zone
   \param l : clock lower bounds
   \param u : clock upper bounds
   \return true if sync(this zone) is included in aLU(sync(zone)), false otherwise
   \pre l and u map offset clocks to clock bounds (no bound for reference clocks)
   the first offset clock has index 0 in l and u
   \note Two zones that do not share the same reference clocks are seen as
   not included even if their reference clocks are identical
   */
  bool is_sync_alu_le(tchecker::refzg::zone_t const & zone, tchecker::clockbounds::map_t const & l,
                      tchecker::clockbounds::map_t const & u) const;

  /*!
   \brief Lexical ordering
   \param zone : a zone
   \return 0 if this and zone are equal, a negative value if this is smaller
   than zone w.r.t. lexical ordering on the clock constraints, a positive value
   otherwise
   */
  int lexical_cmp(tchecker::refzg::zone_t const & zone) const;

  /*!
   \brief Accessor
   \return hash code for this zone
   */
  std::size_t hash() const;

  /*!
   \brief Accessor
   \return dimension of the zone
   */
  std::size_t dim() const;

  /*!
  \brief Accessor
  \return reference clocks of this zone
  */
  std::shared_ptr<tchecker::reference_clock_variables_t const> reference_clock_variables() const;

  /*!
   \brief Output
   \param os : output stream
   \param index : clock index (map clock ID -> clock name)
   \pre index maps all clocks (reference and offset) to a name
   \post this zone has been output to os with clock names from index
   \return os after this zone has been output
   */
  std::ostream & output(std::ostream & os, tchecker::clock_index_t const & index) const;

  /*!
   \brief Accessor
   \return internal DBM of size dim()*dim()
   \note Modifications to the returned DBM should ensure tightness or emptiness
   of the zone, following the convention defined in file tchecker/dbm/refdbm.hh.
   It is thus strongly suggested to use the function defined in that file to modify
   the returned DBM. Otherwise, the methods may not be accurate over this zone.
   */
  tchecker::dbm::db_t * dbm();

  /*!
   \brief Accessor
   \return internal DBM of size dim()*dim()
   */
  tchecker::dbm::db_t const * dbm() const;

  /*!
   \brief Conversion to DBM
   \param dbm : a DBM
   \pre dbm is a dim() * dim() allocated DBM
   \post dbm contains a DBM representation of the zone.
   dbm is tight if the zone is not empty.
   */
  void to_dbm(tchecker::dbm::db_t * dbm) const;

  /*!
   \brief Construction
   \tparam ARGS : type of arguments to a constructor of tchecker::refzg::zone_t
   \tparam ptr : pointer to an allocated zone
   \pre ptr points to an allocated zone of sufficient capacity, i.e. at least
   allocation_size_t<tchecker::refzg::zone_t>::alloc_size(dim)
   \post an instance of tchecker::refzg::zone_t has been built in ptr with
   parameters args
   */
  template <class... ARGS> static inline void construct(void * ptr, ARGS &&... args)
  {
    new (ptr) tchecker::refzg::zone_t(args...);
  }

  /*!
   \brief Destruction
   \param ptr : a zone
   \post the destructor of tchecker::refzg::zone_t has been called on ptr
   */
  static inline void destruct(tchecker::refzg::zone_t * ptr) { ptr->~zone_t(); }

protected:
  /*!
   \brief Constructor
   \param ref_clocks : reference clocks
   \post this zone is the universal positive zone over ref_clocks
   */
  zone_t(std::shared_ptr<tchecker::reference_clock_variables_t const> const & ref_clocks);

  /*!
   \brief Copy constructor
   \param zone : a reference DBM zone
   \pre this has been allocated with the same dimension as zone
   \post this is a copy of zone
   */
  zone_t(tchecker::refzg::zone_t const & zone);

  /*!
   \brief Move constructor
   \note deleted (move construction is the same as copy construction)
   */
  zone_t(tchecker::refzg::zone_t && zone) = delete;

  /*!
   \brief Destructor
   */
  virtual ~zone_t();

  /*!
   \brief Accessor
   \return pointer to DBM
   */
  constexpr tchecker::dbm::db_t * dbm_ptr() const
  {
    return static_cast<tchecker::dbm::db_t *>(static_cast<void *>(const_cast<tchecker::refzg::zone_t *>(this) + 1));
  }

  /*!
   \brief Accessor
   \param i : clock ID
   \param j : clock ID
   \return constraint on xi-xj in this DBM
   */
  tchecker::dbm::db_t dbm(tchecker::clock_id_t i, tchecker::clock_id_t j) const
  {
    return dbm_ptr()[i * _ref_clocks->size() + j];
  }

  std::shared_ptr<tchecker::reference_clock_variables_t const> _ref_clocks; /*!< Reference clocks */
};

/*!
 \brief Boost compatible hash function on zones
 \param zone : a reference DBM zone
 \return hash value for zone
 */
inline std::size_t hash_value(tchecker::refzg::zone_t const & zone) { return zone.hash(); }

} // end of namespace refzg

/*!
 \class allocation_size_t
 \brief Specialization of class tchecker::allocation_size_t for type
 tchecker::refzg::zone_t
 */
template <> class allocation_size_t<tchecker::refzg::zone_t> {
public:
  /*!
   \brief Accessor
   \param ref_clocks : reference clocks
   \return Allocation size for objects of type tchecker::refzg::zone_t
   with reference clocks ref_clocks
   */
  static std::size_t alloc_size(std::shared_ptr<tchecker::reference_clock_variables_t const> const & ref_clocks)
  {
    return (sizeof(tchecker::refzg::zone_t) + ref_clocks->size() * ref_clocks->size() * sizeof(tchecker::dbm::db_t));
  }

  /*!
   \brief Accessor
   \param ref_clocks : reference clocks
   \param args : arguments to a constructor of class tchecker::refzg::zone_t
   \return Allocation size for objects of type tchecker:refzg::zone_t with
   reference clocks ref_clocks
   */
  template <class... ARGS>
  static constexpr std::size_t alloc_size(std::shared_ptr<tchecker::reference_clock_variables_t const> const & ref_clocks,
                                          ARGS &&... args)
  {
    return allocation_size_t<tchecker::refzg::zone_t>::alloc_size(ref_clocks);
  }
};

// Allocation and deallocation

namespace refzg {

/*!
 \brief Allocation and construction of zones implemented by DBMs with reference clocks
 \param ref_clocks : reference clocks
 \param args : arguments to a constructor of tchecker::refzg::zone_t
 \pre ref_clocks.size() >= 1
 \return an instance of tchecker::refzg::zone_t of over reference clocks
 ref_clocks and constructed with args
 \throw std::invalid_argument if ref_clocks.size() < 1
 */
template <class... ARGS>
tchecker::refzg::zone_t *
zone_allocate_and_construct(std::shared_ptr<tchecker::reference_clock_variables_t const> const & ref_clocks, ARGS &&... args)
{
  if (ref_clocks->size() < 1)
    throw std::invalid_argument("reference clocks size should be >= 1");

  void * ptr = new char[tchecker::allocation_size_t<tchecker::refzg::zone_t>::alloc_size(ref_clocks)];
  tchecker::refzg::zone_t::construct(ptr, args...);
  return reinterpret_cast<tchecker::refzg::zone_t *>(ptr);
}

/*!
 \brief Destruction and deallocation of zones implemented by DBMs with reference
 clocks
 \param zone : a zone
 \pre pre has been allocated by tchecker::refzg::zone_allocate_and_construct
 \post the destructor of zone has been called, and zone has been deleted
 */
void zone_destruct_and_deallocate(tchecker::refzg::zone_t * zone);

} // end of namespace refzg

/*!
 \brief Output to string
 \param zone : z zone
 \param index : clock index (map clock ID -> clock name)
 \return a string corresponding to the output of zone using index
 */
std::string to_string(tchecker::refzg::zone_t const & zone, tchecker::clock_index_t const & index);

} // end of namespace tchecker

#endif // TCHECKER_REFZG_ZONE_HH
