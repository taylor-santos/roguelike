//
// Created by taylor-santos on 5/10/2021 at 18:38.
//

#ifndef ROGUELIKE_INCLUDE_CAMERA_H
#define ROGUELIKE_INCLUDE_CAMERA_H

#include <utility>

#if defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunknown-warning-option"
#    pragma clang diagnostic ignored "-Wdeprecated-volatile"
#elif defined(__GNUC__) || defined(__GNUG__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wpragmas"
#    pragma GCC diagnostic ignored "-Wvolatile"
#endif

#define GLM_FORCE_SILENT_WARNINGS // Suppress 'nonstandard extension used: nameless struct/union'
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

#if defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#    pragma GCC diagnostic pop
#endif

#include "transform.h"

class Camera {
public:
    Transform transform;

public:
    [[nodiscard]] std::pair<float, float>
    getSensitivity() const;

    void
    setSensitivity(const std::pair<float, float> &sens);

    void
    setSensitivity(float xSens, float ySens);

    [[nodiscard]] float
    getFOV() const;

    void
    setFOV(float fov);

    void
    addRotation(float yaw, float pitch);

    [[nodiscard]] std::pair<float, float>
    getRotation() const;

    void
    setRotation(float yaw, float pitch);

    [[nodiscard]] float
    getNear() const;

    void
    setNear(float near);

    [[nodiscard]] float
    getFar() const;

    void
    setFar(float far);

    [[nodiscard]] glm::vec3
    forward() const;

    [[nodiscard]] glm::vec3
    right() const;

    [[nodiscard]] glm::vec3
    up() const;

    [[nodiscard]] glm::mat4
    getMatrix(float aspect) const;

private:
    std::pair<float, float> sens_{0.1f, 0.1f};
    float                   yaw_{glm::pi<float>()};
    float                   pitch_{0.0f};
    float                   fov_{glm::half_pi<float>()};
    float                   near_{0.01f};
    float                   far_{1000.0f};

public:
private:
    [[nodiscard]] glm::mat4
    viewMatrix() const;
};

#endif // ROGUELIKE_INCLUDE_CAMERA_H
