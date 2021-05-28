//
// Created by taylor-santos on 5/24/2021 at 21:34.
//

#include "shader.h"

#include <stdexcept>

#include "doctest/doctest.h"
#include "glfw.h"

static std::string
glErrorString(GLenum error) {
    switch (error) {
        case GL_NO_ERROR: return "GL_NO_ERROR: No error has been recorded.";
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated "
                   "argument.";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE: A numeric argument is out of range.";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION: The specified operation is not allowed in the current "
                   "state.";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that "
                   "would cause an internal stack to underflow.";
        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would "
                   "cause an internal stack to overflow.";
        default: return "No Description";
    }
}

static void
glCheckError() {
    GLint status = glGetError();
    if (status != GL_NO_ERROR) {
        throw std::runtime_error(glErrorString(status));
    }
}

Shader::Shader(const std::string &src, Type type)
    : shader_{std::make_shared<ShaderID>(static_cast<GLenum>(type))} {
    CHECK(glIsShader(shader_->id));
    const char *src_c = src.c_str();
    glShaderSource(shader_->id, 1, (const GLchar **)&src_c, nullptr);
    glCheckError();

    glCompileShader(shader_->id);
    // Check compilation status: this will report syntax errors
    GLint status;
    glGetShaderiv(shader_->id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader_->id, GL_INFO_LOG_LENGTH, &maxLength);
        // The maxLength includes the NULL character
        std::string errorLog(maxLength, '\0');
        glGetShaderInfoLog(shader_->id, maxLength, &maxLength, &errorLog[0]);
        throw std::invalid_argument(errorLog);
    }
}

Shader::ShaderID::ShaderID(GLenum type)
    : id{glCreateShader(type)} {
    if (id == 0) {
        throw std::runtime_error(glErrorString(glGetError()));
    }
    CHECK(glIsShader(id));
}
Shader::ShaderID::~ShaderID() {
    CHECK(glIsShader(id));
    glDeleteShader(id);
    glCheckError();
}

ShaderProgram::ShaderProgram(GLenum program)
    : program_{program} {
    CHECK(glIsProgram(program_));
}

ShaderProgram::~ShaderProgram() {
    CHECK(glIsProgram(program_));
    glDeleteProgram(program_);
    glCheckError();
}

void
ShaderProgram::use() const {
    glUseProgram(program_);
    glCheckError();
    GLint curr = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &curr);
    CHECK(curr == program_);
}

GLint
ShaderProgram::getUniformLocation(const std::string &uniform) const {
    GLint loc = glGetUniformLocation(program_, uniform.c_str());
    glCheckError();
    return loc;
}

ShaderProgram::Builder &
ShaderProgram::Builder::withShader(const Shader &shader) {
    shaders_.emplace_back(shader.shader_);
    return *this;
}

ShaderProgram
ShaderProgram::Builder::build() {
    auto program = glCreateProgram();
    if (program == 0) {
        throw std::runtime_error(glErrorString(glGetError()));
    }
    for (auto &shader : shaders_) {
        glAttachShader(program, shader->id);
        auto status = glGetError();
        if (status != GL_NO_ERROR) {
            GLint len = 0;
            glGetShaderiv(shader->id, GL_INFO_LOG_LENGTH, &len);
            std::string infoLog(len, '\0');
            glGetShaderInfoLog(shader->id, len, &len, &infoLog[0]);
            glDeleteProgram(program);
            throw std::runtime_error(glErrorString(glGetError()));
        }
    }
    glLinkProgram(program);
    glCheckError();
    GLint linkStatus = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
        // The maxLength includes the NULL character
        std::string errorLog(maxLength, '\0');
        glGetProgramInfoLog(program, maxLength, &maxLength, &errorLog[0]);
        glDeleteProgram(program);
        throw std::runtime_error(errorLog);
    }
    for (auto &shader : shaders_) {
        glDetachShader(program, shader->id);
        glCheckError();
    }
    return ShaderProgram(program);
}

TEST_SUITE_BEGIN("Shader");

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
        Shader shader(src, Shader::Type::FRAGMENT);
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

TEST_CASE("glErrorString") {
    glErrorString(GL_NO_ERROR);
    glErrorString(GL_INVALID_ENUM);
    glErrorString(GL_INVALID_VALUE);
    glErrorString(GL_INVALID_OPERATION);
    glErrorString(GL_INVALID_FRAMEBUFFER_OPERATION);
    glErrorString(GL_OUT_OF_MEMORY);
    glErrorString(GL_STACK_UNDERFLOW);
    glErrorString(GL_STACK_OVERFLOW);
}
