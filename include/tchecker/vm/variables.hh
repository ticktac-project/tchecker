/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VM_VARIABLES_HH
#define TCHECKER_VM_VARIABLES_HH

#include <limits>
#include <sstream>

#include "tchecker/variables/clocks.hh"
#include "tchecker/variables/intvars.hh"


/*!
 \file variables.hh
 \brief Virtual machine variables
 */

namespace tchecker {

  /*!
   \class vm_variables_t
   \brief Virtual machine variables (proxy to flat bounded integer variables and flat clock variables)
   */
  class vm_variables_t {
  public:
    /*!
     \brief Constructor
     \param intvars : flat bounded integer variables
     \param clocks : flat clock variables
     \note this keeps a reference on intvars and on clocks
     */
    vm_variables_t(tchecker::flat_integer_variables_t const & intvars, tchecker::flat_clock_variables_t const & clocks)
    : _intvars(intvars), _clocks(clocks)
    {}
    
    /*!
     \brief Copy constructor
     */
    vm_variables_t(tchecker::vm_variables_t const &) = default;
    
    /*!
     \brief Move constructor
     */
    vm_variables_t(tchecker::vm_variables_t &&) = default;
    
    /*!
     \brief Destructor
     */
    ~vm_variables_t() = default;
    
    /*!
     \brief Assignement operator
     */
    tchecker::vm_variables_t & operator= (tchecker::vm_variables_t const &) = default;
    
    /*!
     \brief Move-assignment operator
     */
    tchecker::vm_variables_t & operator= (tchecker::vm_variables_t &&) = default;
    
    /*!
     \brief Accessor
     \return flat bounded integer variables
     */
    inline constexpr tchecker::flat_integer_variables_t const & bounded_integers() const
    {
      return _intvars;
    }
    
    /*!
     \brief Accessor
     \return flat clock variables
     */
    inline constexpr tchecker::flat_clock_variables_t const & clocks() const
    {
      return _clocks;
    }
    
    /*!
     \brief Compatibility check
     \tparam INTVARS_VAL : type of integer variables valuation
     \param ival : integer variables valuation
     \return true if ival can store a valuation of VM bounded integer variables
     */
    template <class INTVARS_VAL>
    constexpr bool compatible(INTVARS_VAL const & ival) const
    {
      return (ival.size() >= _intvars.size());
    }
  protected:
    tchecker::flat_integer_variables_t const & _intvars; /*!< Flat bounded integers */
    tchecker::flat_clock_variables_t const & _clocks;    /*!< Flat clocks */
  };
  
} // end of namespace tchecker

#endif // TCHECKER_VM_VARIABLES_HH
