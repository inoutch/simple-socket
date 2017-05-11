cmake_minimum_required(VERSION 3.7)
project(examples)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp ../ServerSocket.cpp ../ClientSocket.cpp)
add_executable(examples ${SOURCE_FILES})