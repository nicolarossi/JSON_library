# We will use it for the Json_Reader
#find_package(Boost 1.81 REQUIRED COMPONENTS json )

file(GLOB_RECURSE test_sources *.cpp )
add_executable(testing testing.cpp "${test_sources}")

set(CMAKE_BUILD_TYPE Debug)
add_test( TARGET testing )
add_compile_options(-fconcepts-diagnostics-depth=4 -g3 -ggdb)

target_include_directories(testing PRIVATE ../include)
#target_link_libraries( testing PRIVATE gtest Boost::json )

target_link_libraries( testing PRIVATE gtest )



