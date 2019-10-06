# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

find_package(BISON REQUIRED 3.0.4)
find_package(FLEX REQUIRED)

BISON_TARGET(system_parser system.yy
${CMAKE_CURRENT_BINARY_DIR}/system.tab.cc
DEFINES_FILE  ${CMAKE_CURRENT_BINARY_DIR}/system.tab.hh
COMPILE_FLAGS -v)

FLEX_TARGET(system_lexer system.ll ${CMAKE_CURRENT_BINARY_DIR}/system.lex.cc COMPILE_FLAGS -Pspyy)

ADD_FLEX_BISON_DEPENDENCY(system_lexer system_parser)

# Fixing flex output (register variable class deprecated in c++11)
add_custom_command(OUTPUT system.lex-fixed.cc
COMMAND sed -e "s/register//g" ${FLEX_system_lexer_OUTPUTS} > system.lex-fixed.cc
DEPENDS ${FLEX_system_lexer_OUTPUTS})

# Build system parsing static object library
add_library(system_parsing_static OBJECT
  ${BISON_system_parser_OUTPUTS} system.lex-fixed.cc system.ll system.yy)
set_property(TARGET system_parsing_static PROPERTY CXX_STANDARD 17)
set_property(TARGET system_parsing_static PROPERTY CXX_STANDARD_REQUIRED ON)

# Build system parsing shared object library
if(LIBTCHECKER_ENABLE_SHARED)
  add_library(system_parsing_shared OBJECT
    ${BISON_system_parser_OUTPUTS} system.lex-fixed.cc system.ll system.yy)
  set_property(TARGET system_parsing_shared PROPERTY CXX_STANDARD 17)
  set_property(TARGET system_parsing_shared PROPERTY CXX_STANDARD_REQUIRED ON)
  if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
    set_property(TARGET system_parsing_shared PROPERTY
      POSITION_INDEPENDENT_CODE 1)
  endif()
endif()
