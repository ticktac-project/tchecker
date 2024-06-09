/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_SYSTEM_LOC_HH
#define TCHECKER_SYSTEM_LOC_HH

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "tchecker/basictypes.hh"
#include "tchecker/system/attribute.hh"
#include "tchecker/utils/index.hh"
#include "tchecker/utils/iterator.hh"

/*!
 \file loc.hh
 \brief System locations
 */

namespace tchecker {

namespace system {

/*!
 \class loc_t
 \brief System location
 */
class loc_t {
public:
  /*!
   \brief Constructor
   \param pid : process identifier
   \param id : location identifier
   \param name : location name
   \param attributes : location attributes
   \pre name is not empty
   \throw std::invalid_argument : if the precondition is violated
   */
  loc_t(tchecker::process_id_t pid, tchecker::loc_id_t id, std::string const & name,
        tchecker::system::attributes_t const & attributes);

  /*!
   \brief Copy constructor
   */
  loc_t(tchecker::system::loc_t const &) = default;

  /*!
   \brief Move constructor
   */
  loc_t(tchecker::system::loc_t &&) = default;

  /*!
   \brief Destructor
   */
  ~loc_t() = default;

  /*!
   \brief Assignement operator
   */
  tchecker::system::loc_t & operator=(tchecker::system::loc_t const &) = default;

  /*!
   \brief Move assignement operator
   */
  tchecker::system::loc_t & operator=(tchecker::system::loc_t &&) = default;

  /*!
   \brief Accessor
   \return process identifier
   */
  inline tchecker::process_id_t pid() const { return _pid; }

  /*!
   \brief Accessor
   \return Identifier
   */
  inline tchecker::loc_id_t id() const { return _id; }

  /*!
   \brief Accessor
   \return Name
   */
  inline std::string const & name() const { return _name; }

  /*!
   \brief Accessor
   \return Attributes
   */
  inline tchecker::system::attributes_t const & attributes() const { return _attributes; }

private:
  tchecker::process_id_t _pid;                /*!< Process ID */
  tchecker::loc_id_t _id;                     /*!< Identifier */
  std::string _name;                          /*!< Name */
  tchecker::system::attributes_t _attributes; /*!< Attributes */
};

/*!
 \brief Type of shared pointer on location
 */
using loc_shared_ptr_t = std::shared_ptr<tchecker::system::loc_t>;

/*!
 \brief Type of shared pointer on const location
 */
using loc_const_shared_ptr_t = std::shared_ptr<tchecker::system::loc_t const>;

/*!
 \class locs_t
 \brief Collection of locations
 */
class locs_t {
public:
  /*!
   \brief Constructor
   */
  locs_t() = default;

  /*!
   \brief Copy constructor
   */
  locs_t(tchecker::system::locs_t const &);

  /*!
   \brief Move constructor
   */
  locs_t(tchecker::system::locs_t &&);

  /*!
   \brief Destructor
   */
  ~locs_t();

  /*!
   \brief Assignment operator
   */
  tchecker::system::locs_t & operator=(tchecker::system::locs_t const &);

  /*!
   \brief Move-assignment operator
   */
  tchecker::system::locs_t & operator=(tchecker::system::locs_t &&);

  /*!
   \brief Clear
   \post this is empty
   */
  void clear();

  /*!
   \brief Add a location
   \param pid : process identifier
   \param name : location name
   \param attributes : location attributes
   \pre name is not already declared in process pid
   \throw std::invalid_argument : if name is a declared location of process pid
   \throw std::runtime_error : if location identifiers have been exhausted
   \note if attribute `initial` is in attr, then the location is added as an initial location of process pid
   */
  void add_location(tchecker::process_id_t pid, std::string const & name, tchecker::system::attributes_t const & attributes);

  /*!
   \brief Accessor
   \return number of locations
   \note all integers in 0..locations_count()-1 are valid location identifiers
   */
  inline std::size_t locations_count() const { return _locs.size(); }

  /*!
   \brief Type of range of locations identifiers
   */
  using locations_identifiers_range_t = tchecker::integer_range_t<tchecker::loc_id_t>;

  /*!
   \brief Accessor
   \return range of locations identifiers 0..locations_count()-1
   */
  locations_identifiers_range_t locations_identifiers() const;

  /*!
   \brief Accessor
   \param id : location id
   \pre id < locations_count()  (checked by assertion)
   \return location with identifier id
  */
  inline tchecker::system::loc_const_shared_ptr_t location(tchecker::loc_id_t id) const
  {
    assert(id < _locs.size());
    assert(_locs[id]->id() == id);
    return _locs[id];
  }

  /*!
   \brief Accessor
   \param pid : process identifier
   \param name : location name
   \return location name from process pid
   \throw std::invalid_argument : if name is not a location of process pid
   */
  tchecker::system::loc_const_shared_ptr_t location(tchecker::process_id_t pid, std::string const & name) const;

  /*!
   \class const_iterator_t
   \brief Type of iterator over locations
   */
  class const_iterator_t : public std::vector<tchecker::system::loc_shared_ptr_t>::const_iterator {
  public:
    /*!
     \brief Default constructor
    */
    const_iterator_t() = default;

    /*!
     \brief Constructor
     */
    const_iterator_t(std::vector<tchecker::system::loc_shared_ptr_t>::const_iterator const & it);

    /*!
     \brief Dereference operator
     \return location marked const
     */
    inline tchecker::system::loc_const_shared_ptr_t operator*() const
    {
      return std::vector<tchecker::system::loc_shared_ptr_t>::const_iterator::operator*();
    }
  };

  /*!
   \brief Accessor
   \return range of locations
   */
  tchecker::range_t<tchecker::system::locs_t::const_iterator_t> locations() const;

  /*!
   \brief Accessor
   \param pid : process identifier
   \return rangle of locations of process pid
   \throw std::invalid_argument : if pid is not a valid process identifier
  */
  tchecker::range_t<tchecker::system::locs_t::const_iterator_t> locations(tchecker::process_id_t pid) const;

  /*!
   \brief Accessor
   \param pid : process identifier
   \return range of initial locations of process pid
   */
  tchecker::range_t<tchecker::system::locs_t::const_iterator_t> initial_locations(tchecker::process_id_t pid) const;

  /*!
   \brief Checks validity of location identifier
   \param id : location identifier
   \return true if id is a declared location identifier, false othwerise
   */
  bool is_location(tchecker::loc_id_t id) const;

  /*!
   \brief Checks validity of location name
   \param pid : process identifier
   \param name : location name
   \return true if name is a declared location of process pid, false otherwise
   */
  bool is_location(tchecker::process_id_t pid, std::string const & name) const;

  /*!
   \brief Checks if a location is initial
   \param id : location identifier
   \pre id is a valid location identifier
   \return true if id is an initial location, false otherwise
   \throw std::invalid_argument : if is is not a valid location identifier
  */
  bool is_initial_location(tchecker::loc_id_t id) const;

private:
  /*!
   \brief Duplicate locations
   \param locs : collection of locations
   \post all locations in locs have been duplicated in this collection
   */
  void add_locations(tchecker::system::locs_t const & locs);

  /*!< Locations */
  std::vector<tchecker::system::loc_shared_ptr_t> _locs;
  /*!< Map pid -> locations */
  std::vector<std::vector<tchecker::system::loc_shared_ptr_t>> _process_locs;
  /*!< Map pid -> initial locations */
  std::vector<std::vector<tchecker::system::loc_shared_ptr_t>> _initial_locs;
  /*!< Map (pid, loc name) -> location */
  std::vector<tchecker::index_t<std::string, tchecker::system::loc_shared_ptr_t>> _locs_index;
  /*!< Empty set of locations */
  static std::vector<tchecker::system::loc_shared_ptr_t> const _empty_locs;
  /* IMPLEMENTATION NOTE
    _process_locs could be obtained by filtering _locs using a filter iterator for instance. However
    we need to use it in our own cartesian_iterator_t which is not good enough to be combied with
    filter iterators. This will be resolved once std::range is implemented as part as c++20 in compilers
  */
};

} // end of namespace system

} // end of namespace tchecker

#endif // TCHECKER_SYSTEM_LOC_HH
