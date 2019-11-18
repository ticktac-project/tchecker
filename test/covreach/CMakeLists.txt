# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

option(TCK_ENABLE_COVREACH_TESTS "enable tests related to covreach" ON)

if(NOT TCK_ENABLE_COVREACH_TESTS)
    message(STATUS "Covreach tests are disabled.")
    return()
endif()

set(PARSE_LABELS_SH "${CMAKE_CURRENT_SOURCE_DIR}/parse-labels.sh")

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
    train_gate.sh:3
    )

if(NOT USE16)
    list(APPEND INPUTS
         ad94_mid.sh:)
endif()

if(USE64)
    list(APPEND INPUTS
         ad94_Long.sh:)
endif()

set(ACCEPT_TEST_REGEX "")
set(REJECT_TEST_REGEX "^.*inclusion.*NOextra.*$")
set(MEMCHECKED_TEST_REGEX
    "^.*ad94_[0-9].*raw.*$"
    "^.*critical-region-async.*raw.*$")

if(USE16)
    list(APPEND MEMCHECKED_TEST_REGEX
         "^.*ad94_[0-9].*raw.*$")
elseif(USE64)
    list(APPEND MEMCHECKED_TEST_REGEX
         "^.*ad94_Long.*raw.*$")
else()
    list(APPEND MEMCHECKED_TEST_REGEX
         "^.*ad94_mid.*raw.*$")
endif()


set(NOT_MEMCHECKED_TEST_REGEX "^.*$")

set(COVREACH_OUTPUT_FORMATS
    raw
    dot
    )

set(COVREACH_SEARCH_ORDERS
    dfs
    bfs
    )

set(COVREACH_ZONE_GRAPHS
    zg
    async_zg
    )

set(COVREACH_SEMANTICS
    elapsed
    non-elapsed
    )

set(COVREACH_ZG_EXTRAPOLATIONS
    NOextra
    extraMg
    extraMl
    extraM+g
    extraM+l
    extraLUg
    extraLUl
    extraLU+g
    extraLU+l
    )

set(COVREACH_ASYNC_ZG_EXTRAPOLATIONS
    extraLU+l
    )

set(COVREACH_ZG_COVERS
    inclusion
    aLUg
    aLUl
    aMg
    aMl
    )
set(COVREACH_ASYNC_ZG_COVERS
    inclusion
    )

file(RELATIVE_PATH here ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Generating '${here}' tests. This may take a long time.")
tck_register_testcases("covreach-" CHECK_TESTCASES_ savelist COVREACH_INPUT_FILES ${INPUTS})

set(nb_tests 0)
foreach (inputfile ${COVREACH_INPUT_FILES})
    get_filename_component(testname ${inputfile} NAME_WE)

    foreach (zonegraph ${COVREACH_ZONE_GRAPHS})
        if (zonegraph STREQUAL "zg")
            set(EXTRAPOLATIONS ${COVREACH_ZG_EXTRAPOLATIONS})
            set(COVERS ${COVREACH_ZG_COVERS})
        else ()
            set(EXTRAPOLATIONS ${COVREACH_ASYNC_ZG_EXTRAPOLATIONS})
            set(COVERS ${COVREACH_ASYNC_ZG_COVERS})
        endif ()
        foreach (cover ${COVERS})
            foreach (extra ${EXTRAPOLATIONS})
                foreach (sem ${COVREACH_SEMANTICS})
                    set(model "${zonegraph}:${sem}:${extra}")
                    string(REPLACE ":" "_" testname_model "${model}")

                    foreach (outformat ${COVREACH_OUTPUT_FORMATS})
                        foreach (so ${COVREACH_SEARCH_ORDERS})
                            set(TEST_NAME "${testname}_${cover}_${testname_model}_${so}_${outformat}")
                            tck_filter_testcase(accepted ${TEST_NAME} ACCEPT_TEST_REGEX REJECT_TEST_REGEX)
                            if(NOT accepted)
                                continue()
                            endif()

                            tck_add_test (${TEST_NAME} ${TEST_NAME} savelist)

                            set_tests_properties(${TEST_NAME}
                                                 PROPERTIES FIXTURES_REQUIRED "BUILD_TCHECKER;CHECK_TESTCASES_${testname}")

                            tck_add_test_envvar(testenv TEST "${PARSE_LABELS_SH}")
                            tck_add_test_envvar(testenv TEST_ARGS "${TCHECKER} covreach -c ${cover} -f ${outformat} -m ${model} -s ${so} -S ${labels} ${inputfile}")
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
                                                 PROPERTIES FIXTURES_REQUIRED "BUILD_TCHECKER;CHECK_TESTCASES_${testname}")

                            tck_add_test_envvar(testenv IS_MEMCHECK_TEST "yes")
                            tck_add_test_envvar(testenv TEST "${VALGRIND_PROGRAM}")
                            tck_add_test_envvar(testenv TEST_ARGS "${VALGRIND_OPTIONS} ${TCHECKER} covreach -c ${cover} -f ${outformat} -m ${model} -s ${so} -S ${labels} ${inputfile}")
                            tck_set_test_env(${TEST_NAME} testenv)
                            unset(testenv)

                            math(EXPR nb_tests "${nb_tests}+1")
                        endforeach ()
                    endforeach ()
                endforeach ()
            endforeach ()
        endforeach ()
    endforeach ()
endforeach ()

message(STATUS "${nb_tests} generated tests in ${here}.")

tck_add_savelist(save-covreach ${savelist})
