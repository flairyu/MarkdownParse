cmake_minimum_required(VERSION 2.8)
project(MARKDOWNPARSE)

set(CMAKE_C_FLAGS "-std=c99 -pedantic")

add_subdirectory(lib)
add_subdirectory(test)
