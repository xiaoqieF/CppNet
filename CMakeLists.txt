cmake_minimum_required(VERSION 3.16)
project(cppnet)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-g -O0 -Wall -Werror -std=c++14")
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(base)
add_subdirectory(logger)
add_subdirectory(net)
add_subdirectory(examples)