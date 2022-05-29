# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

find_package(Boost REQUIRED)

option(LIBTCHECKER_ENABLE_SHARED "Build TChecker shared library" OFF)

message(STATUS "Build type for tchecker: ${CMAKE_BUILD_TYPE}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(CMAKE_MACOSX_RPATH 1)

tck_check_cxx_flags("-ferror-limit=150" ERROR_LIMIT)
tck_check_cxx_flags("-fstrict-vtable-pointers" STRICT_VTABLE_POINTERS)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 ${ERROR_LIMIT}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O2 -flto ${STRICT_VTABLE_POINTERS}")

set(TCHECKER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include")
set(TCHECKER_BINARY_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/../include")

include_directories(${TCHECKER_INCLUDE_DIR})
include_directories(${TCHECKER_BINARY_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(algorithms)
add_subdirectory(clockbounds)
add_subdirectory(dbm)
add_subdirectory(expression)
add_subdirectory(graph)
add_subdirectory(parsing)
add_subdirectory(refzg)
add_subdirectory(statement)
add_subdirectory(syncprod)
add_subdirectory(system)
add_subdirectory(ta)
add_subdirectory(ts)
add_subdirectory(utils)
add_subdirectory(variables)
add_subdirectory(vm)
add_subdirectory(waiting)
add_subdirectory(zg)

set(OTHER_SRC
${CMAKE_CURRENT_SOURCE_DIR}/basictypes.cc
${TCHECKER_INCLUDE_DIR}/tchecker/basictypes.hh
)

set(LIBTCHECKER_SRC
${ALGORITHMS_SRC}
${CLOCKBOUNDS_SRC}
${DBM_SRC}
${EXPRESSION_SRC}
${GRAPH_SRC}
${OTHER_SRC}
${PARSING_SRC}
${REFZG_SRC}
${STATEMENT_SRC}
${SYNCPROD_SRC}
${SYSTEM_SRC}
${TA_SRC}
${TS_SRC}
${UTILS_SRC}
${VARIABLES_SRC}
${WAITING_SRC}
${VM_SRC}
${ZG_SRC})


# Build TChecker static library
add_library(libtchecker_static STATIC ${LIBTCHECKER_SRC}
            $<TARGET_OBJECTS:program_parsing_static>
            $<TARGET_OBJECTS:system_parsing_static>)
set_property(TARGET libtchecker_static PROPERTY OUTPUT_NAME tchecker)
set_property(TARGET libtchecker_static PROPERTY CXX_STANDARD 17)
set_property(TARGET libtchecker_static PROPERTY CXX_STANDARD_REQUIRED ON)


# Build TChecker shared library if required
if(LIBTCHECKER_ENABLE_SHARED)
  add_library(libtchecker_shared SHARED ${LIBTCHECKER_SRC}
              $<TARGET_OBJECTS:program_parsing_shared>
              $<TARGET_OBJECTS:system_parsing_shared>)
  set_property(TARGET libtchecker_shared PROPERTY OUTPUT_NAME tchecker)
  set_property(TARGET libtchecker_shared PROPERTY CXX_STANDARD 17)
  set_property(TARGET libtchecker_shared PROPERTY CXX_STANDARD_REQUIRED ON)
  if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Linux")
    set_property(TARGET libtchecker_shared PROPERTY POSITION_INDEPENDENT_CODE 1)
  endif()
endif()

# Build tck-syntax executable
add_executable(tck-syntax ${CMAKE_CURRENT_SOURCE_DIR}/tck-syntax/tck-syntax.cc)
target_link_libraries(tck-syntax libtchecker_static)
set_property(TARGET tck-syntax PROPERTY CXX_STANDARD 17)
set_property(TARGET tck-syntax PROPERTY CXX_STANDARD_REQUIRED ON)

# Build tck-reach executable
add_executable(tck-reach
               ${CMAKE_CURRENT_SOURCE_DIR}/tck-reach/concur19.hh
               ${CMAKE_CURRENT_SOURCE_DIR}/tck-reach/concur19.cc
               ${CMAKE_CURRENT_SOURCE_DIR}/tck-reach/tck-reach.cc
               ${CMAKE_CURRENT_SOURCE_DIR}/tck-reach/zg-covreach.cc
               ${CMAKE_CURRENT_SOURCE_DIR}/tck-reach/zg-covreach.hh
               ${CMAKE_CURRENT_SOURCE_DIR}/tck-reach/zg-reach.cc
               ${CMAKE_CURRENT_SOURCE_DIR}/tck-reach/zg-reach.hh)
target_link_libraries(tck-reach libtchecker_static)
set_property(TARGET tck-reach PROPERTY CXX_STANDARD 17)
set_property(TARGET tck-reach PROPERTY CXX_STANDARD_REQUIRED ON)

# Build tck-simulate executable
add_executable(tck-simulate
               ${CMAKE_CURRENT_SOURCE_DIR}/tck-simulate/graph.hh
               ${CMAKE_CURRENT_SOURCE_DIR}/tck-simulate/graph.cc
               ${CMAKE_CURRENT_SOURCE_DIR}/tck-simulate/simulate.hh
               ${CMAKE_CURRENT_SOURCE_DIR}/tck-simulate/simulate.cc
               ${CMAKE_CURRENT_SOURCE_DIR}/tck-simulate/tck-simulate.cc)
target_link_libraries(tck-simulate libtchecker_static)
set_property(TARGET tck-simulate PROPERTY CXX_STANDARD 17)
set_property(TARGET tck-simulate PROPERTY CXX_STANDARD_REQUIRED ON)

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
install(TARGETS tck-reach tck-syntax libtchecker_static
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib)

if(LIBTCHECKER_ENABLE_SHARED)
  install(TARGETS libtchecker_shared LIBRARY DESTINATION lib)
endif()

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
