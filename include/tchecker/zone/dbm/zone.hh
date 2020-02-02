/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZONE_DBM_HH
#define TCHECKER_ZONE_DBM_HH

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/dbm/dbm.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file zone.hh
 \brief DBM implementation of zones
 */

namespace tchecker {
  
  namespace dbm {
    
    /*!
     \class zone_t
     \brief DBM implementation of zones
     */
    class zone_t {
    public:
      /*!
       \brief Assignment operator
       \param zone : a DBM zone
       \pre this and zone have the same dimension
       \post this is a copy of zone
       \return this after assignment
       \throw std::invalid_argument : if this and zone do not have the same dimension
       */
      tchecker::dbm::zone_t & operator= (tchecker::dbm::zone_t const & zone);
      
      /*!
       \brief Move assignment operator
       \note deleted (same as assignment operator)
       */
      tchecker::dbm::zone_t & operator= (tchecker::dbm::zone_t && zone) = delete;
      
      /*!
       \brief Emptiness check
       \return true if this zone is empty, false otherwise
       */
      bool is_empty() const;
      
      /*!
       \brief Universal-positive check
       \return true if this zone is universal-positive (i.e. no constraint on clocks except x>=0), false otherwise
       */
      bool is_universal_positive() const;
      
      /*!
       \brief Equality predicate
       \param zone : a zone
       \return true if this is equal to zone, false otherwise
       */
      bool operator== (tchecker::dbm::zone_t const & zone) const;
      
      /*!
       \brief Disequality predicate
       \param zone : a zone
       \return true if this is not equal to zone, false otherwise
       */
      bool operator!= (tchecker::dbm::zone_t const & zone) const;
      
      /*!
       \brief Inclusion check
       \param zone : a zone
       \return true if this is included or equal to zone
       */
      bool operator<= (tchecker::dbm::zone_t const & zone) const;
      
      /*!
       \brief Checks inclusion wrt abstraction aM
       \param zone : a zone
       \param m : clock bounds
       \return true if this zone is include in aM(zone), false otherwise
       \pre clocks have same IDs in zone and m
       */
      bool am_le(tchecker::dbm::zone_t const & zone, tchecker::clockbounds::map_t const & m) const;
      
      /*!
       \brief Checks inclusion wrt abstraction aLU
       \param zone : a zone
       \param l : clock lower bounds
       \param u : clock upper bounds
       \return true if this zone is included in aLU(zone), false otherwise
       \pre clocks have same IDs in zone, l and u
       */
      bool alu_le(tchecker::dbm::zone_t const & zone,
                  tchecker::clockbounds::map_t const & l,
                  tchecker::clockbounds::map_t const & u) const;
      
      /*!
       \brief Lexical ordering
       \param zone : a zone
       \return 0 if this and zone are equal, a negative value if this is smaller than zone w.r.t. lexical ordering on the clock constraints,
       a positive value otherwise
       */
      int lexical_cmp(tchecker::dbm::zone_t const & zone) const;
      
      /*!
       \brief Accessor
       \return hash code for this zone
       */
      std::size_t hash() const;
      
      /*!
       \brief Accessor
       \return dimension of the zone
       */
      inline std::size_t dim() const
      {
        return _dim;
      }
      
      /*!
       \brief Output
       \param os : output stream
       \param index : clock index (map clock ID -> clock name)
       \post this zone has been output to os with clock names from index
       \return os after this zone has been output
       */
      std::ostream & output(std::ostream & os, tchecker::clock_index_t const & index) const;
      
      /*!
       \brief Accessor
       \return internal DBM of size dim()*dim()
       \note Modifications to the returned DBM should ensure tightness or emptiness of the zone, following the convention defined
       in file tchecker/dbm/dbm.hh. It is thus strongly suggested to use the function defined in that file to modify the returned
       DBM. Otherwise, the methods may not be accurate over this zone.
       */
      tchecker::dbm::db_t * dbm();
      
      /*!
       \brief Accessor
       \return internal DBM of size dim()*dim()
       */
      tchecker::dbm::db_t const * dbm() const;
      
      /*!
       \brief Construction
       \tparam ARGS : type of arguments to a constructor of tchecker::zone_dbm_t
       \tparam ptr : pointer to an allocated zone
       \pre ptr points to an allocated zone of sufficient capacity, i.e. at least
       allocation_size_t<tchecker::zone_dbm_t>::alloc_size(dim)
       \post an instance of thcecker::zone_dbm_t has been built in ptr with
       parameters args
       */
      template <class... ARGS>
      static inline void construct(void * ptr, ARGS && ... args)
      {
        new (ptr) tchecker::dbm::zone_t(args...);
      }
      
      /*!
       \brief Destruction
       \param ptr : a zone
       \post the destructor of tchecker::zone_dbm_t has been called on ptr
       */
      static inline void destruct(tchecker::dbm::zone_t * ptr)
      {
        ptr->~zone_t();
      }
    protected:
      /*!
       \brief Constructor
       \param dim : dimension
       \post this zone has dimension dim and is the universal zone
       */
      zone_t(tchecker::clock_id_t dim);
      
      /*!
       \brief Copy constructor
       \param zone : a zone
       \pre this has been allocated with the same dimension as zone
       \post this is a copy of zone
       */
      zone_t(tchecker::dbm::zone_t const & zone);
      
      /*!
       \brief Move constructor
       \note deleted (move construction is the same as copy construction)
       */
      zone_t(tchecker::dbm::zone_t && zone) = delete;
      
      /*!
       \brief Destructor
       */
      ~zone_t();
      
      /*!
       \brief Accessor
       \return pointer to DBM
       */
      constexpr tchecker::dbm::db_t * dbm_ptr() const
      {
        return
        static_cast<tchecker::dbm::db_t *>(static_cast<void *>(const_cast<tchecker::dbm::zone_t *>(this) + 1));
      }
      
      /*!
       \brief Accessor
       \param i : clock ID
       \param j : clock ID
       \return constraint on xi-xj in this DBM
       */
      constexpr tchecker::dbm::db_t dbm(tchecker::clock_id_t i, tchecker::clock_id_t j) const
      {
        return dbm_ptr()[i * _dim + j];
      }
      
      tchecker::clock_id_t _dim;  /*!< Dimension of DBM */
    };
    
    
    
    
    /*!
     \brief Boost compatible hash function on zones
     \param zone : a zone
     \return hash value for zone
     */
    inline std::size_t hash_value(tchecker::dbm::zone_t const & zone)
    {
      return zone.hash();
    }
    
  } // end of namespace dbm
  
  
  
  
  /*!
   \class allocation_size_t
   \brief Specialization of class tchecker::allocation_size_t for type
   tchecker::dbm::zone_t
   */
  template <>
  class allocation_size_t<tchecker::dbm::zone_t> {
  public:
    /*!
     \brief Accessor
     \param dim : dimension
     \return Allocation size for objects of type tchecker::dbm::zone_t
     with dimension dim
     */
    static constexpr std::size_t alloc_size(tchecker::clock_id_t dim)
    {
      return (sizeof(tchecker::dbm::zone_t) + dim * dim * sizeof(tchecker::dbm::db_t));
    }
    
    /*!
     \brief Accessor
     \param dim : dimension
     \param args : arguments to a constructor of class tchecker::dbm::zone_t
     \return Allocation size for objects of type tchecker::dbm::zone_t
     with dimension dim
     */
    template <class ... ARGS>
    static constexpr std::size_t alloc_size(tchecker::clock_id_t dim, ARGS && ... args)
    {
      return allocation_size_t<tchecker::dbm::zone_t>::alloc_size(dim);
    }
  };
  
  
  
  // Allocation and deallocation
  
  namespace dbm {
    
    /*!
     \brief Allocation and construction of DBM zones
     \param dim : dimension
     \param args : arguments to a constructor of tchecker::dbm_zone_t
     \pre dim >= 1
     \return an instance of tchecker::dbm_zone_t of dimension dim and
     constructed with args
     \throw std::invalid_argument if dim < 1
     */
    template<class... ARGS>
    tchecker::dbm::zone_t * zone_allocate_and_construct(tchecker::clock_id_t dim, ARGS && ... args)
    {
      if (dim < 1)
        throw std::invalid_argument("dimension should be >= 1");
      
      void * ptr = new char[tchecker::allocation_size_t<tchecker::dbm::zone_t>::alloc_size(dim)];
      tchecker::dbm::zone_t::construct(ptr, args...);
      return reinterpret_cast<tchecker::dbm::zone_t *>(ptr);
    }
    
    
    /*!
     \brief Destruction and deallocation of DBM zones
     \param zone : a zone
     \pre pre has been allocated by tchecker::dbm::zone_allocate_and_construct
     \post the destructor of zone has been called, and zone has been deleted
     */
    void zone_destruct_and_deallocate(tchecker::dbm::zone_t * zone);
    
  } // end of namespace dbm
  
} // end of namespace tchecker

#endif // TCHECKER_ZONE_DBM_HH

