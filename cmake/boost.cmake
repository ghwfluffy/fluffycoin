set(BOOST_VERSION 1.28.0)
set(BOOST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/boost)
set(BOOST_EMPTY ${CMAKE_CURRENT_BINARY_DIR}/blank-boost.c)
set(BOOST_CONFIG ${BOOST_DIR}/boost/config.hpp)

add_custom_command(
    OUTPUT
        ${BOOST_CONFIG}
        ${BOOST_EMPTY}
    WORKING_DIRECTORY
        ${BOOST_DIR}
    COMMAND
        ./bootstrap.sh
    COMMAND
        ./b2 headers
    COMMAND
        touch ${BOOST_EMPTY}
    COMMENT
        "Building boost ${BOOST_VERSION}"
)

set_source_files_properties(
    ${BOOST_EMPTY}
    PROPERTIES GENERATED TRUE
)

add_custom_target(
    boost-build
    DEPENDS
        ${BOOST_CONFIG}
)

add_library(
    boost
    STATIC
        ${BOOST_EMPTY}
)

target_include_directories(
    boost
    SYSTEM
    PUBLIC
        ${BOOST_DIR}
)

add_dependencies(
    boost
    boost-build
)