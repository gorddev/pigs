#pragma once
#include <cstddef>
#include <type_traits>


#include <toolkit/intdef.h>
#include <toolkit/serialization/concepts/is_array_like.hpp>
#include <toolkit/serialization/concepts/is_bytes_t.hpp>
#include <toolkit/serialization/concepts/is_specialization_of.hpp>
#include <toolkit/serialization/concepts/is_static_storage_array.hpp>

namespace pg::ser {

    template<typename T>
    concept has_static_layout = requires{
        typename T::static_layout;
        requires std::is_trivially_copyable_v<T>;
    };

    /// If a variable's data can be copied without worry of additional members being added
    template<typename T>
    concept is_statically_copyable =
        std::is_fundamental_v<T> ||
        std::is_array_v<T> ||
        std::is_union_v<T> ||
        std::is_enum_v<T> ||
        has_static_layout<T>;

    /// If this is a type we can just memcpy without writing object size.
    template<typename T>
    concept is_memcpy_compatable = (
            std::is_fundamental_v<T> ||
            has_static_layout<T> ||
            std::is_union_v<T> ||
            std::is_enum_v<T>
        ) && (
            !std::is_void_v<T> &&
            !std::is_same_v<T, std::nullptr_t>
        );

    template<typename T>
    concept is_container_t = (
        is_array_like<T> ||
        is_static_storage_array_v<T> ||
        is_specialization_of_v<SettingVar, T>) &&
        !has_static_layout<T>;

    template <typename T, bool IsEnum = std::is_enum_v<T>>
    struct resolve_type_helper {
        using type = T;
    };
    template <typename T>
    struct resolve_type_helper<T, true> {
        using type = std::underlying_type_t<T>;
    };
    template <typename T>
    using r = typename resolve_type_helper<T>::type;

    /// Determines the methodology of binary storage for serialization.
    /// @warning The order of this enum matters for optimization. Do not move values around.
    enum class bin_t : unsigned char {
        // -------- Normal Variables ------------ //
        // Unsigned variables
        U_BYTE,
        U_SHORT,
        U_INT,
        U_LONG,
        // Signed variables
        BYTE,
        SHORT,
        INT,
        LONG,
        // Floating point
        FLOAT32,
        FLOAT64,
        // ---------- Static Container Types ------------ //
        TRIVIAL_STRUCT,
        UNION,
        // ---------- Dynamic Container Types ------------ //
        SETTING_VAR,
        ARRAY_LIKE,
        NAMED_STRUCT,
    };

    constexpr bool isFundamentalSize(bin_t t) {
        return static_cast<unsigned char>(t) <= static_cast<unsigned char>(bin_t::FLOAT64);
    }

    constexpr bool isMemcpyCompatable(bin_t t) {
        return static_cast<unsigned char>(t) <= static_cast<unsigned char>(bin_t::UNION);
    }

    template<typename T>
    consteval bin_t typeToEnum() {
        // ------------------------------
        // FIRST WE DO ALL THE PRIMITIVES
        // ------------------------------
        if constexpr(is_uchar_t<T>)
            return bin_t::U_BYTE;
        else if constexpr(is_u16_t<T>)
            return bin_t::U_SHORT;
        else if constexpr(is_u32_t<T>)
            return bin_t::U_INT;
        else if constexpr(is_u64_t<T>)
            return bin_t::U_LONG;
        else if constexpr(is_char_t<T>)
            return bin_t::BYTE;
        else if constexpr(is_i16_t<T>)
            return bin_t::SHORT;
        else if constexpr(is_i32_t<T>)
            return bin_t::INT;
        else if constexpr(is_i64_t<T>)
            return bin_t::LONG;
        else if constexpr(std::is_same_v<f32, T>)
            return bin_t::FLOAT32;
        else if constexpr(std::is_same_v<f64, T>)
            return bin_t::FLOAT64;
        else if constexpr(std::is_union_v<T>) {
            static_assert(std::is_trivially_copyable_v<T>, "Unions must be trivially copyable to be used in the serializer.");
            return bin_t::UNION;
        } else if constexpr(has_static_layout<T>)
            return bin_t::TRIVIAL_STRUCT;
        // ----------------------------------
        // THEN WE DO ALL THE CONTAINER TYPES
        // ----------------------------------
        else if constexpr(is_array_like<T>)
            return bin_t::ARRAY_LIKE;
        else if constexpr(is_specialization_of_v<SettingVar, T>)
            return bin_t::SETTING_VAR;
        // ----------------------------------
        // FINALLY WE HAVE NAMED STRUCTS
        // ----------------------------------
        else if constexpr(std::is_class_v<T>)
            return bin_t::NAMED_STRUCT;
        // oops
        else
            static_assert(false, "Type mismatch no valid type to serialize.");
    }

    constexpr std::size_t enumToSize(bin_t type_enum) {
        switch(type_enum) {
        // Primitives
        case bin_t::U_BYTE:          return sizeof(unsigned char);
        case bin_t::BYTE:            return sizeof(char);
        case bin_t::U_SHORT:         return sizeof(u16);
        case bin_t::SHORT:           return sizeof(i16);
        case bin_t::U_INT:           return sizeof(u32);
        case bin_t::INT:             return sizeof(i32);
        case bin_t::U_LONG:          return sizeof(u64);
        case bin_t::LONG:            return sizeof(i64);
        case bin_t::FLOAT32:         return sizeof(f32);
        case bin_t::FLOAT64:         return sizeof(f64);

        // Dynamic / Variable Layout Types
        case bin_t::UNION:
        case bin_t::TRIVIAL_STRUCT:
        case bin_t::ARRAY_LIKE:
        case bin_t::SETTING_VAR:
        case bin_t::NAMED_STRUCT:
        default:
            return 0;
        }
    }
}
