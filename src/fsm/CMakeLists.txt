# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(FSM_SRC
${CMAKE_CURRENT_SOURCE_DIR}/details/builder.cc
${CMAKE_CURRENT_SOURCE_DIR}/details/transition.cc
${CMAKE_CURRENT_SOURCE_DIR}/fsm.cc
${CMAKE_CURRENT_SOURCE_DIR}/system.cc
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/details/allocators.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/details/builder.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/details/edge.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/details/fsm.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/details/loc.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/details/model.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/details/output.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/details/state.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/details/static_analysis.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/details/system.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/details/transition.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/details/ts.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/details/variables.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/fsm.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/system.hh
PARENT_SCOPE)
