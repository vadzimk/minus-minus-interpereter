cmake_minimum_required(VERSION 3.13)
project(MinusMinusV3Shell)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(MinusMinusV3Shell
        main.cpp
        Program.cpp
        Program.h
        Stack.cpp
        Stack.h
        support.cpp
        support.hpp
        SymbolTable.cpp
        SymbolTable.h)
