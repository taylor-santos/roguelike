//
// Created by taylor-santos on 5/10/2021 at 18:38.
//

#include "camera.h"
#include "doctest/doctest.h"

TEST_SUITE_BEGIN("Camera");
DOCTEST_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wunused-variable")

#define INFO_VEC(msg, v) INFO((msg), "(", (v).x, ", ", (v).y, ", ", (v).z, ")")

#define TEST_VEC_EQ(a, b)                       \
    do {                                        \
        INFO_VEC("a = ", a);                    \
        INFO_VEC("b = ", b);                    \
        CHECK((a).x == doctest::Approx((b).x)); \
        CHECK((a).y == doctest::Approx((b).y)); \
        CHECK((a).z == doctest::Approx((b).z)); \
    } while (0)

TEST_CASE("CameraProjection") {
    Camera camera;
    camera.transform.setLocalPosition({1, -2, 3});
    float near = 1.0f;
    float far  = 10.0f;
    camera.setNear(near);
    camera.setFar(far);
    camera.setFOV(90);
    for (int pitch = -90; pitch <= 90; pitch += 10) {
        INFO("Pitch   = ", pitch, " degrees");
        for (int yaw = 0; yaw < 360; yaw += 10) {
            INFO("Yaw     = ", yaw, " degrees");
            camera.setRotation(static_cast<float>(yaw), static_cast<float>(pitch));
            auto right = camera.right();
            INFO("Right   = ", right);
            auto up = camera.up();
            INFO("Up      = ", up);
            auto forward = camera.forward();
            INFO("Forward = ", forward);

            CHECK(glm::dot(right, up) == doctest::Approx(0));
            CHECK(glm::dot(right, forward) == doctest::Approx(0));
            CHECK(glm::dot(up, forward) == doctest::Approx(0));

            TEST_VEC_EQ(glm::cross(right, up), -forward);
            TEST_VEC_EQ(glm::cross(up, -forward), right);
            TEST_VEC_EQ(glm::cross(-forward, right), up);

            auto      mat = camera.getMatrix(1.0, 1.0);
            glm::vec3 pos = camera.transform.position();
            for (int x = -1; x <= 1; x++) {
                auto xpos = static_cast<float>(x) * right;
                for (int y = -1; y <= 1; y++) {
                    auto ypos = static_cast<float>(y) * up;
                    for (int z = -1; z <= 1; z += 2) {
                        auto dist = z == -1 ? near : far;
                        auto vec  = glm::vec4{pos + dist * (forward + xpos + ypos), 1};
                        vec       = mat * vec;
                        vec /= vec.w;
                        glm::vec3 expected{x, y, z};
                        TEST_VEC_EQ(vec, expected);
                    }
                }
            }
        }
    }
}

TEST_CASE("CameraSensitivity") {
    Camera cam;
    SUBCASE("TwoArgs") {
        cam.setSensitivity(1.5f, 2.0f);
        auto [x, y] = cam.getSensitivity();
        CHECK(x == doctest::Approx(1.5));
        CHECK(y == doctest::Approx(2));
    }
    SUBCASE("PairArg") {
        cam.setSensitivity({3.0f, 4.5f});
        auto [x, y] = cam.getSensitivity();
        CHECK(x == doctest::Approx(3));
        CHECK(y == doctest::Approx(4.5));
    }
}

TEST_CASE("CameraFOV") {
    Camera cam;
    cam.setFOV(100);
    CHECK(cam.getFOV() == doctest::Approx(100));
    cam.setFOV(15);
    CHECK(cam.getFOV() == doctest::Approx(15));
}

TEST_CASE("CameraNear") {
    Camera cam;
    cam.setNear(0.1f);
    CHECK(cam.getNear() == doctest::Approx(0.1f));
    cam.setNear(1.5f);
    CHECK(cam.getNear() == doctest::Approx(1.5f));
}

TEST_CASE("CameraFar") {
    Camera cam;
    cam.setFar(100.1f);
    CHECK(cam.getFar() == doctest::Approx(100.1f));
    cam.setFar(1000.0f);
    CHECK(cam.getFar() == doctest::Approx(1000.0f));
}

TEST_CASE("CameraRotation") {
    Camera cam;
    SUBCASE("") {
        cam.setRotation(12.34f, 65);
        auto [yaw, pitch] = cam.getRotation();
        CHECK(yaw == doctest::Approx(12.34));
        CHECK(pitch == doctest::Approx(65));
    }
    SUBCASE("") {
        cam.setRotation(-90, 0);
        auto [yaw, pitch] = cam.getRotation();
        CHECK(yaw == doctest::Approx(270));
        CHECK(pitch == doctest::Approx(0));
    }
    SUBCASE("") {
        cam.setRotation(0, 100);
        auto [yaw, pitch] = cam.getRotation();
        CHECK(yaw == doctest::Approx(0));
        CHECK(pitch == doctest::Approx(90));
    }
    SUBCASE("") {
        cam.setRotation(0, -200);
        auto [yaw, pitch] = cam.getRotation();
        CHECK(yaw == doctest::Approx(0));
        CHECK(pitch == doctest::Approx(-90));
    }
}

TEST_CASE("CameraAddRotation") {
    Camera cam;
    cam.setSensitivity(1, 1);
    cam.setRotation(0, 0);
    cam.addRotation(12.34f, 0);
    SUBCASE("RotateYaw") {
        auto [yaw, pitch] = cam.getRotation();
        CHECK(yaw == doctest::Approx(12.34));
        CHECK(pitch == doctest::Approx(0));
    }
    cam.addRotation(0, 30);
    SUBCASE("RotatePitch") {
        auto [yaw, pitch] = cam.getRotation();
        CHECK(yaw == doctest::Approx(12.34));
        CHECK(pitch == doctest::Approx(30));
    }
    cam.addRotation(360, 0);
    SUBCASE("360DegreeRotationWraps") {
        auto [yaw, pitch] = cam.getRotation();
        CHECK(yaw == doctest::Approx(12.34));
        CHECK(pitch == doctest::Approx(30));
    }
    cam.addRotation(-50, 0);
    SUBCASE("NegativeYawWraps") {
        auto [yaw, pitch] = cam.getRotation();
        CHECK(yaw == doctest::Approx(322.34));
        CHECK(pitch == doctest::Approx(30));
    }
    cam.addRotation(0, 100);
    SUBCASE("90DegreePitchMax") {
        auto [yaw, pitch] = cam.getRotation();
        CHECK(yaw == doctest::Approx(322.34));
        CHECK(pitch == doctest::Approx(90));
    }
    cam.addRotation(0, -200);
    SUBCASE("-90DegreePitchMin") {
        auto [yaw, pitch] = cam.getRotation();
        CHECK(yaw == doctest::Approx(322.34));
        CHECK(pitch == doctest::Approx(-90));
    }
}
