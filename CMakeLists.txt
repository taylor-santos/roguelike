cmake_minimum_required(VERSION 3.16)
project(roguelike)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external)
add_subdirectory(core)
add_subdirectory(test)
add_subdirectory(engine)
add_subdirectory(plugins)
add_subdirectory(src)
