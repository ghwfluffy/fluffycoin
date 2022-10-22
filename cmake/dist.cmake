# fluffydist(
#    target
# )
function(
    fluffydist
    dist_target
)
    set(DIST_DIR ${CMAKE_SOURCE_DIR}/dist)
    set(DIST_FILE ${DIST_DIR}/${dist_target})

    add_custom_command(
        COMMAND
            mkdir -p ${DIST_DIR}
        COMMAND
            cp -v $<TARGET_FILE:${dist_target}> ${DIST_FILE}
        OUTPUT
            ${DIST_FILE}
        DEPENDS
            $<TARGET_FILE:${dist_target}>
        COMMENT
            "Distributing ${DIST_FILE}"
    )
    set_source_files_properties(
        ${DIST_FILE}
        PROPERTIES GENERATED TRUE)
    add_custom_target(
        fluffycoin-dist-${dist_target}
        DEPENDS
            ${dist_target}
            ${DIST_FILE}
    )
endfunction()
