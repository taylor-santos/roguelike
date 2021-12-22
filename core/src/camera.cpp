//
// Created by taylor-santos on 5/10/2021 at 18:38.
//

#include "camera.h"

#include "transform.h"

std::pair<float, float>
Camera::getSensitivity() const {
    return sens_;
}

void
Camera::setSensitivity(const std::pair<float, float> &sens) {
    sens_ = sens;
}

void
Camera::setSensitivity(float xSens, float ySens) {
    sens_ = {xSens, ySens};
}

float
Camera::getFOV() const {
    return glm::degrees(fov_);
}

void
Camera::setFOV(float fov) {
    fov_ = glm::radians(fov);
}

void
Camera::addRotation(float yaw, float pitch) {
    auto [xSens, ySens] = sens_;
    yaw_                = glm::radians(glm::mod(glm::degrees(yaw_) + yaw * xSens, 360.0f));
    pitch_ = glm::radians(glm::clamp(glm::degrees(pitch_) + pitch * ySens, -90.0f, 90.0f));
}

std::pair<float, float>
Camera::getRotation() const {
    return {glm::degrees(yaw_), glm::degrees(pitch_)};
}

void
Camera::setRotation(float yaw, float pitch) {
    yaw_   = glm::radians(glm::mod(yaw, 360.0f));
    pitch_ = glm::radians(glm::clamp(pitch, -90.0f, 90.0f));
}

float
Camera::getNear() const {
    return near_;
}

void
Camera::setNear(float near) {
    near_ = near;
}

float
Camera::getFar() const {
    return far_;
}

void
Camera::setFar(float far) {
    far_ = far;
}

glm::vec3
Camera::forward() const {
    auto cosP = glm::cos(pitch_);
    auto x    = -glm::sin(yaw_) * cosP;
    auto y    = glm::sin(pitch_);
    auto z    = glm::cos(yaw_) * cosP;
    auto f    = glm::vec3(x, y, z);
    return f;
}

glm::vec3
Camera::right() const {
    return glm::vec3{-glm::cos(yaw_), 0, -glm::sin(yaw_)};
}

glm::vec3
Camera::up() const {
    return {
        glm::sin(pitch_) * glm::sin(yaw_),
        glm::cos(pitch_),
        -glm::cos(yaw_) * glm::sin(pitch_)};
}

glm::mat4
Camera::viewMatrix() const {
    glm::vec3 pos = transform.position();
    return glm::lookAt(pos, pos + forward(), up());
}

glm::mat4
Camera::getMatrix(float width, float height) const {
    auto aspect     = width / height;
    auto model      = glm::mat4(1.0f);
    auto view       = viewMatrix();
    auto projection = glm::perspective(fov_, aspect, near_, far_);
    auto mvp        = projection * view * model;
    return mvp;
}
