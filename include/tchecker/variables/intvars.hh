/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_INTVARS_HH
#define TCHECKER_INTVARS_HH

#include <iostream>
#include <string>

#include "tchecker/basictypes.hh"
#include "tchecker/utils/allocation_size.hh"
#include "tchecker/utils/array.hh"
#include "tchecker/utils/index.hh"
#include "tchecker/variables/variables.hh"

/*!
 \file intvars.hh
 \brief Integer variables
 */

namespace tchecker {
  
  // integer variables
  
  /*!
   \class intvar_info_t
   \brief Informations on integer variable
   */
  class intvar_info_t : public tchecker::size_info_t {
  public:
    /*!
     \brief Constructor
     \param size : array size
     \param min : minimal value
     \param max : maximal value
     \param initial_value : initial value
     \pre size > 0  and  min <= init <= max
     \throw std::invalid_argument : if precondition is violated
     */
    intvar_info_t(unsigned int size,
                  tchecker::integer_t min,
                  tchecker::integer_t max,
                  tchecker::integer_t initial_value);
    
    
    /*!
     \brief Copy constructor
     */
    intvar_info_t(tchecker::intvar_info_t const &) = default;
    
    
    /*!
     \brief Move constructor
     */
    intvar_info_t(tchecker::intvar_info_t &&) = default;
    
    
    /*!
     \brief Destructor
     */
    ~intvar_info_t() = default;
    
    
    /*!
     \brief Assignment operator
     */
    tchecker::intvar_info_t & operator= (tchecker::intvar_info_t const &) = default;
    
    
    /*!
     \brief Move assignment operator
     */
    tchecker::intvar_info_t & operator= (tchecker::intvar_info_t &&) = default;
    
    
    /*!
     \brief Accessor
     \return Minimal value
     */
    inline constexpr tchecker::integer_t min() const
    {
      return _min;
    }
    
    
    /*!
     \brief Accessor
     \return Maximal value
     */
    inline constexpr tchecker::integer_t max() const
    {
      return _max;
    }
    
    
    /*!
     \brief Accessor
     \return Initial value
     */
    inline constexpr tchecker::integer_t initial_value() const
    {
      return _initial_value;
    }
  private:
    tchecker::integer_t _min;              /*!< Minimal value */
    tchecker::integer_t _max;              /*!< Maximal value */
    tchecker::integer_t _initial_value;    /*!< Initial value */
  };
  
  
  /*!
   \brief Index of bounded integer variables
   */
  using intvar_index_t = tchecker::index_t<tchecker::intvar_id_t, std::string>;
  
  
  
  /*!
   \class integer_variables_t
   \brief Declaration of integer variables
   */
  class integer_variables_t
  : public tchecker::array_variables_t<tchecker::intvar_id_t, tchecker::intvar_info_t, tchecker::intvar_index_t> {
  public:
    using
    tchecker::array_variables_t<tchecker::intvar_id_t, tchecker::intvar_info_t, tchecker::intvar_index_t>::array_variables_t;
    
    /*!
     \brief Declare a bounded integer variable
     \param name : variable name
     \param size : variabel size (array)
     \param min : minimal value
     \param max : maximal value
     \param initial : initial value
     \pre 'name' is not a declared variable,
     size > 0,
     and min <= initial <= max
     \post A bounded integer variable with base name 'name', with size 'size', with minimal value 'min',
     wirh maximal value 'max', and with initial value 'initial' has been declared
     \throw std::invalid_argument : if the precondition is violated
     */
    void declare(std::string const & name,
                 tchecker::intvar_id_t size,
                 tchecker::integer_t min,
                 tchecker::integer_t max,
                 tchecker::integer_t initial)
    {
      tchecker::intvar_info_t info{size, min, max, initial};
      tchecker::array_variables_t<tchecker::intvar_id_t, tchecker::intvar_info_t, tchecker::intvar_index_t>::declare(name, info);
    }
  protected:
    using tchecker::array_variables_t<tchecker::intvar_id_t, tchecker::intvar_info_t, tchecker::intvar_index_t>::declare;
  };
  
  
  
  /*!
   \brief Type of flat bounded integer variables
   */
  using flat_integer_variables_t
  = tchecker::flat_variables_t<tchecker::intvar_id_t, tchecker::intvar_info_t, tchecker::intvar_index_t>;
  
  
  
  
  // Integer variables valuation
  
  /*!
   \brief Type of integer variables array
   */
  using intvars_array_t =
  tchecker::make_array_t<tchecker::integer_t, sizeof(tchecker::integer_t), tchecker::array_capacity_t<unsigned short>>;
  
  
  
  
  /*!
   \class intvars_valuation_t
   \brief Valuation of integer variables
   */
  class intvars_valuation_t : public tchecker::intvars_array_t {
  public:
    /*!
     \brief Assignment operator
     \param v : intvars valuation
     \post this is a copy of v
     \return this after assignment
     */
    tchecker::intvars_valuation_t & operator= (tchecker::intvars_valuation_t const & v) = default;
    
    /*!
     \brief Move assignment operator
     \param v : intvars valuation
     \post v has been moved to this
     \return this after assignment
     */
    tchecker::intvars_valuation_t & operator= (tchecker::intvars_valuation_t && v) = default;
    
    /*!
     \brief Accessor
     \return Size
     \note Size coincide with capacity for intvars valuations
     */
    inline constexpr typename tchecker::intvars_valuation_t::capacity_t size() const
    {
      return capacity();
    }
    
    
    /*!
     \brief Construction
     \param args : arguments to a constructor of intvars_valuation_t
     \pre ptr points to an allocated zone of capacity at least
     allocation_size_t<intvars_valuation_t>::alloc_size(args)
     \post intvars_valuation_t(args) has been called on ptr
     */
    template <class... ARGS>
    static inline void construct(void * ptr, ARGS && ... args)
    {
      new (ptr) intvars_valuation_t(args...);
    }
    
    
    /*!
     \brief Destruction
     \param v : intvars valuation
     \post ~intvars_valuation_t() has been called on v
     */
    static inline void destruct(tchecker::intvars_valuation_t * v)
    {
      assert(v != nullptr);
      v->~intvars_valuation_t();
    }
  protected:
    /*!
     \brief Constructor
     \param size : intvars valuation size
     \param value : initial value
     */
    intvars_valuation_t(unsigned short size, tchecker::integer_t value = 0)
    : intvars_array_t(std::make_tuple(size), std::make_tuple(value))
    {}
    
    /*!
     \brief Copy constructor
     \param v : intvars valuation
     \post this is a copy of v
     */
    intvars_valuation_t(tchecker::intvars_valuation_t const & v) = default;
    
    /*!
     \brief Move constructor
     \param v : intvars valuation
     \post v has been moved to this
     */
    intvars_valuation_t(tchecker::intvars_valuation_t && v) = default;
    
    /*!
     \brief Destructor
     */
    ~intvars_valuation_t() = default;
  };
  
  
  
  /*!
   \class allocation_size_t
   \brief Specialization of tchecker::allocation_size_t for class
   tchecker::intvars_valuation_t
   */
  template <>
  class allocation_size_t<tchecker::intvars_valuation_t> {
  public:
    /*!
     \brief Allocation size
     \param args : arguments for a constructor of class
     tchecker::invars_valuation_t
     \return allocation size for objects of class tchecker::intvars_valuation_t
     */
    template <class... ARGS>
    static constexpr std::size_t alloc_size(ARGS && ... args) {
      return tchecker::allocation_size_t<tchecker::intvars_array_t>::alloc_size(args...);
    }
  };
  
  
  
  
  /*!
   \brief Allocate and construct an intvars valuation
   \param size : intvars valuation size
   \param args : arguments to a constructor of tchecker::intvars_valuation_t
   \return an instance of intvars_valuation_t of size values constructed
   from args
   */
  template <class... ARGS>
  tchecker::intvars_valuation_t * intvars_valuation_allocate_and_construct(unsigned short size, ARGS && ...args)
  {
    char * ptr = new char[tchecker::allocation_size_t<tchecker::intvars_valuation_t>::alloc_size(size)];
    
    tchecker::intvars_valuation_t::construct(ptr, args...);
    
    return reinterpret_cast<tchecker::intvars_valuation_t *>(ptr);
  }
  
  
  
  
  /*!
   \brief Destruct and deallocate an intvars valuation
   \param v : intvars valuation
   \pre v has been returned by intvars_valuation_allocate_and_construct
   \post v has been destructed and deallocated
   */
  void intvars_valuation_destruct_and_deallocate(tchecker::intvars_valuation_t * v);
  
  
  
  
  /*!
   \brief Output integer variables valuation
   \param os : output stream
   \param intvars_val : integer variables valuation
   \param index : an index of integer variables
   \post intvars_val has been output to os with variable names from index
   \return os after output
   */
  std::ostream & output(std::ostream & os,
                        tchecker::intvars_valuation_t const & intvars_val,
                        tchecker::intvar_index_t const & index);
  
  
  /*!
   \brief Write integer variables valuation to string
   \param intvars_val : integer variables valuation
   \param index : an index of integer variables
   \return An std::string representation of intvars_val using variable names from index
   */
  std::string to_string(tchecker::intvars_valuation_t const & intvars_val,
                        tchecker::intvar_index_t const & index);
  
  
  /*!
   \brief Lexical ordering on integer valuations
   \param intvars_val1 : first integer variable valuation
   \param intvars_val2 : second integer variable valuation
   \return 0 if intvars_val1 and intvars_val2 are equal, a negative value if intvars_val1 is smaller than intvars_val2 w.r.t. lexical ordering, and
   a positive value otherwise.
   */
  int lexical_cmp(tchecker::intvars_valuation_t const & intvars_val1,
                  tchecker::intvars_valuation_t const & intvars_val2);
  
} // end of namespace tchecker

#endif // TCHECKER_INTVARS_HH
