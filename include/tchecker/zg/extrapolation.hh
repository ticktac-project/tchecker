/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_ZG_EXTRAPOLATION_HH
#define TCHECKER_ZG_EXTRAPOLATION_HH

#include <memory>

#include "tchecker/basictypes.hh"
#include "tchecker/clockbounds/clockbounds.hh"
#include "tchecker/dbm/db.hh"
#include "tchecker/dbm/dbm.hh"
#include "tchecker/syncprod/vloc.hh"
#include "tchecker/ta/system.hh"

/*!
 \file extrapolation.hh
 \brief Zone extrapolations to ensure finiteness of zone graphs
 */

namespace tchecker {

namespace zg {

/*!
 \class extrapolation_t
 \brief Zone extrapolation
 */
class extrapolation_t {
public:
  /*!
    \brief Destructor
   */
  virtual ~extrapolation_t() = default;

  /*!
   \brief Zone extrapolation
   \param dbm : a dbm
   \param dim : dimension of dbm
   \param vloc : a tuple of locations
   \post dbm has been extrapolated using clocks bounds in vloc
   */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc) = 0;
};

/*!
 \class no_extrapolation_t
 \brief No zone extrapolation
*/
class no_extrapolation_t final : public tchecker::zg::extrapolation_t {
public:
  /*!
   \brief Destructor
  */
  virtual ~no_extrapolation_t() = default;

  /*!
   \brief Zone extrapolation
   \param dbm : a dbm
   \param dim : dimension of dbm
   \param vloc : a tuple of locations
   \post dbm has not been modified
   */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc);
};

namespace details {

/*!
 \class global_lu_extrapolation_t
 \brief Zone extrapolation with global LU clock bounds (abstract class)
*/
class global_lu_extrapolation_t : public tchecker::zg::extrapolation_t {
public:
  /*!
   \brief Constructor
   \param clock_bounds : global LU clock bounds map
  */
  global_lu_extrapolation_t(std::shared_ptr<tchecker::clockbounds::global_lu_map_t const> const & clock_bounds);

  /*!
  \brief Copy constructor
  */
  global_lu_extrapolation_t(tchecker::zg::details::global_lu_extrapolation_t const & e) = default;

  /*!
  \brief Move constructor
  */
  global_lu_extrapolation_t(tchecker::zg::details::global_lu_extrapolation_t && e) = default;

  /*!
   \brief Destructor
  */
  virtual ~global_lu_extrapolation_t() = default;

  /*!
  \brief Assignment operator
  */
  tchecker::zg::details::global_lu_extrapolation_t &
  operator=(tchecker::zg::details::global_lu_extrapolation_t const & e) = default;

  /*!
  \brief Move-assignment operator
  */
  tchecker::zg::details::global_lu_extrapolation_t & operator=(tchecker::zg::details::global_lu_extrapolation_t && e) = default;

protected:
  std::shared_ptr<tchecker::clockbounds::global_lu_map_t const> _clock_bounds; /*!< global LU clock bounds map */
};

} // end of namespace details

/*!
 \class global_extra_lu_t
 \brief ExtraLU zone extrapolation with global LU clock bounds
 */
class global_extra_lu_t final : public tchecker::zg::details::global_lu_extrapolation_t {
public:
  using tchecker::zg::details::global_lu_extrapolation_t::global_lu_extrapolation_t;

  /*!
  \brief Destructor
  */
  virtual ~global_extra_lu_t() = default;

  /*!
  \brief Zone extrapolation
  \param dbm : a dbm
  \param dim : dimension of dbm
  \param vloc : a tuple of locations
  \pre dim is 1 plus the number of clocks in the global LU clock bounds map (checked by assertion)
  \post ExtraLU has been applied to dbm with global LU clock bounds
 */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc);
};

/*!
 \class global_extra_lu_plus_t
 \brief ExtraLU+ zone extrapolation with global LU clock bounds
 */
class global_extra_lu_plus_t final : public tchecker::zg::details::global_lu_extrapolation_t {
public:
  using tchecker::zg::details::global_lu_extrapolation_t::global_lu_extrapolation_t;

  /*!
  \brief Destructor
  */
  virtual ~global_extra_lu_plus_t() = default;

  /*!
  \brief Zone extrapolation
  \param dbm : a dbm
  \param dim : dimension of dbm
  \param vloc : a tuple of locations
  \pre dim is 1 plus the number of clocks in the global LU clock bounds map (checked by assertion)
  \post ExtraLU+ has been applied to dbm with global LU clock bounds
 */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc);
};

namespace details {

/*!
 \class local_lu_extrapolation_t
 \brief Zone extrapolation with local LU clock bounds (abstract class)
*/
class local_lu_extrapolation_t : public tchecker::zg::extrapolation_t {
public:
  /*!
   \brief Constructor
   \param clock_bounds : local LU clock bounds map
  */
  local_lu_extrapolation_t(std::shared_ptr<tchecker::clockbounds::local_lu_map_t const> const & clock_bounds);

  /*!
  \brief Copy constructor
  */
  local_lu_extrapolation_t(tchecker::zg::details::local_lu_extrapolation_t const & e);

  /*!
  \brief Move constructor
  */
  local_lu_extrapolation_t(tchecker::zg::details::local_lu_extrapolation_t && e);

  /*!
   \brief Destructor
  */
  virtual ~local_lu_extrapolation_t();

  /*!
  \brief Assignment operator
  */
  tchecker::zg::details::local_lu_extrapolation_t & operator=(tchecker::zg::details::local_lu_extrapolation_t const & e);

  /*!
  \brief Move-assignment operator
  */
  tchecker::zg::details::local_lu_extrapolation_t & operator=(tchecker::zg::details::local_lu_extrapolation_t && e);

protected:
  tchecker::clockbounds::map_t * _l;                                          /*!< clock bounds L map */
  tchecker::clockbounds::map_t * _u;                                          /*!< clock bounds U map */
  std::shared_ptr<tchecker::clockbounds::local_lu_map_t const> _clock_bounds; /*!< local LU clock bounds map */
};

} // end of namespace details

/*!
 \class local_extra_lu_t
 \brief ExtraLU zone extrapolation with local LU clock bounds
 */
class local_extra_lu_t final : public tchecker::zg::details::local_lu_extrapolation_t {
public:
  using tchecker::zg::details::local_lu_extrapolation_t::local_lu_extrapolation_t;

  /*!
  \brief Destructor
  */
  virtual ~local_extra_lu_t() = default;

  /*!
  \brief Zone extrapolation
  \param dbm : a dbm
  \param dim : dimension of dbm
  \param vloc : a tuple of locations
  \pre dim is 1 plus the number of clocks in the global LU clock bounds map (checked by assertion)
  \post ExtraLU has been applied to dbm with local LU clock bounds in vloc
 */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc);
};

/*!
 \class local_extra_lu_plus_t
 \brief ExtraLU+ zone extrapolation with local LU clock bounds
 */
class local_extra_lu_plus_t final : public tchecker::zg::details::local_lu_extrapolation_t {
public:
  using tchecker::zg::details::local_lu_extrapolation_t::local_lu_extrapolation_t;

  /*!
  \brief Destructor
  */
  virtual ~local_extra_lu_plus_t() = default;

  /*!
  \brief Zone extrapolation
  \param dbm : a dbm
  \param dim : dimension of dbm
  \param vloc : a tuple of locations
  \pre dim is 1 plus the number of clocks in the global LU clock bounds map (checked by assertion)
  \post ExtraLU+ has been applied to dbm with local LU clock bounds in vloc
 */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc);
};

namespace details {

/*!
 \class global_m_extrapolation_t
 \brief Zone extrapolation with global M clock bounds (abstract class)
*/
class global_m_extrapolation_t : public tchecker::zg::extrapolation_t {
public:
  /*!
   \brief Constructor
   \param clock_bounds : global M clock bounds map
  */
  global_m_extrapolation_t(std::shared_ptr<tchecker::clockbounds::global_m_map_t const> const & clock_bounds);

  /*!
  \brief Copy constructor
  */
  global_m_extrapolation_t(tchecker::zg::details::global_m_extrapolation_t const & e) = default;

  /*!
  \brief Move constructor
  */
  global_m_extrapolation_t(tchecker::zg::details::global_m_extrapolation_t && e) = default;

  /*!
   \brief Destructor
  */
  virtual ~global_m_extrapolation_t() = default;

  /*!
  \brief Assignment operator
  */
  tchecker::zg::details::global_m_extrapolation_t &
  operator=(tchecker::zg::details::global_m_extrapolation_t const & e) = default;

  /*!
  \brief Move-assignment operator
  */
  tchecker::zg::details::global_m_extrapolation_t & operator=(tchecker::zg::details::global_m_extrapolation_t && e) = default;

protected:
  std::shared_ptr<tchecker::clockbounds::global_m_map_t const> _clock_bounds; /*!< global LU clock bounds map */
};

} // end of namespace details

/*!
 \class global_extra_m_t
 \brief ExtraM zone extrapolation with global M clock bounds
 */
class global_extra_m_t final : public tchecker::zg::details::global_m_extrapolation_t {
public:
  using tchecker::zg::details::global_m_extrapolation_t::global_m_extrapolation_t;

  /*!
  \brief Destructor
  */
  virtual ~global_extra_m_t() = default;

  /*!
  \brief Zone extrapolation
  \param dbm : a dbm
  \param dim : dimension of dbm
  \param vloc : a tuple of locations
  \pre dim is 1 plus the number of clocks in the global LU clock bounds map (checked by assertion)
  \post ExtraM has been applied to dbm with global M clock bounds
 */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc);
};

/*!
 \class global_extra_m_plus_t
 \brief ExtraM+ zone extrapolation with global M clock bounds
 */
class global_extra_m_plus_t final : public tchecker::zg::details::global_m_extrapolation_t {
public:
  using tchecker::zg::details::global_m_extrapolation_t::global_m_extrapolation_t;

  /*!
  \brief Destructor
  */
  virtual ~global_extra_m_plus_t() = default;

  /*!
  \brief Zone extrapolation
  \param dbm : a dbm
  \param dim : dimension of dbm
  \param vloc : a tuple of locations
  \pre dim is 1 plus the number of clocks in the global LU clock bounds map (checked by assertion)
  \post ExtraM+ has been applied to dbm with global M clock bounds
 */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc);
};

namespace details {

/*!
 \class local_m_extrapolation_t
 \brief Zone extrapolation with local M clock bounds (abstract class)
*/
class local_m_extrapolation_t : public tchecker::zg::extrapolation_t {
public:
  /*!
   \brief Constructor
   \param clock_bounds : local M clock bounds map
  */
  local_m_extrapolation_t(std::shared_ptr<tchecker::clockbounds::local_m_map_t const> const & clock_bounds);

  /*!
  \brief Copy constructor
  */
  local_m_extrapolation_t(tchecker::zg::details::local_m_extrapolation_t const & e);

  /*!
  \brief Move constructor
  */
  local_m_extrapolation_t(tchecker::zg::details::local_m_extrapolation_t && e);

  /*!
   \brief Destructor
  */
  virtual ~local_m_extrapolation_t();

  /*!
  \brief Assignment operator
  */
  tchecker::zg::details::local_m_extrapolation_t & operator=(tchecker::zg::details::local_m_extrapolation_t const & e);

  /*!
  \brief Move-assignment operator
  */
  tchecker::zg::details::local_m_extrapolation_t & operator=(tchecker::zg::details::local_m_extrapolation_t && e);

protected:
  tchecker::clockbounds::map_t * _m;                                         /*!< clock bounds M map */
  std::shared_ptr<tchecker::clockbounds::local_m_map_t const> _clock_bounds; /*!< local M clock bounds map */
};

} // end of namespace details

/*!
 \class local_extra_m_t
 \brief ExtraM zone extrapolation with local M clock bounds
 */
class local_extra_m_t final : public tchecker::zg::details::local_m_extrapolation_t {
public:
  using tchecker::zg::details::local_m_extrapolation_t::local_m_extrapolation_t;

  /*!
  \brief Destructor
  */
  virtual ~local_extra_m_t() = default;

  /*!
  \brief Zone extrapolation
  \param dbm : a dbm
  \param dim : dimension of dbm
  \param vloc : a tuple of locations
  \pre dim is 1 plus the number of clocks in the global LU clock bounds map (checked by assertion)
  \post ExtraM has been applied to dbm with local M clock bounds in vloc
 */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc);
};

/*!
 \class local_extra_m_plus_t
 \brief ExtraM+ zone extrapolation with local M clock bounds
 */
class local_extra_m_plus_t final : public tchecker::zg::details::local_m_extrapolation_t {
public:
  using tchecker::zg::details::local_m_extrapolation_t::local_m_extrapolation_t;

  /*!
  \brief Destructor
  */
  virtual ~local_extra_m_plus_t() = default;

  /*!
  \brief Zone extrapolation
  \param dbm : a dbm
  \param dim : dimension of dbm
  \param vloc : a tuple of locations
  \pre dim is 1 plus the number of clocks in the global LU clock bounds map (checked by assertion)
  \post ExtraM+ has been applied to dbm with local M clock bounds in vloc
 */
  virtual void extrapolate(tchecker::dbm::db_t * dbm, tchecker::clock_id_t dim, tchecker::vloc_t const & vloc);
};

/*!
 \brief Type of extrapolation
*/
enum extrapolation_type_t {
  NO_EXTRAPOLATION,     /*!< see tchecker::zg::no_extrapolation_t */
  EXTRA_LU_GLOBAL,      /*!< see tchecker::zg::global_extra_lu_t */
  EXTRA_LU_LOCAL,       /*!< see tchecker::zg::local_extra_lu_t */
  EXTRA_LU_PLUS_GLOBAL, /*!< see tchecker::zg::global_extra_lu_plus_t */
  EXTRA_LU_PLUS_LOCAL,  /*!< see tchecker::zg::local_extra_lu_plus_t */
  EXTRA_M_GLOBAL,       /*!< see tchecker::zg::global_extra_m_t */
  EXTRA_M_LOCAL,        /*!< see tchecker::zg::local_extra_m_t */
  EXTRA_M_PLUS_GLOBAL,  /*!< see tchecker::zg::global_extra_m_plus_t */
  EXTRA_M_PLUS_LOCAL,   /*!< see tchecker::zg::local_extra_m_plus_t */
};

/*!
 \brief Zone extrapolation factory
 \param extrapolation_type : type of extrapolation
 \param system : system of timed processes
 \return a zone extrapolation of type extrapolation_type using clock bounds
 inferred from system, nullptr if clock bounds cannot be inferred from system (see
 tchecker::clockbounds::compute_clockbounds)
 \note the returned extrapolation must be deallocated by the caller
 \throw std::invalid_argument : if extrapolation_type is unknown
 */
tchecker::zg::extrapolation_t * extrapolation_factory(enum extrapolation_type_t extrapolation_type,
                                                      tchecker::ta::system_t const & system);

/*!
 \brief Zone extrapolation factory
 \param extrapolation_type : type of extrapolation
 \param clock_bounds : clock bounds
 \return a zone extrapolation of type extrapolation_type using clock bounds from
 clock_bounds
 \note the returned extrapolation must be deallocated by the caller
 \throw std::invalid_argument : if extrapolation_type is unknown
 */
tchecker::zg::extrapolation_t * extrapolation_factory(enum extrapolation_type_t extrapolation_type,
                                                      tchecker::clockbounds::clockbounds_t const & clock_bounds);

} // end of namespace zg

} // end of namespace tchecker

#endif // TCHECKER_ZG_EXTRAPOLATION_HH