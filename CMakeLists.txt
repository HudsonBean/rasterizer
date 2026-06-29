cmake_minimum_required(VERSION 3.16)
project(rasterizer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED CONFIG)

add_executable(rasterizer main.cpp)
target_link_libraries(rasterizer PRIVATE SDL3::SDL3)