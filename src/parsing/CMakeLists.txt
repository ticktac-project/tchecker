# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

add_subdirectory(program_parser)
add_subdirectory(system_parser)

set(PARSING_SRC
${CMAKE_CURRENT_SOURCE_DIR}/declaration.cc
${TCHECKER_INCLUDE_DIR}/tchecker/parsing/declaration.hh
${TCHECKER_INCLUDE_DIR}/tchecker/parsing/parsing.hh
PARENT_SCOPE)
