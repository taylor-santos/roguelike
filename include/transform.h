//
// Created by taylor-santos on 5/16/2021 at 14:58.
//

#ifndef ROGUELIKE_INCLUDE_TRANSFORM_H
#define ROGUELIKE_INCLUDE_TRANSFORM_H

#include <vector>
#include <ostream>

#define GLM_FORCE_SILENT_WARNINGS // Suppress 'nonstandard extension used: nameless struct/union'
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

class Transform {
public:
    Transform();

    explicit Transform(glm::vec3 position);

    explicit Transform(Transform &parent);

    Transform(Transform &parent, glm::vec3 position);

    ~Transform();

    [[nodiscard]] Transform *
    parent() const;

    Transform &
    setParent(Transform *parent);

    [[nodiscard]] glm::vec3
    position() const;

    Transform &
    setPosition(glm::vec3 position);

    [[nodiscard]] glm::vec3
    localPosition() const;

    Transform &
    setLocalPosition(glm::vec3 localPosition);

    [[nodiscard]] glm::quat
    localRotation() const;

    Transform &
    setLocalRotation(glm::quat localRotation);

    [[nodiscard]] glm::vec3
    localScale() const;

    Transform &
    setLocalScale(glm::vec3 localScale);

    [[nodiscard]] glm::vec3
    localSkew() const;

    Transform &
    setLocalSkew(glm::vec3 localSkew);

    [[nodiscard]] glm::mat4
    parentToLocalMatrix() const;

    [[nodiscard]] glm::mat4
    localToParentMatrix() const;

    [[nodiscard]] glm::mat4
    worldToLocalMatrix() const;

    [[nodiscard]] glm::mat4
    localToWorldMatrix() const;

    Transform(const Transform &) = delete;
    Transform &
    operator=(const Transform &) = delete;

    friend std::ostream &
    operator<<(std::ostream &, const Transform &);

private:
    Transform *              parent_;
    std::vector<Transform *> children_;
    glm::vec3                localPosition_{0, 0, 0};
    glm::quat                localRotation_{1, 0, 0, 0};
    glm::vec3                localScale_{1, 1, 1};
    glm::vec3                localSkew_{0, 0, 0};

private:
    Transform(Transform *parent, glm::vec3 position);
};

#endif // ROGUELIKE_INCLUDE_TRANSFORM_H
