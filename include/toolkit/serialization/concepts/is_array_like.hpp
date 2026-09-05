#pragma once
#include <type_traits>
#include <concepts>

/* Created by Gordie Novak on 8/11/26.
 * Purpose: 
 */

namespace pg::ser {

    template<typename T, auto N>
    consteval decltype(N) getCArraySize(T (&arr)[N]) {
        return N;
    }

    template<typename>
    struct is_array_like_struct : std::false_type {
        using value_type = void;
    };

    template<typename T>
        requires requires(T t) {
        requires std::is_unsigned_v<decltype(t.size())>;
        { t[0] }     -> std::same_as<typename T::value_type&>;
        { t.data() } -> std::same_as<typename T::value_type*>;
        { t.begin() } -> std::same_as<decltype(t.end())>;
    } struct is_array_like_struct<T> : std::true_type {
        using value_type = T::value_type;
    };

    template<typename T, auto N>
    struct is_array_like_struct<T[N]> : std::true_type {
        using value_type = T;
    };

    template<typename T>
    concept is_array_like = is_array_like_struct<T>::value;

}
