/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/statement/clock_updates.hh"

namespace tchecker {

/* clock_update_t */

clock_update_t::clock_update_t(tchecker::clock_id_t clock_id, std::shared_ptr<tchecker::expression_t const> const & value)
    : _clock_id(clock_id), _value(value)
{
  if (_value == nullptr)
    throw std::invalid_argument("Unexpected nullptr value");
}

clock_update_t::clock_update_t(std::shared_ptr<tchecker::expression_t const> const & value)
    : clock_update_t(tchecker::REFCLOCK_ID, value)
{
}

tchecker::clock_update_t operator+(tchecker::clock_update_t const & u,
                                   std::shared_ptr<tchecker::expression_t const> const & expr)
{
  return tchecker::clock_update_t{u._clock_id,
                                  std::make_shared<tchecker::binary_expression_t>(tchecker::EXPR_OP_PLUS, u._value, expr)};
}

/* clock_updates_list_t */

clock_updates_list_t::clock_updates_list_t() = default;

clock_updates_list_t::clock_updates_list_t(tchecker::clock_update_t const & u) { push_back(u); }

bool clock_updates_list_t::empty() const { return _list.empty(); }

void clock_updates_list_t::clear() { _list.clear(); }

void clock_updates_list_t::push_back(tchecker::clock_update_t const & u) { _list.emplace_back(u); }

void clock_updates_list_t::absorbing_push_back(tchecker::clock_update_t const & u)
{
  if (!empty())
    push_back(u);
}

void clock_updates_list_t::set(tchecker::clock_update_t const & u)
{
  clear();
  push_back(u);
}

void clock_updates_list_t::absorbing_merge(tchecker::clock_updates_list_t const & l)
{
  if (empty() || l.empty())
    clear();
  else
    _list.insert(_list.end(), l.begin(), l.end());
}

tchecker::clock_updates_list_t::const_iterator_t clock_updates_list_t::begin() const { return _list.begin(); }

tchecker::clock_updates_list_t::const_iterator_t clock_updates_list_t::end() const { return _list.cend(); }

tchecker::clock_updates_list_t operator+(tchecker::clock_updates_list_t const & l,
                                         std::shared_ptr<tchecker::expression_t const> const & expr)
{
  tchecker::clock_updates_list_t lsum;
  for (tchecker::clock_update_t const & u : l._list)
    lsum.push_back(u + expr);
  return lsum;
}

/* clock_updates_map_t */

clock_updates_map_t::clock_updates_map_t(std::size_t clock_nb) : _map(clock_nb)
{
  for (tchecker::clock_id_t x = 0; x < clock_nb; ++x)
    _map[x].set(tchecker::clock_update_t{x, std::make_shared<tchecker::int_expression_t>(0)});
}

tchecker::clock_updates_list_t const & clock_updates_map_t::operator[](tchecker::clock_id_t x) const
{
  if (x >= _map.size())
    throw std::invalid_argument("out-of-range clock identifier");
  return _map[x];
}

tchecker::clock_updates_list_t & clock_updates_map_t::operator[](tchecker::clock_id_t x)
{
  if (x >= _map.size())
    throw std::invalid_argument("out-of-range clock identifier");
  return _map[x];
}

void clock_updates_map_t::absorbing_merge(tchecker::clock_updates_map_t & m)
{
  for (tchecker::clock_id_t x = 0; x < _map.size(); ++x)
    _map[x].absorbing_merge(m._map[x]);
}

} // namespace tchecker