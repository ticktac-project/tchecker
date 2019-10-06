# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

find_package(BISON REQUIRED 3.0.4)
find_package(FLEX REQUIRED)

BISON_TARGET(program_parser program.yy
${CMAKE_CURRENT_BINARY_DIR}/program.tab.cc
DEFINES_FILE   ${CMAKE_CURRENT_BINARY_DIR}/program.tab.hh
COMPILE_FLAGS  -v)

FLEX_TARGET(program_lexer program.ll ${CMAKE_CURRENT_BINARY_DIR}/program.lex.cc COMPILE_FLAGS -Pppyy)

ADD_FLEX_BISON_DEPENDENCY(program_lexer program_parser)

# Fixing flex output (register variable class deprecated in c++11)
add_custom_command(OUTPUT program.lex-fixed.cc
COMMAND sed -e "s/register//g" ${FLEX_program_lexer_OUTPUTS} > program.lex-fixed.cc
DEPENDS ${FLEX_program_lexer_OUTPUTS})

# Build program parsing static object library
add_library(program_parsing_static OBJECT
  ${BISON_program_parser_OUTPUTS} program.lex-fixed.cc program.ll program.yy)
set_property(TARGET program_parsing_static PROPERTY CXX_STANDARD 17)
set_property(TARGET program_parsing_static PROPERTY CXX_STANDARD_REQUIRED ON)

# Build system parsing shared object library
if(LIBTCHECKER_ENABLE_SHARED)
  add_library(program_parsing_shared OBJECT
    ${BISON_program_parser_OUTPUTS} program.lex-fixed.cc program.ll program.yy)
  set_property(TARGET program_parsing_shared PROPERTY CXX_STANDARD 17)
  set_property(TARGET program_parsing_shared PROPERTY CXX_STANDARD_REQUIRED ON)
  if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
    set_property(TARGET program_parsing_shared PROPERTY
      POSITION_INDEPENDENT_CODE 1)
  endif()
endif()
