# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

option(TCK_ENABLE_ALGOS_TESTS "enable tests related to tck_reach" ON)

if(NOT TCK_ENABLE_ALGOS_TESTS)
    message(STATUS "tck_reach tests are disabled.")
    return()
endif()

#set(PARSE_LABELS_SH "${CMAKE_CURRENT_SOURCE_DIR}/parse-labels.sh")

# Max number of lines for DOT results
set(DOT_MAX_SIZE 20000)

# Elements of INPUTS are colon-separated lists. The first element of each
# list is a test-case script located in ${EXAMPLES_DIR}; the tail of the list
# contains arguments passed to the generator script.
set(INPUTS
    ad94.sh:
    corsso.sh:2:2:10:1:2
    critical-region-async.sh:2:10
    csmacd.sh:3
    dining-philosophers.sh:3:3:10:0
    fischer-async.sh:3:10
    fischer-async-concurrent.sh:3:10
    parallel-c.sh:3
    train_gate.sh:2
    train_gate.sh:3
    )

if(NOT USEINT16)
    list(APPEND INPUTS
         ad94_mid.sh:)
endif()

if(USEINT64)
    list(APPEND INPUTS
         ad94_Long.sh:)
endif()

set(ACCEPT_TEST_REGEX "")
set(REJECT_TEST_REGEX "^.*no-one.*$")
set(MEMCHECKED_TEST_REGEX "^.*no-one.*$")

if(USEINT16)
    list(APPEND MEMCHECKED_TEST_REGEX  "^.*no-one.*$")
elseif(USEINT64)
    list(APPEND MEMCHECKED_TEST_REGEX  "^.*no-one.*$")
else()
    list(APPEND MEMCHECKED_TEST_REGEX "^.*no-one.*$")
endif()


set(NOT_MEMCHECKED_TEST_REGEX "^.*$")

set(SEARCH_ORDERS
    dfs
    bfs
    )

set(REACHABILITY_ALGORITHMS
    reach
    concur19
    covreach
    )

file(RELATIVE_PATH here ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Generating '${here}' tests. This may take a long time.")
tck_register_testcases("tck-reach-" CHECK_TESTCASES_ savelist TCK_REACH_INPUT_FILES ${INPUTS})

set(nb_tests 0)
foreach (inputfile ${TCK_REACH_INPUT_FILES})
    get_filename_component(testname ${inputfile} NAME_WE)

    foreach (algorithm ${REACHABILITY_ALGORITHMS})
        foreach (so ${SEARCH_ORDERS})
            set(TEST_NAME "${testname}_${algorithm}_${so}")
            tck_filter_testcase(accepted ${TEST_NAME} ACCEPT_TEST_REGEX REJECT_TEST_REGEX)
            if(NOT accepted)
                continue()
            endif()

            tck_add_test (${TEST_NAME} ${TEST_NAME} savelist)

            set_tests_properties(${TEST_NAME}
                                 PROPERTIES FIXTURES_REQUIRED "BUILD_TCK_REACH;CHECK_TESTCASES_${testname}")

            tck_add_test_envvar(testenv TCK_REACH "${TCK_REACH}")
            tck_add_test_envvar(testenv TEST "${TCK_REACH_SH}")
            tck_add_test_envvar(testenv TEST_ARGS "-a ${algorithm} -s ${so} ${inputfile}")
            tck_add_test_envvar(testenv DOT_MAX_SIZE "${DOT_MAX_SIZE}")
            tck_set_test_env(${TEST_NAME} testenv)
            unset(testenv)
            math(EXPR nb_tests "${nb_tests}+1")

            if(NOT TCK_ENABLE_MEMCHECK_TESTS)
                continue()
            endif()

            tck_filter_testcase(accepted ${TEST_NAME} MEMCHECKED_TEST_REGEX NOT_MEMCHECKED_TEST_REGEX)
            if(NOT accepted)
                continue()
            endif()

            set(TEST_NAME "memcheck-${TEST_NAME}")
            tck_add_test (${TEST_NAME} ${TEST_NAME} nopelist)

            set_tests_properties(${TEST_NAME}
                                 PROPERTIES FIXTURES_REQUIRED "BUILD_TCK_REACH;CHECK_TESTCASES_${testname}")

            tck_add_test_envvar(testenv IS_MEMCHECK_TEST "yes")
            tck_add_test_envvar(testenv TEST "${VALGRIND_PROGRAM}")
            tck_add_test_envvar(testenv TEST_ARGS "${VALGRIND_OPTIONS} ${TCK_REACH} -a ${algorithm} -s ${so} ${inputfile}")
            tck_set_test_env(${TEST_NAME} testenv)
            unset(testenv)

            math(EXPR nb_tests "${nb_tests}+1")
        endforeach ()
    endforeach ()
endforeach()

message(STATUS "${nb_tests} generated tests in ${here}.")

tck_add_savelist(save-algos ${savelist})
