# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(SYSTEM_SRC
${CMAKE_CURRENT_SOURCE_DIR}/attribute.cc
${CMAKE_CURRENT_SOURCE_DIR}/clock.cc
${CMAKE_CURRENT_SOURCE_DIR}/edge.cc
${CMAKE_CURRENT_SOURCE_DIR}/event.cc
${CMAKE_CURRENT_SOURCE_DIR}/intvar.cc
${CMAKE_CURRENT_SOURCE_DIR}/loc.cc
${CMAKE_CURRENT_SOURCE_DIR}/output.cc
${CMAKE_CURRENT_SOURCE_DIR}/process.cc
${CMAKE_CURRENT_SOURCE_DIR}/static_analysis.cc
${CMAKE_CURRENT_SOURCE_DIR}/synchronization.cc
${CMAKE_CURRENT_SOURCE_DIR}/system.cc
${TCHECKER_INCLUDE_DIR}/tchecker/system/attribute.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/clock.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/edge.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/event.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/intvar.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/loc.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/output.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/process.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/static_analysis.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/synchronization.hh
${TCHECKER_INCLUDE_DIR}/tchecker/system/system.hh
PARENT_SCOPE)
