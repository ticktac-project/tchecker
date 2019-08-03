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
  class vm_variables_t : public tchecker::clock_variables_t, public tchecker::integer_variables_t {
  public:
    /*!
     \brief Declare a clock variable
     \param name : variable name
     \param dim : dimension (array)
     \pre 'name' is not a declared variable
     dim > 0
     \post 'dim' VM clock variables have been declared with base name 'name'
     \throw std::invalid_argument : if the precondition is violated
     */
    void declare_clock(std::string const & name, tchecker::clock_id_t dim);
    
    /*!
     \brief Declare a bounded integer variable
     \param name : variable name
     \param dim : dimension (array)
     \param min : minimal value
     \param max : maximal value
     \param initial : initial value
     \pre 'name' is not a declared variable,
     dim > 0,
     and min <= initial <= max
     \post 'dim' VM bounded integer variables have been declared with base name 'name', minimal value 'min',
     maximal value 'max' and initial value 'initial'
     \throw std::invalid_argument : if the precondition is violated
     */
    void declare_bounded_integer(std::string const & name,
                                 tchecker::intvar_id_t dim,
                                 tchecker::integer_t min,
                                 tchecker::integer_t max,
                                 tchecker::integer_t initial);
    
    /*!
     \brief Accessor
     \return VM clock variables
     */
    inline tchecker::clock_variables_t const & clocks() const
    {
      return *this;
    }
    
    /*!
     \brief Accessor
     \return VM bounded integer variables
     */
    inline tchecker::integer_variables_t const & bounded_integers() const
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
      return (ival.size() >= tchecker::integer_variables_t::size());
    }
  private:
    /*!
     \brief Declare a variable
     \tparam VARIABLES : type of variables
     \tparam INFO : type of variable information
     \tparam ID : type of variable identifier
     \tparam ARGS : type of arguments to a constructor (size excluded)
     \param name : variable name
     \param dim : variable dimension (array)
     \param args : extra parameters for INFO (beyond size)
     \pre variable name is not declared yet, size > 0, and INFO has a
     constructor with parameters (size, args)
     \post the variable has been added to VARIABLES
     \throw std::invalid_argument : if the precondition is violated
     or if there are less than dim variable identifiers left
     */
    template <class VARIABLES, class INFO, class ID, class ... ARGS>
    void declare(std::string const & name, ID dim, ARGS && ... args)
    {
      if (std::numeric_limits<ID>::max() - VARIABLES::size() + 1 < dim)
        throw std::invalid_argument("Not enough identifiers left for VM variable declaration");
      
      ID id = static_cast<ID>(VARIABLES::size());
      INFO info(1, args...);
      
      if (dim == 1)
        VARIABLES::declare(id, name, info);
      else {
        std::stringstream s;
        for (ID i = 0; i < dim; ++i) {
          s << name << "[" << i << "]";
          VARIABLES::declare(static_cast<ID>(id + i), s.str(), info);
          s.clear();
        }
      }
    }
  };
  
  
  
  
  /*!
   \brief Translate system bounded integer variables into VM bounded integer variables
   \tparam SYSTEM : type of system (should derive from tchecker::fsm::details::system_t)
   \param system : a system
   \param vm_variables : virtual machine variables
   \pre system has bounded integer variables
   \post all bounded integer variables from system have been declared to vm_variables
   */
  template <class SYSTEM>
  void vm_variables_declare_bounded_integers(SYSTEM const & system, tchecker::vm_variables_t & vm_variables)
  {
    tchecker::integer_variables_t const & intvars = system.intvars();
    
    for (auto && [id, name] : intvars.index()) {
      tchecker::intvar_info_t const & info = intvars.info(id);
      vm_variables.declare_bounded_integer(name, info.size(), info.min(), info.max(), info.initial_value());
    }
  }
  
  
  
  
  /*!
   \brief Translate system clock variables into VM clock variables
   \tparam SYSTEM : type of system (should derive from tchecker::ta::details::system_t)
   \param system : a system
   \param vm_variables : virtual machine variables
   \pre system has clock variables
   \post all clock variables from system have been declared to vm_variables
   */
  template <class SYSTEM>
  void vm_variables_declare_clocks(SYSTEM const & system, tchecker::vm_variables_t & vm_variables)
  {
    tchecker::clock_variables_t const & clocks = system.clocks();
    
    for (auto && [id, name] : clocks.index()) {
      tchecker::clock_info_t const & info = clocks.info(id);
      vm_variables.declare_clock(name, info.size());
    }
  }
  
} // end of namespace tchecker

#endif // TCHECKER_VM_VARIABLES_HH
