#pragma once

#include <tuple>

#include "../components/ComponentBank.hpp"
#include "../components/ComponentView.hpp"
#include "Entity.hpp"
#include "EntityLinker.hpp"

// created by gordie novak, feb 12th, 2026

// the EntitySelector keeps track of an internal
// array of Entities, filtered based on the provided components.
// super easy iteration thru all entities that impl the templated components

namespace gan {

    /// The Entity selector allows for easy iteration through all entities
    /// that implement a collection of components.
    template<typename... C>
    class EntityFilter {
    private:
        /// Reference to the underlying component bank
        ComponentBank& bank;
        /// Array which contains all relevant entities.
        EntityLinker& linker;
        /// Bitmask representing the components
        ComponentBitmask bitmask;

        // <><><><>
        /// Tuple which contains all the relevant component views
        std::tuple<ComponentView<C>...> components;
        // <><><>
    public:
        /// Mandatory constructor for an Entity selector which requires a bunch of component views. 
        EntityFilter(EntityLinker& linker, ComponentBank& bank)
            : linker(linker), bitmask(ECSTypeID::createBitmask<C...>()), components(bank.createComponentViews<C...>()), bank(bank)
            {}


        /// Iterator to easily generate tuples of the corresponding types.
        class iterator {
        public:
            /// Start position within the entityArr
            EntityBitmask* startPos;
            /// Current position within the entityArr
            EntityBitmask* currPos;
            /// Size of the bitmask array
            EntityBitmask* endPos;
            /// Bitmask representing the specific component we want to filter.
            ComponentBitmask bitmask;
            /// Reference to the tuple of components for dereferencing
            std::tuple<ComponentView<C>...>& components;

            void toNextEntity() {
                while (currPos != endPos && (*currPos & bitmask) != bitmask)
                    currPos++;
            }
        public:
            /// Constructor that must be created by the EntitySelector class.
            explicit iterator(EntityLinker& linker, const ComponentBitmask mask, std::tuple<ComponentView<C>...>& components)
                : startPos(linker.data()), currPos(linker.data()), endPos(linker.data() + linker.size()), bitmask(mask), components(components) {
                toNextEntity();
            }

            explicit iterator(EntityBitmask* endPos, std::tuple<ComponentView<C>...>& components)
                : startPos(nullptr), currPos(endPos), endPos(endPos), components(components) {}

            /// Dereference operator
            std::tuple<C&...> operator*() {
                return std::apply([&](auto&... comp) -> decltype(auto) {
                    return std::tie(comp[currPos - startPos]...);
                }, components);
            }
            /// Pointer operator is deleted
            void operator->() = delete;

            // <><><> Boilerplate operators <><><>
            iterator operator++(int)    { auto ret = *this; currPos++; toNextEntity(); return ret;}
            iterator& operator++()       { currPos++; toNextEntity(); return *(this); }

            void operator+=(size_t i) { currPos += i; }
            void operator-=(size_t i) { currPos -= i; }

            bool operator==(const iterator& rhs) const { return currPos == rhs.currPos; }
            bool operator!=(const iterator& rhs) const { return currPos != rhs.currPos; }
        };

        /// generates the first iterator
        iterator begin() { return iterator(linker, bitmask, components); }
        /// generates the final interator. points to no element. UB if dereferences.
        iterator end() { return iterator(linker.data()+linker.size(), components); }

        /// filters through the entities again
        template<typename... Cs>
            requires(are_unique_types<C...,Cs...>)
        EntityFilter<C..., Cs...> refilter() {
            return EntityFilter<C..., Cs...>(linker, bank);
        }

        /// gets the number of entities who passed the screen
        [[nodiscard]] size_t numEntities() const noexcept
            { return linker.size(); }

        /// applies the function for each entity that implements the called traits
        EntityFilter& forEach(void(*forEachFunction)(C&...args)) {
            for (size_t i = 0; i < numEntities(); ++i) {
                (std::apply([&](ComponentView<C>&... args) {
                    if ((linker[i] & bitmask) == bitmask)
                        forEachFunction(std::forward<decltype(args[i])>(args[i])...);
                }, components));
            }
            return *this;
        }

        /// Completely removes all components of each entity
        /// that this filter has access to. Very dangerous
        /// @return This object.
        /// @warning After destroying components, they are unrecoverable
        EntityFilter& removeComponent() {
            for (size_t i = 0; i < numEntities(); ++i) {
                ([&]<typename T>() {
                    if (linker[i] & bitmask)
                        bank.removeComponent<T>(i);
                }.template operator()<C>(), ...);
            }
            return *this;
        }



        EntityFilter(const EntityFilter&) = delete;
        EntityFilter& operator=(const EntityFilter&) = delete;
    };
}