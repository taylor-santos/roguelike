find_package(OpenGL REQUIRED)

# Add non-main source files here
set(BUILD_SRC
        glfw.cpp
        gui.cpp
        transform.cpp
        camera.cpp)

set(TEST_NAME ${PROJECT_NAME}_tests)

add_executable(${PROJECT_NAME} ${BUILD_SRC} main.cpp)
add_executable(${TEST_NAME} ${BUILD_SRC} test.cpp)

# Disable doctest code generation for the main project
target_compile_definitions(${PROJECT_NAME} PUBLIC
        DOCTEST_CONFIG_DISABLE
        IMGUI_USER_CONFIG="imgui_config.h")

target_link_libraries(${PROJECT_NAME}
        lib_imgui
        glm
        doctest)

target_link_libraries(${TEST_NAME}
        lib_imgui
        glm
        doctest)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    target_compile_options(${TEST_NAME} PRIVATE /W4 /WX)
else ()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
    target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif ()
