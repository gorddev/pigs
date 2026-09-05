#pragma once

#include "is_bytes_t.hpp"
#include "is_vector.hpp"

/* Created by Gordie Novak on 8/11/26.
 * Purpose: 
 */

namespace pg::ser {
    // Next we can check if something is a byte vector or not
    template<typename>
    struct is_byte_vector_struct : std::false_type {};

    // If it can match this specialization
    template<template<typename, typename...> typename T, typename Vt, typename...Args>
        requires(is_byte_t<typename T<Vt, Args...>::value_type> &&
                 is_vector_v<T<Vt, Args...>>)
    struct is_byte_vector_struct<T<Vt, Args...>> : std::true_type {};


    // We will also check if something is a byte_view (like a string view)
    template<typename>
    struct is_byte_view_struct : std::false_type{};
    template<typename T> requires requires(T t) {
        requires is_byte_t<typename T::value_type>;
        requires std::is_unsigned_v<decltype(t.size())>;
        requires is_byte_t<std::remove_pointer_t<decltype(t.data())>>;
        requires is_byte_t<std::remove_reference_t<decltype(t[0])>>;
        t = T(t.data()+2, t.size()-2);
    } struct is_byte_view_struct<T> : std::true_type{};

    template<typename T>
    concept is_byte_vector = is_byte_vector_struct<T>::value;

    template<typename T>
    concept is_byte_view = is_byte_view_struct<T>::value;

}
