/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_WAITING_FACTORY_HH
#define TCHECKER_WAITING_FACTORY_HH

#include <stdexcept>
#include <functional>

#include "tchecker/waiting/pqueue.hh"
#include "tchecker/waiting/queue.hh"
#include "tchecker/waiting/stack.hh"
#include "tchecker/waiting/waiting.hh"

namespace tchecker {

namespace waiting {

/*!
 \brief Type of waiting policies
*/
enum policy_t {
  QUEUE = 0,          /*!< Queue: fifo polocy */
  FAST_REMOVE_QUEUE,  /*!< Queue: fifo policy, with fast removal of elements */
  STACK,              /*!< Stack: lifo policy */
  FAST_REMOVE_STACK,  /*!< Stack: lifo policy, with fast removal of elements */
  PQUEUE,             /*!< Priority Queue: heap policy */
  FAST_REMOVE_PQUEUE, /*!< Priority Queue: heap policy, with fast removal of elements */
};

/*!
 \brief Factory of waiting containers
 \tparam T : type of waiting elements
 \param policy : waiting policy
 \return a newly allocated empty waiting container of elements of type T
 that implements policy
 */
template <class T> tchecker::waiting::waiting_t<T> * factory(enum policy_t policy)
{
  switch (policy) {
  case tchecker::waiting::QUEUE:
    return new tchecker::waiting::queue_t<T>{};
  case tchecker::waiting::FAST_REMOVE_QUEUE:
    return new tchecker::waiting::fast_remove_queue_t<T>{};
  case tchecker::waiting::STACK:
    return new tchecker::waiting::stack_t<T>{};
  case tchecker::waiting::FAST_REMOVE_STACK:
    return new tchecker::waiting::fast_remove_stack_t<T>{};
  case tchecker::waiting::PQUEUE:
  case tchecker::waiting::FAST_REMOVE_PQUEUE:
    throw std::invalid_argument("priority queue not supported");
  default:
    throw std::invalid_argument("Unknown waiting policy");
  }
}

template <class T, class Compare> tchecker::waiting::waiting_t<T> * factory(enum policy_t policy)
{
  switch (policy) {
  case tchecker::waiting::PQUEUE:
    return new tchecker::waiting::priority_queue_t<T,Compare>{};
  case tchecker::waiting::FAST_REMOVE_PQUEUE:
    return new tchecker::waiting::fast_remove_priority_queue_t<T,Compare>{};
  default:
    return factory<T>(policy);
  }
}

} // end of namespace waiting

} // end of namespace tchecker

#endif // TCHECKER_WAITING_FACTORY_HH
