# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(STATEMENT_SRC
${CMAKE_CURRENT_SOURCE_DIR}/clock_updates.cc
${CMAKE_CURRENT_SOURCE_DIR}/statement.cc
${CMAKE_CURRENT_SOURCE_DIR}/static_analysis.cc
${CMAKE_CURRENT_SOURCE_DIR}/type_inference.cc
${CMAKE_CURRENT_SOURCE_DIR}/typechecking.cc
${CMAKE_CURRENT_SOURCE_DIR}/typed_statement.cc
${TCHECKER_INCLUDE_DIR}/tchecker/statement/clock_updates.hh
${TCHECKER_INCLUDE_DIR}/tchecker/statement/statement.hh
${TCHECKER_INCLUDE_DIR}/tchecker/statement/static_analysis.hh
${TCHECKER_INCLUDE_DIR}/tchecker/statement/type_inference.hh
${TCHECKER_INCLUDE_DIR}/tchecker/statement/typechecking.hh
${TCHECKER_INCLUDE_DIR}/tchecker/statement/typed_statement.hh
PARENT_SCOPE)
