#pragma once

namespace pg::concepts {

    template<typename T, typename U = T>
    concept has_add_op = requires(T a, U b) {
            a + b;
    };

    template<typename T, typename U = T>
    concept has_sub_op = requires(T a, U b) {
        a - b;
    };

    template<typename T, typename U = T>
    concept has_mult_op = requires(T a, U b) {
        a * b;
    };

    template<typename T, typename U = T>
    concept has_div_op = requires(T a, U b) {
        a / b;
    };
}