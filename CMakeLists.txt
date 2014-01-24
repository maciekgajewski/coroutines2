project(coroutines2)
cmake_minimum_required(VERSION 2.6)

include_directories(${Boost_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-unused-local-typedefs")

add_subdirectory(coroutines)
add_subdirectory(test)
