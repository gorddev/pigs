#pragma once
#include <string_view>
#include <type_traits>
#include <meta>

namespace pg::ser {

    template <typename E>
    requires std::is_enum_v<E>
    constexpr std::string_view enumToString(E value) {
        // Loop over the enumerators of the enum type
        template for (constexpr std::meta::info enumerator : std::define_static_array(std::meta::enumerators_of(^^E))) {
            if (value == [:enumerator:]) {
                return std::meta::identifier_of(enumerator);
            }
        }
        return "Unknown";
    }
}
