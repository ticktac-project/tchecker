#
# This macro transmits the value of some variables to the parent scope.
# Arguments of the macro are name of variables. We assume that, for each
# name of variable, say 'x', passed as argument, there exists in the scope
# of the caller, another variable called 'xvar' and that contains the name
# of a variable to assign in the parent scope of the caller with the value of
# 'x'.
#
macro(tck_set_parent_scope)
    foreach (var ${ARGN})
        set(${${var}var} ${${var}} PARENT_SCOPE)
    endforeach ()
endmacro(tck_set_parent_scope)

#
# This function parse a string that describes a test-case with its arguments.
# 'test_spec' is a colon-separated list of strings. The head of the  list a
# a file name (assumed to be a shell script used to generate a model) and the
# tail of the list contains arguments passed to the script.
# The function returns into:
# - 'testnamevar' an identifier for the test to be used within add_test()
# - 'testscriptvar' the script file of the generator
# - 'testargsvar' the arguments to be passed to the script
#
function(tck_parse_test_spec
         test_spec testnamevar testscriptvar testargsvar)
    # transform the spec into a ;-list
    string(REPLACE ":" ";" test_spec ${test_spec})

    # pop the shell script generator
    list(GET test_spec 0 testscript)
    list(REMOVE_AT test_spec 0)

    # remove the extension of the file to get a cleaner test name
    get_filename_component(testname ${testscript} NAME_WE)

    # build a space-separated list of parameters passed to the script and
    # add parameters to test name.
    set(testargs "")
    foreach (arg IN LISTS test_spec)
        set(testargs "${testargs} ${arg}")
        set(testname "${testname}_${arg}")
    endforeach ()

    tck_set_parent_scope(testscript testname testargs)
endfunction(tck_parse_test_spec)

#
# This function assign ${value} to variable ${var} and append the list of
# variables ${varlistname} the identifier ${var}
# This list is then used to setup the environment of a test.
#
function(tck_add_test_envvar varlistname var value)
    set(${var} ${value} PARENT_SCOPE)
    list(APPEND ${varlistname} ${var})
    set(${varlistname} ${${varlistname}} PARENT_SCOPE)
endfunction(tck_add_test_envvar)

#
# This function extends the set of environment variables of a test ${testname}
# with the variable in the list ${varlist}. 'varlist' is the identifier of list
# not its content.
#
function(tck_set_test_env testname varlist)
    foreach (var IN LISTS ${varlist})
        set_property(TEST ${testname} APPEND PROPERTY ENVIRONMENT
                     "${var}=${${var}}")
    endforeach (var)
endfunction(tck_set_test_env)

function(tck_add_test testname filenamebase slistvar)
    if (NOT TEST_DRIVER)
        message(FATAL_ERROR "TEST_DRIVER variable is not defined.")
    endif ()

    tck_add_test_envvar(testenv INPUT_DIR
                        "${CMAKE_CURRENT_SOURCE_DIR}")
    tck_add_test_envvar(testenv OUTPUT_DIR
                        "${CMAKE_CURRENT_BINARY_DIR}")
    tck_add_test_envvar(testenv OUTPUT_FILE
                        "${OUTPUT_DIR}/${filenamebase}.out")
    tck_add_test_envvar(testenv ERROR_FILE
                        "${OUTPUT_DIR}/${filenamebase}.err")
    tck_add_test_envvar(testenv EXPECTED_OUTPUT_FILE
                        "${INPUT_DIR}/${filenamebase}.out-expected")
    tck_add_test_envvar(testenv EXPECTED_ERROR_FILE
                        "${INPUT_DIR}/${filenamebase}.err-expected")
    add_test(NAME ${testname}
             COMMAND ${SHELL} ${TEST_DRIVER}
             WORKING_DIRECTORY "${OUTPUT_DIR}")
    tck_set_test_env(${testname} testenv)
    unset(testenv)
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
                 "${OUTPUT_FILE}" "${ERROR_FILE}" ${${testname}_CLEANFILES})

    list(APPEND ${slistvar}
         ${OUTPUT_FILE} ${EXPECTED_OUTPUT_FILE}
         ${ERROR_FILE} ${EXPECTED_ERROR_FILE})

    set(${slistvar} ${${slistvar}} PARENT_SCOPE)
endfunction(tck_add_test)

function(tck_add_savelist targetname)
    if (NOT ARGN)
        add_custom_target(${targetname}
                          ${CMAKE_COMMAND} -E echo "There is nothing to save here.")
        return()
    endif ()

    message(STATUS "Writing save list")

    set(SAVE_FILE "${CMAKE_CURRENT_BINARY_DIR}/save_tests.txt")
    file(RELATIVE_PATH here ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    add_custom_target(${targetname}
                      ${SAVE_RESULTS} "${SAVE_FILE}"
                      COMMENT "Saving results as expected ones from ${here}")

    file(WRITE ${SAVE_FILE} "")
    list(LENGTH ARGN slistlen)
    set(i 0)
    while (i LESS slistlen)
        list(GET ARGN ${i} src)
        math(EXPR next "${i}+1")
        list(GET ARGN ${next} dst)
        file(APPEND "${SAVE_FILE}" "${src}:${dst}\n")
        math(EXPR i "${i}+2")
    endwhile ()
endfunction(tck_add_savelist)

function(tck_register_testcases prefix fsetup savelist outputfiles)
    if (NOT ARGN)
        return()
    endif ()

    set(slist "")
    list(LENGTH ARGN slistlen)
    foreach (testcase ${ARGN})
        tck_parse_test_spec(${testcase} testname testscript testarguments)
        set(testname "${prefix}${testname}")
        tck_add_test(${testname} ${testname} slist)

        tck_add_test_envvar(testenv TEST "${TCK_EXAMPLES_DIR}/${testscript}")
        tck_add_test_envvar(testenv TEST_ARGS "${testarguments}")
        tck_set_test_env(${testname} testenv)
        set_tests_properties(${testname} PROPERTIES FIXTURES_SETUP "${fsetup}${testname}")

        unset(testenv)
    endforeach ()

    set(ofiles "${slist}")
    list(FILTER ofiles INCLUDE REGEX ".*[.]out\$")

    list(APPEND ${savelist} ${slist})
    list(APPEND ${outputfiles} ${ofiles})
    set(${savelist} ${${savelist}} PARENT_SCOPE)
    set(${outputfiles} ${${outputfiles}} PARENT_SCOPE)
    file(RELATIVE_PATH here ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    message(STATUS "${slistlen} test-cases in ${here}.")
endfunction()

function(tck_filter_testcase result test_name re_acceptlist re_rejectlist)
    set(${result} TRUE PARENT_SCOPE)
    foreach (filter IN LISTS ${re_acceptlist})
        string(REGEX MATCH ${filter} match ${test_name})
        if (match)
            return()
        endif ()
    endforeach ()

    set(${result} FALSE PARENT_SCOPE)
    foreach (filter IN LISTS ${re_rejectlist})
        string(REGEX MATCH ${filter} match ${test_name})
        if (match)
            return()
        endif ()
    endforeach ()

    set(${result} TRUE PARENT_SCOPE)
endfunction()

function(add_testbin_target_ target srcfile)
    if (TARGET ${target})
        return()
    endif ()

    add_executable(${target} ${srcfile})
    target_link_libraries(${target} testutils)
    target_link_libraries(${target} libtchecker_static)
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)
    set_property(TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED ON)

    # Add test invoking the build of the test executable
    add_test(NAME ${target}-build COMMAND ${CMAKE_COMMAND}
             --build "${CMAKE_BINARY_DIR}"
             --config "$<CONFIG>"
             --target ${target})
    set_tests_properties(${target}-build PROPERTIES
                         FIXTURES_SETUP BUILD_${target})
endfunction(add_testbin_target_)


function(tck_add_nr_test testfile testnamevar slist)
    if(DEFINED ${slist})
        set(testlist "${${slist}}")
    else()
        set(testlist "")
    endif()
    get_filename_component(ext ${testfile} EXT)
    get_filename_component(filename ${testfile} NAME_WE)
    set(testname ${filename})

    tck_add_test(${testname} ${testname} testlist)
    tck_add_test_envvar(testenv SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}")
    tck_add_test_envvar(testenv TCK_EXAMPLES_DIR "${TCK_EXAMPLES_DIR}")
    tck_add_test_envvar(testenv TCHECKER "${TCHECKER}")

    if (ext STREQUAL ".cc")
        # Add target to build binary for this test
        set(TESTBIN "${filename}-bin")
        add_testbin_target_(${TESTBIN} ${testfile})
        set_tests_properties(${testname} PROPERTIES FIXTURES_REQUIRED BUILD_${TESTBIN})
        tck_add_test_envvar(testenv TEST "$<TARGET_FILE:${TESTBIN}>")
    elseif (ext STREQUAL ".sh")
        tck_add_test_envvar(testenv TEST "${CMAKE_CURRENT_SOURCE_DIR}/${testfile}")
    elseif (ext STREQUAL ".tck")
        tck_add_test_envvar(testenv TEST "${TCHECKER}")
        tck_add_test_envvar(testenv TEST_ARGS "explore -f raw -m ta ${CMAKE_CURRENT_SOURCE_DIR}/${testfile}")
    else ()
        message(FATAL_ERROR "Don't know what kind of test is ${testfile}.")
    endif ()
    set_property(TEST ${testname} APPEND PROPERTY FIXTURES_REQUIRED BUILD_TCHECKER)

    tck_set_test_env(${testname} testenv)
    unset(testenv)
    set(${testnamevar} ${testname} PARENT_SCOPE)
    set(${slist} ${testlist} PARENT_SCOPE)
endfunction()

function(tck_add_nr_memcheck_test testfile testnamevar slist)
    if (NOT TCK_ENABLE_MEMCHECK_TESTS)
        return()
    endif ()
    if(DEFINED ${slist})
        set(testlist "${${slist}}")
    else()
        set(testlist "")
    endif()
    get_filename_component(ext ${testfile} EXT)
    get_filename_component(filename ${testfile} NAME_WE)
    set(testname memcheck-${filename})

    tck_add_test(${testname} ${testname} testlist)
    tck_add_test_envvar(testenv SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}")
    tck_add_test_envvar(testenv TCK_EXAMPLES_DIR "${TCK_EXAMPLES_DIR}")
    tck_add_test_envvar(testenv TCHECKER "${VALGRIND_PROGRAM} ${VALGRIND_OPTIONS} ${TCHECKER}")
    tck_add_test_envvar(testenv IS_MEMCHECK_TEST "yes")

    if (ext STREQUAL ".cc")
        # Add target to build binary for this test
        set(TESTBIN "${filename}-bin")
        add_testbin_target_(${TESTBIN} ${testfile})
        set_tests_properties(${testname} PROPERTIES FIXTURES_REQUIRED BUILD_${TESTBIN})
        tck_add_test_envvar(testenv TEST "${VALGRIND_PROGRAM}")
        tck_add_test_envvar(testenv TEST_ARGS "${VALGRIND_OPTIONS} $<TARGET_FILE:${TESTBIN}>")
    elseif (ext STREQUAL ".sh")
        tck_add_test_envvar(testenv TEST "${CMAKE_CURRENT_SOURCE_DIR}/${testfile}")
    elseif (ext STREQUAL ".tck")
        tck_add_test_envvar(testenv TEST "${TCHECKER}")
        tck_add_test_envvar(testenv TEST_ARGS "explore -f raw -m ta ${CMAKE_CURRENT_SOURCE_DIR}/${testfile}")
    else ()
        message(FATAL_ERROR "Don't know what kind of test is ${testfile}.")
    endif ()
    set_property(TEST ${testname} APPEND PROPERTY FIXTURES_REQUIRED BUILD_TCHECKER)

    tck_set_test_env(${testname} testenv)
    unset(testenv)
    set(${testnamevar} ${testname} PARENT_SCOPE)
    set(${slist} ${testlist} PARENT_SCOPE)
endfunction()

option(TCK_ENABLE_MEMCHECK_TESTS "enable tests related to bug fixes" OFF)
find_program(VALGRIND_PROGRAM NAMES valgrind)

if (VALGRIND_PROGRAM)
    set(VALGRIND_FOUND TRUE)
    string(CONCAT VALGRIND_OPTIONS
           "--quiet "
           "--tool=memcheck "
           "--leak-check=full "
           "--num-callers=20 "
           "--trace-children=yes "
           "--error-exitcode=123 ")
else ()
    message(STATUS "Valgrind not found.")
    set(VALGRIND_NOTFOUND FALSE)
    set(TCK_ENABLE_MEMCHECK_TESTS OFF)
endif ()

if(NOT TCK_ENABLE_MEMCHECK_TESTS)
    message(STATUS "Memcheck tests are disabled.")
endif()
