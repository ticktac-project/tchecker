# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

option(TCK_ENABLE_EXPLORE_TESTS "enable tests related to explore" ON)

if(NOT TCK_ENABLE_EXPLORE_TESTS)
    message(STATUS "Explore tests are disabled.")
    return()
endif()

set(INPUTS
    ad94.sh:
    critical-region.sh:2:5
    critical-region-async.sh:2:10
    csmacd.sh:3
    fischer.sh:3:10
    fischer-async.sh:3:10
    gps-mc.sh:2:2:100:150
    parallel.sh:5
    parallel-b.sh:3
    parallel-c.sh:3
    train_gate.sh:3
    )

set(ACCEPT_TEST_REGEX
    "^.*ad94.*_extra.*$"
    "^.*gps-mc_2.*_extra.*$"
    "^.*parallel_5.*_extra.*$")

set(REJECT_TEST_REGEX
    "^.*NOextra.*$"
    "^.*async_zg_.*elapsed.*$"
    "^.*parallel-[bc]_3_zg.*extraM[+]g.*$"
    "^.*parallel-[bc]_3_zg.*extraMg.*$"
    "^.*parallel-[bc]_3_zg.*extraLUg.*$"
    "^.*job-shop_3_5.*_zg.*$"
    "^.*gps-mc_3_2.*_zg.*$"
    )

set(EXPLORE_OUTPUT_FORMATS raw dot)

set(EXPLORE_SEARCH_ORDERS dfs bfs)

set(EXPLORE_SEMANTICS elapsed non-elapsed)

set(EXPLORE_ZG_EXTRAPOLATIONS
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

set(EXPLORE_MODELS ta)

foreach(sem ${EXPLORE_SEMANTICS})
    foreach(extra ${EXPLORE_ZG_EXTRAPOLATIONS})
        list(APPEND EXPLORE_MODELS "zg:${sem}:${extra}")
    endforeach()
    list(APPEND EXPLORE_MODELS "async_zg:${sem}")
endforeach()

file(RELATIVE_PATH here ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Generating '${here}' tests. This may take a long time.")
tck_register_testcases("explore-" CHECK_TESTCASES_ savelist EXPLORE_INPUT_FILES ${INPUTS})

set(nb_tests 0)
foreach (inputfile ${EXPLORE_INPUT_FILES})
    get_filename_component(testname ${inputfile} NAME_WE)

    foreach(model ${EXPLORE_MODELS})
        string(REPLACE ":" "_" testname_model "${model}")

        foreach (outformat ${EXPLORE_OUTPUT_FORMATS})
            foreach (so ${EXPLORE_SEARCH_ORDERS})
                set(TEST_NAME "${testname}_${testname_model}_${so}_${outformat}")
                tck_filter_testcase(accepted ${TEST_NAME} ACCEPT_TEST_REGEX REJECT_TEST_REGEX)
                if(NOT accepted)
                    continue()
                endif()

                tck_add_test (${TEST_NAME} ${TEST_NAME} savelist)

                set_tests_properties(${TEST_NAME} PROPERTIES FIXTURES_REQUIRED "BUILD_TCHECKER;CHECK_TESTCASES_${testname}")

                tck_add_test_envvar(testenv TEST "${TCHECKER}")
                tck_add_test_envvar(testenv TEST_ARGS "explore -f ${outformat} -m ${model} -s ${so} ${inputfile}")
                tck_set_test_env(${TEST_NAME} testenv)
                unset(testenv)
                math(EXPR nb_tests "${nb_tests}+1")
            endforeach ()
        endforeach ()
    endforeach ()
endforeach ()

message(STATUS "${nb_tests} generated tests in ${here}.")

tck_add_savelist(save-explore ${savelist})
