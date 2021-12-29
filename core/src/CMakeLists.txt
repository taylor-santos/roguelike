# Add non-main source files here
set(BUILD_SRC
        glfw.cpp
        gui.cpp
        transform.cpp
        camera.cpp
        shader.cpp
        plugin.cpp)

set(PUBLIC_LIBS
        imgui
        glm)

add_library(core
        ${BUILD_SRC})

target_link_libraries(core
        PUBLIC ${PUBLIC_LIBS})

target_include_directories(core
        INTERFACE ../include)

set_target_properties(core PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
    # g++8 doesn't automatically link the std::filesystem library, so -lstdc++fs must be explicitly linked
    target_link_libraries(core
            PUBLIC stdc++fs)
endif ()

if (MSVC)
    target_compile_definitions(core
            PRIVATE _CRT_SECURE_NO_WARNINGS)
else ()
    target_compile_options(core
            PRIVATE -fPIC)
    if (CYGWIN)
        # Enable mkstemp(3) (see https://man7.org/linux/man-pages/man3/mkstemp.3.html)
        target_compile_options(core
                PRIVATE -D_POSIX_C_SOURCE=200809L)
    endif ()
endif ()
