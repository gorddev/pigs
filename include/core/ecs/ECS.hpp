#pragma once
#include "ecs-core/ECSCore.hpp"
#include "entities/EntityBuilder.hpp"

// made by gordie novak on feb 15th, 2026

namespace gan {

    class ECS {
        ECSCore core;

    public:
        ECS() = default;

        EntityBuilder spawn() {
            return {core, core.spawn()};
        }

        template<typename... C>
        EntityFilter<C...> filter() {
            return core.filter<C...>();
        }
    };
}