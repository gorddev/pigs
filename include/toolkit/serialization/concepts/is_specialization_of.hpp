#pragma once
#include <type_traits>

/* Created by Gordie Novak on 8/10/26.
 * Purpose: 
 * Determines if a class is a specialization of another class. */


namespace pg::ser {
    template<template<typename...> typename Ref, typename Test>
    struct is_specialization_of : std::false_type {};

    template<template<typename...> typename Ref, typename... Args>
    struct is_specialization_of<Ref, Ref<Args...>> : std::true_type {};

    // Concepts below
    template<template<typename...> typename Ref, typename Test>
    concept is_specialization_of_v = is_specialization_of<Ref, Test>::value;
}
