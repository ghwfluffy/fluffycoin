# fluffylib(
#    libname
#    PUBLIC <dependencies>
#    PRIVATE <dependencies>
# )
macro(fluffylib)
    # Parse input
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PUBLIC PRIVATE)
    cmake_parse_arguments(FLUFFYLIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(FLUFFYLIB_NAME ${FLUFFYLIB_UNPARSED_ARGUMENTS})

    # Find all source files
    file(
        GLOB_RECURSE
        SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}
        REGEX *.cpp
    )

    # Find all header files
    file(
        GLOB_RECURSE
        ALL_HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}
        REGEX *.h
    )

    # Split between private and not private
    set(HEADER_FILES ${ALL_HEADER_FILES})
    list(FILTER HEADER_FILES EXCLUDE REGEX "priv")

    set(PRIV_HEADER_FILES ${ALL_HEADER_FILES})
    list(FILTER PRIV_HEADER_FILES INCLUDE REGEX "priv")

    # Stage header files with fluffycoin/lib/ prefix
    set(HEADER_BASEDIR ${CMAKE_CURRENT_BINARY_DIR}/dist/include/)
    set(HEADER_DIR ${HEADER_BASEDIR}/fluffycoin/${FLUFFYLIB_NAME})
    set(HEADER_TIMESTAMP ${CMAKE_CURRENT_BINARY_DIR}/.${FLUFFYLIB_NAME}.headers.timestamp)
    add_custom_command(
        OUTPUT
            ${HEADER_TIMESTAMP}
        COMMAND
            rm -rf ${HEADER_DIR}
        COMMAND
            mkdir -p ${HEADER_DIR}
        COMMAND
            ln -s ${HEADER_FILES} ${HEADER_DIR}/
        COMMAND
            touch ${HEADER_TIMESTAMP}
        DEPENDS
            ${HEADER_FILES}
        COMMENT
            "Setting up fluffycoin-${FLUFFYLIB_NAME} headers"
    )

    # Stage private headers too
    if (PRIV_HEADER_FILES)
        set(PRIV_HEADER_DIR ${HEADER_DIR}/priv)
        set(PRIV_HEADER_TIMESTAMP ${CMAKE_CURRENT_BINARY_DIR}/.${FLUFFYLIB_NAME}.priv.headers.timestamp)
        add_custom_command(
            OUTPUT
                ${PRIV_HEADER_TIMESTAMP}
            COMMAND
                rm -rf ${PRIV_HEADER_DIR}
            COMMAND
                mkdir -p ${PRIV_HEADER_DIR}
            COMMAND
                ln -s ${PRIV_HEADER_FILES} ${PRIV_HEADER_DIR}/
            COMMAND
                touch ${PRIV_HEADER_TIMESTAMP}
            DEPENDS
                ${PRIV_HEADER_FILES}
            COMMENT
                "Setting up fluffycoin-${FLUFFYLIB_NAME} private headers"
        )
        set(HEADER_TIMESTAMP ${HEADER_TIMESTAMP} ${PRIV_HEADER_TIMESTAMP})
    endif()

    set_source_files_properties(
        ${HEADER_TIMESTAMP}
        PROPERTIES GENERATED TRUE)
    add_custom_target(
        fluffycoin-${FLUFFYLIB_NAME}-headers
        DEPENDS
            ${HEADER_TIMESTAMP}
    )

    # Library target
    add_library(
        fluffycoin-${FLUFFYLIB_NAME}
        ${SOURCE_FILES}
    )

    # Our headers
    add_dependencies(
        fluffycoin-${FLUFFYLIB_NAME}
        fluffycoin-${FLUFFYLIB_NAME}-headers
    )
    target_include_directories(
        fluffycoin-${FLUFFYLIB_NAME}
        PUBLIC
            ${HEADER_BASEDIR}
    )

    # Our link dependencies
    target_link_libraries(
        fluffycoin-${FLUFFYLIB_NAME}
        PUBLIC
            ${FLUFFYLIB_PUBLIC}
        PRIVATE
            ${FLUFFYLIB_PRIVATE}
    )
endmacro()
