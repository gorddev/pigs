// ReSharper disable CppDoxygenUnresolvedReference
#pragma once

// created by gordie novak feb 11th, 2026

// this file holds the ComponentBank class which
// * really just keeps track of all the components

#include <vector>
#include "ComponentData.hpp"
#include "ComponentVector.hpp"

#ifdef GAN_DEBUG
#include <cassert>
#endif

namespace gan {

    /// The component bank contains all components
    class ComponentBank {
    public:
        /// All the components stored in the component bank.
        ComponentVector components;
        /// Component Bank is only default constructed.
        ComponentBank() = default;

        friend class ECSCore;///< Engine needs to be able to construct a component bank

        /// Adds a component to the component bank
        template<typename C>
        void createComponent() {
            // grab our component_id
            auto id = ECSTypeID::getComponentID<C>();
            #ifdef GAN_DEBUG
            assert(id >= components.size());
            #endif
            // make sure to resize our vector to the proper size
            components.make<C>();
        }
    public:
        /// Default constructs a component in place
        /// @tparam C The type you want to add the component to
        /// @param e The entity you want to
        template<typename C>
        void addComponentToEntity(Entity e) {
            // adds the current component
            components.get<C>().template addEntity<C>(e);
        }

        /// Default constructs a component in place
        /// @tparam C The type you want to add the component to
        /// @param e The entity you want to
        /// @param args arguments to construct the object in-place
        template<typename C, typename... Args>
        void setComponentToEntity(Entity e, Args&&... args) {
            // sets the current component
            components.get<C>().template setEntity<C>(e, std::forward<Args>(args)...);
        }

        /// Returns true if the entity has the requested component.
        template<typename C>
        [[nodiscard]] bool hasComponent(Entity e) const noexcept{
            return components.has<C>(e);
        }

        /// Removes a component from a componentData object
        /// @tparam C The type of the component you want to remove
        /// @param e The entity ou want to remove the component from.
        template<typename C>
        void removeComponent(Entity e) {
            // get the component id
            #ifdef GAN_DEBUG
            auto id = ECSTypeID::getComponentID<C>();
            assert(id < components.size());
            #endif
            components.get<C>().removeEntity(e);
        }

        /// Removes a component from a componentData object
        /// @tparam C The type of the component you want to remove
        /// @param e The entity ou want to remove the component from.
        template<typename C>
        void removeComponent(entity_id e) {
            // get the component id
            #ifdef GAN_DEBUG
            auto id = ECSTypeID::getComponentID<C>();
            assert(id < components.size());
            #endif
            components.get<C>().removeEntity(e);
        }

        /// Gets the specific component from the entity.
        /// @tparam C The type of component you want to retrieve
        /// @param e The entity you want to retrieve the component from
        /// @return A @code std::optional<C&>@endcode which either may, or may not
        /// contain the element.
        /// @note This function already does a @code hasComponent@endcode check built in,
        /// so you don't need to call @code hasComponent@endcode before calling this.
        /// Instead, check whether the component has the entity with @code .has_value()@endcode
        template<typename C>
        std::optional<C*> getComponent(Entity e) {
            // get our component id
            auto id = ECSTypeID::getComponentID<C>();
            if (components[id].hasComponent(e)) {
                return &components[id].template getComponent<C>(e);
            } return std::nullopt;
        }

        /// @tparam C The component you want to extract the data from
        /// @param c$tag pass to clarify the type you want to extract.
        /// @return An @code std::optional<ComponentData*>@endcode representing the data.
        template<typename C>
        std::optional<ComponentData*> getComponentData() {
            auto id = ECSTypeID::getComponentID<C>();
            if (id < components.size())
                return &components[id];
            return std::nullopt;
        }

        /// Creates a tuple of component views to form an EntityFilter.
        /// @tparam C The components you want views into.
        /// @return A tuple of component views.
        template<typename... C>
        std::tuple<ComponentView<C>...> createComponentViews() {
            return [&]<typename... Ts>() {
                return std::make_tuple(
                    components.get<C>().template view<Ts>()...
                );
            }.template operator()<C...>();
        }

        /// Resizes all components in the ComponentBank
        void resizeEntityMaps(const size_t newSize) {
            components.bulkResize(newSize);
        }
    };
}
