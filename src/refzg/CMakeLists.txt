# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(REFZG_SRC
${CMAKE_CURRENT_SOURCE_DIR}/refzg.cc
${CMAKE_CURRENT_SOURCE_DIR}/semantics.cc
${CMAKE_CURRENT_SOURCE_DIR}/state.cc
${CMAKE_CURRENT_SOURCE_DIR}/transition.cc
${CMAKE_CURRENT_SOURCE_DIR}/zone.cc
${TCHECKER_INCLUDE_DIR}/tchecker/refzg/allocators.hh
${TCHECKER_INCLUDE_DIR}/tchecker/refzg/refzg.hh
${TCHECKER_INCLUDE_DIR}/tchecker/refzg/semantics.hh
${TCHECKER_INCLUDE_DIR}/tchecker/refzg/state.hh
${TCHECKER_INCLUDE_DIR}/tchecker/refzg/transition.hh
${TCHECKER_INCLUDE_DIR}/tchecker/refzg/zone.hh
PARENT_SCOPE)
