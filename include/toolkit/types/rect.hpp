#pragma once
#include <cstdint>

#include "dim2.hpp"

/* Created by Gordie Novak on 3/13/26.
 * Purpose: 
 * It's literally just a rectangle.
 */

namespace pg {

    struct rect {
        int32_t x, y;
        uint16_t w, h;

        static uint32_t area(const rect& r) {
            return r.w*r.h;
        }
        static dim2 dim(const rect& r) {
            return dim2{r.x, r.y};
        }
        static void move(rect& r, const int32_t x, const int32_t y) {
            r.x += x;
            r.y += y;
        }
    };

}
