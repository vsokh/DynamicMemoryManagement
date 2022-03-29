cmake_minimum_required(VERSION 3.20)
project(SharedPointer)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(memory
        main.cpp
        Memory.hpp)
