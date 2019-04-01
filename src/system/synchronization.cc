/*
 * This file is a part of the TChecker project.
 *
 * See files AUTHORS and LICENSE for copyright details.
 *
 */

#include "tchecker/system/synchronization.hh"

namespace tchecker {
  
  // sync_constraint_t
  
  sync_constraint_t::sync_constraint_t(tchecker::process_id_t pid, tchecker::event_id_t event_id,
                                       tchecker::sync_strength_t strength)
  : _pid(pid),
  _event_id(event_id),
  _strength(strength)
  {}
  
} // end of namespace tchecker
