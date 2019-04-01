# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

add_subdirectory(covreach)
add_subdirectory(explore)

set(ALGORITHMS_SRC
${COVREACH_SRC}
${EXPLORE_SRC}
PARENT_SCOPE)
