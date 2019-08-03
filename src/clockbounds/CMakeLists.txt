# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(CLOCKBOUNDS_SRC
${CMAKE_CURRENT_SOURCE_DIR}/clockbounds.cc
${CMAKE_CURRENT_SOURCE_DIR}/solver.cc
${TCHECKER_INCLUDE_DIR}/tchecker/clockbounds/clockbounds.hh
${TCHECKER_INCLUDE_DIR}/tchecker/clockbounds/model.hh
${TCHECKER_INCLUDE_DIR}/tchecker/clockbounds/solver.hh
${TCHECKER_INCLUDE_DIR}/tchecker/clockbounds/variables.hh
${TCHECKER_INCLUDE_DIR}/tchecker/clockbounds/vlocbounds.hh
PARENT_SCOPE)
