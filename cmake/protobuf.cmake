set(PROTOBUF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/protobuf)
option(protobuf_BUILD_TESTS "" OFF)
option(ABSL_PROPAGATE_CXX_STD "" ON)
add_subdirectory(${PROTOBUF_DIR})
