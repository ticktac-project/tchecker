include(CheckCXXCompilerFlag)

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
