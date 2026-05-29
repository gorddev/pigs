#pragma once
#include <vector>
#include "../entities/EntityFilter.hpp"
#include "../components/ComponentBank.hpp"
#include "../entities/EntityLinker.hpp"

// made by gordie novak feb 12th, 2026

// the point of the ECSCore is to provide a source
// for all entity creation. the ECS manages the ECS core
// because some objects like EntityBuilder rely on subobjects
// in the ECS core, so the ECSCore can't include EntityBuilder
// . It's then the ECS's responsibility to create objects like that.


namespace gan {

    /// used for platform independence: ensures our size is the right starting capacity.
    constexpr size_t initVecCapacity = std::vector<int>().capacity();
    //^ We define it here to "ecsdef.hpp" doesn't have to #include <vector>

    /// sole creator of Entity objects.
    /// preserves the invariant that ecsCapacity <= ecsSize. updates
    /// the linker and the bank if more entities are requested.
    class ECSCore {
        /// Internal Vector of Entities
        std::vector<Entity> entities;
        /// Links each entity to a set of components.
        EntityLinker linker;
        /// Contains all @code ComponentData@endcode objects.
        ComponentBank bank;
        /// The current size of the linker
        size_t ecsSize = 0, ecsCapacity = (initVecCapacity);

        ECSCore() = default;
        friend class ECS;
        friend class EntityBuilder;

        /// Resize all subcomponents in the ECS
        void resizeECS() {
            if (ecsCapacity == 0)
                ecsCapacity = 1;
            else
                ecsCapacity *= 2;
            linker.resize(ecsCapacity);
            bank.resizeEntityMaps(ecsCapacity);
        }
    public:

        Entity spawn() noexcept {
            if (ecsSize++ == ecsCapacity)
                resizeECS();
            entities.push_back(Entity(entities.size()));
            return entities.back();
        }

        template<typename T, typename... Args>
        void set(const Entity e, Args&&... args) {
            bank.setComponentToEntity<T>(e, std::forward<Args>(args)...);
            linker.assignComponent(e.id, ECSTypeID::getComponentID<T>());
        }

        template<typename T>
        void add(const Entity e) {
            bank.addComponentToEntity<T>(e);
            linker.assignComponent(e.id, ECSTypeID::getComponentID<T>());
        }

        template<typename T>
        void remove (const Entity e) {
            bank.removeComponent<T>(e);
            linker.unassignComponent(e.id, ECSTypeID::getComponentID<T>());
        }

        template<typename C>
        [[nodiscard]] bool has(const Entity e) const noexcept {
            if (e.id < ecsSize) return false;
            return linker.hasComponent(e.id, ECSTypeID::getComponentID<C>());
        }

        template<typename... C>
            requires(are_unique_types<C...>)
        EntityFilter<C...> filter() {
            return EntityFilter<C...>(linker, bank);
        }
    };
}