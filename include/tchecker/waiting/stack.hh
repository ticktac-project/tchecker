/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_WAITING_STACK_HH
#define TCHECKER_WAITING_STACK_HH

#include <deque>

#include "tchecker/waiting/waiting.hh"

/*!
 \file stack.hh
 \brief Waiting stack (lifo)
 */

namespace tchecker {

namespace waiting {

/*!
 \class stack_t
 \brief Waiting container implementing a stack (lifo)
 \tparam T : type of waiting elements
 */
template <class T> class stack_t final : public tchecker::waiting::waiting_t<T> {
public:
  /*!
   \brief Destructor
  */
  virtual ~stack_t() = default;

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
   \post t has been inserted on top of the stack
   */
  virtual inline void insert(T const & t) { _dq.push_back(t); }

  /*!
   \brief Remove top element
   \pre not empty()
   \post top element has been removed from the stack
   */
  virtual inline void remove_first() { _dq.pop_back(); }

  /*!
   \brief Accessor
   \pre not empty()
   \return top element of the stack
   */
  virtual inline T const & first() { return _dq.back(); }

  /*!
    \brief Remove an element
    \param t : element
    \post all occurrences of t have been removed from the stack
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
 \brief Waiting stack with fast remove
 \tparam T : type of elements, should be a pointer to a type deriving from tchecker::waiting::element_t
*/
template <class T> using fast_remove_stack_t = tchecker::waiting::fast_remove_waiting_t<tchecker::waiting::stack_t<T>>;

} // end of namespace waiting

} // end of namespace tchecker

#endif // TCHECKER_WAITING_STACK_HH