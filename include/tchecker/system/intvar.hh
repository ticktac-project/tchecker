/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_INTVAR_HH
#define TCHECKER_SYSTEM_INTVAR_HH

#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/system/attribute.hh"
#include "tchecker/utils/iterator.hh"
#include "tchecker/variables/intvars.hh"

/*!
 \file intvar.hh
 \brief Bounded integer variables in systems
 */

namespace tchecker {

namespace system {

/*!
 \class intvars_t
 \brief Collection of bounded integer variables
 */
class intvars_t {
public:
  /*!
   \brief Add an integer variable
   \param name : variable name
   \param size : variable size (array of integer variables)
   \param min : minimal value
   \param max : maximal value
   \param initial : initial calue
   \param attributes :  attributes
   \pre name is not a declared integer variable
   \post variable name with minimal value min, maximal value max, initial value init and attributes attr has been added
   \throw std::invalid_argument : if name is a declared integer variable
   */
  void add_intvar(std::string const & name, tchecker::intvar_id_t size = 1,
                  tchecker::integer_t min = std::numeric_limits<tchecker::integer_t>::min(),
                  tchecker::integer_t max = std::numeric_limits<tchecker::integer_t>::max(),
                  tchecker::integer_t initial = std::numeric_limits<tchecker::integer_t>::min(),
                  tchecker::system::attributes_t const & attributes = tchecker::system::attributes_t());

  /*!
   \brief Accessor
   \param kind : kind of declared variable
   \return number of declared bounded integer variables if kind = tchecker::VK_DECLARED,
   number of flattened bounded integer variables if kind = tchecker::VK_FLATTENED
   \note all integers in 0..intvars_count(tchecker::VK_FLATTENED)-1 are valid flattened integer variable indetifiers
   \note 0..intvars_count(tchecker::VK_DECLARED)-1 is NOT the range of declared integer variable identifiers: use
   intvars_identifiers(tchecker::VK_DECLARED) for that purpose
   */
  inline std::size_t intvars_count(enum tchecker::variable_kind_t kind) const { return _integer_variables.size(kind); }

  /*!
   \brief Accessor
   \param kind : kind of declared variable
   \return the range of declared bounded integer variables identifiers if kind = tchecker::VK_DECLARED,
   the range of flattened bounded integer variables identifiers if kind = tchecker::VK_FLATTENED
   \note if kind = tchecker::VK_FLATTENED, then the returned range is 0..intvars_count(tchecker::VK_FLATTENED)-1
   */
  inline tchecker::integer_variables_t::identifiers_range_t intvars_identifiers(enum tchecker::variable_kind_t kind) const
  {
    return _integer_variables.identifiers(kind);
  }

  /*!
   \brief Accessor
   \param name : variable name
   \return identifier of variable name
   \throw std::invalid_argument : if name is not am integer variable
   */
  inline tchecker::intvar_id_t intvar_id(std::string const & name) const { return _integer_variables.id(name); }

  /*!
   \brief Accessor
   \param id : variable identifier
   \return name of integer variable id
   \throw std::invalid_argument : if id is not an integer variable
   */
  inline std::string const & intvar_name(tchecker::intvar_id_t id) const { return _integer_variables.name(id); }

  /*!
   \brief Accessor
   \param id : integer variable identifier
   \return attributes of integer variable id
   \throw std::invalid_argument : if id is not an integer variable
   */
  tchecker::system::attributes_t const & intvar_attributes(tchecker::intvar_id_t id) const;

  /*!
   \brief Accessor
   \param name : integer variable name
   \return true if name is a declared integer variable, false otherwise
   */
  bool is_intvar(std::string const & name) const;

  /*!
   \brief Accessor
   \return integer variables
   */
  inline tchecker::integer_variables_t const & integer_variables() const { return _integer_variables; }

private:
  tchecker::integer_variables_t _integer_variables;                          /*!< Integer variables */
  std::vector<tchecker::system::attributes_t> _integer_variables_attributes; /*!< Integer variables attributes */
};

} // end of namespace system

} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_INTVAR_HH
