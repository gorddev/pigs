#pragma once
#include <bitset>
#include <vector>

#ifdef GAN_DEBUG
#include <cassert>
#endif

#include "../ecs-core/ECSBitmask.hpp"

// made by gordie novak on feb 15th

// the entity bitmask allows us to easily check whether an entity belongs
// to a specific ComponentData object without going through its internal Sparse array.

namespace gan {

    /// @class EntityLinker. @brief A wrapper class for a vector that ensures that bitmasks are only interacted
    /// with in very specific ways.
    /// @warning Does not resize itself. Must be handled by the ECS.
    class EntityLinker final {
        /** Internal vector that holds the bitmask for each entity.
         * The entity ID is the index to access the bitmask. @related EntityBitmask*/
        std::vector<EntityBitmask> bitmasks;
    protected:
        EntityLinker() = default; ///< Default Constructor
        friend class ECSCore; ///< Only the ECS can construct an entityLinker.
    public:
        /// @param eID The entity_id that the component will be added to.
        /// @param cID The component_id you want to add to the associated entity
        void assignComponent(const entity_id eID, const component_id cID) {
            #ifdef GAN_DEBUG
            assert(cID < ecsBitsetSize);
            #endif
            bitmasks[eID].assignComponent(cID);
        }

        /// @param eID The entity_id that the component will be removed from.
        /// @param cID The component_id you want to remove to the associated entity
        /// @note If the Entity does not already have the component, this does nothing.
        void unassignComponent(const entity_id eID, const component_id cID) {
            #ifdef GAN_DEBUG
            assert(cID < ecsBitsetSize);
            #endif
            bitmasks[eID].unassignComponent(cID);
        }

        [[nodiscard]] bool hasComponent(const entity_id eID, component_id cID) const {
            #ifdef GAN_DEBUG
            assert(cID < ecsBitsetSize);
            #endif
            return bitmasks[eID].hasComponent(cID);
        }

        /// @param eID The id of the entity you want to check against.
        /// @param cID The component_id you want to check against.
        /// @return A boolean › True if the entity has the component.
        bool hasComponent(const entity_id eID, const component_id cID) {
            #ifdef GAN_DEBUG
            assert(cID < ecsBitsetSize);
            #endif
            return bitmasks[eID].hasComponent(cID);
        }

        /// Returns the number of entities in the vector
        [[nodiscard]] size_t size() const noexcept {
            return bitmasks.size();
        }

        /// @warning @code newSize@endcode should be a power of two.
        /// @param newSize The updated size of the bitmask.
        void resize(const size_t newSize) {
            bitmasks.resize(newSize);
        }

        /// Gets the raw underlying data from the vector for iterators.
        EntityBitmask* data() {
            return bitmasks.data();
        }

        /// Gets the bitmasks at the specified index
        EntityBitmask operator[](const size_t i) const {
            return bitmasks[i];
        }
    };
}// namespace gan