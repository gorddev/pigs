#pragma once

#include "dim2.hpp"
#include "vec.hpp"

/* Created by Gordie Novak on 3/13/26.
 * Purpose:
 * a rectangle, except uses floats for all its values. */

namespace pg {

    struct frect {
        float x, y, w, h;

        [[nodiscard]] float area() const noexcept {
            return w*h;
        }
        [[nodiscard]] dim2 dim() const {
            return {static_cast<uint16_t>(x), static_cast<uint16_t>(y)};
        }
        void move(const vec2 delta) noexcept {
            this->x += delta.x;
            this->y += delta.y;
        }
        /** Returns the frect in normalized device coordinates */
        [[nodiscard]] frect ndc(const dim2& dim) const {
            return frect {
                (x*2.f/dim.w) - 1.f,
                (y*2.f/dim.h) - 1.f,
                (w*2.f/dim.w) - 1.f,
                (h*2.f/dim.h) - 1.f
            };
        }
    };

}