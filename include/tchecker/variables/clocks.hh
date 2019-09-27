/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_CLOCKS_HH
#define TCHECKER_CLOCKS_HH

#include <iostream>
#include <string>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/utils/index.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/variables/variables.hh"

/*!
 \file clocks.hh
 \brief Clock variables and operations
 */

namespace tchecker {
  
  /*!
   \class clock_info_t
   \brief Informations on clock variables
   */
  class clock_info_t : public tchecker::size_info_t {
  public:
    using tchecker::size_info_t::size_info_t;
  };

  
  
  
  /*!
   \brief Index of clock variables
   */
  using clock_index_t = tchecker::index_t<tchecker::clock_id_t, std::string>;
  
  
  
  
  /*!
   \class clock_variables_t
   \brief Declaration of clock variables
   */
  class clock_variables_t
  : public tchecker::array_variables_t<tchecker::clock_id_t, tchecker::clock_info_t, tchecker::clock_index_t> {
  public:
    using tchecker::array_variables_t<tchecker::clock_id_t, tchecker::clock_info_t, tchecker::clock_index_t>::array_variables_t;
    
    /*!
     \brief Declare a clock variable
     \param name : variable name
     \param size : size (array)
     \pre 'name' is not a declared variable
     size > 0
     \post a clock variable with base name 'name' and with size 'size' has been declared
     \throw std::invalid_argument : if the precondition is violated
     */
    void declare(std::string const & name, tchecker::clock_id_t size)
    {
      tchecker::clock_info_t info{size};
      tchecker::array_variables_t<tchecker::clock_id_t, tchecker::clock_info_t, tchecker::clock_index_t>::declare(name, info);
    }
  protected:
    using tchecker::array_variables_t<tchecker::clock_id_t, tchecker::clock_info_t, tchecker::clock_index_t>::declare;
  };
  
  
  /*!
   \brief Type of flat clocks
   */
  using flat_clock_variables_t
  = tchecker::flat_variables_t<tchecker::clock_id_t, tchecker::clock_info_t, tchecker::clock_index_t>;

  
  
  
  /*!
   \class clock_constraint_t
   \brief Clock constraint
   \note Represents a constraint x_i - x_j # k  where i and j are IDs of clock
   variables, # is either < or <=, and k is an integer. A constraint is diagonal
   if both i and j are greater than 0. If one of i and j is zero, the constraint
   is simple.
   */
  class clock_constraint_t {
  public:
    /*!
     \brief Type of comparator
     */
    enum comparator_t {
      LT = 0,  /*!< less-than < */
      LE = 1,  /*!< less-than-or-equal-to <= */
    };
    
    /*!
     \brief Constructor
     \param id1 : index of 1st clock
     \param id2 : index of 2nd clock
     \param cmp : < or <=
     \param value : constraint value
     \post this represents id1 - id2 # value where # is < or <= depending on cmp
     */
    clock_constraint_t(tchecker::clock_id_t id1, tchecker::clock_id_t id2, enum tchecker::clock_constraint_t::comparator_t cmp,
                       tchecker::integer_t value);
    
    /*!
     \brief Copy constructor
     */
    clock_constraint_t(tchecker::clock_constraint_t const &) = default;
    
    /*!
     \brief Move constructor
     */
    clock_constraint_t(tchecker::clock_constraint_t &&) = default;
    
    /*!
     \brief Destructor
     */
    ~clock_constraint_t() = default;
    
    /*!
     \brief Assignment operator
     */
    tchecker::clock_constraint_t & operator= (tchecker::clock_constraint_t const &) = default;
    
    /*!
     \brief Move assignment operator
     */
    tchecker::clock_constraint_t & operator= (tchecker::clock_constraint_t &&) = default;
    
    /*!
     \brief Accessor
     \return ID of 1st clock
     */
    inline constexpr tchecker::clock_id_t id1() const
    {
      return _id1;
    }
    
    /*!
     \brief Accessor
     \return ID of 1st clock
     */
    inline constexpr tchecker::clock_id_t & id1()
    {
      return _id1;
    }
    
    /*!
     \brief Accessor
     \return ID of 2nd clock
     */
    inline constexpr tchecker::clock_id_t id2() const
    {
      return _id2;
    }
    
    /*!
     \brief Accessor
     \return ID of 2nd clock
     */
    inline constexpr tchecker::clock_id_t & id2()
    {
      return _id2;
    }
    
    /*!
     \brief Accessor
     \return comparator (< or <=)
     */
    inline constexpr enum tchecker::clock_constraint_t::comparator_t comparator() const
    {
      return _cmp;
    }
    
    /*!
     \brief Accessor
     \return comparator (< or <=)
     */
    inline constexpr enum tchecker::clock_constraint_t::comparator_t & comparator()
    {
      return _cmp;
    }
    
    /*!
     \brief Accessor
     \return value
     */
    inline constexpr tchecker::integer_t value() const
    {
      return _value;
    }
    
    /*!
     \brief Accessor
     \return value
     */
    inline constexpr tchecker::integer_t & value()
    {
      return _value;
    }
    
    /*!
     \brief Accessor
     \return true if this is a diagonal constraint, false otherwise
     \note diagonal() is equivalent to !simple()
     */
    inline constexpr bool diagonal() const
    {
      return ( (_id1 != tchecker::zero_clock_id) && (_id2 != tchecker::zero_clock_id) );
    }
    
    /*!
     \brief Accessor
     \return true if this is a simple constraint, false otherwise
     \note simple() is equivalent to !diagonal()
     */
    inline constexpr bool simple() const
    {
      return ( (_id1 == tchecker::zero_clock_id) || (_id2 == tchecker::zero_clock_id) );
    }
  protected:
    friend std::ostream & operator << (std::ostream & os, tchecker::clock_constraint_t const & c);
    
    tchecker::clock_id_t _id1;                             /*!< ID of 1st clock */
    tchecker::clock_id_t _id2;                             /*!< ID of 2nd clock */
    enum tchecker::clock_constraint_t::comparator_t _cmp;  /*!< Comparator < or <= */
    tchecker::integer_t _value;                            /*!< Value */
  };
  
  
  /*!
   \brief Output operator
   \param os : output stream
   \param c : clock constraint
   \post c has been output to os
   \return os after output
   */
  std::ostream & operator<< (std::ostream & os, tchecker::clock_constraint_t const & c);
  
  
  /*!
   \brief Output a clock constraint
   \param os : output stream
   \param c : clock constraint
   \param index : clock index
   \post c has been output to os with clock names from index
   \return os after c has been output
   */
  std::ostream & output(std::ostream & os, tchecker::clock_constraint_t const & c, tchecker::clock_index_t const & index);
  
  
  /*!
   \brief Output a range of clock constraints
   \param os : output stream
   \param range : range of clock constraints
   \param index : clock index
   \post all the clock constraints in range have been output to os with clock names from index
   \return os after output
   */
  template <class CLKCONSTR_ITERATOR>
  std::ostream & output_clock_constraints(std::ostream & os,
                                          tchecker::range_t<CLKCONSTR_ITERATOR> const & range,
                                          tchecker::clock_index_t const & index)
  {
    auto begin = range.begin(), end = range.end();
    for (auto it = begin; it != end; ++it) {
      if (it != begin)
        os << " & ";
      tchecker::output(os, *it, index);
    }
    return os;
  }
  
  
  
  
  /*!
   \brief Clock constraint container
   */
  using clock_constraint_container_t = std::vector<tchecker::clock_constraint_t>;
  
  /*!
   \brief Const iterator over clock constraint container
   */
  using clock_constraint_container_const_iterator_t = tchecker::clock_constraint_container_t::const_iterator;
  
  
  
  
  
  /*!
   \class clock_reset_t
   \brief Clock reset
   \note Represents a clock reset x_i = x_j + k where i and j are IDs of clock
   variables and k is a non-negative integer.
   */
  class clock_reset_t {
  public:
    /*!
     \brief Constructor
     \param left_id : ID of left-value clock
     \param right_id : ID of right-value clock
     \param value : reset value
     \pre left_id is not tchecker::zero_clock_id, and value is non-negative
     \post this represents clock reset left_id = right_id + value
     \throw std::invalid_argument : if the precondition is violated
     */
    clock_reset_t(tchecker::clock_id_t left_id, tchecker::clock_id_t right_id, tchecker::integer_t value);
    
    /*!
     \brief Copy constructor
     */
    clock_reset_t(tchecker::clock_reset_t const &) = default;
    
    /*!
     \brief Move constructor
     */
    clock_reset_t(tchecker::clock_reset_t &&) = default;
    
    /*!
     \brief Destructor
     */
    ~clock_reset_t() = default;
    
    /*!
     \brief Assignment operator
     */
    tchecker::clock_reset_t & operator= (tchecker::clock_reset_t const &) = default;
    
    /*!
     \brief Move qssignment operator
     */
    tchecker::clock_reset_t & operator= (tchecker::clock_reset_t &&) = default;
    
    /*!
     \brief Accessor
     \return ID of the left-value clock
     */
    inline constexpr tchecker::clock_id_t left_id() const
    {
      return _left_id;
    }
    
    /*!
     \brief Accessor
     \return ID of the left-value clock
     */
    inline constexpr tchecker::clock_id_t & left_id()
    {
      return _left_id;
    }
    
    /*!
     \brief Accessor
     \return ID of the right-value clock
     */
    inline constexpr tchecker::clock_id_t right_id() const
    {
      return _right_id;
    }
    
    /*!
     \brief Accessor
     \return ID of the right-value clock
     */
    inline constexpr tchecker::clock_id_t & right_id()
    {
      return _right_id;
    }
    
    /*!
     \brief Accessor
     \return reset value
     */
    inline constexpr tchecker::integer_t value() const
    {
      return _value;
    }
    
    /*!
     \brief Accessor
     \return reset value
     */
    inline constexpr tchecker::integer_t & value()
    {
      return _value;
    }
    
    /*!
     \brief Accessor
     \return true if this is a reset to 0 (i.e. x=0), false otherwise
     */
    inline constexpr bool reset_to_zero() const
    {
      return (reset_to_constant() && (_value == 0) );
    }
    
    /*!
     \brief Accessor
     \return true if this is a reset to a constant (i.e. x=k), false otherwise
     */
    inline constexpr bool reset_to_constant() const
    {
      return (_right_id == tchecker::zero_clock_id);
    }
    
    /*!
     \brief Accessor
     \return true if this is a reset to a clock (i.e. x=y), false otherwise
     */
    inline constexpr bool reset_to_clock() const
    {
      return ( ! reset_to_constant() && (_value == 0) );
    }
    
    /*!
     \brief Accessor
     \return true if this is a reset to a clock plus a positive constant (i.e.
     x=y+k with k>0), false otherwise
     */
    inline constexpr bool reset_to_sum() const
    {
      return ( ! reset_to_constant() && (_value > 0) );
    }
  protected:
    friend std::ostream & operator<< (std::ostream & os, tchecker::clock_reset_t const & r);
    
    tchecker::clock_id_t _left_id;   /*!< Left-value clock ID */
    tchecker::clock_id_t _right_id;  /*!< Right-value clock ID */
    tchecker::integer_t _value;      /*!< Reset value */
  };
  
  
  /*!
   \brief Output operator
   \param os : output stream
   \param r : clock reset
   \post r has been output to os
   \return os after output
   */
  std::ostream & operator<< (std::ostream & os, tchecker::clock_reset_t const & r);
  
  
  /*!
   \brief Output a clock reset
   \param os : output stream
   \param r : clock reset
   \param index : clock index
   \post r has been output to os with clock names from index
   \return os after r has been output
   */
  std::ostream & output(std::ostream & os, tchecker::clock_reset_t const & r, tchecker::clock_index_t const & index);
  
  
  /*!
   \brief Output a range of clock resets
   \param os : output stream
   \param range : range of clock resets
   \param index : clock index
   \post all the clock resets in range have been output to os with clock names from index
   \return os after output
   */
  template <class CLKRESET_ITERATOR>
  std::ostream & output_clock_resets(std::ostream & os,
                                     tchecker::range_t<CLKRESET_ITERATOR> const & range,
                                     tchecker::clock_index_t const & index)
  {
    auto begin = range.begin(), end = range.end();
    for (auto it = begin; it != end; ++it) {
      if (it != begin)
        os << "; ";
      tchecker::output(os, *it, index);
    }
    return os;
  }
  
  
  
  
  /*!
   \brief Clock reset container
   */
  using clock_reset_container_t = std::vector<tchecker::clock_reset_t>;
  
  /*!
   \brief Const iterator over clock reset container
   */
  using clock_reset_container_const_iterator_t = tchecker::clock_reset_container_t::const_iterator;
  
} // end of namespace tchecker

#endif // TCHECKER_CLOCKS_HH
