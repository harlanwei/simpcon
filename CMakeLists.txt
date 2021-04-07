cmake_minimum_required(VERSION 3.16)

project(simpcon C)

set(CMAKE_C_STANDARD 11)

add_executable(simpcon main.c include/id.h lib/id.c lib/fork.c include/errno.h include/fork.h)
