# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(ZG_SRC
${CMAKE_CURRENT_SOURCE_DIR}/zg_ta.cc
${TCHECKER_INCLUDE_DIR}/tchecker/zg/details/allocators.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/details/model.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/details/output.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/details/state.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/details/transition.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/details/ts.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/details/variables.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/details/zg.hh
${TCHECKER_INCLUDE_DIR}/tchecker/zg/zg_ta.hh
PARENT_SCOPE)
