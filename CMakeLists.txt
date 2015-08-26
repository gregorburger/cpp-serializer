cmake_minimum_required(VERSION 3.2)
project(serialize)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")

set(SOURCE_FILES main.cpp)
add_executable(serialize ${SOURCE_FILES})