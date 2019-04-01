/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ALLOCATION_SIZE_HH
#define TCHECKER_ALLOCATION_SIZE_HH

#include <stdexcept>

/*!
 \file allocation_size.hh
 \brief Allocation size computation
 */

namespace tchecker {
  
  /*!
   \class allocation_size_t
   \tparam T : type of object
   \brief Defines allocation size for objects of type T
   \note Default allocation size class. Should not be use. Should be specialized
   instead. Specializations should be defined in namespace tchecker.
   */
  template <class T>
  class allocation_size_t {
  public:
    /*!
     \brief Allocation size for objects of type T
     \param args : parameters needed to determine the allocation size of T
     \throw std::runtime_error
     \note the default implementation for this method is to return sizeof(T).
     However, this default implementation throws std::runtime_error to force
     specialization and avoid bugs difficult to detect
     */
    template <class ... ARGS>
    static std::size_t alloc_size(ARGS && ... args)
    {
      throw std::runtime_error("default allocation_size_t should not be used");
    }
  };
  
} // end of namespace tchecker

#endif // TCHECKER_ALLOCATION_SIZE_HH
