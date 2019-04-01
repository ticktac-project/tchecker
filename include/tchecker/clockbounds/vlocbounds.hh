/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_CLOCKBOUNDS_VLOCBOUNDS_HH
#define TCHECKER_CLOCKBOUNDS_VLOCBOUNDS_HH

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/clockbounds.hh"

/*!
 \file vlocbounds.hh
 \brief Clock bounds for tuples of locations
 */

namespace tchecker {
  
  namespace clockbounds {
    
    /*!
     \brief Global LU clock bounds computation for a tuple of locations
     \tparam VLOC : type of tuple of locations, should instantiate tchecker::vloc_t<LOC> for a type
     LOC of locations that derive from tchecker::loc_t
     \param map : global LU map
     \param vloc : tuple of locations
     \param L : L clock bounds map
     \param U : U clock bounds map
     \pre L and U are allocated. L and U have capacity equal to the number of clocks in map (checked by assertion).
     Every location in vloc has an identifier in the range of identifiers known to map.
     \post L and U are the global L and U clock bounds map for vloc, according to map.
     \note It is usually more efficient to directly access the pointers inside map.L() and map.U(), than call this
     function
     */
    template <class VLOC>
    void vloc_bounds(tchecker::clockbounds::global_lu_map_t const & map,
                     VLOC const & vloc,
                     tchecker::clockbounds::map_t & L,
                     tchecker::clockbounds::map_t & U)
    {
      assert(L.capacity() == map.clock_number());
      assert(U.capacity() == map.clock_number());
      tchecker::clockbounds::clear(L);
      tchecker::clockbounds::clear(U);
      tchecker::clockbounds::update(L, map.L());
      tchecker::clockbounds::update(U, map.U());
    }
    
    
    
    
    /*!
     \brief Local LU clock bounds computation for a tuple of locations
     \tparam VLOC : type of tuple of locations, should instantiate tchecker::vloc_t<LOC> for a type
     LOC of locations that derive from tchecker::loc_t
     \param map : local LU map
     \param vloc : tuple of locations
     \param L : L clock bounds map
     \param U : U clock bounds map
     \pre L and U are allocated. L and U have capacity equal to the number of clocks in map (checked by assertion).
     Every location in vloc has an identifier in the range of identifiers known to map.
     \post L and U are the local L and U clock bounds map for vloc, according to map.
     */
    template <class VLOC>
    void vloc_bounds(tchecker::clockbounds::local_lu_map_t const & map,
                     VLOC const & vloc,
                     tchecker::clockbounds::map_t & L,
                     tchecker::clockbounds::map_t & U)
    {
      assert(L.capacity() == map.clock_number());
      assert(U.capacity() == map.clock_number());
      tchecker::clockbounds::clear(L);
      tchecker::clockbounds::clear(U);
      for (auto const * loc : vloc) {
        tchecker::loc_id_t loc_id = loc->id();
        tchecker::clockbounds::update(L, map.L(loc_id));
        tchecker::clockbounds::update(U, map.U(loc_id));
      }
    }
    
    
    
    
    /*!
     \brief Global M clock bounds computation for a tuple of locations
     \tparam VLOC : type of tuple of locations, should instantiate tchecker::vloc_t<LOC> for a type
     LOC of locations that derive from tchecker::loc_t
     \param map : global M map
     \param vloc : tuple of locations
     \param M : clock bounds map
     \pre M is allocated. M has capacity equal to the number of clocks in map (checked by assertion).
     Every location in vloc has an identifier in the range of identifiers known to map.
     \post M is the global clock bounds map for vloc, according to map.
     \note It is usually more efficient to directly access the pointer inside map.M(), than call this
     function
     */
    template <class VLOC>
    void vloc_bounds(tchecker::clockbounds::global_m_map_t const & map,
                     VLOC const & vloc,
                     tchecker::clockbounds::map_t & M)
    {
      assert(M.capacity() == map.clock_number());
      tchecker::clockbounds::clear(M);
      tchecker::clockbounds::update(M, map.M());
    }
    
    
    
    
    /*!
     \brief Local M clock bounds computation for a tuple of locations
     \tparam VLOC : type of tuple of locations, should instantiate tchecker::vloc_t<LOC> for a type
     LOC of locations that derive from tchecker::loc_t
     \param map : local M map
     \param vloc : tuple of locations
     \param M : clock bounds map
     \pre M is allocated. M has capacity equal to the number of clocks in map (checked by assertion).
     Every location in vloc has an identifier in the range of identifiers known to map.
     \post M is the local clock bounds map for vloc, according to map.
     */
    template <class VLOC>
    void vloc_bounds(tchecker::clockbounds::local_m_map_t const & map,
                     VLOC const & vloc,
                     tchecker::clockbounds::map_t & M)
    {
      assert(M.capacity() == map.clock_number());
      tchecker::clockbounds::clear(M);
      for (auto const * loc : vloc)
        tchecker::clockbounds::update(M, map.M(loc->id()));
    }
    
  } // end of namespace clockbounds
  
} // end of namespace tchecker

#endif // TCHECKER_CLOCKBOUNDS_VLOCBOUNDS_HH
