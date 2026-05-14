#pragma once

/* Created by Gordie Novak on 4/5/26.
 * Purpose: 
 * Contains the minimal requirements for a vertex object, and
 * some simple vertex object implementations. */

#include <type_traits>

#include "../apidef.h"

namespace pig {
    /** Is_Vertex requires that a vertex struct have an @code static void attribute()@endcode
     * function that properly sets up the attribute pointers for OpenGL shaders */
    template<typename V>
        concept Is_Vertex = requires(V v)
    {
        std::is_same_v<decltype(V::setup_attributes()), void>;
        std::is_same_v<decltype(V::num_attributes), const size_t>;
    };

    /** Standard vertex with a total of 3 attributes:
     * - @code float x, y, z;@endcode       > position
     * - @code float u, v;@endcode          > texture position
     * - @code float nx, ny, nz:@endcode    > normalized coords */
    struct Vertex {
        using Self = Vertex;

        float x, y, z;
        float u, v;
        float nx, ny, nz;

        static void setup_attributes() {
            // x, y, z positions
            glVertexAttribPointer(0, 3,
                GL_FLOAT, GL_FALSE,
                sizeof(Self), reinterpret_cast<void*>(offsetof(Self, x))
            ); glEnableVertexAttribArray(0);
            // u, v coordinates
            glVertexAttribPointer(1, 2,
                GL_FLOAT, GL_FALSE,
                sizeof(Self), reinterpret_cast<void*>(offsetof(Self, u))
            ); glEnableVertexAttribArray(1);
            // nx, ny, nz normals
            glVertexAttribPointer(2, 3,
                GL_FLOAT, GL_FALSE,
                sizeof(Self), reinterpret_cast<void*>(offsetof(Self, u))
            ); glEnableVertexAttribArray(2);
        }

        // —————————————————————————— //
        static constexpr size_t num_attributes = 3;
        static_assert(Is_Vertex<Self>);
    };


    /** (Shader Vertex): Barebones vertex with three position attributes (x,y,z) and nothing else. Intended to be used
     * with fragment shaders for various purposes. */
    struct SVertex {
        using Self = SVertex;

        float x, y, z; //< position data

        static void setup_attributes() {
            // setting up (x,y,z)
            glVertexAttribPointer(0, 3,
                GL_FLOAT, GL_FALSE,
                sizeof(Self), reinterpret_cast<void*>(offsetof(Self, x))
            ); glEnableVertexAttribArray(0);
        }
        // —————————————————————————— //

        static constexpr size_t num_attributes = 1;
        static_assert(Is_Vertex<Self>);
    };

    /** Color vertex with three position attributes and four color attributes (r,g,b,a).
     * Primarily used for drawing primitive shapes without lighting. */
    struct CVertex {
        using Self = CVertex;

        float x, y, z;
        float r, g, b, a;
        static constexpr size_t num_attributes = 2;

        static void setup_attributes() {
            // setting up position pointer (x,y,z)
            glVertexAttribPointer(0, 3,
                GL_FLOAT, GL_FALSE,
                sizeof(Self), reinterpret_cast<void*>(offsetof(Self, x))
            ); glEnableVertexAttribArray(0);
            // setting up color pointer (r, g, b, a)
            glVertexAttribPointer(1, 4,
                GL_FLOAT, GL_FALSE,
                sizeof(Self), reinterpret_cast<void*>(offsetof(Self, r))
            ); glEnableVertexAttribArray(1);
        }
        // —————————————————————————— //
        static_assert(Is_Vertex<Self>);
    };
}