/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZONE_OFFSET_DBM_HH
#define TCHECKER_ZONE_OFFSET_DBM_HH

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file zone.hh
 \brief DBM implementation of zones
 */

namespace tchecker {
  
  namespace offset_dbm {
    
    /*!
     \class zone_t
     \brief Offset DBM implementation of zones (implements tchecker/zone/offset_zone.hh)
     */
    class zone_t {
    public:
      /*!
       \brief Assignment operator
       \param zone : an offset DBM zone
       \pre this and zone have the same dimension
       \post this is a copy of zone
       \return this after assignment
       \throw std::invalid_argument : if this and zone do not have the same dimension
       */
      tchecker::offset_dbm::zone_t & operator= (tchecker::offset_dbm::zone_t const & zone);
      
      /*!
       \brief Move assignment operator
       \note deleted (same as assignment operator)
       */
      tchecker::offset_dbm::zone_t & operator= (tchecker::offset_dbm::zone_t && zone) = delete;
      
      /*!
       \brief Emptiness check
       \return true if this zone is empty, false otherwise
       \note only checks that (0,0) has a non-negative constraint for efficiency. Every DBM manipulation function
       (see tchecker/dbm/offset_dbm.hh) sets (0,0) to a negative value if the DBM is empty. Take care with direct
       access to internal DBM as arbitrary manipulations may invalidate this optimized emptiness predicate.
       */
      bool empty() const;
      
      /*!
       \brief Equality predicate
       \param zone : a zone
       \return true if this is equal to zone, false otherwise
       */
      bool operator== (tchecker::offset_dbm::zone_t const & zone) const;
      
      /*!
       \brief Disequality predicate
       \param zone : a zone
       \return true if this is not equal to zone, false otherwise
       */
      bool operator!= (tchecker::offset_dbm::zone_t const & zone) const;
      
      /*!
       \brief Inclusion check
       \param zone : a zone
       \return true if this is included or equal to zone
       */
      bool operator<= (tchecker::offset_dbm::zone_t const & zone) const;
      
      /*!
       \brief Lexical ordering
       \param zone : a zone
       \return 0 if this and zone are equal, a negative value if this is smalelr than zone w.r.t.t lexical ordering on the clock constraints,
       a positive value otherwise
       */
      int lexical_cmp(tchecker::offset_dbm::zone_t const & zone) const;
      
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
       */
      tchecker::dbm::db_t * dbm();
      
      /*!
       \brief Accessor
       \return internal DBM of size dim()*dim()
       */
      tchecker::dbm::db_t const * dbm() const;
      
      /*!
       \brief Construction
       \tparam ARGS : type of arguments to a constructor of tchecker::offset_dbm::zone_t
       \tparam ptr : pointer to an allocated zone
       \pre ptr points to an allocated zone of sufficient capacity, i.e. at least
       allocation_size_t<tchecker::offset_dbm::zone_t>::alloc_size(dim)
       \post an instance of thcecker::offset_dbm::zone_t has been built in ptr with
       parameters args
       */
      template <class... ARGS>
      static inline void construct(void * ptr, ARGS && ... args)
      {
        new (ptr) tchecker::offset_dbm::zone_t(args...);
      }
      
      /*!
       \brief Destruction
       \param ptr : a zone
       \post the destructor of tchecker::offset_dbm::zone_t has been called on ptr
       */
      static inline void destruct(tchecker::offset_dbm::zone_t * ptr)
      {
        ptr->~zone_t();
      }
    protected:
      /*!
       \brief Constructor
       \param dim : dimension
       \post this zone has dimension dim.
       this zone if not initialized
       */
      zone_t(tchecker::clock_id_t dim);
      
      /*!
       \brief Copy constructor
       \param zone : a zone
       \pre this has been allocated with the same dimension as zone
       \post this is a copy of zone
       */
      zone_t(tchecker::offset_dbm::zone_t const & zone);
      
      /*!
       \brief Move constructor
       \note deleted (move construction is the same as copy construction)
       */
      zone_t(tchecker::offset_dbm::zone_t && zone) = delete;
      
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
        return static_cast<tchecker::dbm::db_t *>(static_cast<void *>(const_cast<tchecker::offset_dbm::zone_t *>(this) + 1));
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
    inline std::size_t hash_value(tchecker::offset_dbm::zone_t const & zone)
    {
      return zone.hash();
    }


    /*!
     \brief Lexical ordering
     \param zone1 : first zone
     \param zone2 : second zone
     \return 0 if zone1 and zone2 are equal, a negative value if zone1 is smaller than zone2 w.r.t. lexical ordering, a positive value otherwise
     */
    int lexical_cmp(tchecker::offset_dbm::zone_t const & zone1, tchecker::offset_dbm::zone_t const & zone2);
    
  } // end of namespace offset_dbm
  
  
  
  
  /*!
   \class allocation_size_t
   \brief Specialization of class tchecker::allocation_size_t for type
   tchecker::offset_dbm::zone_t
   */
  template <>
  class allocation_size_t<tchecker::offset_dbm::zone_t> {
  public:
    /*!
     \brief Accessor
     \param dim : dimension
     \return Allocation size for objects of type tchecker::offset_dbm::zone_t
     with dimension dim
     \note Allocates memory for the zone + the DBM
     */
    static constexpr std::size_t alloc_size(tchecker::clock_id_t dim)
    {
      return (sizeof(tchecker::offset_dbm::zone_t) + dim * dim * sizeof(tchecker::dbm::db_t));
    }
    
    /*!
     \brief Accessor
     \param dim : dimension
     \param args : arguments to a constructor of class tchecker::offset_dbm::zone_t
     \return Allocation size for objects of type tchecker::offset_dbm::zone_t
     with dimension dim
     */
    template <class ... ARGS>
    static constexpr std::size_t alloc_size(tchecker::clock_id_t dim, ARGS && ... args)
    {
      return allocation_size_t<tchecker::offset_dbm::zone_t>::alloc_size(dim);
    }
  };
  
  
  
  // Allocation and deallocation
  
  namespace offset_dbm {
    
    /*!
     \brief Allocation and construction of offset DBM zones
     \param dim : dimension
     \param args : arguments to a constructor of tchecker::offset_dbm::zone_t
     \pre dim >= 1
     \return an instance of tchecker::offset_dbm::zone_t of dimension dim and
     constructed with args
     \throw std::invalid_argument if dim < 1
     */
    template<class... ARGS>
    tchecker::offset_dbm::zone_t * zone_allocate_and_construct(tchecker::clock_id_t dim, ARGS && ... args)
    {
      if (dim < 1)
        throw std::invalid_argument("dimension should be >= 1");
      
      void * ptr = new char[tchecker::allocation_size_t<tchecker::offset_dbm::zone_t>::alloc_size(dim)];
      tchecker::offset_dbm::zone_t::construct(ptr, args...);
      return reinterpret_cast<tchecker::offset_dbm::zone_t *>(ptr);
    }
    
    
    /*!
     \brief Destruction and deallocation of DBM zones
     \param zone : a zone
     \pre pre has been allocated by tchecker::offset_dbm::zone_allocate_and_construct
     \post the destructor of zone has been called, and zone has been deleted
     */
    void zone_destruct_and_deallocate(tchecker::offset_dbm::zone_t * zone);
    
  } // end of namespace offset_dbm
  
} // end of namespace tchecker

#endif // TCHECKER_ZONE_OFFSET_DBM_HH

