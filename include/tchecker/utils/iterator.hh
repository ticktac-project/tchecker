/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ITERATOR_HH
#define TCHECKER_ITERATOR_HH

#include <cassert>
#include <functional>
#include <iterator>
#include <limits>
#include <tuple>
#include <vector>

#include <boost/iterator/transform_iterator.hpp>

#include "tchecker/basictypes.hh"

/*!
 \file iterator.hh
 \brief Iterators
 */

namespace tchecker {

/*!
 \class range_t
 \brief Range [begin,end) of iterators
 \tparam IBEGIN : type of iterator to first element
 \tparam IEND : type of past-the-end iterator
 */
template <class IBEGIN, class IEND = IBEGIN> class range_t {
public:
  /*!
   \brief Type of iterator to first element
   */
  using begin_iterator_t = IBEGIN;

  /*!
  \brief Type of past-the-end iterator
  */
  using end_iterator_t = IEND;

  /*!
   \brief Constructor
   \param begin : iterator to first element
   \param end : past-the-end iterator
   */
  range_t(IBEGIN const & begin, IEND const & end) : _begin(begin), _end(end) {}

  /*!
   \brief Constructor from different type of iterators
   \tparam TBEGIN : type of iterator to first element, IBEGIN iterators must be
   constructible from TBEGIN iterators
   \tparam TEND : type of past-the-end iterator, IEND iterators must be
   constructible from TEND iterators
   \param begin : iterator to first element
   \param end : past-the-end iterator
   */
  template <class TBEGIN, class TEND = TBEGIN> range_t(TBEGIN const & begin, TEND const & end) : _begin(begin), _end(end) {}

  /*!
   \brief Copy constructor
   \param range : a range
   \post this is a copy of range
   */
  range_t(tchecker::range_t<IBEGIN, IEND> const & range) = default;

  /*!
   \brief Move constructor
   \param range : a range
   \post range gas been moved to this
   */
  range_t(tchecker::range_t<IBEGIN, IEND> && range) = default;

  /*!
   \brief Destructor
   */
  ~range_t() = default;

  /*!
   \brief Assignment operator
   \param range : a range
   \post this is a copy of range
   \return this after assignment
   */
  tchecker::range_t<IBEGIN, IEND> & operator=(tchecker::range_t<IBEGIN, IEND> const & range) = default;

  /*!
   \brief Move assignment operator
   \param range : a range
   \post range has been moved to this
   \return this after assignment
   */
  tchecker::range_t<IBEGIN, IEND> & operator=(tchecker::range_t<IBEGIN, IEND> && range) = default;

  /*!
   \brief Accessor
   \return iterator to first element
   */
  inline constexpr IBEGIN const & begin() const { return _begin; }

  /*!
   \brief Accessor
   \return past-the-end iterator
   */
  inline constexpr IEND const & end() const { return _end; }

  /*!
   \brief Makes a tuple of iterators from a range
   \return tuple of iterators (begin,end) corresponding to this range
   */
  inline std::tuple<IBEGIN, IEND> iterators() const { return std::make_tuple(_begin, _end); }

  /*!
   \brief Equality check
   \param range : a range
   \return true if this and range are equal, false otherwise
   */
  inline constexpr bool operator==(tchecker::range_t<IBEGIN, IEND> const & range) const
  {
    return ((_begin == range._begin) && (_end == range._end));
  }

  /*!
   \brief Disequality check
   \param range : a range
   \return true if this and range are not equal, false otherwise
   */
  inline constexpr bool operator!=(tchecker::range_t<IBEGIN, IEND> const & range) const { return (!(*this == range)); }

  /*!
   \brief Emptiness check
   \return true if this is an empty range, false otherwise
   */
  inline constexpr bool empty() const { return (_begin == _end); }

protected:
  IBEGIN _begin; /*!< Iterator to first element */
  IEND _end;     /*!< Past-the-end iterator */
};

/*!
 \brief Make a range
 \tparam IBEGIN : type of iterator to first element
 \tparam IEND : type of past-the-end iterator
 \param begin : iterator on first element
 \param end : past-the-end operator
 \return range over [begin,end)
 */
template <class IBEGIN, class IEND = IBEGIN> tchecker::range_t<IBEGIN, IEND> make_range(IBEGIN const & begin, IEND const & end)
{
  return tchecker::range_t<IBEGIN, IEND>(begin, end);
}

/*!
 \brief Make a range from iterators of different types
 \tparam IBEGIN : type of iterator to first element in resulting range
 \tparam IEND : type of past-the-end iterator in resulting range
 \tparam TBEGIN : type of iterator to first element as input, IBEGIN iterators
 must be constructible from TBEGIN iterators
 \tparam TEND : type of past-the-end iterator as input, IEND iterators must be
 constructible from TEND iterators
 \param begin : iterator on first element
 \param end : past-the-end operator
 \return range of type <IBEGIN, IEND> over [begin,end)
 */
template <class IBEGIN, class IEND = IBEGIN, class TBEGIN, class TEND = TBEGIN>
tchecker::range_t<IBEGIN, IEND> make_range(TBEGIN const & begin, TEND const & end)
{
  return tchecker::range_t<IBEGIN, IEND>{begin, end};
}

/*!
 \brief Make range from a container (or any object with a access to a range
 with methods begin() and end())
 \tparam C : type of container
 \param c : container
 \return range over values in c
 */
template <class C> decltype(auto) make_range(C & c) { return tchecker::make_range(c.begin(), c.end()); }

/*!
\brief Type of past-the-end iterator for fast end-of-range detection
\note used to represent past-the-end iterators in ranges, and define dedicated
past-the-end checks that can be faster than equality checks on some iterators.
*/
class end_iterator_t {};

static end_iterator_t const past_the_end_iterator; /*!< Past-the-end iterator */

/*!
 \brief Equality predicate on past-the-end iterators
 \param it1 : past-the-end iterator
 \param it2 : past-the-end iterator
 \return true if it1 and it2 have the same address, false otherwise
*/
bool operator==(tchecker::end_iterator_t const & it1, tchecker::end_iterator_t const & it2);

/*!
 \brief Disequality predicate on past-the-end iterators
 \param it1 : past-the-end iterator
 \param it2 : past-the-end iterator
 \return true if it1 and it2 have distinct addresses, false otherwise
*/
bool operator!=(tchecker::end_iterator_t const & it1, tchecker::end_iterator_t const & it2);

/*!
 \class join_iterator_t
 \brief Join iterator over ranges. join_iterator_t makes a range of ranges
 ((x11,...,x1N),...,(xK1,...,xKM)) appear as the flat range
 (x11,...,x1N,...,xK1,...,xKM), skipping empty sub-ranges
 \tparam R : type of range of sub-ranges, should be a tchecker::range_t
 \tparam SUBR : type of sub-range, should be a tchecker::range_t
 \note All ranges must be constant ranges
 */
template <class R, class SUBR> class join_iterator_t {
public:
  /*!
   \brief Type of range
   */
  using range_t = R;

  /*!
  \brief Type of sub-range
  */
  using sub_range_t = SUBR;

  /*!
   \brief Default constructor
   \note required by C++ concept ForwardIterator
   */
  join_iterator_t()
      : _get_sub_range([](typename R::begin_iterator_t const & /*it*/) {
          return tchecker::make_range(typename SUBR::begin_iterator_t{}, typename SUBR::end_iterator_t{});
        })
  {
  }

  /*!
   \brief Constructor
   \param begin : iterator on a range of type R
   \param end : past-the-end iterator on a range of type R
   \param get_sub_range : function that yields a SUBR range from an iterator
   over [begin,end)
   \post this iterator ranges over [begin,end) and uses get_sub_range to access sub ranges
   \note this iterator stores get_sub_range
   */
  join_iterator_t(typename R::begin_iterator_t const & it, typename R::end_iterator_t const & end,
                  std::function<SUBR(typename R::begin_iterator_t const &)> get_sub_range)
      : _it(it), _end(end), _get_sub_range(get_sub_range)
  {
    advance_while_empty_range();
  }

  /*!
   \brief Constructor
   \param r : range
   \param get_sub_range : function that yields a SUBR range from an iterator
   over r
   \post this iterator ranges from r.begin() to r.end()
   */
  join_iterator_t(R const & r, std::function<SUBR(typename R::begin_iterator_t const &)> get_sub_range)
      : join_iterator_t(r.begin(), r.end(), get_sub_range)
  {
  }

  /*!
   \brief Copy constructor
   \param it : join iterator
   \post this is a copy of it
   */
  join_iterator_t(tchecker::join_iterator_t<R, SUBR> const & it) = default;

  /*!
   \brief Move constructor
   \param it : join iterator
   \post it has been moved to this
   */
  join_iterator_t(tchecker::join_iterator_t<R, SUBR> && it) = default;

  /*!
   \brief Destructor
   */
  ~join_iterator_t() = default;

  /*!
   \brief Assignment operator
   \param it : iterator
   \post this is a copy of it
   \return this after assignment
   */
  tchecker::join_iterator_t<R, SUBR> & operator=(tchecker::join_iterator_t<R, SUBR> const & it) = default;

  /*!
   \brief Move assignment operator
   \param it : iterator
   \post it has been moved to this
   \return this after assignment
   */
  tchecker::join_iterator_t<R, SUBR> & operator=(tchecker::join_iterator_t<R, SUBR> && it) = default;

  /*!
   \brief Equality check
   \param it : join iterator
   \return true if this and it are equal, false otherwise
   */
  bool operator==(tchecker::join_iterator_t<R, SUBR> const & it) const
  {
    return ((_it == it._it) && (_end == it._end) && (_range_it == it._range_it) && (_range_end == it._range_end));
  }

  /*!
   \brief Disequality check
   \param it : join iterator
   \return negation of join_iterator_t::operator==
   */
  inline bool operator!=(tchecker::join_iterator_t<R, SUBR> const & it) const { return (!(*this == it)); }

  /*!
  \brief Equality check w.r.t. past-the-end iterator
  \param end : past-the-end iterator
  \return see at_end()
  */
  inline bool operator==(tchecker::end_iterator_t const & /*end*/) const { return at_end(); }

  /*!
  \brief Disequality check w.r.t. past-the-end iterator
  \param end : past-the-end iterator
  \return negation of join_iterator_t::operator==
  */
  inline bool operator!=(tchecker::end_iterator_t const & end) const { return !(*this == end); }

  /*!
   \brief Accessor (dereference)
   \pre this is not past-the-end (checked by assertion)
   \return current value
   */
  inline decltype(auto) operator*() const
  {
    assert((_it != _end) && (_range_it != _range_end));
    return (*_range_it);
  }

  /*!
   \brief Increment (next)
   \pre this is not past-the-end (checked by assertion)
   \post this has been incremented
   \return this after incrementation
   */
  tchecker::join_iterator_t<R, SUBR> & operator++()
  {
    assert((_it != _end) && (_range_it != _range_end));

    // Advance range iterator
    ++_range_it;

    // Ok if range iterator is not past-the-end
    if (_range_it != _range_end)
      return *this;

    // Otherwise, advance iterator
    ++_it;
    advance_while_empty_range();
    return *this;
  }

protected:
  /*!
  \brief End-of-range check
  \return true if iterator is past-the-end, false otherwise
  \note this is more efficient that operator== on join_iterator_t
  */
  inline bool at_end() const { return (_it == _end); }

  /*!
   \brief Advance iterator
   \post this has been advanced till a non empty range is found, or
   past-the-end if all ranges are empty. Does not move if current range is
   not empty
   */
  void advance_while_empty_range()
  {
    // Find first non-empty range, if any
    while (_it != _end) {
      std::tie(_range_it, _range_end) = _get_sub_range(_it).iterators();
      if (_range_it != _range_end)
        break;
      ++_it;
    }
    if (_it == _end) {
      _range_it = typename SUBR::begin_iterator_t{};
      _range_end = typename SUBR::end_iterator_t{};
    }
  }

  typename R::begin_iterator_t _it;          /*!< Iterator to first sub-range (if any) */
  typename R::end_iterator_t _end;           /*!< Past-the-end iterator on sub-ranges */
  typename SUBR::begin_iterator_t _range_it; /*!< Iterator in current sub-range */
  typename SUBR::end_iterator_t _range_end;  /*!< Past-the-end iterator in current sub-range */
  std::function<SUBR(typename R::begin_iterator_t const &)>
      _get_sub_range; /*!< Function to retrieve a SUBR range from an iterator over R */
};

} // end of namespace tchecker

/*!
 \brief Iterator traits for join_iterator_t
 */
template <class R, class SUBR> struct std::iterator_traits<tchecker::join_iterator_t<R, SUBR>> {
  using subr_iterator_t = typename tchecker::join_iterator_t<R, SUBR>::sub_range_t::begin_iterator_t;

  using difference_type = typename subr_iterator_t::difference_type;

  using value_type = typename subr_iterator_t::value_type;

  using pointer = typename subr_iterator_t::pointer;

  using reference = typename subr_iterator_t::reference;

  using iterator_category = std::forward_iterator_tag;
};

namespace tchecker {

/*!
 \class cartesian_iterator_t
 \brief Iterator over a cartesian product of ranges of the same type
 \tparam R : type of ranges, should have type tchecker::range_t<...>
 */
template <class R> class cartesian_iterator_t {
public:
  /*!
   \brief Constructor
   \post this is an empty cartesian iterator
   */
  cartesian_iterator_t() : _nranges(0) {}

  /*!
   \brief Copy constructor
   \param it : cartesian iterator
   \post this is a copy of it
   */
  cartesian_iterator_t(tchecker::cartesian_iterator_t<R> const & it) = default;

  /*!
   \brief Move constructor
   \param it : cartesian iterator
   \post it has been moved to this
   */
  cartesian_iterator_t(tchecker::cartesian_iterator_t<R> && it) = default;

  /*!
   \brief Destructor
   */
  ~cartesian_iterator_t() = default;

  /*!
   \brief Assignment operator
   \param it : cartesian iterator
   \post this is a copy of it
   \return this after assignment
   */
  tchecker::cartesian_iterator_t<R> & operator=(tchecker::cartesian_iterator_t<R> const & it) = default;

  /*!
   \brief Move assignment operator
   \param it : cartesian iterator
   \post it has been moved to this
   \return this after assignment
   */
  tchecker::cartesian_iterator_t<R> & operator=(tchecker::cartesian_iterator_t<R> && it) = default;

  /*!
   \brief Clear the cartesian product
   \post this is an empty cartesian product
   \note clear invalidates iterations and ranges returned by operator*
   */
  inline void clear()
  {
    _nranges = 0;
    _begins.clear();
    _ends.clear();
    _its.clear();
    assert(_nranges == _begins.size());
    assert(_nranges == _ends.size());
    assert(_nranges == _its.size());
  }

  /*!
   \brief Add a range to this cartesian product
   \param r : range
   \post r has been added to this
   \note push_back invalidates iterations and ranges returned by operator*
   */
  void push_back(R const & r)
  {
    ++_nranges;
    _begins.push_back(r.begin());
    _ends.push_back(r.end());
    _its.push_back(r.begin());

    if (r.empty()) // detect empty cartesian product
      to_end();

    assert(_nranges == _begins.size());
    assert(_nranges == _ends.size());
    assert(_nranges == _its.size());
  }

  /*!
   \brief Move a range to this cartesian product
   \param r : range
   \post r has been moved to this
   \note push_back invalidates iterations and ranges returned by operator*
   */
  void push_back(R && r)
  {
    ++_nranges;
    _begins.emplace(_begins.end(), std::move(r.begin()));
    _ends.emplace(_ends.end(), std::move(r.end()));
    _its.emplace(_its.end(), std::move(r.begin()));

    if (r.empty()) // detect empty cartesian product
      to_end();

    assert(_nranges == _begins.size());
    assert(_nranges == _ends.size());
    assert(_nranges == _its.size());
  }

  /*!
   \brief Equality check
   \param it : iterator
   \return true if it and this are equal, false otherwise
   */
  bool operator==(tchecker::cartesian_iterator_t<R> const & it) const
  {
    return ((_nranges == it._nranges) && (_begins == it._begins) && (_ends == it._ends) && (_its == it._its));
  }

  /*!
   \brief Disequality check
   \param it : iterator
   \return true if it and this differ, false otherwise
   */
  bool operator!=(tchecker::cartesian_iterator_t<R> const & it) const { return (!(it == *this)); }

  /*!
  \brief Equality check w.r.t. past-the-end iterator
  \param end : past-the-end iterator
  \return see at_end()
  */
  inline bool operator==(tchecker::end_iterator_t const & /*end*/) const { return at_end(); }

  /*!
  \brief Disequality check w.r.t. past-the-end iterator
  \param end : past-the-end iterator
  \return negation of operator==
  */
  inline bool operator!=(tchecker::end_iterator_t const & end) const { return !(*this == end); }

  /*!
   \class values_iterator_t
   \brief Iterator over a tuple of values in the cartesian product
   \note Makes a vector of iterator appear as the sequence of values pointed
   by those iterators. Could be achieved using boost::transform_iterator, but
   has typing issues
   */
  class values_iterator_t {
  public:
    /*!
     \brief Default constructor
     */
    values_iterator_t() = default;

    /*!
     \brief Constructor
     \param it : iterator over a a tuple of iterators
     \post this equals it
     */
    values_iterator_t(typename std::vector<typename R::begin_iterator_t>::iterator it) : _it(it) {}

    /*!
     \brief Copy-constructor
     */
    values_iterator_t(values_iterator_t const &) = default;

    /*!
     \brief Move constructor
     */
    values_iterator_t(values_iterator_t &&) = default;

    /*!
     \brief Destructor
     */
    ~values_iterator_t() = default;

    /*!
     \brief Assignment operator
     */
    values_iterator_t & operator=(values_iterator_t const &) = default;

    /*!
     \brief Move assignment operator
     */
    values_iterator_t & operator=(values_iterator_t &&) = default;

    /*!
     \brief Equality check
     \param it : iterator
     \return true if this equals it, false otherwise
     */
    bool operator==(values_iterator_t const & it) const { return (_it == it._it); }

    /*!
     \brief Disequality check
     \param it : iterator
     \return true if this differs from it, false otherwise
     */
    bool operator!=(values_iterator_t const & it) const { return (!(*this == it)); }

    /*!
     \brief Accessor
     \return value pointed by current pointed iterator
     */
    decltype(auto) operator*() { return **_it; }

    /*!
     \brief Forward move
     \post this has been moved one step forward
     \return thsi after it has been moved
     */
    values_iterator_t & operator++()
    {
      ++_it;
      return *this;
    }

  private:
    typename std::vector<typename R::begin_iterator_t>::iterator _it; /*!< Iterator */
  };

  /*!
   \brief Accessor
   \return current element
   \note the returned ranged is invalidated by calls to operator++
   */
  tchecker::range_t<tchecker::cartesian_iterator_t<R>::values_iterator_t> operator*()
  {
    values_iterator_t begin(_its.begin()), end(_its.end());
    return tchecker::make_range(begin, end);
  }

  /*!
   \brief Move to next element
   \pre not at_end()  (checked by assertion)
   \post this points to nest element or past-the-end
   \note operator++ invalidates all ranges returned by operator*
   */
  tchecker::cartesian_iterator_t<R> & operator++()
  {
    assert(!at_end());

    ssize_t i = _nranges - 1;
    while (i >= 0) {
      ++_its[i];
      if (_its[i] != _ends[i]) // range i not at end
        break;
      if (i > 0) // set range i to beginning if not done
        _its[i] = _begins[i];
      --i;
    }
    return *this;
  }

private:
  /*!
   \brief Accessor
   \return true if current element is past-the-end cartesian product, false
   otherwise
   \note Complexity is constant time
   */
  inline bool at_end() const { return ((_nranges == 0) || (_its[0] == _ends[0])); }

  /*!
   \brief Move to past-the-end
   \note Complexity is constant time
   */
  inline void to_end()
  {
    if (_nranges != 0)
      _its[0] = _ends[0];
  }

  std::size_t _nranges;                              /*!< Number of ranges in the cartesian product */
  std::vector<typename R::begin_iterator_t> _begins; /*!< Iterators to first elements */
  std::vector<typename R::end_iterator_t> _ends;     /*!< Past-the-end-iterators */
  std::vector<typename R::begin_iterator_t> _its;    /*!< Iterators on current elements */
};

/*!
 \class cartesian_iterator2_t
 \brief Iterator over a cartesian product of two ranges of potentially distinct types
 \tparam R1 : type of first range, should have type tchecker::range_t<...>
 \tparam R2 : type of second range, should have type tchecker::range_t<...>
 */
template <typename R1, typename R2> class cartesian_iterator2_t {
public:
  /*!
   \brief Constructor
   \param r1 : first range
   \param r2 : second range
   \post this is an iterator over r1 * r2
   */
  cartesian_iterator2_t(R1 const & r1, R2 const & r2) : _r1(r1), _r2(r2), _it1(r1.begin()), _it2(r2.begin()), _empty(false)
  {
    if (r1.begin() == r1.end() || r2.begin() == r2.end())
      _empty = true;
  }

  /*!
   \brief Copy constructor
   \param it : cartesian iterator
   \post this is a copy of it
   */
  cartesian_iterator2_t(tchecker::cartesian_iterator2_t<R1, R2> const & it) = default;

  /*!
   \brief Move constructor
   \param it : cartesian iterator
   \post it has been moved to this
   */
  cartesian_iterator2_t(tchecker::cartesian_iterator2_t<R1, R2> && it) = default;

  /*!
   \brief Destructor
   */
  ~cartesian_iterator2_t() = default;

  /*!
   \brief Assignment operator
   \param it : cartesian iterator
   \post this is a copy of it
   \return this after assignment
   */
  tchecker::cartesian_iterator2_t<R1, R2> & operator=(tchecker::cartesian_iterator2_t<R1, R2> const & it) = default;

  /*!
   \brief Move assignment operator
   \param it : cartesian iterator
   \post it has been moved to this
   \return this after assignment
   */
  tchecker::cartesian_iterator2_t<R1, R2> & operator=(tchecker::cartesian_iterator2_t<R1, R2> && it) = default;

  /*!
   \brief Equality check
   \param it : iterator
   \return true if it and this are equal, false otherwise
   */
  bool operator==(tchecker::cartesian_iterator2_t<R1, R2> const & it) const
  {
    return (_r1 == it._r1 && _r2 == it._r2 && _it1 == it._it1 && _it2 == it._it2 && _empty == it._empty);
  }

  /*!
   \brief Disequality check
   \param it : iterator
   \return true if it and this differ, false otherwise
   */
  bool operator!=(tchecker::cartesian_iterator2_t<R1, R2> const & it) const { return !(it == *this); }

  /*!
  \brief Equality check w.r.t. past-the-end iterator
  \param end : past-the-end iterator
  \return see at_end()
  */
  inline bool operator==(tchecker::end_iterator_t const & /*end*/) const { return at_end(); }

  /*!
  \brief Disequality check w.r.t. past-the-end iterator
  \param end : past-the-end iterator
  \return negation of operator==
  */
  inline bool operator!=(tchecker::end_iterator_t const & end) const { return !(*this == end); }

  /*!
   \brief Accessor
   \pre not at_end() (checked by assertion)
   \return pair <v1, v2> in ranges r1 and r2 pointed to by this iterator
   */
  decltype(auto) operator*()
  {
    assert(!at_end());
    return std::make_tuple(*_it1, *_it2);
  }

  /*!
   \brief Increment operator
   \pre not at_end() (checked by assertion)
   \post this points to next element or past-the-end
   */
  tchecker::cartesian_iterator2_t<R1, R2> & operator++()
  {
    assert(!at_end());

    // increment _it2 if not at end
    ++_it2;
    if (_it2 != _r2.end())
      return *this;

    // else increment _it1
    ++_it1;
    if (_it1 == _r1.end())
      return *this;

    // if _it1 not at end, put back _it2 at beginning
    _it2 = _r2.begin();
    return *this;
  }

private:
  /*!
   \brief Accessor
   \return true if current element is past-the-end cartesian product, false
   otherwise
   */
  inline bool at_end() const { return (_empty || (_it1 == _r1.end() && _it2 == _r2.end())); }

  R1 _r1;                             /*!< First range */
  R2 _r2;                             /*!< Second range */
  typename R1::begin_iterator_t _it1; /*!< Current iterator in first range */
  typename R2::begin_iterator_t _it2; /*!< Current iterator in second range */
  bool _empty;                        /*!< True if this iterator is empty */
};

} // end of namespace tchecker

/*!
 \brief Iterator traits for cartesian_iterator_t
 */
template <class R> struct std::iterator_traits<tchecker::cartesian_iterator_t<R>> {
  using difference_type = typename std::iterator_traits<typename R::begin_iterator_t>::difference_type;

  using value_type = tchecker::range_t<typename tchecker::cartesian_iterator_t<R>::values_iterator_t>;

  using pointer = value_type *;

  using reference = value_type &;

  using iterator_category = std::forward_iterator_tag;
};

/*!
 \brief Iterator traits for cartesian_iterator2_t
 */
template <typename R1, typename R2> struct std::iterator_traits<tchecker::cartesian_iterator2_t<R1, R2>> {
  using difference_type = nullptr_t;

  using value_type = std::tuple<typename std::iterator_traits<typename R1::begin_iterator_t>::value_type,
                                typename std::iterator_traits<typename R2::begin_iterator_t>::value_type>;

  using pointer = value_type *;

  using reference = value_type &;

  using iterator_category = std::forward_iterator_tag;
};

namespace tchecker {

/*!
 \class integer_iterator_t
 \tparam I : type of integer values
 \brief Iterator over integer values
*/
template <typename I = tchecker::integer_t> class integer_iterator_t {
  static_assert(std::numeric_limits<I>::is_integer, "I must be an integer type");

public:
  /*!
   \brief Constructor
   \param value : initial value
  */
  integer_iterator_t(I value = 0) : _current(value) {}

  /*!
   \brief Copy constructor
  */
  integer_iterator_t(tchecker::integer_iterator_t<I> const &) = default;

  /*!
   \brief Move constructor
  */
  integer_iterator_t(tchecker::integer_iterator_t<I> &&) = default;

  /*!
   \brief Destructor
  */
  ~integer_iterator_t() = default;

  /*!
   \brief Assignment operator
  */
  tchecker::integer_iterator_t<I> & operator=(tchecker::integer_iterator_t<I> const &) = default;

  /*!
   \brief Assignment operator
  */
  tchecker::integer_iterator_t<I> & operator=(I value)
  {
    _current = value;
    return *this;
  }

  /*!
   \brief Move-ssignment operator
  */
  tchecker::integer_iterator_t<I> & operator=(tchecker::integer_iterator_t<I> &&) = default;

  /*!
   \brief Equality check
   \param it : iterator
   \return true if this iterator and it have the same current value, false otherwise
  */
  bool operator==(tchecker::integer_iterator_t<I> const & it) const { return _current == it._current; }

  /*!
   \brief Equality check
   \param value : integer value
   \return true if this current value is equal to value, false otherwise
  */
  bool operator==(I value) const { return _current == value; }

  /*!
   \brief Disequality check
  */
  bool operator!=(tchecker::integer_iterator_t<I> const & it) const { return !(*this == it); }

  /*!
   \brief Disequality check
  */
  bool operator!=(I value) const { return !(*this == value); }

  /*!
   \brief Dereference operator
   \return current value of this iterator
  */
  inline I operator*() const { return _current; }

  /*!
   \brief Increment operator
   \post this iterator points to the next integer according to modular arithmetic
  */
  tchecker::integer_iterator_t<I> & operator++()
  {
    ++_current;
    return *this;
  }

  /*!
   \brief Decrement operator
   \post this iterator points to the previous integer according to modular arithmetic
  */
  tchecker::integer_iterator_t<I> & operator--()
  {
    --_current;
    return *this;
  }

private:
  tchecker::integer_t _current; /*!< Current integer value */
};

/*!
 \brief Type of range over integers
 \tparam I : type of integer values
 */
template <typename I = tchecker::integer_t> using integer_range_t = tchecker::range_t<tchecker::integer_iterator_t<I>>;

/*!
 \brief Make a range of integers
 \tparam I : type of integer values
 \param begin : first integer
 \param end : past-the-end integer
 \return range over [begin,end)
 */
template <typename I = tchecker::integer_t> tchecker::integer_range_t<I> make_integer_range(I begin, I end)
{
  static_assert(std::numeric_limits<I>::is_integer, "I must be an integer tyoe");
  return tchecker::integer_range_t<I>(tchecker::integer_iterator_t<I>{begin}, tchecker::integer_iterator_t<I>{end});
}

} // namespace tchecker

/*!
 \brief Iterator traits for integer_iterator_t
 */
template <typename I> struct std::iterator_traits<tchecker::integer_iterator_t<I>> {
  using difference_type = I;

  using value_type = I;

  using pointer = value_type *;

  using reference = value_type &;

  using iterator_category = std::forward_iterator_tag;
};

#endif // TCHECKER_ITERATOR_HH
