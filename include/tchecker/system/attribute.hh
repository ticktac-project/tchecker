/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_ATTRIBUTE_HH
#define TCHECKER_SYSTEM_ATTRIBUTE_HH

#include <array>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>

#include "tchecker/parsing/declaration.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file attribute.hh
 \brief Attributes for system components
 */

namespace tchecker {

namespace system {

/*!
 \class attr_parsing_position_t
 \brief Parsing position of attributes
*/
class attr_parsing_position_t : public tchecker::parsing::attr_parsing_position_t {
public:
  /*!
   \brief Default constructor
  */
  attr_parsing_position_t() = default;

  /*!
   \brief Constructor
   \param parsing_position : parsing position of attribute
  */
  attr_parsing_position_t(tchecker::parsing::attr_parsing_position_t const & parsing_position);

  using tchecker::parsing::attr_parsing_position_t::attr_parsing_position_t;
};

/*!
 \class attr_t
 \brief Representation of an attribute
 \note We provide parsing position to allow precise error reporting when attributes are parsed
*/
class attr_t : public tchecker::parsing::attr_t {
public:
  /*!
   \brief Constructor
   \param attr : attribute
  */
  attr_t(tchecker::parsing::attr_t const & attr);

  using tchecker::parsing::attr_t::attr_t;
};

/*!
 \class attributes_t
 \brief Collection of attributes as a map key -> attribute to allow iteration
 on keys
 */
class attributes_t {
  /*!
   \brief Type of map key -> attribute
  */
  using map_t = std::unordered_multimap<std::string, tchecker::system::attr_t>;

public:
  /*!
   \brief Default contructor
   */
  attributes_t() = default;

  /*!
   \brief Constructor
   \param attributes : parsed attributes
   \post this is a copy of attributes
   */
  attributes_t(tchecker::parsing::attributes_t const &);

  /*!
   \brief Copy constructor
   */
  attributes_t(tchecker::system::attributes_t const &) = default;

  /*!
   \brief Move constructor
   */
  attributes_t(tchecker::system::attributes_t &&) = default;

  /*!
   \brief Destructor
   */
  ~attributes_t() = default;

  /*!
   \brief Assignment operator
   */
  tchecker::system::attributes_t & operator=(tchecker::system::attributes_t const &) = default;

  /*!
   \brief Move-assignment operator
   */
  tchecker::system::attributes_t & operator=(tchecker::system::attributes_t &&) = default;

  /*!
   \brief Add attribute
   \param key : attribute key
   \param value : attribute value
   \param parsing_position : parsing position of the attribute
   \post attribute (key, value, parsing_position) has been added to the map
   */
  void add_attribute(std::string const & key, std::string const & value,
                     tchecker::system::attr_parsing_position_t const & parsing_position);

  /*!
   \brief Merge attributes
   \param attr : attributes
   \post all attributes from attr have been added to the map
   */
  void add_attributes(tchecker::system::attributes_t const & attr);

  /*!
   \class const_iterator_t
   \brief Const iterator on attributes, that dereferences to tchecker::system::attr_t
   */
  class const_iterator_t : public map_t::const_iterator {
  public:
    /*!
     \brief Constructor
     \param it : map iterator
     \post this iterator point to it
     */
    const_iterator_t(map_t::const_iterator const & it);

    /*!
     \brief Accessor
     \return attribute at pointed map entry
     */
    inline tchecker::system::attr_t const & operator*() const { return (*this)->second; }
  };

  /*!
   \brief Accessor
   \param key : attribute key
   \pre key exists in this map
   \return range (begin,end) of values associated to key
   \note the returned range is empty (i.e. begin=end) if there is no attribute key in this map
   */
  tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> range(std::string const & key) const;

  /*!
   \brief Accessor
   \return range of attributes
   \note the returned range is empty (i.e. begin=end) is there is no attribute in this map
   */
  tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> range() const;

private:
  map_t _map; /*!< Map : key -> attribute */
};

/*!
 \brief Attributes categories
*/
enum attributes_category_t {
  ATTR_CLOCK,    /*!< Attributes on clock */
  ATTR_EDGE,     /*!< Attributes on egde */
  ATTR_EVENT,    /*!< Attributes on event */
  ATTR_INTVAR,   /*!< Attributes on integer variable */
  ATTR_LOCATION, /*!< Attributes on location */
  ATTR_PROCESS,  /*!< Attributes on process */
  ATTR_SYNC,     /*!< Attributes on synchronisation */
  ATTR_SYSTEM,   /*!< Attributes on system */
  ATTR_END       /*!< End mark */
};

/*!
 \brief Map from attributes categories to set of attribute keys for that category
 */
using attribute_keys_map_t = std::array<std::set<std::string>, tchecker::system::ATTR_END>;

} // end of namespace system

} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_ATTRIBUTE_HH
