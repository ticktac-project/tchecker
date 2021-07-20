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
#include "tchecker/variables/access.hh"
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
   \post a clock variable with base name 'name' and with size 'size' has been
   declared
   \return the identifier of the declared clock
   \throw std::invalid_argument : if the precondition is violated
   \throw std::runtime_error : if there is no clock identifier left
   */
  tchecker::clock_id_t declare(std::string const & name, tchecker::clock_id_t size);

protected:
  using tchecker::array_variables_t<tchecker::clock_id_t, tchecker::clock_info_t, tchecker::clock_index_t>::declare;
};

/*!
 \brief Type of flat clocks
 */
using flat_clock_variables_t =
    tchecker::flat_variables_t<tchecker::clock_id_t, tchecker::clock_info_t, tchecker::clock_index_t>;

/*!
 \class clock_constraint_t
 \brief Clock constraint
 \note Represents a constraint x_i - x_j # k  where i and j are IDs of clock
 variables, # is either < or <=, and k is an integer.
 A constraint is diagonal if both i and j are distinct from tchecker::REFCLOCK_ID.
 If one of i and j is equal to tchecker::REFCLOCK_ID, the constraint is simple.
 Hence, x_i - x_j # k with j = tchecker::REFCLOCK_ID encodes the constraint x_i # k.
 And similarly, the constraints encodes -x_j # k when i = tchecker::REFCLOCK_ID.
 */
class clock_constraint_t {
public:
  /*!
   \brief Type of comparator
   */
  enum comparator_t {
    LT = 0, /*!< less-than < */
    LE = 1, /*!< less-than-or-equal-to <= */
  };

  /*!
   \brief Constructor
   \param id1 : index of 1st clock
   \param id2 : index of 2nd clock
   \param cmp : < or <=
   \param value : constraint value
   \pre id1 and id2 are not both tchecker::REFCLOCK_ID
   \post this represents id1 - id2 # value where # is < or <= depending on cmp
   \throw std::invalid_argument : if both id1 and id2 are tchecker::REFCLOCK_ID
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
  tchecker::clock_constraint_t & operator=(tchecker::clock_constraint_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::clock_constraint_t & operator=(tchecker::clock_constraint_t &&) = default;

  /*!
   \brief Accessor
   \return ID of 1st clock
   */
  inline constexpr tchecker::clock_id_t id1() const { return _id1; }

  /*!
   \brief Accessor
   \return ID of 1st clock
   */
  inline constexpr tchecker::clock_id_t & id1() { return _id1; }

  /*!
   \brief Accessor
   \return ID of 2nd clock
   */
  inline constexpr tchecker::clock_id_t id2() const { return _id2; }

  /*!
   \brief Accessor
   \return ID of 2nd clock
   */
  inline constexpr tchecker::clock_id_t & id2() { return _id2; }

  /*!
   \brief Accessor
   \return comparator (< or <=)
   */
  inline constexpr enum tchecker::clock_constraint_t::comparator_t comparator() const { return _cmp; }

  /*!
   \brief Accessor
   \return comparator (< or <=)
   */
  inline constexpr enum tchecker::clock_constraint_t::comparator_t & comparator() { return _cmp; }

  /*!
   \brief Accessor
   \return value
   */
  inline constexpr tchecker::integer_t value() const { return _value; }

  /*!
   \brief Accessor
   \return value
   */
  inline constexpr tchecker::integer_t & value() { return _value; }

  /*!
   \brief Accessor
   \return true if this is a diagonal constraint, false otherwise
   \note diagonal() is equivalent to !simple()
   */
  inline constexpr bool diagonal() const { return ((_id1 != tchecker::REFCLOCK_ID) && (_id2 != tchecker::REFCLOCK_ID)); }

  /*!
   \brief Accessor
   \return true if this is a simple constraint, false otherwise
   \note simple() is equivalent to !diagonal()
   */
  inline constexpr bool simple() const { return ((_id1 == tchecker::REFCLOCK_ID) || (_id2 == tchecker::REFCLOCK_ID)); }

protected:
  friend std::ostream & operator<<(std::ostream & os, tchecker::clock_constraint_t const & c);

  tchecker::clock_id_t _id1;                            /*!< ID of 1st clock */
  tchecker::clock_id_t _id2;                            /*!< ID of 2nd clock */
  enum tchecker::clock_constraint_t::comparator_t _cmp; /*!< Comparator < or <= */
  tchecker::integer_t _value;                           /*!< Value */
};

/*!
 \brief Equality predicate
 \param c1 : first clock constraint
 \param c2 : second clock constraint
 \return true if c1 and c2 have same: 1st clock, 2nd clock, comparator and value,
 false otherwise
 */
bool operator==(tchecker::clock_constraint_t const & c1, tchecker::clock_constraint_t const & c2);

/*!
 \brief Disequality predicate
 \param c1 : first clock constraint
 \param c2 : second clock constraint
 \return false if c1 and c2 have same: 1st clock, 2nd clock, comparator and value,
 true otherwise
 */
bool operator!=(tchecker::clock_constraint_t const & c1, tchecker::clock_constraint_t const & c2);

/*!
 \brief Hash function
 \param c : clock constraint
 \return hash value for clock constraint c
 */
std::size_t hash_value(tchecker::clock_constraint_t const & c);

/*!
 \brief Output operator
 \param os : output stream
 \param c : clock constraint
 \post c has been output to os
 \return os after output
 */
std::ostream & operator<<(std::ostream & os, tchecker::clock_constraint_t const & c);

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
 \post all the clock constraints in range have been output to os with clock
 names from index
 \return os after output
 */
template <class CLKCONSTR_ITERATOR>
std::ostream & output_clock_constraints(std::ostream & os, tchecker::range_t<CLKCONSTR_ITERATOR> const & range,
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
 \brief Lexical ordering on clock constraints
 \param c1 : first clock constraint
 \param c2 : second clock constraint
 \return 0 if c1 and c2 are equal, a negative value if c1 is smaller than c2
 w.r.t. lexical ordering, a positive value otherwise
 */
int lexical_cmp(tchecker::clock_constraint_t const & c1, tchecker::clock_constraint_t const & c2);

/*!
 \brief Clock constraint container
 */
using clock_constraint_container_t = std::vector<tchecker::clock_constraint_t>;

/*!
 \brief Const iterator over clock constraint container
 */
using clock_constraint_container_const_iterator_t = tchecker::clock_constraint_container_t::const_iterator;

/*!
 \brief Lexical ordering on clock constraint containers
 \param c1 : first clock constraint container
 \param c2 : second clock constraint container
 \return 0 if c1 and c2 are equal, a negative value if c1 is smaller than c2
 w.r.t. lexical ordering, a positive value otherwise
 */
int lexical_cmp(tchecker::clock_constraint_container_t const & c1, tchecker::clock_constraint_container_t const & c2);

/*!
 \class clock_reset_t
 \brief Clock reset
 \note Represents a clock reset x_i = x_j + k where i and j are IDs of clock
 variables and k is a non-negative integer.
 A reset to constant x_i = k is encoded by letting j = tchecker::REFCLOCK_ID.
 */
class clock_reset_t {
public:
  /*!
   \brief Constructor
   \param left_id : ID of left-value clock
   \param right_id : ID of right-value clock
   \param value : reset value
   \pre left_id is not tchecker::REFCLOCK_ID, and value is non-negative
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
  tchecker::clock_reset_t & operator=(tchecker::clock_reset_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::clock_reset_t & operator=(tchecker::clock_reset_t &&) = default;

  /*!
   \brief Accessor
   \return ID of the left-value clock
   */
  inline constexpr tchecker::clock_id_t left_id() const { return _left_id; }

  /*!
   \brief Accessor
   \return ID of the left-value clock
   */
  inline constexpr tchecker::clock_id_t & left_id() { return _left_id; }

  /*!
   \brief Accessor
   \return ID of the right-value clock
   */
  inline constexpr tchecker::clock_id_t right_id() const { return _right_id; }

  /*!
   \brief Accessor
   \return ID of the right-value clock
   */
  inline constexpr tchecker::clock_id_t & right_id() { return _right_id; }

  /*!
   \brief Accessor
   \return reset value
   */
  inline constexpr tchecker::integer_t value() const { return _value; }

  /*!
   \brief Accessor
   \return reset value
   */
  inline constexpr tchecker::integer_t & value() { return _value; }

  /*!
   \brief Accessor
   \return true if this is a reset to 0 (i.e. x=0), false otherwise
   */
  inline constexpr bool reset_to_zero() const { return (reset_to_constant() && (_value == 0)); }

  /*!
   \brief Accessor
   \return true if this is a reset to a constant (i.e. x=k), false otherwise
   */
  inline constexpr bool reset_to_constant() const { return (_right_id == tchecker::REFCLOCK_ID); }

  /*!
   \brief Accessor
   \return true if this is a reset to a clock (i.e. x=y), false otherwise
   */
  inline constexpr bool reset_to_clock() const { return (!reset_to_constant() && (_value == 0)); }

  /*!
   \brief Accessor
   \return true if this is a reset to a clock plus a positive constant (i.e.
   x=y+k with k>0), false otherwise
   */
  inline constexpr bool reset_to_sum() const { return (!reset_to_constant() && (_value > 0)); }

protected:
  friend std::ostream & operator<<(std::ostream & os, tchecker::clock_reset_t const & r);

  tchecker::clock_id_t _left_id;  /*!< Left-value clock ID */
  tchecker::clock_id_t _right_id; /*!< Right-value clock ID */
  tchecker::integer_t _value;     /*!< Reset value */
};

/*!
 \brief Equality predicate
 \param r1 : first clock reset
 \param r2 : second clock reset
 \return true if r1 and r2 have same: left id, right id, and value,
 false otherwise
 */
bool operator==(tchecker::clock_reset_t const & r1, tchecker::clock_reset_t const & r2);

/*!
 \brief Disequality predicate
 \param r1 : first clock reset
 \param r2 : second clock reset
 \return false if r1 and r2 have same: left id, right id, and value,
 true otherwise
 */
bool operator!=(tchecker::clock_reset_t const & r1, tchecker::clock_reset_t const & r2);

/*!
 \brief Hash function
 \param r : clock reset
 \return hash value for clock reset r
 */
std::size_t hash_value(tchecker::clock_reset_t const & r);

/*!
 \brief Output operator
 \param os : output stream
 \param r : clock reset
 \post r has been output to os
 \return os after output
 */
std::ostream & operator<<(std::ostream & os, tchecker::clock_reset_t const & r);

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
 \post all the clock resets in range have been output to os with clock names
 from index
 \return os after output
 */
template <class CLKRESET_ITERATOR>
std::ostream & output_clock_resets(std::ostream & os, tchecker::range_t<CLKRESET_ITERATOR> const & range,
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
 \brief Lexical ordering on clock resets
 \param r1 : first clock reset
 \param r2 : second clock reset
 \return 0 if r1 and r2 are equal, a negative value if r1 is smaller than r2
 w.r.t. lexical ordering, a positive value otherwise
 */
int lexical_cmp(tchecker::clock_reset_t const & r1, tchecker::clock_reset_t const & r2);

/*!
 \brief Clock reset container
 */
using clock_reset_container_t = std::vector<tchecker::clock_reset_t>;

/*!
 \brief Const iterator over clock reset container
 */
using clock_reset_container_const_iterator_t = tchecker::clock_reset_container_t::const_iterator;

/*!
 \brief Lexical ordering on clock reset containers
 \param c1 : first clock reset container
 \param c2 : second clock reset container
 \return 0 if c1 and c2 are equal, a negative value if c1 is smaller than c2
 w.r.t. lexical ordering, a positive value otherwise
 */
int lexical_cmp(tchecker::clock_reset_container_t const & c1, tchecker::clock_reset_container_t const & c2);

/*!
 \class reference_clock_variables_t
 \brief Declaration of clock variables w.r.t. reference clock variables.
 \note Reference clocks allow to model polychronous time. Each reference clock
 has its own time scale. Clocks that are mapped to the same reference clock
 evolve on the same time scale. The standard semantics for timed automata rely
 on a single time scale: a single reference clock usually denoted 0. Reference
 clocks allow to extend the standard semantics to multiple time scales (see
 tchecker/dbm/refdbm.hh for DBMs with reference clocks).
 We distinguish between system clock variables with have IDs 0..N-1 and are all
 actual clocks on the one hand, and reference clock variables with the first
 0..refcount-1 clocks which are reference clocks, followed by N actual clocks.
 */
class reference_clock_variables_t : public tchecker::flat_clock_variables_t {
public:
  /*!
   \brief Constructor
   \param proc_refname_map : map from process IDs to reference clock names
   \pre proc_refname_map is not empty
   \post All reference clocks in proc_refname_map have been declared.
   Each process i is mapped to reference clock proc_refname_map[i].
   Two processes are mapped to the same reference clock if proc_refname_aap maps
   them to the same reference clock name.
   The first reference clock has ID 0, and all other reference clocks get their
   ID w.r.t. the order in which they appear in proc_refname_map (the ID of the
   first occurrence in case of multiple occurrences)
   \throw std::invalid_argument : if proc_refname_map is empty
   */
  reference_clock_variables_t(std::vector<std::string> const & proc_refname_map);

  /*!
   \brief Copy constructor
   */
  reference_clock_variables_t(tchecker::reference_clock_variables_t const &) = default;

  /*!
   \brief Move constructor
   */
  reference_clock_variables_t(tchecker::reference_clock_variables_t &&) = default;

  /*!
   \brief Destructor
   */
  ~reference_clock_variables_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::reference_clock_variables_t & operator=(tchecker::reference_clock_variables_t const &) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::reference_clock_variables_t & operator=(tchecker::reference_clock_variables_t &&) = default;

  /*!
   \brief Declare a clock
   \param name : clock name
   \param refclock : name of reference clock
   \pre 'name' is not a declared variable
   'refclock' is the name of a declared reference clock
   \post a clock variable with base name 'name', dimension 1 and reference clock
   'refclock' has been declared
   \return identifier of the declared clock
   \throw std::invalid_argument : if the precondition is violated
   */
  tchecker::clock_id_t declare(std::string const & name, std::string const & refclock);

  /*!
   \brief Accessor
   \return Number of reference clocks
   */
  constexpr inline tchecker::clock_id_t refcount() const { return _refcount; }

  /*!
   \brief Accessor
   \return Reference map: clock ID -> reference clock ID
   \note Each reference clock is mapped to itself, and each clock variable is
   mapped to its reference clock
   */
  constexpr inline std::vector<tchecker::clock_id_t> const & refmap() const { return _refmap; }

  /*!
   \brief Accessor
   \return Map : process ID -> reference clock ID that maps each process to its
   reference clock
  */
  constexpr inline std::vector<tchecker::clock_id_t> const & procmap() const { return _procmap; }

  /*!
    \brief Translate a clock constraint w.r.t. reference clocks
    \param c : a clock constraint
    \pre c is expressed over system clocks 0..size() - refcount() plus
    tchecker::REFCLOCK_ID (checked by assertion)
    \return c translated over this reference clocks
   */
  tchecker::clock_constraint_t translate(tchecker::clock_constraint_t const & c) const;

  /*!
    \brief Translate a clock reset w.r.t reference clocks
    \param r : a clock reset
    \pre r is expressed over system clocks 0..size() - refcount() plus
    tchecker::REFCLOCK_ID (checked by assertion)
    \return r translated over this reference clocks
  */
  tchecker::clock_reset_t translate(tchecker::clock_reset_t const & r) const;

  /*!
    \brief Translate system clock ID to this reference map
    \brief id : system clock ID
    \pre id belongs to 0..size(tchecker::VK_FLATTENED) - _refcount (checked by
    assertion)
    \return the identifier of clock id in this reference map
  */
  constexpr inline tchecker::clock_id_t translate_system_clock(tchecker::clock_id_t id) const
  {
    assert(id < size() - _refcount);
    return id + _refcount;
  }

  /*!
    \brief Returns reference clock of a system clock
    \brief id : system clock ID
    \pre id belongs to 0..size(tchecker::VK_FLATTENED) - _refcount (checked by
    assertion)
    \return the ID of the reference clock of id in this reference map
   */
  inline tchecker::clock_id_t refclock_of_system_clock(tchecker::clock_id_t id) const
  {
    assert(id < size() - _refcount);
    return _refmap[translate_system_clock(id)];
  }

private:
  using tchecker::flat_clock_variables_t::declare;

  /*!
   \brief Declare a variable
   \param name : variable name
   \param refid : ID of reference clock
   \pre 'name' is not a declared variable
   'refid' is a valid reference clock identifier (checked by assertion)
   \post an offset variable with base name 'name', dimension 1 and reference
   clock 'refid' has been declared
   \return identifier of declared clock
   \throw std::invalid_argument : if a variable 'name' has already been declared
   */
  tchecker::clock_id_t declare(std::string const & name, tchecker::clock_id_t refid);

  /*!
   \brief Declare a reference clock
   \param name : reference clock name
   \pre 'name' is not a declared variable
   \post a reference clock with base name 'name' has been declared. The
   identifier of the reference clock is refcount() - 1
   \return identifier of declared reference clock
   \throw std::invalid_argument : if a variable with base name 'name' has
   already been declared
   \throw std::runtime_error : if an offset variable has already been declared
   */
  tchecker::clock_id_t declare_reference_clock(std::string const & name);

  tchecker::clock_id_t _refcount;             /*!< Number of reference clocks */
  std::vector<tchecker::clock_id_t> _refmap;  /*!< Map : offset clock ID -> reference clock ID */
  std::vector<tchecker::clock_id_t> _procmap; /*!< Map : process ID -> reference clock ID */
};

/* Reference clock builders */

/*!
 \brief Build reference clock variables w.r.t to single reference clock 0
 \param flat_clocks : flat clock variables
 \param proc_count : number of processes
 \return reference clocks with a single reference clock 0, and clock variables
 as in flat_clocks, all mapped to reference clock 0
*/
tchecker::reference_clock_variables_t single_reference_clocks(tchecker::flat_clock_variables_t const & flat_clocks,
                                                              tchecker::clock_id_t proc_count);

/*!
 \brief Build refeence clock variables from a variable access map
 \param vaccess_map : a variable access map
 \param flat_clocks : flat clock variables
 \param proc_count : number of processes
 \pre proc_count > 0
 all process IDs in vaccess_map should belong to [0,proc_count)
 \return reference clock variables with proc_count reference clocks, along with
 all clocks from flat_clocks.
 Each clock is mapped to the reference clock of the process that accesses the
 clock according to vaccess_map
 \throw std::invalid_argument : if proc_count == 0
 \throw std::invalid_argument : if the number of clocks (i.e. proc_count
 reference clocks + flat_clocks size) exceeds the number of representable clock
 identifiers
 */
tchecker::reference_clock_variables_t process_reference_clocks(tchecker::variable_access_map_t const & vaccess_map,
                                                               tchecker::flat_clock_variables_t const & flat_clocks,
                                                               tchecker::process_id_t proc_count);

} // end of namespace tchecker

#endif // TCHECKER_CLOCKS_HH
