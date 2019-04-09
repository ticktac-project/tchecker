# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

find_package(Boost REQUIRED)

tck_check_cxx_flags("-ferror-limit=150" ERROR_LIMIT)
tck_check_cxx_flags("-fstrict-vtable-pointers" STRICT_VTABLE_POINTERS)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g ${ERROR_LIMIT}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -DNDEBUG -O2 -flto ${STRICT_VTABLE_POINTERS}")

set(TCHECKER_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)

include_directories(${TCHECKER_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(algorithms)
add_subdirectory(async_zg)
add_subdirectory(clockbounds)
add_subdirectory(dbm)
add_subdirectory(expression)
add_subdirectory(flat_system)
add_subdirectory(fsm)
add_subdirectory(graph)
add_subdirectory(parsing)
add_subdirectory(statement)
add_subdirectory(system)
add_subdirectory(ta)
add_subdirectory(ts)
add_subdirectory(utils)
add_subdirectory(variables)
add_subdirectory(vm)
add_subdirectory(zg)
add_subdirectory(zone)

set(OTHER_SRC
${CMAKE_CURRENT_SOURCE_DIR}/basictypes.cc
${TCHECKER_INCLUDE_DIR}/tchecker/basictypes.hh
)

set(LIBTCHECKER_SRC
${ALGORITHMS_SRC}
${ASYNC_ZG_SRC}
${CLOCKBOUNDS_SRC}
${DBM_SRC}
${EXPRESSION_SRC}
${FSM_SRC}
${FLAT_SYSTEM_SRC}
${GRAPH_SRC}
${OTHER_SRC}
${PARSING_SRC}
${STATEMENT_SRC}
${SYNTAX_SRC}
${SYSTEM_SRC}
${TA_SRC}
${TS_SRC}
${UTILS_SRC}
${VARIABLES_SRC}
${VM_SRC}
${ZG_SRC}
${ZONE_SRC})

add_library(libtchecker STATIC ${LIBTCHECKER_SRC})
set_property(TARGET libtchecker PROPERTY CXX_STANDARD 17)
set_property(TARGET libtchecker PROPERTY CXX_STANDARD_REQUIRED ON)

add_executable(tchecker ${CMAKE_CURRENT_SOURCE_DIR}/tchecker/tchecker.cc)
target_link_libraries(tchecker libtchecker)
set_property(TARGET tchecker PROPERTY CXX_STANDARD 17)
set_property(TARGET tchecker PROPERTY CXX_STANDARD_REQUIRED ON)

# Project view in IDEs (Xcode, etc)
foreach(FILE ${LIBTCHECKER_SRC}) 
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
    # Remove common directory prefix to make the group
    if ("${FILE}" MATCHES ".*\\.cc")
      string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "/" GROUP "${PARENT_DIR}")
    else()
      string(REPLACE "${TCHECKER_INCLUDE_DIR}/tchecker" "/" GROUP "${PARENT_DIR}")
    endif()
    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")
    # Put into group
    source_group("${GROUP}" FILES "${FILE}")
endforeach()

# Install rule for binaries, lib and header files
install(TARGETS tchecker libtchecker
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

install(DIRECTORY ../include/ DESTINATION include)

# Doxygen documentation + install rule
find_package(Doxygen)

if(NOT DOXYGEN_FOUND)
    message(WARNING "Doxygen not found: API documentation not installed.")
else()
    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/../doc/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc/tchecker)
endif()
