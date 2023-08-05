# fluffyservice(
#    name
#    PUBLIC <dependencies>
#    PRIVATE <dependencies>
#    NAMESPACES <include namespaces>
# )
macro(fluffyservice)
    # Parse input
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PUBLIC PRIVATE NAMESPACES)
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

    # Stage header files with fluffycoin/<service>/<subdir> prefix
    set(HEADER_BASEDIR ${CMAKE_CURRENT_BINARY_DIR}/dist/include/)
    set(HEADER_DIR ${HEADER_BASEDIR}/fluffycoin/${FLUFFYSVC_NAME})

    # For each namespace
    set(HEADER_TIMESTAMP_FILES)
    foreach(namespace ${FLUFFYSVC_NAMESPACES})
        set(NAMESPACED_HEADERS)
        # Find all headers in HEADER_FILES that are under that subdirectory
        foreach(file ${HEADER_FILES})
            if (${file} MATCHES ${namespace})
                # Add to namespaced headers list
                list(APPEND NAMESPACED_HEADERS ${file})
                # Remove from top level headers list
                list(REMOVE_ITEM HEADER_FILES ${file})
            endif()
        endforeach()
        if(NOT "${NAMESPACED_HEADERS}" STREQUAL "")
            # Timestamp file to tie dependencies together
            set(HEADER_TIMESTAMP_FILENAME ${FLUFFYSVC_NAME}.headers.${namespace}.namespace)
            string(REPLACE "/" "." HEADER_TIMESTAMP_FILENAME ${HEADER_TIMESTAMP_FILENAME})
            # Create the namespace subdirectory and symlink files in
            set(HEADER_TIMESTAMP ${CMAKE_CURRENT_BINARY_DIR}/.${HEADER_TIMESTAMP_FILENAME})
            set(NAMESPACE_HEADER_DIR ${HEADER_DIR}/${namespace})
            add_custom_command(
                OUTPUT
                    ${HEADER_TIMESTAMP}
                COMMAND
                    rm -rf ${NAMESPACE_HEADER_DIR}
                COMMAND
                    mkdir -p ${NAMESPACE_HEADER_DIR}
                COMMAND
                    ln -s ${NAMESPACED_HEADERS} ${NAMESPACE_HEADER_DIR}/
                COMMAND
                    touch ${HEADER_TIMESTAMP}
                DEPENDS
                    ${NAMESPACED_HEADERS}
                COMMENT
                    "Setting up fluffycoin-${FLUFFYSVC_NAME}/${namespace} headers"
            )
            list(APPEND HEADER_TIMESTAMP_FILES ${HEADER_TIMESTAMP})
        endif()
    endforeach()

    # Symlink the top level headers
    set(HEADER_TIMESTAMP ${CMAKE_CURRENT_BINARY_DIR}/.${FLUFFYSVC_NAME}.headers.timestamp)
    add_custom_command(
        OUTPUT
            ${HEADER_TIMESTAMP}
        COMMAND
            rm -f ${HEADER_DIR}/*.h
        COMMAND
            mkdir -p ${HEADER_DIR}
        COMMAND
            ln -s ${HEADER_FILES} ${HEADER_DIR}/
        COMMAND
            touch ${HEADER_TIMESTAMP}
        DEPENDS
            ${HEADER_FILES}
        COMMENT
            "Setting up fluffycoin-${FLUFFYSVC_NAME} headers"
    )
    list(APPEND HEADER_TIMESTAMP_FILES ${HEADER_TIMESTAMP})
    set_source_files_properties(
        ${HEADER_TIMESTAMP_FILES}
        PROPERTIES GENERATED TRUE)
    add_custom_target(
        fluffycoin-${FLUFFYSVC_NAME}-headers
        DEPENDS
            ${HEADER_TIMESTAMP_FILES}
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
