#pragma once

#include <core/Camera.hpp>

#include "toolkit/OpenGL/FrameBuffer.hpp"

namespace pg {

    struct Scene {
        Camera camera;
        FrameBuffer framebuffer;

        Scene(uint32_t w, uint32_t h) :
            framebuffer(PG_UNWRAP(FrameBuffer::make(10, 10, PG_PIXEL)))
        {}

    };
}
