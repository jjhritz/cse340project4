cmake_minimum_required(VERSION 3.6)
project(cse340project4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES semantic.c syntax.h)
add_executable(cse340project4 ${SOURCE_FILES})