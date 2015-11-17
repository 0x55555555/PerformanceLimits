cmake_minimum_required(VERSION 3.1)

add_executable(tests
    Limits.h
    Limits.cpp
    MemCpy.cpp
)

add_definitions(-std=c++14)
