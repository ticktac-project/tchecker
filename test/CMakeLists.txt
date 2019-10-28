# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

include(${CMAKE_CURRENT_SOURCE_DIR}/TestUtils.cmake)

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


set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "Testing")

set(save_commands "")
foreach(dir unit-tests bugfixes simple-nr covreach explore)
    add_subdirectory(${dir})
    if(TARGET save-${dir})
        list(APPEND save_commands
             COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/${dir} --target save-${dir} --config "$<CONFIG>")
    endif()
    set_property(DIRECTORY ${dir}
                 APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "Testing")
endforeach()

add_custom_target(save
                  ${save_commands}
                  COMMENT "Recursive 'save' target"
                  )
