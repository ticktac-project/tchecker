# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(EXPRESSION_SRC
${CMAKE_CURRENT_SOURCE_DIR}/expression.cc
${CMAKE_CURRENT_SOURCE_DIR}/static_analysis.cc
${CMAKE_CURRENT_SOURCE_DIR}/type_inference.cc
${CMAKE_CURRENT_SOURCE_DIR}/typechecking.cc
${CMAKE_CURRENT_SOURCE_DIR}/typed_expression.cc
${TCHECKER_INCLUDE_DIR}/tchecker/expression/expression.hh
${TCHECKER_INCLUDE_DIR}/tchecker/expression/static_analysis.hh
${TCHECKER_INCLUDE_DIR}/tchecker/expression/type_inference.hh
${TCHECKER_INCLUDE_DIR}/tchecker/expression/typechecking.hh
${TCHECKER_INCLUDE_DIR}/tchecker/expression/typed_expression.hh
PARENT_SCOPE)
