# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

find_package(BISON REQUIRED 3.0.4)
find_package(FLEX REQUIRED)

BISON_TARGET(program_parser program.yy
${CMAKE_CURRENT_BINARY_DIR}/program.tab.cc
DEFINES_FILE   ${CMAKE_CURRENT_BINARY_DIR}/program.tab.hh
COMPILE_FLAGS  -v)

FLEX_TARGET(program_lexer program.ll ${CMAKE_CURRENT_BINARY_DIR}/program.lex.cc COMPILE_FLAGS -Ppp)

ADD_FLEX_BISON_DEPENDENCY(program_lexer program_parser)

# Fixing flex output (register variable class deprecated in c++11)
add_custom_command(OUTPUT program.lex-fixed.cc
COMMAND sed -e "s/register//g" ${FLEX_program_lexer_OUTPUTS} > program.lex-fixed.cc
DEPENDS ${FLEX_program_lexer_OUTPUTS})

add_library(program_parsing OBJECT ${BISON_program_parser_OUTPUTS} program.lex-fixed.cc program.ll program.yy)
set_property(TARGET program_parsing PROPERTY CXX_STANDARD 17)
set_property(TARGET program_parsing PROPERTY CXX_STANDARD_REQUIRED ON)
