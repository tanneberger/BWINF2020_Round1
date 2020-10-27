cmake_minimum_required(VERSION 3.16)
project(BWINF2020_Aufgabe4 CXX)
set(CMAKE_CXX_STANDARD 20)

add_executable(Aufgabe4
        main.cpp
        src/vertex.hpp
        src/vertex.cpp
        src/graph.hpp
        src/graph.cpp
        )