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
        return;
    }
    std::stringstream ss;
    ss << "GLFW Error " << error << ": " << desc;
    throw std::runtime_error(ss.str());
}

Initializer &
Initializer::get() {
    static Initializer instance;
    return instance;
}

Initializer::Initializer() {
    if (!glfwInit()) {
        throwError();
    }
}

Initializer::~Initializer() {
    glfwTerminate();
}

void
Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto *handler                      = static_cast<Window *>(glfwGetWindowUserPointer(window));
    auto &[callback, pressWasCaptured] = handler->keyCallbacks_[key];
    if (!callback) return;
    if (action == GLFW_PRESS) {
        if (handler->guiCtx_.wantCaptureKeyboard()) {
            // ImGUI captured this press action, so any following non-press actions (repeat/release)
            // should be ignored.
            pressWasCaptured = true;
        } else {
            // ImGUI *did not* capture this press action, so any following non-press actions should
            // *not* be ignored, regardless of whether ImGUI wants to capture them or not.
            pressWasCaptured = false;
            callback(key, scancode, static_cast<Action>(action), mods);
        }
    } else if (!pressWasCaptured) {
        // Even if ImGUI wants to capture a non-press action, we should still send it to the
        // callback iif the initial press action was *not* captured. This allows callback functions
        // to assume that any press actions it receives will always get an associated release. On
        // the other hand, if the initial press *was* captured, any further non-press actions should
        // *not* be sent to the callback.
        callback(key, scancode, static_cast<Action>(action), mods);
    }
}

void
Window::mouseCallback(GLFWwindow *window, int button, int action, int mods) {
    auto *handler                      = static_cast<Window *>(glfwGetWindowUserPointer(window));
    auto &[callback, pressWasCaptured] = handler->mouseCallbacks_[button];
    if (!callback) return;
    if (action == GLFW_PRESS) {
        if (handler->guiCtx_.wantCaptureMouse()) {
            pressWasCaptured = true;
        } else {
            pressWasCaptured = false;
            callback(button, static_cast<Action>(action), mods);
        }
    } else if (!pressWasCaptured) {
        callback(button, static_cast<Action>(action), mods);
    }
}

void
Window::cursorCallback(GLFWwindow *window, double x, double y) {
    auto *handler  = static_cast<Window *>(glfwGetWindowUserPointer(window));
    auto  callback = handler->cursorCallback_;
    if (callback) {
        callback(x, y);
    }
}

static GLFWwindow *
createWindow(int width, int height, const char *title) {
    GLFW::Initializer::get();
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
#else
    // GL 3.0 + GLSL 130
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif
    // Enable 4xMSAA
    glfwWindowHint(GLFW_SAMPLES, 4);
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

Window::Window(int width, int height, const char *title)
    : window_{createWindow(width, height, title)}
    , guiCtx_() {
    // Point the glfwWindow's User Pointer to this instance to be used in callback functions.
    glfwSetWindowUserPointer(window_, this);
    // Set callback functions
    glfwSetKeyCallback(window_, keyCallback);
    glfwSetMouseButtonCallback(window_, mouseCallback);
    glfwSetCursorPosCallback(window_, cursorCallback);
    // Tell ImGui to insert its own callbacks after we've set our own.
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char *glsl_version = "#version 100";
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);
    glEnable(GL_MULTISAMPLE);
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
Window::drawBackground(float r, float g, float b) const {
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
Window::registerKeyCallback(Key key, const KeyFun &callback) {
    int k = static_cast<int>(key);
    if (k < 0 || GLFW_KEY_LAST <= k) {
        std::stringstream ss;
        ss << "error: key " << k << " is outside the valid range [0, " << GLFW_KEY_LAST << ")";
        throw std::invalid_argument(ss.str());
    }
    return std::exchange(keyCallbacks_[k].first, callback);
}

MouseFun
Window::registerMouseCallback(Button button, const MouseFun &callback) {
    int b = static_cast<int>(button);
    if (b < 0 || GLFW_MOUSE_BUTTON_LAST <= b) {
        std::stringstream ss;
        ss << "error: button " << b << " is outside the valid range [0, " << GLFW_MOUSE_BUTTON_LAST
           << ")";
        throw std::invalid_argument(ss.str());
    }
    return std::exchange(mouseCallbacks_[b].first, callback);
}

CursorFun
Window::registerCursorCallback(const CursorFun &callback) {
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
