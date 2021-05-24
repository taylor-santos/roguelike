//
// Created by taylor-santos on 5/8/2021 at 00:56.
//

#ifndef ROGUELIKE_INCLUDE_GLFW_H
#define ROGUELIKE_INCLUDE_GLFW_H

#include <array>
#include <functional>

#include "gui.h"

struct GLFWwindow;

namespace GLFW {

enum class Key {
    UNKNOWN       = -1,
    SPACE         = 32,
    APOSTROPHE    = 39, /* ' */
    COMMA         = 44, /* , */
    MINUS         = 45, /* - */
    PERIOD        = 46, /* . */
    SLASH         = 47, /* / */
    ZERO          = 48, /* 0 */
    ONE           = 49, /* 1 */
    TWO           = 50, /* 2 */
    THREE         = 51, /* 3 */
    FOUR          = 52, /* 4 */
    FIVE          = 53, /* 5 */
    SIX           = 54, /* 6 */
    SEVEN         = 55, /* 7 */
    EIGHT         = 56, /* 8 */
    NINE          = 57, /* 9 */
    SEMICOLON     = 59, /* ; */
    EQUAL         = 61, /* = */
    A             = 65,
    B             = 66,
    C             = 67,
    D             = 68,
    E             = 69,
    F             = 70,
    G             = 71,
    H             = 72,
    I             = 73,
    J             = 74,
    K             = 75,
    L             = 76,
    M             = 77,
    N             = 78,
    O             = 79,
    P             = 80,
    Q             = 81,
    R             = 82,
    S             = 83,
    T             = 84,
    U             = 85,
    V             = 86,
    W             = 87,
    X             = 88,
    Y             = 89,
    Z             = 90,
    LEFT_BRACKET  = 91,  /* [ */
    BACKSLASH     = 92,  /* \ */
    RIGHT_BRACKET = 93,  /* ] */
    GRAVE_ACCENT  = 96,  /* ` */
    WORLD_1       = 161, /* non-US #1 */
    WORLD_2       = 162, /* non-US #2 */
    ESCAPE        = 256,
    ENTER         = 257,
    TAB           = 258,
    BACKSPACE     = 259,
    INSERT        = 260,
    DELETE        = 261,
    RIGHT         = 262,
    LEFT          = 263,
    DOWN          = 264,
    UP            = 265,
    PAGE_UP       = 266,
    PAGE_DOWN     = 267,
    HOME          = 268,
    END           = 269,
    CAPS_LOCK     = 280,
    SCROLL_LOCK   = 281,
    NUM_LOCK      = 282,
    PRINT_SCREEN  = 283,
    PAUSE         = 284,
    F1            = 290,
    F2            = 291,
    F3            = 292,
    F4            = 293,
    F5            = 294,
    F6            = 295,
    F7            = 296,
    F8            = 297,
    F9            = 298,
    F10           = 299,
    F11           = 300,
    F12           = 301,
    F13           = 302,
    F14           = 303,
    F15           = 304,
    F16           = 305,
    F17           = 306,
    F18           = 307,
    F19           = 308,
    F20           = 309,
    F21           = 310,
    F22           = 311,
    F23           = 312,
    F24           = 313,
    F25           = 314,
    KP_0          = 320,
    KP_1          = 321,
    KP_2          = 322,
    KP_3          = 323,
    KP_4          = 324,
    KP_5          = 325,
    KP_6          = 326,
    KP_7          = 327,
    KP_8          = 328,
    KP_9          = 329,
    KP_DECIMAL    = 330,
    KP_DIVIDE     = 331,
    KP_MULTIPLY   = 332,
    KP_SUBTRACT   = 333,
    KP_ADD        = 334,
    KP_ENTER      = 335,
    KP_EQUAL      = 336,
    LEFT_SHIFT    = 340,
    LEFT_CONTROL  = 341,
    LEFT_ALT      = 342,
    LEFT_SUPER    = 343,
    RIGHT_SHIFT   = 344,
    RIGHT_CONTROL = 345,
    RIGHT_ALT     = 346,
    RIGHT_SUPER   = 347,
    MENU          = 348,
    COUNT
};

enum class Button {
    ONE   = 0,
    TWO   = 1,
    THREE = 2,
    FOUR  = 3,
    FIVE  = 4,
    SIX   = 5,
    SEVEN = 6,
    EIGHT = 7,
    COUNT,
    LEFT   = ONE,
    RIGHT  = TWO,
    MIDDLE = THREE
};

enum class Action { RELEASE = 0, PRESS = 1, REPEAT = 2 };

class Initializer {
public:
    static Initializer &
    get();

    Initializer(const Initializer &) = delete;

private:
    Initializer();
    ~Initializer();
};

using KeyFun    = std::function<void(int key, int scancode, Action action, int mods)>;
using MouseFun  = std::function<void(int button, Action action, int mods)>;
using CursorFun = std::function<void(double x, double y)>;

class Window {
public:
    static Window &
    get(int width, int height, const char *title);

    ~Window();

    Window(const Window &) = delete;
    Window &
    operator=(const Window &) = delete;

    [[nodiscard]] bool
    shouldClose() const;

    void
    setShouldClose(bool value) const;

    [[nodiscard]] std::pair<int, int>
    getFrameBufferSize() const;

    void
    swapBuffers() const;

    void
    makeCurent() const;

    void
    drawBackground(float r, float g, float b) const;

    void
    updatePlatformWindows() const;

    KeyFun
    registerKeyCallback(Key key, const KeyFun &callback);

    MouseFun
    registerMouseCallback(Button button, const MouseFun &callback);

    CursorFun
    registerCursorCallback(const CursorFun &callback);

    [[nodiscard]] std::pair<double, double>
    getCursorPos() const;

    void
    setCursorPos(double xpos, double ypos) const;
    void
    setCursorPos(std::pair<double, double> pos) const;

    void
    lockCursor();

    void
    unlockCursor() const;

private:
    Initializer &init_ = Initializer::get();

    GLFWwindow *                                                              window_;
    GUI::Context                                                              guiCtx_;
    std::pair<double, double>                                                 prevCursorPos_{};
    std::array<std::pair<KeyFun, bool>, static_cast<size_t>(Key::COUNT)>      keyCallbacks_{};
    std::array<std::pair<MouseFun, bool>, static_cast<size_t>(Button::COUNT)> mouseCallbacks_{};
    CursorFun                                                                 cursorCallback_{};

private:
    Window(int width, int height, const char *title);

private:
    // Encapsulates callback handler functions.
    // Declared as friend to access Window's private callbacks.
    friend struct WindowAccessor;
};

class Manager {
public:
    static Manager &
    get();

    Manager(const Manager &) = delete;

    ~Manager();

    void
    pollEvents() const;

private:
    Manager();
    Initializer &init_ = Initializer::get();
};

} // namespace GLFW

#endif // ROGUELIKE_INCLUDE_GLFW_H
