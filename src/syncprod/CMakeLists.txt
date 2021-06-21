# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(SYNCPROD_SRC
${CMAKE_CURRENT_SOURCE_DIR}/edges_iterators.cc
${CMAKE_CURRENT_SOURCE_DIR}/label.cc
${CMAKE_CURRENT_SOURCE_DIR}/state.cc
${CMAKE_CURRENT_SOURCE_DIR}/syncprod.cc
${CMAKE_CURRENT_SOURCE_DIR}/system.cc
${CMAKE_CURRENT_SOURCE_DIR}/transition.cc
${CMAKE_CURRENT_SOURCE_DIR}/vedge.cc
${CMAKE_CURRENT_SOURCE_DIR}/vloc.cc
${TCHECKER_INCLUDE_DIR}/tchecker/syncprod/allocators.hh
${TCHECKER_INCLUDE_DIR}/tchecker/syncprod/edges_iterators.hh
${TCHECKER_INCLUDE_DIR}/tchecker/syncprod/label.hh
${TCHECKER_INCLUDE_DIR}/tchecker/syncprod/state.hh
${TCHECKER_INCLUDE_DIR}/tchecker/syncprod/syncprod.hh
${TCHECKER_INCLUDE_DIR}/tchecker/syncprod/system.hh
${TCHECKER_INCLUDE_DIR}/tchecker/syncprod/transition.hh
${TCHECKER_INCLUDE_DIR}/tchecker/syncprod/vedge.hh
${TCHECKER_INCLUDE_DIR}/tchecker/syncprod/vloc.hh
PARENT_SCOPE)
