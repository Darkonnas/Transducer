cmake_minimum_required(VERSION 3.17)
project(Transducer)

set(CMAKE_CXX_STANDARD 20)

add_executable(Transducer main.cpp transducer.cpp utility.hpp utility.cpp)
