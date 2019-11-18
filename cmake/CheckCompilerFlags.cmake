# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

include(CheckCXXCompilerFlag)

# Options to support 16/32/64 bit dbm's
# 32 is default
option(USE16 "Use 16bit int" OFF)
option(USE64 "Use 64bit int" OFF)

set(INTEGER_T_SIZE 32)
if (USE16)
    set(INTEGER_T_SIZE 16)
elseif(USE64)
    set(INTEGER_T_SIZE 64)
else()
endif()

message(STATUS "Setting sizeof(integer_t) to ${INTEGER_T_SIZE}")

#
# Check if "flag" is accepted by the current CXX compiler. If the flag is
# supported its value is assigned to the variable "var"; else "var" is asigned
# an empty string.
#
function(tck_check_cxx_flags flag var)
    check_cxx_compiler_flag("${flag}" ${var}_IS_SUPPORTED)
    if(${var}_IS_SUPPORTED)
        set(${var} ${flag} PARENT_SCOPE)
    else()
        set(${var} "")
    endif()
endfunction()
