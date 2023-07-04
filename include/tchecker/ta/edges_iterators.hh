/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_TA_EDGES_ITERATORS_HH
#define TCHECKER_TA_EDGES_ITERATORS_HH

namespace tchecker {

namespace ta {

/*!
 \class edges_valuations_iterator_t
 \brief Type of iterator over edges and valuations
 \tparam EDGES_RANGE : type of range over edges
 \tparam VALUATIONS_RANGE : type of range over bounded integer variables valuations
 \note Behaves as a cartesian product of EDGES_RANGE and VALUATIONS_RANGE, except that
 it will iterate over EDGES_RANGE if VALUATIONS_RANGE is empty
*/
template <typename EDGES_RANGE, typename VALUATIONS_RANGE> class edges_valuations_iterator_t {
public:
  /*!
   \brief Constructor
   \param edges_range : range of edges
   \param valuations_range : range of bounded integer variables valuations
   \post this iterates over the cartesian product final_range * valuations_range if valuations_range
   is not empty, and over final_range only otherwise
  */
  edges_valuations_iterator_t(EDGES_RANGE const & edges_range, VALUATIONS_RANGE const & valuations_range)
      : _edges_range(edges_range), _edges_it(_edges_range.begin()), _valuations_range(valuations_range),
        _valuations_it(_valuations_range.begin())
  {
  }

  /*!
   \brief Copy constructor
   */
  edges_valuations_iterator_t(tchecker::ta::edges_valuations_iterator_t<EDGES_RANGE, VALUATIONS_RANGE> const &) = default;

  /*!
   \brief Move constructor
  */
  edges_valuations_iterator_t(tchecker::ta::edges_valuations_iterator_t<EDGES_RANGE, VALUATIONS_RANGE> &&) = default;

  /*!
   \brief Destructor
  */
  ~edges_valuations_iterator_t() = default;

  /*!
   \brief Assignment operator
  */
  tchecker::ta::edges_valuations_iterator_t<EDGES_RANGE, VALUATIONS_RANGE> &
  operator=(tchecker::ta::edges_valuations_iterator_t<EDGES_RANGE, VALUATIONS_RANGE> const &) = default;

  /*!
   \brief More-assignment operator
  */
  tchecker::ta::edges_valuations_iterator_t<EDGES_RANGE, VALUATIONS_RANGE> &
  operator=(tchecker::ta::edges_valuations_iterator_t<EDGES_RANGE, VALUATIONS_RANGE> &&) = default;

  /*!
   \brief Equality check
   \param it : iterator
   \return true if it and this iterator have same ranges and current iterator
  */
  bool operator==(tchecker::ta::edges_valuations_iterator_t<EDGES_RANGE, VALUATIONS_RANGE> const & it) const
  {
    return (_edges_range == it._edges_range && _edges_it == it._edges_it && _valuations_range == it._valuations_range &&
            _valuations_it == it._valuations_it);
  }

  /*!
   \brief Equality check
   \param it : past-the-end iterator
   \return true if this iterator is past-the-end
  */
  inline bool operator==(tchecker::end_iterator_t const &) const { return at_end(); }

  /*!
   \brief Disequality check
   \param it : iterator
   \return true if it and this iterator do not have same ranges and current iterator
  */
  inline bool operator!=(tchecker::ta::edges_valuations_iterator_t<EDGES_RANGE, VALUATIONS_RANGE> const & it) const
  {
    return !(*this == it);
  }

  /*!
   \brief Disequality check
   \param it : past-the-end iterator
   \return true if this iterator is not past-the-end
  */
  inline bool operator!=(tchecker::end_iterator_t const & it) const { return !(*this == it); }

  /*!
   \brief Dereference operator
   \return the tuple <e, v> of final edges valuation e, and flat bounded integer variables valuation v
   that is pointed to by this iterator (if any)
   \pre this iterator is not past-the-end (checked by assertion)
  */
  decltype(auto) operator*()
  {
    assert(!at_end());
    return std::make_tuple(*_edges_it, *_valuations_it);
  }

  /*!
   \brief Increment operator
   \return this after incrementation
   \pre this iterator is not past-the-end (checked by assertion)
  */
  tchecker::ta::edges_valuations_iterator_t<EDGES_RANGE, VALUATIONS_RANGE> & operator++()
  {
    assert(!at_end());

    // Increment valuations iterator if range is not empty, we are done if iterator not at end
    if (!_valuations_range.empty()) {
      ++_valuations_it;
      if (_valuations_it != _valuations_range.end())
        return *this;
    }

    // Increment edges iterator, return if past-the-end
    ++_edges_it;
    if (_edges_it == _edges_range.end())
      return *this;

    // Sets back valuations iterator at beginning
    _valuations_it = _valuations_range.begin();

    return *this;
  }

private:
  /*!
   \brief Check for past-the-end iterator
   \return true if this operator is past-the-end
   \note this operator is past-the-end once there is no more edges left: it does not
   depend in valuations
  */
  inline bool at_end() const { return (_edges_it == _edges_range.end()); }

  EDGES_RANGE _edges_range;                                   /*!< Range of edges */
  typename EDGES_RANGE::begin_iterator_t _edges_it;           /*!< Iterator over range of edges */
  VALUATIONS_RANGE _valuations_range;                         /*!< Range of valuations */
  typename VALUATIONS_RANGE::begin_iterator_t _valuations_it; /*!< Iterator over range of valuations */
};

} // namespace ta

} // namespace tchecker

#endif // TCHECKER_TA_EDGES_ITERATORS_HH