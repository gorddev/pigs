#pragma once

#include <bitset>
#include "ecsdef.hpp"

// made by gordie novak feb 15th, 2026

// bitmasks allow us to easily check whether entities have specific components
// its chill like that

namespace gan {

    /// @struct ECSBitmask
    /// An entity bitmask holds a "0" in each bit for not having a component, and a "1"
    /// in each bit if they do have the component.
    class ECSBitmask {
        /// Representation of the number of components that can be held by the ECS
        std::bitset<ecsBitsetSize> bits;
    public:
        /// Base constructor for EntityBitmask. Sets all bits to zero.
        ECSBitmask()
            : bits(0) {} //< initialize with zero values for each entity.
        /// Implicit constructor for EntityBitmask
        explicit ECSBitmask(std::bitset<ecsBitsetSize> bits)
            : bits(bits) {}

        /// @param id Add this component to the bitmask's representation.
        void assignComponent(const component_id id) {
            bits.set(id);
        }
        /// @param id Remove this component to the bitmask's representation.
        void unassignComponent(const component_id id) {
            bits.reset(id);
        }

        /// @param id The id of the component you want to check against.
        /// @return True if the entity has the component, false otherwise.
        [[nodiscard]] bool hasComponent(const component_id id) const {
            return bits.test(id);
        }

        /// @param other The bitmask you want run the comparison for
        /// @return A new composite bitmask (logical AND)
        ECSBitmask operator&(const ECSBitmask& other) const {
            return ECSBitmask(bits & other.bits);
        }

        /// @param other The bitmask you want to compare again
        /// @return a boolean › true if bitsets are equal, false otherwise.
        bool operator==(const ECSBitmask& other) const {
            return bits == other.bits;
        }

        /// Returns true if the bitmask is not zero.
        explicit operator bool() const {
            return bits != 0;
        }

        /// puts it to a string
        [[nodiscard]] std::string toString() const {
            std::string my_str;
            my_str.reserve(ecsBitsetSize*2);
            for (int i = ecsBitsetSize - 1; i >= 0; --i) {
                my_str += (bits.test(i)) ? '1' : '0';
                if (i % 8 == 0)
                    my_str += ' ';
            }
            return my_str;
        }
    };

    // definitions so better readability
    /// EntityBitmasks is an alias of @code ECSBitmask@endcode.
    using EntityBitmask = ECSBitmask;
    /// ComponentBitmask is an alias of @code ECSBitmask@endcode.
    using ComponentBitmask = ECSBitmask;
}