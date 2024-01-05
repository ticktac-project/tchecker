# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

option(TCK_ENABLE_UNITTESTS "enable unit tests" ON)

if(NOT TCK_ENABLE_UNITTESTS)
    message(STATUS "Unit tests are disabled.")
    return()
endif()

if(NOT DEFINED Catch2_CONFIG)
    message(WARNING "Package Catch2 not found. Unit tests will be ignored")
    return()
endif()

set(TCHECKER_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${TCHECKER_TEST_DIR})

set(TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/test-cache.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-clockbounds.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-clock_updates.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-clocks.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-db.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-dbm.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-delay_allowed.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-extract_variables.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-finite-path.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-from_string.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-guard_weak_sync.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-hashtable.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-labels.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-ordering.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-refdbm.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-reference_clock_variables.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-refzg-semantics.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-variables-access.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-zg-semantics.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-waiting.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/unittest.cc
)

add_executable(unittest ${TEST_SRC})
target_link_libraries(unittest testutils)
target_link_libraries(unittest libtchecker_static)
target_link_libraries(unittest Catch2::Catch2WithMain)

set_property(TARGET unittest PROPERTY CXX_STANDARD 17)
set_property(TARGET unittest PROPERTY CXX_STANDARD_REQUIRED ON)

foreach(FILE ${TEST_SRC})
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "/" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Put into group
    source_group("${GROUP}" FILES "${FILE}")
endforeach()

include(Catch)
catch_discover_tests(unittest PROPERTIES FIXTURES_SETUP "BUILD_UNITTEST")

if(TCK_ENABLE_MEMCHECK_TESTS)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    set(TESTED_EXECUTABLE "${CMAKE_CURRENT_BINARY_DIR}/unittest")
    configure_file(memcheck-unittest.hh.in memcheck-unittest.hh)
    add_executable(memcheck-unittest memcheck-unittest.cc memcheck-unittest.hh)
    add_dependencies(memcheck-unittest unittest)

    # A warning is emitted by 'catch_discover_tests' due to a strange usage of
    # the exit code of the target that is interpreted as the number of tests
    # supported by the executable. Fortunately this value is not used to produce
    # the test-suite.
    catch_discover_tests(memcheck-unittest TEST_SUFFIX " [MEMCHECKED]")
endif()
