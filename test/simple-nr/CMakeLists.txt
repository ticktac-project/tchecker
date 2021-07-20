# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

option(TCK_ENABLE_SIMPLE_NR_TESTS "enable simplest non-regression tests" ON)

if(NOT TCK_ENABLE_SIMPLE_NR_TESTS)
    message(STATUS "Simplest non-regression tests are disabled.")
    return()
endif()

file(GLOB SIMPLE_NR_TESTS
     LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     "bubble-sort-?.tck"
     "expr-*.tck"
     "local-array-?.tck"
     "loc-prop-?.tck"
     "parsing-*.tck"
     "properties-?.tck"
     "stuck-process-?.tck"
     "simple-loop-?.tck"
     "syracuse-?.tck"
    )

foreach(testfile ${SIMPLE_NR_TESTS})
    tck_add_nr_test(${testfile} test_name savelist)
    tck_add_nr_memcheck_test(${testfile} test_name savelist)
endforeach()

tck_add_savelist(save-simple-nr ${savelist})

