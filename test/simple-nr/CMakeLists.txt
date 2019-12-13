# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

option(TCK_ENABLE_SIMPLE_NR_TESTS "enable simplest non-regression tests" ON)

if(NOT TCK_ENABLE_SIMPLE_NR_TESTS)
    message(STATUS "Simplest non-regression tests are disabled.")
    return()
endif()

set(SIMPLE_NR_TESTS
    bubble-sort-1.tck
    bubble-sort-2.tck
    bubble-sort-3.tck
    bubble-sort-4.tck
    bubble-sort-5.tck
    bubble-sort-6.tck
    bubble-sort-7.tck
    bubble-sort-8.tck

    local-array-1.tck
    local-array-2.tck
    local-array-3.tck
    local-array-4.tck

    stuck-process-1.tck
    stuck-process-2.tck
    stuck-process-3.tck

    syracuse-1.tck
    syracuse-2.tck
    syracuse-3.tck
    syracuse-4.tck
    syracuse-5.tck
    syracuse-5b.tck
    syracuse-6.tck
    syracuse-7.tck
    )

foreach(testfile ${SIMPLE_NR_TESTS})
    tck_add_nr_test(${testfile} test_name savelist)
    tck_add_nr_memcheck_test(${testfile} test_name savelist)
endforeach()

tck_add_savelist(save-simple-nr ${savelist})

