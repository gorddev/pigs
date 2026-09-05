#pragma once
#include <type_traits>
#include <concepts>

namespace pg::ser {
    template<typename>
    struct is_static_storage_array : std::false_type {};

    template<template<typename, auto> typename T, typename Tn, auto N>
        requires requires(T<Tn, N> t) {
        requires std::is_unsigned_v<decltype(N)>;
        requires std::is_unsigned_v<decltype(t.size())>;
        { t[0] }     -> std::same_as<Tn&>;
        { t.data() } -> std::same_as<Tn*>;
        typename T<Tn, N>::value_type;
    }
    struct is_static_storage_array<T<Tn, N>> : std::true_type {};

    template<typename T>
    concept is_static_storage_array_v = is_static_storage_array<T>::value;
}
