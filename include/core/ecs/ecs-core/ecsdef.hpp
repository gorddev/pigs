#pragma once
#include <cstdint>

namespace gan {
    // <><><> typedefs <><><>
    using vec_ptr = void; ///< typedef to specify a pointer to an std::vector.
    using vec_index = uint32_t; ///< typedef to specify a vector index
    using component_id = uint_fast8_t; ///< used to specify the index of a specific component.
    using entity_id = uint64_t;

    // <><><> Constants <><><>
    constexpr uint32_t ecsBitsetSize = 128;

    // *******************************
    // <><><><> Unique Typing <><><><>
    // *******************************
    template<typename T, typename... Args>
    constexpr bool isUniqueType() {
        return (!std::is_same_v<T, Args> && ...);
    }
    /// Concept that requires that all provided types are unique types
    template<typename...Args>
    concept are_unique_types = isUniqueType<Args...>();

    /// Used to index entities withing ComponentData objects.
    struct EntityIndexer {
        uint64_t idx;
        EntityIndexer() : idx(-1) {}
        explicit EntityIndexer(const uint64_t id) : idx(id) {}
    };

}