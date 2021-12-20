//
// Created by taylor-santos on 12/15/2021 at 16:27.
//

#include "doctest/doctest.h"

#include "../core/include/glfw.h"

#include <GLFW/glfw3.h>

// Forward-declare GLFW's internal input handlers defined in glfw/src/internal.h, so that they may
// be invoked by the unit tests to emulate user input.
extern "C" {
extern void
_glfwInputCursorPos(struct _GLFWwindow *window, double xpos, double ypos);
extern void
_glfwInputKey(struct _GLFWwindow *window, int key, int scancode, int action, int mods);
extern void
_glfwInputMouseClick(struct _GLFWwindow *window, int button, int action, int mods);
}

TEST_SUITE_BEGIN("GLFW");
DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wunused-variable")

TEST_CASE("WindowSingletonConstructor") {
    CHECK_NOTHROW(GLFW::Window::get(200, 100, "title"));
}

TEST_CASE("WindowSetShouldClose") {
    auto &window = GLFW::Window::get(200, 100, "title");
    SUBCASE("false") {
        window.setShouldClose(false);
        CHECK(window.shouldClose() == false);
    }
    SUBCASE("true") {
        window.setShouldClose(true);
        CHECK(window.shouldClose() == true);
    }
}

TEST_CASE("WindowGetFrameBufferSizeShouldBeWindowSize") {
    auto &window = GLFW::Window::get(200, 100, "title");
    auto [x, y]  = window.getFrameBufferSize();
    SUBCASE("X") {
        CHECK(x == 200);
    }
    SUBCASE("Y") {
        CHECK(y == 100);
    }
}

TEST_CASE("SwapBuffers") {
    auto &window = GLFW::Window::get(200, 100, "title");
    window.swapBuffers();
}

TEST_CASE("WindowMakeCurrent") {
    auto &window = GLFW::Window::get(200, 100, "title");
    window.makeCurent();
}

TEST_CASE("WindowDrawBackground") {
    auto &window = GLFW::Window::get(200, 100, "title");
    window.drawBackground(0.5f, 1.0f, 0.8f);
}

TEST_CASE("WindowRegisterKeyCallback") {
    bool called   = false;
    auto callback = [&called](int, int, GLFW::Action, int) {
        called = true;
    };
    auto &window = GLFW::Window::get(200, 100, "title");
    SUBCASE("ValidKey") {
        window.registerKeyCallback(GLFW::Key::SPACE, callback);
        GLFWwindow *windowPtr = glfwGetCurrentContext();

        SUBCASE("BeforeCallback") {
            CHECK(called == false);
        }
        SUBCASE("AfterCallback") {
            _glfwInputKey((_GLFWwindow *)windowPtr, GLFW_KEY_SPACE, 0, GLFW_PRESS, 0);
            CHECK(called == true);
        }
        SUBCASE("DifferentKey") {
            _glfwInputKey((_GLFWwindow *)windowPtr, GLFW_KEY_ESCAPE, 0, GLFW_PRESS, 0);
            CHECK(called == false);
        }
    }
    SUBCASE("InvalidKey") {
        CHECK_THROWS(window.registerKeyCallback(GLFW::Key::COUNT, callback));
    }
}

TEST_CASE("WindowRegisterMouseCallback") {
    bool called   = false;
    auto callback = [&called](int, GLFW::Action, int) {
        called = true;
    };
    auto &window = GLFW::Window::get(200, 100, "title");
    SUBCASE("ValidKey") {
        window.registerMouseCallback(GLFW::Button::LEFT, callback);
        auto *windowPtr = glfwGetCurrentContext();

        SUBCASE("BeforeCallback") {
            CHECK(called == false);
        }
        SUBCASE("AfterCallback") {
            _glfwInputMouseClick((_GLFWwindow *)windowPtr, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
            CHECK(called == true);
        }
        SUBCASE("DifferentButton") {
            _glfwInputMouseClick((_GLFWwindow *)windowPtr, GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS, 0);
            CHECK(called == false);
        }
    }
    SUBCASE("InvalidKey") {
        CHECK_THROWS(window.registerMouseCallback(GLFW::Button::COUNT, callback));
    }
}

TEST_CASE("WindowRegisterCursorCallback") {
    bool called   = false;
    auto callback = [&called](double, double) {
        called = true;
    };
    auto &window = GLFW::Window::get(200, 100, "title");
    window.registerCursorCallback(callback);
    auto *windowPtr = glfwGetCurrentContext();

    SUBCASE("BeforeCallback") {
        CHECK(called == false);
    }
    SUBCASE("AfterCallback") {
        _glfwInputCursorPos((_GLFWwindow *)windowPtr, 5, 10);
        CHECK(called == true);
    }
}

TEST_CASE("WindowGetSetCursorPos") {
    auto &window = GLFW::Window::get(200, 100, "title");
    SUBCASE("TwoArgs") {
        window.setCursorPos(1, 2);
        SUBCASE("ZeroedCursorPos") {
            auto [x, y] = window.getCursorPos();
            SUBCASE("X") {
                CHECK(x == 1);
            }
            SUBCASE("Y") {
                CHECK(y == 2);
            }
        }
    }
    SUBCASE("Pair") {
        window.setCursorPos({3, 4});
        SUBCASE("ZeroedCursorPos") {
            auto [x, y] = window.getCursorPos();
            SUBCASE("X") {
                CHECK(x == 3);
            }
            SUBCASE("Y") {
                CHECK(y == 4);
            }
        }
    }
}

TEST_CASE("WindowLockCursor") {
    auto &window = GLFW::Window::get(200, 100, "title");
    SUBCASE("Locked") {
        window.lockCursor();
        SUBCASE("UnlockLocked") {
            window.unlockCursor();
        }
        SUBCASE("LockLocked") {
            window.lockCursor();
        }
    }
    SUBCASE("Unlocked") {
        window.unlockCursor();
        SUBCASE("UnlockUnlocked") {
            window.unlockCursor();
        }
        SUBCASE("LockUnlocked") {
            window.lockCursor();
        }
    }
}

DOCTEST_CLANG_SUPPRESS_WARNING_POP
