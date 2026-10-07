
cmake_minimum_required(VERSION 4.3)
project(Lab2_WindowsApp)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-DUNICODE -D_UNICODE)

add_executable(T1 WIN32 T1.cpp)
add_executable(T2 WIN32 T2.cpp)