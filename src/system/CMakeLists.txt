# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(SYSTEM_SRC
${CMAKE_CURRENT_SOURCE_DIR}/static_analysis.cc
${CMAKE_CURRENT_SOURCE_DIR}/synchronization.cc
${TCHECKER_INCLUDE_DIR}/tchecker/system/builder.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/edge.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/loc.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/output.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/static_analysis.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/synchronization.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/system.hh
PARENT_SCOPE)
