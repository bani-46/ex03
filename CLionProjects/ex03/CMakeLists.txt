cmake_minimum_required(VERSION 3.7)
project(ex03)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/compiler.h
        src/main.c
        src/parser.c
        src/scan.c
        src/id-list.c)

add_executable(ex03 ${SOURCE_FILES})