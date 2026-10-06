#pragma once
#include <glm/mat4x4.hpp>
#include <toolkit/intdef.h>
#include <string_view>

/* Created by Gordie Novak on 8/20/26.
 * Purpose:
 * Contains data to universalize all uniforms in accordance with universal_uniforms.glsl*/

namespace pg {

    class Engine;

    constexpr char universal_block_header[] =
        "pg_SceneData";

    struct UniversalUniforms {
        glm::mat4 screenProjection;
        glm::vec2 resolution;
        u32       frame;
        float     time;

    public:
        static GLuint globalUbo;
        friend class ShaderManager;
        void init();
        void updateUniforms(const Engine& e);
    };

    extern std::string_view universal_uniforms;

}
