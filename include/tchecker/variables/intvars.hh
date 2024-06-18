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
#include "tchecker/utils/cache.hh"
#include "tchecker/utils/index.hh"
#include "tchecker/utils/iterator.hh"
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
  intvar_info_t(unsigned int size, tchecker::integer_t min, tchecker::integer_t max, tchecker::integer_t initial_value);

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
  tchecker::intvar_info_t & operator=(tchecker::intvar_info_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::intvar_info_t & operator=(tchecker::intvar_info_t &&) = default;

  /*!
   \brief Accessor
   \return Minimal value
   */
  inline constexpr tchecker::integer_t min() const { return _min; }

  /*!
   \brief Accessor
   \return Maximal value
   */
  inline constexpr tchecker::integer_t max() const { return _max; }

  /*!
   \brief Accessor
   \return Initial value
   */
  inline constexpr tchecker::integer_t initial_value() const { return _initial_value; }

private:
  tchecker::integer_t _min;           /*!< Minimal value */
  tchecker::integer_t _max;           /*!< Maximal value */
  tchecker::integer_t _initial_value; /*!< Initial value */
};

/*!
 \brief Type of range over values of a bounded integer variable
*/
using intvar_values_range_t = tchecker::range_t<tchecker::integer_iterator_t<tchecker::integer_t>>;

/*!
 \brief Compute range of values of a bounded integer variable
 \param intvar_info : informations on bounded integer variable
 \return a range of values of the bounded integer variable described by intvar_info
 \throw std::overflow_error : if intvar_info.max() + 1 cannot be represented as an tchecker::integer_t
 */
tchecker::intvar_values_range_t intvar_values_range(tchecker::intvar_info_t const & intvar_info);

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
  using tchecker::array_variables_t<tchecker::intvar_id_t, tchecker::intvar_info_t,
                                    tchecker::intvar_index_t>::array_variables_t;

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
  tchecker::intvar_id_t declare(std::string const & name, tchecker::intvar_id_t size, tchecker::integer_t min,
                                tchecker::integer_t max, tchecker::integer_t initial);

protected:
  using tchecker::array_variables_t<tchecker::intvar_id_t, tchecker::intvar_info_t, tchecker::intvar_index_t>::declare;
};

/*!
 \brief Type of flat bounded integer variables
 */
using flat_integer_variables_t =
    tchecker::flat_variables_t<tchecker::intvar_id_t, tchecker::intvar_info_t, tchecker::intvar_index_t>;

/*!
 \brief Type of iterator over valuations of flat integer variables
 */
using flat_integer_variables_valuations_iterator_t = tchecker::cartesian_iterator_t<tchecker::intvar_values_range_t>;

/*!
 \brief Type of range over valuations of flat integer variables
*/
using flat_integer_variables_valuations_range_t =
    tchecker::range_t<tchecker::flat_integer_variables_valuations_iterator_t, tchecker::end_iterator_t>;

/*!
 \brief Yype of values in a range over valuations of flat integer variables
*/
using flat_integer_variables_valuations_value_t =
    std::iterator_traits<flat_integer_variables_valuations_iterator_t>::value_type;

/*!
 \brief Return the range of valuations of flat integer variables
 \param intvars : flat bounded integer variables
 \return the range of valuations of intvars
 */
tchecker::flat_integer_variables_valuations_range_t
flat_integer_variables_valuations_range(tchecker::flat_integer_variables_t const & intvars);

// Integer variables valuation

/*!
 \class intval_base_t
 \brief Base class for integer variables valuations which can be stored in cache
*/
class intval_base_t : public tchecker::array_capacity_t<unsigned short>, public tchecker::cached_object_t {
public:
  using tchecker::array_capacity_t<unsigned short>::array_capacity_t;
};

/*!
 \brief Type of integer variables array
 */
using integer_array_t = tchecker::make_array_t<tchecker::integer_t, sizeof(tchecker::integer_t), intval_base_t>;

/*!
 \class intval_t
 \brief Valuation of bounded integer variables
 \note NO FIELD SHOULD BE ADDED TO THIS CLASS (either by definition or
 inheritance). See tchecker::make_array_t for details
 */
class intval_t : public tchecker::integer_array_t {
public:
  /*!
   \brief Assignment operator
   \param v : intvars valuation
   \post this is a copy of v
   \return this after assignment
   */
  tchecker::intval_t & operator=(tchecker::intval_t const & v) = default;

  /*!
   \brief Move assignment operator
   \param v : intvars valuation
   \post v has been moved to this
   \return this after assignment
   */
  tchecker::intval_t & operator=(tchecker::intval_t && v) = default;

  /*!
   \brief Accessor
   \return Size
   \note Size coincide with capacity for intvars valuations
   */
  inline constexpr typename tchecker::intval_t::capacity_t size() const { return capacity(); }

  /*!
   \brief Construction
   \param args : arguments to a constructor of intval_t
   \pre ptr points to an allocated zone of capacity at least
   allocation_size_t<intval_t>::alloc_size(args)
   \post intval_t(args) has been called on ptr
   */
  template <class... ARGS> static inline void construct(void * ptr, ARGS &&... args) { new (ptr) intval_t(args...); }

  /*!
   \brief Destruction
   \param v : intvars valuation
   \post ~intval_t() has been called on v
   */
  static inline void destruct(tchecker::intval_t * v)
  {
    assert(v != nullptr);
    v->~intval_t();
  }

protected:
  /*!
   \brief Constructor
   \param size : intvars valuation size
   \param value : initial value
   */
  intval_t(unsigned short size, tchecker::integer_t value = 0)
      : tchecker::integer_array_t(std::make_tuple(size), std::make_tuple(value))
  {
  }

  /*!
   \brief Copy constructor
   \param v : intvars valuation
   \post this is a copy of v
   */
  intval_t(tchecker::intval_t const & v) = default;

  /*!
   \brief Move constructor
   \param v : intvars valuation
   \post v has been moved to this
   */
  intval_t(tchecker::intval_t && v) = default;

  /*!
   \brief Destructor
   */
  ~intval_t() = default;
};

/*!
 \class allocation_size_t
 \brief Specialization of tchecker::allocation_size_t for class
 tchecker::intval_t
 */
template <> class allocation_size_t<tchecker::intval_t> {
public:
  /*!
   \brief Allocation size
   \param args : arguments for a constructor of class tchecker::intval_t
   \return allocation size for objects of class tchecker::intval_t
   */
  template <class... ARGS> static constexpr std::size_t alloc_size(ARGS &&... args)
  {
    return tchecker::allocation_size_t<tchecker::integer_array_t>::alloc_size(args...);
  }
};

/*!
 \brief Allocate and construct a bounded integer variables valuation
 \param size : size of bounded integer variables valuation
 \param args : arguments to a constructor of tchecker::intval_t
 \return an instance of intval_t of size values constructed from args
 */
template <class... ARGS> tchecker::intval_t * intval_allocate_and_construct(unsigned short size, ARGS &&... args)
{
  char * ptr = new char[tchecker::allocation_size_t<tchecker::intval_t>::alloc_size(size)];

  tchecker::intval_t::construct(ptr, args...);

  return reinterpret_cast<tchecker::intval_t *>(ptr);
}

/*!
 \brief Destruct and deallocate a bounded integer variables valuation
 \param v : bounded integer variables valuation
 \pre v has been returned by intval_allocate_and_construct
 \post v has been destructed and deallocated
 */
void intval_destruct_and_deallocate(tchecker::intval_t * v);

/*!
 \brief Output bounded integer variables valuation
 \param os : output stream
 \param intval : bounded integer variables valuation
 \param index : an index of integer variables
 \pre index is an index of variables in intval
 \post intval has been output to os with variable names from index
 \return os after output
 */
std::ostream & output(std::ostream & os, tchecker::intval_t const & intval, tchecker::intvar_index_t const & index);

/*!
 \brief Convert bounded integer variables valuation to string
 \param intval : ounded integer variables valuation
 \param index : an index of integer variables
 \pre index is an index of variables in intval
 \return An std::string representation of intvval using variable names from index
 */
std::string to_string(tchecker::intval_t const & intval, tchecker::intvar_index_t const & index);

/*!
 \brief Initialize a bounded integer variables valuation from a string
 \param intval : ounded integer variables valuation
 \param variables : flat bounded integer variables
 \param str : a string
 \pre str is a comma-separated list assignments.
 The lhs of each assignment is the name of a flattened bounded integer variable in variables
 The rhs of each assignment is an integer that can be represented as tchecker::integer_t
 The value assigned to each variable fits within the domain of the variable according to variables
 The list contains one assignment for each flattened bounded integer variables in variables
 \post the values in intval have been updated according to str
 \throw std::invalid_argument : if str is not syntactically correct,
 or if str does not match the precondition
 */
void from_string(tchecker::intval_t & intval, tchecker::flat_integer_variables_t const & variables, std::string const & str);

/*!
 \brief Lexical ordering on bounded integer variables valuations
 \param intval1 : first valuation
 \param intval2 : second valuation
 \return 0 if intval1 and intval2 are equal, a negative value if intval1 is smaller than intval2 w.r.t.
 lexical ordering, and a positive value otherwise.
 */
int lexical_cmp(tchecker::intval_t const & intval1, tchecker::intval_t const & intval2);

/*!
 \brief Type of shared integer variables valuation
 */
using shared_intval_t = tchecker::make_shared_t<tchecker::intval_t>;

/*!
 \brief Type of shared pointer to integer variables valuation
*/
using intval_sptr_t = tchecker::intrusive_shared_ptr_t<tchecker::shared_intval_t>;

} // end of namespace tchecker

#endif // TCHECKER_INTVARS_HH
