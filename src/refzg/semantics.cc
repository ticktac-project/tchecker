/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/refzg/semantics.hh"

namespace tchecker {

namespace refzg {

/* factory */

tchecker::refzg::semantics_t * semantics_factory(enum tchecker::refzg::semantics_type_t semantics)
{
  switch (semantics) {
  case tchecker::refzg::STANDARD_SEMANTICS:
    return new tchecker::refzg::standard_semantics_t<tchecker::refzg::empty_sync_zone_policy_t::VALID>{};
  case tchecker::refzg::ELAPSED_SEMANTICS:
    return new tchecker::refzg::elapsed_semantics_t<tchecker::refzg::empty_sync_zone_policy_t::VALID>{};
  case tchecker::refzg::SYNC_STANDARD_SEMANTICS:
    return new tchecker::refzg::standard_semantics_t<tchecker::refzg::empty_sync_zone_policy_t::INVALID>{};
  case tchecker::refzg::SYNC_ELAPSED_SEMANTICS:
    return new tchecker::refzg::elapsed_semantics_t<tchecker::refzg::empty_sync_zone_policy_t::INVALID>{};
  default:
    throw std::invalid_argument("Unknown semantics over zones with reference clocks");
  }
}

} // end of namespace refzg

} // end of namespace tchecker