/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_CLOCK_HH
#define TCHECKER_SYSTEM_CLOCK_HH

#include <stdexcept>
#include <string>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/system/attribute.hh"
#include "tchecker/variables/clocks.hh"

/*!
 \file clock.hh
 \brief Clocks in systems
 */

namespace tchecker {

namespace system {

/*!
 \class clocks_t
 \brief Collection of clocks
 */
class clocks_t {
public:
  /*!
   \brief Add a clock
   \param name : clock name
   \param size : clock size (array of clocks)
   \param attr : clock attributes
   \pre name is not a declared clock
   \post clock name has been added with size and attr
   \throw std::invalid_argument : if name is a declared clock
   */
  void add_clock(std::string const & name, tchecker::clock_id_t size = 1,
                 tchecker::system::attributes_t const & attr = tchecker::system::attributes_t());

  /*!
   \brief Accessor
   \param kind : kind of variable declaration
   \return number of declared clock variables if kind = tchecker::VK_DECLARED,
   number of flattened clock variables if kind = tchecker::VK_FLATTENED
   */
  inline tchecker::clock_id_t clocks_count(enum tchecker::variable_kind_t kind) const { return _clock_variables.size(kind); }

  /*!
   \brief Accessor
   \param name : clock name
   \return identifier of clock name
   \throw std::invalid_argument : if name is not a clock
   */
  inline tchecker::clock_id_t clock_id(std::string const & name) const { return _clock_variables.id(name); }

  /*!
   \brief Accessor
   \param id : clock identifier
   \return name of clock id
   \throw std::invalid_argument : if id is not a clock
   */
  inline std::string const & clock_name(tchecker::clock_id_t id) const { return _clock_variables.name(id); }

  /*!
   \brief Accessor
   \param id : clock identifier
   \return attributes of clock id
   \throw std::invalid_argument : if id is not a clock
   */
  tchecker::system::attributes_t const & clock_attributes(tchecker::clock_id_t id) const;

  /*!
   \brief Accessor
   \param name : clock name
   \return true if name is a declared clock variable, false otherwise
   */
  bool is_clock(std::string const & name) const;

  /*!
   \brief Accessor
   \return clock variables
   */
  inline tchecker::clock_variables_t const & clock_variables() const { return _clock_variables; }

private:
  tchecker::clock_variables_t _clock_variables;                      /*!< Clock variables */
  std::vector<tchecker::system::attributes_t> _clock_variables_attr; /*!< Clocks attributes */
};

} // end of namespace system

} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_CLOCK_HH
