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
  
#ifdef HIDDEN_TO_COMPILER
  
  /*!
   \class zone_t
   \brief Representation of a zone (interface)
   \note this class is only meant for documentation. It defines the methods
   that must be implemented for a given zone representation.
   */
  class zone_t {
  public:
    /*!
     \brief Emptiness check
     \return true if this zone is empty, false otherwise
     */
    bool empty() const;
    
    /*!
     \brief Universality check
     \return true if this zone is universal, false otherwise
     */
    bool universal() const;
    
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
     \brief Checks inclusion wrt abstraction aM
     \param zone : a zone
     \param M : clock bounds
     \return true if this zone is include in aM(zone), false otherwise
     \pre clocks have same IDs in zone and M
     */
    bool aM_le(/* actual zone type */ const & zone, tchecker::clockbounds::map_t const & M) const;
    
    /*!
     \brief Checks inclusion wrt abstraction aLU
     \param zone : a zone
     \param L : clock lower bounds
     \param U : clock upper bounds
     \return true if this zone is included in aLU(zone), false otherwise
     \pre clocks have same IDs in zone, L and U
     */
    bool aLU_le(/* actual zone type */ const & zone,
                tchecker::clockbounds::map_t const & L,
                tchecker::clockbounds::map_t const & U) const;
    
    /*!
     \brief Lexical ordering
     \param zone : a zone
     \return 0 if this and zone are equal, a negative value if this is smaller than zone w.r.t. lexical ordering on the clock constraints,
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

#endif // TCHECKER_ZONE_HH
