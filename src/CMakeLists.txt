find_package(OpenGL REQUIRED)

set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/external/imgui)

set(IMGUI_SRC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

# Add non-main source files here
set(BUILD_SRC
        sample.cpp)

set(TEST_NAME ${PROJECT_NAME}_tests)

add_executable(${PROJECT_NAME} ${IMGUI_SRC} ${BUILD_SRC} main.cpp)
add_executable(${TEST_NAME} ${BUILD_SRC} test.cpp)

# Disable doctest code generation for the main project
target_compile_definitions(${PROJECT_NAME} PUBLIC
        DOCTEST_CONFIG_DISABLE
        IMGUI_USER_CONFIG="imgui_config.h")

target_link_libraries(${PROJECT_NAME}
        glad
        glfw
        OpenGL::GL
        ${CMAKE_DL_LIBS})

target_link_libraries(${TEST_NAME}
        doctest::doctest)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    target_compile_options(${TEST_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif ()
