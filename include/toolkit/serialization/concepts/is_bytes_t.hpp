#pragma once
#include <cstddef>
#include <type_traits>

/* Created by Gordie Novak on 8/10/26.
 * Purpose:
 */


namespace pg::ser {
    template<typename T>
    concept is_uchar_t =
        (sizeof(T)==1u && std::is_unsigned_v<T>) ||
        std::is_same_v<std::byte, T> ||
        std::is_same_v<bool, T>;

    template<typename T>
    concept is_char_t =
        sizeof(T)==1u && std::is_signed_v<T>;

    template<typename T>
    concept is_byte_t =
        is_uchar_t<T> || is_char_t<T>;

    template<typename T>
    concept is_u16_t =
        sizeof(T)==2u && std::is_unsigned_v<T> && std::is_integral_v<T>;

    template<typename T>
    concept is_i16_t =
        sizeof(T)==2u && std::is_signed_v<T> && std::is_integral_v<T>;

    template<typename T>
    concept is_u32_t =
        sizeof(T)==4u && std::is_unsigned_v<T> && std::is_integral_v<T>;

    template<typename T>
    concept is_i32_t =
        sizeof(T)==4u && std::is_signed_v<T> && std::is_integral_v<T>;

    template<typename T>
    concept is_u64_t =
        sizeof(T)==8u && std::is_unsigned_v<T> && std::is_integral_v<T>;

    template<typename T>
    concept is_i64_t =
        sizeof(T)==8u && std::is_signed_v<T> && std::is_integral_v<T>;

}
