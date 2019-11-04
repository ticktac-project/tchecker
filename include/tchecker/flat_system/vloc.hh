/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VLOC_HH
#define TCHECKER_VLOC_HH

#include <functional>
#include <sstream>

#include "tchecker/basictypes.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/array.hh"
#include "tchecker/utils/ordering.hh"

/*!
 \file vloc.hh
 \brief Vectors of locations
 */

namespace tchecker {
  
  /*!
   \brief Type of fixed capacity array of locations
   \tparam LOC : type of locations
   */
  template <class LOC>
  using loc_array_t = tchecker::make_array_t<LOC *, sizeof(LOC *), tchecker::array_capacity_t<unsigned short>>;
  
  
  
  
  /*!
   \brief Type of vectors of locations
   \tparam LOC : type of locations, should derive from tchecker::loc_t
   */
  template <class LOC>
  class vloc_t : public tchecker::loc_array_t<LOC> {
  public:
    /*!
     \brief Type of locations
     */
    using loc_t = LOC;
    
    /*!
     \brief Assignment operator
     \param vloc : vector of locations
     \post this is a copy of vloc
     \return this after assignment
     */
    tchecker::vloc_t<LOC> & operator= (tchecker::vloc_t<LOC> const & vloc) = default;
    
    /*!
     \brief Move assignment operator
     \param vloc : vector of locations
     \post vloc has been moved to this
     \return this after assignment
     */
    tchecker::vloc_t<LOC> & operator= (tchecker::vloc_t<LOC> && vloc) = default;
    
    /*!
     \brief Accessor
     \return Size
     \note Size coincides with capacity
     */
    inline typename tchecker::loc_array_t<LOC>::capacity_t size() const
    {
      return tchecker::loc_array_t<LOC>::capacity();
    }
    
    /*!
     \brief Construction
     \param args : arguments to a constructor of vloc_t<LOC>
     \pre ptr points to an allocated zone of size at least
     tchecker::allocations_size_t<vloc_t<LOC>>::alloc_size(args)
     \post vloc_t<LOC>(args) has been called on ptr
     */
    template <class... ARGS>
    static inline void construct(void * ptr, ARGS && ... args)
    {
      new (ptr) tchecker::vloc_t<LOC>(args...);
    }
    
    /*!
     \brief Destruction
     \param vloc : vector of locations
     \post ~intvars_valuation_t() has been called on vloc
     */
    static inline void destruct(tchecker::vloc_t<LOC> * vloc)
    {
      assert(vloc != nullptr);
      vloc->~vloc_t<LOC>();
    }
  protected:
    /*!
     \brief Constructor
     \param size : size of vector of locations
     \post this is a vector of size location pointers initialized to nullptr
     */
    vloc_t(unsigned short size)
    : tchecker::loc_array_t<LOC>(std::make_tuple(size), std::make_tuple(nullptr))
    {}
    
    /*!
     \brief Copy constructor
     \param vloc : vector of locations
     \post this is a copy of vloc
     */
    vloc_t(tchecker::vloc_t<LOC> const & vloc) = default;
    
    /*!
     \brief Move constructor
     \param vloc : vector of locations
     \post vloc has been moved to this
     */
    vloc_t(tchecker::vloc_t<LOC> && vloc) = default;
    
    /*!
     \brief Destructor
     \note No destructor call on the pointers to locations in the vector
     */
    ~vloc_t() = default;
  };
  
  
  
  
  /*!
   \class allocation_size_t
   \brief Specialization of tchecker::allocation_size_t for class
   tchecker::vloc_t
   \tparam LOC : type of locations
   */
  template <class LOC>
  class allocation_size_t<tchecker::vloc_t<LOC>> {
  public:
    /*!
     \brief Allocation size
     \param args : arguments for a constructor of class tchecker::vloc_t
     \return allocation size for objects of class tchecker::vloc_t
     */
    template <class... ARGS>
    static constexpr std::size_t alloc_size(ARGS && ... args) {
      return tchecker::allocation_size_t<tchecker::loc_array_t<LOC>>::alloc_size(args...);
    }
  };
  
  
  
  
  /*!
   \brief Allocate and construct a vector of locations
   \tparam LOC : type of locations
   \param size : vector size
   \param args : arguments to a constuctor of vloc_t<LOC>
   \return an instance of vloc_t<LOC> constructed with size
   */
  template <class LOC, class... ARGS>
  tchecker::vloc_t<LOC> * vloc_allocate_and_construct(unsigned short size, ARGS && ... args)
  {
    char * ptr = new char[tchecker::allocation_size_t<tchecker::vloc_t<LOC>>::alloc_size(size)];
    
    tchecker::vloc_t<LOC>::construct(ptr, args...);
    
    return reinterpret_cast<tchecker::vloc_t<LOC> *>(ptr);
  }
  
  
  
  
  /*!
   \brief Destruct and deallocate an intvars valuation
   \param vloc : vector of locations
   \pre vloc has been allocated by vloc_allocate_and_construct
   \post vloc has been destructed and deallocated
   */
  template <class LOC>
  void vloc_destruct_and_deallocate(tchecker::vloc_t<LOC> * vloc)
  {
    tchecker::vloc_t<LOC>::destruct(vloc);
    delete[] reinterpret_cast<char *>(vloc);
  }
  
  
  
  
  /*!
   \brief Output vector of locations
   \param os : output stream
   \param vloc : vector of locations
   \post vloc has been output to os
   \return os after output
   */
  template <class LOC>
  std::ostream & output(std::ostream & os, tchecker::vloc_t<LOC> const & vloc)
  {
    auto begin = vloc.begin(), end = vloc.end();
    os << "<";
    for (auto it = begin; it != end; ++it) {
      if (it != begin)
        os << ",";
      os << (*it)->name();
    }
    os << ">";
    return os;
  }
  
  
  
  
  /*!
   \brief Write vector of locations to string
   \param vloc : vector of locations
   */
  template <class LOC>
  std::string to_string(tchecker::vloc_t<LOC> const & vloc)
  {
    std::stringstream sstream;
    tchecker::output(sstream, vloc);
    return sstream.str();
  }
  
  
  
  
  /*!
   \brief Lexical ordering
   \param vloc1 : first tuple of locations
   \param vloc2 : second tuple of locations
   \return 0 if vloc1 and vloc2 are equal, a negative value if vloc1 is smaller than vloc2 w.r.t. lexical ordering, and a positive value otherwise
   */
  template <class LOC>
  int lexical_cmp(tchecker::vloc_t<LOC> const & vloc1, tchecker::vloc_t<LOC> const & vloc2)
  {
    return tchecker::lexical_cmp
    (vloc1.begin(), vloc1.end(), vloc2.begin(), vloc2.end(),
     [] (LOC const * loc1, LOC const * loc2) -> int
     {
      tchecker::loc_id_t id1 = loc1->id(), id2 = loc2->id();
      return (id1 < id2 ? -1 : (id1 == id2 ? 0 : 1));
    });
  }
  
} // end of namespace tchecker

#endif // TCHECKER_VLOC_HH
