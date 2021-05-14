//
// Created by taylor-santos on 5/10/2021 at 18:38.
//

#include "camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "doctest/doctest.h"

TEST_SUITE_BEGIN("Camera");

#define INFO_VEC(msg, v) INFO((msg), "(", (v).x, ", ", (v).y, ", ", (v).z, ")")

#define TEST_VEC_EQ(a, b)                       \
    do {                                        \
        INFO_VEC("a = ", a);                    \
        INFO_VEC("b = ", b);                    \
        CHECK((a).x == doctest::Approx((b).x)); \
        CHECK((a).y == doctest::Approx((b).y)); \
        CHECK((a).z == doctest::Approx((b).z)); \
    } while (0)

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

void
Camera::setRotation(float yaw, float pitch) {
    yaw_   = glm::radians(yaw);
    pitch_ = glm::radians(pitch);
}

[[nodiscard]] std::pair<float, float>
Camera::getRotation() const {
    return {glm::degrees(yaw_), glm::degrees(pitch_)};
}

glm::vec3
Camera::forward() const {
    float cosP = cos(pitch_);
    float x    = -sin(yaw_) * cosP;
    float y    = sin(pitch_);
    float z    = cos(yaw_) * cosP;
    auto  f    = glm::vec3(x, y, z);
    return f;
}

glm::vec3
Camera::left() const {
    return glm::vec3{glm::cos(yaw_), 0, glm::sin(yaw_)};
}

glm::vec3
Camera::up() const {
    return {
        glm::sin(pitch_) * glm::sin(yaw_),
        glm::cos(pitch_),
        -glm::cos(yaw_) * glm::sin(pitch_)};
}

TEST_CASE("CameraRotation") {
    Camera camera;
    camera.setSensitivity(1, 1);

    SUBCASE("0DegreesX") {
        auto forward = camera.forward();

        glm::vec3 target{0, 0, -1};
        TEST_VEC_EQ(forward, target);

        SUBCASE("90DegreesY") {
            camera.addRotation(0, 90);
            forward = camera.forward();

            target = {0, 1, 0};
            TEST_VEC_EQ(forward, target);

            SUBCASE("180DegreesY") {
                camera.addRotation(0, 90);
                forward = camera.forward();

                target = {0, 1, 0};
                TEST_VEC_EQ(forward, target);
            }
        }

        SUBCASE("-90DegreesY") {
            camera.addRotation(0, -90);
            forward = camera.forward();

            target = {0, -1, 0};
            TEST_VEC_EQ(forward, target);

            SUBCASE("-180DegreesY") {
                camera.addRotation(0, -90);
                forward = camera.forward();

                target = {0, -1, 0};
                TEST_VEC_EQ(forward, target);
            }
        }
    }

    SUBCASE("90DegreesX") {
        camera.addRotation(90, 0);
        auto forward = camera.forward();

        glm::vec3 target{1, 0, 0};
        TEST_VEC_EQ(forward, target);

        SUBCASE("90DegreesY") {
            camera.addRotation(0, 90);
            forward = camera.forward();

            target = {0, 1, 0};
            TEST_VEC_EQ(forward, target);

            SUBCASE("180DegreesY") {
                camera.addRotation(0, 90);
                forward = camera.forward();

                target = {0, 1, 0};
                TEST_VEC_EQ(forward, target);
            }
        }

        SUBCASE("-90DegreesY") {
            camera.addRotation(0, -90);
            forward = camera.forward();

            target = {0, -1, 0};
            TEST_VEC_EQ(forward, target);

            SUBCASE("-180DegreesY") {
                camera.addRotation(0, -90);
                forward = camera.forward();

                target = {0, -1, 0};
                TEST_VEC_EQ(forward, target);
            }
        }
    }

    SUBCASE("180DegreesX") {
        camera.addRotation(180, 0);
        auto forward = camera.forward();

        glm::vec3 target{0, 0, 1};
        TEST_VEC_EQ(forward, target);

        SUBCASE("90DegreesY") {
            camera.addRotation(0, 90);
            forward = camera.forward();

            target = {0, 1, 0};
            TEST_VEC_EQ(forward, target);

            SUBCASE("180DegreesY") {
                camera.addRotation(0, 90);
                forward = camera.forward();

                target = {0, 1, 0};
                TEST_VEC_EQ(forward, target);
            }
        }

        SUBCASE("-90DegreesY") {
            camera.addRotation(0, -90);
            forward = camera.forward();

            target = {0, -1, 0};
            TEST_VEC_EQ(forward, target);

            SUBCASE("-180DegreesY") {
                camera.addRotation(0, -90);
                forward = camera.forward();

                target = {0, -1, 0};
                TEST_VEC_EQ(forward, target);
            }
        }
    }

    SUBCASE("270DegreesX") {
        camera.addRotation(270, 0);
        auto forward = camera.forward();

        glm::vec3 target{-1, 0, 0};
        TEST_VEC_EQ(forward, target);

        SUBCASE("90DegreesY") {
            camera.addRotation(0, 90);
            forward = camera.forward();

            target = {0, 1, 0};
            TEST_VEC_EQ(forward, target);

            SUBCASE("180DegreesY") {
                camera.addRotation(0, 90);
                forward = camera.forward();

                target = {0, 1, 0};
                TEST_VEC_EQ(forward, target);
            }
        }

        SUBCASE("-90DegreesY") {
            camera.addRotation(0, -90);
            forward = camera.forward();

            target = {0, -1, 0};
            TEST_VEC_EQ(forward, target);

            SUBCASE("-180DegreesY") {
                camera.addRotation(0, -90);
                forward = camera.forward();

                target = {0, -1, 0};
                TEST_VEC_EQ(forward, target);
            }
        }
    }

    SUBCASE("360DegreesX") {
        camera.addRotation(360, 0);
        auto forward = camera.forward();

        glm::vec3 target{0, 0, -1};
        TEST_VEC_EQ(forward, target);

        SUBCASE("90DegreesY") {
            camera.addRotation(0, 90);
            forward = camera.forward();

            target = {0, 1, 0};
            TEST_VEC_EQ(forward, target);

            SUBCASE("180DegreesY") {
                camera.addRotation(0, 90);
                forward = camera.forward();

                target = {0, 1, 0};
                TEST_VEC_EQ(forward, target);
            }
        }

        SUBCASE("-90DegreesY") {
            camera.addRotation(0, -90);
            forward = camera.forward();

            target = {0, -1, 0};
            TEST_VEC_EQ(forward, target);

            SUBCASE("-180DegreesY") {
                camera.addRotation(0, -90);
                forward = camera.forward();

                target = {0, -1, 0};
                TEST_VEC_EQ(forward, target);
            }
        }
    }
}

static glm::mat4
view(float, float, const Camera &cam) {
    // return glm::translate(glm::eulerAngleXY(pitch, yaw), -pos);
    auto pos = cam.getPosition();
    return glm::lookAt(pos, pos + cam.forward(), cam.up());
}

glm::mat4
Camera::viewMatrix() const {
    return view(pitch_, yaw_, *this);
}

glm::vec3
Camera::getPosition() const {
    return pos_;
}

void
Camera::setPosition(const glm::vec3 &pos) {
    pos_ = pos;
}

glm::mat4
Camera::getMatrix(float aspect) const {
    if (isnan(aspect)) {
        aspect = 1;
    }
    glm::mat4 model      = glm::mat4(1.0f);
    glm::mat4 view       = viewMatrix();
    glm::mat4 projection = glm::perspective(fov_, aspect, 0.01f, 100.0f);
    glm::mat4 mvp        = projection * view * model;
    return mvp;
}

#define TEST_PROJECTION(cam)                                                   \
    do {                                                                       \
        SUBCASE("Projection") {                                                \
            auto pos          = (cam).getPosition();                           \
            auto [yaw, pitch] = (cam).getRotation();                           \
            auto left         = (cam).left();                                  \
            auto up           = (cam).up();                                    \
            auto forward      = (cam).forward();                               \
            INFO_VEC("left =    ", left);                                      \
            INFO_VEC("up =      ", up);                                        \
            INFO_VEC("forward = ", forward);                                   \
            SUBCASE("BasisVectors") {                                          \
                CHECK(glm::dot(forward, left) == doctest::Approx(0));          \
                CHECK(glm::dot(forward, up) == doctest::Approx(0));            \
                CHECK(glm::dot(left, up) == doctest::Approx(0));               \
                                                                               \
                TEST_VEC_EQ(glm::cross(left, up), forward);                    \
                TEST_VEC_EQ(glm::cross(up, forward), left);                    \
                TEST_VEC_EQ(glm::cross(forward, left), up);                    \
            }                                                                  \
            auto matrix = view(glm::radians(pitch), glm::radians(yaw), (cam)); \
            SUBCASE("Position") {                                              \
                auto vec = matrix * glm::vec4{pos, 1};                         \
                vec /= vec.w;                                                  \
                glm::vec3 target{0, 0, 0};                                     \
                TEST_VEC_EQ(vec, target);                                      \
            }                                                                  \
            SUBCASE("Forward") {                                               \
                auto vec = matrix * glm::vec4{pos + forward, 1};               \
                vec /= vec.w;                                                  \
                glm::vec3 target{0, 0, -1};                                    \
                TEST_VEC_EQ(vec, target);                                      \
            }                                                                  \
            SUBCASE("Left") {                                                  \
                auto vec = matrix * glm::vec4{pos + left, 1};                  \
                vec /= vec.w;                                                  \
                glm::vec3 target{-1, 0, 0};                                    \
                TEST_VEC_EQ(vec, target);                                      \
            }                                                                  \
            SUBCASE("Up") {                                                    \
                auto vec = matrix * glm::vec4{pos + up, 1};                    \
                vec /= vec.w;                                                  \
                glm::vec3 target{0, 1, 0};                                     \
                TEST_VEC_EQ(vec, target);                                      \
            }                                                                  \
        }                                                                      \
    } while (0)

TEST_CASE("ProjectionMatrix") {
    Camera cam;
    cam.setSensitivity(1, 1);
    cam.addRotation(90, 0);

    cam.setPosition({5, 0, 0});

    SUBCASE("Default") {
        TEST_PROJECTION(cam);
        SUBCASE("90Y") {
            cam.addRotation(0, 90);
            TEST_PROJECTION(cam);
        }
        SUBCASE("-90Y") {
            cam.addRotation(0, -90);
            TEST_PROJECTION(cam);
        }
    }
    SUBCASE("90X") {
        cam.addRotation(90, 0);
        TEST_PROJECTION(cam);

        SUBCASE("90Y") {
            cam.addRotation(0, 90);
            TEST_PROJECTION(cam);
        }
        SUBCASE("-90Y") {
            cam.addRotation(0, -90);
            TEST_PROJECTION(cam);
        }
    }
}
