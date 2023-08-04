# fluffyproto(
#    namespace
#    PUBLIC <dependencies>
#    PRIVATE <dependencies>
# )
macro(fluffyproto)
    # Parse input
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PUBLIC PRIVATE)
    cmake_parse_arguments(FLUFFYPROTO "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(FLUFFY_PROTO_PATH ${FLUFFYPROTO_UNPARSED_ARGUMENTS})

    # Convert module/v1 to module-v1
    string(REPLACE "/" "-" FLUFFY_PROTO_NAME ${FLUFFY_PROTO_PATH})

    # Find all IDL files
    file(
        GLOB_RECURSE
        SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}
        REGEX *.proto
    )

    set(STAGEDIR ${CMAKE_BINARY_DIR}/proto-stage)
    set(MODULE_STAGEDIR ${STAGEDIR}/fcpb/${FLUFFY_PROTO_PATH})

    # Convert SOURCE_FILES to staged/generated paths
    set(PROTO_FILES)
    set(CPP_SOURCE_FILES)
    foreach(file ${SOURCE_FILES})
        # Need to remove subdirectory from file
        # MODULE_STAGEDIR = %/proto-stage/fcpb/comm
        # file = %/comm/somesubdirmaybe/blah.proto
        # staged_file = MODULE_STAGEDIR/blah.proto
        get_filename_component(basename ${file} NAME)
        set(staged_file "${MODULE_STAGEDIR}/${basename}")
        string(REPLACE ".proto" ".pb.cc" source_file ${staged_file})
        list(APPEND PROTO_FILES ${staged_file})
        list(APPEND CPP_SOURCE_FILES ${source_file})
    endforeach()

    # Stage header files with fcpb/module/ prefix
    set(IDL_TIMESTAMP ${CMAKE_CURRENT_BINARY_DIR}/.${FLUFFY_PROTO_NAME}.fcpb.stage.timestamp)
    add_custom_command(
        OUTPUT
            ${IDL_TIMESTAMP}
            ${PROTO_FILES}
        COMMAND
            rm -rf ${MODULE_STAGEDIR}
        COMMAND
            mkdir -p ${MODULE_STAGEDIR}
        COMMAND
            ln -s ${SOURCE_FILES} ${MODULE_STAGEDIR}/
        COMMAND
            touch ${IDL_TIMESTAMP}
        DEPENDS
            ${SOURCE_FILES}
        COMMENT
            "Staging protobuf files for fluffyproto-${FLUFFY_PROTO_NAME}"
    )

    # Compile protobuf files into C++ files
    add_custom_command(
        OUTPUT
            ${CPP_SOURCE_FILES}
        DEPENDS
            ${SOURCE_FILES}
            ${PROTO_FILES}
            ${IDL_TIMESTAMP}
        COMMAND
            protobuf::protoc
                --proto_path=${PROTOBUF_DIR}/src
                --proto_path=${STAGEDIR}
                --cpp_out=${STAGEDIR}
                ${PROTO_FILES}
        COMMENT
            "Compile fluffyproto-${FLUFFY_PROTO_NAME}"
    )
    set_source_files_properties(
        ${IDL_TIMESTAMP}
        ${CPP_SOURCE_FILES}
        PROPERTIES GENERATED TRUE
    )

    # Create shared library
    add_library(
        fluffyproto-${FLUFFY_PROTO_NAME}
        STATIC
            ${CPP_SOURCE_FILES}
    )
    # No warnings
    target_compile_options(
        fluffyproto-${FLUFFY_PROTO_NAME}
        PRIVATE
            -w)

    target_include_directories(
        fluffyproto-${FLUFFY_PROTO_NAME}
        PUBLIC
            ${STAGEDIR}
    )

    # Our link dependencies
    target_link_libraries(
        fluffyproto-${FLUFFY_PROTO_NAME}
        PUBLIC
            libprotobuf
            ${FLUFFYPROTO_PUBLIC}
        PRIVATE
            ${FLUFFYPROTO_PRIVATE}
    )
endmacro()
