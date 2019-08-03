# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(ASYNC_ZG_SRC
${CMAKE_CURRENT_SOURCE_DIR}/async_zg_ta.cc
${TCHECKER_INCLUDE_DIR}/tchecker/async_zg/async_zg_ta.hh
${TCHECKER_INCLUDE_DIR}/tchecker/async_zg/details/allocators.hh
${TCHECKER_INCLUDE_DIR}/tchecker/async_zg/details/model.hh
${TCHECKER_INCLUDE_DIR}/tchecker/async_zg/details/output.hh
${TCHECKER_INCLUDE_DIR}/tchecker/async_zg/details/state.hh
${TCHECKER_INCLUDE_DIR}/tchecker/async_zg/details/transition.hh
${TCHECKER_INCLUDE_DIR}/tchecker/async_zg/details/ts.hh
${TCHECKER_INCLUDE_DIR}/tchecker/async_zg/details/variables.hh
${TCHECKER_INCLUDE_DIR}/tchecker/async_zg/details/zg.hh
PARENT_SCOPE)
