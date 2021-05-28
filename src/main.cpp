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
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include "shader.h"

int
main(int, char **) {
    auto &glfw   = GLFW::Manager::get();
    auto &window = GLFW::Window::get(1280, 720, "Roguelike");

    Camera camera;
    camera.transform.setPosition({0, 0, 2});
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
        "uniform bool red;"
        "void main()"
        "{"
        "    outputColor = red ? vec4(1,fColor.y,fColor.z,1) : vec4(fColor, 1.0);" // Color it (r,
                                                                                   // g, b, 1.0) for
                                                                                   // fully opaque
        "}";
    auto program = ShaderProgram::Builder()
                       .withShader(Shader(FRAGMENT_SRC, Shader::Type::FRAGMENT))
                       .withShader(Shader(VERTEX_SRC, Shader::Type::VERTEX))
                       .build();

    program.use();

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
    bool                   show_demo_window    = true;
    bool                   show_another_window = false;
    ImVec4                 clear_color         = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    double                 lastTime            = glfwGetTime();
    double                 deltaTime           = 0;
    std::vector<Transform> transforms;
    transforms.emplace_back();
    transforms.emplace_back(Transform::Builder().withParent(transforms[0]).withPosition({2, 0, 0}));
    transforms.emplace_back(Transform::Builder().withParent(transforms[1]));
    transforms.emplace_back(Transform::Builder().withParent(transforms[1]).withPosition({2, 0, 0}));
    transforms.emplace_back(Transform::Builder().withParent(transforms[1]).withPosition({4, 0, 0}));

    glfwSwapInterval(0);
    // Main loop
    while (!window.shouldClose()) {
        deltaTime = glfwGetTime() - lastTime;
        lastTime  = glfwGetTime();

        auto pos     = camera.transform.position();
        auto forward = camera.forward();
        auto right   = camera.right();
        pos += static_cast<float>(deltaTime) * (velocity.x * right + velocity.y * forward);
        camera.transform.setLocalPosition(pos);

        transforms[0].setLocalRotation(glm::angleAxis(lastTime, glm::dvec3{0, 0, 1}));
        transforms[0].setLocalSkew({glm::cos(lastTime), 0, 0});
        transforms[1].setLocalRotation(glm::angleAxis(lastTime, glm::dvec3{0, 1, 0}));
        transforms[1].setLocalScale({1, 1, 0.5});
        transforms[2].setPosition({1, 1, 1});
        transforms[3].setScale({1, 1, 1});
        transforms[4].setSkew({0, 0, 0});

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if
        // dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your
        // main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to
        // your main application. Generally you may always pass all inputs to dear
        // imgui, and hide them from your application based on those two flags.

        window.makeCurent();
        glfw.pollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

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
            if (ImGui::Button("CloseMe")) show_another_window = false;
            ImGui::End();
        }

        // Rendering
        glClear(GL_DEPTH_BUFFER_BIT);
        ImGui::Render();
        window.drawBackground(clear_color.x, clear_color.y, clear_color.z);

        auto [display_w, display_h] = window.getFrameBufferSize();

        glm::mat4 mvp   = camera.getMatrix((float)display_w / (float)display_h);
        GLint     mvpID = program.getUniformLocation("MVP");
        glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(mvp));
        GLint objID = program.getUniformLocation("obj");
        for (auto &transform : transforms) {
            glm::mat4 obj = transform.localToWorldMatrix();
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
