# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(ZG_SRC
${CMAKE_CURRENT_SOURCE_DIR}/extrapolation.cc
${CMAKE_CURRENT_SOURCE_DIR}/semantics.cc
${CMAKE_CURRENT_SOURCE_DIR}/state.cc
${CMAKE_CURRENT_SOURCE_DIR}/transition.cc
${CMAKE_CURRENT_SOURCE_DIR}/zg.cc
${CMAKE_CURRENT_SOURCE_DIR}/zone.cc
${TCHECKER_INCLUDE_DIR}/tchecker/zg/allocators.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/extrapolation.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/semantics.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/state.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/transition.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/zg.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/zone.hh
PARENT_SCOPE)
