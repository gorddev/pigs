#pragma once

// Returns true if the type can be counted as a vect
#include <type_traits>
#include <concepts>

namespace pg::ser {
    // First we check if something is a regular vector
    template<typename>
        struct is_vector : std::false_type {};

    template<template<typename, typename...> typename T, typename Val, typename... Args>
        requires requires(T<Val, Args...> t, Val v) {
            t.resize(1);
            t.reserve(1);
            t.push_back(v);
            requires std::is_unsigned_v<decltype(t.size())>;
            { t[0] }     -> std::same_as<Val&>;
            { t.data() } -> std::same_as<Val*>;
            typename T<Val>::value_type;
        }
    struct is_vector<T<Val, Args...>> : std::true_type {};

    template<typename T>
    concept is_vector_v = is_vector<T>::value;

}
