# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

cmake_minimum_required(VERSION 2.8)
project(tchecker)

include("${CMAKE_SOURCE_DIR}/cmake/CheckCompilerFlags.cmake")

# The project version number.
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   1   CACHE STRING "Project minor version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR)

add_subdirectory(src)
add_subdirectory(test)
