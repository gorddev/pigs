#pragma once

// Returns true if the type can be counted as a string or not

#include <type_traits>
#include <concepts>
#include "is_bytes_t.hpp"


namespace pg::ser {
    template<typename>
        struct is_string : std::false_type {};

    template<typename T>
        requires requires(T t) {
            t.resize(1);
            t.reserve(1);
            t.push_back('c');
            requires std::is_unsigned_v<decltype(t.length())>;
            requires std::is_unsigned_v<decltype(t.size())>;
            requires is_uchar_t<std::remove_reference_t<decltype(t[0])>> ||
                is_char_t<std::remove_reference_t<decltype(t[0])>>;
            requires is_uchar_t<std::remove_pointer_t<decltype(t.data())>> ||
                is_char_t<std::remove_pointer_t<decltype(t.data())>>;
        }
    struct is_string<T> : std::true_type {};

    template<typename T>
    concept is_string_v = is_string<T>::value;

}
