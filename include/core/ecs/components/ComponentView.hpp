#pragma once
#include <unordered_map>
#include <vector>

#include "../ecs-core/ecsdef.hpp"
#include "../entities/Entity.hpp"

// created by gordie novak, feb 12th, 2025

// the purpose of the ComponentView is to allow easy iteration through
// the data in a single component. it does not allow for filtering based on entity,
// * only entity-by-entity access via at() or operator[]
// * or enhanced for loops that capture everything.

namespace gan {

    /// Holds the information to view into a specific component at any given time.
    template<typename C>
        struct ComponentView {

    public:
        std::vector<EntityIndexer>& entityMap;
        std::vector<C>& vec;

        /// Creates a component view to iterate through elements
        ComponentView(std::vector<EntityIndexer>& entityMap, std::vector<C>& vec) noexcept
            : entityMap(entityMap), vec(vec) {}

    public:

        /// Component iterators iterate through all valid entities that contain the component.
        class iterator {
            /// iterates through our unordered map
            std::vector<EntityIndexer>::const_iterator iter;
            /// contains the pointer to the vector
            std::vector<C>& vec;
            // constructor
            iterator(std::vector<EntityIndexer>::const_iterator iter, std::vector<C>& vec) noexcept
                : iter(iter), vec(vec)
            {} friend struct ComponentView;
        public:
            /// moves the iterator forward by one (preincrement)
            iterator operator++() { ++iter; return *this; }
            /// move the iterator forward by one (postincrement)
            iterator operator++(int) { auto temp = *this; ++iter; return *temp; }

            /// Dereferences the iterator
            C& operator*() const { return vec[iter->idx]; }
            /// Dereferences the iterator
            C* operator->() const { return vec.data() + iter->idx; }

            /// Equality check
            bool operator==(const iterator& other) const noexcept {
                return iter == other.iter;
            }
            /// inequality check
            bool operator!=(const iterator& other) const noexcept {
                return iter != other.iter;
            }
        };

        /// Returns a beginning iterator to the component class
        iterator begin() const noexcept {
            return iterator(entityMap.begin(), vec);
        }
        /// Returns the end iterator to the component class. There are no more component instances after this point.
        iterator end() const noexcept {
            return iterator(entityMap.end(), vec);
        }

        /// Accesses the entity's data with the given Entity.
        /// @param e - Entity you want to access
        /// @return An optional reference to the position, if it exists
        std::optional<C&> at(const Entity e) noexcept {
            if (e.id < entityMap.size())
                return vec[entityMap[e.id].idx];
            return std::nullopt;
        }

        /// Access the entity's data without a bounds check.
        /// @param id - the id of the entity you want to access the component of
        /// @return A reference to the entity in question
        /// @warning Unsafe function: no bounds check
        C& operator[](const entity_id id) {
            return vec[entityMap[id].idx];
        }
    };
}
