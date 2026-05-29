#pragma once

#include "../ecs-core/ECSCore.hpp"

// made by gordie novak on feb 15th, 2026

// the entity builder allows for method chaining and
// easily creation and modification of entities.

namespace gan {

    class EntityBuilder {
        /// The core of the ECS system
        ECSCore& core;
        /// The target entity of EntityBuilder action.
        const Entity e;

        /// Private constructor only accessible by the ECS
        EntityBuilder(ECSCore& ecsCore, const Entity e)
            : core(ecsCore), e(e) {}

        friend class ECS;

    public:
        EntityBuilder operator=(const EntityBuilder& other) = delete; ///< Delete assignment constructor


        /// Assigns a component to the entity & constructs it.
        /// @tparam C The component you want to set
        /// @tparam Args Arguments to the constructor for your Component
        template<typename C, typename... Args>
        EntityBuilder& set(Args&&... args) {
            core.set<C>(e, std::forward<Args>(args)...);
            return *this;
        }

        /// Adds a component to the entity
        /// @tparam C The component you want to add.
        /// @note Requires a default constructor for your component.
        template<typename C>
            requires(std::is_default_constructible_v<C>)
        EntityBuilder& add() {
            core.add<C>(e);
            return *this;
        }

        /// Removes a component from the entity
        /// @tparam C The component you want to remove.
        template<typename C>
        EntityBuilder& remove() {
            core.remove<C>();
            return *this;
        }

        /// Returns true if the Entity has the component
        template<typename C>
        [[nodiscard]] bool has() const noexcept{
            return core.has<C>(e);
        }


    };

}