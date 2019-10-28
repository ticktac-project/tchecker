#
# Arguments of this function are name of variables. The function prints out the
# content of the variables
#
function(tck_display_variables)
    foreach (var IN LISTS ARGN)
        message("${var}=${${var}}")
    endforeach ()
endfunction(tck_display_variables)

function(tck_check_defined_variables)
    foreach(var IN LISTS ARGN)
        if(NOT DEFINED ${var})
            message(FATAL_ERROR "missing required variable ${var}")
        endif()
    endforeach()
endfunction(tck_check_defined_variables)
