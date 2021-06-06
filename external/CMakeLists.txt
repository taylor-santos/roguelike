include(FetchContent)

add_subdirectory(glad)

#------------------------------------------------------------------------------------
# Fetch glfw
#------------------------------------------------------------------------------------

message(STATUS "Fetching glfw...")
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 814b7929c5add4b0541ccad26fb81f28b71dc4d8 # 3.3.4
)
FetchContent_MakeAvailable(glfw)

#------------------------------------------------------------------------------------
# Fetch glm
#------------------------------------------------------------------------------------

message(STATUS "Fetching glm...")
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e # 0.9.9.8
)
FetchContent_MakeAvailable(glm)

#------------------------------------------------------------------------------------
# Fetch doctest
#------------------------------------------------------------------------------------

message(STATUS "Fetching doctest...")
set(DOCTEST_WITH_TESTS OFF)
set(DOCTEST_NO_INSTALL ON)
FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/onqtam/doctest
        GIT_TAG 4d8716f1efc1d14aa736ef52ee727bd4204f4c40 # 2.4.6
)
FetchContent_MakeAvailable(doctest)

#------------------------------------------------------------------------------------
# Fetch assimp
#------------------------------------------------------------------------------------

message(STATUS "Fetching assimp...")
FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp
        GIT_TAG 8f0c6b04b2257a520aaab38421b2e090204b69df # 5.0.1
)
FetchContent_MakeAvailable(assimp)

#------------------------------------------------------------------------------------
# Fetch imgui
#------------------------------------------------------------------------------------

message(STATUS "Fetching imgui...")
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG 1b435ae3e07ca813eb3ef40aaabe7053f5570fae # 1.83-docking
)
FetchContent_GetProperties(imgui)
if (NOT imgui_POPULATED)
    FetchContent_Populate(imgui)

    add_library(lib_imgui
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
            ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
    target_link_libraries(lib_imgui
            glad
            glfw)
    target_include_directories(lib_imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
endif ()
