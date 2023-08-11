set(OZO_VERSION 0.1.x-abb0)
set(OZO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/ozo)
set(OZO_EMPTY ${CMAKE_CURRENT_BINARY_DIR}/blank-ozo.c)
set(OZO_INC ${OZO_DIR}/include)
set(YAMAIL_INC ${OZO_DIR}/contrib/resource_pool/include)

add_custom_command(
    OUTPUT
        ${OZO_EMPTY}
    COMMAND
        touch ${OZO_EMPTY}
    COMMENT
        "Building ozo ${OZO_VERSION}"
)

set_source_files_properties(
    ${OZO_EMPTY}
    PROPERTIES GENERATED TRUE
)

add_custom_target(
    ozo-build
    DEPENDS
        ${OZO_EMPTY}
)

add_library(
    ozo
    STATIC
        ${OZO_EMPTY}
)

target_include_directories(
    ozo
    SYSTEM
    PUBLIC
        ${OZO_INC}
        ${YAMAIL_INC}
)

add_dependencies(
    ozo
    ozo-build
)
