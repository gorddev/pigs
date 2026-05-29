#pragma once
#include <vector>
#include "ComponentData.hpp"

// created by gordie novak on feb 13th

// provides safe access through the vector to iterate through components

namespace gan {

    class ComponentVector {
        /// internal vector of components
        std::vector<ComponentData> components;
        /// default capacity for each component
        size_t defaultCapacity = 0;

        /// Adds a component to the component bank
        template<typename C>
        void createComponent() {
            // grab our component_id
            auto id = ECSTypeID::getComponentID<C>();
            #ifdef GAN_DEBUG
            assert(id >= components.size());
            #endif
            // make sure to resize our vector to the proper size
            components.push_back(ComponentData(c$tag<C>()));
            components.back().resizeEntityMap(defaultCapacity);
        }
    public:

        /// Gets the component with the specific id.
        template<typename C>
        ComponentData& get() {
            component_id id = ECSTypeID::getComponentID<C>();
            if (id < components.size())
                return components[id];
            createComponent<C>();
            return components[id];
        }

        template<typename C>
        [[nodiscard]] bool has(Entity e){
            return get<C>().hasComponent(e);
        }

        template<typename C>
        void make() {
            const component_id id = ECSTypeID::getComponentID<C>();
            if (id >= components.size())
                createComponent<C>();
        }

        [[nodiscard]] size_t size() const noexcept{
            return components.size();
        }

        std::optional<ComponentData*> operator[](size_t index) {
            if (index < components.size())
                return &components[index];
            return std::nullopt;
        }

        /// Updates the internal size of all componentData objects.
        void bulkResize(const size_t newSize) {
            defaultCapacity = newSize;
            for (auto& c: components) {
                c.resizeEntityMap(newSize);
            }
        }

    };
}