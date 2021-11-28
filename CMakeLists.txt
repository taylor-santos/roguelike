cmake_minimum_required(VERSION 3.16)
project(roguelike)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external)

include_directories(include)
add_subdirectory(src)
