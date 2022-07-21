/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include <limits>

#include "tchecker/utils/hashtable.hh"

namespace tchecker {

tchecker::collision_table_position_t const COLLISION_TABLE_NOT_STORED =
    std::numeric_limits<tchecker::collision_table_position_t>::max();

collision_table_object_t::collision_table_object_t()
    : _position_in_table(tchecker::COLLISION_TABLE_NOT_STORED),
      _position_in_collision_list(tchecker::COLLISION_TABLE_NOT_STORED)
{
}

collision_table_object_t::collision_table_object_t(tchecker::collision_table_object_t const & o)
    : _position_in_table(o._position_in_table), _position_in_collision_list(o._position_in_collision_list)
{
  if (o.is_stored())
    throw std::invalid_argument("Cannot copy an object that is stored in a hashtable");
}

tchecker::collision_table_object_t & collision_table_object_t::operator=(tchecker::collision_table_object_t & o)
{
  if (is_stored())
    throw std::runtime_error("Cannot assign an object that is stored in a hashtable");
  if (o.is_stored())
    throw std::invalid_argument("Cannot assign from an object that is stored in a hashtable");
  _position_in_table = o._position_in_table;
  _position_in_collision_list = o._position_in_collision_list;
  return *this;
}

tchecker::collision_table_object_t & collision_table_object_t::operator=(tchecker::collision_table_object_t && o)
{
  if (is_stored())
    throw std::runtime_error("Cannot assign an object that is stored in a hashtable");
  _position_in_table = o._position_in_table;
  _position_in_collision_list = o._position_in_collision_list;
  o._position_in_table = tchecker::COLLISION_TABLE_NOT_STORED;
  o._position_in_collision_list = tchecker::COLLISION_TABLE_NOT_STORED;
  return *this;
}

void collision_table_object_t::set_position(tchecker::collision_table_position_t position_in_table,
                                            tchecker::collision_table_position_t position_in_container)
{
  if (is_stored())
    throw std::runtime_error("Cannot set position of a stored object");
  if (position_in_table == tchecker::COLLISION_TABLE_NOT_STORED ||
      position_in_container == tchecker::COLLISION_TABLE_NOT_STORED)
    throw std::invalid_argument("Invalid position(s)");
  _position_in_table = position_in_table;
  _position_in_collision_list = position_in_container;
}

void collision_table_object_t::clear_position()
{
  _position_in_table = tchecker::COLLISION_TABLE_NOT_STORED;
  _position_in_collision_list = tchecker::COLLISION_TABLE_NOT_STORED;
}

bool collision_table_object_t::is_stored() const
{
  return (_position_in_table != tchecker::COLLISION_TABLE_NOT_STORED &&
          _position_in_collision_list != tchecker::COLLISION_TABLE_NOT_STORED);
}

} // end of namespace tchecker
