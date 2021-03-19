/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZONE_REFDBM_HH
#define TCHECKER_ZONE_REFDBM_HH

#include <memory>

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/dbm/dbm.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/variables/clocks.hh"
#include "tchecker/zone/zone.hh"

/*!
 \file zone.hh
 \brief reference DBM implementation of zones
 */

namespace tchecker {

namespace refdbm {

/*!
 \class zone_t
 \brief reference DBM implementation of zones
 */
class zone_t final : public tchecker::zone_t {
public:
  /*!
   \brief Assignment operator
   \param zone : a reference DBM zone
   \pre this and zone have the same dimension
   \post this is a copy of zone
   \return this after assignment
   \throw std::invalid_argument : if this and zone do not have the same dimension
   */
  tchecker::refdbm::zone_t & operator=(tchecker::refdbm::zone_t const & zone);

  /*!
   \brief Move assignment operator
   \note deleted (same as assignment operator)
   */
  tchecker::refdbm::zone_t & operator=(tchecker::refdbm::zone_t && zone) = delete;

  /*!
   \brief Emptiness check
   \return true if this zone is empty, false otherwise
   */
  virtual bool is_empty() const;

  /*!
   \brief Universal-positive check
   \return true if this zone is universal-positive (i.e. no constraint on clocks except x>=0), false otherwise
   */
  virtual bool is_universal_positive() const;

  /*!
   \brief Equality predicate
   \param zone : a reference DBM zone
   \return true if this is equal to zone (same shared reference clocks, same
   DBM), false otherwise
   \note Two zones that do not share the same reference clocks are seen as
   different even if their reference clocks are identical
   */
  bool operator==(tchecker::refdbm::zone_t const & zone) const;

  /*!
   \brief Equality predicate
   \param zone : a zone
   \return true if this is equal to zone, false otherwise
   \pre zone should be an instance of tchecker::refdbm::zone_t
   \note Two zones that do not share the same reference clocks are seen as
   different even if their reference clocks are identical
   */
  virtual bool operator==(tchecker::zone_t const & zone) const;

  /*!
   \brief Disequality predicate
   \param zone : a reference DBM zone
   \return true if this is not equal to zone, false otherwise
   \note Two zones that do not share the same reference clocks are seen as
   different even if their reference clocks are identical
   */
  bool operator!=(tchecker::refdbm::zone_t const & zone) const;

  /*!
   \brief Disequality predicate
   \param zone : a zone
   \return true if this is not equal to zone, false otherwise
   \pre zone should be an instance of tchecker::refdbm::zone_t
   \note Two zones that do not share the same reference clocks are seen as
   different even if their reference clocks are identical
   */
  virtual bool operator!=(tchecker::zone_t const & zone) const;

  /*!
   \brief Inclusion check
   \param zone : a reference DBM zone
   \return true if this is included or equal to zone
   \note Two zones that do not share the same reference clocks are seen as
   not included even if their reference clocks are identical
   */
  bool operator<=(tchecker::refdbm::zone_t const & zone) const;

  /*!
   \brief Inclusion check
   \param zone : a zone
   \return true if this is included or equal to zone
   \pre zone should be an instance of tchecker::refdbm::zone_t
   \note Two zones that do not share the same reference clocks are seen as
   not included even if their reference clocks are identical
   */
  virtual bool operator<=(tchecker::zone_t const & zone) const;

  /*!
   \brief Checks inclusion wrt abstraction aM
   \param zone : a reference DBM zone
   \param m : clock bounds
   \return true if this zone is included in aM(zone), false otherwise
   \pre m maps offset clocks to clock bounds (no bound for reference clocks)
   the first offset clock is indexed 0 in m
   \note Two zones that do not share the same reference clocks are seen as
   not included even if their reference clocks are identical
   */
  bool am_le(tchecker::refdbm::zone_t const & zone, tchecker::clockbounds::map_t const & m) const;

  /*!
   \brief Checks inclusion wrt abstraction aM
   \param zone : a zone
   \param M : clock bounds
   \return true if this zone is included in aM(zone), false otherwise
   \pre m maps offset clocks to clock bounds (no bound for reference clocks)
   the first offset clock is indexed 0 in m.
   zone should be an instance of tchecker::refdbm::zone_t
   \note Two zones that do not share the same reference clocks are seen as
   not included even if their reference clocks are identical
   */
  virtual bool am_le(tchecker::zone_t const & zone, tchecker::clockbounds::map_t const & M) const;

  /*!
   \brief Checks inclusion wrt abstraction aLU
   \param zone : a reference DBM  zone
   \param l : clock lower bounds
   \param u : clock upper bounds
   \return true if this zone is included in aLU(zone), false otherwise
   \pre l and u map offset clocks to clock bounds (no bound for reference clocks)
   the first offset clock is indexed 0 in l and u
   \note Two zones that do not share the same reference clocks are seen as
   not included even if their reference clocks are identical
   */
  bool alu_le(tchecker::refdbm::zone_t const & zone, tchecker::clockbounds::map_t const & l,
              tchecker::clockbounds::map_t const & u) const;

  /*!
   \brief Checks inclusion wrt abstraction aLU
   \param zone : a zone
   \param L : clock lower bounds
   \param U : clock upper bounds
   \return true if this zone is included in aLU(zone), false otherwise
   \pre l and u map offset clocks to clock bounds (no bound for reference clocks)
   the first offset clock is indexed 0 in l and u.
   zone should be an instance of tchecker::refdbm::zone_t
   \note Two zones that do not share the same reference clocks are seen as
   not included even if their reference clocks are identical
   */
  virtual bool alu_le(tchecker::zone_t const & zone, tchecker::clockbounds::map_t const & L,
                      tchecker::clockbounds::map_t const & U) const;

  /*!
   \brief Lexical ordering
   \param zone : a reference DBM zone
   \return 0 if this and zone are equal, a negative value if this is smaller
   than zone w.r.t. lexical ordering on the clock constraints, a positive value
   otherwise
   */
  int lexical_cmp(tchecker::refdbm::zone_t const & zone) const;

  /*!
   \brief Lexical ordering
   \param zone : a zone
   \return 0 if this and zone are equal, a negative value if this is smaller
   than zone w.r.t. lexical ordering on the clock constraints, a positive value
   otherwise
   \pre zone should be an instance of tchecker::refdbm::zone_t
   */
  virtual int lexical_cmp(tchecker::zone_t const & zone) const;

  /*!
   \brief Accessor
   \return hash code for this zone
   */
  virtual std::size_t hash() const;

  /*!
   \brief Accessor
   \return dimension of the zone
   */
  virtual std::size_t dim() const;

  /*!
  \brief Accessor
  \return reference clocks of this zone
  */
  std::shared_ptr<tchecker::reference_clock_variables_t const> reference_clock_variables() const;

  /*!
   \brief Output
   \param os : output stream
   \param index : clock index (map clock ID -> clock name)
   \post this zone has been output to os with clock names from index
   \return os after this zone has been output
   */
  virtual std::ostream & output(std::ostream & os, tchecker::clock_index_t const & index) const;

  /*!
   \brief Accessor
   \return internal DBM of size dim()*dim()
   \note Modifications to the returned DBM should ensure tightness or emptiness
   of the zone, following the convention defined in file tchecker/dbm/dbm.hh. It
   is thus strongly suggested to use the function defined in that file to modify
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
  virtual void to_dbm(tchecker::dbm::db_t * dbm) const;

  /*!
   \brief Construction
   \tparam ARGS : type of arguments to a constructor of tchecker::refdbm::zone_t
   \tparam ptr : pointer to an allocated zone
   \pre ptr points to an allocated zone of sufficient capacity, i.e. at least
   allocation_size_t<tchecker::refdbm::zone_t>::alloc_size(dim)
   \post an instance of thcecker::refdbm::zone_t has been built in ptr with
   parameters args
   */
  template <class... ARGS> static inline void construct(void * ptr, ARGS &&... args)
  {
    new (ptr) tchecker::refdbm::zone_t(args...);
  }

  /*!
   \brief Destruction
   \param ptr : a zone
   \post the destructor of tchecker::refdbm::zone_t has been called on ptr
   */
  static inline void destruct(tchecker::refdbm::zone_t * ptr) { ptr->~zone_t(); }

protected:
  /*!
   \brief Constructor
   \param ref_clocks : reference clocks
   \post this zone is the universal zone over ref_clocks
   */
  zone_t(std::shared_ptr<tchecker::reference_clock_variables_t const> const & ref_clocks);

  /*!
   \brief Copy constructor
   \param zone : a reference DBM zone
   \pre this has been allocated with the same dimension as zone
   \post this is a copy of zone
   */
  zone_t(tchecker::refdbm::zone_t const & zone);

  /*!
   \brief Move constructor
   \note deleted (move construction is the same as copy construction)
   */
  zone_t(tchecker::refdbm::zone_t && zone) = delete;

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
    return static_cast<tchecker::dbm::db_t *>(static_cast<void *>(const_cast<tchecker::refdbm::zone_t *>(this) + 1));
  }

  /*!
   \brief Accessor
   \param i : clock ID
   \param j : clock ID
   \return constraint on xi-xj in this DBM
   */
  constexpr tchecker::dbm::db_t dbm(tchecker::clock_id_t i, tchecker::clock_id_t j) const
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
inline std::size_t hash_value(tchecker::refdbm::zone_t const & zone) { return zone.hash(); }

} // namespace refdbm

/*!
 \class allocation_size_t
 \brief Specialization of class tchecker::allocation_size_t for type
 tchecker::refdbm::zone_t
 */
template <> class allocation_size_t<tchecker::refdbm::zone_t> {
public:
  /*!
   \brief Accessor
   \param ref_clocks : reference clocks
   \return Allocation size for objects of type tchecker::refdbm::zone_t
   with reference clocks ref_clocks
   */
  static constexpr std::size_t alloc_size(std::shared_ptr<tchecker::reference_clock_variables_t const> const & ref_clocks)
  {
    return (sizeof(tchecker::refdbm::zone_t) + ref_clocks->size() * ref_clocks->size() * sizeof(tchecker::dbm::db_t));
  }

  /*!
   \brief Accessor
   \param ref_clocks : reference clocks
   \param args : arguments to a constructor of class tchecker::refdbm::zone_t
   \return Allocation size for objects of type tchecker:ref:dbm::zone_t with
   reference clocks ref_clocks
   */
  template <class... ARGS>
  static constexpr std::size_t alloc_size(std::shared_ptr<tchecker::reference_clock_variables_t const> const & ref_clocks,
                                          ARGS &&... args)
  {
    return allocation_size_t<tchecker::refdbm::zone_t>::alloc_size(ref_clocks);
  }
};

// Allocation and deallocation

namespace refdbm {

/*!
 \brief Allocation and construction of reference DBM zones
 \param ref_clocks : reference clocks
 \param args : arguments to a constructor of tchecker::refdbm_zone_t
 \pre ref_clocks.size() >= 1
 \return an instance of tchecker::refdbm::zone_t of over reference clocks
 ref_clocks and constructed with args
 \throw std::invalid_argument if ref_clocks.size() < 1
 */
template <class... ARGS>
tchecker::refdbm::zone_t *
zone_allocate_and_construct(std::shared_ptr<tchecker::reference_clock_variables_t const> const & ref_clocks, ARGS &&... args)
{
  if (ref_clocks->size() < 1)
    throw std::invalid_argument("reference clocks size should be >= 1");

  void * ptr = new char[tchecker::allocation_size_t<tchecker::refdbm::zone_t>::alloc_size(ref_clocks)];
  tchecker::refdbm::zone_t::construct(ptr, args...);
  return reinterpret_cast<tchecker::refdbm::zone_t *>(ptr);
}

/*!
 \brief Destruction and deallocation of reference DBM zones
 \param zone : a reference DBM zone
 \pre pre has been allocated by tchecker::refdbm::zone_allocate_and_construct
 \post the destructor of zone has been called, and zone has been deleted
 */
void zone_destruct_and_deallocate(tchecker::refdbm::zone_t * zone);

} // namespace refdbm

} // end of namespace tchecker

#endif // TCHECKER_ZONE_REFDBM_HH
