# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(GRAPH_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/edge.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/node.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/output.cc
    ${TCHECKER_INCLUDE_DIR}/tchecker/graph/allocators.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/graph/cover_graph.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/graph/directed_graph.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/graph/edge.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/graph/find_graph.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/graph/node.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/graph/output.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/graph/path.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/graph/reachability_graph.hh
    ${TCHECKER_INCLUDE_DIR}/tchecker/graph/subsumption_graph.hh
    PARENT_SCOPE)
