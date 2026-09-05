#pragma once
#include <cstdint>

namespace pg {
    class tex_id {
        uint32_t index;
        friend class TextureRegister;
        explicit constexpr tex_id(const uint32_t i) : index(i) {}
    public:
        constexpr tex_id() = default;
        constexpr bool is_empty() {
            return index == 0;
        }
    };
}