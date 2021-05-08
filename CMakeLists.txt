cmake_minimum_required(VERSION 3.16)
project(roguelike)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external)

include_directories(include external/imgui external/imgui/backends external/doctest)
add_subdirectory(src)
