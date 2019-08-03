# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(TA_SRC
${CMAKE_CURRENT_SOURCE_DIR}/details/builder.cc
${CMAKE_CURRENT_SOURCE_DIR}/details/output.cc
${CMAKE_CURRENT_SOURCE_DIR}/details/transition.cc
${CMAKE_CURRENT_SOURCE_DIR}/system.cc
${CMAKE_CURRENT_SOURCE_DIR}/ta.cc
${TCHECKER_INCLUDE_DIR}/tchecker/ta/details/allocators.hh
${TCHECKER_INCLUDE_DIR}/tchecker/ta/details/builder.hh
${TCHECKER_INCLUDE_DIR}/tchecker/ta/details/edge.hh
${TCHECKER_INCLUDE_DIR}/tchecker/ta/details/loc.hh
${TCHECKER_INCLUDE_DIR}/tchecker/ta/details/model.hh
${TCHECKER_INCLUDE_DIR}/tchecker/ta/details/output.hh
${TCHECKER_INCLUDE_DIR}/tchecker/ta/details/state.hh
${TCHECKER_INCLUDE_DIR}/tchecker/ta/details/system.hh
${TCHECKER_INCLUDE_DIR}/tchecker/ta/details/ta.hh
${TCHECKER_INCLUDE_DIR}/tchecker/ta/details/transition.hh
${TCHECKER_INCLUDE_DIR}/tchecker/ta/details/ts.hh
${TCHECKER_INCLUDE_DIR}/tchecker/ta/details/variables.hh
${TCHECKER_INCLUDE_DIR}/tchecker/ta/system.hh
${TCHECKER_INCLUDE_DIR}/tchecker/ta/ta.hh
PARENT_SCOPE)
