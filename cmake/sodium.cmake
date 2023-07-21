set(LIBSODIUM_VERSION 23.3.0)
set(LIBSODIUM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/libsodium)
set(LIBSODIUM_LIBDIR ${LIBSODIUM_DIR}/src/libsodium/.libs)
set(LIBSODIUM_LIB_BASE ${LIBSODIUM_LIBDIR}/libsodium.so)
set(LIBSODIUM_LIB ${LIBSODIUM_LIB_BASE}.${LIBSODIUM_VERSION})
set(LIBSODIUM_INC ${LIBSODIUM_DIR}/src/libsodium/include)
set(LIBSODIUM_EMPTY ${CMAKE_CURRENT_BINARY_DIR}/blank-sodium.c)

add_custom_command(
    OUTPUT
        ${LIBSODIUM_LIB}
        ${LIBSODIUM_EMPTY}
    WORKING_DIRECTORY
        ${LIBSODIUM_DIR}
    COMMAND
        ./autogen.sh
    COMMAND
        ./configure
    COMMAND
        make -j12
    COMMAND
        mkdir -p ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND
        cp -P ${LIBSODIUM_LIB_BASE}* ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND
        touch ${LIBSODIUM_EMPTY}
    COMMENT
        "Building libsodium ${LIBSODIUM_VERSION}"
)

set_source_files_properties(
    ${LIBSODIUM_LIB}
    ${LIBSODIUM_EMPTY}
    PROPERTIES GENERATED TRUE
)

add_custom_target(
    libsodium-build
    DEPENDS
        ${LIBSODIUM_LIB}
)

add_library(
    libsodium
    STATIC
        ${LIBSODIUM_EMPTY}
)

target_include_directories(
    libsodium
    PUBLIC
        ${LIBSODIUM_INC}
)

target_link_libraries(
    libsodium
    PUBLIC
        ${LIBSODIUM_LIB}
)

add_dependencies(
    libsodium
    libsodium-build
)
