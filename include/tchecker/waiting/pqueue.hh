/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_WAITING_PRIORITY_QUEUE_HH
#define TCHECKER_WAITING_PRIORITY_QUEUE_HH

#include <set>

#include "tchecker/waiting/waiting.hh"

/*!
 \file queue.hh
 \brief Waiting queue (fifo)
 */

namespace tchecker {

namespace waiting {

/*!
 \class queue_t
 \brief Waiting container implementing a priority queue
 \tparam T : type of waiting elements
 */
template <class T, class Compare = std::less<T>> class priority_queue_t : public tchecker::waiting::waiting_t<T> {
public:
  /*!
   \brief Destructor
  */
  virtual ~priority_queue_t() = default;

  /*!
   \brief Accessor
   \return true if the container is empty, false otherwise
   */
  virtual inline bool empty() { return _s.empty(); }

  /*!
   \brief Clear the container
   \post this container is empty
   */
  virtual inline void clear() { _s.clear(); }

  /*!
   \brief Insert
   \param t : element
   \post t has been inserted at the end of the queue
   */
  virtual inline void insert(T const & t) { _s.insert(t); }

  /*!
   \brief Remove first element
   \pre not empty()
   \post first element has been removed from the queue
   */
  virtual inline void remove_first() { assert(! empty()); _s.erase(_s.begin()); }

  /*!
   \brief Accessor
   \pre not empty()
   \return first element in the queue
   */
  virtual inline T const & first() { return *(_s.begin()); }

  /*!
    \brief Remove an element
    \param t : element
    \post all occurrences of t have been removed from the queue
    \note complexity is linear in the size of the container
  */
  virtual void remove(T const & t)
  {
    for (auto it = _s.begin(); it != _s.end();) {
      if (*it == t)
        it = _s.erase(it);
      else
        ++it;
    }
  }

private:
  std::multiset<T,Compare> _s; /*!< Container */
};

/*!
 \brief Waiting queue with fast remove
 \tparam T : type of elements, should be a pointer to a type deriving from tchecker::waiting::element_t
*/
template <class T, class Compare=std::less<T>> using fast_remove_priority_queue_t = tchecker::waiting::fast_remove_waiting_t<tchecker::waiting::priority_queue_t<T,Compare>>;

} // end of namespace waiting

} // end of namespace tchecker

#endif // TCHECKER_WAITING_QUEUE_HH