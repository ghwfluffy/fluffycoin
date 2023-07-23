cmake_minimum_required(VERSION 3.16)
project(fluffycoin)

# Distributables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dist/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dist/lib)

# gtest
add_subdirectory(lib/googletest)

# Local build dependencies
include(cmake/sodium.cmake)
include(cmake/zmq.cmake)
include(cmake/protobuf.cmake)
include(cmake/boost.cmake)

# Build configuration
include(cmake/cxxflags.cmake)

# Utilities
include(cmake/dist.cmake)
include(cmake/fluffybin.cmake)
include(cmake/fluffylib.cmake)

# Dependencies
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)

# Protobuf files
add_subdirectory(proto)

# Modules
add_subdirectory(src/log)
add_subdirectory(src/utils)
add_subdirectory(src/ossl)
add_subdirectory(src/block)
add_subdirectory(src/alg)
add_subdirectory(src/zmq)
add_subdirectory(src/async)
add_subdirectory(src/pb)
add_subdirectory(src/svc)

# Executables
add_subdirectory(src/cli)

# Unit tests
add_subdirectory(tests/unit)
