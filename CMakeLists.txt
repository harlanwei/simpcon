cmake_minimum_required(VERSION 3.16)

project(simpcon C)

set(CMAKE_C_STANDARD 11)

add_executable(simpcon main.c lib/id.c lib/fork.c lib/sys_wrap.c lib/cgroup.c)
