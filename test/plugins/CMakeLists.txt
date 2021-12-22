cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PLUGINS
        plugin.cpp
        plugin_patch.cpp)

foreach (plugin_cpp ${PLUGINS})
    get_filename_component(plugin ${plugin_cpp} NAME_WE)
    project(${plugin})

    add_library(${plugin} SHARED ${plugin_cpp})
    target_link_libraries(${plugin} engine)
    target_compile_definitions(${plugin} PUBLIC BUILDING_DLL)
    add_dependencies(${TEST_NAME} ${plugin})
endforeach ()
