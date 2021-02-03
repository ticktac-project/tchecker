/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_ATTRIBUTE_HH
#define TCHECKER_SYSTEM_ATTRIBUTE_HH

#include <iostream>
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
 \class attributes_t
 \brief Attributes map key -> values for system components
 */
class attributes_t {
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
   \post the pair (key, value) has been added to the map
   */
  void add_attribute(std::string const & key, std::string const & value);

  /*!
   \brief Merge attributes
   \param attr : attributes
   \post all attributes from attr have been added to the map
   */
  void add_attributes(tchecker::system::attributes_t const & attr);

  /*!
   \brief Type of attribute iterator
   \note attributes are pairs (key, value)
   */
  using const_iterator_t = std::unordered_multimap<std::string, std::string>::const_iterator;

  /*!
   \brief Accessor
   \param key : attribute key
   \pre key exists in this map
   \return range (begin,end) of values associated to key
   \note the returned range is empty (i.e. begin=end) if there is no attribute key in this map
   */
  tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> values(std::string const & key) const;

  /*!
   \brief Accessor
   \return range of attributes
   \note the returned range is empty (i.e. begin=end) is there is no attribute in this map
   */
  tchecker::range_t<tchecker::system::attributes_t::const_iterator_t> attributes() const;

private:
  std::unordered_multimap<std::string, std::string> _attr; /*!< Map : key -> values of attributes */
};

} // end of namespace system

} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_ATTRIBUTE_HH
