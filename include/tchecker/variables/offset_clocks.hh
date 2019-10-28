/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_OFFSET_CLOCKS_HH
#define TCHECKER_OFFSET_CLOCKS_HH

#include <string>

#include "tchecker/basictypes.hh"
#include "tchecker/variables/access.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file offset_clocks.hh
 \brief Offset clock variables (encoding of clocks with a reference clock and an offset variable)
 */

namespace tchecker {
  
  /*!
   \class offset_clock_variables_t
   \brief Declaration of offset clock variables. Offset clock variables consists in a set of reference clocks, and a set
   of offset variables. Each reference offset variable is mapped to a reference clock.
   */
  class offset_clock_variables_t : public tchecker::clock_variables_t {
  public:
    /*!
     \brief Constructor
     */
    offset_clock_variables_t();
    
    /*!
     \brief Copy constructor
     */
    offset_clock_variables_t(tchecker::offset_clock_variables_t const &);
    
    /*!
     \brief Move constructor
     */
    offset_clock_variables_t(tchecker::offset_clock_variables_t &&);
    
    /*!
     \brief Destructor
     */
    ~offset_clock_variables_t();
    
    /*!
     \brief Assignment operator
     */
    tchecker::offset_clock_variables_t & operator= (tchecker::offset_clock_variables_t const &);
    
    /*!
     \brief Move-assignment operator
     */
    tchecker::offset_clock_variables_t & operator= (tchecker::offset_clock_variables_t &&);
    
    /*!
     \brief Declare a reference clock
     \param name : variable name
     \pre 'name' is not a declared variable
     No offset variable has been declared yet
     \post a reference clock with base name 'name' has been declared. The identifier of the reference clock is
     refcount() - 1
     \throw std::invalid_argument : if a variable with base name 'name' has already been declared
     \throw std::runtime_error : if an offset variable has already been declared
     */
    void declare_reference_clock(std::string const & name);
    
    /*!
     \brief Declare an offset variable
     \param name : variable name
     \param dim : dimension (array)
     \param refclock : ID of reference clock
     \pre 'name' is not a declared variable
     dim > 0
     'refclock' is the ID of a declared reference clock
     \post an offset variable with base name 'name', dimension 'dim' and reference clock 'refclock' has been declared
     \throw std::invalid_argument : if the precondition is violated
     */
    void declare_offset_variable(std::string const & name, tchecker::clock_id_t dim, tchecker::clock_id_t refclock);
    
    /*!
     \brief Accessor
     \return Number of reference clocks
     */
    constexpr inline tchecker::clock_id_t refcount() const
    {
      return _refcount;
    }
    
    /*!
     \brief Accessor
     \return Reference map: offset clock ID -> reference clock ID
     \note Each reference clock is mapped to itself, and each offset variable is mapped to its reference clock
     */
    constexpr inline tchecker::clock_id_t const * refmap() const
    {
      return _refmap;
    }
  private:
    /*!
     \brief Declare a variable
     \param name : variable name
     \param dim : dimension (array)
     \param refclock : ID of reference clock
     \pre 'name' is not a declared variable
     dim > 0
     \post an offset variable with base name 'name', dimension 'dim' and reference clock 'refclock' has been declared
     \throw std::invalid_argument : if the precondition is violated
     */
    void declare(std::string const & name, tchecker::clock_id_t dim, tchecker::clock_id_t refclock);
    
    tchecker::clock_id_t _refcount;        /*!< Number of reference clocks */
    tchecker::clock_id_t * _refmap;        /*!< Map : offset clocks ID -> reference clocks ID */
  };
  
  
  
  
  /*!
   \brief Build offset clock variables from a variable access map
   \param vaccess_map : a variable access map
   \param proc_count : number of processes
   \param flat_clocks : flat clock variables
   \pre all process IDs in vaccess_map should belong to [0,proc_count)
   \return the offset clocks containing proc_count reference clocks, and offset variables corresponding to flat_clocks, and
   with reference clocks defined according to vaccess_map
   */
  tchecker::offset_clock_variables_t build_from_variable_access(tchecker::variable_access_map_t const & vaccess_map,
                                                                tchecker::process_id_t proc_count,
                                                                tchecker::flat_clock_variables_t const & flat_clocks);

} // end of namespace tchecker
    
#endif // TCHECKER_OFFSET_CLOCKS_HH
