#pragma once

#include "Transform.hpp"
#include "pig_err.hpp"
#include "shaders/ShaderRef.hpp"
#include "textures/tex_id.hpp"
#include "core/rendering/OpenGL/VBuffer.hpp"

namespace pg {

    class Model {
    public:
        tex_id tex;
        Transform t;

        void render() {
            if (!vb.mbo) {
                panic_if(vb.genMatrixBuffer(4), "Model::render()", "Could not generate vertex matrix buffer");
                vb.updateMatrixBuffer()
            }
        }

    private:
        VBuffer<> vb;
        ShaderRef shader;

    };
}
