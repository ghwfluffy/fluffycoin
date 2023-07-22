set(LIBZMQ_VERSION 5.2.4)
set(LIBZMQ_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/libzmq)
set(LIBZMQ_LIB_BASE ${LIBZMQ_DIR}/src/.libs/libzmq.so)
set(LIBZMQ_LIB ${LIBZMQ_LIB_BASE}.${LIBZMQ_VERSION})
set(LIBZMQ_INC ${LIBZMQ_DIR}/include)
set(LIBZMQ_EMPTY ${CMAKE_CURRENT_BINARY_DIR}/blank-zmq.c)

set(__LIBZMQ_CFLAGS "-I${LIBSODIUM_INC} -w")
set(__LIBZMQ_LDFLAGS "-L${LIBSODIUM_LIBDIR} -lsodium")
add_custom_command(
    OUTPUT
        ${LIBZMQ_LIB}
        ${LIBZMQ_EMPTY}
    WORKING_DIRECTORY
        ${LIBZMQ_DIR}
    COMMAND
        ./autogen.sh
    COMMAND
        ${CMAKE_COMMAND} -E env
            LIBS=${__LIBZMQ_LDFLAGS}
            CFLAGS=${__LIBZMQ_CFLAGS}
            CXXFLAGS=${__LIBZMQ_CFLAGS}
            sodium_LIBS=${__LIBZMQ_LDFLAGS}
            sodium_CFLAGS=${__LIBZMQ_CFLAGS}
        ./configure
            --with-libsodium=yes
            --enable-shared=yes
    COMMAND
        make -j12
    COMMAND
        mkdir -p ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND
        cp -P ${LIBZMQ_LIB_BASE}* ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    COMMAND
        touch ${LIBZMQ_EMPTY}
    DEPENDS
        libsodium
    COMMENT
        "Building libzmq ${LIBZMQ_VERSION}"
)

set_source_files_properties(
    ${LIBZMQ_LIB}
    ${LIBZMQ_EMPTY}
    PROPERTIES GENERATED TRUE
)

add_custom_target(
    libzmq-build
    DEPENDS
        ${LIBZMQ_LIB}
)

add_library(
    libzmq
    STATIC
        ${LIBZMQ_EMPTY}
)

target_include_directories(
    libzmq
    PUBLIC
        ${LIBZMQ_INC}
)

target_link_libraries(
    libzmq
    PUBLIC
        ${LIBZMQ_LIB}
        libsodium
)

add_dependencies(
    libzmq
    libzmq-build
)
