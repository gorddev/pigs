#pragma once

#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "glm/detail/type_mat4x4.hpp"
#include <toolkit/types/dim2.hpp>

// made by gordie novak on feb 22nd, 2026

// used to perform camera transformations and all sorts of other things.

namespace pg {


    struct Camera {
        // <><><> Vector Properties <><><>
        glm::vec3 pos{0, 2.0, 10.f};    ///< Position of the camera.
        glm::vec3 forward{0, 0, -1.f};  ///< Current forward direction of the camera (where it is looking at).
        glm::vec3 up{0, 1.f, 0};        ///< The direction that the camera considers to be "up."

        // <><><> Primitive Properties <><><>
        float fov = 1.f;    ///< The "field of view" of the camera in radians.
        float near = 0.1f;  ///< @code near@endcode: How close to the camera objects get clipped (cut off)
        float far = 400.f;  ///< @code far@endcode: How far away the renderer will render something.

        /// @return The view matrix of the camera.
        [[nodiscard]] glm::mat4 view() const {
            return glm::lookAt(pos, pos + forward, up);
        }

        /// @param sceneDimensions The current dimensions of the window you are looking through.
        /// @return The projection matrix for the camera.
        [[nodiscard]] glm::mat4 projection(dim2 sceneDimensions) const {
            const float aspectRatio =
                sceneDimensions.w /
                static_cast<float>(sceneDimensions.h);
            return glm::perspective(fov, aspectRatio, near, far);

        }

        void moveHorizontal(float amount) {
            pos -= glm::normalize(glm::cross(up, forward)) * amount;
        }

        void moveVertical(float amount) {
            pos += glm::normalize(up) * amount;
        }

        void moveForward(float amount) {
            pos += glm::normalize(forward) * amount;
        }

        [[nodiscard]] glm::vec3 right() const {
            return glm::normalize(glm::cross(up, forward));
        }

        void toggleOrthographic() {
            static float o_fov = 0.1f;
            static glm::vec3 o_pos = {100.f, 100.f, 100.f};
            static glm::vec3 o_forward = {-1.f, -1.f, -1.f};

            std::swap(o_fov, fov);
            std::swap(o_pos, pos);
            std::swap(o_forward, forward);
        }
    };

}