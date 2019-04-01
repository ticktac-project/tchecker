# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(COVREACH_SRC
${CMAKE_CURRENT_SOURCE_DIR}/options.cc
${CMAKE_CURRENT_SOURCE_DIR}/run.cc
${CMAKE_CURRENT_SOURCE_DIR}/stats.cc
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/covreach/accepting.hh
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/covreach/algorithm.hh
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/covreach/builder.hh
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/covreach/cover.hh
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/covreach/graph.hh
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/covreach/options.hh
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/covreach/output.hh
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/covreach/run.hh
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/covreach/stats.hh
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/covreach/waiting.hh
PARENT_SCOPE)
