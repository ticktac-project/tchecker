# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(VARIABLES_SRC
${CMAKE_CURRENT_SOURCE_DIR}/clocks.cc
${CMAKE_CURRENT_SOURCE_DIR}/intvars.cc
${CMAKE_CURRENT_SOURCE_DIR}/variables.cc
${TCHECKER_INCLUDE_DIR}/tchecker/variables/clocks.hh
${TCHECKER_INCLUDE_DIR}/tchecker/variables/intvars.hh
${TCHECKER_INCLUDE_DIR}/tchecker/variables/variables.hh
PARENT_SCOPE)
