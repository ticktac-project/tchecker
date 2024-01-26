/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_WAITING_HH
#define TCHECKER_WAITING_HH

#include <cassert>

/*!
 \file waiting.hh
 \brief Waiting containers
 */

namespace tchecker {

namespace waiting {

/*!
 \class waiting_t
 \brief Interface to waiting container
 \tparam T : type of waiting elements, should derive from
 tchecker::waiting::element_t
 */
template <class T> class waiting_t {
public:
  /*!
  \brief Type of stored elements
  */
  using element_t = T;

  /*!
   \brief Destructor
   */
  virtual ~waiting_t() = default;

  /*!
   \brief Accessor
   \return true if the container is empty, false otherwise
   \note this method is not marked const to allow implementations that update
   the container (see tchecker::waiting::fast_remove_waiting_t)
   */
  virtual bool empty() = 0;

  /*!
   \brief Clear the container
   \post this container is empty
   */
  virtual void clear() = 0;

  /*!
   \brief Insert
   \param t : element
   \post t has been inserted in this container
   */
  virtual void insert(T const & t) = 0;

  /*!
   \brief Remove first element
   \pre not empty()
   \post the first element has been removed from this container
   */
  virtual void remove_first() = 0;

  /*!
   \brief Accessor
   \pre not empty()
   \return first element of this container
   \note the first element is not removed from the container
   \note this method is not marked const to allow implementations that update
   the container (see tchecker::waiting::fast_remove_waiting_t)
   */
  virtual T const & first() = 0;

  /*!
    \brief Remove an element
    \param t : element
    \post t is not waiting anymore
    \note t may have been removed from this container or it may still be stored
    by the container. Implementation should ensure that t will never appear as
    the first element of the container, and that successive calls to
    remove_first eventually remove t from the container
  */
  virtual void remove(T const & t) = 0;
};

// forward declaration
template <class W> class fast_remove_waiting_t;

/*!
\brief Status of elements in a fast-remove waiting container
*/
enum status_t {
  WAITING,     /*!< Element is waiting */
  NOT_WAITING, /*!< Element is not waiting */
};

/*!
 \class element_t
 \brief Elements of fast remove waiting containers
 */
class element_t {
public:
  /*!
    \brief Constructor
  */
  element_t();

protected:
  template <class W> friend class fast_remove_waiting_t;

  mutable enum tchecker::waiting::status_t _status; /*!< Waiting status */
};

/*!
 \class fast_remove_waiting_t
 \brief Waiting container that simulates fast removing of elements anywhere in the
 container
 \tparam W : type of waiting container, should implement
 tchecker::waiting::waiting_t. The type of elements W::element_t should be a
 pointer to a type deriving from tchecker::waiting::element_t
 */
template <class W> class fast_remove_waiting_t : public tchecker::waiting::waiting_t<typename W::element_t> {
public:
  /*!
  \brief Constructor
  \param wargs : parameters to a constructor of class W
  */
  template <class... WARGS> fast_remove_waiting_t(WARGS &&... wargs) : _w(wargs...) {}

  /*!
  \brief Copy constructor
  */
  fast_remove_waiting_t(tchecker::waiting::fast_remove_waiting_t<W> const &) = default;

  /*!
  \brief Move constructor
  */
  fast_remove_waiting_t(tchecker::waiting::fast_remove_waiting_t<W> &&) = default;

  /*!
  \brief Destructor
  */
  virtual ~fast_remove_waiting_t() = default;

  /*!
  \brief Assignment operator
  */
  tchecker::waiting::fast_remove_waiting_t<W> & operator=(tchecker::waiting::fast_remove_waiting_t<W> const &) = default;

  /*!
  \brief Move-assignment operator
  */
  tchecker::waiting::fast_remove_waiting_t<W> & operator=(tchecker::waiting::fast_remove_waiting_t<W> &&) = default;

  /*!
   \brief Accessor
   \return true if the container is empty, false otherwise
   */
  virtual bool empty()
  {
    remove_non_waiting_first();
    return _w.empty();
  }

  /*!
   \brief Clear the container
   \post this container is empty
  */
  virtual void clear() { _w.clear(); }

  /*!
   \brief Insert
   \param t : element
   \post t has been inserted in this container
   */
  virtual void insert(typename W::element_t const & t)
  {
    _w.insert(t);
    t->_status = tchecker::waiting::WAITING;
  }

  /*!
   \brief Remove first element
   \pre not empty()
   \post the first element has been removed from this container
   */
  virtual void remove_first()
  {
    remove_non_waiting_first();
    assert(!_w.empty());
    _w.first()->_status = tchecker::waiting::NOT_WAITING;
    _w.remove_first();
  }

  /*!
   \brief Accessor
   \pre not empty()
   \return first element of this container
   */
  virtual typename W::element_t const & first()
  {
    remove_non_waiting_first();
    assert(!_w.empty());
    assert(_w.first()->_status == tchecker::waiting::WAITING);
    return _w.first();
  }

  /*!
   \brief Remove an element
   \param t : element
   \post t is not waiting anymore
   \note t is marked but it may not be removes from the container. t will then
   be transparently removed when first in the container.
  */
  virtual void remove(typename W::element_t const & t)
  {
    t->_status = tchecker::waiting::NOT_WAITING;
    remove_non_waiting_first();
  }

private:
  /*!
  \brief Removes all first non-waiting elements from the container until a
  waiting element is found or the container is empty
  \post if the container only contained non-waiting elements, then the container
  has been emptied. Otherwise, all non-waiting first elements of the container
  have been removed, and the first element is a waiting element. The order
  between elements still in the container has been preserved.
  */
  void remove_non_waiting_first()
  {
    while (!_w.empty()) {
      typename W::element_t const & t = _w.first();
      if (t->_status == tchecker::waiting::WAITING)
        break;
      _w.remove_first();
    }
  }

  W _w; /*!< Waiting container */
};

} // end of namespace waiting

} // end of namespace tchecker

#endif // TCHECKER_WAITING_HH
