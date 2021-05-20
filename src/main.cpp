// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs,
// OpenGL/Vulkan/Metal graphics context creation, etc.) If you are new to Dear ImGui, read
// documentation from the docs/ folder + read the top of imgui.cpp. Read online:
// https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glfw.h"
#include "camera.h"
#include "transform.h"

#include "glm/gtx/matrix_decompose.hpp"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of
// testing and compatibility with old VS compilers. To link with VS2010-era libraries, VS2015+
// requires linking with legacy_stdio_definitions.lib, which we do using this pragma. Your own
// project should not be affected, as you are likely to link with a newer binary of GLFW that is
// adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#    pragma comment(lib, "legacy_stdio_definitions")
#endif

#include <glad/glad.h>
// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

GLuint
createShader(const char *src, GLenum shaderType) {
    // Create a shader and load the string as source code and compile it
    GLuint s = glCreateShader(shaderType);
    glShaderSource(s, 1, (const GLchar **)&src, NULL);
    glCompileShader(s);

    // Check compilation status: this will report syntax errors
    GLint status;
    glGetShaderiv(s, GL_COMPILE_STATUS, &status);
    if (!status) {
        std::cerr << "Compiling of shader failed: ";
        char log[512];
        glGetShaderInfoLog(s, 512, NULL, log);
        std::cerr << log << std::endl;
        return 0;
    }

    return s;
}

GLuint
createShaderProgram(GLuint vertex, GLuint fragment) {
    // Create a shader program and attach the vertex and fragment shaders
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    return program;
}

bool
linkShader(GLuint program) {
    // Link the program and check the status: this will report semantics errors
    glLinkProgram(program);
    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        std::cerr << "Linking of shader failed: ";
        char log[512];
        glGetProgramInfoLog(program, 512, NULL, log);
        std::cerr << log << std::endl;
        return false;
    }
    return true;
}

int
main(int, char **) {
    Transform transform;
    transform.setLocalSkew({5, -3, 1});
    transform.setLocalScale({2, 1, 4});
    auto mat = transform.localToWorldMatrix();

    glm::vec3 expect_scale{};
    glm::quat expect_rotation{};
    glm::vec3 expect_translation{};
    glm::vec3 expect_skew{};
    glm::vec4 expect_perspective{};
    glm::decompose(
        mat,
        expect_scale,
        expect_rotation,
        expect_translation,
        expect_skew,
        expect_perspective);

    auto &glfw   = GLFW::Manager::get();
    auto &window = GLFW::Window::get(1280, 720, "Roguelike");

    Camera camera;
    camera.setPosition({0, 0, 2});
    bool cursorLocked = false;

    window.registerKeyCallback(GLFW::Key::ESCAPE, [&](int, int, GLFW::Action, int) {
        window.unlockCursor();
        cursorLocked = false;
    });
    window.registerMouseCallback(GLFW::Button::LEFT, [&](int, GLFW::Action, int) {
        window.lockCursor();
        cursorLocked = true;
    });
    window.registerCursorCallback([&](double x, double y) {
        if (cursorLocked) {
            // Downwards mouse movement increases y, so invert it.
            camera.addRotation(static_cast<float>(x), -static_cast<float>(y));
            window.setCursorPos(0, 0);
        }
    });
    glm::vec2 velocity{0, 0};
    window.registerKeyCallback(GLFW::Key::W, [&](int, int, GLFW::Action action, int) {
        switch (action) {
            case GLFW::Action::PRESS: velocity.y++; break;
            case GLFW::Action::RELEASE: velocity.y--; break;
            case GLFW::Action::REPEAT: break;
        }
    });
    window.registerKeyCallback(GLFW::Key::S, [&](int, int, GLFW::Action action, int) {
        switch (action) {
            case GLFW::Action::PRESS: velocity.y--; break;
            case GLFW::Action::RELEASE: velocity.y++; break;
            case GLFW::Action::REPEAT: break;
        }
    });
    window.registerKeyCallback(GLFW::Key::D, [&](int, int, GLFW::Action action, int) {
        switch (action) {
            case GLFW::Action::PRESS: velocity.x++; break;
            case GLFW::Action::RELEASE: velocity.x--; break;
            case GLFW::Action::REPEAT: break;
        }
    });
    window.registerKeyCallback(GLFW::Key::A, [&](int, int, GLFW::Action action, int) {
        switch (action) {
            case GLFW::Action::PRESS: velocity.x--; break;
            case GLFW::Action::RELEASE: velocity.x++; break;
            case GLFW::Action::REPEAT: break;
        }
    });

    ImGuiIO &io = ImGui::GetIO();
    io.Fonts->AddFontDefault();
    io.Fonts->Build();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.IniFilename = nullptr; // Disable saving of window positions between executions
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 0.8f;
    }
    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple
    // fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the
    // font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in
    // your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture
    // when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below
    // will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to
    // write a double backslash \\ !
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL,
    // io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

    const char *VERTEX_SRC =
        "#version 330 core\n"
        "layout(location=0) in vec3 position;" // Vertex position (x, y, z)
        "layout(location=1) in vec3 color;"    // Vertex color (r, g, b)
        "out vec3 fColor;"                     // Vertex shader has to pass color to fragment shader
        "uniform mat4 MVP;"
        "uniform mat4 obj;"
        "void main()"
        "{"
        "    fColor = color;"                            // Pass color to fragment shader
        "    gl_Position = MVP*obj*vec4(position, 1.0);" // Place vertex at (x, y, z, 1)
        "}";

    const char *FRAGMENT_SRC =
        "#version 330 core\n"
        "in vec3 fColor;"       // From the vertex shader
        "out vec4 outputColor;" // The color of the resulting fragment
        "void main()"
        "{"
        "    outputColor = vec4(fColor, 1.0);" // Color it (r, g, b, 1.0) for fully opaque
        "}";

    GLuint vertex = createShader(VERTEX_SRC, GL_VERTEX_SHADER);
    if (!vertex) {
        return -1;
    }
    GLuint fragment = createShader(FRAGMENT_SRC, GL_FRAGMENT_SHADER);
    if (!fragment) {
        return -1;
    }
    GLuint program = createShaderProgram(vertex, fragment);
    if (!program) {
        return -1;
    }
    if (!linkShader(program)) {
        return -1;
    }
    glDetachShader(program, vertex);
    glDeleteShader(vertex);
    glDetachShader(program, fragment);
    glDeleteShader(fragment);

    glUseProgram(program);

    const float vertices[] = {
        /* vertex0 */
        -0.5f,
        -0.5f,
        -0.5f,
        /* color0 */
        0.0f,
        0.0f,
        0.0f,

        /* vertex1 */
        0.5f,
        -0.5f,
        -0.5f,
        /* color1 */
        1.0f,
        0.0f,
        0.0f,

        /* vertex2 */
        -0.5f,
        0.5f,
        -0.5f,
        /* color2 */
        0.0f,
        1.0f,
        0.0f,

        /* vertex3 */
        0.5f,
        0.5f,
        -0.5f,
        /* color3 */
        1.0f,
        1.0f,
        0.0f,

        /* vertex4 */
        -0.5f,
        -0.5f,
        0.5f,
        /* color4 */
        0.0f,
        0.0f,
        1.0f,

        /* vertex5 */
        0.5f,
        -0.5f,
        0.5f,
        /* color5 */
        1.0f,
        0.0f,
        1.0f,

        /* vertex6 */
        -0.5f,
        0.5f,
        0.5f,
        /* color6 */
        0.0f,
        1.0f,
        1.0f,

        /* vertex7 */
        0.5f,
        0.5f,
        0.5f,
        /* color7 */
        1.0f,
        1.0f,
        1.0f,
    };

    const GLushort indices[]{
        /* Triangle 0 */
        0,
        2,
        1,
        /* Triangle 1 */
        1,
        2,
        3,
        /* Triangle 2 */
        0,
        4,
        2,
        /* Triangle 3 */
        2,
        4,
        6,
        /* Triangle 4 */
        4,
        7,
        6,
        /* Triangle 5 */
        4,
        5,
        7,
        /* Triangle 6 */
        1,
        7,
        5,
        /* Triangle 7 */
        1,
        3,
        7,
        /* Triangle 8 */
        7,
        3,
        2,
        /* Triangle 9 */
        7,
        2,
        6,
        /* Triangle 10 */
        1,
        5,
        0,
        /* Triangle 11 */
        5,
        4,
        0,
    };

    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Upload the vertices to the buffer
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Enable the vertex attributes and upload their data (see: layout(location=x))
    glEnableVertexAttribArray(0); // position
    glEnableVertexAttribArray(1); // color

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(GLfloat),
        (void *)(3 * sizeof(GLfloat)));

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Our state
    //    bool      show_demo_window    = true;
    //    bool      show_another_window = false;
    ImVec4    clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    float     lastTime    = static_cast<float>(glfwGetTime());
    Transform parentTransform;
    Transform otherTransform;
    Transform childTransform(parentTransform);
    otherTransform.setLocalPosition({-1, 1, 0});
    otherTransform.setLocalScale({1, 0.5f, 1});
    otherTransform.setLocalRotation(glm::angleAxis(glm::radians(5.f), glm::vec3{0, 1, 0}));
    otherTransform.setLocalSkew({0.2, -1, 0.5});

    childTransform.setLocalPosition({-1.5, 0, 0});
    //    childTransform.setLocalScale({-0.5f, 1.f, -1.f});

    //    parentTransform.scaleDirection({1, 0, 0}, 0.5);

    int scale = 0;
    int rot   = 0;

    window.registerKeyCallback(GLFW::Key::Q, [&](int, int, GLFW::Action action, int) {
        if (action == GLFW::Action::PRESS) {
            childTransform.setParent(&parentTransform);
        }
    });
    window.registerKeyCallback(GLFW::Key::E, [&](int, int, GLFW::Action action, int) {
        if (action == GLFW::Action::PRESS) {
            childTransform.setParent(&otherTransform);
        }
    });
    window.registerKeyCallback(GLFW::Key::R, [&](int, int, GLFW::Action action, int) {
        switch (action) {
            case GLFW::Action::PRESS: rot--; break;
            case GLFW::Action::RELEASE: rot++; break;
            default: break;
        }
    });
    window.registerKeyCallback(GLFW::Key::F, [&](int, int, GLFW::Action action, int) {
        switch (action) {
            case GLFW::Action::PRESS: rot++; break;
            case GLFW::Action::RELEASE: rot--; break;
            default: break;
        }
    });

    glfwSwapInterval(1);
    // Main loop
    while (!window.shouldClose()) {
        float deltaTime = static_cast<float>(glfwGetTime()) - lastTime;
        lastTime        = static_cast<float>(glfwGetTime());

        auto pos     = camera.position();
        auto forward = camera.forward();
        auto right   = camera.right();
        pos += deltaTime * (velocity.x * right + velocity.y * forward);
        camera.setPosition(pos);

        if (scale) {
            childTransform.setParent(&otherTransform);
        }

        otherTransform.setLocalSkew({0, 0, 1 + glm::cos(lastTime)})
            .setLocalScale({glm::sin(2 * lastTime), 1, 1});
        // parentTransform.setLocalRotation(glm::angleAxis(lastTime, glm::vec3{1, 0, 0}));
        parentTransform.setLocalRotation(glm::angleAxis(lastTime / 2, glm::vec3{0, 1, 0}));
        //        childTransform.setLocalSkew({0, 0, 1 + glm::cos(lastTime)});

        //        objectTransform.resetScale();
        //        objectTransform.scaleDirection(
        //            glm::vec3{1, 1, 0} * (1 + glm::cos(static_cast<float>(lastTime))));

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear
        // imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main
        // application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your
        // main application. Generally you may always pass all inputs to dear imgui, and hide
        // them from your application based on those two flags.

        window.makeCurent();
        glfw.pollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        /*
        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You
        // can browse its code to learn more about Dear ImGui!).
        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a
        // named window.
        {
            static float f       = 0.0f;
            static int   counter = 0;

            ImGui::Begin(
                "Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text."); // Display some text (you can use a format
                                                      // strings too)
            ImGui::Checkbox(
                "Demo Window",
                &show_demo_window); // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat(
                "float",
                &f,
                0.0f,
                1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3(
                "clear color",
                (float *)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return
                                         // true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text(
                "Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window) {
            ImGui::Begin(
                "Another Window",
                &show_another_window); // Pass a pointer to our bool variable (the window will have
                                       // a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me")) show_another_window = false;
            ImGui::End();
        }
         */

        // Rendering
        glClear(GL_DEPTH_BUFFER_BIT);
        ImGui::Render();
        window.drawBackground(clear_color.x, clear_color.y, clear_color.z);

        auto [display_w, display_h] = window.getFrameBufferSize();

        glm::mat4 mvp   = camera.getMatrix((float)display_w / (float)display_h);
        GLint     mvpID = glGetUniformLocation(program, "MVP");
        glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(mvp));

        {
            glm::mat4 obj   = parentTransform.localToWorldMatrix();
            GLint     objID = glGetUniformLocation(program, "obj");
            glUniformMatrix4fv(objID, 1, GL_FALSE, glm::value_ptr(obj));

            glBindVertexArray(vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_SHORT, nullptr);
        }
        {
            glm::mat4 obj   = childTransform.localToWorldMatrix();
            GLint     objID = glGetUniformLocation(program, "obj");
            glUniformMatrix4fv(objID, 1, GL_FALSE, glm::value_ptr(obj));

            glBindVertexArray(vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_SHORT, nullptr);
        }
        {
            glm::mat4 obj   = otherTransform.localToWorldMatrix();
            GLint     objID = glGetUniformLocation(program, "obj");
            glUniformMatrix4fv(objID, 1, GL_FALSE, glm::value_ptr(obj));

            glBindVertexArray(vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_SHORT, nullptr);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.updatePlatformWindows();
        window.swapBuffers();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    return 0;
}
