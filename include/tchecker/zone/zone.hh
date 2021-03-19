/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZONE_HH
#define TCHECKER_ZONE_HH

/*!
 \file zone.hh
 \brief Symbolic representations for zones
 */

namespace tchecker {

/*!
 \class zone_t
 \brief Representation of a zone (interface)
 \note this class is only meant for documentation. It defines the methods
 that must be implemented for a given zone representation.
 */
class zone_t {
public:
  /*!
  \brief Destructor
  */
  virtual ~zone_t() = default;

  /*!
   \brief Emptiness check
   \return true if this zone is empty, false otherwise
   */
  virtual bool is_empty() const = 0;

  /*!
   \brief Universal-positive check
   \return true if this zone is universal-positive (i.e. no constraint on clocks except x>=0), false otherwise
   */
  virtual bool is_universal_positive() const = 0;

  /*!
   \brief Equality predicate
   \param zone : a zone
   \return true if this is equal to zone, false otherwise
   */
  virtual bool operator==(tchecker::zone_t const & zone) const = 0;

  /*!
   \brief Disequality predicate
   \param zone : a zone
   \return true if this is not equal to zone, false otherwise
   */
  virtual bool operator!=(tchecker::zone_t const & zone) const = 0;

  /*!
   \brief Inclusion check
   \param zone : a zone
   \return true if this is included or equal to zone
   */
  virtual bool operator<=(tchecker::zone_t const & zone) const = 0;

  /*!
   \brief Checks inclusion wrt abstraction aM
   \param zone : a zone
   \param m : clock bounds
   \return true if this zone is include in aM(zone), false otherwise
   \pre m is a clock bound map over the clocks in zone
   */
  virtual bool am_le(tchecker::zone_t const & zone, tchecker::clockbounds::map_t const & m) const = 0;

  /*!
   \brief Checks inclusion wrt abstraction aLU
   \param zone : a zone
   \param l : clock lower bounds
   \param u : clock upper bounds
   \return true if this zone is included in aLU(zone), false otherwise
   \pre m is a clock bound map over the clocks in zone
   */
  virtual bool alu_le(tchecker::zone_t const & zone, tchecker::clockbounds::map_t const & l,
                      tchecker::clockbounds::map_t const & u) const = 0;

  /*!
   \brief Lexical ordering
   \param zone : a zone
   \return 0 if this and zone are equal, a negative value if this is smaller than zone w.r.t. lexical ordering on the clock
   constraints, a positive value otherwise
   */
  virtual int lexical_cmp(tchecker::zone_t const & zone) const = 0;

  /*!
   \brief Accessor
   \return hash code for this zone
   */
  virtual std::size_t hash() const = 0;

  /*!
   \brief Accessor
   \return dimension of the zone
   */
  virtual std::size_t dim() const = 0;

  /*!
   \brief Output
   \param os : output stream
   \param index : clock index (map clock ID -> clock name)
   \post this zone has been output to os with clock names from index
   \return os after this zone has been output
   */
  virtual std::ostream & output(std::ostream & os, tchecker::clock_index_t const & index) const = 0;

  /*!
  \brief Conversion to DBM
  \param dbm : a DBM
  \pre dbm is a dim() * dim() allocated DBM
  \post dbm contains a DBM representation of the zone.
  dbm is tight if the zone is not empty.
   */
  virtual void to_dbm(tchecker::dbm::db_t * dbm) const = 0;
};

/*!
 \brief Boost compatible hash function on zones
 \param zone : a zone
 \return hash value for zone
 */
inline std::size_t hash_value(tchecker::zone_t const & zone) { return zone.hash(); }

} // end of namespace tchecker

#endif // TCHECKER_ZONE_HH
