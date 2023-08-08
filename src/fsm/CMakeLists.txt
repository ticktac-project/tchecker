# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(FSM_SRC
${CMAKE_CURRENT_SOURCE_DIR}/state.cc
${CMAKE_CURRENT_SOURCE_DIR}/transition.cc
${CMAKE_CURRENT_SOURCE_DIR}/fsm.cc
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/fsm.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/state.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/transition.hh
${TCHECKER_INCLUDE_DIR}/tchecker/fsm/allocators.hh
PARENT_SCOPE)
