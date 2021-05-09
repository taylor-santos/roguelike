//
// Created by taylor-santos on 5/8/2021 at 00:56.
//

#include "glfw.h"

#include <glad/glad.h>
// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <sstream>
#include <utility>

#include "doctest/doctest.h"

TEST_SUITE_BEGIN("GLFW");

// Forward-declare ImGui's callback registration
bool
ImGui_ImplGlfw_InitForOpenGL(GLFWwindow *window, bool install_callbacks);
bool
ImGui_ImplOpenGL3_Init(const char *glsl_version);

int keyCount = GLFW_KEY_LAST;

namespace GLFW {

static void
throwError() {
    const char *desc;
    int         error = glfwGetError(&desc);
    if (error == GLFW_NO_ERROR) {
        throw std::logic_error("throwError() called, but no error was detected");
    }
    std::stringstream ss;
    ss << "GLFW Error " << error << ": " << desc;
    throw std::runtime_error(ss.str());
}

TEST_CASE("throwErrorShouldThrow") {
    CHECK_THROWS(throwError());
}

Initializer &
Initializer::get() {
    static Initializer instance;
    return instance;
}

TEST_CASE("InitializerShouldBeSingleton") {
    auto &init1 = Initializer::get();
    auto &init2 = Initializer::get();

    CHECK(&init1 == &init2);
}

Initializer::Initializer() {
    if (!glfwInit()) {
        throwError();
    }
}

Initializer::~Initializer() {
    glfwTerminate();
}

/***
 * Friend struct of Window class. Encapsulates all the GLFW input callback functions. Each function
 * retrieves the GLFWwindow's associated Window instance via the GLFW User Pointer, which is
 * initialized to point to the Window instance on its construction.
 */
struct WindowAccessor {
    static void
    keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        auto *handler = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (handler->guiCtx_.wantCaptureKeyboard()) {
            return;
        }
        auto callback = handler->keyCallbacks_[key];
        if (callback) {
            callback(key, scancode, action, mods);
        }
    }

    static void
    mouseCallback(GLFWwindow *window, int button, int action, int mods) {
        auto *handler = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (handler->guiCtx_.wantCaptureMouse()) {
            return;
        }
        auto callback = handler->mouseCallbacks_[button];
        if (callback) {
            callback(button, action, mods);
        }
    }

    static void
    cursorCallback(GLFWwindow *window, double x, double y) {
        auto *handler  = static_cast<Window *>(glfwGetWindowUserPointer(window));
        auto  callback = handler->cursorCallback_;
        if (callback) {
            callback(x, y);
        }
    }
};

static GLFWwindow *
createWindow(int width, int height, const char *title) {
    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        throwError();
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        throw std::runtime_error("Failed to initialize OpenGL loader");
    }
    return window;
}

TEST_CASE("WindowCreation") {
    glfwInit();
    auto *window = createWindow(100, 100, "foo");
    SUBCASE("WindowShouldNotBeNull") {
        CHECK(window != nullptr);
    }
    SUBCASE("CurrentContextShouldBeWindow") {
        CHECK(window == glfwGetCurrentContext());
    }
    glfwTerminate();
}

Window::Window(int width, int height, const char *title)
    : window_{createWindow(width, height, title)}
    , guiCtx_{GUI::Context()} {
    // Point the glfwWindow's User Pointer to this instance to be used in callback functions.
    glfwSetWindowUserPointer(window_, this);
    // Set callback functions
    glfwSetKeyCallback(window_, WindowAccessor::keyCallback);
    glfwSetMouseButtonCallback(window_, WindowAccessor::mouseCallback);
    glfwSetCursorPosCallback(window_, WindowAccessor::cursorCallback);
    // Tell ImGui to insert its own callbacks after we've set our own.
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char *glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);
}

Window &
Window::get(int width, int height, const char *title) {
    static Window instance(width, height, title);
    return instance;
}

Window::~Window() {
    glfwDestroyWindow(window_);
}

bool
Window::shouldClose() const {
    return glfwWindowShouldClose(window_);
}

void
Window::setShouldClose(bool value) const {
    glfwSetWindowShouldClose(window_, value);
}

std::pair<int, int>
Window::getFrameBufferSize() const {
    int display_w, display_h;
    glfwGetFramebufferSize(window_, &display_w, &display_h);
    return {display_w, display_h};
}

void
Window::swapBuffers() const {
    glfwSwapBuffers(window_);
}

void
Window::makeCurent() const {
    glfwMakeContextCurrent(window_);
    guiCtx_.makeCurrent();
}

void
Window::render(float r, float g, float b) const {
    auto [display_w, display_h] = getFrameBufferSize();

    glViewport(0, 0, display_w, display_h);
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void
Window::updatePlatformWindows() const {
    auto *window = glfwGetCurrentContext();
    guiCtx_.updatePlatformWindows();
    glfwMakeContextCurrent(window);
}

KeyFun
Window::registerKeyCallback(Key key, KeyFun callback) {
    int k = static_cast<int>(key);
    if (k < 0 || GLFW_KEY_LAST <= k) {
        std::stringstream ss;
        ss << "error: key " << k << " is outside the valid range [0, " << GLFW_KEY_LAST << ")";
        throw std::invalid_argument(ss.str());
    }
    return std::exchange(keyCallbacks_[k], callback);
}

MouseFun
Window::registerMouseCallback(Button button, MouseFun callback) {
    int b = static_cast<int>(button);
    if (b < 0 || GLFW_MOUSE_BUTTON_LAST <= b) {
        std::stringstream ss;
        ss << "error: button " << b << " is outside the valid range [0, " << GLFW_MOUSE_BUTTON_LAST
           << ")";
        throw std::invalid_argument(ss.str());
    }
    return std::exchange(mouseCallbacks_[b], callback);
}

CursorFun
Window::registerCursorCallback(CursorFun callback) {
    return std::exchange(cursorCallback_, callback);
}

std::pair<double, double>
Window::getCursorPos() const {
    double x, y;
    glfwGetCursorPos(window_, &x, &y);
    return {x, y};
}

void
Window::setCursorPos(double xpos, double ypos) const {
    glfwSetCursorPos(window_, xpos, ypos);
}
void
Window::setCursorPos(std::pair<double, double> pos) const {
    glfwSetCursorPos(window_, pos.first, pos.second);
}

void
Window::lockCursor() {
    if (glfwGetInputMode(window_, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        return;
    }
    prevCursorPos_ = getCursorPos();
    // Set cursor mode
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    // Set the cursor position to 0,0 initially so it doesn't jump when first moved.
    glfwSetCursorPos(window_, 0, 0);
}

void
Window::unlockCursor() const {
    if (glfwGetInputMode(window_, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
        return;
    }
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    setCursorPos(prevCursorPos_);
}

void
Manager::pollEvents() const {
    glfwPollEvents();
}

Manager &
Manager::get() {
    static Manager instance;
    return instance;
}
Manager::Manager() = default;

Manager::~Manager() = default;

} // namespace GLFW
