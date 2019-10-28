# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

option(TCK_ENABLE_UNITTESTS "enable unit tests" ON)

if(NOT TCK_ENABLE_UNITTESTS)
    message(STATUS "Unit tests are disabled.")
    return()
endif()

if (NOT DEFINED Catch2_CONFIG)
    message(WARNING "Package Catch2 not found. Unit tests will be ignored")
    return()
endif ()

set(TCHECKER_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${TCHECKER_TEST_DIR})

set(TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/test-db.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-dbm.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-guard_weak_sync.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/test-offset_dbm.hh
    ${CMAKE_CURRENT_SOURCE_DIR}/unittest.cc
    )

add_executable(unittest ${TEST_SRC})
target_link_libraries(unittest testutils)
target_link_libraries(unittest libtchecker_static)
target_link_libraries(unittest Catch2::Catch2)

set_property(TARGET unittest PROPERTY CXX_STANDARD 17)
set_property(TARGET unittest PROPERTY CXX_STANDARD_REQUIRED ON)

foreach (FILE ${TEST_SRC})
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "/" GROUP "${PARENT_DIR}")
    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")
    # Put into group
    source_group("${GROUP}" FILES "${FILE}")
endforeach ()

add_test(NAME build-unittest
         COMMAND ${CMAKE_COMMAND}
         --build "${CMAKE_BINARY_DIR}"
         --config "$<CONFIG>"
         --target unittest
         )
set_tests_properties(build-unittest PROPERTIES FIXTURES_REQUIRED BUILD_TCHECKER)
set_tests_properties(build-unittest PROPERTIES FIXTURES_SETUP BUILD_UNITTEST)

include(Catch)
catch_discover_tests(unittest PROPERTIES FIXTURES_SETUP "BUILD_UNITTEST")
