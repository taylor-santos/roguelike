//
// Created by taylor-santos on 5/24/2021 at 21:34.
//

#pragma once

#include <glad/glad.h>
#include <memory>
#include <vector>
#include <string>

class Shader {
public:
    enum class Type {
        COMPUTE         = GL_COMPUTE_SHADER,
        VERTEX          = GL_VERTEX_SHADER,
        TESS_CONTROL    = GL_TESS_CONTROL_SHADER,
        TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
        GEOMETRY        = GL_GEOMETRY_SHADER,
        FRAGMENT        = GL_FRAGMENT_SHADER
    };
    Shader(const std::string &src, Type type);

    Shader(const Shader &other)     = default;
    Shader(Shader &&other) noexcept = default;
    Shader &
    operator=(const Shader &other) = default;
    Shader &
    operator=(Shader &&other) noexcept = default;
    ~Shader()                          = default;

    friend class ShaderProgram;

private:
    struct ShaderID {
        GLenum id;

        explicit ShaderID(GLenum type);
        ~ShaderID();
    };
    std::shared_ptr<ShaderID> shader_;
};

class ShaderProgram {
public:
    friend class Builder;
    class Builder {
    public:
        Builder()                         = default;
        Builder(const Builder &other)     = default;
        Builder(Builder &&other) noexcept = default;
        Builder &
        operator=(const Builder &other) = default;
        Builder &
        operator=(Builder &&other) noexcept = default;
        ~Builder()                          = default;

        Builder &
        withShader(const Shader &shader);

        [[nodiscard]] ShaderProgram
        build();

    private:
        std::vector<std::shared_ptr<Shader::ShaderID>> shaders_;
    };

    ~ShaderProgram();

    void
    use() const;

    [[nodiscard]] GLint
    getUniformLocation(const std::string &uniform) const;

private:
    GLenum program_;

    explicit ShaderProgram(GLenum program);
};
