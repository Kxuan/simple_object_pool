cmake_minimum_required(VERSION 3.12)
project(simple_object_pool C)

set(CMAKE_C_STANDARD 99)

add_library(sop STATIC pool.h pool.c)
add_subdirectory(tests)