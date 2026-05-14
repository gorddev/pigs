#pragma once
#include <cstdint>

/* Created by Gordie Novak on 3/15/26. */

namespace pig {
    struct dim2 {
        union {
            struct { uint32_t x, y; };
            struct { uint32_t w, h; };
        };

        dim2() = default;

        dim2(int w, int h)
            : w(w), h(h) {}
    };
}