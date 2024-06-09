/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_EVENT_HH
#define TCHECKER_SYSTEM_EVENT_HH

#include <string>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/system/attribute.hh"
#include "tchecker/utils/index.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file event.hh
 \brief Events in systems
 */

namespace tchecker {

namespace system {

/*!
 \class events_t
 \brief Collection of events
 */
class events_t {
public:
  /*!
   \brief Add an event
   \param name : event name
   \param attributes : event attributes
   \pre event name is not declared yet
   \post event name has been added with attributes attr
   \throw std::invalid_argument : if event name is already declared
   */
  void add_event(std::string const & name,
                 tchecker::system::attributes_t const & attributes = tchecker::system::attributes_t());

  /*!
   \brief Accessor
   \return number of events
   \note all integers in 0..events_count()-1 are valid event identifiers
   */
  inline std::size_t events_count() const { return _events_attributes.size(); }

  /*!
   \brief Type of range of events identifiers
   */
  using events_identifiers_range_t = tchecker::integer_range_t<tchecker::event_id_t>;

  /*!
  \brief Accesor
  \return range of clock identifiers 0..events_count()-1
  */
  events_identifiers_range_t events_identifiers() const;

  /*!
   \brief Accessor
   \param name : event name
   \return identifier of event name
   \throw std::invalid_argument : if name is not an event
   */
  inline tchecker::event_id_t event_id(std::string const & name) const { return _events_index.value(name); }

  /*!
   \brief Accessor
   \param id : event identifier
   \return name of event id
   \throw std::invalid_argument : if id is not an event
   */
  inline std::string const & event_name(tchecker::event_id_t id) const { return _events_index.key(id); }

  /*!
   \brief Accessor
   \param id : event identifier
   \return attributes of event id
   \throw std::invalid_argument : if id is not an event identifier
   */
  tchecker::system::attributes_t const & event_attributes(tchecker::event_id_t id) const;

  /*!
   \brief Checks validity of event identifier
   \param id : event identifier
   \return true if id is a declared event identifier, false othwerise
   */
  bool is_event(tchecker::event_id_t id) const;

  /*!
   \brief Check validity of event name
   \param event : event name
   \return true if name is a declared event, false otherwise
   */
  bool is_event(std::string const & event) const;

private:
  std::vector<tchecker::system::attributes_t> _events_attributes;     /*!< Events attributes */
  tchecker::index_t<std::string, tchecker::event_id_t> _events_index; /*!< Map event name <-> event identifier */
};

} // end of namespace system

} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_EVENT_HH
