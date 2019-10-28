# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(LIBTESTUTILS_SRC
    utils.cc utils.hh)


# Build TChecker static library
add_library(testutils STATIC ${LIBTESTUTILS_SRC})

set_property(TARGET testutils PROPERTY CXX_STANDARD 17)
set_property(TARGET testutils PROPERTY CXX_STANDARD_REQUIRED ON)
