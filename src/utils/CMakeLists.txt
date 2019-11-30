# This file is a part of the TChecker project.
#
# See files AUTHORS and LICENSE for copyright details.

set(UTILS_SRC
${CMAKE_CURRENT_SOURCE_DIR}/gc.cc
${CMAKE_CURRENT_SOURCE_DIR}/log.cc
${TCHECKER_INCLUDE_DIR}/tchecker/utils/allocation_size.hh
${TCHECKER_INCLUDE_DIR}/tchecker/utils/array.hh
${TCHECKER_INCLUDE_DIR}/tchecker/utils/cache.hh
${TCHECKER_INCLUDE_DIR}/tchecker/utils/gc.hh
${TCHECKER_INCLUDE_DIR}/tchecker/utils/index.hh
${TCHECKER_INCLUDE_DIR}/tchecker/utils/iterator.hh
${TCHECKER_INCLUDE_DIR}/tchecker/utils/log.hh
${TCHECKER_INCLUDE_DIR}/tchecker/utils/ordering.hh
${TCHECKER_INCLUDE_DIR}/tchecker/utils/pool.hh
${TCHECKER_INCLUDE_DIR}/tchecker/utils/shared_objects.hh
${TCHECKER_INCLUDE_DIR}/tchecker/utils/singleton_pool.hh
${TCHECKER_INCLUDE_DIR}/tchecker/utils/spinlock.hh
${TCHECKER_INCLUDE_DIR}/tchecker/utils/waiting.hh
PARENT_SCOPE)
