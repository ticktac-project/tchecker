/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_WAITING_QUEUE_HH
#define TCHECKER_WAITING_QUEUE_HH

#include <deque>

#include "tchecker/waiting/waiting.hh"

/*!
 \file queue.hh
 \brief Waiting queue (fifo)
 */

namespace tchecker {

namespace waiting {

/*!
 \class queue_t
 \brief Waiting container implementing a queue (fifo)
 \tparam T : type of waiting elements
 */
template <class T> class queue_t : public tchecker::waiting::waiting_t<T> {
public:
  /*!
   \brief Destructor
  */
  virtual ~queue_t() = default;

  /*!
   \brief Accessor
   \return true if the container is empty, false otherwise
   */
  virtual inline bool empty() { return _dq.empty(); }

  /*!
   \brief Clear the container
   \post this container is empty
   */
  virtual inline void clear() { _dq.clear(); }

  /*!
   \brief Insert
   \param t : element
   \post t has been inserted at the end of the queue
   */
  virtual inline void insert(T const & t) { _dq.push_back(t); }

  /*!
   \brief Remove first element
   \pre not empty()
   \post first element has been removed from the queue
   */
  virtual inline void remove_first() { _dq.pop_front(); }

  /*!
   \brief Accessor
   \pre not empty()
   \return first element in the queue
   */
  virtual inline T const & first() { return _dq.front(); }

  /*!
    \brief Remove an element
    \param t : element
    \post all occurrences of t have been removed from the queue
    \note complexity is linear in the size of the container
  */
  virtual void remove(T const & t)
  {
    for (auto it = _dq.begin(); it != _dq.end();) {
      if (*it == t)
        it = _dq.erase(it);
      else
        ++it;
    }
  }

private:
  std::deque<T> _dq; /*!< Container */
};

/*!
 \brief Waiting queue with fast remove
 \tparam T : type of elements, should be a pointer to a type deriving from tchecker::waiting::element_t
*/
template <class T> using fast_remove_queue_t = tchecker::waiting::fast_remove_waiting_t<tchecker::waiting::queue_t<T>>;

} // end of namespace waiting

} // end of namespace tchecker

#endif // TCHECKER_WAITING_QUEUE_HH