# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

tck_check_cxx_flags("-ferror-limit=150" ERROR_LIMIT)
tck_check_cxx_flags("-fstrict-vtable-pointers" STRICT_VTABLE_POINTERS)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g ${ERROR_LIMIT}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O2 -flto ${STRICT_VTABLE_POINTERS}")

set(TCHECKER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(TCHECKER_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${TCHECKER_INCLUDE_DIR})
include_directories(${TCHECKER_TEST_DIR})
include_directories(${Boost_INCLUDE_DIRS})

set(TEST_SRC
${CMAKE_CURRENT_SOURCE_DIR}/test-db.hh
${CMAKE_CURRENT_SOURCE_DIR}/test-dbm.hh
${CMAKE_CURRENT_SOURCE_DIR}/test-extract_variables.hh
${CMAKE_CURRENT_SOURCE_DIR}/test-guard_weak_sync.hh
${CMAKE_CURRENT_SOURCE_DIR}/test-offset_clock_variables.hh
${CMAKE_CURRENT_SOURCE_DIR}/test-offset_dbm.hh
${CMAKE_CURRENT_SOURCE_DIR}/test-variables-access.hh
${CMAKE_CURRENT_SOURCE_DIR}/utils.hh
${CMAKE_CURRENT_SOURCE_DIR}/unittest.cc
${CMAKE_CURRENT_SOURCE_DIR}/utils.cc)

add_executable(unittest ${TEST_SRC})
target_link_libraries(unittest libtchecker_static)
target_link_libraries(unittest Catch2::Catch2)

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
catch_discover_tests(unittest)
