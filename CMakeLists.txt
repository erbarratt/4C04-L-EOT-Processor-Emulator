cmake_minimum_required(VERSION 3.16)
project(4C04)

set(CMAKE_C_STANDARD 11)

add_executable(
               common.h main.c main.h gfx.c gfx.h)