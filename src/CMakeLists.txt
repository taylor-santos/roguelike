find_package(OpenGL REQUIRED)

# Add non-main source files here
set(BUILD_SRC
        glfw.cpp
        gui.cpp
        transform.cpp
        camera.cpp
        shader.cpp)

add_executable(${PROJECT_NAME} ${BUILD_SRC} main.cpp)
target_compile_definitions(${PROJECT_NAME} PUBLIC
        DOCTEST_CONFIG_DISABLE
        IMGUI_USER_CONFIG="imgui_config.h")
target_link_libraries(${PROJECT_NAME}
        lib_imgui
        glm
        doctest)

include(CTest)
enable_testing()
set(TEST_NAME all_tests)
add_executable(${TEST_NAME}
        ${BUILD_SRC}
        test.cpp)
target_link_libraries(${TEST_NAME}
        lib_imgui
        glm
        doctest)
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(${TEST_NAME}
        ADD_LABELS 1) # https://github.com/onqtam/doctest/pull/490

set_target_properties(${PROJECT_NAME} ${TEST_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -pedantic -Werror)
    target_compile_options(${TEST_NAME} PRIVATE
            -Wall -Wextra -pedantic -Werror)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -pedantic -Werror)
    target_compile_options(${TEST_NAME} PRIVATE
            -Wall -Wextra -pedantic -Werror)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    target_compile_options(${PROJECT_NAME} PRIVATE
            /W4 /WX)
    target_compile_options(${TEST_NAME} PRIVATE
            /W4 /WX)
endif ()
