# fluffybin(
#    name
#    PRIVATE <dependencies>
# )
macro(fluffybin)
    # Parse input
    set(options)
    set(oneValueArgs)
    set(multiValueArgs PRIVATE)
    cmake_parse_arguments(FLUFFYBIN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    set(FLUFFYBIN_NAME ${FLUFFYBIN_UNPARSED_ARGUMENTS})

    # Source file
    set(SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${FLUFFYBIN_NAME}.cpp)

    # binrary target
    add_executable(
        fluffycoin-${FLUFFYBIN_NAME}
        ${SOURCE_FILE}
    )

    # Our link dependencies
    target_link_libraries(
        fluffycoin-${FLUFFYBIN_NAME}
        PRIVATE
            ${FLUFFYBIN_PRIVATE}
    )
endmacro()
