#pragma once

#include "../ecs-core/ecsdef.hpp"

namespace gan {

    class Entity final {
        friend class ECSCore; ///< Only the ECSCore can form Entities

        /// Requires that all entities come from the ECSCore.
        explicit Entity(const entity_id id)
            : id(id) {}

    public:
        /// ID of the Entity. Default is -1, but it should be reassigned.
        const entity_id id = -1;
        /// Returns the id of the entity
        [[nodiscard]] entity_id value() const { return id; }

    };
}
