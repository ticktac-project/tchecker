/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_VARIABLES_ACCESS_HH
#define TCHECKER_VARIABLES_ACCESS_HH

#include <tuple>

#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

#include "tchecker/basictypes.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file access.hh
 \brief Variable access map
 */

namespace tchecker {

/*!
 \brief Type of variable access
 */
enum variable_access_t {
  VACCESS_READ,
  VACCESS_WRITE,
  VACCESS_ANY,
};

/*!
 \brief Type of variable
 */
enum variable_type_t {
  VTYPE_CLOCK,
  VTYPE_INTVAR,
};

/*!
 \class variable_access_map_t
 \brief Access maps from processes to variables and, conversely, variables to processes
 */
class variable_access_map_t {
  using v2p_key_t = std::tuple<tchecker::variable_id_t, enum tchecker::variable_type_t, enum tchecker::variable_access_t>;
  using p2v_key_t = std::tuple<tchecker::process_id_t, enum tchecker::variable_type_t, enum tchecker::variable_access_t>;
  using pid_set_t = boost::container::flat_set<tchecker::process_id_t>;
  using vid_set_t = boost::container::flat_set<tchecker::variable_id_t>;
  using v2p_map_t = boost::container::flat_map<v2p_key_t, pid_set_t>;
  using p2v_map_t = boost::container::flat_map<p2v_key_t, vid_set_t>;

public:
  /*!
   \brief Clear
   \post this map is empty
   */
  void clear();

  /*!
   \brief Add a variable access
   \param vid : variable identifier
   \param vtype : variable type
   \param vaccess : variable access
   \param pid : process identifier
   \pre vaccess is either READ or WRITE, and vtype is either CLOCK or INTVAR
   \post Accesss of type `vaccess` and tchecker::ANY, to variable `vid` of type `vtype`, by process `pid` have been added
   \throw std::invalid_argument : if the precondition is violated
   */
  void add(tchecker::variable_id_t vid, enum tchecker::variable_type_t vtype, enum tchecker::variable_access_t vaccess,
           tchecker::process_id_t pid);

  /*!
   \brief Accessor
   \return true if there is a shared variable of any type in the map (i.e. a variable accessed by at least 2 processes),
   false otherwise
   */
  bool has_shared_variable() const;

  /*!
   \brief Accessor
   \param vtype : type of variable
   \return true if there is a shared variable of type vtype in the map,
   false otherwise
   */
  bool has_shared_variable(enum tchecker::variable_type_t vtype) const;

  /*!
   \brief Type of iterator over process identifiers
   */
  using process_id_iterator_t = pid_set_t::const_iterator;

  /*!
   \brief Accessor
   \param vid : variable identifier
   \param vtype : type of variable
   \param vaccess : variable access
   \return the range of identifiers of the processes that perform an access of type `vaccess` on variable `vid` of type `vtype`
   */
  tchecker::range_t<process_id_iterator_t> accessing_processes(tchecker::variable_id_t vid,
                                                               enum tchecker::variable_type_t vtype,
                                                               enum tchecker::variable_access_t vaccess) const;

  /*!
   \brief Accessor
   \param vid : variable identifier
   \param vtype : type of variable
   \param vaccess : variable access
   \pre there is exactly one process that perform an access of type `vaccess` on variable `vid` of type `vtype`
   \return identifier of the process that perform an access of type `vaccess` on variable `vid` of type `vtype`
   \throw std::invalid_argument : if the precondition is violated
   */
  process_id_t accessing_process(tchecker::variable_id_t vid, enum tchecker::variable_type_t vtype,
                                 enum tchecker::variable_access_t vaccess) const;

  /*!
   \brief Type of iterator over variable identifiers
   */
  using variable_id_iterator_t = vid_set_t::const_iterator;

  /*!
   \brief Accessor
   \param pid : process identifier
   \param vtype : type of variable
   \param vaccess : variable access
   \return the range of identifiers of the variables of type `vtype` that are accessed by process `pid`, with an access of type
   `vaccess`
   */
  tchecker::range_t<variable_id_iterator_t> accessed_variables(tchecker::process_id_t pid, enum tchecker::variable_type_t vtype,
                                                               enum tchecker::variable_access_t vaccess) const;

  /*!
   \brief Accessor
   \param pid : process identifier
   \param vid : variable identifier
   \param vtype : type of variable
   \param vaccess : variable access
   \return true if process pid access variable vid of type vtype with access type vaccess,
   false otherwise
   */
  bool access_variable(tchecker::process_id_t pid, tchecker::variable_id_t vid, enum tchecker::variable_type_t vtype,
                       enum tchecker::variable_access_t vaccess = tchecker::VACCESS_ANY) const;

private:
  v2p_map_t _v2p_map;       /*!< Map : variable ID -> accessing process IDs */
  p2v_map_t _p2v_map;       /*!< Map : process ID -> accessed variable IDs */
  pid_set_t _empty_pid_set; /*!< Empty process ID set */
  vid_set_t _empty_vid_set; /*!< Empty variable ID set */
};

} // end of namespace tchecker

#endif // TCHECKER_VARIABLES_ACCESS_HH
