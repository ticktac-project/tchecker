if(NOT DEFINED SRC)
    message(FATAL_ERROR "missing required variable SRC")
endif()
if(NOT DEFINED DST)
    message(FATAL_ERROR "missing required variable DST")
endif()

if(EXISTS "${SRC}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${SRC}" "${DST}")
endif()
