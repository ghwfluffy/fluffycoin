macro(fluffytestlib)
    # Parse input
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PUBLIC PRIVATE)
    cmake_parse_arguments(FLUFFYLIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(FLUFFYLIB_NAME ${FLUFFYLIB_UNPARSED_ARGUMENTS})

    # Find all header files
    file(
        GLOB_RECURSE
        HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}
        REGEX *.h
    )

    # Stage header files with fluffycoin/lib/ prefix
    set(HEADER_BASEDIR ${CMAKE_CURRENT_BINARY_DIR}/dist/include/)
    set(HEADER_DIR ${HEADER_BASEDIR}/fluffytest/${FLUFFYLIB_NAME})
    set(HEADER_TIMESTAMP ${CMAKE_CURRENT_BINARY_DIR}/.${FLUFFYLIB_NAME}.headers.timestamp)
    add_custom_command(
        OUTPUT
            ${HEADER_TIMESTAMP}
        COMMAND
            rm -rf ${HEADER_DIR}
        COMMAND
            mkdir -p ${HEADER_DIR}
        COMMAND
            cp -v ${HEADER_FILES} ${HEADER_DIR}/
        COMMAND
            touch ${HEADER_TIMESTAMP}
        DEPENDS
            ${HEADER_FILES}
        COMMENT
            "Setting up fluffytest-${FLUFFYLIB_NAME} headers"
    )
    set_source_files_properties(
        ${HEADER_TIMESTAMP}
        PROPERTIES GENERATED TRUE)
    add_custom_target(
        fluffytest-${FLUFFYLIB_NAME}-headers
        DEPENDS
            ${HEADER_TIMESTAMP}
    )
    add_dependencies(
        fluffycoin-gtest
        fluffytest-${FLUFFYLIB_NAME}-headers
    )
    target_include_directories(
        fluffycoin-gtest
        PUBLIC
            ${HEADER_BASEDIR}
    )
endmacro()
