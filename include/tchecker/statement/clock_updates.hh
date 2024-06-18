/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_CLOCK_UPDATES_HH
#define TCHECKER_CLOCK_UPDATES_HH

#include <memory>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/expression/expression.hh"

/*!
 \file clock_updates.hh
 \brief Clock updates
 */

namespace tchecker {

/*!
 \class clock_update_t
 \brief Representation of a clock update `y + c` where `y` is a clock index
 (or tchecker::REFCLOCK_ID), and `c` is an (integer-valued) expression
 */
class clock_update_t {
public:
  /*!
   \brief Constructor
   \param clock_id : right-clock id
   \param value : value
   \post this represents the clock update to `clock_id + value`, and this keeps a pointer
   on `value`
   \throw std::invalid_argument : if value is nullptr
   */
  clock_update_t(tchecker::clock_id_t clock_id, std::shared_ptr<tchecker::expression_t const> const & value);

  /*!
   \brief Constructor
   \param value : value
   \post this represents the clock update to `value`, and this keeps a pointer on `value`
   \throw std::invalid_argument : if value is nullptr
   */
  clock_update_t(std::shared_ptr<tchecker::expression_t const> const & value);

  /*!
   \brief Accessor
   \return clock identifier, which has value tchecker::REFCLOCK_ID is this is an update to
   integer value
   */
  inline tchecker::clock_id_t clock_id() const { return _clock_id; }

  /*!
   \brief Accessor
   \return value
   */
  inline tchecker::expression_t const & value() const { return *_value; }

private:
  friend tchecker::clock_update_t operator+(tchecker::clock_update_t const & u,
                                            std::shared_ptr<tchecker::expression_t const> const & expr);

  tchecker::clock_id_t _clock_id;                       /*!< Clock id */
  std::shared_ptr<tchecker::expression_t const> _value; /*!< Value */
};

/*!
 \brief Sum of clock update and expression
 \param u : clock udpate
 \param expr : expression
 \pre expr is an integer valued expression
 \return clock udpate (clock_id, value + expr) for u=(clock_id, value).
 The returned expression keeps a pointer on expr and on value
 */
tchecker::clock_update_t operator+(tchecker::clock_update_t const & u,
                                   std::shared_ptr<tchecker::expression_t const> const & expr);

/*!
 \class clock_updates_list_t
 \brief List of clock updates
 */
class clock_updates_list_t {
public:
  /*!
   \brief Default constructor
   */
  clock_updates_list_t();

  /*!
   \brief Constructor from a clock update
   \param u : clock update
   \post this is the list {u}
   */
  clock_updates_list_t(tchecker::clock_update_t const & u);

  /*!
   \brief Emptiness check
   \return true if this list is empty, false otherwise
   */
  bool empty() const;

  /*!
   \brief Clear the list
   \post this is empty
   */
  void clear();

  /*!
   \brief Push a clock update at the end of this list
   \param u : clock update
   \post u has been added at the end of this list
   */
  void push_back(tchecker::clock_update_t const & u);

  /*!
   \brief Absorbing push back
   \param u : clock update
   \post u has been added to this if this is not empty, otherwise this
   is left empty
   */
  void absorbing_push_back(tchecker::clock_update_t const & u);

  /*!
   \brief Set clock update list to a single clock update
   \param u : clock update
   \post this has been set to {u}
   */
  void set(tchecker::clock_update_t const & u);

  /*!
   \brief Absorbing merge
   \param l : clock updates list
   \post this is empty if either this was empty before or l is empty,
   otherwise l has been concatenated at the end of this
   */
  void absorbing_merge(tchecker::clock_updates_list_t const & l);

  /*!
   \brief Const iterator
   */
  using const_iterator_t = std::vector<tchecker::clock_update_t>::const_iterator;

  /*!
   \brief Accessor
   \return iterator on first clock update if this is not empty, past-the-end iterator otherwise
   */
  const_iterator_t begin() const;

  /*!
   \brief Accessor
   \return past-the-end iterator
   */
  const_iterator_t end() const;

private:
  friend tchecker::clock_updates_list_t operator+(tchecker::clock_updates_list_t const & l,
                                                  std::shared_ptr<tchecker::expression_t const> const & expr);

  std::vector<tchecker::clock_update_t> _list; /*!< List of clock updates */
};

/*!
 \brief Sum of a clock updates list and an expression
 \param l : clock update list
 \param expr : expression
 \pre expr is integer valued
 \return list of clock updates obtained by summing expr with each clock update in this list. The
 clock updates in the returned list keep pointers on expr
 */
tchecker::clock_updates_list_t operator+(tchecker::clock_updates_list_t const & l,
                                         std::shared_ptr<tchecker::expression_t const> const & expr);

/*!
 \class clock_updates_map_t
 \brief Maps clocks to list of clock updates
 */
class clock_updates_map_t {
public:
  /*!
   \brief Constructor
   \param clock_nb : number of clocks
   \post this is the identity map: each clock x in [0..clock_nb) is mapped to the list {x}
   */
  clock_updates_map_t(std::size_t clock_nb);

  /*!
   \brief Accessor
   \param x : clock identifier
   \return clock updates list for x
   \throw std::invalid_argument : if x is not a clock identifier in this map
   */
  tchecker::clock_updates_list_t const & operator[](tchecker::clock_id_t x) const;

  /*!
   \brief Accessor
   \param x : clock identifier
   \return clock updates list for x
   \throw std::invalid_argument : if x is not a clock identifier in this map
   */
  tchecker::clock_updates_list_t & operator[](tchecker::clock_id_t x);

  /*!
   \brief Absorbing merge
   \param m : clock updated map
   \post m has been merged into this, where the clock updates list of x is empty
   if it was empty in m of this, and the concatenation of the two lists otherwise
   \post m is empty
   \throw std::invalid_argument : if m and this no not have the same number of clocks
   */
  void absorbing_merge(tchecker::clock_updates_map_t & m);

private:
  std::vector<tchecker::clock_updates_list_t> _map; /*!< Map : clock id -> clock updates list */
};

} // namespace tchecker

#endif // TCHECKER_CLOCK_UPDATES_HH