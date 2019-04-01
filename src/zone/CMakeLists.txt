# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(ZONE_SRC
${CMAKE_CURRENT_SOURCE_DIR}/dbm/semantics.cc
${CMAKE_CURRENT_SOURCE_DIR}/dbm/zone.cc
${CMAKE_CURRENT_SOURCE_DIR}/offset_dbm/semantics.cc
${CMAKE_CURRENT_SOURCE_DIR}/offset_dbm/zone.cc
${TCHECKER_INCLUDE_DIR}/tchecker/zone/dbm/extrapolation.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zone/dbm/semantics.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zone/dbm/zone.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zone/offset_dbm/semantics.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zone/offset_dbm/zone.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zone/offset_zone.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zone/zone.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zone/semantics.hh
PARENT_SCOPE)
