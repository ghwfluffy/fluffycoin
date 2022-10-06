cmake_minimum_required(VERSION 3.16)
project(fluffycoin)

set(CMAKE_CXX_STANDARD 17)

include(cmake/cxxflags.cmake)
include(cmake/fluffylib.cmake)

find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)

add_subdirectory(src/log)
add_subdirectory(src/utils)
add_subdirectory(src/block)
