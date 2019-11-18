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
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/dbm/db.hh"
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
    tchecker::clockbounds::bound_t const NO_BOUND = - tchecker::dbm::INF_VALUE;
    
    /*!
     \brief Max clock bound
     */
    tchecker::clockbounds::bound_t const MAX_BOUND = tchecker::dbm::MAX_VALUE;
    
    /*!
     \brief Type of map from clock ID to clock bound
     */
    using map_t =
    tchecker::make_array_t
    <tchecker::clockbounds::bound_t, sizeof(tchecker::clockbounds::bound_t), tchecker::array_capacity_t<tchecker::clock_id_t>>;
    
    /*!
     \brief Clock bound map allocation
     \param clock_nb : number of clocks
     \return A clock bound map of domain [0..clock_nb)
     */
    tchecker::clockbounds::map_t * allocate_map(tchecker::clock_id_t clock_nb);
    
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
     \post the bound for clock 0 has been set to 0, and all otehr bounds have been set to NO_BOUND
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
     \post map has been updated to the max of map and upd
     \return true is map has been modified, false otherwise
     */
    bool update(tchecker::clockbounds::map_t & map, tchecker::clockbounds::map_t const & upd);
    
    /*!
     \brief Output operator
     \param os : output stream
     \param map : clock bound map
     \post map has been output to os
     \return os after map has been output
     */
    std::ostream & operator<< (std::ostream & os, tchecker::clockbounds::map_t const & map);
    
    
    
    
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
       \note All location ID in [0..loc_nb) are assumed to be valid
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
      tchecker::clockbounds::local_lu_map_t & operator= (tchecker::clockbounds::local_lu_map_t const & m);
      
      /*!
       \brief Move-assignment operator
       \param m : map
       \post m has been moved to this
       */
      tchecker::clockbounds::local_lu_map_t & operator= (tchecker::clockbounds::local_lu_map_t && m);
      
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
       */
      tchecker::clockbounds::map_t & L(tchecker::loc_id_t id);
      
      /*!
       \brief Accessor
       \param id : location ID
       \return L clock bound map for location id
       \pre 0 <= id < _loc_nb (checked by assertion)
       */
      tchecker::clockbounds::map_t const & L(tchecker::loc_id_t id) const;
      
      /*!
       \brief Accessor
       \param id : location ID
       \return U clock bound map for location id
       \pre 0 <= id < _loc_nb (checked by assertion)
       */
      tchecker::clockbounds::map_t & U(tchecker::loc_id_t id);
      
      /*!
       \brief Accessor
       \param id : location ID
       \return U clock bound map for location id
       \pre 0 <= id < _loc_nb (checked by assertion)
       */
      tchecker::clockbounds::map_t const & U(tchecker::loc_id_t id) const;
    private:
      tchecker::loc_id_t _loc_nb;                      /*!< Number of system locations */
      tchecker::clock_id_t _clock_nb;                  /*!< Number of clocks */
      std::vector<tchecker::clockbounds::map_t *> _L;   /*!< Clock lower-bound map */
      std::vector<tchecker::clockbounds::map_t *> _U;   /*!< Clock upper-bound map */
    };
    
    
    /*!
     \brief Output operator
     \param os : output stream
     \param map : local LU map
     \post map has been output to os
     \return os after map has been output
     */
    std::ostream & operator<< (std::ostream & os, tchecker::clockbounds::local_lu_map_t const & map);
    
    
    
    
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
      tchecker::clockbounds::global_lu_map_t & operator= (tchecker::clockbounds::global_lu_map_t const & m);
      
      /*!
       \brief Move-assignment operator
       \param m : map
       \post m has been moved to this
       */
      tchecker::clockbounds::global_lu_map_t & operator= (tchecker::clockbounds::global_lu_map_t && m);
      
      /*!
       \brief Accessor
       \return Number of clocks
       */
      tchecker::clock_id_t clock_number() const;
      
      /*!
       \brief Accessor
       \return L clock bound map
       */
      tchecker::clockbounds::map_t & L(void);
      
      /*!
       \brief Accessor
       \return L clock bound map
       */
      tchecker::clockbounds::map_t const & L(void) const;
      
      /*!
       \brief Accessor
       \return U clock bound map
       */
      tchecker::clockbounds::map_t & U(void);
      
      /*!
       \brief Accessor
       \return U clock bound map
       */
      tchecker::clockbounds::map_t const & U(void) const;
    private:
      tchecker::clock_id_t _clock_nb;     /*!< Number of clocks */
      tchecker::clockbounds::map_t * _L;   /*!< Clock lower-bound map */
      tchecker::clockbounds::map_t * _U;   /*!< Clock upper-bound map */
    };
    
    
    /*!
     \brief Output operator
     \param os : output stream
     \param map : global LU map
     \post map has been output to os
     \return os after map has been output
     */
    std::ostream & operator<< (std::ostream & os, tchecker::clockbounds::global_lu_map_t const & map);
    
    
    
    
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
      tchecker::clockbounds::local_m_map_t & operator= (tchecker::clockbounds::local_m_map_t const & m);
      
      /*!
       \brief Move-assignment operator
       \param m : map
       \post m has been moved to this
       */
      tchecker::clockbounds::local_m_map_t & operator= (tchecker::clockbounds::local_m_map_t && m);
      
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
       */
      tchecker::clockbounds::map_t & M(tchecker::loc_id_t id);
      
      /*!
       \brief Accessor
       \param id : location ID
       \return m clock bound map for location id
       \pre 0 <= id < _loc_nb (checked by assertion)
       */
      tchecker::clockbounds::map_t const & M(tchecker::loc_id_t id) const;
    private:
      tchecker::loc_id_t _loc_nb;                      /*!< Number of system locations */
      tchecker::clock_id_t _clock_nb;                  /*!< Number of clocks */
      std::vector<tchecker::clockbounds::map_t *> _M;   /*!< Clock bound map */
    };
    
    
    /*!
     \brief Output operator
     \param os : output stream
     \param map : local m map
     \post map has been output to os
     \return os after map has been output
     */
    std::ostream & operator<< (std::ostream & os, tchecker::clockbounds::local_m_map_t const & map);
    
    
    
    
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
      tchecker::clockbounds::global_m_map_t & operator= (tchecker::clockbounds::global_m_map_t const & m);
      
      /*!
       \brief Move-assignment operator
       \param m : map
       \post m has been moved to this
       */
      tchecker::clockbounds::global_m_map_t & operator= (tchecker::clockbounds::global_m_map_t && m);
      
      /*!
       \brief Accessor
       \return Number of clocks
       */
      tchecker::clock_id_t clock_number() const;
      
      /*!
       \brief Accessor
       \return M clock bound map
       */
      tchecker::clockbounds::map_t & M(void);
      
      /*!
       \brief Accessor
       \return M clock bound map
       */
      tchecker::clockbounds::map_t const & M(void) const;
    private:
      tchecker::clock_id_t _clock_nb;     /*!< Number of clocks */
      tchecker::clockbounds::map_t * _M;   /*!< Clock bound map */
    };
    
    
    /*!
     \brief Output operator
     \param os : output stream
     \param map : global m map
     \post map has been output to os
     \return os after map has been output
     */
    std::ostream & operator<< (std::ostream & os, tchecker::clockbounds::global_m_map_t const & map);
    
  } // end of namespace clockbound
  
} // end of namespace tchecker

# endif // TCHECKER_CLOCKBOUNDS_HH
