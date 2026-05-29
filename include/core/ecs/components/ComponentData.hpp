#pragma once

// created by gordie novak feb 11, 2026

// This file just concerns the ComponentData object, which
// keeps track of all the data for one individual component.

#define GAN_DEBUG

#include <unordered_map>

#include <ecs/ecs-core/ecsdef.hpp>
#include "ComponentView.hpp"
#include "../ecs-core/ECSTypeID.hpp"
#include "../entities/Entity.hpp"
#include <toolkit/types/stack.hpp>

//TODO Vector internal vector not deleted at runtime, must create destruction method.

#ifdef GAN_DEBUG
#include <cassert>
#endif

namespace gan {

    template<typename c_tag_t>
    struct c$tag{}; ///< Used to relay template information through the component constructor.

    /// @brief search up what an ECS component is i'm not going to explain it here.
    /// @details Essentially, this component is type-erased.
    /// It holds a void* which secretly points to a @code std::vector<C>@endcode of the
    /// type of the component, but the @code class Component@endcode does not actually
    /// know what it's pointing to until it gets help from a template parameter.
    /// - Thus, @code Components@endcode cannot be created without a @code ComponentBank@endcode, because
    /// the @code ComponentBank@endcode needs to manage the type identification without using RTTI (Run Time
    /// Type Inference), which increases executable size. Note that @code dynamic_cast<C>@endcode is quite
    /// an expensive operation.
    class ComponentData {
    public:
        /// Maps each entity id to an index in the type-erased vector;
        std::vector<EntityIndexer> entityMap;
        /// Pointer to the underlying vector of individual struct instances
        vec_ptr* vec;
        /// ID stack of discarded Entities to be added again
        gutils::stack<entity_id> idStack;

        #ifdef GAN_DEBUG
        /// Keeps track of the component id
        const component_id id;
        #endif

    public:
        /// A component represents a single struct added to any entity.
        /// Keeps track of the relationship between the entity and the struct.
        template<typename C>
        explicit ComponentData(c$tag<C>) :
            vec(new std::vector<C>())
            #ifdef GAN_DEBUG
            , id(ECSTypeID::getComponentID<C>())
            #endif
        {};

        /// @param e The entity you want to check if the map has the component
        /// @return True if the entity has this component.
        [[nodiscard]] bool hasComponent(const Entity e) const {
            return entityMap[e.id].idx != static_cast<uint64_t>(-1);
        }

        /// @param e The entity you want to check if the map has the component
        /// @return True if the entity has this component.
        [[nodiscard]] bool hasComponent(const entity_id e) const {
            return entityMap[e].idx != static_cast<uint64_t>(-1);
        }

        /// Add a component to a given entity with its default construction
        template<typename C>
        void addEntity(Entity e) {
            #ifdef GAN_DEBUG
            assert(ECSTypeID::getComponentID<C>() == id);
            #endif
            if (!hasComponent(e)) {
                // insert into our map
                // if our idStack is empty we can just emplace onto the back
                if (idStack.is_empty()) {
                    entityMap[e.id] = EntityIndexer(static_cast<std::vector<C>*>(vec)->size());
                    static_cast<std::vector<C>*>(vec)->emplace_back(C());
                }
                // otherwise, we construct in one of the places
                else {
                    auto eID = idStack.pop();
                    entityMap[e.id] = EntityIndexer(eID);
                    (*static_cast<std::vector<C>*>(vec))[eID] = C();
                }
            } else {
                static_cast<std::vector<C>*>(vec)->at(entityMap[e.id].idx) = (C());
            }
        }

        /// Constructs a component for the component
        template<typename C, typename... Args>
        void setEntity(const Entity e, Args&&... args) {
            #ifdef GAN_DEBUG
            assert(ECSTypeID::getComponentID<C>() == id);
            #endif
            if (!hasComponent(e)) {
                // insert into our map
                // if our idStack is empty we can just emplace onto the back
                if (idStack.is_empty()) {
                    entityMap[e.id] = EntityIndexer(static_cast<std::vector<C>*>(vec)->size());
                    static_cast<std::vector<C>*>(vec)->emplace_back(C(std::forward<Args>(args)...));
                }
                // otherwise, we construct in one of the places
                else {
                    auto eIDX = idStack.pop();
                    entityMap[e.id] = EntityIndexer(eIDX);
                    (*static_cast<std::vector<C>*>(vec))[eIDX] = C(std::forward<Args>(args)...);
                }
            } else {
                static_cast<std::vector<C>*>(vec)->at(entityMap[e.id].idx) = (C(std::forward<Args>(args)...));
            }
        }

        /// Gets a reference of the component for a specific entity
        template<typename C>
        C& getComponent(const Entity e) {
            #ifdef GAN_DEBUG
            assert(ECSTypeID::getComponentID<C>() == id);
            assert(hasComponent(e));
            #endif
            return static_cast<std::vector<C>*>(vec)->at(entityMap.at(e.id));
        }

        /// Gets the component relevant to a specific entity
        template<typename C>
        C& operator[](const Entity e) {
            #ifdef GAN_DEBUG
            assert(ECSTypeID::getComponentID<C>() == id);
            assert(hasComponent(e));
            #endif
            return static_cast<std::vector<C>*>(vec)->at(entityMap.at(e.id));
        }

        /// Operator variant of the @code getComponent@endcode function
        void removeEntity(const Entity e) {
            if (hasComponent(e)) {
                idStack.push(entityMap.at(e.id).idx);
                entityMap[e.id] = EntityIndexer(-1);
            }
        }

        /// Operator variant of the @code getComponent@endcode function
        void removeEntity(const entity_id e) {
            if (hasComponent(e)) {
                idStack.push(entityMap.at(e).idx);
                entityMap[e] = EntityIndexer(-1);
            }
        }

        /// Gets an iterable view into the component
        template<typename C>
        ComponentView<C> view() noexcept {
            return ComponentView<C>(entityMap,
                *static_cast<std::vector<C>*>(this->vec));
        }

        /// Grabs the raw vector data from the component information
        template<typename C>
        const std::vector<C>& rawData() {
            #ifdef GAN_DEBUG
            assert(ECSTypeID::getComponentID<C>() == id);
            #endif
            return static_cast<const std::vector<C>*>(vec);
        }

        /// Returns the number of entities that implement this components
        [[nodiscard]] size_t numEntities() const noexcept {
            return entityMap.size();
        }

        /// Resizes the internal entity_map vector
        void resizeEntityMap(const size_t newSize) {
            entityMap.resize(newSize);
        }

        /// Destroys all struts in the components.
        template<typename C>
        void destroy() {
            #ifdef GAN_DEBUG
            assert(ECSTypeID::getComponentID<C>() == id);
            #endif
            delete static_cast<std::vector<C>*>(vec);
        }
    };
}
