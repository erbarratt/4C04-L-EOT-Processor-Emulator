cmake_minimum_required(VERSION 3.16)
project(4c04)

set(CMAKE_C_STANDARD 11)

ADD_DEFINITIONS(-Wall -Wextra -Wconversion -pedantic -c -g -O3 -std=gnu99)

find_package(X11 REQUIRED)

message(STATUS "X11_FOUND = ${X11_FOUND}")
message(STATUS "X11_INCLUDE_DIR = ${X11_INCLUDE_DIR}")
message(STATUS "X11_LIBRARIES = ${X11_LIBRARIES}")

include_directories(${X11_INCLUDE_DIR})

add_executable(
               4c04 4c04.c 4c04.h cpu.c draw.c)

target_link_libraries(4c04 X11)