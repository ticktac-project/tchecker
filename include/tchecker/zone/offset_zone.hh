/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_OFFSET_ZONE_HH
#define TCHECKER_OFFSET_ZONE_HH

/*!
 \file offset_zone.hh
 \brief Symbolic representations for offset zones
 */

namespace tchecker {
  
#ifdef HIDDEN_TO_COMPILER
  
  /*!
   \class offset_zone_t
   \brief Representation of an offset zone (interface)
   \note this class is only meant for documentation. It defines the methods
   that must be implemented for a given offset zone representation.
   */
  class offset_zone_t {
  public:
    /*!
     \brief Emptiness check
     \return true if this zone is empty, false otherwise
     */
    bool empty() const;
    
    /*!
     \brief Equality predicate
     \param zone : a zone
     \return true if this is equal to zone, false otherwise
     */
    bool operator== (/* actual zone type */ const & zone) const;
    
    /*!
     \brief Disequality predicate
     \param zone : a zone
     \return true if this is not equal to zone, false otherwise
     */
    bool operator!= (/* actual zone type */ const & zone) const;
    
    /*!
     \brief Inclusion check
     \param zone : a zone
     \return true if this is included or equal to zone
     */
    bool operator<= (/* actual zone type */ const & zone) const;
    
    /*!
     \brief Lexical ordering
     \param zone : a zone
     \return 0 if this and zone are equal, a negative value if this is smalelr than zone w.r.t.t lexical ordering on the clock constraints,
     a positive value otherwise
     */
    int lexical_cmp(/* actual zone type */ const & zone) const;
    
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
     \brief Output
     \param os : output stream
     \param index : clock index (map clock ID -> clock name)
     \post this zone has been output to os with clock names from index
     \return os after this zone has been output
     */
    std::ostream & output(std::ostream & os, tchecker::clock_index_t const & index) const;
  };
  
  
  
  
  /*!
   \brief Boost compatible hash function on zones
   \param zone : a zone
   \return hash value for zone
   */
  inline std::size_t hash_value(/* actual zone type */ const & zone)
  {
    return zone.hash();
  }
  
#endif // HIDDEN_TO_COMPILER
  
  
} // end of namespace tchecker

#endif // TCHECKER_OFFSET_ZONE_HH
