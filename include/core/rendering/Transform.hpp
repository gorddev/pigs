#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace pg {


    struct Transform {
        /// The current position of the model: x, y, z
        glm::vec3 pos{0.f,0.f, 0.f};
        /// The rotation of the model: x, y, z, w
        glm::quat rotation{0.f, 0.f, 0.f, 0.f};
        /// The x, y, z scale of the model.
        glm::vec3 scale{1.f, 1.f, 1.f};

        /// Composes the 4x4 transformation matrix for this model.
        [[nodiscard]] glm::mat4 compose() const {
            const glm::mat4 scaleMat = glm::scale(glm::mat4(1.f), scale);
            const glm::mat4 rotMat   = glm::mat4_cast(rotation);
            const glm::mat4 transMat = glm::translate(glm::mat4(1.f), pos);
            return transMat * rotMat * scaleMat;
        }

        /// Sets the scale of the model
        /// @param sx How much you want to scale x-direction
        /// @param sy How much you want to scale y-direction
        /// @param sz How much you want to scale z-direction
        void setScale(float sx, float sy, float sz) {
            scale.x = sx;
            scale.y = sy;
            scale.z = sz;
        }

        /// Moves the model projection a certain distance
        /// @param dx The amount x you want to move
        /// @param dy The mount y you want to move
        /// @param dz The amount z you want to move
        void move(float dx, float dy, float dz) {
            pos.x += dx;
            pos.y += dy;
            pos.z += dz;
        }

        /// Sets the position of the model.
        /// @param x The x-position the model will be set to.
        /// @param y The y-position the model will be set to.
        /// @param z The z-position the model will be set to.
        void setPos(float x, float y, float z) {
            pos.x = x;
            pos.y = y;
            pos.z = z;
        }
    };
    static_assert(std::is_aggregate_v<Transform>);
}
