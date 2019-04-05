# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

find_package(BOOST REQUIRED)
find_package(Catch2 REQUIRED)

tck_check_cxx_flags("-ferror-limit=150" ERROR_LIMIT)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g ${ERROR_LIMIT}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -DNDEBUG -O2 -flto -fstrict-vtable-pointers")

set(TCHECKER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)

include_directories(${TCHECKER_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

set(TEST_SRC
${CMAKE_CURRENT_SOURCE_DIR}/test-db.hh
${CMAKE_CURRENT_SOURCE_DIR}/test-dbm.hh
${CMAKE_CURRENT_SOURCE_DIR}/test-model.hh
${CMAKE_CURRENT_SOURCE_DIR}/test-offset_dbm.hh
${CMAKE_CURRENT_SOURCE_DIR}/unittest.cc)

add_executable(unittest ${TEST_SRC})
target_link_libraries(unittest libtchecker)
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
