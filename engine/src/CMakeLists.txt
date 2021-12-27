add_library(engine SHARED engine.cpp)

target_include_directories(engine
        INTERFACE ../include)
target_link_libraries(engine
        INTERFACE glm
        PRIVATE core) # PRIVATE so that core symbols are not exposed through the engine interface
target_compile_definitions(engine
        PUBLIC BUILDING_DLL)
set_target_properties(engine PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (MSVC)
    target_compile_definitions(engine
            PRIVATE _CRT_SECURE_NO_WARNINGS)
endif ()
