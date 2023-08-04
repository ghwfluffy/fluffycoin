# fluffyservice(
#    name
#    PUBLIC <dependencies>
#    PRIVATE <dependencies>
# )
macro(fluffyservice)
    # Parse input
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PUBLIC PRIVATE)
    cmake_parse_arguments(FLUFFYSVC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(FLUFFYSVC_NAME ${FLUFFYSVC_UNPARSED_ARGUMENTS})

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
        HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}
        REGEX *.h
    )

    # Stage header files with fluffycoin/<service>/<sbudir> prefix
    set(HEADER_BASEDIR ${CMAKE_CURRENT_BINARY_DIR}/dist/include/)
    set(HEADER_DIR ${HEADER_BASEDIR}/fluffycoin/services/${FLUFFYSVC_NAME})
    set(HEADER_TIMESTAMP ${CMAKE_CURRENT_BINARY_DIR}/.${FLUFFYSVC_NAME}.headers.timestamp)
    add_custom_command(
        OUTPUT
            ${HEADER_TIMESTAMP}
        COMMAND
            mkdir -p ${HEADER_DIR}
        COMMAND
            rm -rf ${HEADER_DIR}
        COMMAND
            ln -s ${CMAKE_CURRENT_SOURCE_DIR} ${HEADER_DIR}
        COMMAND
            touch ${HEADER_TIMESTAMP}
        DEPENDS
            ${HEADER_FILES}
        COMMENT
            "Setting up fluffycoin-${FLUFFYSVC_NAME} headers"
    )
    set_source_files_properties(
        ${HEADER_TIMESTAMP}
        PROPERTIES GENERATED TRUE)
    add_custom_target(
        fluffycoin-${FLUFFYSVC_NAME}-headers
        DEPENDS
            ${HEADER_TIMESTAMP}
    )

    # Executable
    add_executable(
        fluffycoin-${FLUFFYSVC_NAME}
        ${SOURCE_FILES}
    )

    # Our headers
    add_dependencies(
        fluffycoin-${FLUFFYSVC_NAME}
        fluffycoin-${FLUFFYSVC_NAME}-headers
    )
    target_include_directories(
        fluffycoin-${FLUFFYSVC_NAME}
        PUBLIC
            ${HEADER_BASEDIR}
    )

    # Our link dependencies
    target_link_libraries(
        fluffycoin-${FLUFFYSVC_NAME}
        PUBLIC
            ${FLUFFYSVC_PUBLIC}
        PRIVATE
            ${FLUFFYSVC_PRIVATE}
    )
endmacro()
