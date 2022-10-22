project(fluffycoin-gtest)

file(
    GLOB_RECURSE
    SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}
    REGEX *.cpp
)

add_executable(
    fluffycoin-gtest
    ${SOURCE_FILES}
)

target_include_directories(
    fluffycoin-gtest
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main
)

target_link_libraries(
    fluffycoin-gtest
    PRIVATE
        gtest
        fluffycoin-log
        fluffycoin-utils
        fluffycoin-ossl
        fluffycoin-block
        fluffycoin-alg
)

# Create the fluffytest/ header namespace
include(cmake/headers.cmake)
fluffytestlib(alg)
fluffytestlib(block)
fluffytestlib(ossl)
fluffytestlib(utils)
