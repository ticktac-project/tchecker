/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYNCPROD_EDGES_ITERATORS_HH
#define TCHECKER_SYNCPROD_EDGES_ITERATORS_HH

#include <functional>
#include <vector>

#include <boost/iterator/transform_iterator.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/syncprod/system.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/system/edge.hh"
#include "tchecker/system/synchronization.hh"
#include "tchecker/system/system.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/utils/shared_objects.hh"

/*!
 \file edges_iterators.hh
 \brief Iterators on collection of edges to abstract synchronized and asynchronous edges
 */

namespace tchecker {

namespace syncprod {

/* Iterator over asynchronous edges from a tuple of locations */

/*!
 \brief Type of map: location identifier -> asynchronous edges
 */
using loc_to_asynchronous_edges_map_t =
    std::function<tchecker::range_t<tchecker::syncprod::system_t::asynchronous_edges_const_iterator_t>(tchecker::loc_id_t)>;

/*!
 \brief Type of iterator over range of asynchronous edges from a tuple of locations
 */
using vloc_asynchronous_edges_const_iterator_t =
    boost::transform_iterator<tchecker::syncprod::loc_to_asynchronous_edges_map_t, tchecker::vloc_t::const_iterator_t>;

/*!
 \brief Type of iterator over asynchronous edges from a tuple of locations
 */
using vloc_asynchronous_edges_iterator_t =
    tchecker::join_iterator_t<tchecker::range_t<tchecker::syncprod::vloc_asynchronous_edges_const_iterator_t>,
                              tchecker::range_t<tchecker::syncprod::system_t::asynchronous_edges_const_iterator_t>>;

/*!
 \brief Accessor to asynchronous edges from a tuple of locations
 \param system : a system
 \param vloc : a tuple of locations
 \return range of outgoing asynchronous edges from vloc in system
 */
tchecker::range_t<tchecker::syncprod::vloc_asynchronous_edges_iterator_t, tchecker::end_iterator_t>
outgoing_asynchronous_edges(tchecker::syncprod::system_t const & system,
                            tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc);

/* Iterator over collection of synchronized edges from a tuple of locations */

/*!
 \class vloc_synchronized_edges_iterator_t
 \brief Iterator over collection of synchronized edges (i.e. collection of edges) from a tuple of locations
 */
class vloc_synchronized_edges_iterator_t {
public:
  /*!
   \brief Constructor
   \param vloc : tuple of locations
   \param loc_edges_maps : maps loc id -> edges/events
   \param sync_begin : iterator on first synchronization
   \param sync_end : past-the-end iterator on synchronizations
   */
  vloc_synchronized_edges_iterator_t(tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc,
                                     std::shared_ptr<tchecker::system::loc_edges_maps_t const> const & loc_edges_maps,
                                     tchecker::system::synchronizations_t::const_iterator_t const & sync_begin,
                                     tchecker::system::synchronizations_t::const_iterator_t const & sync_end);

  /*!
   \brief Copy constructor
   */
  vloc_synchronized_edges_iterator_t(tchecker::syncprod::vloc_synchronized_edges_iterator_t const &) = default;

  /*!
   \brief Move constructor
   */
  vloc_synchronized_edges_iterator_t(tchecker::syncprod::vloc_synchronized_edges_iterator_t &&) = default;

  /*!
   \brief Destructor
   */
  ~vloc_synchronized_edges_iterator_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::syncprod::vloc_synchronized_edges_iterator_t &
  operator=(tchecker::syncprod::vloc_synchronized_edges_iterator_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::syncprod::vloc_synchronized_edges_iterator_t &
  operator=(tchecker::syncprod::vloc_synchronized_edges_iterator_t &&) = default;

  /*!
   \brief Equality check
   \param it : synchronized edges iterator
   \return true if this and it are equal, false otherwise
   \note equality relies on the fact that this and it reference the same loc_edges_maps
   */
  bool operator==(tchecker::syncprod::vloc_synchronized_edges_iterator_t const & it) const;

  /*!
   \brief Disequality check
   \param it : synchronized edges iterator
   \return true if this and it are different, false otherwise
   */
  bool operator!=(tchecker::syncprod::vloc_synchronized_edges_iterator_t const & it) const;

  /*!
  \brief Equality check w.r.t. past-the-end iterator
  \param end : past-the-end iterator
  \return see at_end()
  */
  bool operator==(tchecker::end_iterator_t const & end) const;

  /*!
  \brief Disequality check w.r.t. past-the-end iterator
  \param end : past-the-end iterator
  \return negation of operator==
  */
  bool operator!=(tchecker::end_iterator_t const & end) const;

  /*!
   \brief Iterator over synchronized edges
   */
  using edges_iterator_t =
      tchecker::cartesian_iterator_t<tchecker::range_t<tchecker::system::edges_collection_const_iterator_t>>::values_iterator_t;

  /*!
   \brief Accessor
   \pre not at_end()  (checked by assertion)
   \return current synchronized edge
   \note the returned range is invalidated by operator++
   */
  inline tchecker::range_t<edges_iterator_t> operator*()
  {
    assert(!at_end());
    return (_cartesian_it.operator*());
  }

  /*!
   \brief Increment (next)
   \pre not at_end()  (checked by assertion)
   \post this points to the next element (if any)
   \return this after incrementation
   \note operator++ invalidates all ranges returned by operator*
   */
  tchecker::syncprod::vloc_synchronized_edges_iterator_t & operator++();

protected:
  /*!
  \brief Fast end-of-range check
  \return true if this is past-the-end, false otherwise
  */
  inline bool at_end() const { return (_sync_it == _sync_end); }

  /*!
   \brief Fills cartesian product
   \post either this range is at_end(), or _cartesian_it has been filled with ranges of edges corresponding to
   synchronzation pointed by _sync_it
   */
  void advance_while_empty_cartesian_product();

  tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> _vloc; /*!< Vector of locations */
  /*!< Maps loc id -> edges/events */
  std::shared_ptr<tchecker::system::loc_edges_maps_t const> _loc_edges_maps;
  tchecker::system::synchronizations_t::const_iterator_t _sync_it;  /*!< Iterator on synchronizations */
  tchecker::system::synchronizations_t::const_iterator_t _sync_end; /*!< Past-the-end iterator on synchronizations */
  /*!< Cartesian iterator */
  tchecker::cartesian_iterator_t<tchecker::range_t<tchecker::system::edges_collection_const_iterator_t>> _cartesian_it;
};

/*!
 \brief Accessor to outgoing synchronized edges from a tuple of locations
 \param system : a system
 \param vloc : a tuple of locations
 \return range of outgoing synchronized edges from vloc in system
 */
tchecker::range_t<tchecker::syncprod::vloc_synchronized_edges_iterator_t, tchecker::end_iterator_t>
outgoing_synchronized_edges(tchecker::syncprod::system_t const & system,
                            tchecker::intrusive_shared_ptr_t<tchecker::shared_vloc_t const> const & vloc);

/* Iterator over edges */

/*!
 \class edges_iterator_t
 \brief Iterator over a collection of edges
 \note this class provides an abstraction that let an asynchronous edge and a set synchronized edges appear in the same way
 */
class edges_iterator_t {
public:
  /*!
   \brief Constructor
   \param edge : single asynchronous edge
   \param at_end : past-the-end flag
   \pre edge != nullptr (checked by assertion)
   \post this is an iterator on edge.
   this is past-the-end iff at_end==true
   */
  explicit edges_iterator_t(tchecker::system::edge_const_shared_ptr_t const & edge, bool at_end);

  /*!
   \brief Constructor
   \param it : iterator on synchronized edges
   \post this is an iterator on it
   */
  edges_iterator_t(tchecker::syncprod::vloc_synchronized_edges_iterator_t::edges_iterator_t const & it);

  /*!
   \brief Copy constructor
   */
  edges_iterator_t(tchecker::syncprod::edges_iterator_t const & it) = default;

  /*!
   \brief Move constructor
   */
  edges_iterator_t(tchecker::syncprod::edges_iterator_t &&) = default;

  /*!
   \brief Destructor
   */
  ~edges_iterator_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::syncprod::edges_iterator_t & operator=(tchecker::syncprod::edges_iterator_t const & it) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::syncprod::edges_iterator_t & operator=(tchecker::syncprod::edges_iterator_t && it) = default;

  /*!
   \brief Equality check
   \return true if it and this point to same edge, false otherwise
   */
  bool operator==(tchecker::syncprod::edges_iterator_t const & it) const;

  /*!
   \brief Disequality check
   \return false if it and this point to same edge, true otherwise
   */
  bool operator!=(tchecker::syncprod::edges_iterator_t const & it) const;

  /*!
   \brief Dereference operator
   \pre this is valid
   \return edge pointed by this
   */
  tchecker::system::edge_const_shared_ptr_t operator*();

  /*!
   \brief Increment operator
   \pre this is valid
   \post this points to next edge in vedge
   \return this after incrementation
   */
  tchecker::syncprod::edges_iterator_t & operator++();

private:
  /*!< Single asynchronous edge */
  tchecker::system::edge_const_shared_ptr_t _async_edge;
  /*!< Past-the-end flag for asynchronous edge */
  bool _async_at_end;
  /*!< Iterator over synchronized edges */
  tchecker::syncprod::vloc_synchronized_edges_iterator_t::edges_iterator_t _sync_it;
};

/*!
 \class vloc_edges_iterator_t
 \brief Iterator over synchronous and asynchronous tuples of edges from a tuple of locations
 */
class vloc_edges_iterator_t {
public:
  /*!
   \brief Constructor
   \param sync_it : iterator over synchronous edges
   \param async_it : iterator over asynchronous edges
   */
  vloc_edges_iterator_t(tchecker::syncprod::vloc_synchronized_edges_iterator_t const & sync_it,
                        tchecker::syncprod::vloc_asynchronous_edges_iterator_t const & async_it);

  /*!
   \brief Copy constructor
   */
  vloc_edges_iterator_t(tchecker::syncprod::vloc_edges_iterator_t const &) = default;

  /*!
   \brief Move constructor
   */
  vloc_edges_iterator_t(tchecker::syncprod::vloc_edges_iterator_t &&) = default;

  /*!
   \brief Destructor
   */
  ~vloc_edges_iterator_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::syncprod::vloc_edges_iterator_t & operator=(tchecker::syncprod::vloc_edges_iterator_t const &) = default;

  /*!
   \brief Move assignment operator
   */
  tchecker::syncprod::vloc_edges_iterator_t & operator=(tchecker::syncprod::vloc_edges_iterator_t &&) = default;

  /*!
   \brief Equality check
   \param it : iterator
   \return true if this and it point to the same synchronous and asynchronous iterators, false otherwise
   */
  bool operator==(tchecker::syncprod::vloc_edges_iterator_t const & it) const;

  /*!
   \brief Disequality check
   \param it : iterator
   \return false if this and it point to the same synchronous and asynchronous iterators, true otherwise
   */
  bool operator!=(tchecker::syncprod::vloc_edges_iterator_t const & it) const;

  /*!
  \brief Equality check w.r.t. past-the-end iterator
  \param end : past-the-end iterator
  \return see at_end()
  */
  bool operator==(tchecker::end_iterator_t const & end) const;

  /*!
  \brief Disequality check w.r.t. past-the-end iterator
  \param end : past-the-end iterator
  \return negation of join_iterator_t::operator==
  */
  bool operator!=(tchecker::end_iterator_t const & end) const;

  /*!
   \brief Accessor
   \pre not at_end() (checked by assertion)
   \return Range of iterator over collection of edges pointed to by this
   \note return range is invalidated by operator++
   */
  tchecker::range_t<tchecker::syncprod::edges_iterator_t> operator*();

  /*!
   \brief Move to next
   \pre not at_end() (checked by assertion)
   \post this points to next tuple of edges (if any)
   \return this after increment
   \note invaldates ranges returned by operator*
   */
  tchecker::syncprod::vloc_edges_iterator_t & operator++();

private:
  /*!
   \brief Accessor
   \return true if this is at the end of the range, false otherwise
   */
  bool at_end() const;

  tchecker::syncprod::vloc_synchronized_edges_iterator_t _sync_it;  /*!< Iterator on synchronous edges */
  tchecker::syncprod::vloc_asynchronous_edges_iterator_t _async_it; /*!< Iterator on asynchronous edges */
};

} // end of namespace syncprod

} // end of namespace tchecker

/*!
 \brief Iterator traits for tchecker::syncprod::edges_iterator_t
 */
template <> struct std::iterator_traits<tchecker::syncprod::edges_iterator_t> {
  using difference_type = nullptr_t;
  using value_type = std::shared_ptr<tchecker::system::edge_t>;
  using pointer = value_type *;
  using reference = value_type &;
  using iterator_category = std::forward_iterator_tag;
};

/*!
 \brief Iterator traits for tchecker::syncprod::vloc_edges_iterator_t
 */
template <> struct std::iterator_traits<tchecker::syncprod::vloc_edges_iterator_t> {
  using difference_type = nullptr_t;
  using value_type = tchecker::range_t<tchecker::syncprod::edges_iterator_t>;
  using pointer = value_type *;
  using reference = value_type &;
  using iterator_category = std::forward_iterator_tag;
};

#endif // TCHECKER_SYNCPROD_EDGES_ITERATORS_HH
