cmake_minimum_required(VERSION 3.13)
project(
    serialization
    VERSION 0.0.1
    DESCRIPTION "A header-only library to serialize/deserialize objects"
    LANGUAGES CXX
    
)

set( CMAKE_CXX_STANDARD 20)

configure_file(
    "${PROJECT_SOURCE_DIR}/version.h.in"
    "${PROJECT_SOURCE_DIR}/include/version.h"
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# to avoid the risk of build in the source directory
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# In the future the executable will be the web server 
# add_executable(colosseumd src/webserver.cpp "${test_sources}" )

file(GLOB RECURSE headers include/*.h )


add_library(serialization INTERFACE "${headers}" )

# Testing
enable_testing()
add_subdirectory(test)
