/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_CLOCKBOUNDS_HH
#define TCHECKER_CLOCKBOUNDS_HH

#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/utils/array.hh"

/*!
 \file clockbounds.hh
 \brief Clock bounds in a system
 */

namespace tchecker {

namespace clockbounds {

/*!
 \brief Type of a clock bound
 */
using bound_t = tchecker::integer_t;

/*!
 \brief Encoding of absence of bound
 */
tchecker::clockbounds::bound_t const NO_BOUND = -tchecker::dbm::INF_VALUE;

/*!
 \brief Max clock bound
 */
tchecker::clockbounds::bound_t const MAX_BOUND = tchecker::dbm::MAX_VALUE;

static_assert(tchecker::clockbounds::NO_BOUND != tchecker::clockbounds::MAX_BOUND, "");

/*!
 \brief Type of map from clock ID to clock bound
 */
using map_t = tchecker::make_array_t<tchecker::clockbounds::bound_t, sizeof(tchecker::clockbounds::bound_t),
                                     tchecker::array_capacity_t<tchecker::clock_id_t>>;

/*!
 \brief Clock bound map allocation
 \param clock_nb : number of clocks
 \return A clock bound map of domain [0..clock_nb)
 */
tchecker::clockbounds::map_t * allocate_map(std::size_t clock_nb);

/*!
 \brief Clone clock bound map
 \param m : map
 \return A clone of m
 */
tchecker::clockbounds::map_t * clone_map(tchecker::clockbounds::map_t const & m);

/*!
 \brief Clock bound map deallocation
 \param m : clock bound map
 \post m has been deallocated
 */
void deallocate_map(tchecker::clockbounds::map_t * m);

/*!
 \brief Clear a clock bound map
 \param map : clock bound map
 \post the bound of each clock has been set to tchecker::clockbounds::NO_BOUND
 */
void clear(tchecker::clockbounds::map_t & map);

/*!
 \brief Update a clock bound map
 \param map : clock bound map
 \param id : clock id
 \param bound : clock bound
 \pre id is a valid clock identifier w.r.t. map (checked by assertion)
 \post map[id] has been updated to the max of map[id] and bound
 \return true is map[id] has been modified, false otherwise
 */
bool update(tchecker::clockbounds::map_t & map, tchecker::clock_id_t id, tchecker::clockbounds::bound_t bound);

/*!
 \brief Update a clock bound map
 \param map : clock bound map
 \param upd : clock bound map
 \pre map and upd have same capacity (checked by assertion)
 \post map has been updated to the max of map and upd
 \return true is map has been modified, false otherwise
 */
bool update(tchecker::clockbounds::map_t & map, tchecker::clockbounds::map_t const & upd);

/*!
 \brief Copy clock bound map
 \param dst : clock bound map
 \param src : clock bound map
 \pre dst and src have same capacity (checked by assertion)
 \post src has been copied into dst
 */
void copy(tchecker::clockbounds::map_t & dst, tchecker::clockbounds::map_t const & src);

/*!
 \brief Output operator
 \param os : output stream
 \param map : clock bound map
 \post map has been output to os
 \return os after map has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::clockbounds::map_t const & map);

/*!
 \class local_lu_map_t
 \brief Map from system locations to LU clock bound maps
 */
class local_lu_map_t {
public:
  /*!
   \brief Constructor
   \param loc_nb : number of locations
   \param clock_nb : number of clocks
   \note all location ID in [0..loc_nb) are assumed to be valid
   and all clock ID in [0..clock_nb) are assumed to be valid
   */
  local_lu_map_t(tchecker::loc_id_t loc_nb, tchecker::clock_id_t clock_nb);

  /*!
   \brief Copy constructor
   \param m : map
   \brief this is a copy of m
   */
  local_lu_map_t(tchecker::clockbounds::local_lu_map_t const & m);

  /*!
   \brief Move constructor
   \param m : map
   \post m has been moved to this
   */
  local_lu_map_t(tchecker::clockbounds::local_lu_map_t && m);

  /*!
   \brief Destructor
   */
  ~local_lu_map_t();

  /*!
   \brief Assignment operator
   \param m : map
   \post this is a copy of m
   */
  tchecker::clockbounds::local_lu_map_t & operator=(tchecker::clockbounds::local_lu_map_t const & m);

  /*!
   \brief Move-assignment operator
   \param m : map
   \post m has been moved to this
   */
  tchecker::clockbounds::local_lu_map_t & operator=(tchecker::clockbounds::local_lu_map_t && m);

  /*!
   \brief Clear the map
   \post This map is empty: locations number and clocks number are 0, and all memory has been deallocated
  */
  void clear();

  /*!
   \brief Resize the map
   \param loc_nb : number of locations
   \param clock_nb : number of clocks
   \note all location ID in [0..loc_nb) are assumed to be valid
   and all clock ID in [0..clock_nb) are assumed to be valid
   \post this map has been cleared and resized to loc_nb locations and clock_nb clocks
   */
  void resize(tchecker::loc_id_t loc_nb, tchecker::clock_id_t clock_nb);

  /*!
   \brief Accessor
   \return Number of locations
   */
  tchecker::loc_id_t loc_number() const;

  /*!
   \brief Accessor
   \return Number of clocks
   */
  tchecker::clock_id_t clock_number() const;

  /*!
   \brief Accessor
   \param id : location ID
   \return L clock bound map for location id
   \pre 0 <= id < _loc_nb (checked by assertion)
   _clock_nb > 0 (checked by assertion)
   */
  tchecker::clockbounds::map_t & L(tchecker::loc_id_t id);

  /*!
   \brief Accessor
   \param id : location ID
   \return L clock bound map for location id
   \pre 0 <= id < _loc_nb (checked by assertion)
   _clock_nb > 0 (checked by assertion)
   */
  tchecker::clockbounds::map_t const & L(tchecker::loc_id_t id) const;

  /*!
   \brief Accessor
   \param id : location ID
   \return U clock bound map for location id
   \pre 0 <= id < _loc_nb (checked by assertion)
   _clock_nb > 0 (checked by assertion)
   */
  tchecker::clockbounds::map_t & U(tchecker::loc_id_t id);

  /*!
   \brief Accessor
   \param id : location ID
   \return U clock bound map for location id
   \pre 0 <= id < _loc_nb (checked by assertion)
   _clock_nb > 0 (checked by assertion)
   */
  tchecker::clockbounds::map_t const & U(tchecker::loc_id_t id) const;

  /*!
  \brief Accessor
  \param id : location identifier
  \param L : clock lower-bound map
  \param U : clock upper-bound map
  \pre 0 <= id < _loc_nb (checked by assertion)
  _clock_nb > 0 (checked by assertion)
  \post L and U are the lower-bound and upper-bound maps for location id
  */
  void bounds(tchecker::loc_id_t id, tchecker::clockbounds::map_t & L, tchecker::clockbounds::map_t & U) const;

  /*!
  \brief Accessor
  \param vloc : tuple of location identifiers
  \param L : clock lower-bound map
  \param U : clock upper-bound map
  \pre all locations identifiers in vloc are in [0.._loc_nb) (checked by assertion)
  _clock_nb > 0 (checked by assertion)
  \post L and U are the lower-bound and upper-bound maps for vloc
  */
  void bounds(tchecker::vloc_t const & vloc, tchecker::clockbounds::map_t & L, tchecker::clockbounds::map_t & U) const;

private:
  tchecker::loc_id_t _loc_nb;                     /*!< Number of system locations */
  tchecker::clock_id_t _clock_nb;                 /*!< Number of clocks */
  std::vector<tchecker::clockbounds::map_t *> _L; /*!< Clock lower-bound map */
  std::vector<tchecker::clockbounds::map_t *> _U; /*!< Clock upper-bound map */
};

/*!
 \brief Output operator
 \param os : output stream
 \param map : local LU map
 \post map has been output to os
 \return os after map has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::clockbounds::local_lu_map_t const & map);

/*!
 \class global_lu_map_t
 \brief Map from system to LU clock bound maps
 */
class global_lu_map_t {
public:
  /*!
   \brief Constructor
   \param clock_nb : number of clocks
   \note All clock ID in [0..clock_nb) are assumed to be valid
   */
  global_lu_map_t(tchecker::clock_id_t clock_nb);

  /*!
   \brief Copy constructor
   \param m : map
   \brief this is a copy of m
   */
  global_lu_map_t(tchecker::clockbounds::global_lu_map_t const & m);

  /*!
   \brief Move constructor
   \param m : map
   \post m has been moved to this
   */
  global_lu_map_t(tchecker::clockbounds::global_lu_map_t && m);

  /*!
   \brief Destructor
   */
  ~global_lu_map_t();

  /*!
   \brief Assignment operator
   \param m : map
   \post this is a copy of m
   */
  tchecker::clockbounds::global_lu_map_t & operator=(tchecker::clockbounds::global_lu_map_t const & m);

  /*!
   \brief Move-assignment operator
   \param m : map
   \post m has been moved to this
   */
  tchecker::clockbounds::global_lu_map_t & operator=(tchecker::clockbounds::global_lu_map_t && m);

  /*!
   \brief Clear the map
   \post This map is empty: clocks number is 0, and all memory has been deallocated
  */
  void clear();

  /*!
   \brief Resize the map
   \param clock_nb : number of clocks
   \note all clock ID in [0..clock_nb) are assumed to be valid
   \post this map has been cleared and resized to clock_nb clocks
   */
  void resize(tchecker::clock_id_t clock_nb);

  /*!
   \brief Accessor
   \return Number of clocks
   */
  tchecker::clock_id_t clock_number() const;

  /*!
   \brief Accessor
   \pre _clock_nb > 0 (checked by assertion)
   \return L clock bound map
   */
  tchecker::clockbounds::map_t & L(void);

  /*!
   \brief Accessor
   \pre _clock_nb > 0 (checked by assertion)
   \return L clock bound map
   */
  tchecker::clockbounds::map_t const & L(void) const;

  /*!
   \brief Accessor
   \pre _clock_nb > 0 (checked by assertion)
   \return U clock bound map
   */
  tchecker::clockbounds::map_t & U(void);

  /*!
   \brief Accessor
   \pre _clock_nb > 0 (checked by assertion)
   \return U clock bound map
   */
  tchecker::clockbounds::map_t const & U(void) const;

  /*!
  \brief Accessor
  \param L : clock lower-bound map
  \param U : clock upper-bound map
  \pre _clock_nb > 0 (checked by assertion)
  \post L and U are the global lower-bound and global upper-bound maps
  */
  void bounds(tchecker::clockbounds::map_t & L, tchecker::clockbounds::map_t & U) const;

  /*!
  \brief Accessor
  \param id : location identifier
  \param L : clock lower-bound map
  \param U : clock upper-bound map
  \pre _clock_nb > 0 (checked by assertion)
  \post L and U are the global lower-bound and global upper-bound maps
  */
  void bounds(tchecker::loc_id_t id, tchecker::clockbounds::map_t & L, tchecker::clockbounds::map_t & U) const;

  /*!
  \brief Accessor
  \param vloc : tuple of location identifiers
  \param L : clock lower-bound map
  \param U : clock upper-bound map
  \pre _clock_nb > 0 (checked by assertion)
  \post L and U are the global lower-bound and global upper-bound maps
  */
  void bounds(tchecker::vloc_t const & vloc, tchecker::clockbounds::map_t & L, tchecker::clockbounds::map_t & U) const;

private:
  tchecker::clock_id_t _clock_nb;    /*!< Number of clocks */
  tchecker::clockbounds::map_t * _L; /*!< Clock lower-bound map */
  tchecker::clockbounds::map_t * _U; /*!< Clock upper-bound map */
};

/*!
 \brief Output operator
 \param os : output stream
 \param map : global LU map
 \post map has been output to os
 \return os after map has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::clockbounds::global_lu_map_t const & map);

/*!
 \brief Fill a global LU map from a local LU map
 \param global_lu_map : global LU map
 \param local_lu_map : local LU map
 \pre global_lu_map and local_lu_map havs same number of clocks
 \post the bounds in global_lu_map have been set to the maximum bounds over all locations
 in local_lu_map
 \throw std::invalid_argument : if global_lu_map and local_lu_map do not have the same number
 of clocks
 */
void fill_global_lu_map(tchecker::clockbounds::global_lu_map_t & global_lu_map,
                        tchecker::clockbounds::local_lu_map_t const & local_lu_map);

/*!
 \class local_m_map_t
 \brief Map from system locations to M clock bound maps
 */
class local_m_map_t {
public:
  /*!
   \brief Constructor
   \param loc_nb : number of locations
   \param clock_nb : number of clocks
   \note All location ID in [0..loc_nb) are assumed to be valid
   and all clock ID in [0..clock_nb) are assumed to be valid
   */
  local_m_map_t(tchecker::loc_id_t loc_nb, tchecker::clock_id_t clock_nb);

  /*!
   \brief Copy constructor
   \param m : map
   \brief this is a copy of m
   */
  local_m_map_t(tchecker::clockbounds::local_m_map_t const & m);

  /*!
   \brief Move constructor
   \param m : map
   \post m has been moved to this
   */
  local_m_map_t(tchecker::clockbounds::local_m_map_t && m);

  /*!
   \brief Destructor
   */
  ~local_m_map_t();

  /*!
   \brief Assignment operator
   \param m : map
   \post this is a copy of m
   */
  tchecker::clockbounds::local_m_map_t & operator=(tchecker::clockbounds::local_m_map_t const & m);

  /*!
   \brief Move-assignment operator
   \param m : map
   \post m has been moved to this
   */
  tchecker::clockbounds::local_m_map_t & operator=(tchecker::clockbounds::local_m_map_t && m);

  /*!
   \brief Clear the map
   \post This map is empty: locations number and clocks number are 0, and all memory has been deallocated
  */
  void clear();

  /*!
   \brief Resize the map
   \param loc_nb : number of locations
   \param clock_nb : number of clocks
   \note all location ID in [0..loc_nb) are assumed to be valid
   and all clock ID in [0..clock_nb) are assumed to be valid
   \post this map has been cleared and resized to loc_nb locations and clock_nb clocks
   */
  void resize(tchecker::loc_id_t loc_nb, tchecker::clock_id_t clock_nb);

  /*!
   \brief Accessor
   \return Number of locations
   */
  tchecker::loc_id_t loc_number() const;

  /*!
   \brief Accessor
   \return Number of clocks
   */
  tchecker::clock_id_t clock_number() const;

  /*!
   \brief Accessor
   \param id : location ID
   \return m clock bound map for location id
   \pre 0 <= id < _loc_nb (checked by assertion)
   _clock_nb > 0 (checked by assertion)
   */
  tchecker::clockbounds::map_t & M(tchecker::loc_id_t id);

  /*!
   \brief Accessor
   \param id : location ID
   \return m clock bound map for location id
   \pre 0 <= id < _loc_nb (checked by assertion)
   _clock_nb > 0 (checked by assertion)
   */
  tchecker::clockbounds::map_t const & M(tchecker::loc_id_t id) const;

  /*!
  \brief Accessor
  \param id : location identifier
  \param M : clock bound map
  \pre _clock_nb > 0 (checked by assertion)
  \post M is the clock bound map for location id
  */
  void bounds(tchecker::loc_id_t id, tchecker::clockbounds::map_t & M) const;

  /*!
  \brief Accessor
  \param vloc : tuple of location identifiers
  \param M : clock bound map
  \pre all locations identifiers in vloc are in [0.._loc_nb) (checked by assertion)
  _clock_nb > 0 (checked by assertion)
  \post M is the clock bound map for vloc
  */
  void bounds(tchecker::vloc_t const & vloc, tchecker::clockbounds::map_t & M) const;

private:
  tchecker::loc_id_t _loc_nb;                     /*!< Number of system locations */
  tchecker::clock_id_t _clock_nb;                 /*!< Number of clocks */
  std::vector<tchecker::clockbounds::map_t *> _M; /*!< Clock bound map */
};

/*!
 \brief Output operator
 \param os : output stream
 \param map : local m map
 \post map has been output to os
 \return os after map has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::clockbounds::local_m_map_t const & map);

/*!
 \brief Fill a local M map from a local LU map
 \param local_m_map : local M map
 \param local_lu_map : local LU map
 \pre local_m_map and local_lu_map havs same number of clocks and locations
 \post the bounds in local_m_map have been set to the maximum of L and U bounds for each clock
 and location
 \throw std::invalid_argument : if local_m_map and local_lu_map do not have the same number
 of clocks or locations
 */
void fill_local_m_map(tchecker::clockbounds::local_m_map_t & local_m_map,
                      tchecker::clockbounds::local_lu_map_t const & local_lu_map);

/*!
 \class global_m_map_t
 \brief Map from system to M clock bound maps
 */
class global_m_map_t {
public:
  /*!
   \brief Constructor
   \param clock_nb : number of clocks
   \note All clock ID in [0..clock_nb) are assumed to be valid
   */
  global_m_map_t(tchecker::clock_id_t clock_nb);

  /*!
   \brief Copy constructor
   \param m : map
   \brief this is a copy of m
   */
  global_m_map_t(tchecker::clockbounds::global_m_map_t const & m);

  /*!
   \brief Move constructor
   \param m : map
   \post m has been moved to this
   */
  global_m_map_t(tchecker::clockbounds::global_m_map_t && m);

  /*!
   \brief Destructor
   */
  ~global_m_map_t();

  /*!
   \brief Assignment operator
   \param m : map
   \post this is a copy of m
   */
  tchecker::clockbounds::global_m_map_t & operator=(tchecker::clockbounds::global_m_map_t const & m);

  /*!
   \brief Move-assignment operator
   \param m : map
   \post m has been moved to this
   */
  tchecker::clockbounds::global_m_map_t & operator=(tchecker::clockbounds::global_m_map_t && m);

  /*!
   \brief Clear the map
   \post This map is empty: clocks number is 0, and all memory has been deallocated
  */
  void clear();

  /*!
   \brief Resize the map
   \param clock_nb : number of clocks
   \note all clock ID in [0..clock_nb) are assumed to be valid
   \post this map has been cleared and resized to clock_nb clocks
   */
  void resize(tchecker::clock_id_t clock_nb);

  /*!
   \brief Accessor
   \return Number of clocks
   */
  tchecker::clock_id_t clock_number() const;

  /*!
   \brief Accessor
   \pre _clock_nb > 0 (checked by assertion)
   \return M clock bound map
   */
  tchecker::clockbounds::map_t & M(void);

  /*!
   \brief Accessor
   \pre _clock_nb > 0 (checked by assertion)
   \return M clock bound map
   */
  tchecker::clockbounds::map_t const & M(void) const;

  /*!
  \brief Accessor
  \param M : clock bound map
  \pre _clock_nb > 0 (checked by assertion)
  \post M is the global clock bound map
  */
  void bounds(tchecker::clockbounds::map_t & M) const;

  /*!
  \brief Accessor
  \param id : location identifier
  \param M : clock bound map
  \pre _clock_nb > 0 (checked by assertion)
  \post M is the global clock bound map
  */
  void bounds(tchecker::loc_id_t id, tchecker::clockbounds::map_t & M) const;

  /*!
  \brief Accessor
  \param vloc : tuple of location identifiers
  \param M : clock bound map
  \pre _clock_nb > 0 (checked by assertion)
  \post M is the global clock bound map
  */
  void bounds(tchecker::vloc_t const & vloc, tchecker::clockbounds::map_t & M) const;

private:
  tchecker::clock_id_t _clock_nb;    /*!< Number of clocks */
  tchecker::clockbounds::map_t * _M; /*!< Clock bound map */
};

/*!
 \brief Output operator
 \param os : output stream
 \param map : global m map
 \post map has been output to os
 \return os after map has been output
 */
std::ostream & operator<<(std::ostream & os, tchecker::clockbounds::global_m_map_t const & map);

/*!
 \brief Fill a global M map from a local LU map
 \param global_m_map : global M map
 \param local_lu_map : local LU map
 \pre global_m_map and local_lu_map havs same number of clocks
 \post the bounds in global_m_map have been set to the maximum of L and U bounds for each clock
 over all locations
 \throw std::invalid_argument : if global_m_map and local_lu_map do not have the same number
 of clocks
 */
void fill_global_m_map(tchecker::clockbounds::global_m_map_t & global_m_map,
                       tchecker::clockbounds::local_lu_map_t const & local_lu_map);

/*!
\class clockbounds_t
\brief Clock bounds for timed automata
*/
class clockbounds_t {
public:
  /*!
  \brief Constructor
  \param loc_nb : number of locations
  \param clock_nb : number of clocks
  */
  clockbounds_t(tchecker::loc_id_t loc_nb = 0, tchecker::clock_id_t clock_nb = 0);

  /*!
  \brief Copy constructor
  */
  clockbounds_t(tchecker::clockbounds::clockbounds_t const &) = default;

  /*!
  \brief Move constructor
  */
  clockbounds_t(tchecker::clockbounds::clockbounds_t &&) = default;

  /*!
  \brief Destructor
  */
  ~clockbounds_t() = default;

  /*!
  \brief Assignment operator
  */
  tchecker::clockbounds::clockbounds_t & operator=(tchecker::clockbounds::clockbounds_t const &) = default;

  /*!
  \brief Move-assignment operator
  */
  tchecker::clockbounds::clockbounds_t & operator=(tchecker::clockbounds::clockbounds_t &&) = default;

  /*!
   \brief Clear clock bounds
   \post This clock bounds is empty: locations number and clocks number are 0, and all memory has been deallocated
  */
  void clear();

  /*!
   \brief Resize the clock bounds mapsa
   \param loc_nb : number of locations
   \param clock_nb : number of clocks
   \note all location ID in [0..loc_nb) are assumed to be valid
   and all clock ID in [0..clock_nb) are assumed to be valid
   \post all clock bounds has been cleared and resized to loc_nb locations and clock_nb clocks
   */
  void resize(tchecker::loc_id_t loc_nb, tchecker::clock_id_t clock_nb);

  /*!
  \brief Accessor
  \return global LU map (const)
  */
  std::shared_ptr<tchecker::clockbounds::global_lu_map_t const> global_lu_map() const;

  /*!
  \brief Accessor
  \return global LU map (non const)
  */
  std::shared_ptr<tchecker::clockbounds::global_lu_map_t> global_lu_map();

  /*!
  \brief Accessor
  \return global M map (const)
  */
  std::shared_ptr<tchecker::clockbounds::global_m_map_t const> global_m_map() const;

  /*!
  \brief Accessor
  \return global M map (non const)
  */
  std::shared_ptr<tchecker::clockbounds::global_m_map_t> global_m_map();

  /*!
  \brief Accessor
  \return local LU map (const)
  */
  std::shared_ptr<tchecker::clockbounds::local_lu_map_t const> local_lu_map() const;

  /*!
  \brief Accessor
  \return local LU map (non const)
  */
  std::shared_ptr<tchecker::clockbounds::local_lu_map_t> local_lu_map();

  /*!
  \brief Accessor
  \return local M map (const)
  */
  std::shared_ptr<tchecker::clockbounds::local_m_map_t const> local_m_map() const;

  /*!
  \brief Accessor
  \return local M map (non const)
  */
  std::shared_ptr<tchecker::clockbounds::local_m_map_t> local_m_map();

  /*!
  \brief Accessor
  \param id : location identifier
  \param L : clock lower-bound map
  \param U : clock upper-bound map
  \pre see tchecker::clockbounds::local_lu_map_t::bounds
  \post L and U are the local lower-bound and local upper-bound maps for location id
  */
  inline void local_lu(tchecker::loc_id_t id, tchecker::clockbounds::map_t & L, tchecker::clockbounds::map_t & U) const
  {
    return _local_lu->bounds(id, L, U);
  }

  /*!
  \brief Accessor
  \param vloc : tuple of location identifiers
  \param L : clock lower-bound map
  \param U : clock upper-bound map
  \pre see tchecker::clockbounds::local_lu_map_t::bounds
  \post L and U are the local lower-bound and local upper-bound maps for vloc
  */
  inline void local_lu(tchecker::vloc_t const & vloc, tchecker::clockbounds::map_t & L, tchecker::clockbounds::map_t & U) const
  {
    return _local_lu->bounds(vloc, L, U);
  }

  /*!
  \brief Accessor
  \param id : location identifier
  \param M : clock bound map
  \pre see tchecker::clockbounds::local_m_map_t::bounds
  \post M is the local bound map for location id
  */
  inline void local_m(tchecker::loc_id_t id, tchecker::clockbounds::map_t & M) const { return _local_m->bounds(id, M); }

  /*!
  \brief Accessor
  \param vloc : tuple of location identifiers
  \param M : clock bound map
  \pre see tchecker::clockbounds::local_m_map_t::bounds
  \post M is the local bound map for vloc
  */
  inline void local_m(tchecker::vloc_t const & vloc, tchecker::clockbounds::map_t & M) const
  {
    return _local_m->bounds(vloc, M);
  }

  /*!
  \brief Accessor
  \param L : clock lower-bound map
  \param U : clock upper-bound map
  \pre see tchecker::clockbounds::global_lu_map_t::bounds
  \post L and U are the global lower-bound and global upper-bound maps
  */
  inline void global_lu(tchecker::clockbounds::map_t & L, tchecker::clockbounds::map_t & U) const
  {
    return _global_lu->bounds(L, U);
  }

  /*!
  \brief Accessor
  \param id : location identifier
  \param L : clock lower-bound map
  \param U : clock upper-bound map
  \pre see tchecker::clockbounds::global_lu_map_t::bounds
  \post L and U are the global lower-bound and global upper-bound maps for location id
  */
  inline void global_lu(tchecker::loc_id_t id, tchecker::clockbounds::map_t & L, tchecker::clockbounds::map_t & U) const
  {
    return _global_lu->bounds(id, L, U);
  }

  /*!
  \brief Accessor
  \param vloc : tuple of location identifiers
  \param L : clock lower-bound map
  \param U : clock upper-bound map
  \pre see tchecker::clockbounds::global_lu_map_t::bounds
  \post L and U are the global lower-bound and global upper-bound maps for vloc
  */
  inline void global_lu(tchecker::vloc_t const & vloc, tchecker::clockbounds::map_t & L, tchecker::clockbounds::map_t & U) const
  {
    return _global_lu->bounds(vloc, L, U);
  }

  /*!
  \brief Accessor
  \param M : clock bound map
  \pre see tchecker::clockbounds::global_m_map_t::bounds
  \post M is the global bound map
  */
  inline void global_m(tchecker::clockbounds::map_t & M) const { return _global_m->bounds(M); }

  /*!
  \brief Accessor
  \param id : location identifier
  \param M : clock bound map
  \pre see tchecker::clockbounds::global_lu_map_t::bounds
  \post M is the global bound map
  */
  inline void global_m(tchecker::loc_id_t id, tchecker::clockbounds::map_t & M) const { return _global_m->bounds(id, M); }

  /*!
  \brief Accessor
  \param vloc : tuple of location identifiers
  \param M : clock bound map
  \pre see tchecker::clockbounds::global_lu_map_t::bounds
  \post M is the global bound map
  */
  inline void global_m(tchecker::vloc_t const & vloc, tchecker::clockbounds::map_t & M) const
  {
    return _global_m->bounds(vloc, M);
  }

  /*!
   \brief Accessor
   \return Number of clocks
  */
  inline tchecker::clock_id_t clocks_number() const { return _global_lu->clock_number(); }

  /*!
   \brief Accessor
   \return Number of locations
   */
  inline tchecker::loc_id_t locations_number() const { return _local_lu->loc_number(); }

private:
  std::shared_ptr<tchecker::clockbounds::global_lu_map_t> _global_lu; /*!< Global LU map */
  std::shared_ptr<tchecker::clockbounds::global_m_map_t> _global_m;   /*!< Global M map */
  std::shared_ptr<tchecker::clockbounds::local_lu_map_t> _local_lu;   /*!< Local LU map */
  std::shared_ptr<tchecker::clockbounds::local_m_map_t> _local_m;     /*!< Local M map */
};

} // namespace clockbounds

} // namespace tchecker

#endif // TCHECKER_CLOCKBOUNDS_HH
