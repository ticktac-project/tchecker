# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

add_subdirectory(reach)

set(ALGORITHMS_SRC
${CMAKE_CURRENT_SOURCE_DIR}/search_order.cc
${CMAKE_CURRENT_SOURCE_DIR}/stats.cc
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/search_order.hh
${TCHECKER_INCLUDE_DIR}/tchecker/algorithms/stats.hh
${REACH_SRC}
PARENT_SCOPE)
