set(TCHECKER_CONFIG_HH_IN "${CMAKE_CURRENT_SOURCE_DIR}/tchecker/config.hh.in")
set(TCHECKER_CONFIG_HH "${CMAKE_CURRENT_BINARY_DIR}/tchecker/config.hh")

configure_file(${TCHECKER_CONFIG_HH_IN} ${TCHECKER_CONFIG_HH} @ONLY)

install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/"
    DESTINATION "include"
    FILES_MATCHING
    PATTERN "*.hh"
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
    DESTINATION "include"
    FILES_MATCHING
    PATTERN "*.hh"
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}/tchecker")
