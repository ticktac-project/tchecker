# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.
option(TCK_ENABLE_BUGFIXES_TESTS "enable tests related to bug fixes" ON)

if(NOT TCK_ENABLE_BUGFIXES_TESTS)
    message(STATUS "Tests of bug fixes are disabled.")
    return()
endif()

set(BUGFIX_TESTS
    bug-002.tck
    bug-003a.tck
    bug-003b.tck
    bug-004.sh
    bug-005a.sh
    bug-005b.sh
    bug-005c.sh
    bug-007.cc
    bug-008a.sh
    bug-008b.sh
    bug-009.tck
    bug-041.sh

    memleak-01.tck
    )

foreach(testfile ${BUGFIX_TESTS})
    tck_add_nr_test(${testfile} test_name savelist)
    tck_add_nr_memcheck_test(${testfile} test_name savelist)
endforeach()

tck_add_savelist(save-bugfixes ${savelist})
