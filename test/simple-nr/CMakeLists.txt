# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

option(TCK_ENABLE_SIMPLE_NR_TESTS "enable simplest non-regression tests" ON)

if(NOT TCK_ENABLE_SIMPLE_NR_TESTS)
    message(STATUS "Simplest non-regression tests are disabled.")
    return()
endif()

set(SIMPLE_NR_TESTS
    stuck-process-1.tck
    stuck-process-2.tck
    stuck-process-3.tck
    )

foreach(testfile ${SIMPLE_NR_TESTS})
    tck_add_nr_test(${testfile} test_name savelist)
endforeach()

tck_add_savelist(save-simple-nr ${savelist})
