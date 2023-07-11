/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#ifndef TCHECKER_GRAPH_EDGE_HH
#define TCHECKER_GRAPH_EDGE_HH

#include <map>
#include <string>

#include "tchecker/syncprod/vedge.hh"
#include "tchecker/zg/zg.hh"

namespace tchecker {

namespace graph {

/*!
\class edge_vedge_t
\brief Edge with a vedge
*/
class edge_vedge_t {
public:
  /*!
   \brief Constructor
   \param vedge : a tuple of edges
   \post this node keeps a shared pointer on vedge
  */
  edge_vedge_t(tchecker::vedge_sptr_t const & vedge);

  /*!
   \brief Constructor
   \param vedge : a tuple of edges
   \post this node keeps a shared pointer on vedge
  */
  edge_vedge_t(tchecker::const_vedge_sptr_t const & vedge);

  /*!
   \brief Accessor
   \return zone graph vedge in this edge
  */
  inline tchecker::vedge_t const & vedge() const { return *_vedge; }

  /*!
   \brief Accessor
   \return shared pointer to the zone graph vedge in this edge
  */
  inline tchecker::intrusive_shared_ptr_t<tchecker::shared_vedge_t const> vedge_ptr() const { return _vedge; }

private:
  tchecker::const_vedge_sptr_t const _vedge; /*!< Tuple of edges */
};

} // namespace graph

} // namespace tchecker

#endif // TCHECKER_GRAPH_EDGE_HH