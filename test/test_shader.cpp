//
// Created by taylor-santos on 5/24/2021 at 21:34.
//

#include "shader.h"
#include "doctest/doctest.h"

#include "glfw.h"

TEST_SUITE_BEGIN("Shader");

#ifndef DISABLE_RENDER_TESTS

TEST_CASE("SyntaxError") {
    GLFW::Window::get(500, 500, "window");
    CHECK_THROWS(Shader("this is a syntax error", Shader::Type::FRAGMENT));
}

TEST_CASE("ShaderProgram") {
    GLFW::Window::get(500, 500, "window");
    auto builder = ShaderProgram::Builder();
    SUBCASE("WithShader") {
        std::string src = "#version 140\n"
                          "out vec4 outputColor;"
                          "uniform vec3 aUniform;"
                          "void main() {"
                          "  outputColor = vec4(aUniform, 1);"
                          "}";
        Shader      shader(src, Shader::Type::FRAGMENT);
        builder.withShader(shader);
        SUBCASE("Build") {
            CHECK_NOTHROW((void)builder.build());
        }
        SUBCASE("Use") {
            auto program = builder.build();
            program.use();
        }
        SUBCASE("GetUniformLocation") {
            auto program = builder.build();
            CHECK(program.getUniformLocation("aUniform") != -1);
        }
        SUBCASE("InvalidUniform") {
            auto program = builder.build();
            CHECK(program.getUniformLocation("invalidName") == -1);
        }
        SUBCASE("ShaderAttachedTwice") {
            builder.withShader(shader);
            CHECK_THROWS((void)builder.build());
        }
    }
    SUBCASE("UnresolvedFunction") {
        std::string src = "#version 140\n"
                          "void foo();"
                          "void main() {"
                          "  foo();"
                          "}";
        builder.withShader(Shader(src, Shader::Type::FRAGMENT));
        CHECK_THROWS((void)builder.build());
    }
}

#endif // DISABLE_RENDER_TESTS
