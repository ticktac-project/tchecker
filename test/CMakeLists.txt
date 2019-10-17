# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

include(${CMAKE_CURRENT_SOURCE_DIR}/TestUtils.cmake)

set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS 1)

set(TCHECKER "$<TARGET_FILE:tchecker>")

set(TEST_DRIVER "${CMAKE_CURRENT_SOURCE_DIR}/test-driver.sh")
set(SAVE_RESULTS "${CMAKE_CURRENT_SOURCE_DIR}/save-results.sh")
set(TCK_EXAMPLES_DIR "${CMAKE_SOURCE_DIR}/examples")

#
# Common setup for all compiled tests
#
find_package(Boost REQUIRED)
find_package(Catch2 QUIET)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if (${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif ()

tck_check_cxx_flags("-ferror-limit=150" ERROR_LIMIT)
tck_check_cxx_flags("-fstrict-vtable-pointers" STRICT_VTABLE_POINTERS)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g ${ERROR_LIMIT}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O2 -flto ${STRICT_VTABLE_POINTERS}")

set(TCHECKER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
include_directories(${TCHECKER_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(testutils)

get_target_property(TESTUTILS_DIR testutils SOURCE_DIR)
include_directories(${TESTUTILS_DIR})

build_command(BUILD_TCHECKER_COMMAND
              CONFIGURATION "$<CONFIG>"
              TARGET tchecker)

add_test(NAME build-tchecker
         COMMAND ${CMAKE_COMMAND}
         --build "${CMAKE_BINARY_DIR}"
         --config "$<CONFIG>"
         --target tchecker
         )

set_tests_properties(build-tchecker PROPERTIES FIXTURES_SETUP BUILD_TCHECKER)

add_custom_target(save
                  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/covreach --target save --config "$<CONFIG>"
                  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/explore --target save --config "$<CONFIG>"
                  COMMENT "Recursive 'save' target"
                  )

set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "Testing")

foreach(dir unit-tests bugfixes simple-nr covreach explore)
    add_subdirectory(${dir})
    set_property(DIRECTORY ${dir}
                 APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "Testing")
endforeach()
