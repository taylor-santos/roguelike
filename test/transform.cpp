//
// Created by taylor-santos on 12/15/2021 at 17:29.
//

#include "doctest/doctest.h"

#include "transform.h"
#include "glm/gtc/random.hpp"

TEST_SUITE_BEGIN("Transform");
DOCTEST_CLANG_SUPPRESS_WARNING_PUSH
DOCTEST_CLANG_SUPPRESS_WARNING("-Wunused-parameter")
DOCTEST_CLANG_SUPPRESS_WARNING("-Wunused-variable")

#define CHECK_EPS_EQ(a, b)                          \
    do {                                            \
        CHECK(glm::epsilonEqual((a), (b), 0.0001)); \
    } while (0)

#define CHECK_VEC3_EQ(a, b)           \
    do {                              \
        CHECK_EPS_EQ((a)[0], (b)[0]); \
        CHECK_EPS_EQ((a)[1], (b)[1]); \
        CHECK_EPS_EQ((a)[2], (b)[2]); \
    } while (0)

#define CHECK_VEC4_EQ(a, b)           \
    do {                              \
        CHECK_EPS_EQ((a)[0], (b)[0]); \
        CHECK_EPS_EQ((a)[1], (b)[1]); \
        CHECK_EPS_EQ((a)[2], (b)[2]); \
        CHECK_EPS_EQ((a)[3], (b)[3]); \
    } while (0)

#define CHECK_MAT3_EQ(a, b)            \
    do {                               \
        CHECK_VEC3_EQ((a)[0], (b)[0]); \
        CHECK_VEC3_EQ((a)[1], (b)[1]); \
        CHECK_VEC3_EQ((a)[2], (b)[2]); \
    } while (0)

#define CHECK_MAT4_EQ(a, b)            \
    do {                               \
        CHECK_VEC4_EQ((a)[0], (b)[0]); \
        CHECK_VEC4_EQ((a)[1], (b)[1]); \
        CHECK_VEC4_EQ((a)[2], (b)[2]); \
        CHECK_VEC4_EQ((a)[3], (b)[3]); \
    } while (0)

DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4505) // unreferenced local function has been removed
DOCTEST_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wunused-function")
DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wunused-function")
static Transform
randomTransform(Transform *parent = nullptr) {
    auto pos = glm::linearRand(glm::dvec3{-10, -10, -10}, glm::dvec3{10, 10, 10});
    auto rot = glm::angleAxis(glm::linearRand(0.0, 2 * glm::pi<double>()), glm::sphericalRand(1.0));
    auto scale = glm::linearRand(glm::dvec3(0.01), glm::dvec3(10.0));
    auto skew  = glm::linearRand(glm::dvec3(-0.5), glm::dvec3(0.5));
    auto builder =
        Transform::Builder().withPosition(pos).withRotation(rot).withScale(scale).withSkew(skew);
    if (parent) {
        builder = builder.withParent(*parent);
    }
    return builder.build();
}
DOCTEST_CLANG_SUPPRESS_WARNING_POP
DOCTEST_GCC_SUPPRESS_WARNING_POP
DOCTEST_MSVC_SUPPRESS_WARNING_POP

TEST_CASE("BuilderImplicitConversion") {
    auto      builder = Transform::Builder().withPosition({1, 2, 3});
    Transform t       = builder; // Implicit conversion
    CHECK_VEC3_EQ(t.localPosition(), glm::dvec3(1, 2, 3));
}

TEST_CASE("ParentReassignment") {
    for (int i = 0; i < 1000; i++) {
        auto parent1 = randomTransform();
        auto parent2 = randomTransform();
        auto child   = randomTransform(&parent1);
        {
            auto oldMat = child.localToWorldMatrix();
            child.setParent(&parent2);
            auto newMat = child.localToWorldMatrix();
            CHECK_MAT4_EQ(oldMat, newMat);
        }
        {
            auto child2     = randomTransform();
            auto grandChild = randomTransform();
            {
                auto oldMat = grandChild.localToWorldMatrix();
                grandChild.setParent(&child2);
                auto newMat = grandChild.localToWorldMatrix();
                CHECK_MAT4_EQ(oldMat, newMat);
            }
            {
                auto oldMat = grandChild.localToWorldMatrix();
                child.setParent(&parent2);
                auto newMat = grandChild.localToWorldMatrix();
                CHECK_MAT4_EQ(oldMat, newMat);
            }
        }
    }
}

TEST_CASE("ParentCycle") {
    Transform t1;
    SUBCASE("OneLevel") {
        CHECK_THROWS_AS(t1.setParent(&t1), std::invalid_argument);
    }
    Transform t2;
    t2.setParent(&t1);
    SUBCASE("TwoLevels") {
        CHECK_THROWS_AS(t1.setParent(&t2), std::invalid_argument);
    }
    Transform t3;
    t3.setParent(&t2);
    SUBCASE("ThreeLevels") {
        CHECK_THROWS_AS(t1.setParent(&t3), std::invalid_argument);
    }
}

DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wself-assign-overloaded")
TEST_CASE("Copy") {
    Transform parent;
    Transform original = randomTransform(&parent);
    Transform child    = randomTransform(&original);
    auto      pos      = original.localPosition();
    auto      rot      = original.localRotation();
    auto      scl      = original.localScale();
    auto      skw      = original.localSkew();
    SUBCASE("Constructor") {
        Transform copy = original;
        CHECK(copy == original);
        auto rotMat    = toMat3(rot);
        auto newRotMat = toMat3(copy.localRotation());
        CHECK(copy.parent() == &parent);
        CHECK(child.parent() == &original);
        CHECK_VEC3_EQ(pos, copy.localPosition());
        CHECK_MAT3_EQ(rotMat, newRotMat);
        CHECK_VEC3_EQ(scl, copy.localScale());
        CHECK_VEC3_EQ(skw, copy.localSkew());
    }
    SUBCASE("Assignment") {
        Transform copy;
        copy = original;
        CHECK(copy == original);
        auto rotMat    = toMat3(rot);
        auto newRotMat = toMat3(copy.localRotation());
        CHECK(copy.parent() == &parent);
        CHECK(child.parent() == &original);
        CHECK_VEC3_EQ(pos, copy.localPosition());
        CHECK_MAT3_EQ(rotMat, newRotMat);
        CHECK_VEC3_EQ(scl, copy.localScale());
        CHECK_VEC3_EQ(skw, copy.localSkew());
    }
    SUBCASE("SelfAssignment") {
        original = original;
        CHECK(original == original);
        auto rotMat    = toMat3(rot);
        auto newRotMat = toMat3(original.localRotation());
        CHECK(original.parent() == &parent);
        CHECK(child.parent() == &original);
        CHECK_VEC3_EQ(pos, original.localPosition());
        CHECK_MAT3_EQ(rotMat, newRotMat);
        CHECK_VEC3_EQ(scl, original.localScale());
        CHECK_VEC3_EQ(skw, original.localSkew());
    }
}
DOCTEST_CLANG_SUPPRESS_WARNING_POP

DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wself-move")
TEST_CASE("Move") {
    Transform parent;
    Transform original = randomTransform(&parent);
    Transform child    = randomTransform(&original);
    auto      pos      = original.localPosition();
    auto      rot      = original.localRotation();
    auto      scl      = original.localScale();
    auto      skw      = original.localSkew();
    SUBCASE("Constructor") {
        Transform copy      = std::move(original);
        auto      rotMat    = toMat3(rot);
        auto      newRotMat = toMat3(copy.localRotation());
        CHECK(copy.parent() == &parent);
        CHECK(child.parent() == &copy);
        CHECK_VEC3_EQ(pos, copy.localPosition());
        CHECK_MAT3_EQ(rotMat, newRotMat);
        CHECK_VEC3_EQ(scl, copy.localScale());
        CHECK_VEC3_EQ(skw, copy.localSkew());
    }
    SUBCASE("Assignment") {
        Transform copy;
        copy           = std::move(original);
        auto rotMat    = toMat3(rot);
        auto newRotMat = toMat3(copy.localRotation());
        CHECK(copy.parent() == &parent);
        CHECK(child.parent() == &copy);
        CHECK_VEC3_EQ(pos, copy.localPosition());
        CHECK_MAT3_EQ(rotMat, newRotMat);
        CHECK_VEC3_EQ(scl, copy.localScale());
        CHECK_VEC3_EQ(skw, copy.localSkew());
    }
    SUBCASE("SelfAssignment") {
        original       = std::move(original);
        auto rotMat    = toMat3(rot);
        auto newRotMat = toMat3(original.localRotation());
        CHECK(original.parent() == &parent);
        CHECK(child.parent() == &original);
        CHECK_VEC3_EQ(pos, original.localPosition());
        CHECK_MAT3_EQ(rotMat, newRotMat);
        CHECK_VEC3_EQ(scl, original.localScale());
        CHECK_VEC3_EQ(skw, original.localSkew());
    }
}
DOCTEST_CLANG_SUPPRESS_WARNING_POP

TEST_CASE("SetLocalProps") {
    auto parent = randomTransform();
    auto child  = randomTransform(&parent);

    auto oldPos   = child.localPosition();
    auto oldRot   = glm::toMat3(child.localRotation());
    auto oldScale = child.localScale();
    auto oldSkew  = child.localSkew();

    SUBCASE("Position") {
        auto pos = glm::linearRand(glm::dvec3{-10, -10, -10}, glm::dvec3{10, 10, 10});
        child.setLocalPosition(pos);
        auto newRot = glm::toMat3(child.localRotation());
        CHECK_VEC3_EQ(child.localPosition(), pos);
        CHECK_MAT3_EQ(newRot, oldRot);
        CHECK_VEC3_EQ(child.localScale(), oldScale);
        CHECK_VEC3_EQ(child.localSkew(), oldSkew);
    }
    SUBCASE("Rotation") {
        auto rot =
            glm::angleAxis(glm::linearRand(0.0, 2 * glm::pi<double>()), glm::sphericalRand(1.0));
        child.setLocalRotation(rot);
        oldRot      = glm::toMat3(rot);
        auto newRot = glm::toMat3(child.localRotation());
        CHECK_VEC3_EQ(child.localPosition(), oldPos);
        CHECK_MAT3_EQ(newRot, oldRot);
        CHECK_VEC3_EQ(child.localScale(), oldScale);
        CHECK_VEC3_EQ(child.localSkew(), oldSkew);
    }
    SUBCASE("Scale") {
        auto scale = glm::linearRand(glm::dvec3(0.01), glm::dvec3(10.0));
        child.setLocalScale(scale);
        auto newRot = glm::toMat3(child.localRotation());
        CHECK_VEC3_EQ(child.localPosition(), oldPos);
        CHECK_MAT3_EQ(newRot, oldRot);
        CHECK_VEC3_EQ(child.localScale(), scale);
        CHECK_VEC3_EQ(child.localSkew(), oldSkew);
    }
    SUBCASE("Skew") {
        auto skew = glm::linearRand(glm::dvec3(-0.5), glm::dvec3(0.5));
        child.setLocalSkew(skew);
        auto newRot = glm::toMat3(child.localRotation());
        CHECK_VEC3_EQ(child.localPosition(), oldPos);
        CHECK_MAT3_EQ(newRot, oldRot);
        CHECK_VEC3_EQ(child.localScale(), oldScale);
        CHECK_VEC3_EQ(child.localSkew(), skew);
    }
}

TEST_CASE("SetWorldProps") {
    auto parent = randomTransform();
    auto child  = randomTransform(&parent);

    auto worldProps = Transform::decompose(child.localToWorldMatrix());
    auto oldRot     = glm::toMat3(worldProps.rotation);
    SUBCASE("Position") {
        auto pos = glm::linearRand(glm::dvec3{-10, -10, -10}, glm::dvec3{10, 10, 10});
        child.setPosition(pos);
        auto newRot = glm::toMat3(child.rotation());
        CHECK_VEC3_EQ(child.position(), pos);
        CHECK_MAT3_EQ(oldRot, newRot);
        CHECK_VEC3_EQ(child.scale(), worldProps.scale);
        CHECK_VEC3_EQ(child.skew(), worldProps.skew);
    }
    SUBCASE("Rotation") {
        auto rot =
            glm::angleAxis(glm::linearRand(0.0, 2 * glm::pi<double>()), glm::sphericalRand(1.0));
        child.setRotation(rot);
        auto newRot = glm::toMat3(child.rotation());
        CHECK_VEC3_EQ(child.position(), worldProps.translation);
        CHECK_MAT3_EQ(glm::toMat3(rot), newRot);
        CHECK_VEC3_EQ(child.scale(), worldProps.scale);
        CHECK_VEC3_EQ(child.skew(), worldProps.skew);
    }
    SUBCASE("Scale") {
        auto scale = glm::linearRand(glm::dvec3(0.01), glm::dvec3(10.0));
        child.setScale(scale);
        auto newRot = glm::toMat3(child.rotation());
        CHECK_VEC3_EQ(child.position(), worldProps.translation);
        CHECK_MAT3_EQ(oldRot, newRot);
        CHECK_VEC3_EQ(child.scale(), scale);
        CHECK_VEC3_EQ(child.skew(), worldProps.skew);
    }
    SUBCASE("Skew") {
        auto skew = glm::linearRand(glm::dvec3(-0.5), glm::dvec3(0.5));
        child.setSkew(skew);
        auto newRot = glm::toMat3(child.rotation());
        CHECK_VEC3_EQ(child.position(), worldProps.translation);
        CHECK_MAT3_EQ(oldRot, newRot);
        CHECK_VEC3_EQ(child.scale(), worldProps.scale);
        CHECK_VEC3_EQ(child.skew(), skew);
    }
}

TEST_CASE("Directions") {
    auto parent = randomTransform();
    auto child  = randomTransform(&parent);

    auto right   = child.right();
    auto up      = child.up();
    auto forward = child.forward();

    SUBCASE("Orthogonality") {
        CHECK(glm::dot(right, up) == doctest::Approx(0));
        CHECK(glm::dot(right, forward) == doctest::Approx(0));
        CHECK(glm::dot(up, forward) == doctest::Approx(0));
    }

    SUBCASE("Transformations") {
        auto rot = child.rotation();
        CHECK_VEC3_EQ(glm::conjugate(rot) * forward, glm::dvec3(0, 0, -1));
    }
}

TEST_CASE("DestructorRemovesChildren") {
    auto child = randomTransform();
    auto mat   = child.localToWorldMatrix();
    {
        auto parent = randomTransform();
        child.setParent(&parent);
        CHECK(child.parent() == &parent);
        CHECK_MAT4_EQ(child.localToWorldMatrix(), mat);
    }
    CHECK(child.parent() == nullptr);
    CHECK_MAT4_EQ(child.localToWorldMatrix(), mat);
}

DOCTEST_CLANG_SUPPRESS_WARNING_POP
