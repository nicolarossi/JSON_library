

cmake_minimum_required(VERSION 3.13)
project(
    serialization
    VERSION 0.0.1
    DESCRIPTION "A header-only library to serialize/deserialize objects"
    LANGUAGES CXX
)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -ggdb")

file(GLOB_RECURSE test_sources test/*.cpp )

add_executable(serialization main.cpp ${test_sources} )

include_directories(serialization ./include )

include(GoogleTest)
include(CTest)

gtest_add_tests(  TARGET serialization SOURCES  ${test_sources})

target_link_libraries( serialization  gtest )