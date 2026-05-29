#pragma once
#include "Transform.hpp"
#include "toolkit/OpenGL/VBuffer.hpp"

namespace pg {

    struct Model {
        Transform t;
        VBuffer<Vertex> vb;
        GLuint shader;
        uint32_t tex;
    };
}
