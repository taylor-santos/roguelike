//
// Created by taylor-santos on 5/16/2021 at 15:10.
//
#include "transform.h"

#include <functional>
#include <ostream>
#include <iostream>

#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtc/random.hpp"
#include "doctest/doctest.h"
#include "util.h"

static glm::mat3
cholesky(glm::mat3 A) {
    // https://rosettacode.org/wiki/Cholesky_decomposition
    auto l11 = glm::sqrt(A[0][0]);
    auto l21 = 1.f / l11 * A[0][1];
    auto l22 = glm::sqrt(A[1][1] - l21 * l21);
    auto l31 = 1.f / l11 * A[0][2];
    auto l32 = 1.f / l22 * (A[1][2] - l31 * l21);
    auto l33 = glm::sqrt(A[2][2] - (l31 * l31 + l32 * l32));
    return glm::mat3{{l11, 0, 0}, {l21, l22, 0}, {l31, l32, l33}};
};

static void
decompose(glm::mat4 mat, glm::vec3 &T, glm::mat3 &R, glm::vec3 &Z, glm::vec3 &S) {
    // https://github.com/matthew-brett/transforms3d/blob/master/transforms3d/affines.py
    T             = mat[3];
    glm::mat3 RZS = mat;
    auto      ZS  = cholesky(glm::transpose(RZS) * RZS);
    Z             = glm::vec3{ZS[0][0], ZS[1][1], ZS[2][2]};
    glm::mat3 shears{ZS[0] / Z[0], ZS[1] / Z[1], ZS[2] / Z[2]};
    S = glm::vec3{shears[1][0], shears[2][0], shears[2][1]};
    R = RZS * glm::inverse(ZS);
    if (glm::determinant(R) < 0) {
        Z[0] *= -1;
        ZS[0] *= -1;
        R = RZS * glm::inverse(ZS);
    }
    R = glm::transpose(R);
}

static glm::quat
matToQuat(glm::mat3 mat) {
    // https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
    glm::quat q;
    float trace = mat[0][0] + mat[1][1] + mat[2][2]; // I removed + 1.0f; see discussion with Ethan
    if (trace > 0) {                                 // I changed M_EPSILON to 0
        float s = 0.5f / sqrtf(trace + 1.0f);
        q.w     = 0.25f / s;
        q.x     = (mat[1][2] - mat[2][1]) * s;
        q.y     = (mat[2][0] - mat[0][2]) * s;
        q.z     = (mat[0][1] - mat[1][0]) * s;
    } else {
        if (mat[0][0] > mat[1][1] && mat[0][0] > mat[2][2]) {
            float s = 2.0f * sqrtf(1.0f + mat[0][0] - mat[1][1] - mat[2][2]);
            q.w     = (mat[1][2] - mat[2][1]) / s;
            q.x     = 0.25f * s;
            q.y     = (mat[1][0] + mat[0][1]) / s;
            q.z     = (mat[2][0] + mat[0][2]) / s;
        } else if (mat[1][1] > mat[2][2]) {
            float s = 2.0f * sqrtf(1.0f + mat[1][1] - mat[0][0] - mat[2][2]);
            q.w     = (mat[2][0] - mat[0][2]) / s;
            q.x     = (mat[1][0] + mat[0][1]) / s;
            q.y     = 0.25f * s;
            q.z     = (mat[2][1] + mat[1][2]) / s;
        } else {
            float s = 2.0f * sqrtf(1.0f + mat[2][2] - mat[0][0] - mat[1][1]);
            q.w     = (mat[0][1] - mat[1][0]) / s;
            q.x     = (mat[2][0] + mat[0][2]) / s;
            q.y     = (mat[2][1] + mat[1][2]) / s;
            q.z     = 0.25f * s;
        }
    }
    return q;
}

Transform::Transform(Transform *parent, glm::vec3 position)
    : parent_{parent}
    , localPosition_{position} {
    if (parent_) {
        parent_->children_.emplace_back(this);
    }
}

Transform::Transform()
    : Transform(nullptr, {0, 0, 0}) {}

Transform::Transform(glm::vec3 position)
    : Transform(nullptr, position) {}

Transform::Transform(Transform &parent)
    : Transform(&parent, {0, 0, 0}) {}

Transform::Transform(Transform &parent, glm::vec3 position)
    : Transform(&parent, position) {}

Transform::~Transform() {
    if (parent_) {
        parent_->children_.erase(
            std::remove(parent_->children_.begin(), parent_->children_.end(), this),
            parent_->children_.end());
    }
    for (auto child : children_) {
        // TODO: make this a method call that sets its transform relative to global
        child->parent_ = nullptr;
    }
}

Transform *
Transform::parent() const {
    return parent_;
}

Transform &
Transform::setParent(Transform *parent) {
    const glm::mat4 I(1.0f);
    // Get the current world transformation
    auto mat = localToWorldMatrix();
    if (parent) {
        // Remove the new parent's transformation from the matrix;
        mat = parent->worldToLocalMatrix() * mat;
    }
    glm::vec3 position{};
    glm::mat3 rotation{};
    glm::vec3 scale{};
    glm::vec3 skew{};
    decompose(mat, position, rotation, scale, skew);
    parent_        = parent;
    localPosition_ = position;
    localRotation_ = matToQuat(rotation);
    localScale_    = scale;
    localSkew_     = skew;
    return *this;
}

glm::vec3
Transform::position() const {
    // TODO: Transform into world space
    return localPosition_;
}

Transform &
Transform::setPosition(glm::vec3 position) {
    if (parent_) {
        auto vec = parent_->worldToLocalMatrix() * glm::vec4{position, 1};
        position = vec / vec.w;
    }
    localPosition_ = position;
    return *this;
}

glm::vec3
Transform::localPosition() const {
    return localPosition_;
}

Transform &
Transform::setLocalPosition(glm::vec3 localPosition) {
    localPosition_ = localPosition;
    return *this;
}

glm::quat
Transform::localRotation() const {
    return localRotation_;
}

Transform &
Transform::setLocalRotation(glm::quat localRotation) {
    localRotation_ = localRotation;
    return *this;
}

glm::vec3
Transform::localScale() const {
    return localScale_;
}

Transform &
Transform::setLocalScale(glm::vec3 localScale) {
    localScale_ = localScale;
    return *this;
}

[[nodiscard]] glm::vec3
Transform::localSkew() const {
    return localSkew_;
}

Transform &
Transform::setLocalSkew(glm::vec3 localSkew) {
    localSkew_ = localSkew;
    return *this;
}

glm::mat4
Transform::parentToLocalMatrix() const {
    const glm::mat4 I(1.0f);
    glm::mat4       Sc = glm::scale(I, 1.0f / localScale_);
    glm::mat4       Sk(1.0f);
    Sk[1][0]    = -localSkew_.x;
    Sk[2][0]    = localSkew_.x * localSkew_.z - localSkew_.y;
    Sk[2][1]    = -localSkew_.z;
    glm::mat4 R = glm::toMat4(localRotation_);
    glm::mat4 T = glm::translate(I, -localPosition_);
    return Sc * Sk * R * T;
}

glm::mat4
Transform::localToParentMatrix() const {
    glm::mat4 I(1.0f);
    glm::mat4 T = glm::translate(I, localPosition_);
    glm::mat4 R = glm::toMat4(glm::inverse(localRotation_));
    glm::mat4 Sk(1.0f);
    Sk[1][0]     = localSkew_.x;
    Sk[2][0]     = localSkew_.y;
    Sk[2][1]     = localSkew_.z;
    glm::mat4 Sc = glm::scale(I, localScale_);
    return T * R * Sk * Sc;
}

glm::mat4
Transform::worldToLocalMatrix() const {
    glm::mat4 mat  = parentToLocalMatrix();
    auto      curr = this->parent_;
    while (curr) {
        mat  = mat * curr->parentToLocalMatrix();
        curr = curr->parent_;
    }
    return mat;
}
glm::mat4
Transform::localToWorldMatrix() const {
    glm::mat4 mat  = localToParentMatrix();
    auto      curr = this->parent_;
    while (curr) {
        mat  = curr->localToParentMatrix() * mat;
        curr = curr->parent_;
    }
    return mat;
}

std::ostream &
operator<<(std::ostream &os, const Transform &t) {
    os << "{\n\t";
    os << R"("position":)" << t.localPosition_ << "\",\n\t";
    os << R"("rotation":)" << t.localRotation_ << "\",\n\t";
    os << R"("scale":)" << t.localScale_ << "\",\n\t";
    os << R"("skew":)" << t.localSkew_ << "\",\n";
    os << "}";
    return os;
}

TEST_SUITE_BEGIN("Transform");

template<int N, typename T>
void
checkVecEquality(glm::vec<N, T, glm::defaultp> a, glm::vec<N, T, glm::defaultp> b) {
    for (int i = 0; i < N; ++i) {
        CHECK(a[i] == doctest::Approx(b[i]));
    }
}

template<int N, typename T>
void
checkMatEquality(glm::mat<N, N, T, glm::defaultp> a, glm::mat<N, N, T, glm::defaultp> b) {
    for (int i = 0; i < N; ++i) {
        checkVecEquality(a[i], b[i]);
    }
}

template<typename T>
void
checkQuatEquality(glm::qua<T, glm::defaultp> a, glm::qua<T, glm::defaultp> b) {
    auto matA = glm::toMat4(a);
    auto matB = glm::toMat4(b);
    checkMatEquality(matA, matB);
}

#define VERIFY_TRANSFORM(transform)                                                 \
    do {                                                                            \
        const auto &t           = (transform);                                      \
        glm::vec3   scale       = t.localScale();                                   \
        glm::quat   rotation    = t.localRotation();                                \
        glm::mat3   rotMat      = toMat3(rotation);                                 \
        glm::vec3   translation = t.localPosition();                                \
        glm::vec3   skew        = t.localSkew();                                    \
        glm::mat4   mat         = t.localToParentMatrix();                          \
        glm::vec3   expectScale{};                                                  \
        glm::mat3   expectRotation{};                                               \
        glm::vec3   expectTranslation{};                                            \
        glm::vec3   expectSkew{};                                                   \
        glm::vec4   expectPerspective{};                                            \
        decompose(mat, expectTranslation, expectRotation, expectScale, expectSkew); \
        SUBCASE("Scale") {                                                          \
            checkVecEquality(scale, expectScale);                                   \
        }                                                                           \
        SUBCASE("Rotation") {                                                       \
            checkMatEquality(rotMat, expectRotation);                               \
            checkQuatEquality(rotation, matToQuat(expectRotation));                 \
        }                                                                           \
        SUBCASE("Translation") {                                                    \
            checkVecEquality(translation, expectTranslation);                       \
        }                                                                           \
        SUBCASE("Skew") {                                                           \
            checkVecEquality(skew, expectSkew);                                     \
        }                                                                           \
        SUBCASE("Inverse") {                                                        \
            glm::mat4 inv = t.parentToLocalMatrix();                                \
            checkMatEquality(inv, glm::inverse(mat));                               \
        }                                                                           \
    } while (0)

TEST_CASE("foo") {
    {
        glm::mat4 M{
            {1, 0, 0, 0},
            {-0.406486958, -0.0448754206, 1.03280044, 0},
            {-1.21086121, -0.970314204, 0.0477686599, 0},
            {-0.222729996, -0.0297529995, -0.107712999, 1}};
        glm::vec3 pos;
        glm::mat3 rot;
        glm::vec3 scl;
        glm::vec3 skw;
        decompose(M, pos, rot, scl, skw);
    }

    Transform t;
    for (int i = 0; i < 1000; ++i) {
        auto pos = glm::linearRand(glm::vec3{-10, -10, -10}, glm::vec3{10, 10, 10});
        INFO("position: ", pos);
        t.setLocalPosition(pos);

        auto rot = glm::angleAxis(
            glm::linearRand(0.0f, 2.0f * glm::pi<float>()),
            glm::sphericalRand(1.0f));
        auto rotMat = glm::toMat3(rot);
        (void)rotMat;
        INFO("rotation: ", rot);
        t.setLocalRotation(rot);

        auto scale = glm::linearRand(glm::vec3(0.0f), glm::vec3(2.0f));
        INFO("scale: ", scale);
        t.setLocalScale(scale);

        auto skew = glm::linearRand(glm::vec3(-2.f), glm::vec3(2.f));
        INFO("skew: ", skew);
        t.setLocalSkew(skew);

        glm::mat4 mat = t.localToWorldMatrix();
        glm::vec3 newPos{};
        glm::mat3 newRot{};
        glm::vec3 newScl{};
        glm::vec3 newSkw{};
        decompose(mat, newPos, newRot, newScl, newSkw);
        {
            INFO("Scale expect: ", newScl);
            checkVecEquality(scale, newScl);
        }
        {
            INFO("Rotation expect: ", newRot);
            checkMatEquality(rotMat, newRot);
        }
        {
            INFO("Translation expect: ", newPos);
            checkVecEquality(pos, newPos);
        }
        {
            INFO("Skew expect: ", newSkw);
            checkVecEquality(skew, newSkw);
        }
        {
            glm::mat4 inv = t.parentToLocalMatrix();
            INFO("Inverse expect: ", inv);
            checkMatEquality(inv, glm::inverse(mat));
        }
    }
}

TEST_CASE("MatrixDecomposition") {
    Transform transform;
    transform.setLocalPosition({-3.4f, 2.8, 10});
    transform.setLocalRotation(glm::angleAxis(5.0f, glm::vec3{0, 1, 0}));
    transform.setLocalScale({2, 1, 1});
    transform.setLocalSkew({5, -3, 1});

    VERIFY_TRANSFORM(transform);
}

TEST_CASE("ParentReassignment") {
    Transform parent1, parent2;
    //    parent1.setLocalRotation(glm::angleAxis(glm::radians(90.0f), glm::vec3{0, 1, 0}));
    //    parent2.setLocalRotation(glm::angleAxis(glm::radians(90.0f), glm::vec3{1, 0, 0}));
    parent1.setLocalPosition({-0.222730f, -0.029753f, -0.107713f})
        .setLocalRotation({0.691589f, -0.722292f, 0.000000f, -0.000000f})
        .setLocalScale({1.000000f, 1.033776f, 0.967327f})
        .setLocalSkew({0.092879f, -1.251760f, -0.393206f});

    auto      mat = parent1.localToWorldMatrix();
    glm::vec3 scale{};
    glm::quat rotation{};
    glm::vec3 position{};
    glm::vec3 skew{};
    glm::vec4 perspective{};
    glm::decompose(mat, scale, rotation, position, skew, perspective);

    parent2.setLocalPosition({-1.000000f, 1.000000f, 0.000000f})
        .setLocalScale({1.000000f, 0.500000f, 1.000000f})
        .setLocalSkew({0.000000f, 0.000000f, 0.396004f});
    Transform child(parent1);
    //    child.setLocalPosition({-0.222730f, -0.029753f, -0.107713f})
    //        .setLocalRotation({0.691589f, -0.722292f, 0.000000f, -0.000000f})
    //        .setLocalScale({1.000000f, 1.033776f, 0.967327f})
    //        .setLocalSkew({0.092879f, -1.251760f, -0.393206f});
    auto oldMat = child.localToWorldMatrix();
    child.setParent(&parent2);
    auto newMat = child.localToWorldMatrix();
    checkMatEquality(oldMat, newMat);
}
