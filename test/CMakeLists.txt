include(CTest)
enable_testing()

option(DISABLE_RENDER_TESTS "Don't run any tests that use the GPU. Useful for headless CI test runs." OFF)

set(TEST_NAME
        ${PROJECT_NAME}_tests)

set(TEST_SRC
        test_transform.cpp
        test_camera.cpp
        test_shader.cpp
        test_glfw.cpp
        test_plugin.cpp)

add_executable(${TEST_NAME}
        test_main.cpp
        ${TEST_SRC})

if (DISABLE_RENDER_TESTS)
    target_compile_definitions(${TEST_NAME}
            PUBLIC DISABLE_RENDER_TESTS)
endif ()

target_link_libraries(${TEST_NAME}
        PRIVATE core
        PRIVATE doctest)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(${TEST_NAME}
        ADD_LABELS 1) # https://github.com/onqtam/doctest/pull/490

add_subdirectory(plugins)
