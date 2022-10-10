cmake_minimum_required(VERSION 3.16)
project(fluffycoin)

# gtest
add_subdirectory(lib/googletest)

# Build configuration
include(cmake/cxxflags.cmake)

# Utilities
include(cmake/fluffybin.cmake)
include(cmake/fluffylib.cmake)

# Dependencies
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)

# Modules
add_subdirectory(src/log)
add_subdirectory(src/utils)
add_subdirectory(src/ossl)
add_subdirectory(src/block)

# Executables
add_subdirectory(src/cli)

# Unit tests
add_subdirectory(tests/unit)
