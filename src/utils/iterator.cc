/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <limits>

#include "tchecker/utils/iterator.hh"

namespace tchecker {

/* end_iterator_t */

bool operator==(tchecker::end_iterator_t const & it1, tchecker::end_iterator_t const & it2) { return (&it1 == &it2); }

bool operator!=(tchecker::end_iterator_t const & it1, tchecker::end_iterator_t const & it2) { return !(it1 == it2); }

/* integer_iterator_t */

integer_iterator_t::integer_iterator_t(tchecker::integer_t value) : _current(value) {}

tchecker::integer_iterator_t & integer_iterator_t::operator=(tchecker::integer_t const & value)
{
  _current = value;
  return *this;
}

bool integer_iterator_t::operator==(tchecker::integer_iterator_t const & it) const { return _current == it._current; }

bool integer_iterator_t::operator==(tchecker::integer_t value) const { return _current == value; }

bool integer_iterator_t::operator!=(tchecker::integer_iterator_t const & it) const { return !(*this == it); }

bool integer_iterator_t::operator!=(tchecker::integer_t value) const { return !(*this == value); }

tchecker::integer_t integer_iterator_t::operator*() const { return _current; }

tchecker::integer_iterator_t & integer_iterator_t::operator++()
{
  ++_current;
  return *this;
}

tchecker::integer_iterator_t & integer_iterator_t::operator--()
{
  --_current;
  return *this;
}

} // namespace tchecker