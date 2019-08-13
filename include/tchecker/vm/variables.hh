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
   \brief Virtual machine variables (flattened variables: arrays are translated to consecutive 1-dimensional variables)
   */
  class vm_variables_t : public tchecker::flat_integer_variables_t, public tchecker::flat_clock_variables_t {
  public:
    /*!
     \brief Default constructor
     */
    vm_variables_t() = default;
    
    /*!
     \brief Constructor
     \param intvars : integer variables
     \post Flat bounded integer variables have been computed from invars
     \note this has an empty set of clocks
     */
    vm_variables_t(tchecker::integer_variables_t const & intvars)
    : tchecker::flat_integer_variables_t(intvars)
    {}
    
    /*!
     \brief Constructor
     \param intvars : integer variables
     \param clocks : clock variables
     \post Flat bounded integer variables and flat clocks have been computed from intvars and clocks
     */
    vm_variables_t(tchecker::integer_variables_t const & intvars, tchecker::clock_variables_t const & clocks)
    : tchecker::flat_integer_variables_t(intvars), tchecker::flat_clock_variables_t(clocks)
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
     \brief Assignment operator
     */
    tchecker::vm_variables_t & operator= (tchecker::vm_variables_t const &) = default;
    
    /*!
     \brief Move-assignment operator
     */
    tchecker::vm_variables_t & operator= (tchecker::vm_variables_t &&) = default;

    /*!
     \brief Accessor
     \return VM clock variables
     */
    inline constexpr tchecker::flat_clock_variables_t const & clocks() const
    {
      return *this;
    }
    
    /*!
     \brief Accessor
     \return VM bounded integer variables
     */
    inline constexpr tchecker::flat_integer_variables_t const & bounded_integers() const
    {
      return *this;
    }
    
    /*!
     \brief Compatibility check
     \tparam INTVARS_VAL : type of integer variables valuation
     \param ival : integer variables valuation
     \return true if ival has enough variables w.r.t. to VM integer variables
     */
    template <class INTVARS_VAL>
    bool compatible(INTVARS_VAL const & ival) const
    {
      return (ival.size() >= tchecker::flat_integer_variables_t::size());
    }
  };
  
} // end of namespace tchecker

#endif // TCHECKER_VM_VARIABLES_HH
