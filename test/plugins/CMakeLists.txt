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
    set_target_properties(${plugin} PROPERTIES
            "RUNTIME_OUTPUT_DIRECTORY_$<cfg>" "${CMAKE_BINARY_DIR}/test/plugins")
    add_dependencies(${TEST_NAME} ${plugin})

    if (MSVC)
        # MSVC places build artifacts in configuration-specific folders ("Debug/", "Release/", etc), but our plugin
        # tests expect the built library to be in a specific directory. The following will force MSVC to place the built
        # dlls into the same directory as other compilers.
        foreach (CFG ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${CFG} UPPERCFG)
            set_target_properties(${plugin} PROPERTIES
                    "RUNTIME_OUTPUT_DIRECTORY_${UPPERCFG}" "${CMAKE_BINARY_DIR}/test/plugins")
        endforeach ()
    endif ()
endforeach ()
